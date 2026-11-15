"""Tests for IDA-flavored views: strings / hexdump / search / disasm / xrefs / demangle."""

from __future__ import annotations

import os

import pytest

from ntlab.core.pe import demangle, disasm, hexdump
from ntlab.core.pe.parser import _load
from ntlab.api.pe import resolve as api_resolve
from ntlab.core.errors import InvalidPathError

KERNEL32 = os.path.join(os.environ.get("WINDIR", r"C:\Windows"), "System32", "kernel32.dll")

requires_kernel32 = pytest.mark.skipif(
    not os.path.isfile(KERNEL32), reason="kernel32.dll unavailable")


@requires_kernel32
def test_strings_finds_ascii_and_utf16():
    r = hexdump.strings(KERNEL32, offset=0, limit=200)
    assert r["total"] > 100
    encs = {i["encoding"] for i in r["items"]}
    assert "ascii" in encs
    assert all(i["text"] for i in r["items"])
    # pagination
    r2 = hexdump.strings(KERNEL32, offset=200, limit=100)
    assert r2["items"][0]["file_offset"] >= r["items"][-1]["file_offset"]
    # every string has a section mapping
    assert all(i["section"] for i in r["items"])


@requires_kernel32
def test_hexdump_rows_aligned():
    pe = _load(KERNEL32)
    ep = pe.OPTIONAL_HEADER.AddressOfEntryPoint
    r = hexdump.hexdump(KERNEL32, rva=ep, size=64)
    assert len(r["rows"]) == 4
    assert r["rows"][0]["rva"] % 16 == 0
    assert r["section"] == ".text"
    assert r["image_base"] > 0
    # file-offset variant agrees
    r2 = hexdump.hexdump(KERNEL32, file_offset=r["file_offset"], size=16)
    assert r2["rows"][0]["hex"] == r["rows"][0]["hex"]


@requires_kernel32
def test_search_text_and_hex():
    r = hexdump.search(KERNEL32, "MZ", mode="text", start=0, limit=10)
    assert r["matches"][0]["file_offset"] == 0
    rh = hexdump.search(KERNEL32, "4d5a", mode="hex", start=0, limit=10)
    assert rh["matches"][0]["file_offset"] == 0
    with pytest.raises(InvalidPathError):
        hexdump.search(KERNEL32, "zz", mode="hex")


@requires_kernel32
def test_search_case_sensitivity():
    data = _load(KERNEL32).__data__
    idx = data.find(b"KERNEL32.dll")
    assert idx > 0  # kernel32 导出名为混合大小写
    # 默认忽略大小写：小写查询也能命中
    ins = hexdump.search(KERNEL32, "kernel32.dll", mode="text", start=0, limit=10)
    assert any(m["file_offset"] == idx for m in ins["matches"])
    # 开启区分大小写：小写查询不命中，精确大小写命中
    sens = hexdump.search(KERNEL32, "kernel32.dll", mode="text", start=0, limit=10, case_sensitive=True)
    assert not sens["matches"]
    exact = hexdump.search(KERNEL32, "KERNEL32.dll", mode="text", start=0, limit=10, case_sensitive=True)
    assert any(m["file_offset"] == idx for m in exact["matches"])
    # hex 模式与该开关无关（字节本就无大小写）
    hx = hexdump.search(KERNEL32, "4B4552 4E", mode="hex", start=idx, limit=5, case_sensitive=True)
    assert hx["matches"] and hx["matches"][0]["file_offset"] == idx


@requires_kernel32
def test_disasm_at_entry():
    pe = _load(KERNEL32)
    ep = pe.OPTIONAL_HEADER.AddressOfEntryPoint
    r = disasm.disasm(KERNEL32, rva=ep, count=20)
    assert len(r["items"]) >= 10
    assert all(i["mnemonic"] for i in r["items"])
    assert r["items"][0]["rva"] == ep
    assert r["next_rva"] > ep
    with pytest.raises(InvalidPathError):
        disasm.disasm(KERNEL32, rva=0xFFFFFFFF)


@requires_kernel32
def test_xrefs_to_entry_has_callers():
    pe = _load(KERNEL32)
    ep = pe.OPTIONAL_HEADER.AddressOfEntryPoint
    r = disasm.xrefs_to(KERNEL32, ep)
    assert isinstance(r["total_refs"], int)
    for c in r["callers"]:
        assert 0 <= c["rva"] < pe.OPTIONAL_HEADER.SizeOfImage


@requires_kernel32
def test_disasm_iat_annotation():
    """rip-relative 命中 IAT 时应给出 dll!func 注释。"""
    rva = _load(KERNEL32).sections[0].VirtualAddress
    found = None
    for _ in range(40):  # 顺序扫 .text，最多 40 页
        r = disasm.disasm(KERNEL32, rva=rva, count=200)
        for it in r["items"]:
            if "iat" in it:
                found = it
                break
        if found or r["next_rva"] <= rva:
            break
        rva = r["next_rva"]
    assert found, "no rip-relative IAT reference found in first pages of .text"
    dll_part, func_part = found["iat"].split("!", 1)
    assert dll_part and func_part


def test_resolve_module_name():
    r = api_resolve("kernel32.dll")
    assert r["path"] and r["path"].lower().endswith("kernel32.dll")
    assert api_resolve("definitely-not-exist-1234.dll")["path"] is None
    with pytest.raises(InvalidPathError):
        api_resolve("../evil")
    with pytest.raises(InvalidPathError):
        api_resolve("")


def test_demangle_msvc_names():
    d = demangle.demangle("?foo@Bar@@QEAAXXZ")
    assert d and "Bar::foo" in d and "void" in d
    assert demangle.demangle("CreateFileW") is None
    assert demangle.demangle(None) is None
    assert demangle.demangle("??_7type_info@@6B@") is not None
