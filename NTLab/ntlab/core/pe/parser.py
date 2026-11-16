"""pefile wrapper: headers / sections / imports / exports / resources / signature → plain dicts.

All functions are read-only. Parsed PE objects are cached per (path, mtime, size)
so a 20 MB ntoskrnl.exe is not re-parsed on every tab switch.
"""

from __future__ import annotations

import os
from pathlib import Path
from typing import Any

import pefile

from ntlab.core.errors import InvalidPathError, NotFoundError, ParseError
from .demangle import demangle as _demangle

# ---------------------------------------------------------------- constants

MACHINE_NAMES = {
    0x014C: "i386",
    0x8664: "x64 (AMD64)",
    0xAA64: "ARM64",
    0x01C4: "ARMNT",
    0x01C0: "ARM",
}

SUBSYSTEM_NAMES = {
    1: "Native",
    2: "Windows GUI",
    3: "Windows CUI (console)",
    5: "OS/2 CUI",
    7: "POSIX CUI",
    9: "Windows CE GUI",
    10: "EFI application",
    11: "EFI boot driver",
    12: "EFI runtime driver",
    13: "EFI ROM",
    14: "XBOX",
    16: "Windows boot application",
}

DIRECTORY_NAMES = [
    "Export", "Import", "Resource", "Exception", "Security",
    "BaseRelocation", "Debug", "Architecture", "GlobalPtr", "TLS",
    "LoadConfig", "BoundImport", "IAT", "DelayImport", "CLR", "Reserved",
]

SECTION_FLAGS = [
    (0x00000020, "CODE"),
    (0x00000040, "INITIALIZED_DATA"),
    (0x00000080, "UNINITIALIZED_DATA"),
    (0x02000000, "DISCARDABLE"),
    (0x04000000, "NOT_CACHED"),
    (0x08000000, "NOT_PAGED"),
    (0x10000000, "SHARED"),
    (0x20000000, "EXECUTE"),
    (0x40000000, "READ"),
    (0x80000000, "WRITE"),
]

RESOURCE_TYPE_NAMES = {
    1: "CURSOR", 2: "BITMAP", 3: "ICON", 4: "MENU", 5: "DIALOG",
    6: "STRING", 7: "FONTDIR", 8: "FONT", 9: "ACCELERATOR", 10: "RCDATA",
    11: "MESSAGETABLE", 12: "GROUP_CURSOR", 14: "GROUP_ICON", 16: "VERSION",
    17: "DLGINCLUDE", 19: "PLUGPLAY", 20: "VXD", 21: "ANICURSOR",
    22: "ANIICON", 23: "HTML", 24: "MANIFEST",
}

IMAGE_DEBUG_TYPE_CODEVIEW = 2
IMAGE_DEBUG_TYPE_POGO = 3
IMAGE_DEBUG_TYPE_REPRO = 16

# ---------------------------------------------------------------- cache

_CACHE: dict[tuple[str, int, int], pefile.PE] = {}
_CACHE_MAX = 8


def _load(path: str, fast: bool = True) -> pefile.PE:
    p = Path(path)
    if not p.is_absolute():
        p = Path.cwd() / p
    p = p.resolve()
    if not p.exists():
        raise NotFoundError(f"file not found: {p}")
    if not p.is_file():
        raise InvalidPathError(f"not a regular file: {p}")
    st = p.stat()
    key = (str(p).lower(), st.st_mtime_ns, st.st_size)
    pe = _CACHE.get(key)
    if pe is not None:
        return pe
    try:
        pe = pefile.PE(str(p), fast_load=fast)
    except pefile.PEFormatError as exc:
        raise ParseError(f"not a valid PE file: {exc}") from exc
    _CACHE[key] = pe
    while len(_CACHE) > _CACHE_MAX:
        _CACHE.pop(next(iter(_CACHE)))
    return pe


# public aliases / helpers for sibling modules (hexdump, disasm, ...)

def get_pe(path: str, fast: bool = True) -> pefile.PE:
    """Cached PE object for the given file path."""
    return _load(path, fast=fast)


def section_of_rva(pe: pefile.PE, rva: int) -> str | None:
    for s in pe.sections:
        if s.VirtualAddress <= rva < s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData):
            return s.Name.rstrip(b"\x00").decode("utf-8", "replace")
    return None


