r"""NTLab entry point.

Usage: .venv\Scripts\python.exe main.py [--port 8765] [--server] [--no-browser]
Binds 127.0.0.1 only — never expose this to a network.

默认入口 = 内嵌浏览器客户端（pywebview 窗口，与最终 EXE 行为一致）：
  - 端口/终端开/关等设置在「调试」页热应用（由客户端负责）
  --server 才是纯服务器模式（调试页的部分设置在此模式生效）。
"""

from __future__ import annotations

import argparse
import os
import sys
import webbrowser

import uvicorn

from ntlab.api import create_app
from ntlab.core import appsettings
from ntlab.core.loghub import build_log_config

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 8765


def run_server(host: str, port: int, color: bool, open_browser: bool) -> None:
    if host != DEFAULT_HOST:
        print("WARNING: NTLab is a local research tool; binding non-loopback is discouraged.")
    app = create_app()
    os.environ["NTLAB_PORT"] = str(port)   # 调试页展示「当前监听」用
    url = f"http://{host}:{port}/"
    print(f"NTLab → {url}  (终端彩色: {'开' if color else '关'})")
    if open_browser:
        webbrowser.open(url)
    uvicorn.run(app, host=host, port=port, log_level="info",
                use_colors=color, log_config=build_log_config())


def main() -> None:
    parser = argparse.ArgumentParser(description="NTLab local server / client")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=None,
                        help=f"覆盖设置文件中的端口（默认 {DEFAULT_PORT}）")
    parser.add_argument("--server", action="store_true",
                        help="纯服务器模式（不开内嵌窗口；默认以客户端窗口启动）")
    parser.add_argument("--no-browser", action="store_true", help="do not open a browser tab")
    args = parser.parse_args()

    settings = appsettings.load()
    port = args.port if args.port is not None else int(settings["port"])
    color = bool(settings["color_terminal"])

    if args.server:
        open_browser = (not args.no_browser) and bool(settings["open_browser"])
        run_server(args.host, port, color, open_browser)
        return

    # 默认：内嵌浏览器客户端（与最终 EXE 的行为一致）
    try:
        from ntlab.clientapp import main as client_main
    except ImportError:
        print("pywebview 未安装，回退到纯服务器模式。安装客户端依赖："
              ".venv\\Scripts\\pip.exe install pywebview")
        open_browser = (not args.no_browser) and bool(settings["open_browser"])
        run_server(args.host, port, color, open_browser)
        return
    client_main(port=port)


if __name__ == "__main__":
    main()
