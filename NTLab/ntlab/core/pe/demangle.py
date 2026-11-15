"""MSVC C++ name undecoration via dbghelp.UnDecorateSymbolName (IDA "Names"
window flavor). Pure ctypes, read-only; non-mangled names return None.
"""

from __future__ import annotations

import ctypes
from ctypes import wintypes

_dbghelp = ctypes.WinDLL("dbghelp")

UNDNAME_COMPLETE = 0x0000

_dbghelp.UnDecorateSymbolName.argtypes = [
    wintypes.LPCSTR,   # DecoratedName
    wintypes.LPSTR,    # UnDecoratedName
    wintypes.DWORD,    # MaximumLength
    wintypes.DWORD,    # Flags
]
_dbghelp.UnDecorateSymbolName.restype = wintypes.DWORD  # chars written or 0

_BUF = 1024


def demangle(name: str | None) -> str | None:
    """Return the undecorated form of an MSVC-mangled name, else None."""
    if not name or not name.startswith(("?", ".?")):
        return None
    out = ctypes.create_string_buffer(_BUF)
    try:
        n = _dbghelp.UnDecorateSymbolName(
            name.encode("utf-8"), out, _BUF, UNDNAME_COMPLETE)
    except Exception:
        return None
    if not n:
        return None
    try:
        return out.value[:n].decode("utf-8", "replace")
    except Exception:
        return None
