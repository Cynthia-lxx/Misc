"""上传端点回归：同名文件重复上传不得因解析缓存 mmap 锁而失败（2026-11-17 修复，原为 500）。"""

from __future__ import annotations

import asyncio
import io
import os

import pytest
from starlette.datastructures import UploadFile

from ntlab.api import pe as pe_api
from ntlab.core.pe import parser

NOTEPAD = os.path.join(os.environ.get("WINDIR", r"C:\Windows"), "System32", "notepad.exe")

requires_windows_binaries = pytest.mark.skipif(
    not os.path.isfile(NOTEPAD), reason="Windows system binary unavailable"
)


async def _put(data: bytes, name: str) -> dict:
    up = UploadFile(filename=name, file=io.BytesIO(data))
    return await pe_api.upload(up)


@requires_windows_binaries
def test_upload_twice_same_name_with_cached_parse(tmp_path, monkeypatch):
    """首次上传→解析（缓存持有 mmap）→同名再上传：唯一文件名路径必须两次成功。"""
    monkeypatch.setattr(pe_api, "_UPLOAD_DIR", str(tmp_path))
    with open(NOTEPAD, "rb") as f:
        data = f.read()

    r1 = asyncio.run(_put(data, "mod.exe"))
    assert os.path.isfile(r1["path"])

    # 触发解析缓存：pefile.PE 对 r1 路径持有 mmap（旧代码下第二次同名上传会 errno 22）
    parser._load(r1["path"])

    r2 = asyncio.run(_put(data, "mod.exe"))
    assert r2["path"] != r1["path"], "每次上传应落到唯一文件名"
    assert os.path.isfile(r2["path"])
    assert r2["name"] == "mod.exe"


@requires_windows_binaries
def test_upload_rejects_bad_ext(tmp_path, monkeypatch):
    from ntlab.core.errors import InvalidPathError

    monkeypatch.setattr(pe_api, "_UPLOAD_DIR", str(tmp_path))
    with pytest.raises(InvalidPathError):
        asyncio.run(_put(b"MZ...", "evil.txt"))
