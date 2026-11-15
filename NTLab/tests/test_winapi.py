"""WinAPI playground: whitelist catalog, call queue with refs, NTSTATUS, asm."""

from __future__ import annotations

import pytest

from ntlab.core import winapi


def test_catalog_nonempty_and_schema():
    cat = winapi.catalog()
    assert len(cat["apis"]) >= 15
    names = {a["name"] for a in cat["apis"]}
    assert "GetCurrentProcessId" in names and "MessageBoxW" in names
    a = next(a for a in cat["apis"] if a["name"] == "MessageBoxW")
    assert a["side"] in ("safe", "observable")
    assert {p["name"] for p in a["params"]} == {"hwnd", "text", "caption", "type"}


def test_queue_simple_calls():
    s = winapi.run_queue([
        {"api": "GetCurrentProcessId", "args": {}},
        {"api": "RtlGetVersion", "args": {}},
        {"api": "GetCursorPos", "args": {}},
    ])
    assert s["failed"] == 0
    pid = s["steps"][0]["ret"]["value"]
    assert pid > 0
    ver = s["steps"][1]
    assert ver["ret"]["success"] is True
    assert ver["out"]["ver"]["dwMajorVersion"] >= 10
    cur = s["steps"][2]["out"]["pt"]
    assert isinstance(cur["x"], int) and isinstance(cur["y"], int)


def test_queue_refs_and_file_handles(tmp_path):
    f = tmp_path / "probe.txt"
    f.write_text("hello ntlab", encoding="utf-8")
    s = winapi.run_queue([
        {"api": "CreateFileW", "args": {"path": str(f)}},
        {"api": "CloseHandle", "args": {"handle": "$1.ret"}},
    ])
    assert s["failed"] == 0
    h = s["steps"][0]["ret"]
    assert h["valid"] is True
    assert s["steps"][1]["ret"]["value"] == 1  # TRUE


def test_not_callable_and_bad_ref():
    s = winapi.run_queue([
        {"api": "BogusAPI", "args": {}},
        {"api": "RtlNtStatusToDosError", "args": {"status": "$9.ret"}},
    ])
    assert s["steps"][0]["code"] == "not_callable"
    assert s["steps"][1]["code"] == "bad_ref"
    assert s["failed"] == 2


def test_ntstatus_decode():
    s = winapi.run_queue([{"api": "RtlNtStatusToDosError", "args": {"status": "0xC0000005"}}])
    step = s["steps"][0]
    assert step["ret"]["value"] == 998  # ERROR_NOACCESS


def test_sessions_recorded():
    winapi.run_queue([{"api": "GetCurrentProcessId", "args": {}}])
    sessions = winapi.sessions()
    assert sessions and sessions[0]["ok"] >= 1


def test_asm_assemble_and_roundtrip():
    pytest.importorskip("keystone")
    r = winapi.assemble("mov rax, rcx\nxor rax, rbx\nret")
    assert r["size"] == 7  # 3 + 3 + 1
    assert r["bytes_hex"].startswith("48 89 c8")
    assert r["roundtrip"][0]["mnemonic"] == "mov"


def test_asm_errors():
    with pytest.raises(Exception):
        winapi.assemble("   ")
