# -*- coding: utf-8 -*-
"""Raw gdb MI behavior check: what does it print, and when?"""
import subprocess
import sys
import threading
import time

sys.path.insert(0, r"p:/Dev/OI/web")
from analyzer import debugger  # noqa: E402

print("gdb:", debugger.GDB, debugger.GDB.is_file(), flush=True)
proc = subprocess.Popen(
    [str(debugger.GDB), "--interpreter=mi2", "--quiet", "--nx"],
    stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
)


def reader(name, stream):
    while True:
        line = stream.readline()
        if not line:
            print(f"[{name}] EOF", flush=True)
            return
        print(f"[{name}] {line!r}", flush=True)


threading.Thread(target=reader, args=("out", proc.stdout), daemon=True).start()
threading.Thread(target=reader, args=("err", proc.stderr), daemon=True).start()

time.sleep(2)
print("--- sending 1-gdb-exit after 2s silence", flush=True)
try:
    proc.stdin.write(b"1-gdb-exit\n")
    proc.stdin.flush()
except Exception as exc:
    print("stdin failed:", exc, flush=True)
time.sleep(2)
if proc.poll() is None:
    print("gdb still alive, killing", flush=True)
    proc.kill()
else:
    print("gdb exited:", proc.returncode, flush=True)
print("RAW PROBE DONE")
