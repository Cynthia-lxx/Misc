# -*- coding: utf-8 -*-
"""VERBOSE 复现：起 server 后直接 POST /api/run 计时（无 GUI）"""
import json
import sys
import threading
import time
import urllib.request

sys.path.insert(0, r"P:\Dev\OI\web")

import server  # noqa: E402  (web/server.py)

PORT = 8779

srv = server.ThreadingHTTPServer(("127.0.0.1", PORT), server.ConsoleHandler)
srv.daemon_threads = True
threading.Thread(target=srv.serve_forever, daemon=True).start()
print(f"server up on {PORT}", flush=True)

CODE = '#include <bits/stdc++.h>\nint main(){ std::cout << "Hello World!" << std::endl; return 0; }\n'

def post(path, payload):
    t0 = time.perf_counter()
    req = urllib.request.Request(
        f"http://127.0.0.1:{PORT}{path}",
        data=json.dumps(payload).encode(),
        headers={"Content-Type": "application/json"},
        method="POST",
    )
    try:
        with urllib.request.urlopen(req, timeout=40) as resp:
            body = json.loads(resp.read().decode())
        print(f"POST {path} -> {time.perf_counter()-t0:.1f}s ok", flush=True)
        return body
    except Exception as exc:
        print(f"POST {path} -> {time.perf_counter()-t0:.1f}s EXC {type(exc).__name__}: {exc}", flush=True)
        return None

r = post("/api/run", {"code": CODE, "stdin": "", "timeout": 5})
if r:
    for k, v in r.items():
        print(" ", k, "=", (str(v)[:160]), flush=True)
print("DONE", flush=True)
srv.shutdown()
