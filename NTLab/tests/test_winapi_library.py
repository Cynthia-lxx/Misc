"""PE 导出库发现、信任分级与动态调用隔离的回归测试。"""

from __future__ import annotations

import pytest

from ntlab.core import winapi, winapi_library


def test_classify_trust_levels():
    assert winapi_library.classify("GetTickCount64") == "generic"
    assert winapi_library.classify("MessageBoxW") == "restricted"
    assert winapi_library.classify("CreateProcessW") == "blocked"
    assert winapi_library.classify("NtWriteVirtualMemory") == "blocked"
    assert winapi_library.classify("RegSetValueExW") == "blocked"


def test_blocked_api_rejected_before_worker(monkeypatch):
    """硬拦截名单在子进程启动前生效。"""
    item = {"name": "CreateProcessW", "ordinal": 1, "rva": 0, "trust": "blocked"}
    monkeypatch.setitem(winapi_library._STATE["modules"], "kernel32", {
        "name": "kernel32", "path": r"C:\Windows\System32\kernel32.dll",
        "count": 1, "dll_name": "kernel32.dll", "exports": [item],
    })
    monkeypatch.setitem(winapi_library._STATE["by_name"], "createprocessw",
                        [("kernel32", item)])
    with pytest.raises(Exception) as exc:
        winapi.run_call("CreateProcessW", {}, [], "kernel32")
    assert getattr(exc.value, "code", "") == "blocked_api"


def test_dynamic_call_runs_in_worker(monkeypatch):
    """通用签名调用在独立进程完成，返回有符号 64 位值。"""
    item = {"name": "GetCurrentProcess", "ordinal": 2, "rva": 0, "trust": "generic"}
    monkeypatch.setitem(winapi_library._STATE["modules"], "kernel32", {
        "name": "kernel32", "path": r"C:\Windows\System32\kernel32.dll",
        "count": 1, "dll_name": "kernel32.dll", "exports": [item],
    })
    monkeypatch.setitem(winapi_library._STATE["by_name"], "getcurrentprocess",
                        [("kernel32", item)])
    refs: list[dict] = []
    step = winapi.run_call("GetCurrentProcess", {}, refs, "kernel32")
    assert step["module"] == "kernel32" and step["api"] == "GetCurrentProcess"
    assert step["side"] == "dynamic" and step["ret"]["value"] == -1
    assert refs[0]["ret"]["__raw__"] == -1


def test_whitelist_still_takes_precedence():
    """白名单内的同名 API 走原有类型化调用，不受动态分级影响。"""
    step = winapi.run_call("GetTickCount64", {}, [])
    assert step["module"] == "kernel32" and step["side"] == "safe"
