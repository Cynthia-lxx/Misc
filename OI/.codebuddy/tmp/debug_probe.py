# -*- coding: utf-8 -*-
"""Isolated debugger probe with stack dump on hang."""
import faulthandler
import sys

sys.path.insert(0, r"p:/Dev/OI/web")
faulthandler.dump_traceback_later(40, exit=True)

from analyzer import debugger  # noqa: E402

CODE = """#include <bits/stdc++.h>
int main(){
    int n;
    std::cin >> n;
    for (int i = 0; i < n; ++i) {
        std::cout << i * 2 << std::endl;
    }
    return 0;
}
"""
BP = 6  # the cout line, 1-based

print("start...", flush=True)
result = debugger.start(CODE, stdin="3", breakpoints=[BP])
print("start ->", result, flush=True)

if result.get("ok"):
    sid = result["session"]
    print("watch ->", debugger.command(sid, "watch_add", {"expr": "i"}), flush=True)
    print("next ->", debugger.command(sid, "next"), flush=True)
    print("continue ->", debugger.command(sid, "continue"), flush=True)
    print("stop ->", debugger.stop(sid), flush=True)
print("DEBUG PROBE DONE")
