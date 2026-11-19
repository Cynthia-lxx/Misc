"""OI助手 发行入口 —— fspack 用。

默认：GUI 客户端（pywebview 窗口 + 后台服务线程，NTLab 同款开箱即用）。
`--server`：纯服务模式（无窗口，浏览器打开，供调试/无 GUI 场景）。
`--port N`：本次运行覆盖端口（不写回设置文件）。

顶层 `import webview` 是给 fspack 看：据此把 AppType 推断为 gui
（产出无控制台 loader）；开发环境没装 pywebview 也不影响 --server 模式。
"""

from __future__ import annotations

import sys
from pathlib import Path

try:
    import webview  # noqa: F401  (fspack AppType=gui 推断标记)
except ImportError:  # pragma: no cover - 开发环境未装 pywebview 时仍可用 --server
    webview = None

_SRC = Path(__file__).resolve().parent
if str(_SRC) not in sys.path:
    sys.path.insert(0, str(_SRC))          # OI.py / web/
if str(_SRC / "web") not in sys.path:
    sys.path.insert(1, str(_SRC / "web"))  # server / guiclient / appsettings / analyzer

import server  # noqa: E402


def _cli_port() -> int | None:
    argv = sys.argv[1:]
    if "--port" in argv:
        idx = argv.index("--port")
        if idx + 1 < len(argv):
            try:
                return int(argv[idx + 1])
            except ValueError:
                pass
    return None


def main() -> None:
    argv = sys.argv[1:]
    if "--server" in argv:
        # server.main() 的 argparse 不认识 --server（这是本入口的分发标志），
        # 转发前摘掉；其余参数（--host/--port/--no-browser）原样透传
        sys.argv = [sys.argv[0]] + [a for a in argv if a != "--server"]
        server.main()
        return
    if webview is None:
        print("pywebview 未安装：GUI 不可用，回退纯服务模式（--server）", flush=True)
        server.main()
        return
    import guiclient

    guiclient.main(_cli_port())


if __name__ == "__main__":
    main()
