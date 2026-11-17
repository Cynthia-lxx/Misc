r"""NTLab 内嵌浏览器客户端（pywebview + EdgeChromium/WebView2）。

B/S 套壳：本文件在 GUI 进程内起 uvicorn 服务线程，再以内嵌 WebView2 打开
项目页面——对用户就是一个桌面客户端。高 DPI 由进程级 DPI aware + WebView2 缩放保证。

「调试」页设置的热应用：
  - 服务器端口变更   → 在本进程内重启服务线程（端口被占用则保持旧端口并回报错误）
  - 程序终端开/关    → 以 python.exe / pythonw.exe 重新拉起自身后退出当前进程
  - 终端彩色 / 自动浏览器 → 落盘，进程级参数（下次启动生效）

用法： .venv\Scripts\python.exe main.py          （默认入口，直接打开内嵌窗口）
       .venv\Scripts\python.exe client.pyw      （等价的客户端入口，可双击）
       .venv\Scripts\python.exe main.py --server（纯服务器模式，不开窗口）
"""

from __future__ import annotations

import ctypes
import json
import os
import socket
import subprocess
import sys
import threading
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]   # 项目根（ntlab/ 的上一级）

import uvicorn

from ntlab.api import create_app
from ntlab.core import appsettings
from ntlab.core.loghub import build_log_config

_IS_FROZEN = getattr(sys, "frozen", False)


# ---- 高 DPI：进程级 Per-Monitor DPI Aware（必须在创建任何窗口之前） ----
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


def _console_attached() -> bool:
    try:
        return ctypes.windll.kernel32.GetConsoleWindow() != 0
    except Exception:
        return True


def _port_is_ntlab(port: int) -> bool:
    """探测端口上是否已是 NTLab 服务（避免重复起服务）。"""
    try:
        import urllib.request

        with urllib.request.urlopen(
            f"http://127.0.0.1:{port}/api/system/info", timeout=1.5
        ) as r:
            return r.status == 200 and b"ntlab" in r.read().lower()
    except Exception:
        return False


def _port_has_app_api(port: int) -> bool:
    """已有 NTLab 是否为新版本（带调试设置 API）。"""
    try:
        import urllib.request

        with urllib.request.urlopen(
            f"http://127.0.0.1:{port}/api/app/settings", timeout=1.5
        ) as r:
            return r.status == 200
    except Exception:
        return False


class ServerRunner:
    """可重启的 uvicorn 服务线程（同进程内换端口）。"""

    def __init__(self, port: int, color_terminal: bool) -> None:
        self._port = port
        self._color = color_terminal
        self._server: uvicorn.Server | None = None
        self._thread: threading.Thread | None = None
        os.environ["NTLAB_PORT"] = str(port)

    @property
    def port(self) -> int:
        return self._port

    def start(self) -> None:
        cfg = uvicorn.Config(
            create_app(), host="127.0.0.1", port=self._port,
            log_level="info", use_colors=self._color and _console_attached(),
            log_config=build_log_config(),
        )
        self._server = uvicorn.Server(cfg)
        self._thread = threading.Thread(target=self._server.run, daemon=True)
        self._thread.start()

    def stop(self) -> None:
        if self._server is not None:
            self._server.should_exit = True
        if self._thread is not None:
            self._thread.join(timeout=5)
        self._server = None
        self._thread = None

    def restart(self, port: int, color_terminal: bool) -> tuple[bool, str]:
        """换端口重启；绑定失败时保持旧端口并回报（不静默换端口）。"""
        self.stop()
        old_port, old_color = self._port, self._color
        self._port, self._color = port, color_terminal
        try:
            self.start()
        except OSError as exc:
            self._port, self._color = old_port, old_color
            self.start()
            return False, f"端口 {port} 绑定失败（{exc}），服务保持旧端口 {old_port}"
        # 等待真正监听（uvicorn 线程启动是异步的）
        deadline = time.time() + 8
        while time.time() < deadline:
            if _port_in_use(self._port):
                os.environ["NTLAB_PORT"] = str(self._port)
                return True, f"服务已切换到端口 {self._port}"
            time.sleep(0.15)
        self.stop()
        self._port, self._color = old_port, old_color
        self.start()
        return False, f"端口 {port} 未能在超时内就绪（可能被占用），服务保持旧端口 {old_port}"


