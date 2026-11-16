"""PE parsing core (pefile wrapper). Pure Python, no FastAPI dependency."""

from .parser import (
    exports,
    imports,
    parse_summary,
    parse_headers,
    resources,
    signature,
)
from .resedit import extract, pack_zip, replace_resources

__all__ = [
    "exports",
    "imports",
    "parse_summary",
    "parse_headers",
    "resources",
    "signature",
    "extract",
    "pack_zip",
    "replace_resources",
]
