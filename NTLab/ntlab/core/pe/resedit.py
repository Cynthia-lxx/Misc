"""PE 资源编辑器：提取、替换（.rsrc 整节重建）、打包。

资源替换采用 Resource Hacker 同款思路——按 pefile 读出的三层目录树
（type / name / lang）重建整个 .rsrc 节内容：
- 新内容 ≤ 旧节 raw size：原地覆写（零填充）；
- 超出：仅当 .rsrc 是最后一节时允许把文件向后扩展（更新 SizeOfImage）；
- 输出统一写到新文件，原地保存 = 覆盖原文件（自动留 .bak 备份）。
"""

from __future__ import annotations

import io
import os
import struct
import zipfile
from pathlib import Path

import pefile

from ntlab.core.errors import NtlabError, NotFoundError
from ntlab.core.pe.parser import (
    RESOURCE_TYPE_NAMES,
    _entry_label,
    _ensure_dirs,
    _load,
)

RESOURCE_TYPE_NAMES_INV = {v: k for k, v in RESOURCE_TYPE_NAMES.items()}


# ---------------------------------------------------------------- helpers

def _walk(pe: pefile.PE) -> list[dict]:
    """展平资源树为 [{type,name,lang,codepage,data}]（data 为 bytes）。

    type/name 保留 id-vs-字符串 区分：label 为清单显示名；is_name 标记是否
    为目录字符串名（数字 id 的 label 是十进制字符串）。
    """
    _ensure_dirs(pe, "resources")
    root = getattr(pe, "DIRECTORY_ENTRY_RESOURCE", None)
    if root is None:
        return []
    out = []
    for rtype in root.entries:
        tlabel, _tid = _entry_label(rtype, is_type=True)
        for rname in rtype.directory.entries:
            nlabel, _nid = _entry_label(rname, is_type=False)
            for rlang in rname.directory.entries:
                lang = getattr(rlang.struct, "Id", 0) or 0
                data = bytes(pe.get_data(rlang.data.struct.OffsetToData,
                                         rlang.data.struct.Size))
                out.append({
                    "type": tlabel, "name": nlabel, "lang": lang,
                    "codepage": rlang.data.struct.CodePage, "data": data,
                })
    return out


def extract(path: str, rtype: str, name: str, lang: int) -> bytes:
    """按 (type, name, lang) 提取单个资源数据。"""
    pe = _load(path)
    for it in _walk(pe):
        if it["type"] == rtype and it["name"] == name and it["lang"] == int(lang):
            return it["data"]
    raise NotFoundError(
        f"resource ({rtype}, {name}, lang={lang}) not found in {path}")


def pack_zip(path: str) -> tuple[bytes, int, list[dict]]:
    """全部资源打包为 zip（type/name_lang.bin），返回 (zip字节, 数量, 清单)。"""
    pe = _load(path)
    items = _walk(pe)
    buf = io.BytesIO()
    manifest = []
    used: set[str] = set()
    with zipfile.ZipFile(buf, "w", zipfile.ZIP_STORED) as zf:
        for it in items:
            fname = f"{it['type']}/{it['name']}_lang{it['lang']}.bin"
            while fname in used:
                fname = fname[:-4] + f"_{len(used)}.bin"
            used.add(fname)
            zf.writestr(fname, it["data"])
            manifest.append({"file": fname, "type": it["type"],
                             "name": it["name"], "lang": it["lang"],
                             "size": len(it["data"])})
    return buf.getvalue(), len(items), manifest


# ---------------------------------------------------------------- rebuild

def _checksum(data: bytes) -> int:
    """PE 头 CheckSum 算法：16 位字零和 + 进位折叠 + 长度。"""
    if len(data) % 2:
        data += b"\x00"
    total = 0
    for i in range(0, len(data), 2):
        total += data[i] + (data[i + 1] << 8)
        total = (total & 0xFFFF) + (total >> 16)
    total = (total & 0xFFFF) + (total >> 16)
    return (total + len(data)) & 0xFFFFFFFF


def _checksum_offset(pe: pefile.PE) -> int:
    """CheckSum 字段的文件偏移：DOS 头 + 签名 + FileHeader + 64。"""
    return pe.DOS_HEADER.e_lfanew + 4 + 20 + 64


def _rva_to_offset(pe: pefile.PE, rva: int) -> int | None:
    for s in pe.sections:
        va, vsz = s.VirtualAddress, max(s.Misc_VirtualSize, s.SizeOfRawData)
        if va <= rva < va + vsz:
            return rva - va + s.PointerToRawData
    return None


