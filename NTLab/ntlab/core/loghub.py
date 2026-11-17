"""日志中枢：把服务端日志以 ANSI 彩色行广播给「调试」页（SSE）。

- HubHandler 挂进 uvicorn 的 logging 配置，按 uvicorn 自带的彩色格式渲染
  （DefaultFormatter/AccessFormatter，use_colors=True → 行内含 ANSI 转义序列）；
- 环形缓冲保留最近 N 行（页面打开时先回放），订阅者由 SSE 轮询拉取增量；
- 终端控制台的颜色由 uvicorn use_colors 设置控制，与页面镜像互不影响
  （页面始终按 ANSI 渲染，无 ANSI 时按纯文本显示）。
"""

from __future__ import annotations

import logging
import threading
import time

import uvicorn.logging
import uvicorn.config


class LogHub:
    """环形缓冲 + 行序号 + SSE 订阅游标。线程安全。"""

    def __init__(self, maxlen: int = 1000) -> None:
        self._lock = threading.Lock()
        self._lines: list[tuple[int, float, str]] = []   # (seq, ts, text)
        self._seq = 0
        self._maxlen = maxlen
        self._waiters: list[tuple[int, threading.Event]] = []  # (seq, event) 等新行

    def emit(self, text: str) -> None:
        with self._lock:
            self._seq += 1
            self._lines.append((self._seq, time.time(), text))
            if len(self._lines) > self._maxlen:
                self._lines = self._lines[-self._maxlen:]
            for wait_seq, ev in self._waiters:
                if wait_seq < self._seq:
                    ev.set()
            self._waiters = [w for w in self._waiters if not w[1].is_set()]

    def after(self, seq: int) -> list[tuple[int, float, str]]:
        """返回 seq 之后的所有行（不阻塞，供 SSE 轮询）。"""
        with self._lock:
            return [x for x in self._lines if x[0] > seq]

    def wait(self, seq: int, timeout: float) -> None:
        """阻塞到有 seq 之后的新行或超时。"""
        ev = threading.Event()
        with self._lock:
            if self._seq > seq:
                return
            self._waiters.append((seq, ev))
        ev.wait(timeout)


hub = LogHub()


class HubHandler(logging.Handler):
    """logging → ANSI 彩色行 → LogHub（uvicorn dictConfig 直接引用本类）。"""

    def __init__(self, level: int | str = logging.NOTSET) -> None:
        super().__init__(level)
        self._fmt_default = uvicorn.logging.DefaultFormatter(
            fmt="%(levelprefix)s %(message)s", use_colors=True)
        self._fmt_access = uvicorn.logging.AccessFormatter(
            fmt=uvicorn.config.LOGGING_CONFIG["formatters"]["access"]["fmt"],
            use_colors=True,
        )

    def emit(self, record: logging.LogRecord) -> None:
        try:
            fmt = self._fmt_access if record.name == "uvicorn.access" else self._fmt_default
            hub.emit(fmt.format(record).rstrip("\n"))
        except Exception:   # 日志镜像永不影响主流程
            pass


def build_log_config() -> dict:
    """uvicorn LOGGING_CONFIG + ntlab_hub 处理器（进 uvicorn/error/access 三路）。"""
    import copy

    cfg = copy.deepcopy(uvicorn.config.LOGGING_CONFIG)
    cfg["handlers"]["ntlab_hub"] = {
        "class": "ntlab.core.loghub.HubHandler",
        "level": "INFO",
    }
    # 只挂 uvicorn（error 记录会传播上来，重复挂会双行）与 uvicorn.access
    for name in ("uvicorn", "uvicorn.access"):
        if name in cfg["loggers"]:
            cfg["loggers"][name].setdefault("handlers", []).append("ntlab_hub")
    return cfg
