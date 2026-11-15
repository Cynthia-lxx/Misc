"""IDA-flavored binary views: Strings window / Hex View / binary search.

All read-only. File bytes come from the cached pefile.PE.__data__ buffer.
"""

from __future__ import annotations

import re
import string
from binascii import unhexlify
from typing import Any

from .parser import get_pe, offset_of_rva, rva_of_offset, section_of_rva

_MIN_PRINTABLE = 5

# printable ASCII (no \t\r\n to keep strings clean)
_ASCII_RUN = re.compile(rb"[\x20-\x7E]{%d,}" % _MIN_PRINTABLE)
_UTF16LE_RUN = re.compile(rb"(?:[\x20-\x7E]\x00){%d,}" % _MIN_PRINTABLE)


def _locate(pe, off: int) -> dict[str, Any]:
    """File offset → {rva, section}."""
    for s in pe.sections:
        if s.PointerToRawData <= off < s.PointerToRawData + s.SizeOfRawData:
            return {
                "rva": s.VirtualAddress + (off - s.PointerToRawData),
                "section": s.Name.rstrip(b"\x00").decode("utf-8", "replace"),
            }
    return {"rva": None, "section": "header"}


def strings(path: str, min_len: int = 5, offset: int = 0, limit: int = 500) -> dict[str, Any]:
    """IDA Strings window: printable ASCII + UTF-16LE runs, paged."""
    pe = get_pe(path)
    data = pe.__data__
    items: list[dict[str, Any]] = []

    for m in _ASCII_RUN.finditer(data):
        items.append({"file_offset": m.start(), "size": len(m.group()),
                      "text": m.group().decode("ascii"), "encoding": "ascii"})
    for m in _UTF16LE_RUN.finditer(data):
        items.append({"file_offset": m.start(), "size": len(m.group()),
                      "text": m.group().decode("utf-16-le"), "encoding": "utf-16le"})

    items.sort(key=lambda x: x["file_offset"])
    for it in items:
        it.update(_locate(pe, it["file_offset"]))

    return {"total": len(items), "offset": offset, "limit": limit,
            "items": items[offset:offset + limit],
            "file_size": len(data)}


def hexdump(path: str, rva: int | None = None, file_offset: int | None = None,
            size: int = 256) -> dict[str, Any]:
    """IDA Hex View: 16-byte rows around an RVA (or raw file offset)."""
    pe = get_pe(path)
    size = max(16, min(size, 4096))
    if rva is None and file_offset is None:
        from ntlab.core.errors import InvalidPathError
        raise InvalidPathError("need 'rva' or 'off'")
    if rva is not None:
        off = offset_of_rva(pe, rva)
    else:
        off = max(0, min(file_offset, len(pe.__data__) - 1))
        rva = rva_of_offset(pe, off)

    # align down to 16 for clean rows
    row0 = off - (off % 16)
    base_rva = rva - (off - row0)          # RVA of the first displayed row
    blob = pe.__data__[row0: row0 + size]
    printable = set(string.printable[:95])
    rows = []
    for i in range(0, len(blob), 16):
        chunk = blob[i:i + 16]
        rows.append({
            "rva": base_rva + i,
            "file_offset": row0 + i,
            "hex": chunk.hex(" "),
            "ascii": "".join(chr(b) if b in printable else "." for b in chunk),
        })
    return {
        "rva": rva,
        "file_offset": off,
        "size": size,
        "section": section_of_rva(pe, rva),
        "image_base": pe.OPTIONAL_HEADER.ImageBase,
        "rows": rows,
        "file_size": len(pe.__data__),
    }


def search(path: str, q: str, mode: str = "text", start: int = 0, limit: int = 100) -> dict[str, Any]:
    """IDA Search: text (ASCII) or hex byte-pattern, returns matches after start."""
    pe = get_pe(path)
    data = pe.__data__
    if mode == "hex":
        hexstr = re.sub(r"\s+", "", q)
        try:
            needle = unhexlify(hexstr)
        except Exception as exc:
            from ntlab.core.errors import InvalidPathError
            raise InvalidPathError(f"bad hex pattern: {exc}") from exc
    else:
        needle = q.encode("latin-1", "replace")
    if not needle:
        from ntlab.core.errors import InvalidPathError
        raise InvalidPathError("empty search pattern")

    matches = []
    pos = max(0, min(start, len(data) - 1))
    while len(matches) < limit:
        pos = data.find(needle, pos)
        if pos < 0:
            break
        loc = _locate(pe, pos)
        matches.append({
            "file_offset": pos,
            "rva": loc["rva"],
            "section": loc["section"],
            "preview": data[pos: pos + min(64, len(needle) * 2)].decode("latin-1", "replace"),
        })
        pos += 1  # allow overlapping matches
    return {"total": len(matches), "mode": mode, "pattern": q,
            "matches": matches, "file_size": len(data)}
