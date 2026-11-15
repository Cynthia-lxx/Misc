"""IDA-flavored disassembly: linear x86-64 disasm via capstone (BSD) plus a
lightweight cross-reference map (call/jmp targets within executable sections).

Read-only; results cached per (path, mtime, size).
"""

from __future__ import annotations

import capstone

from ntlab.core.errors import InvalidPathError
from .parser import get_pe, section_of_rva

_md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
_md.detail = False

_MAX_IMAGE = 64 * 1024 * 1024
_XREF_CACHE: dict[tuple[str, int, int], dict[int, list[int]]] = {}


def _image(pe) -> bytes:
    """Section-aligned memory image (RVA-addressable)."""
    return pe.get_memory_mapped_image()


def disasm(path: str, rva: int = 0, count: int = 200) -> dict:
    pe = get_pe(path)
    img = _image(pe)
    if not (0 <= rva < len(img)):
        raise InvalidPathError(f"RVA 0x{rva:X} outside image (size 0x{len(img):X})")
    count = max(1, min(count, 1000))
    items = []
    pos = rva
    end = min(rva + count * 16 + 64, len(img))
    while pos < end and len(items) < count:
        ins = next(_md.disasm(img[pos: pos + 16], pos), None)
        if ins is not None:
            items.append({
                "rva": ins.address,
                "size": ins.size,
                "bytes": ins.bytes.hex(" "),
                "mnemonic": ins.mnemonic,
                "op": ins.op_str,
                "section": section_of_rva(pe, ins.address),
            })
            pos += ins.size
        else:
            # 无效字节（数据/对齐填充）：标记 db 并跳 1 字节继续线性扫描
            items.append({
                "rva": pos, "size": 1,
                "bytes": f"{img[pos]:02X}",
                "mnemonic": "db", "op": f"0x{img[pos]:02X}",
                "section": section_of_rva(pe, pos),
            })
            pos += 1
    next_rva = items[-1]["rva"] + items[-1]["size"] if items else rva
    return {
        "start_rva": rva,
        "image_base": pe.OPTIONAL_HEADER.ImageBase,
        "image_size": len(img),
        "items": items,
        "next_rva": next_rva,
    }


def _build_xrefs(pe) -> dict[int, list[int]]:
    """Linear-sweep all executable sections; collect call/jmp absolute targets."""
    img = _image(pe)
    targets: dict[int, list[int]] = {}
    for s in pe.sections:
        if not (s.Characteristics & 0x20000000):  # IMAGE_SCN_MEM_EXECUTE
            continue
        va = s.VirtualAddress
        size = min(s.Misc_VirtualSize, s.SizeOfRawData)
        if size <= 0 or va + size > len(img):
            continue
        # cap per-section work (pathological sections)
        size = min(size, 8 * 1024 * 1024)
        for ins in _md.disasm(img[va: va + size], va):
            if ins.mnemonic in ("call", "jmp") and ins.op_str.startswith("0x"):
                try:
                    tgt = int(ins.op_str, 16)
                except ValueError:
                    continue
                if 0 <= tgt < len(img):
                    targets.setdefault(tgt, []).append(ins.address)
    return targets


def xrefs_to(path: str, rva: int, limit: int = 200) -> dict:
    pe = get_pe(path)
    img_len = len(pe.__data__)
    st = (path.lower(), pe.FILE_HEADER.TimeDateStamp, img_len)
    xmap = _XREF_CACHE.get(st)
    if xmap is None:
        xmap = _build_xrefs(pe)
        _XREF_CACHE[st] = xmap
        while len(_XREF_CACHE) > 4:
            _XREF_CACHE.pop(next(iter(_XREF_CACHE)))
    base = pe.OPTIONAL_HEADER.ImageBase
    callers = xmap.get(base + rva, [])
    items = [{"rva": c - base, "va": f"0x{c:X}"} for c in callers[:limit]]
    return {"rva": rva, "total_refs": len(callers), "callers": items,
            "section": section_of_rva(pe, rva)}
