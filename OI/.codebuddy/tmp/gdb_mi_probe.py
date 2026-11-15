# -*- coding: utf-8 -*-
"""Drive gdb MI raw to see exact responses for our command sequence."""
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
print("build ok=%s exe=%s" % (ok, exe), flush=True)
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
            print("[out] EOF", flush=True)
            return
        print("[out]", line.rstrip(), flush=True)


threading.Thread(target=reader, daemon=True).start()
time.sleep(1)

gdb_path = str(exe).replace("\\", "/")
cmds = [
    f'2-file-exec-and-symbols "{gdb_path}"',
    '3-break-insert "main.cpp:6"',
    '4-exec-run < "in.txt" > "out.txt" 2> "err.txt"',
    '5-data-evaluate-expression "$_exitcode"',
]
for c in cmds:
    print("[in ]", c, flush=True)
    proc.stdin.write(c + "\n")
    proc.stdin.flush()
    time.sleep(2)

time.sleep(2)
print("--- final out.txt:", end=" ", flush=True)
try:
    print((folder / "out.txt").read_text(encoding="utf-8", errors="replace"))
except Exception as exc:
    print("missing:", exc)
proc.kill()
print("MI PROBE DONE")
