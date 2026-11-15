# -*- coding: utf-8 -*-
"""Timing isolation: same command sequence with NO sleeps at all."""
import os
import subprocess
import sys
import threading
import time

sys.path.insert(0, r"p:/Dev/OI/web")
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

ok, exe, ms, err, cached = toolchain._cached_build(
    CODE, toolchain.DEBUG_CACHE, ["-g", "-O0", "-static-libgcc", "-static-libstdc++"],
    "main.cpp", "main.exe", env, flags,
)
folder = exe.parent
(folder / "in.txt").write_text("3", encoding="utf-8")

proc = subprocess.Popen(
    [str(toolchain.BIN_DIR / "gdb.exe"), "--interpreter=mi2", "--quiet", "--nx"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
    cwd=str(folder), env=env, creationflags=flags, text=True,
)


def reader():
    while True:
        line = proc.stdout.readline()
        if not line:
            return
        if line.startswith("*stopped") or "Breakpoint" in line:
            print("[out]", line.rstrip()[:130], flush=True)


threading.Thread(target=reader, daemon=True).start()

gdb_path = str(exe).replace("\\", "/")
cmds = [
    f'1-file-exec-and-symbols "{gdb_path}"',
    '2-break-insert "main.cpp:6"',
    '3-interpreter-exec console "run < in.txt > out.txt 2> err.txt"',
]
for c in cmds:
    proc.stdin.write(c + "\n")
    proc.stdin.flush()

time.sleep(8)
print("--- out.txt:", end=" ", flush=True)
try:
    print(repr((folder / "out.txt").read_text(encoding="utf-8", errors="replace")))
except Exception as exc:
    print("missing:", exc)
proc.kill()
print("NOSLEEP PROBE DONE")
