"""Optional local compile/run measurement with the vendored MinGW64.

Only executables under ROOT_DIR/toolchain/MinGW64/bin are used. Every failure
returns a structured `failed` measurement instead of raising, so the static
report always survives (ComfyDL_UI profiling_routes.py:372-378 courtesy rule).
"""
from __future__ import annotations

import ctypes
import os
import subprocess
import tempfile
import threading
import time
from pathlib import Path

ROOT_DIR = Path(__file__).resolve().parents[2]
BIN_DIR = ROOT_DIR / "toolchain" / "MinGW64" / "bin"
GXX = BIN_DIR / "g++.exe"
SIZE = BIN_DIR / "size.exe"

MAX_STDIN = 64 * 1024
MAX_OUTPUT = 4096
MAX_COMPILE_SECONDS = 25.0
MAX_RUN_SECONDS = 15.0
DEFAULT_RUN_SECONDS = 5.0
DEFAULT_MEMORY_BYTES = 256 * 1024 * 1024


def _clip(text: str) -> str:
    text = text or ""
    return text if len(text) <= MAX_OUTPUT else text[:MAX_OUTPUT] + "\n…(truncated)"


def available() -> dict:
    return {
        "gxx": str(GXX),
        "available": GXX.is_file(),
        "size": SIZE.is_file(),
        "version": "TDM-GCC 4.9.2 (vendored)",
    }


def _peak_sampler(handle: int, done: threading.Event, peak: list, limit: int, proc) -> None:
    """Sample PeakWorkingSetSize via Psapi; no third-party dependency."""
    if not hasattr(ctypes, "WinDLL"):
        return
    try:
        from ctypes import wintypes

        class Counters(ctypes.Structure):
            _fields_ = [
                ("cb", wintypes.DWORD),
                ("PageFaultCount", wintypes.DWORD),
                ("PeakWorkingSetSize", ctypes.c_size_t),
                ("WorkingSetSize", ctypes.c_size_t),
                ("QuotaPeakPagedPoolUsage", ctypes.c_size_t),
                ("QuotaPagedPoolUsage", ctypes.c_size_t),
                ("QuotaPeakNonPagedPoolUsage", ctypes.c_size_t),
                ("QuotaNonPagedPoolUsage", ctypes.c_size_t),
                ("PagefileUsage", ctypes.c_size_t),
                ("PeakPagefileUsage", ctypes.c_size_t),
            ]

        psapi = ctypes.WinDLL("psapi")
        psapi.GetProcessMemoryInfo.argtypes = [wintypes.HANDLE, ctypes.POINTER(Counters), wintypes.DWORD]
        psapi.GetProcessMemoryInfo.restype = wintypes.BOOL

        def sample() -> bool:
            counters = Counters()
            counters.cb = ctypes.sizeof(counters)
            if psapi.GetProcessMemoryInfo(handle, ctypes.byref(counters), counters.cb):
                peak[0] = max(peak[0], int(counters.PeakWorkingSetSize))
                return True
            return False

        sample()  # short-lived probes must not escape the first tick
        while not done.wait(0.02):
            if not sample():
                return
            if limit and peak[0] > limit:
                proc.kill()
                return
    except Exception:
        return


def _size_of(exe: Path, env: dict) -> dict:
    if not SIZE.is_file():
        return {"status": "unavailable"}
    try:
        result = subprocess.run(
            [str(SIZE), str(exe)], capture_output=True, text=True,
            timeout=5, env=env, creationflags=getattr(subprocess, "CREATE_NO_WINDOW", 0),
        )
        rows = [line.split() for line in result.stdout.strip().splitlines()]
        if len(rows) >= 2 and len(rows[1]) >= 4:
            return {"status": "ok", "text": int(rows[1][0]), "data": int(rows[1][1]), "bss": int(rows[1][2])}
    except Exception:
        pass
    return {"status": "unavailable"}


