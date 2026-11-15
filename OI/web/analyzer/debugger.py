"""GDB/MI debug sessions backed by the vendored gdbserver.exe.

Architecture (validated against the vendored TDM-GCC 4.9.2 toolchain):
the mingw gdb rejects I/O redirection on both `-exec-run` and the console
`run` command (echoed but not applied), so the inferior is hosted by
gdbserver instead:

  gdbserver --once 127.0.0.1:PORT main.exe   (child stdin/stdout/stderr
                                              forwarded through our pipes)
  gdb --interpreter=mi2
    -file-exec-and-symbols main.exe
    -target-select remote 127.0.0.1:PORT     (child stopped at entry)
    -break-insert main.cpp:LINE ...
    -exec-continue                            (run to first breakpoint)

Child I/O is ours: live stdout/stderr separation, streamable stdin. MI
drives breakpoints/steps/variables. Leaf module like toolchain.py:
standard library only, failures degrade to structured dicts, never raise.
"""
from __future__ import annotations

import os
import queue
import re
import socket
import subprocess
import threading
import time
import uuid

from . import toolchain

GDB = toolchain.BIN_DIR / "gdb.exe"
GDBSERVER = toolchain.BIN_DIR / "gdbserver.exe"

DEFAULT_TIMEOUT = 10.0          # wall clock for continue waits
STEP_TIMEOUT = 6.0
SERVER_START_TIMEOUT = 8.0
IDLE_TTL = 15 * 60              # reap sessions idle for 15 minutes
MAX_VARS = 64
MAX_WATCH = 8
MAX_OUT_BUFFER = 64 * 1024

_COMPILE_OPTS = ["-g", "-O0", "-static-libgcc", "-static-libstdc++"]

_SESSIONS: dict = {}
_SESSIONS_LOCK = threading.Lock()


def available() -> dict:
    return {
        "gdb": str(GDB),
        "gdbserver": str(GDBSERVER),
        "available": GDB.is_file() and GDBSERVER.is_file(),
        "version": "gdb + gdbserver bundled with TDM-GCC 4.9.2 (vendored)",
    }


# --------------------------------------------------------------------------
# minimal MI record parsing (tolerant; gdb output is not perfectly regular)

_TOKEN_RE = re.compile(r"^(\d*)([*^~&=@])(.*)$", re.S)


def _mi_unquote(text: str) -> str:
    """Decode a MI c-string body (lossy but good enough for display)."""
    out = []
    i = 0
    while i < len(text):
        ch = text[i]
        if ch == "\\" and i + 1 < len(text):
            mapped = {"n": "\n", "t": "\t", "r": "\r", '"': '"', "\\": "\\", "e": "\x1b"}.get(text[i + 1])
            if mapped is not None:
                out.append(mapped)
                i += 2
                continue
        if ch == '"':
            i += 1
            continue
        out.append(ch)
        i += 1
    return "".join(out)


def _extract_field(record: str, field: str) -> str:
    match = re.search(field + r'="((?:[^"\\]|\\.)*)"', record)
    return _mi_unquote(match.group(1)) if match else ""


def _parse_variables(record: str) -> list:
    variables = []
    for chunk in re.findall(r"\{([^{}]*)\}", record):
        name = _extract_field(chunk, "name")
        if not name:
            continue
        entry = {"name": name}
        type_match = re.search(r'type="((?:[^"\\]|\\.)*)"', chunk)
        value_match = re.search(r'value="((?:[^"\\]|\\.)*)"', chunk)
        if type_match:
            entry["type"] = _mi_unquote(type_match.group(1))
        entry["value"] = _mi_unquote(value_match.group(1)) if value_match else "…"
        variables.append(entry)
    return variables[:MAX_VARS]


def _pick_port() -> int:
    sock = socket.socket()
    sock.bind(("127.0.0.1", 0))
    port = sock.getsockname()[1]
    sock.close()
    return port


class _OutputTap:
    """Background reader for one gdbserver pipe; keeps a capped tail."""

    def __init__(self, stream):
        self._stream = stream
        self._lock = threading.Lock()
        self._lines: list = []
        self.exhausted = False
        self.thread = threading.Thread(target=self._pump, daemon=True)

    def _pump(self) -> None:
        try:
            for line in self._stream:
                with self._lock:
                    self._lines.append(line.rstrip("\r\n"))
                    while len(self._lines) > 400:
                        self._lines.pop(0)
        except Exception:
            pass
        self.exhausted = True

    def start(self) -> None:
        self.thread.start()

    def text(self) -> str:
        with self._lock:
            return "\n".join(self._lines)[-MAX_OUT_BUFFER:]


