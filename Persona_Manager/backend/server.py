"""Tiny stdlib HTTP server exposing the template manager API and the web UI.

Run with::

    python backend/server.py

Then open http://127.0.0.1:8765/  (the management UI) or call the JSON API
from any tool. No third-party dependency is required.
"""
import json
import mimetypes
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from urllib.parse import urlparse

import config
import store


class Handler(BaseHTTPRequestHandler):
    server_version = "PersonaManager/1.0"

    # ---------- low level helpers ----------
    def _send(self, status: int, body, content_type: str = "application/json; charset=utf-8") -> None:
        if isinstance(body, (dict, list)):
            data = json.dumps(body, ensure_ascii=False).encode("utf-8")
        elif isinstance(body, str):
            data = body.encode("utf-8")
        else:
            data = body
        self.send_response(status)
        self.send_header("Content-Type", content_type)
        self.send_header("Content-Length", str(len(data)))
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()
        if self.command != "HEAD":
            self.wfile.write(data)

    def _json(self, status: int, obj) -> None:
        self._send(status, obj)

    def _error(self, status: int, message: str) -> None:
        self._json(status, {"error": message})

    def _body(self) -> dict:
        length = int(self.headers.get("Content-Length", "0") or 0)
        if not length:
            return {}
        raw = self.rfile.read(length).decode("utf-8")
        try:
            value = json.loads(raw)
        except json.JSONDecodeError:
            return {}
        return value if isinstance(value, dict) else {}

    def _serve_static(self, path: str) -> bool:
        if path in ("/", ""):
            path = "/index.html"
        target = (config.FRONTEND / path.lstrip("/")).resolve()
        if config.FRONTEND.resolve() not in target.parents and target != config.FRONTEND.resolve():
            self._error(403, "forbidden")
            return True
        if not target.is_file():
            return False
        ctype = mimetypes.guess_type(str(target))[0] or "application/octet-stream"
        if ctype.startswith("text/"):
            ctype += "; charset=utf-8"
        self._send(200, target.read_bytes(), ctype)
        return True

    # ---------- routing ----------
    def do_OPTIONS(self) -> None:
        self._send(204, b"")

    def do_GET(self) -> None:
        path = urlparse(self.path).path
        try:
            if path == config.API_HEALTH:
                return self._json(200, {"ok": True, "version": "1.0"})
            if path == config.API_TEMPLATES:
                return self._json(200, {"templates": store.list_templates()})
            if path == config.API_PREFIX:
                return self._json(200, store.build_prefix())
            if path == config.API_VARS:
                return self._json(200, {"variables": store.list_variables()})
            if path == config.API_PRESETS:
                return self._json(200, {"presets": store.list_presets(), "active": store.active_preset()})
            if path == config.API_PROMPT:
                # The single call an LLM tool makes to obtain its system prompt.
                return self._json(200, store.build_prefix())
        except Exception as exc:  # noqa: BLE001 - surface any error to the caller
            return self._error(500, str(exc))

        if not self._serve_static(path):
            self._error(404, "not found")

    def do_POST(self) -> None:
        path = urlparse(self.path).path
        body = self._body()
        try:
            if path == config.API_TEMPLATES:
                key = body.get("key")
                content = body.get("content")
                base = body.get("baseRevision")
                if not isinstance(key, str) or not isinstance(content, str):
                    return self._error(400, "key and content are required")
                doc = store.save_template(key, content, base)
                return self._json(200, {"result": "ok", "template": doc})
            if path == config.API_RESET:
                key = body.get("key")
                if not isinstance(key, str):
                    return self._error(400, "key is required")
                doc = store.reset_template(key)
                if doc is None:
                    return self._error(404, "unknown template")
                return self._json(200, {"result": "ok", "template": doc})
            if path == config.API_VARS:
                values = body.get("values")
                if not isinstance(values, dict):
                    return self._error(400, "values must be an object")
                return self._json(200, {"result": "ok", "variables": store.set_variable_values(values)})
            if path == config.API_ROLE_APPLY:
                preset_id = body.get("id")
                applied = store.apply_preset(preset_id) if isinstance(preset_id, str) else None
                if applied is None:
                    return self._error(404, "unknown preset")
                return self._json(200, {"result": "ok", "preset": applied})
            if path == config.API_RELOAD:
                # Templates are read from disk on every call, so no cache to clear;
                # the endpoint exists so callers can use the same contract as before.
                return self._json(200, {"result": "ok", "prefix": store.build_prefix()})
        except store.ConflictError:
            return self._error(409, "template changed since it was loaded; reload before saving")
        except (KeyError, ValueError) as exc:
            return self._error(400, str(exc))
        except Exception as exc:  # noqa: BLE001
            return self._error(500, str(exc))
        self._error(404, "not found")


def main() -> None:
    config.ensure_data()
    server = ThreadingHTTPServer((config.HOST, config.PORT), Handler)
    print(f"Persona Manager listening on http://{config.HOST}:{config.PORT}/")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
    finally:
        server.server_close()


if __name__ == "__main__":
    main()
