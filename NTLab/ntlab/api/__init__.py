"""NTLab API package — FastAPI app factory and routers (thin layer only)."""

from __future__ import annotations

from pathlib import Path

from fastapi import FastAPI, Request
from fastapi.responses import JSONResponse
from fastapi.staticfiles import StaticFiles

from ntlab.core.errors import NtlabError
from ntlab.core.errors import NotFoundError

from . import ntobj
from . import pe
from . import system

WEB_DIR = Path(__file__).resolve().parent.parent / "web"


def create_app() -> FastAPI:
    app = FastAPI(
        title="NTLab",
        version="0.1.0",
        description="Windows system internals play/test/research toolkit (read-only)",
    )

    app.include_router(system.router, prefix="/api/system", tags=["system"])
    app.include_router(pe.router, prefix="/api/pe", tags=["pe"])
    app.include_router(ntobj.router, prefix="/api/ntobj", tags=["ntobj"])

    @app.exception_handler(NtlabError)
    async def ntlab_error_handler(_: Request, exc: NtlabError) -> JSONResponse:
        status = 404 if isinstance(exc, NotFoundError) else 400
        return JSONResponse(status_code=status, content={"error": exc.to_dict()})

    @app.middleware("http")
    async def cache_headers(request: Request, call_next):
        response = await call_next(request)
        # 本地单用户工具：一律 no-store。曾因无缓存头的旧条目被 Chrome
        # 启发式缓存长期复用（不回源 revalidate），导致改版后浏览器吃旧
        # CSS/JS 排查动效时出现「明明改了却没生效」的假象。
        response.headers["Cache-Control"] = "no-store"
        return response

    # SPA: index at root, everything else static
    app.mount("/", StaticFiles(directory=WEB_DIR, html=True), name="web")

    return app
