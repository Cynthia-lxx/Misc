"""NT object namespace endpoints. Read-only, lazy-load friendly."""

from __future__ import annotations

from typing import Annotated

from fastapi import APIRouter, Query

from ntlab.core import ntobj

router = APIRouter()


@router.get("/roots")
def roots() -> dict:
    return ntobj.well_known_roots()


@router.get("/list")
def list_dir(
    path: str,
) -> dict:
    return ntobj.list_directory(path)


@router.get("/object")
def object_detail(
    path: str,
    type: Annotated[str | None, Query(alias="type")] = None,
) -> dict:
    return ntobj.object_detail(path, type)