def _build_section_blob(items: list[dict], section_rva: int) -> bytes:
    """从展平清单重建 .rsrc 节镜像。

    布局：根目录 → 各类型目录 → 各名字目录 → 数据描述符数组 →
    目录名 UTF-16 串 → 8 对齐的数据块。
    """
    # 三层树（保持读出顺序）；每层记录 (label, is_name, id)
    types: list[tuple[str, bool, int | None]] = []
    names_of: dict[str, list[tuple[str, bool, int | None]]] = {}
    langs_of: dict[tuple[str, str], list[int]] = {}
    leaf: dict[tuple[str, str, int], dict] = {}
    for it in items:
        t, n, l = it["type"], it["name"], it["lang"]
        if not any(x[0] == t for x in types):
            if t.isdigit():                      # 数字 id（清单里即十进制串）
                is_name, tid = False, int(t)
            elif t.startswith("type_"):          # 未知数字 id 的兜底标签
                is_name, tid = False, int(t[5:])
            elif t in RESOURCE_TYPE_NAMES_INV:   # 标准类型的友好标签（数值类型！）
                is_name, tid = False, RESOURCE_TYPE_NAMES_INV[t]
            else:                                # 真正的目录字符串名
                is_name, tid = True, None
            types.append((t, is_name, tid))
        names_of.setdefault(t, [])
        if not any(x[0] == n for x in names_of[t]):
            names_of[t].append((n, not n.isdigit(), None))
        bucket = langs_of.setdefault((t, n), [])
        if l not in bucket:
            bucket.append(l)
        leaf[(t, n, l)] = it

    # 规范排序：命名条目在前（按小写升序），数字 id 条目在后（升序）
    def _dir_sort(x: tuple[str, bool, int | None]) -> tuple:
        label, is_name, tid = x
        if is_name:
            return (0, label.lower(), 0)
        return (1, "", tid if tid is not None else int(label))

    ordered_types = sorted(types, key=_dir_sort)
    ordered_names = {t: sorted(names_of[t], key=_dir_sort) for t in names_of}
    for k in langs_of:
        langs_of[k] = sorted(langs_of[k])

    n_types = len(types)
    n_leaves = len(items)

    # 目录名串区（仅 is_name 的条目）
    named = [x for x in types if x[1]] + [x for v in names_of.values() for x in v if x[1]]
    str_off: dict[str, int] = {}
    str_area_size = sum(2 + 2 * len(s) for s in {x[0] for x in named})

    dir_size = (16 + 8 * n_types) + sum(16 + 8 * len(names_of[t]) for t, _, _ in types) \
        + sum(16 + 8 * len(langs_of[(t, n)]) for t in names_of for n, _, _ in names_of[t])
    desc_base = dir_size
    str_base = desc_base + 16 * n_leaves
    cur = (str_base + str_area_size + 7) & ~7

    type_dir_off = {}
    o = 16 + 8 * n_types
    for t, _, _ in ordered_types:
        type_dir_off[t] = o
        o += 16 + 8 * len(names_of[t])
    name_dir_off = {}
    for t, _, _ in ordered_types:
        for n, _, _ in ordered_names[t]:
            name_dir_off[(t, n)] = o
            o += 16 + 8 * len(langs_of[(t, n)])

    blob = bytearray(cur)
    pos = str_base
    for s in sorted({x[0] for x in named}):
        raw = s.encode("utf-16-le")
        struct.pack_into("<H", blob, pos, len(s))
        blob[pos + 2:pos + 2 + len(raw)] = raw
        str_off[s] = pos
        pos += 2 + len(raw)

    def entry(name_field: int, off_field: int, is_sub: bool) -> bytes:
        if is_sub:
            off_field |= 0x80000000
        return struct.pack("<II", name_field, off_field)

    # 数据描述符数组（槽位顺序与目录遍历顺序一致）
    slots: dict[tuple[str, str, int], tuple[int, int]] = {}
    idx = 0
    data_off = cur
    for t, _, _ in ordered_types:
        for n, _, _ in ordered_names[t]:
            for l in langs_of[(t, n)]:
                it = leaf[(t, n, l)]
                struct.pack_into("<IIII", blob, desc_base + 16 * idx,
                                 section_rva + data_off, len(it["data"]),
                                 it["codepage"], 0)
                slots[(t, n, l)] = (desc_base + 16 * idx, data_off)
                data_off = (data_off + len(it["data"]) + 7) & ~7
                idx += 1

    # 根目录：named / id 计数必须分开写
    n_named_t = sum(1 for x in ordered_types if x[1])
    n_id_t = n_types - n_named_t
    struct.pack_into("<IIHHHH", blob, 0, 0, 0, 0, 0, n_named_t, n_id_t)
    off = 16
    for t, is_name, tid in ordered_types:
        field = (str_off[t] | 0x80000000) if is_name else (tid or 0)
        blob[off:off + 8] = entry(field, type_dir_off[t], True)
        off += 8
    for t, is_name, tid in ordered_types:
        o = type_dir_off[t]
        n_names_t = ordered_names[t]
        n_named_n = sum(1 for x in n_names_t if x[1])
        n_id_n = len(n_names_t) - n_named_n
        struct.pack_into("<IIHHHH", blob, o, 0, 0, 0, 0, n_named_n, n_id_n)
        o += 16
        for n, n_is_name, nid in n_names_t:
            field = (str_off[n] | 0x80000000) if n_is_name else (nid or int(n))
            blob[o:o + 8] = entry(field, name_dir_off[(t, n)], True)
            o += 8
    for t, is_name, tid in ordered_types:
        for n, n_is_name, nid in ordered_names[t]:
            o = name_dir_off[(t, n)]
            langs = langs_of[(t, n)]
            struct.pack_into("<IIHHHH", blob, o, 0, 0, 0, 0, 0, len(langs))
            o += 16
            for l in langs:
                desc_off, _ = slots[(t, n, l)]
                blob[o:o + 8] = entry(l & 0x7FFFFFFF, desc_off, False)
                o += 8
    # 数据块写入前必须把 blob 预扩到最终大小——bytearray 切片赋值对
    # 越界起点会钳制到当前末尾（append），绝不会按 it_off 落位。
    if len(blob) < data_off:
        blob.extend(b"\x00" * (data_off - len(blob)))
    for (t, n, l), (_, it_off) in slots.items():
        it = leaf[(t, n, l)]
        blob[it_off:it_off + len(it["data"])] = it["data"]
    if os.environ.get("NTLAB_RESDEBUG"):
        bad = sum(1 for k, (_, off) in slots.items()
                  if blob[off:off + len(leaf[k]["data"])] != leaf[k]["data"])
        print("RESDEBUG:", "OK" if not bad else f"{bad} mismatches")
    return bytes(blob)


