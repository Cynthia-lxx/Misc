#!/usr/bin/env python3
"""OI Code Console - local web console for OI.py.

Run:  python web/server.py            (then open http://127.0.0.1:8765)
      python web/server.py --port 9000 --no-browser

Dependencies (all already present in the project's OIenv virtualenv):
  Python 3.9+ standard library, openai, click, rich, pyperclip
No build step, no extra packages.

The CLI (OI.py) stays untouched: this server reuses its Config / History /
SYSTEM_PROMPT / extract_code so both frontends share ~/.oicoder_config.json
and ~/.oicoder_history.json.
"""

from __future__ import annotations

import argparse
import json
import mimetypes
import os
import sys
import threading
import traceback
import webbrowser
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import unquote, urlparse

WEB_DIR = Path(__file__).resolve().parent
ROOT_DIR = WEB_DIR.parent
STATIC_DIR = WEB_DIR / "static"
sys.path.insert(0, str(ROOT_DIR))

import OI as oi  # noqa: E402  (project CLI: config / history / prompt / helpers)
from openai import OpenAI  # noqa: E402

STATE_LOCK = threading.Lock()
MAX_BODY = 8 * 1024 * 1024
MAX_FILE = 2 * 1024 * 1024
CONFIG_KEYS = ("base_url", "model", "max_tokens", "temperature")

try:
    sys.stdout.reconfigure(encoding="utf-8")
except Exception:
    pass


def json_bytes(obj) -> bytes:
    return json.dumps(obj, ensure_ascii=False).encode("utf-8")


def is_within(path: Path, root: Path) -> bool:
    """Path containment check that also tolerates different Windows drives."""
    try:
        return os.path.commonpath([str(path), str(root)]) == str(root)
    except ValueError:
        return False


def public_config() -> dict:
    data = dict(oi.config.data)
    key = data.get("api_key") or ""
    data["api_key"] = "*" * 8 if key else ""
    data["api_key_set"] = bool(key)
    return data


def status_payload() -> dict:
    with STATE_LOCK:
        messages = [dict(m) for m in oi.history.messages]
        cfg = public_config()
    last = oi.extract_code(messages[-1]["content"]) if messages and messages[-1]["role"] == "assistant" else None
    return {
        "config": cfg,
        "history_count": len(messages),
        "last_code": last,
        "workspace": str(ROOT_DIR),
        "system_prompt": oi.SYSTEM_PROMPT,
    }


class ConsoleHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"
    server_version = "OICodeConsole/1.0"

    # ------------------------------------------------------------------ helpers
    def log_message(self, fmt, *args):
        if os.environ.get("OI_CONSOLE_VERBOSE"):
            sys.stderr.write("%s - %s\n" % (self.address_string(), fmt % args))

    def _headers(self, status=200, content_type="application/json; charset=utf-8", length=None, extra=None):
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Cache-Control", "no-store")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS")
        if length is not None:
            self.send_header("Content-Length", str(length))
        for key, value in (extra or {}).items():
            self.send_header(key, value)
        self.end_headers()

    def _json(self, obj, status=200):
        body = json_bytes(obj)
        self._headers(status, length=len(body))
        self.wfile.write(body)

    def _error(self, message, status=400):
        self._json({"ok": False, "error": str(message)}, status)

    def _body(self) -> dict:
        length = int(self.headers.get("Content-Length", 0) or 0)
        if length <= 0:
            return {}
        if length > MAX_BODY:
            raise ValueError("request body too large")
        raw = self.rfile.read(length)
        return json.loads(raw.decode("utf-8"))

    # ------------------------------------------------------------------ static
    def _serve_static(self, rel: str):
        rel = unquote(rel).lstrip("/")
        if not rel or rel.endswith("/"):
            rel += "index.html"
        target = (STATIC_DIR / rel).resolve()
        static_root = STATIC_DIR.resolve()
        if not is_within(target, static_root) or not target.is_file():
            self._error("not found", 404)
            return
        mime = mimetypes.guess_type(target.name)[0] or "application/octet-stream"
        if target.suffix in (".js", ".mjs"):
            mime = "text/javascript; charset=utf-8"
        elif target.suffix == ".css":
            mime = "text/css; charset=utf-8"
        elif target.suffix == ".html":
            mime = "text/html; charset=utf-8"
        data = target.read_bytes()
        self._headers(200, mime, len(data))
        self.wfile.write(data)

    # ------------------------------------------------------------------ routes
    def do_OPTIONS(self):
        self._headers(204, length=0)

    def do_GET(self):
        path = urlparse(self.path).path
        try:
            if path == "/api/status":
                return self._json({"ok": True, **status_payload()})
            if path == "/api/history":
                with STATE_LOCK:
                    messages = [dict(m) for m in oi.history.messages]
                return self._json({"ok": True, "messages": messages})
            if path == "/favicon.ico":
                return self._headers(204, length=0)
            if path.startswith("/api/"):
                return self._error("unknown endpoint", 404)
            return self._serve_static("index.html" if path == "/" else path.lstrip("/"))
        except Exception as exc:
            traceback.print_exc()
            return self._error(exc, 500)

    def do_DELETE(self):
        path = urlparse(self.path).path
        if path == "/api/history":
            with STATE_LOCK:
                oi.history.clear()
            return self._json({"ok": True, "history_count": 0})
        return self._error("unknown endpoint", 404)

    def do_POST(self):
        path = urlparse(self.path).path
        try:
            body = self._body()
        except Exception as exc:
            return self._error("invalid JSON body: %s" % exc)

        try:
            if path == "/api/config":
                return self._update_config(body)
            if path == "/api/generate":
                return self._generate(body)
            if path == "/api/save":
                return self._save(body)
            if path == "/api/read-file":
                return self._read_file(body)
            if path == "/api/clipboard":
                return self._clipboard(body)
            return self._error("unknown endpoint", 404)
        except Exception as exc:
            traceback.print_exc()
            return self._error(exc, 500)

    # ------------------------------------------------------------------ handlers
    def _update_config(self, body: dict):
        with STATE_LOCK:
            for key in CONFIG_KEYS:
                if key not in body:
                    continue
                value = body[key]
                if key in ("max_tokens",):
                    value = max(1, min(65536, int(value)))
                elif key == "temperature":
                    value = max(0.0, min(2.0, float(value)))
                else:
                    value = str(value).strip()
                    if not value:
                        continue
                oi.config.data[key] = value
            # blank api_key means "keep the stored one"
            key = str(body.get("api_key") or "").strip()
            if key and key != "*" * 8:
                oi.config.data["api_key"] = key
            oi.config.save()
            cfg = public_config()
        return self._json({"ok": True, "config": cfg})

    def _generate(self, body: dict):
        user_input = str(body.get("input") or "").strip()
        if not user_input:
            return self._error("empty input")
        new_topic = bool(body.get("new_topic"))

        with STATE_LOCK:
            # The CLI only injects the system prompt on the first turn; every
            # request here carries it so style rules also apply to follow-ups.
            messages = [{"role": "system", "content": oi.SYSTEM_PROMPT}]
            if new_topic:
                oi.history.clear()
            else:
                messages.extend({"role": m["role"], "content": m["content"]} for m in oi.history.get_context())
            cfg = dict(oi.config.data)
        messages.append({"role": "user", "content": user_input})

        api_key = cfg.get("api_key")
        if not api_key:
            return self._error("API Key 未设置，请在「设置」中填写", 400)

        # Per-request overrides let the workbench slider act without writing
        # the shared config file (CLI stays in sync with stored values).
        model = str(body.get("model") or cfg.get("model") or "deepseek-chat")
        max_tokens = int(body.get("max_tokens") or cfg.get("max_tokens", 8192))
        temperature = float(body["temperature"] if body.get("temperature") is not None else cfg.get("temperature", 0.3))
        self.send_response(200)
        self.send_header("Content-Type", "text/event-stream; charset=utf-8")
        self.send_header("Cache-Control", "no-cache, no-store")
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("X-Accel-Buffering", "no")
        self.send_header("Connection", "close")
        self.end_headers()

        broken = False

        def emit(obj):
            nonlocal broken
            if broken:
                return
            try:
                self.wfile.write(b"data: " + json_bytes(obj) + b"\n\n")
                self.wfile.flush()
            except (BrokenPipeError, ConnectionResetError, OSError):
                broken = True

        emit({"type": "start", "model": model})
        chunks = []
        try:
            client = OpenAI(api_key=api_key, base_url=cfg.get("base_url"), timeout=180.0)
            stream = client.chat.completions.create(
                model=model,
                messages=messages,
                max_tokens=max_tokens,
                temperature=temperature,
                stream=True,
            )
            for chunk in stream:
                if broken:
                    break
                if not chunk.choices:
                    continue
                delta = chunk.choices[0].delta.content or ""
                if delta:
                    chunks.append(delta)
                    emit({"type": "delta", "text": delta})
        except Exception as exc:
            emit({"type": "error", "message": str(exc)})
            self.close_connection = True
            return

        text = "".join(chunks)
        if text and not broken:
            with STATE_LOCK:
                oi.history.add("user", user_input)
                oi.history.add("assistant", text)
                count = len(oi.history.messages)
        else:
            count = 0
        emit({
            "type": "done",
            "content": text,
            "code": oi.extract_code(text) if text else None,
            "history_count": count,
        })
        self.close_connection = True

    def _save(self, body: dict):
        code = body.get("code")
        if not code:
            code = oi.get_last_code()
        if not code:
            return self._error("没有可保存的代码")
        name = str(body.get("filename") or "solution.cpp").strip()
        target = Path(name)
        if not target.is_absolute():
            target = ROOT_DIR / target
        target = target.resolve()
        if is_within(target, WEB_DIR.resolve()):
            return self._error("不能写入控制台自身目录")
        if target.suffix.lower() != ".cpp":
            target = target.with_suffix(".cpp")
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(code, encoding="utf-8")
        return self._json({"ok": True, "path": str(target), "bytes": len(code.encode("utf-8"))})

    def _read_file(self, body: dict):
        name = str(body.get("path") or "").strip()
        if not name:
            return self._error("路径为空")
        target = Path(name)
        if not target.is_absolute():
            target = ROOT_DIR / target
        target = target.resolve()
        if not target.is_file():
            return self._error("文件不存在: %s" % target, 404)
        if target.stat().st_size > MAX_FILE:
            return self._error("文件超过 2 MB")
        text = target.read_text(encoding="utf-8", errors="replace")
        return self._json({"ok": True, "path": str(target), "content": text})

    def _clipboard(self, body: dict):
        import pyperclip

        action = body.get("action")
        if action == "read":
            return self._json({"ok": True, "text": pyperclip.paste() or ""})
        if action == "write":
            pyperclip.copy(str(body.get("text") or ""))
            return self._json({"ok": True})
        return self._error("unknown clipboard action")


def main():
    parser = argparse.ArgumentParser(description="OI Code Console")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=8765)
    parser.add_argument("--no-browser", action="store_true")
    args = parser.parse_args()

    url = "http://%s:%d" % (args.host, args.port)
    server = ThreadingHTTPServer((args.host, args.port), ConsoleHandler)
    server.daemon_threads = True
    print("OI Code Console  ->  %s" % url)
    print("Workspace        ->  %s" % ROOT_DIR)
    print("Config           ->  %s" % oi.CONFIG_FILE)
    print("History          ->  %s" % oi.HISTORY_FILE)
    print("Press Ctrl+C to stop.")
    if not args.no_browser:
        threading.Timer(0.8, lambda: webbrowser.open(url)).start()
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nBye.")
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