class ClientApi:
    """暴露给页面 JS 的 pywebview API（window.pywebview.api.*）。"""

    def __init__(self, runner: ServerRunner) -> None:
        self._runner = runner

    def applySettings(self, payload_json: str) -> dict:  # noqa: N802
        try:
            payload = json.loads(payload_json)
        except json.JSONDecodeError as exc:
            return {"ok": False, "message": f"参数解析失败：{exc}"}

        old = appsettings.load()
        messages: list[str] = []

        # 1) 终端开/关：需要以 pythonw / python 重启整个进程（最后执行）
        if bool(payload.get("no_console")) != bool(old.get("no_console")):
            threading.Timer(0.8, _relaunch, args=(bool(payload["no_console"]),)).start()
            messages.append("终端模式已切换，程序即将自动重启")

        # 2) 端口变更 → 进程内重启服务线程；彩色立即生效到新服务线程
        want_port = int(payload.get("port", old["port"]))
        want_color = bool(payload.get("color_terminal", old["color_terminal"]))
        if want_port != self._runner.port or want_color != self._runner._color:
            ok, msg = self._runner.restart(want_port, want_color)
            messages.append(msg)
            if not ok:
                return {"ok": False, "message": "；".join(messages)}

        return {"ok": True, "message": "；".join(messages) or "已应用", "port": self._runner.port}


def _relaunch(no_console: bool) -> None:
    """以 python.exe / pythonw.exe 重新拉起客户端后退出当前进程。"""
    if _IS_FROZEN:
        target = [sys.executable]
    else:
        exe = Path(sys.executable)
        name = "pythonw.exe" if no_console else "python.exe"
        target = [str(exe.with_name(name)), str(ROOT / "client.pyw")]
    flags = subprocess.DETACHED_PROCESS | subprocess.CREATE_NEW_PROCESS_GROUP
    subprocess.Popen(target, cwd=str(ROOT), creationflags=flags, close_fds=True)
    os._exit(0)


def main(port: int | None = None) -> None:
    """客户端入口；port 用于 CLI --port 覆盖（仅本次运行，不写回设置文件）。"""
    import webview

    _enable_high_dpi()
    os.environ["NTLAB_CLIENT"] = "1"   # 调试页据此显示运行形态「内嵌客户端」
    settings = appsettings.load()
    port = int(port) if port is not None else int(settings["port"])

    runner = ServerRunner(port, bool(settings["color_terminal"]))
    attached = False
    if _port_in_use(port):
        if _port_is_ntlab(port):
            if not _port_has_app_api(port):
                # 已有实例是旧版本：附着过去只会看到旧页面与 404，明确提示先重启
                webview.create_window(
                    "NTLab — 旧版本正在运行",
                    html=f"<body style='background:#141414;color:#ddd;font-family:sans-serif;"
                         f"padding:40px'><h2>端口 {port} 上运行的是旧版本 NTLab</h2>"
                         f"<p>它没有调试设置等新接口。请先关闭旧实例（其终端 Ctrl+C），"
                         f"再启动客户端；或直接重启旧实例的终端进程。</p></body>",
                    width=560, height=300,
                )
                webview.start()
                return
            attached = True   # 已有新版本 NTLab 在跑：直接附着，不重复起服务
        else:
            webview.create_window(
                "NTLab — 端口被占用",
                html=f"<body style='background:#141414;color:#ddd;font-family:sans-serif;"
                     f"padding:40px'><h2>端口 {port} 已被其他程序占用</h2>"
                     f"<p>请在「调试」页更换端口，或排查占用该端口的进程。</p></body>",
                width=520, height=280,
            )
            webview.start()
            return
    else:
        runner.start()
        deadline = time.time() + 10
        while time.time() < deadline and not _port_in_use(runner.port):
            time.sleep(0.1)

    os.environ["NTLAB_PORT"] = str(runner.port)
    url = f"http://127.0.0.1:{runner.port}/"
    title = "NTLab — Windows 系统底层实验室"
    webview.create_window(
        title, url,
        js_api=ClientApi(runner),
        width=1360, height=860, min_size=(980, 640),
        background_color="#141414",
    )
    webview.start(gui="edgechromium")
    # 窗口关闭：附着模式不动别人的服务；自起模式随 daemon 线程退出
    if not attached and runner._server is not None:
        runner.stop()


if __name__ == "__main__":
    main()
