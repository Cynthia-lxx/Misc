"""跨打包方式统一的根目录与冻结判定。

支持三种运行形态：
  开发模式      项目根（ntlab/core/.. 的上两级）
  PyInstaller   sys.frozen=True，exe 旁边
  fspack        sys.frozen 不设置；dist 根有 _entry_ntlab.py 标记文件，
                sys.executable 为 dist\\ntlab.exe（C loader）或
                dist\\runtime\\python.exe（直接调试入口脚本）
"""

from __future__ import annotations

import sys
from pathlib import Path

_ENTRY_MARKER = "_entry_ntlab.py"   # fspack 生成的入口 wrapper，dist 根唯一


def fspack_dist_root() -> Path | None:
    """fspack 产物根目录（含 _entry_ntlab.py 的目录）；非 fspack 运行返回 None。

    sys.executable 两种形态各向上探测一层：loader exe 本身在 dist 根；
    runtime 解释器在其下一级 runtime/ 目录。
    """
    exe_dir = Path(sys.executable).resolve().parent
    for base in (exe_dir, exe_dir.parent):
        if (base / _ENTRY_MARKER).is_file():
            return base
    return None


def is_frozen() -> bool:
    """是否运行于打包产物（PyInstaller 或 fspack）。"""
    return bool(getattr(sys, "frozen", False)) or fspack_dist_root() is not None


def base_dir() -> Path:
    """数据文件（data/）所在根目录：exe/dist 旁（打包）或项目根（开发）。"""
    if getattr(sys, "frozen", False):
        return Path(sys.executable).resolve().parent
    dist = fspack_dist_root()
    if dist is not None:
        return dist
    return Path(__file__).resolve().parents[2]