def measure(code: str, stdin: str = "", timeout: float = DEFAULT_RUN_SECONDS,
            memory_limit: int = DEFAULT_MEMORY_BYTES) -> dict:
    """Compile once, run once under a wall clock + sampled memory ceiling."""
    if not GXX.is_file():
        return {"status": "failed", "reason": "vendored toolchain missing (toolchain/MinGW64/bin/g++.exe)"}
    timeout = max(0.5, min(float(timeout or DEFAULT_RUN_SECONDS), MAX_RUN_SECONDS))
    memory_limit = max(16 * 1024 * 1024, min(int(memory_limit or DEFAULT_MEMORY_BYTES), 1024 * 1024 * 1024))
    stdin = (stdin or "")[:MAX_STDIN]

    env = dict(os.environ)
    env["PATH"] = str(BIN_DIR) + os.pathsep + env.get("PATH", "")
    flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)

    with tempfile.TemporaryDirectory(prefix="oi-analyze-") as tmp:
        folder = Path(tmp)
        source = folder / "probe.cpp"
        exe = folder / "probe.exe"
        source.write_text(code, encoding="utf-8")
        compile_start = time.perf_counter()
        try:
            compiled = subprocess.run(
                [str(GXX), "-std=c++14", "-O2", "-static-libgcc", "-static-libstdc++",
                 str(source), "-o", str(exe)],
                capture_output=True, text=True, timeout=MAX_COMPILE_SECONDS,
                cwd=str(folder), env=env, creationflags=flags,
            )
        except subprocess.TimeoutExpired:
            return {"status": "failed", "reason": "compile timeout", "phase": "compile"}
        except Exception as exc:
            return {"status": "failed", "reason": f"compile error: {type(exc).__name__}", "phase": "compile"}
        compile_ms = round((time.perf_counter() - compile_start) * 1000, 1)
        if compiled.returncode != 0 or not exe.is_file():
            return {
                "status": "failed", "reason": "compile_error", "phase": "compile",
                "compile_ms": compile_ms,
                "stderr": _clip(compiled.stderr),
            }

        segments = _size_of(exe, env)
        peak = [0]
        done = threading.Event()
        try:
            proc = subprocess.Popen(
                [str(exe)], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                stderr=subprocess.PIPE, cwd=str(folder), env=env, creationflags=flags,
            )
        except Exception as exc:
            return {"status": "failed", "reason": f"launch error: {type(exc).__name__}", "phase": "run"}

        sampler = threading.Thread(
            target=_peak_sampler,
            args=(int(proc._handle), done, peak, memory_limit, proc),
            daemon=True,
        )
        sampler.start()
        start = time.perf_counter()
        timed_out = False
        try:
            stdout, stderr = proc.communicate(input=stdin.encode("utf-8", "replace"), timeout=timeout)
            exit_code = proc.returncode
        except subprocess.TimeoutExpired:
            timed_out = True
            proc.kill()
            stdout, stderr = proc.communicate()
            exit_code = None
        finally:
            done.set()
            sampler.join(timeout=0.2)
        wall_ms = round((time.perf_counter() - start) * 1000, 1)

        return {
            "status": "ok" if not timed_out and exit_code == 0 else ("timeout" if timed_out else "failed"),
            "reason": "" if not timed_out and exit_code == 0 else ("wall timeout" if timed_out else f"exit code {exit_code}"),
            "phase": "run",
            "compile_ms": compile_ms,
            "wall_ms": wall_ms,
            "peak_bytes": peak[0] or None,
            "peak_sampled": bool(peak[0]),
            "memory_limit_bytes": memory_limit,
            "exit_code": exit_code,
            "stdout": _clip(stdout.decode("utf-8", "replace")),
            "stderr": _clip(stderr.decode("utf-8", "replace")),
            "segments": segments,
            "compiler": "g++ (tdm64-1) 4.9.2 -std=c++14 -O2",
            "stdin_bytes": len(stdin.encode("utf-8", "replace")),
        }
