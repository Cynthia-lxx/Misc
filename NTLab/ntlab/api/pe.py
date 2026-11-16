"""PE Viewer endpoints. Read-only; user submits a single file path (typed or uploaded)."""

from __future__ import annotations

import base64
import binascii
import os
import tempfile
import urllib.parse
from typing import Annotated

from fastapi import APIRouter, Query, Response, UploadFile

from ntlab.core import pe as pe_core
from ntlab.core import userdb
from ntlab.core.pe import demangle as dm_core
from ntlab.core.pe import disasm
from ntlab.core.pe import hexdump
from ntlab.core.errors import InvalidPathError

router = APIRouter()

_UPLOAD_DIR = os.path.join(tempfile.gettempdir(), "ntlab_uploads")
_ALLOWED_EXT = {".exe", ".dll", ".sys", ".ocx", ".scr", ".cpl", ".efi", ".mui", ".drv"}


def _require_path(path: str | None) -> str:
    if not path or not path.strip():
        raise InvalidPathError("missing 'path' query/body parameter")
    return path.strip()


class ParseBody:
    def __init__(self, path: str) -> None:
        self.path = path


@router.post("/parse")
def parse(body: dict) -> dict:
    """Parse a PE file by path (summary: headers, sections, directories, checksum)."""
    return pe_core.parse_summary(_require_path(body.get("path")))


@router.post("/headers")
def headers(body: dict) -> dict:
    return pe_core.parse_headers(_require_path(body.get("path")))


@router.get("/imports")
def imports(
    path: str,
    offset: Annotated[int, Query(ge=0)] = 0,
    limit: Annotated[int, Query(ge=1, le=2000)] = 200,
) -> dict:
    return pe_core.imports(_require_path(path), offset, limit)


@router.get("/exports")
def exports(
    path: str,
    offset: Annotated[int, Query(ge=0)] = 0,
    limit: Annotated[int, Query(ge=1, le=2000)] = 200,
) -> dict:
    return pe_core.exports(_require_path(path), offset, limit)


@router.get("/resources")
def resources(path: str) -> dict:
    return pe_core.resources(_require_path(path))


def _download_response(data: bytes, filename: str, media_type: str) -> Response:
    quoted = urllib.parse.quote(filename)
    return Response(
        content=data,
        media_type=media_type,
        headers={"Content-Disposition":
                 f"attachment; filename*=UTF-8''{quoted}"},
    )


@router.get("/resources/extract")
def resource_extract(
    path: str,
    type: str,
    name: str,
    lang: int = 0,
) -> Response:
    """提取单个资源数据，直接作为附件下载。"""
    data = pe_core.extract(_require_path(path), type, name, lang)
    stem = os.path.splitext(os.path.basename(path))[0]
    safe = f"{type}_{name}_lang{lang}.bin".replace("/", "_").replace("\\", "_")
    return _download_response(data, f"{stem}.{safe}", "application/octet-stream")


@router.get("/resources/pack")
def resource_pack_download(path: str) -> Response:
    """全部资源打包为 zip，直接流式下载（不入库）。"""
    blob, count, _manifest = pe_core.pack_zip(_require_path(path))
    stem = os.path.splitext(os.path.basename(path))[0]
    return _download_response(blob, f"{stem}.resources.zip", "application/zip")


@router.post("/resources/pack-save")
def resource_pack_save(body: dict) -> dict:
    """全部资源打包为 zip 并存入用户数据库，返回归档 id。"""
    path = _require_path(body.get("path"))
    blob, count, manifest = pe_core.pack_zip(path)
    stem = os.path.splitext(os.path.basename(path))[0]
    aid = userdb.save_archive(f"{stem}.resources.zip", blob, count, manifest)
    return {"id": aid, "count": count, "size": len(blob),
            "file_name": f"{stem}.resources.zip"}


@router.get("/resources/archives")
def resource_archives() -> dict:
    return {"archives": userdb.list_archives()}


@router.get("/resources/archives/{aid}/download")
def resource_archive_download(aid: int) -> Response:
    arch = userdb.get_archive(aid)
    return _download_response(arch["content"], arch["file_name"], "application/zip")


@router.delete("/resources/archives/{aid}")
def resource_archive_delete(aid: int) -> dict:
    userdb.delete_archive(aid)
    return {"deleted": aid}