def offset_of_rva(pe: pefile.PE, rva: int) -> int:
    try:
        return pe.get_offset_from_rva(rva)
    except Exception as exc:  # pefile raises on unmapped RVA
        raise InvalidPathError(f"RVA 0x{rva:X} not mapped to a file offset") from exc


def rva_of_offset(pe: pefile.PE, off: int) -> int:
    for s in pe.sections:
        if s.PointerToRawData <= off < s.PointerToRawData + s.SizeOfRawData:
            return s.VirtualAddress + (off - s.PointerToRawData)
    return off  # header region: identity mapping (best effort)


_ATTR_GROUPS = {
    "imports": ("DIRECTORY_ENTRY_IMPORT", "DIRECTORY_ENTRY_DELAY_IMPORT"),
    "exports": ("DIRECTORY_ENTRY_EXPORT",),
    "resources": ("DIRECTORY_ENTRY_RESOURCE",),
    "debug": ("DIRECTORY_ENTRY_DEBUG",),
}


def _ensure_dirs(pe: pefile.PE, *dirs: str) -> None:
    """Lazily parse data directories on a fast-loaded PE.

    dirs are short names used at call sites ("imports"/"exports"/...).
    """
    needed: list[str] = []
    for d in dirs:
        for attr in _ATTR_GROUPS.get(d, ()):
            if not hasattr(pe, attr):
                needed.append(attr)
    if needed:
        flags = [_DIR_FLAG_BY_ATTR[a] for a in needed if a in _DIR_FLAG_BY_ATTR]
        if flags:
            pe.parse_data_directories(directories=flags)


_DIR_FLAG_BY_ATTR = {
    "DIRECTORY_ENTRY_IMPORT": pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_IMPORT"],
    "DIRECTORY_ENTRY_DELAY_IMPORT": pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT"],
    "DIRECTORY_ENTRY_EXPORT": pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"],
    "DIRECTORY_ENTRY_RESOURCE": pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_RESOURCE"],
    "DIRECTORY_ENTRY_DEBUG": pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_DEBUG"],
}


def _flags_to_names(value: int, table: list[tuple[int, str]]) -> list[str]:
    return [name for bit, name in table if value & bit]


# ---------------------------------------------------------------- summary

def _file_info(path: str) -> dict[str, Any]:
    p = Path(path)
    st = p.stat()
    return {
        "path": str(p),
        "name": p.name,
        "size": st.st_size,
        "mtime": st.st_mtime,
    }


def parse_summary(path: str) -> dict[str, Any]:
    pe = _load(path)
    fh = pe.FILE_HEADER
    oh = pe.OPTIONAL_HEADER
    secs = []
    for s in pe.sections:
        name = s.Name.rstrip(b"\x00").decode("utf-8", "replace")
        secs.append({
            "name": name,
            "virtual_address": s.VirtualAddress,
            "virtual_size": s.Misc_VirtualSize,
            "raw_size": s.SizeOfRawData,
            "raw_ptr": s.PointerToRawData,
            "characteristics": s.Characteristics,
            "flags": _flags_to_names(s.Characteristics, SECTION_FLAGS),
            "entropy": round(s.get_entropy(), 4),
        })
    dirs = []
    for i, d in enumerate(oh.DATA_DIRECTORY):
        if d.VirtualAddress or d.Size:
            dirs.append({
                "index": i,
                "name": DIRECTORY_NAMES[i] if i < len(DIRECTORY_NAMES) else f"Dir{i}",
                "rva": d.VirtualAddress,
                "size": d.Size,
            })
    return {
        "file": _file_info(path),
        "machine": {"value": fh.Machine, "name": MACHINE_NAMES.get(fh.Machine, "unknown")},
        "timestamp": fh.TimeDateStamp,
        "characteristics": fh.Characteristics,
        "is_dll": bool(fh.Characteristics & 0x2000),
        "magic": {"value": oh.Magic, "name": "PE32+" if oh.Magic == 0x20B else "PE32"},
        "subsystem": {"value": oh.Subsystem, "name": SUBSYSTEM_NAMES.get(oh.Subsystem, "unknown")},
        "entry_point": oh.AddressOfEntryPoint,
        "image_base": oh.ImageBase,
        "size_of_image": oh.SizeOfImage,
        "linker": {"major": oh.MajorLinkerVersion, "minor": oh.MinorLinkerVersion},
        "os_version": f"{oh.MajorOperatingSystemVersion}.{oh.MinorOperatingSystemVersion}",
        "subsystem_version": f"{oh.MajorSubsystemVersion}.{oh.MinorSubsystemVersion}",
        "checksum": {"stated": oh.CheckSum, "actual": pe.generate_checksum()},
        "sections": secs,
        "data_directories": dirs,
    }


