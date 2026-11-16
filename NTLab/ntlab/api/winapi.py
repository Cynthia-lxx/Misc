"""WinAPI playground endpoints. The whitelist lives in core/winapi.py; this
layer only validates JSON shapes and forwards."""

from __future__ import annotations

from typing import Annotated

from fastapi import APIRouter, Query, Request

from ntlab.core import winapi, winapi_library
from ntlab.core.errors import NtlabError

router = APIRouter()


@router.get("/catalog")
def catalog() -> dict:
    """Whitelisted callable APIs with parameter schemas (for UI call cards)."""
    return winapi.catalog()


@router.get("/library")
def library(request: Request) -> dict:
    """PE 导出库状态；`since` 为客户端已收到的模块数，返回其后的增量清单。

    首次请求自动启动后台扫描；扫描进行中随到随回（前端轮询增量拉取）。
    """
    try:
        since = max(0, int(request.query_params.get("since", "0")))
    except ValueError:
        since = 0
    st = winapi_library.status(since=since)
    if st["status"] == "idle":
        return winapi_library.start_scan()
    return st


@router.post("/library/scan")
def library_scan(body: dict | None = None) -> dict:
    """手动触发（可选 force）重新扫描系统 DLL/EXE 导出表。"""
    return winapi_library.start_scan(force=bool((body or {}).get("force")))


@router.get("/library/module")
def library_module(
    request: Request,
    name: Annotated[str, Query(min_length=1)],
) -> dict:
    """单个 DLL 的导出函数列表（分页：offset/limit，limit=0 表示全部）。"""
    qp = request.query_params
    try:
        offset = max(0, int(qp.get("offset", "0")))
        limit = max(0, int(qp.get("limit", "0")))
    except ValueError:
        offset, limit = 0, 0
    return winapi_library.module_exports(name, offset=offset, limit=limit)


@router.post("/run")
def run(body: dict) -> dict:
    """Execute a program: body {"program": [call/repeat/if/break/continue blocks]}.

    Legacy flat queues {"calls": [{"api", "args"}, ...]} still accepted.
    """
    prog = body.get("program")
    if prog is not None:
        if not isinstance(prog, list):
            raise NtlabError("bad_queue", '"program" must be a list of blocks')
        return winapi.run_program(prog)
    calls = body.get("calls")
    if calls is None and body.get("api"):
        calls = [{"api": body["api"], "args": body.get("args") or {}}]
    if not isinstance(calls, list):
        raise NtlabError("bad_queue", 'body must contain "program" or "calls"')
    return winapi.run_queue(calls)


@router.get("/sessions")
def sessions() -> dict:
    """Recent queue executions (most recent first, in-memory only)."""
    return {"sessions": winapi.sessions()}


@router.post("/asm")
def asm(body: dict) -> dict:
    """Assemble x86-64 text to bytes (keystone) + capstone round-trip. Never executed."""
    text = body.get("text") or ""
    return winapi.assemble(str(text))
