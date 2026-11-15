"""IDA-flavored disassembly: linear x86-64 disasm via capstone (BSD) plus a
lightweight cross-reference map (call/jmp targets within executable sections)
and IAT (rip-relative import) annotation.

Read-only; results cached per (path, mtime, size).
"""

from __future__ import annotations

import re

import capstone

from ntlab.core.errors import InvalidPathError
from .parser import _ensure_dirs, get_pe, section_of_rva

_md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_64)
_md.detail = False

_MAX_IMAGE = 64 * 1024 * 1024
_XREF_CACHE: dict[tuple[str, int, int], dict[int, list[int]]] = {}

_RIP_RE = re.compile(r"\[rip\s*([+-])\s*(0x[0-9a-fA-F]+)\]", re.IGNORECASE)


def _image(pe) -> bytes:
    """Section-aligned memory image (RVA-addressable)."""
    return pe.get_memory_mapped_image()


def _iat_map(pe) -> dict[int, str]:
    """IAT slot RVA → "dll!function"（含序号导入的 #ordinal 形式）。"""
    base = pe.OPTIONAL_HEADER.ImageBase
    m: dict[int, str] = {}
    _ensure_dirs(pe, "imports")
    try:
        entries = pe.DIRECTORY_ENTRY_IMPORT
    except AttributeError:
        return m
    for e in entries:
        dll = e.dll.decode("latin-1", "replace") if e.dll else "?"
        for imp in e.imports:
            if not imp.address:
                continue
            rva = imp.address - base
            if imp.name:
                name = imp.name.decode("latin-1", "replace")
            else:
                name = f"#{imp.ordinal}"
            m[rva] = f"{dll}!{name}"
    return m


def disasm(path: str, rva: int = 0, count: int = 200) -> dict:
    pe = get_pe(path)
    img = _image(pe)
    if not (0 <= rva < len(img)):
        raise InvalidPathError(f"RVA 0x{rva:X} outside image (size 0x{len(img):X})")
    count = max(1, min(count, 1000))
    iat = _iat_map(pe)
    items = []
    pos = rva
    end = min(rva + count * 16 + 64, len(img))
    while pos < end and len(items) < count:
        ins = next(_md.disasm(img[pos: pos + 16], pos), None)
        if ins is not None:
            item = {
                "rva": ins.address,
                "size": ins.size,
                "bytes": ins.bytes.hex(" "),
                "mnemonic": ins.mnemonic,
                "op": ins.op_str,
                "section": section_of_rva(pe, ins.address),
            }
            # rip-relative 目标命中 IAT → 附上 dll!func 注释（IDA 风味）
            m = _RIP_RE.search(ins.op_str)
            if m:
                tgt = ins.address + ins.size + int(m.group(2), 16) * (1 if m.group(1) == "+" else -1)
                if tgt in iat:
                    item["iat"] = iat[tgt]
                elif 0 <= tgt < len(img):
                    item["rip_target"] = tgt
            items.append(item)
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
