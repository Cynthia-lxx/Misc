# -*- coding: utf-8 -*-
"""Probe v2: gdbserver hosts inferior, gdb loads symbols, attaches, bp, step."""
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
print("build:", ok, exe, flush=True)

PORT = "127.0.0.1:39992"
server = subprocess.Popen(
    [str(toolchain.BIN_DIR / "gdbserver.exe"), "--once", PORT, str(exe)],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
    env=env, creationflags=flags, text=True,
)


def server_reader():
    while True:
        line = server.stdout.readline()
        if not line:
            print("[srv] EOF", flush=True)
            return
        print("[srv]", line.rstrip()[:110], flush=True)


threading.Thread(target=server_reader, daemon=True).start()
time.sleep(2)

gdb = subprocess.Popen(
    [str(toolchain.BIN_DIR / "gdb.exe"), "--interpreter=mi2", "--quiet", "--nx"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL,
    env=env, creationflags=flags, text=True,
)


def gdb_reader():
    while True:
        line = gdb.stdout.readline()
        if not line:
            return
        body = line.rstrip()
        if body.startswith("*") or "^" in body[:8] or "Breakpoint" in body:
            print("[gdb]", body[:150], flush=True)


threading.Thread(target=gdb_reader, daemon=True).start()
time.sleep(1)

gdb_path = str(exe).replace("\\", "/")
cmds = [
    f'1-file-exec-and-symbols "{gdb_path}"',
    f'2-target-select remote {PORT}',
    '3-break-insert "main.cpp:6"',
    '4-exec-continue',
    '5-stack-list-variables --simple-values',
]
for c in cmds:
    print("[in ]", c, flush=True)
    gdb.stdin.write(c + "\n")
    gdb.stdin.flush()
    time.sleep(3)

print("--- server stdin write '3'", flush=True)
server.stdin.write("3\n")
server.stdin.flush()
time.sleep(2)
print("--- next after bp", flush=True)
gdb.stdin.write("6-exec-next\n")
gdb.stdin.flush()
time.sleep(2)
print("--- continue to end", flush=True)
gdb.stdin.write("7-exec-continue\n")
gdb.stdin.flush()
time.sleep(4)
print("SERVER PROBE DONE", flush=True)
server.kill()
gdb.kill()
