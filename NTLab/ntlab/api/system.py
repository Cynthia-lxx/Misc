"""System info / health check endpoints."""

from __future__ import annotations

import platform
import sys

from fastapi import APIRouter

from ntlab import __version__

router = APIRouter()


@router.get("/info")
def system_info() -> dict:
    win = sys.getwindowsversion()
    return {
        "ntlab": __version__,
        "python": sys.version.split()[0],
        "platform": platform.platform(),
        "machine": platform.machine(),
        "node": platform.node(),
        "build": f"{win.major}.{win.minor}.{win.build}",
    }


@router.get("/health")
def health() -> dict:
    return {"ok": True}
