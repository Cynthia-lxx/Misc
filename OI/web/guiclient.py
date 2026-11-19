"""OI助手 GUI 客户端 —— NTLab clientapp.py 同款方法：
后台服务线程（stdlib http.server）+ pywebview 内嵌页面（WebView2/EdgeChromium）。

- 开箱即用：双击 EXE 无终端，直接出 GUI 窗口
- 端口探测：端口上已是 OI 助手 → 附着（不重复起服务）；
  被其他程序占用 → 明确提示
- 高 DPI：进程级 Per-Monitor DPI Aware，必须在创建任何窗口之前
"""

from __future__ import annotations

import ctypes
import json
import socket
import threading
import time
from pathlib import Path

import appsettings
import server as webserver

_ICON = Path(__file__).resolve().parent / "static" / "assets" / "app.ico"

_PLACEHOLDER = (
    "<body style='background:#141414;color:#ddd;font-family:sans-serif;padding:40px'>"
    "<h2>{title}</h2><p>{body}</p></body>"
)


def _enable_high_dpi() -> None:
    try:
        ctypes.windll.shcore.SetProcessDpiAwareness(2)   # PER_MONITOR_DPI_AWARE
    except Exception:
        try:
            ctypes.windll.user32.SetProcessDPIAware()
        except Exception:
            pass


def _port_in_use(port: int) -> bool:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(0.3)
        return s.connect_ex(("127.0.0.1", port)) == 0


def _port_is_oi(port: int) -> bool:
    """探测端口上是否已是 OI 助手服务（避免误附着到别的程序）。"""
    try:
        import urllib.request

        with urllib.request.urlopen(f"http://127.0.0.1:{port}/api/status", timeout=1.5) as r:
            data = json.loads(r.read().decode("utf-8", "replace"))
            return r.status == 200 and data.get("ok") is True and "history_count" in data
    except Exception:
        return False


def _placeholder_window(icon, title: str, body: str) -> None:
    import webview

    webview.create_window(
        f"OI Code Console — {title}",
        html=_PLACEHOLDER.format(title=title, body=body),
        width=560, height=300,
    )
    webview.start(gui="edgechromium", icon=icon)


def main(port: int | None = None) -> None:
    """GUI 入口；port 用于 CLI --port 覆盖（仅本次运行，不写回设置文件）。"""
    import webview

    _enable_high_dpi()
    settings = appsettings.load()
    port = int(port) if port is not None else int(settings["port"])
    icon = str(_ICON) if _ICON.is_file() else None

    srv = None
    attached = False
    if _port_in_use(port):
        if _port_is_oi(port):
            attached = True   # 已有 OI 助手实例：直接附着，不重复起服务
        else:
            _placeholder_window(
                icon, "端口被占用",
                f"端口 {port} 已被其他程序占用。请关闭占用它的进程，"
                f"或在「设置 → 服务器」里更换端口后重启。",
            )
            return
    else:
        srv = webserver.build_server("127.0.0.1", port)
        threading.Thread(target=srv.serve_forever, daemon=True).start()
        deadline = time.time() + 10
        while time.time() < deadline and not _port_in_use(port):
            time.sleep(0.1)
        print("OI Code Console  ->  http://127.0.0.1:%d" % port, flush=True)

    url = f"http://127.0.0.1:{port}/"
    webview.create_window(
        "OI Code Console", url,
        width=1360, height=860, min_size=(980, 640),
        background_color="#141414",
    )
    webview.start(gui="edgechromium", icon=icon)
    # 窗口关闭：附着模式不动别人的服务；自起模式随 daemon 线程退出前先收尾
    if srv is not None:
        srv.shutdown()
