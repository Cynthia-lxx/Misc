"""OI助手 服务器设置（端口 / 自动打开浏览器）——NTLab appsettings 同款方法。

独立于 CLI 的 ~/.oicoder_config.json：那一份是「API 与生成参数」且与 OI.py 互通，
这里只存 Web 服务器自身的启动行为，避免两套工具互相踩踏。
存放在用户主目录（与 ~/.oicoder_config.json、~/.oicoder_history.json 同级）。

JSON schema / 默认值：
    {"port": 8765, "open_browser": True}

端口优先级：CLI --port > 设置文件 > 默认值。
写入：tempfile 同目录落盘 + os.replace 原子替换，加锁防并发。
"""

from __future__ import annotations

import json
import os
import tempfile
from pathlib import Path
from threading import Lock

_LOCK = Lock()
_PATH = Path.home() / ".oicoder_web_settings.json"
_CACHE: dict | None = None

_DEFAULTS = {"port": 8765, "open_browser": True}


def _validate(data: dict) -> dict:
    """防御性清洗：未知键丢弃，类型不符回退默认。"""
    out = dict(_DEFAULTS)
    if not isinstance(data, dict):
        return out
    port = data.get("port")
    if isinstance(port, int) and 1 <= port <= 65535:
        out["port"] = int(port)
    open_browser = data.get("open_browser")
    if isinstance(open_browser, bool):
        out["open_browser"] = bool(open_browser)
    return out


def load() -> dict:
    """读设置（带进程内缓存）；文件缺失/损坏一律回退默认值。"""
    global _CACHE
    with _LOCK:
        if _CACHE is not None:
            return dict(_CACHE)
        try:
            _CACHE = _validate(json.loads(_PATH.read_text(encoding="utf-8")))
        except FileNotFoundError:
            _CACHE = dict(_DEFAULTS)
        except (ValueError, OSError):
            _CACHE = dict(_DEFAULTS)   # JSON 损坏/读失败：回退默认，不打断启动
        return dict(_CACHE)


def save(data: dict) -> dict:
    """写设置（原子替换 + 校验）。返回清洗后的最终值。"""
    global _CACHE
    clean = _validate(data)
    with _LOCK:
        fd, tmp = tempfile.mkstemp(dir=str(_PATH.parent), prefix=".web_settings.", suffix=".tmp")
        try:
            with os.fdopen(fd, "w", encoding="utf-8") as f:
                json.dump(clean, f, ensure_ascii=False, indent=2)
                f.flush()
                os.fsync(f.fileno())
            os.replace(tmp, _PATH)   # 同目录原子替换，崩溃也不会留半截 JSON
        except BaseException:
            try:
                os.unlink(tmp)
            except OSError:
                pass
            raise
        _CACHE = dict(clean)
        return dict(clean)
