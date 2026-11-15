# -*- coding: utf-8 -*-
"""Backend smoke probe: /api/run stdout/stderr split + /api/debug session."""
import json
import time
import urllib.request

BASE = "http://127.0.0.1:8765"

CODE = """#include <bits/stdc++.h>
int main(){
    int n;
    std::cin >> n;
    std::cerr << "debug line " << n << std::endl;
    for (int i = 0; i < n; ++i) {
        std::cout << i * 2 << (i + 1 < n ? ' ' : '\\n');
    }
    return 0;
}
"""


def call(method, path, payload=None, timeout=60):
    data = None if payload is None else json.dumps(payload).encode("utf-8")
    req = urllib.request.Request(BASE + path, data=data, method=method)
    if data:
        req.add_header("Content-Type", "application/json")
    with urllib.request.urlopen(req, timeout=timeout) as resp:
        return json.loads(resp.read().decode("utf-8"))


# 1. debug info
info = call("GET", "/api/debug/info")
print("gdb available:", info["available"])

# 2. run with stdout/stderr split (first: compile, second: cached)
t0 = time.time()
r1 = call("POST", "/api/run", {"code": CODE, "stdin": "5"})
print("run1: status=%s compile=%.0fms wall=%.1fms cached=%s %.1fs total"
      % (r1["status"], r1["compile_ms"], r1["wall_ms"], r1["compile_cached"], time.time() - t0))
print("  stdout=%r" % r1["stdout"])
print("  stderr=%r" % r1["stderr"])
print("  exit=%s peak=%s" % (r1["exit_code"], r1["peak_bytes"]))
r2 = call("POST", "/api/run", {"code": CODE, "stdin": "5"})
print("run2 cached:", r2["compile_cached"], "compile_ms:", r2["compile_ms"])

# 3. compile error path
bad = call("POST", "/api/run", {"code": "int main(){ return oops; }"})
print("bad run:", bad["status"], (bad.get("stderr") or "")[:80].replace("\n", " | "))

# 4. debug session: breakpoint on the cout line (line 7 in CODE, 1-based)
bp_line = CODE.splitlines().index("        std::cout << i * 2 << (i + 1 < n ? ' ' : '\\n');") + 1
d = call("POST", "/api/debug/start", {"code": CODE, "stdin": "3", "breakpoints": [bp_line]}, timeout=90)
print("debug start:", d["ok"], d.get("error", ""))
if d.get("ok"):
    sid = d["session"]
    st = d["state"]
    print("  state: status=%s line=%s reason=%s" % (st["status"], st["line"], st["reason"]))
    print("  vars:", [(v["name"], v["value"]) for v in st["variables"]][:6])
    w = call("POST", "/api/debug/command", {"session": sid, "action": "watch_add", "expr": "i"})
    print("  watch i:", [(x["expr"], x["value"]) for x in w["state"]["watch"]])
    n = call("POST", "/api/debug/command", {"session": sid, "action": "next"})
    print("  next -> status=%s line=%s reason=%s" % (n["state"]["status"], n["state"]["line"], n["state"]["reason"]))
    c = call("POST", "/api/debug/command", {"session": sid, "action": "continue"}, timeout=60)
    print("  continue -> status=%s line=%s exit=%s stdout=%r"
          % (c["state"]["status"], c["state"]["line"], c["state"]["exit_code"], c["state"]["stdout"]))
    s = call("POST", "/api/debug/stop", {"session": sid})
    print("  stop:", s["ok"])

print("PROBE DONE")
