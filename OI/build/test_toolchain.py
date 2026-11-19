# -*- coding: utf-8 -*-
"""VERBOSE 复现：直接调 toolchain.run_program / measure（不经 HTTP、不起 GUI）"""
import sys
import time

sys.path.insert(0, r"P:\Dev\OI\web")

from analyzer import toolchain  # noqa: E402

CODE = '#include <bits/stdc++.h>\nint main(){ std::cout << "Hello World!" << std::endl; return 0; }\n'

print("available:", toolchain.available(), flush=True)
print("GXX exists:", toolchain.GXX.is_file(), toolchain.GXX, flush=True)

t0 = time.perf_counter()
print("run_program ...", flush=True)
r = toolchain.run_program(CODE, timeout=5.0)
print("run_program elapsed %.1fs" % (time.perf_counter() - t0), flush=True)
for k, v in r.items():
    print(" ", k, "=", (v[:200] if isinstance(v, str) else v), flush=True)

t0 = time.perf_counter()
print("measure ...", flush=True)
m = toolchain.measure(CODE, timeout=3.0)
print("measure elapsed %.1fs" % (time.perf_counter() - t0), flush=True)
for k, v in m.items():
    print(" ", k, "=", (v[:200] if isinstance(v, str) else v), flush=True)
print("DONE", flush=True)