@router.post("/resources/replace")
def resource_replace(body: dict) -> dict:
    """替换若干资源并重建 .rsrc 节（原地保存自动留 .bak；默认另存为）。"""
    path = _require_path(body.get("path"))
    entries = body.get("entries") or []
    if not isinstance(entries, list) or not entries:
        raise InvalidPathError("body must contain a non-empty 'entries' array")
    reps = []
    for e in entries:
        try:
            data = base64.b64decode(e.get("data_b64") or "")
        except (binascii.Error, ValueError):
            raise InvalidPathError(
                f"invalid base64 in data_b64 for "
                f"({e.get('type')}, {e.get('name')}, lang={e.get('lang')})")
        reps.append({
            "type": e.get("type"), "name": e.get("name"),
            "lang": int(e.get("lang") or 0), "data": data,
            "codepage": e.get("codepage"),
        })
    in_place = bool(body.get("in_place"))
    out_path = body.get("out_path")
    if not in_place and not out_path:
        raise InvalidPathError(
            "either in_place=true or out_path is required")
    return pe_core.replace_resources(
        path, reps, out_path=out_path, in_place=in_place)


@router.get("/signature")
def signature(path: str) -> dict:
    return pe_core.signature(_require_path(path))


@router.get("/strings")
def strings(
    path: str,
    min_len: Annotated[int, Query(ge=4, le=64)] = 5,
    offset: Annotated[int, Query(ge=0)] = 0,
    limit: Annotated[int, Query(ge=1, le=2000)] = 500,
) -> dict:
    return hexdump.strings(_require_path(path), min_len, offset, limit)


@router.get("/hex")
def hex_view(
    path: str,
    rva: int | None = None,
    off: int | None = None,
    size: Annotated[int, Query(ge=16, le=4096)] = 256,
) -> dict:
    return hexdump.hexdump(_require_path(path), rva=rva, file_offset=off, size=size)


@router.get("/search")
def search(
    path: str,
    q: str,
    mode: str = "text",
    start: Annotated[int, Query(ge=0)] = 0,
    limit: Annotated[int, Query(ge=1, le=200)] = 100,
    case_sensitive: bool = False,
) -> dict:
    return hexdump.search(_require_path(path), q, mode, start, limit, case_sensitive=case_sensitive)


@router.get("/disasm")
def disasm_view(
    path: str,
    rva: Annotated[int, Query(ge=0)] = 0,
    count: Annotated[int, Query(ge=1, le=1000)] = 200,
) -> dict:
    return disasm.disasm(_require_path(path), rva, count)


@router.get("/xrefs")
def xrefs_to(
    path: str,
    rva: Annotated[int, Query(ge=0)] = 0,
    limit: Annotated[int, Query(ge=1, le=200)] = 200,
) -> dict:
    return disasm.xrefs_to(_require_path(path), rva, limit)


@router.get("/demangle")
def demangle(name: str) -> dict:
    return {"name": name, "demangled": dm_core.demangle(name)}


@router.get("/resolve")
def resolve(name: str) -> dict:
    """按模块名（如 KERNEL32.dll）在系统目录中解析实际路径。"""
    n = (name or "").strip().strip("\\/")
    if not n or "/" in n or "\\" in n or ".." in n:
        raise InvalidPathError(f"invalid module name: {name!r}")
    windir = os.environ.get("WINDIR", r"C:\Windows")
    for d in (os.path.join(windir, "System32"), os.path.join(windir, "SysWOW64")):
        cand = os.path.join(d, n)
        if os.path.isfile(cand):
            return {"name": n, "path": cand}
    return {"name": n, "path": None}


@router.post("/upload")
async def upload(file: UploadFile) -> dict:
    """Save an uploaded PE file to a temp dir and return its server-side path."""
    name = os.path.basename(file.filename or "uploaded.bin")
    ext = os.path.splitext(name)[1].lower()
    if ext not in _ALLOWED_EXT:
        raise InvalidPathError(
            f"not a PE-looking extension: {ext}",
            {"allowed": sorted(_ALLOWED_EXT)},
        )
    os.makedirs(_UPLOAD_DIR, exist_ok=True)
    dest = os.path.join(_UPLOAD_DIR, name)
    size = 0
    with open(dest, "wb") as out:
        while chunk := await file.read(1024 * 1024):
            size += len(chunk)
            if size > 64 * 1024 * 1024:
                out.close()
                os.remove(dest)
                raise InvalidPathError("upload exceeds 64 MB limit")
            out.write(chunk)
    return {"path": dest, "size": size, "name": name}
