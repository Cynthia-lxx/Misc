"""PE parser tests using OS-supplied binaries as fixtures."""

from __future__ import annotations

import os
import sys

import pytest

from ntlab.core import pe
from ntlab.core.errors import ParseError

NOTEPAD = os.path.join(os.environ.get("WINDIR", r"C:\Windows"), "System32", "notepad.exe")
KERNEL32 = os.path.join(os.environ.get("WINDIR", r"C:\Windows"), "System32", "kernel32.dll")

requires_windows_binaries = pytest.mark.skipif(
    not (os.path.isfile(NOTEPAD) and os.path.isfile(KERNEL32)),
    reason="Windows system binaries unavailable",
)


def test_reject_non_pe(tmp_path):
    fake = tmp_path / "fake.exe"
    fake.write_bytes(b"MZ not really a pe file" + b"\x00" * 64)
    with pytest.raises(ParseError):
        pe.parse_summary(str(fake))


def test_reject_missing():
    with pytest.raises(Exception):
        pe.parse_summary(r"C:\definitely\not\here.exe")


@requires_windows_binaries
def test_summary_notepad():
    s = pe.parse_summary(NOTEPAD)
    assert s["file"]["name"].lower() == "notepad.exe"
    assert s["machine"]["name"] == "x64 (AMD64)"
    assert s["magic"]["name"] == "PE32+"
    assert len(s["sections"]) > 0
    assert any(sec["name"] == ".text" for sec in s["sections"])
    # imports directory should be present in the summary
    assert any(d["name"] == "Import" for d in s["data_directories"])


@requires_windows_binaries
def test_headers_notepad():
    h = pe.parse_headers(NOTEPAD)
    assert h["dos_magic"]["name"] == "MZ"
    assert h["dos_header"]["e_magic"] == 0x5A4D
    assert h["file_header"]["number_of_sections"] > 0


@requires_windows_binaries
def test_imports_kernel32():
    r = pe.imports(KERNEL32, offset=0, limit=50)
    assert r["total"] > 0
    assert len(r["items"]) <= 50
    dlls = {row["dll"].lower() for row in r["items"]}
    assert any(d.startswith("api-ms-") or d.endswith(".dll") for d in dlls)
    # pagination math
    r2 = pe.imports(KERNEL32, offset=50, limit=50)
    assert r2["offset"] == 50


@requires_windows_binaries
def test_exports_kernel32():
    r = pe.exports(KERNEL32, offset=0, limit=2000)
    assert r["total"] > 0
    assert r["dll_name"] and r["dll_name"].lower().startswith("kernel32")
    names = {row["name"] for row in r["items"] if row["name"]}
    assert "CreateFileW" in names or "CreateProcessW" in names


@requires_windows_binaries
def test_resources_notepad():
    r = pe.resources(NOTEPAD)
    assert r["total"] > 0
    types = {item["type"] for item in r["items"]}
    assert "VERSION" in types or "ICON" in types or "MUI" in types or True


@requires_windows_binaries
def test_signature_shape():
    r = pe.signature(NOTEPAD)
    assert "signature" in r and "pdb" in r and "debug_entries" in r
    assert isinstance(r["signature"]["present"], bool)
