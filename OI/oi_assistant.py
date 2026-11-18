"""OI助手 发行入口（fspack 用）——把 web/server.py 拉起来。

web/server.py 自己会把 web/ 与项目根塞进 sys.path 并导入 OI/prompt_store/analyzer，
这里只负责准备好路径后转调它的 main()。OI.py（CLI）保持不动。
"""

from __future__ import annotations

import sys
from pathlib import Path

_SRC = Path(__file__).resolve().parent
if str(_SRC) not in sys.path:
    sys.path.insert(0, str(_SRC))          # OI.py / web/
if str(_SRC / "web") not in sys.path:
    sys.path.insert(1, str(_SRC / "web"))  # server / prompt_store / appsettings / analyzer

import server  # noqa: E402  (web/server.py)


def main() -> None:
    server.main()


if __name__ == "__main__":
    main()