class _Session:
    """One gdbserver + gdb pair. Public methods hold `lock`."""

    EOF = object()  # sentinel pushed by the MI pump on gdb stdout EOF

    def __init__(self, code: str, stdin: str, breakpoints: list, watch: list):
        self.lock = threading.Lock()
        self.code = code
        self.gdbserver: subprocess.Popen | None = None
        self.gdb: subprocess.Popen | None = None
        self._mi_queue: queue.Queue | None = None
        self.out_tap: _OutputTap | None = None
        self.err_tap: _OutputTap | None = None
        self.token = 0
        self.status = "starting"          # running|stopped|exited|error|dead
        self.line = 0
        self.reason = ""
        self.exit_code = None
        self.breakpoints = sorted({int(b) for b in (breakpoints or []) if str(b).strip().isdigit()})
        self.watch = [str(w).strip()[:120] for w in (watch or []) if str(w).strip()][:MAX_WATCH]
        self.variables: list = []
        self.watch_values: list = []
        self.frame_file = ""
        self.error = ""
        self.compile_ms = 0.0
        self.compile_cached = False
        self.last_active = time.time()
        self._stdin_text = (stdin or "")[:toolchain.MAX_STDIN]

    # ---- low-level MI -------------------------------------------------
    def _read_mi(self, deadline: float):
        """Next MI line with a hard deadline. str, EOF sentinel, or None=timeout."""
        remaining = deadline - time.time()
        try:
            return self._mi_queue.get(timeout=max(0.02, remaining))
        except queue.Empty:
            return None

    def _send(self, command: str, timeout: float = STEP_TIMEOUT) -> tuple:
        """Send one full MI command, wait for its `token^` result, drain
        to the `(gdb)` prompt. Returns (result_body|None, stopped_records)."""
        self.token += 1
        token = self.token
        try:
            self.gdb.stdin.write(f"{token}{command}\n")
            self.gdb.stdin.flush()
        except Exception as exc:
            self.status = "dead"
            self.error = self.error or f"gdb stdin: {type(exc).__name__}"
            return None, []
        deadline = time.time() + timeout
        stopped = []
        result = None

        def classify(line: str) -> tuple:
            match = _TOKEN_RE.match(line)
            if not match:
                return None, None
            return match.group(2), match.group(3)

        while time.time() < deadline:
            line = self._read_mi(deadline)
            if line is None:
                break
            if line is self.EOF:
                self.status = "dead"
                self.error = self.error or "gdb terminated"
                return None, stopped
            kind, body = classify(line.rstrip("\r\n"))
            if kind == "*" and body.startswith("stopped"):
                stopped.append(body)
            elif kind == "^" and line.rstrip("\r\n").startswith(f"{token}^"):
                result = body
                break
        if result is None:
            return None, stopped
        # drain to the prompt so the next command starts clean
        drain_deadline = time.time() + 2.0
        while time.time() < drain_deadline:
            line = self._read_mi(drain_deadline)
            if line is None or line is self.EOF:
                break
            stripped = line.rstrip("\r\n")
            if stripped.strip() == "(gdb)":
                break
            kind, body = classify(stripped)
            if kind == "*" and body.startswith("stopped"):
                stopped.append(body)
        return result, stopped

    def _consume_stopped(self, records: list) -> None:
        for body in records:
            reason = _extract_field(body, "reason")
            frame_line = _extract_field(body, "line")
            self.line = int(frame_line) if frame_line.isdigit() else 0
            self.frame_file = _extract_field(body, "file")
            if reason == "exited-normally":
                self.status = "exited"
                self.reason = "正常退出"
            elif reason.startswith("exited"):
                self.status = "exited"
                self.reason = reason
            elif reason in ("", None) and self.status == "starting":
                self.status = "stopped"
                self.reason = "已附加到入口（gdbserver）"
            else:
                self.status = "stopped"
                if reason == "breakpoint-hit":
                    bkpt = _extract_field(body, "bkptno")
                    self.reason = f"断点 {bkpt} 命中，行 {self.line}"
                elif reason == "end-stepping-range":
                    self.reason = f"单步到行 {self.line}"
                elif reason == "signal-received":
                    self.reason = "收到中断信号"
                elif reason == "function-finished":
                    self.reason = f"步出返回，行 {self.line}"
                elif reason:
                    self.reason = reason
        if self.status == "exited":
            result, _ = self._send('-data-evaluate-expression "$_exitcode"', timeout=2.0)
            if result and result.startswith("done"):
                code_text = _extract_field(result, "value")
                self.exit_code = int(code_text) if code_text.lstrip("+-").isdigit() else None
        if self.status == "stopped":
            self._collect_variables()
            self._eval_watch()

    def _collect_variables(self) -> None:
        result, _ = self._send("-stack-list-variables --simple-values")
        if result and result.startswith("done"):
            self.variables = _parse_variables(result)

    def _eval_watch(self) -> None:
        values = []
        for expr in self.watch:
            result, _ = self._send(f'-data-evaluate-expression "{expr}"')
            if result and result.startswith("done"):
                values.append({"expr": expr, "value": _extract_field(result, "value") or "?"})
            elif result and result.startswith("error"):
                values.append({"expr": expr, "value": "⟨无法求值⟩", "error": True})
            else:
                break  # gdb is gone; keep previous values
        if values:
            self.watch_values = values

    def _wait_stopped(self, timeout: float) -> bool:
        """Read async records until *stopped arrives; interrupt on wall clock."""
        deadline = time.time() + timeout
        while time.time() < deadline:
            line = self._read_mi(deadline)
            if line is None:
                break  # wall clock expired
            if line is self.EOF:
                self.status = "dead"
                self.error = self.error or "gdb terminated"
                return False
            match = _TOKEN_RE.match(line.rstrip("\r\n"))
            if match and match.group(2) == "*" and match.group(3).startswith("stopped"):
                self._consume_stopped([match.group(3)])
                return self.status in ("stopped", "exited")
        self._interrupt_and_halt()
        return self.status == "stopped"

    def _interrupt_and_halt(self) -> None:
        try:
            self.gdb.stdin.write("-exec-interrupt\n")
            self.gdb.stdin.flush()
        except Exception:
            pass
        deadline = time.time() + 3.0
        while time.time() < deadline:
            line = self._read_mi(deadline)
            if line is None or line is self.EOF:
                break
            match = _TOKEN_RE.match(line.rstrip("\r\n"))
            if match and match.group(2) == "*" and match.group(3).startswith("stopped"):
                self._consume_stopped([match.group(3)])
                break

    def _return_to_user_frame(self) -> None:
        """After next/step this old remote gdb may park inside a library
        frame (line 0); finish back into main.cpp so the UI stays useful."""
        for _ in range(4):
            if self.status != "stopped" or not self.frame_file \
                    or self.frame_file.replace("\\\\", "/").endswith("main.cpp"):
                return
            result, stopped = self._send("-exec-finish", timeout=STEP_TIMEOUT)
            if result and result.startswith("running"):
                self.status = "running"
                if not self._wait_stopped(STEP_TIMEOUT):
                    return
            if stopped:
                self._consume_stopped(stopped)

    def _drain_async(self) -> None:
        """Consume any immediately-available async records (esp. *stopped
        that arrived while nobody was reading, e.g. a breakpoint hit)."""
        while True:
            try:
                line = self._mi_queue.get_nowait()
            except queue.Empty:
                return
            if line is self.EOF:
                self.status = "dead"
                self.error = self.error or "gdb terminated"
                return
            match = _TOKEN_RE.match(line.rstrip("\r\n"))
            if match and match.group(2) == "*" and match.group(3).startswith("stopped"):
                self._consume_stopped([match.group(3)])

    def _write_stdin(self, text: str) -> None:
        """Forward text to the child; gdbserver forwards line-wise."""
        text = (text or "").replace("\r\n", "\n")
        if not text or not self.gdbserver or self.gdbserver.poll() is not None:
            return
        if not text.endswith("\n"):
            text += "\n"
        try:
            self.gdbserver.stdin.write(text)
            self.gdbserver.stdin.flush()
        except Exception:
            pass

    # ---- lifecycle ------------------------------------------------------
    def start(self) -> dict:
        env = dict(os.environ)
        env["PATH"] = str(toolchain.BIN_DIR) + os.pathsep + env.get("PATH", "")
        flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)

        ok, exe, compile_ms, err, cached = toolchain._cached_build(
            self.code, toolchain.DEBUG_CACHE, _COMPILE_OPTS,
            "main.cpp", "main.exe", env, flags,
        )
        self.compile_ms, self.compile_cached = compile_ms, cached
        if not ok:
            self.status = "error"
            self.error = err or "debug build failed"
            return self.state()

        port = _pick_port()
        try:
            self.gdbserver = subprocess.Popen(
                [str(GDBSERVER), "--once", f"127.0.0.1:{port}", str(exe)],
                stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                stderr=subprocess.PIPE, env=env, creationflags=flags, text=True,
            )
        except Exception as exc:
            self.status = "error"
            self.error = f"gdbserver launch failed: {type(exc).__name__}"
            return self.state()
        self.out_tap = _OutputTap(self.gdbserver.stdout)
        self.err_tap = _OutputTap(self.gdbserver.stderr)
        self.out_tap.start()
        self.err_tap.start()

        # wait for "Listening on port" (or death) before gdb connects
        deadline = time.time() + SERVER_START_TIMEOUT
        ready = False
        while time.time() < deadline and not ready:
            if self.gdbserver.poll() is not None:
                break
            text = self.err_tap.text() + "\n" + self.out_tap.text()
            if "Listening on port" in text:
                ready = True
                break
            time.sleep(0.05)
        if not ready:
            self.status = "error"
            self.error = "gdbserver 未能启动监听（端口被占用或可执行文件无法运行）"
            return self.state()

        try:
            self.gdb = subprocess.Popen(
                [str(GDB), "--interpreter=mi2", "--quiet", "--nx"],
                stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                stderr=subprocess.DEVNULL, env=env, creationflags=flags, text=True,
            )
        except Exception as exc:
            self.status = "error"
            self.error = f"gdb launch failed: {type(exc).__name__}"
            return self.state()
        self._mi_queue = queue.Queue()

        def _pump():
            try:
                for line in self.gdb.stdout:
                    self._mi_queue.put(line)
            except Exception:
                pass
            self._mi_queue.put(self.EOF)

        threading.Thread(target=_pump, daemon=True).start()

        gdb_path = str(exe).replace("\\", "/")
        result, _ = self._send(f'-file-exec-and-symbols "{gdb_path}"')
        if result is None or result.startswith("error"):
            self.status = "error"
            self.error = "gdb 无法加载该可执行文件"
            return self.state()

        result, stopped = self._send(f"-target-select remote 127.0.0.1:{port}", timeout=8.0)
        if result is None or not (result.startswith("connected") or result.startswith("done")):
            self.status = "error"
            self.error = "gdb 无法连接 gdbserver"
            return self.state()
        if stopped:
            self._consume_stopped(stopped)

        for line_no in self.breakpoints:
            result, _ = self._send(f'-break-insert "main.cpp:{line_no}"')
            if result is None:
                break  # gdb is gone

        # deliver initial stdin up front (gdbserver forwards line-wise);
        # the child reads it as soon as it reaches its first `cin >> …`
        self._write_stdin(self._stdin_text)

        # run to the first breakpoint (or completion) automatically
        result, stopped = self._send("-exec-continue", timeout=DEFAULT_TIMEOUT)
        if result is None:
            if self.status == "dead":
                return self.state()
        elif result.startswith("running"):
            self.status = "running"
            self._wait_stopped(DEFAULT_TIMEOUT)
        elif result.startswith("error"):
            self.reason = _extract_field(result, "msg")
        if stopped:
            self._consume_stopped(stopped)
        self._drain_async()
        return self.state()

    def state(self) -> dict:
        return {
            "status": self.status,
            "line": self.line,
            "reason": self.reason,
            "exit_code": self.exit_code,
            "variables": self.variables,
            "watch": self.watch_values,
            "breakpoints": self.breakpoints,
            "stdout": self.out_tap.text() if self.out_tap else "",
            "stderr": self.err_tap.text() if self.err_tap else "",
            "error": self.error,
            "compile_ms": self.compile_ms,
            "compile_cached": self.compile_cached,
        }

    def command(self, action: str, extra: dict) -> dict:
        """continue|next|step|finish|stdin|bp_add|bp_del|watch_add|watch_del|state."""
        self._drain_async()  # async *stopped may have landed while idle
        if action in ("continue", "next", "step", "finish"):
            if self.status in ("exited", "running", "dead"):
                return self.state()
            cmd = {"continue": "-exec-continue", "next": "-exec-next",
                   "step": "-exec-step", "finish": "-exec-finish"}[action]
            result, stopped = self._send(
                cmd, timeout=DEFAULT_TIMEOUT if action == "continue" else STEP_TIMEOUT,
            )
            if result is None:
                if self.status == "dead":
                    return self.state()
            elif result.startswith("error"):
                self.reason = _extract_field(result, "msg") or self.reason
            elif result.startswith("running"):
                self.status = "running"
                self._wait_stopped(DEFAULT_TIMEOUT if action == "continue" else STEP_TIMEOUT)
            if stopped:
                self._consume_stopped(stopped)
            if action in ("next", "step"):
                self._return_to_user_frame()
            return self.state()
        if action == "stdin":
            self._write_stdin(str(extra.get("text") or "")[:toolchain.MAX_STDIN])
            return self.state()
        if action == "bp_add":
            line_no = extra.get("line")
            if str(line_no).strip().isdigit():
                n = int(line_no)
                if n not in self.breakpoints:
                    result, _ = self._send(f'-break-insert "main.cpp:{n}"')
                    if result and result.startswith("done"):
                        self.breakpoints = sorted(self.breakpoints + [n])
            return self.state()
        if action == "bp_del":
            line_no = extra.get("line")
            if str(line_no).strip().isdigit() and int(line_no) in self.breakpoints:
                n = int(line_no)
                # find and delete the gdb breakpoint at this line
                result, _ = self._send("-break-list")
                if result and result.startswith("done"):
                    for chunk in re.findall(r"bkpt=\{([^{}]*)\}", result):
                        if _extract_field(chunk, "original-location") in (f"main.cpp:{n}", f"-source main.cpp -line {n}"):
                            number = _extract_field(chunk, "number")
                            if number.isdigit():
                                self._send(f"-break-delete {number}")
                self.breakpoints = sorted(b for b in self.breakpoints if b != n)
            return self.state()
        if action == "watch_add":
            expr = str(extra.get("expr", "")).strip()[:120]
            if expr and expr not in self.watch and len(self.watch) < MAX_WATCH:
                self.watch.append(expr)
            if self.status == "stopped":
                self._eval_watch()
            return self.state()
        if action == "watch_del":
            expr = str(extra.get("expr", "")).strip()
            self.watch = [w for w in self.watch if w != expr]
            if self.status == "stopped":
                self._eval_watch()
            return self.state()
        return self.state()

    def stop(self) -> None:
        """Kill the whole tree: gdbserver (and its child), then gdb."""
        for proc, name in ((self.gdb, "gdb"), (self.gdbserver, "gdbserver")):
            if proc is None:
                continue
            try:
                if proc.poll() is None:
                    subprocess.run(
                        ["taskkill", "/PID", str(proc.pid), "/T", "/F"],
                        capture_output=True, timeout=5,
                        creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0),
                    )
            except Exception:
                try:
                    proc.kill()
                except Exception:
                    pass
        if self.status != "exited":
            self.status = "dead"


