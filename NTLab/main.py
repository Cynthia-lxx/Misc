"""NTLab entry point.

Usage: .venv\Scripts\python.exe main.py [--port 8765] [--no-browser]
Binds 127.0.0.1 only — never expose this to a network.
"""

from __future__ import annotations

import argparse
import webbrowser

import uvicorn

from ntlab.api import create_app

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 8765


def main() -> None:
    parser = argparse.ArgumentParser(description="NTLab local server")
    parser.add_argument("--host", default=DEFAULT_HOST)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument("--no-browser", action="store_true", help="do not open a browser tab")
    args = parser.parse_args()

    if args.host != DEFAULT_HOST:
        print("WARNING: NTLab is a local research tool; binding non-loopback is discouraged.")

    app = create_app()
    url = f"http://{args.host}:{args.port}/"
    print(f"NTLab → {url}")
    if not args.no_browser:
        webbrowser.open(url)
    uvicorn.run(app, host=args.host, port=args.port, log_level="info")


if __name__ == "__main__":
    main()
