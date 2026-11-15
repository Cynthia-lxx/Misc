# -*- coding: utf-8 -*-
"""Replicate _Session.start step by step with prints."""
import faulthandler
import os
import subprocess
import sys
import threading
import time

sys.path.insert(0, r"p:/Dev/OI/web")
faulthandler.dump_traceback_later(60, exit=True)
from analyzer import toolchain  # noqa: E402

GDB = toolchain.BIN_DIR / "gdb.exe"
CODE = open(__file__, encoding="utf-8").read()  # any content; debug build cost only
CODE = "int main(){return 0;}"  # tiny -> fast compile

env = dict(os.environ)
env["PATH"] = str(toolchain.BIN_DIR) + os.pathsep + env.get("PATH", "")
flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)

print("1. building...", flush=True)
t0 = time.time()
ok, exe, ms, err, cached = toolchain._cached_build(
    CODE, toolchain.DEBUG_CACHE, ["-g", "-O0", "-static-libgcc", "-static-libstdc++"],
    "main.cpp", "main.exe", env, flags,
)
print("   build ok=%s exe=%s ms=%s err=%r cached=%s (%.1fs)" % (ok, exe, ms, err[:60], cached, time.time() - t0), flush=True)

folder = exe.parent
(folder / "in.txt").write_text("3", encoding="utf-8")
print("2. spawning gdb...", flush=True)
proc = subprocess.Popen(
    [str(GDB), "--interpreter=mi2", "--quiet", "--nx"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL, cwd=str(folder), env=env, creationflags=flags,
)
print("   pid", proc.pid, flush=True)


def reader():
    while True:
        line = proc.stdout.readline()
        if not line:
            print("   [reader] EOF", flush=True)
            return
        print("   [reader]", line[:80], flush=True)


threading.Thread(target=reader, daemon=True).start()
time.sleep(5)
print("3. alive:", proc.poll() is None, flush=True)
proc.kill()
print("STEP PROBE DONE")
