"""用户数据库（SQLite）回归测试：编排程序 CRUD 与 PE 导出库缓存。"""

from __future__ import annotations

import pytest

from ntlab.core import userdb


@pytest.fixture()
def tmp_db(tmp_path, monkeypatch):
    monkeypatch.setattr(userdb, "_DB_PATH", tmp_path / "user.db")
    monkeypatch.setattr(userdb, "_CONN", None)
    yield


def test_program_crud_roundtrip(tmp_db):
    prog = [{"kind": "call", "api": "GetTickCount64", "args": {}}]
    saved = userdb.save_program("我的编排", prog, "BITS 64")
    assert saved["name"] == "我的编排" and saved["program"] == prog
    assert saved["asm_text"].startswith("BITS 64")

    got = userdb.get_program(saved["id"])
    assert got["program"] == prog and got["asm_text"] == "BITS 64"

    # 同名 upsert：id 不变、内容更新
    again = userdb.save_program("我的编排", [{"kind": "break"}], "")
    assert again["id"] == saved["id"] and again["program"] == [{"kind": "break"}]
    assert len(userdb.list_programs()) == 1

    userdb.delete_program(saved["id"])
    with pytest.raises(Exception):
        userdb.get_program(saved["id"])


def test_library_cache_roundtrip(tmp_db):
    modules = {
        "kernel32": {
            "name": "kernel32", "path": r"C:\Windows\System32\kernel32.dll",
            "dll_name": "kernel32.dll", "count": 2,
            "exports": [
                {"name": "GetTickCount64", "ordinal": 1, "rva": 0x10, "trust": "generic"},
                {"name": "CreateProcessW", "ordinal": 2, "rva": 0x20, "trust": "blocked"},
            ],
        },
        "ntdll": {
            "name": "ntdll", "path": r"C:\Windows\System32\ntdll.dll",
            "dll_name": "ntdll.dll", "count": 1,
            "exports": [
                {"name": "NtQueryInformationProcess", "ordinal": 1, "rva": 0x30, "trust": "restricted"},
            ],
        },
    }
    userdb.save_library(modules, built_at=12345.0)
    loaded = userdb.load_library()
    assert set(loaded) == {"kernel32", "ntdll"}
    k32 = loaded["kernel32"]
    assert k32["path"] == modules["kernel32"]["path"]
    assert [e["name"] for e in k32["exports"]] == ["CreateProcessW", "GetTickCount64"]
    assert {e["trust"] for e in k32["exports"]} == {"generic", "blocked"}
    assert loaded["ntdll"]["exports"][0]["trust"] == "restricted"
    assert userdb.library_built_at() == pytest.approx(12345.0)


def test_load_library_empty(tmp_db):
    assert userdb.load_library() is None


def test_archive_roundtrip(tmp_db):
    manifest = [{"file": "RT_VERSION/1_lang1033.bin", "type": "RT_VERSION",
                 "name": "1", "lang": 1033, "size": 952}]
    aid = userdb.save_archive("notepad.resources.zip", b"PK\x03\x04demo",
                              1, manifest)
    assert aid >= 1

    rows = userdb.list_archives()
    assert len(rows) == 1 and rows[0]["id"] == aid
    assert rows[0]["file_name"] == "notepad.resources.zip"
    assert "content" not in rows[0]          # 列表不携带 BLOB

    got = userdb.get_archive(aid)
    assert got["content"] == b"PK\x03\x04demo"
    assert got["count"] == 1 and got["manifest"] == manifest

    userdb.delete_archive(aid)
    assert userdb.list_archives() == []
    with pytest.raises(Exception):
        userdb.get_archive(aid)