def replace_resources(path: str, replacements: list[dict],
                      out_path: str | None = None, in_place: bool = False) -> dict:
    """替换若干资源并重建 .rsrc 节。replacements: [{type,name,lang,data(bytes),codepage?}]"""
    src = Path(path)
    if not src.is_absolute():
        src = Path.cwd() / src
    src = src.resolve()
    if not src.is_file():
        raise NotFoundError(f"file not found: {src}")
    # 不能用 parser._load：它对文件路径用 mmap 且缓存不释放，映射存续期间
    # 无法以写模式打开同一文件（Windows 上 errno 22）。这里读入字节自建 PE。
    file_bytes = src.read_bytes()
    pe = pefile.PE(data=file_bytes)
    _ensure_dirs(pe, "resources")
    root = getattr(pe, "DIRECTORY_ENTRY_RESOURCE", None)
    if root is None:
        raise NtlabError("no_resources", "file has no .rsrc section")
    section = None
    for s in pe.sections:
        if s.Name.rstrip(b"\x00") == b".rsrc":
            section = s
            break
    if section is None:
        raise NtlabError("no_resources", "no .rsrc section found")

    items = _walk(pe)
    keymap = {(it["type"], it["name"], it["lang"]): it for it in items}
    replaced = []
    for rep in replacements or []:
        key = (rep["type"], str(rep["name"]), int(rep["lang"]))
        if key not in keymap:
            raise NtlabError("unknown_resource",
                             f"resource ({key[0]}, {key[1]}, lang={key[2]}) not found")
        it = keymap[key]
        it["data"] = bytes(rep["data"])
        if rep.get("codepage") is not None:
            it["codepage"] = int(rep["codepage"])
        replaced.append(key)

    rsrc = _build_section_blob(items, section.VirtualAddress)

    raw = bytearray(file_bytes)
    old_raw = section.SizeOfRawData
    sec_align = pe.OPTIONAL_HEADER.SectionAlignment
    file_align = pe.OPTIONAL_HEADER.FileAlignment
    new_virtual = (len(rsrc) + sec_align - 1) // sec_align * sec_align
    new_raw = (len(rsrc) + file_align - 1) // file_align * file_align

    grew = len(rsrc) > old_raw
    sec_idx = pe.sections.index(section)
    opt_off = pe.DOS_HEADER.e_lfanew + 4 + 20
    sec_hdr_base = opt_off + pe.FILE_HEADER.SizeOfOptionalHeader
    dd_off = opt_off + pe.FILE_HEADER.SizeOfOptionalHeader \
        - 8 * pe.OPTIONAL_HEADER.NumberOfRvaAndSizes

    if not grew:
        start = section.PointerToRawData
        raw[start:start + len(rsrc)] = rsrc
        raw[start + len(rsrc):start + old_raw] = b"\x00" * (old_raw - len(rsrc))
    else:
        # .rsrc 变大：整体重建文件——新节镜像替换旧区，其后内容平移。
        # （不能只追加尾部：重建 blob 的目录/描述符布局与原文件不同）
        start = section.PointerToRawData
        old_end = start + old_raw
        delta = new_raw - old_raw
        later_va = [s.VirtualAddress for s in pe.sections
                    if s.VirtualAddress > section.VirtualAddress]
        if later_va and section.VirtualAddress + new_virtual > min(later_va):
            raise NtlabError(
                "rsrc_not_last",
                "growing .rsrc would overlap the next section's virtual range")
        blob_padded = rsrc + b"\x00" * (new_raw - len(rsrc))
        raw = bytearray(raw[:start]) + bytearray(blob_padded) + bytearray(raw[old_end:])
        # 位于旧 .rsrc 之后的节：PointerToRawData += delta
        for i, s in enumerate(pe.sections):
            if s.PointerToRawData >= old_end and s.PointerToRawData > 0:
                struct.pack_into("<I", raw, sec_hdr_base + i * 40 + 20,
                                 s.PointerToRawData + delta)
        # debug 目录条目的 PointerToRawData（RVA 不变，文件偏移平移）
        dbg = pe.OPTIONAL_HEADER.DATA_DIRECTORY[6]
        if dbg.VirtualAddress:
            dbg_off = _rva_to_offset(pe, dbg.VirtualAddress)
            if dbg_off is not None:
                for i in range(dbg.Size // 28):
                    p = dbg_off + 28 * i
                    ptr = struct.unpack_from("<I", raw, p + 24)[0]
                    if ptr >= old_end:
                        struct.pack_into("<I", raw, p + 24, ptr + delta)
        # security 目录（index 4）的 VirtualAddress 是文件偏移而非 RVA
        sec_dir = pe.OPTIONAL_HEADER.DATA_DIRECTORY[4]
        if sec_dir.VirtualAddress and sec_dir.VirtualAddress >= old_end:
            struct.pack_into("<I", raw, dd_off + 8 * 4,
                             sec_dir.VirtualAddress + delta)
    new_raw_final = max(old_raw, new_raw)

    # 节头 / SizeOfImage 的修改必须写回文件字节（pefile 对象上的赋值不会落盘）
    sec_off = sec_hdr_base + sec_idx * 40
    struct.pack_into("<I", raw, sec_off + 8, len(rsrc))          # VirtualSize
    struct.pack_into("<I", raw, sec_off + 16, new_raw_final)     # SizeOfRawData
    size_of_image = max(pe.OPTIONAL_HEADER.SizeOfImage,
                        section.VirtualAddress + new_virtual)
    struct.pack_into("<I", raw, opt_off + 56, size_of_image)     # SizeOfImage

    ck_off = _checksum_offset(pe)
    raw[ck_off:ck_off + 4] = b"\x00" * 4
    ck = _checksum(bytes(raw))
    struct.pack_into("<I", raw, ck_off, ck)

    backup = None
    if in_place:
        bak = src.with_suffix(src.suffix + ".bak")
        if not bak.exists():
            bak.write_bytes(src.read_bytes())
        backup = str(bak)
        target = src
    else:
        target = Path(out_path or str(src.with_suffix(".patched")))
    target = Path(target)
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_bytes(bytes(raw))
    return {
        "path": str(target), "backup": backup,
        "replaced": [list(k) for k in replaced],
        "rsrc_size": len(rsrc), "old_rsrc_size": old_raw, "grew": grew,
        "checksum": f"0x{ck:08X}",
    }