def _reap_idle() -> None:
    now = time.time()
    with _SESSIONS_LOCK:
        stale = [sid for sid, s in _SESSIONS.items() if now - s.last_active > IDLE_TTL]
        for sid in stale:
            session = _SESSIONS.pop(sid)
            try:
                session.stop()
            except Exception:
                pass


# --------------------------------------------------------------------------
# public API

def start(code: str, stdin: str = "", breakpoints: list | None = None,
          watch: list | None = None) -> dict:
    """Compile -g -O0, host via gdbserver, attach gdb, run to first bp."""
    info = available()
    if not info["available"]:
        return {"ok": False, "error": "vendored gdb/gdbserver missing (toolchain/MinGW64/bin)"}
    _reap_idle()
    session = _Session(code, stdin, breakpoints or [], watch or [])
    with session.lock:
        state = session.start()
        if state["status"] in ("error", "dead"):
            session.stop()
            return {"ok": False, "error": state["error"] or "调试会话启动失败"}
        sid = uuid.uuid4().hex[:12]
        with _SESSIONS_LOCK:
            _SESSIONS[sid] = session
        return {"ok": True, "session": sid, "state": state}


def command(sid: str, action: str, extra: dict | None = None) -> dict:
    with _SESSIONS_LOCK:
        session = _SESSIONS.get(sid)
    if session is None:
        return {"ok": False, "error": "调试会话不存在或已过期，请重新启动调试"}
    with session.lock:
        session.last_active = time.time()
        state = session.command(action, extra or {})
        return {"ok": True, "session": sid, "state": state}


def stop(sid: str) -> dict:
    with _SESSIONS_LOCK:
        session = _SESSIONS.pop(sid, None)
    if session is None:
        return {"ok": True, "stopped": False}
    with session.lock:
        session.stop()
    return {"ok": True, "stopped": True}


def stop_all() -> int:
    with _SESSIONS_LOCK:
        sessions = list(_SESSIONS.values())
        _SESSIONS.clear()
    for session in sessions:
        try:
            session.stop()
        except Exception:
            pass
    return len(sessions)