def parse_headers(path: str) -> dict[str, Any]:
    """DOS header fields worth showing + NT magic."""
    pe = _load(path)
    dos = pe.DOS_HEADER
    fields = {}
    for field in ("e_magic", "e_cblp", "e_cp", "e_cparhdr", "e_minalloc", "e_maxalloc",
                  "e_ss", "e_sp", "e_csum", "e_ip", "e_cs", "e_lfarlc", "e_ovno"):
        fields[field] = getattr(dos, field)
    nt_sig_raw = int.from_bytes(pe.__data__[dos.e_lfanew: dos.e_lfanew + 4], "little") \
        if len(pe.__data__) >= dos.e_lfanew + 4 else None
    return {
        "dos_header": fields,
        "dos_magic": {"value": dos.e_magic, "name": "MZ" if dos.e_magic == 0x5A4D else "??"},
        "pe_offset": dos.e_lfanew,
        "nt_signature": {"value": nt_sig_raw, "name": "PE\\0\\0" if nt_sig_raw == 0x00004550 else "??",
                         "hex": f"0x{nt_sig_raw:08X}" if nt_sig_raw else None},
        "file_header": {
            "machine": f"0x{pe.FILE_HEADER.Machine:04X}",
            "number_of_sections": pe.FILE_HEADER.NumberOfSections,
            "time_date_stamp": f"0x{pe.FILE_HEADER.TimeDateStamp:08X}",
            "pointer_to_symbol_table": pe.FILE_HEADER.PointerToSymbolTable,
            "number_of_symbols": pe.FILE_HEADER.NumberOfSymbols,
            "size_of_optional_header": pe.FILE_HEADER.SizeOfOptionalHeader,
            "characteristics": f"0x{pe.FILE_HEADER.Characteristics:04X}",
        },
        "file_size": len(pe.__data__),
    }


# ---------------------------------------------------------------- imports

def imports(path: str, offset: int = 0, limit: int = 200) -> dict[str, Any]:
    pe = _load(path)
    _ensure_dirs(pe, "imports")
    rows: list[dict[str, Any]] = []
    entries = getattr(pe, "DIRECTORY_ENTRY_IMPORT", [])
    for entry in entries:
        dll = entry.dll.decode("utf-8", "replace") if isinstance(entry.dll, bytes) else str(entry.dll)
        for imp in entry.imports:
            name = imp.name.decode("utf-8", "replace") if imp.name else None
            rows.append({
                "dll": dll,
                "function": name,
                "demangled": _demangle(name),
                "ordinal": imp.ordinal,
                "hint": imp.hint,
                "iat_rva": imp.address - pe.OPTIONAL_HEADER.ImageBase if imp.address else None,
            })
    total = len(rows)
    return {"total": total, "offset": offset, "limit": limit,
            "items": rows[offset:offset + limit],
            "dll_count": len(entries)}


def exports(path: str, offset: int = 0, limit: int = 200) -> dict[str, Any]:
    pe = _load(path)
    _ensure_dirs(pe, "exports")
    exp = getattr(pe, "DIRECTORY_ENTRY_EXPORT", None)
    if exp is None:
        return {"total": 0, "offset": offset, "limit": limit, "items": [],
                "dll_name": None, "timestamp": None}
    dll_name = exp.name.decode("utf-8", "replace") if exp.name else None
    rows = []
    for sym in exp.symbols:
        name = sym.name.decode("utf-8", "replace") if sym.name else None
        rows.append({
            "ordinal": sym.ordinal,
            "name": name,
            "demangled": _demangle(name),
            "rva": sym.address,
            "forwarder": sym.forwarder.decode("utf-8", "replace") if sym.forwarder else None,
        })
    total = len(rows)
    return {"total": total, "offset": offset, "limit": limit,
            "items": rows[offset:offset + limit],
            "dll_name": dll_name,
            "timestamp": exp.struct.TimeDateStamp}


# ---------------------------------------------------------------- resources

