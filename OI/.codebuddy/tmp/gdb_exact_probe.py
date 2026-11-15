# -*- coding: utf-8 -*-
"""Replicate with the EXACT code digest used by _Session to bisect folder vs class."""
import faulthandler
import hashlib
import os
import subprocess
import sys
import threading
import time
from pathlib import Path

sys.path.insert(0, r"p:/Dev/OI/web")
faulthandler.dump_traceback_later(60, exit=True)
from analyzer import toolchain  # noqa: E402

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

env = dict(os.environ)
env["PATH"] = str(toolchain.BIN_DIR) + os.pathsep + env.get("PATH", "")
flags = getattr(subprocess, "CREATE_NO_WINDOW", 0)

digest = hashlib.sha256(CODE.encode("utf-8", "replace")).hexdigest()
folder = toolchain.DEBUG_CACHE / digest
print("digest folder:", folder, "exists:", folder.is_dir(), flush=True)

print("1. build (cached expected)...", flush=True)
ok, exe, ms, err, cached = toolchain._cached_build(
    CODE, toolchain.DEBUG_CACHE, ["-g", "-O0", "-static-libgcc", "-static-libstdc++"],
    "main.cpp", "main.exe", env, flags,
)
print("   ok=%s exe=%s cached=%s" % (ok, exe, cached), flush=True)

(folder / "in.txt").write_text("3", encoding="utf-8")
print("2. spawn gdb in that folder...", flush=True)
proc = subprocess.Popen(
    [str(toolchain.BIN_DIR / "gdb.exe"), "--interpreter=mi2", "--quiet", "--nx"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE,
    stderr=subprocess.DEVNULL, cwd=str(folder), env=env, creationflags=flags,
)


def reader():
    while True:
        line = proc.stdout.readline()
        if not line:
            print("   [reader] EOF", flush=True)
            return
        print("   [reader]", line[:70], flush=True)


threading.Thread(target=reader, daemon=True).start()
time.sleep(6)
print("3. alive:", proc.poll() is None, flush=True)
proc.kill()
print("EXACT PROBE DONE")
