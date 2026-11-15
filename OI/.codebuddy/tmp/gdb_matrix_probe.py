# -*- coding: utf-8 -*-
"""Bisect which launch condition makes gdb silent."""
import os
import subprocess
import sys
import threading
import time

sys.path.insert(0, r"p:/Dev/OI/web")
from analyzer import toolchain  # noqa: E402

GDB = toolchain.BIN_DIR / "gdb.exe"
ENV = dict(os.environ)
ENV["PATH"] = str(toolchain.BIN_DIR) + os.pathsep + ENV.get("PATH", "")
FLAGS = getattr(subprocess, "CREATE_NO_WINDOW", 0)
CWD = r"p:/Dev/OI"  # any real dir


def trial(name, cwd=None, env=None, flags=0, stderr_mode=subprocess.PIPE):
    proc = subprocess.Popen(
        [str(GDB), "--interpreter=mi2", "--quiet", "--nx"],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=stderr_mode,
        cwd=cwd, env=env, creationflags=flags,
    )
    lines = []

    def reader():
        while True:
            line = proc.stdout.readline()
            if not line:
                return
            lines.append(line)

    t = threading.Thread(target=reader, daemon=True)
    t.start()
    time.sleep(3)
    alive = proc.poll() is None
    proc.kill()
    proc.wait(timeout=5)
    print(f"{name}: alive_after_3s={alive} first_line={lines[0][:60] if lines else None} n={len(lines)}", flush=True)


trial("A bare")
trial("B +cwd", cwd=CWD)
trial("C +env", env=ENV)
trial("D +flags", flags=FLAGS)
trial("E +stderr devnull", stderr_mode=subprocess.DEVNULL)
trial("F all", cwd=CWD, env=ENV, flags=FLAGS, stderr_mode=subprocess.DEVNULL)
print("MATRIX DONE")