def _entry_label(entry: Any, is_type: bool) -> tuple[str, int | None]:
    """资源目录条目的规范标签。

    pefile 对命名条目设置 entry.name（PE_STRING：带 2 字节长度前缀的
    bytes 子类，必须 str() 取内容）且 entry.id 为 None；数字条目则相反。
    返回 (label, numeric_id)；命名条目 numeric_id 为 None。
    """
    name = getattr(entry, "name", None)
    if name is not None:
        return str(name), None
    eid = getattr(entry, "id", None)
    if eid is None:
        eid = getattr(entry.struct, "Id", 0) or 0
    if is_type:
        return RESOURCE_TYPE_NAMES.get(eid) or f"type_{eid}", eid
    return str(eid), eid


def resources(path: str) -> dict[str, Any]:
    pe = _load(path)
    _ensure_dirs(pe, "resources")
    root = getattr(pe, "DIRECTORY_ENTRY_RESOURCE", None)
    if root is None:
        return {"total": 0, "items": []}
    items = []
    for rtype in root.entries:
        type_name, _tid = _entry_label(rtype, is_type=True)
        for rname in rtype.directory.entries:
            name, _nid = _entry_label(rname, is_type=False)
            for rlang in rname.directory.entries:
                lang_id = getattr(rlang.struct, "Id", 0) or 0
                items.append({
                    "type": type_name,
                    "name": name,
                    "lang_id": lang_id,
                    "language": lang_id & 0x3FF,          # primary language id
                    "sublanguage": (lang_id >> 10) & 0x3F,  # sublanguage id
                    "rva": rlang.data.struct.OffsetToData,
                    "size": rlang.data.struct.Size,
                    "codepage": rlang.data.struct.CodePage,
                })
    items.sort(key=lambda x: (str(x["type"]), str(x["name"])))
    return {"total": len(items), "items": items}


# ---------------------------------------------------------------- signature / debug

def signature(path: str) -> dict[str, Any]:
    """Security directory (Authenticode) presence + CodeView/PDB info from debug dir."""
    pe = _load(path)
    _ensure_dirs(pe, "debug")
    sec_dir = pe.OPTIONAL_HEADER.DATA_DIRECTORY[4]  # IMAGE_DIRECTORY_ENTRY_SECURITY
    sig: dict[str, Any] = {
        "present": bool(sec_dir.VirtualAddress and sec_dir.Size),
        "rva_file_offset": sec_dir.VirtualAddress,
        "size": sec_dir.Size,
    }
    if sig["present"]:
        try:
            blob = pe.__data__[sec_dir.VirtualAddress: sec_dir.VirtualAddress + sec_dir.Size]
            # WIN_CERTIFICATE: wLength(4) wRevision(2) wCertificateType(2)
            if len(blob) >= 8:
                import struct
                length, revision, cert_type = struct.unpack_from("<IHH", blob, 0)
                sig["length"] = length
                sig["revision"] = f"0x{revision:04X}"
                sig["certificate_type"] = {
                    0x0002: "PKCS_SIGNED_DATA (Authenticode)",
                    0x0003: "TS_STACK_SIGNED",
                    0x0004: "PKCS1_SIGN_ONLY",
                }.get(cert_type, f"0x{cert_type:04X}")
        except Exception:
            pass
    pdb = None
    debug_entries = []
    for d in getattr(pe, "DIRECTORY_ENTRY_DEBUG", []):
        dtype = d.struct.Type
        info: dict[str, Any] = {"type": dtype, "size_of_data": d.struct.SizeOfData,
                                "rva": d.struct.AddressOfRawData, "timestamp": d.struct.TimeDateStamp}
        if dtype == IMAGE_DEBUG_TYPE_CODEVIEW:
            try:
                data = pe.get_data(d.struct.AddressOfRawData, d.struct.SizeOfData)
                if data[:4] == b"RSDS":
                    pdb = {
                        "guid": data[4:20].hex(),
                        "age": int.from_bytes(data[20:24], "little"),
                        "path": data[24:].split(b"\x00")[0].decode("utf-8", "replace"),
                    }
                elif data[:4] == b"NB10":
                    pdb = {
                        "age": int.from_bytes(data[8:12], "little"),
                        "path": data[16:].split(b"\x00")[0].decode("utf-8", "replace"),
                    }
            except Exception:
                pass
        debug_entries.append(info)
    return {"signature": sig, "pdb": pdb, "debug_entries": debug_entries}
