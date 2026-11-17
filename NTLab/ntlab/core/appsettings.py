"""应用级设置（客户端/服务器共用的持久化配置）。

存放于 data/settings.json（随仓库忽略，与 user.db 同目录约定）。
字段：
  port            服务器端口（默认 8765；CLI --port 优先于该值）
  color_terminal  终端彩色输出（uvicorn 日志与横幅颜色）
  no_console      程序终端开关：true=默认无黑框（EXE 窗口版 / pythonw.exe），
                  false=带终端（python.exe；冻结版则现场 AllocConsole）
  open_browser    以纯服务器方式启动时是否自动开浏览器（客户端模式忽略）
"""

from __future__ import annotations

import json
import os
import tempfile
from pathlib import Path
from threading import Lock

_PATH = Path(__file__).resolve().parents[2] / "data" / "settings.json"

_DEFAULTS: dict = {
    "port": 8765,
    "color_terminal": True,
    "no_console": True,   # 默认无终端（EXE 不出黑框；开发模式以 python.exe 启动时不受此限）
    "open_browser": True,
    "mirror_terminal": True,
}

_TYPES = {
    "port": int,
    "color_terminal": bool,
    "no_console": bool,
    "open_browser": bool,
    "mirror_terminal": bool,
}

_LOCK = Lock()


class SettingsError(ValueError):
    """字段校验失败。"""


def _validate(data: dict) -> dict:
    out = {}
    for key, typ in _TYPES.items():
        v = data.get(key, _DEFAULTS[key])
        if typ is int:
            if isinstance(v, bool) or not isinstance(v, int):
                raise SettingsError(f"port 必须是整数，得到 {v!r}")
            if not (1 <= v <= 65535):
                raise SettingsError(f"端口超出范围 1-65535：{v}")
            out[key] = int(v)
        else:
            out[key] = bool(v)
    return out


def load() -> dict:
    """读取设置；文件缺失/损坏时回退默认值（坏文件重命名为 .bak 保留现场）。"""
    with _LOCK:
        return _load_unlocked()


def _load_unlocked() -> dict:
    if not _PATH.is_file():
        return dict(_DEFAULTS)
    try:
        raw = json.loads(_PATH.read_text("utf-8-sig"))   # utf-8-sig 容忍 BOM
        return _validate(raw if isinstance(raw, dict) else {})
    except (json.JSONDecodeError, SettingsError, OSError):
        try:
            _PATH.replace(_PATH.with_suffix(".json.bak"))
        except OSError:
            pass
        return dict(_DEFAULTS)


def save(data: dict) -> dict:
    """校验并原子写入（未提供的键保留现值）；返回保存后的完整设置。"""
    with _LOCK:
        merged = {**_load_unlocked(), **dict(data)}
        clean = _validate(merged)
        _PATH.parent.mkdir(parents=True, exist_ok=True)
        tmp_fd, tmp_name = tempfile.mkstemp(dir=str(_PATH.parent), suffix=".tmp")
        try:
            with os.fdopen(tmp_fd, "w", encoding="utf-8") as f:
                json.dump(clean, f, ensure_ascii=False, indent=2)
            os.replace(tmp_name, _PATH)   # Windows 上 os.replace 原子覆盖
        except BaseException:
            try:
                os.remove(tmp_name)
            except OSError:
                pass
            raise
        return dict(clean)


def current_port(cli_port: int | None = None) -> int:
    """生效端口：CLI 参数 > 设置文件 > 默认。"""
    if cli_port is not None:
        return int(cli_port)
    return int(load()["port"])
