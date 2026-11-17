r"""NTLab entry point.

Usage: .venv\Scripts\python.exe main.py [--port 8765] [--no-browser]
Binds 127.0.0.1 only — never expose this to a network.

纯服务器模式（调试页的设置在此生效：端口、终端彩色、自动开浏览器）。
内嵌浏览器客户端见 client.pyw（设置热应用与终端开/关由客户端负责）。
"""

from __future__ import annotations

import argparse
import os
import webbrowser

import uvicorn

from ntlab.api import create_app
from ntlab.core import appsettings
from ntlab.core.loghub import build_log_config

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 8765


def main() -> None:
    parser = argparse.ArgumentParser(description="NTLab local server")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=None,
                        help=f"覆盖设置文件中的端口（默认 {DEFAULT_PORT}）")
    parser.add_argument("--no-browser", action="store_true", help="do not open a browser tab")
    args = parser.parse_args()

    settings = appsettings.load()
    port = args.port if args.port is not None else int(settings["port"])
    color = bool(settings["color_terminal"])
    open_browser = (not args.no_browser) and bool(settings["open_browser"])

    if args.host != DEFAULT_HOST:
        print("WARNING: NTLab is a local research tool; binding non-loopback is discouraged.")

    app = create_app()
    os.environ["NTLAB_PORT"] = str(port)   # 调试页展示「当前监听」用
    url = f"http://{args.host}:{port}/"
    print(f"NTLab → {url}  (终端彩色: {'开' if color else '关'})")
    if open_browser:
        webbrowser.open(url)
    uvicorn.run(app, host=args.host, port=port, log_level="info",
                use_colors=color, log_config=build_log_config())


if __name__ == "__main__":
    main()
