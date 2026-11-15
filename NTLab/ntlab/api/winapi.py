"""WinAPI playground endpoints. The whitelist lives in core/winapi.py; this
layer only validates JSON shapes and forwards."""

from __future__ import annotations

from typing import Annotated

from fastapi import APIRouter, Query

from ntlab.core import winapi
from ntlab.core.errors import NtlabError

router = APIRouter()


@router.get("/catalog")
def catalog() -> dict:
    """Whitelisted callable APIs with parameter schemas (for UI call cards)."""
    return winapi.catalog()


@router.post("/run")
def run(body: dict) -> dict:
    """Execute a call queue: body {"calls": [{"api": ..., "args": {...}}, ...]}."""
    calls = body.get("calls")
    if calls is None and body.get("api"):
        calls = [{"api": body["api"], "args": body.get("args") or {}}]
    if not isinstance(calls, list):
        raise NtlabError("bad_queue", 'body must contain "calls": [{"api", "args"}, ...]')
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
