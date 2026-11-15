"""WinAPI program runner: repeat / if / break / continue / refs / budget."""

from __future__ import annotations

import pytest

from ntlab.core import winapi
from ntlab.core.errors import NtlabError


def _trace_api(s):
    return [(st.get("api"), "error" in st) for st in s["steps"]]


def test_sequential_calls():
    s = winapi.run_program([
        {"kind": "call", "api": "GetCurrentProcessId", "args": {}},
        {"kind": "call", "api": "GetTickCount64", "args": {}},
    ])
    assert s["failed"] == 0
    assert [t[0] for t in _trace_api(s)] == ["GetCurrentProcessId", "GetTickCount64"]
    assert s["steps"][0]["where"].startswith("main/")


def test_repeat_loop_with_break_and_continue():
    s = winapi.run_program([
        {
            "kind": "repeat", "times": 10,
            "body": [
                {"kind": "call", "api": "GetTickCount64", "args": {}},
                {"kind": "if", "cond": "$1.ret.value > 0", "then": [{"kind": "break"}]},
            ],
        },
    ])
    assert s["failed"] == 0
    apis = [t[0] for t in _trace_api(s)]
    assert apis.count("GetTickCount64") == 1  # 第一轮就 break
    assert s["steps"][0]["where"] == "main/1×10[1]/1"  # 轨迹标注循环轮次


def test_if_else_branch():
    s = winapi.run_program([
        {"kind": "call", "api": "GetCurrentProcessId", "args": {}},
        {"kind": "if", "cond": "$1.ret.value > 0",
         "then": [{"kind": "call", "api": "GetTickCount64", "args": {}}],
         "else": [{"kind": "call", "api": "GetCurrentThreadId", "args": {}}]},
    ])
    assert s["failed"] == 0
    assert [t[0] for t in _trace_api(s)] == ["GetCurrentProcessId", "GetTickCount64"]


def test_continue_skips_rest_of_body():
    s = winapi.run_program([
        {"kind": "repeat", "times": 3, "body": [
            {"kind": "continue"},
            {"kind": "call", "api": "GetTickCount64", "args": {}},
        ]},
    ])
    assert [t[0] for t in _trace_api(s)] == []  # continue 每轮跳过调用


def test_static_ref_slot_covered_by_loop():
    """$1 引用循环体内 call 块最近一轮的结果。"""
    s = winapi.run_program([
        {"kind": "repeat", "times": 2, "body": [
            {"kind": "call", "api": "GetTickCount64", "args": {}},
            {"kind": "call", "api": "RtlNtStatusToDosError", "args": {"status": "$1.ret.value"}},
        ]},
    ])
    assert s["failed"] == 0
    dos = s["steps"][1]["ret"]["value"]
    assert isinstance(dos, int)


def test_ref_slot_padding_when_skipped():
    """前面的 call 未执行（条件不满足）时，后面 call 的静态槽位仍能对齐。"""
    s = winapi.run_program([
        {"kind": "if", "cond": "1 > 2", "then": [{"kind": "call", "api": "GetTickCount64", "args": {}}]},
        {"kind": "call", "api": "GetTickCount64", "args": {}},
        {"kind": "call", "api": "RtlNtStatusToDosError", "args": {"status": "$2.ret.value"}},
    ])
    assert s["failed"] == 0


def test_arithmetic_and_logic_expressions():
    s = winapi.run_program([
        {"kind": "call", "api": "GetCurrentProcessId", "args": {}},
        {"kind": "if", "cond": "$1.ret.value > 0 && ($1.ret.value % 2 == 0 || $1.ret.value % 2 == 1)",
         "then": [{"kind": "call", "api": "GetTickCount64", "args": {}}]},
    ])
    assert s["failed"] == 0
    assert len(s["steps"]) == 2


def test_disallowed_expression_rejected():
    s = winapi.run_program([
        {"kind": "call", "api": "GetCurrentProcessId", "args": {}},
        {"kind": "if", "cond": "__import__('os').system('dir')", "then": []},
    ])
    # 恶意表达式被拒绝，记为 bad_expr，不产生系统调用
    assert s["steps"][-1]["code"] == "bad_expr"
    assert len(s["steps"]) == 2  # call + 错误记录，没有第三个块的执行


def test_break_outside_loop():
    s = winapi.run_program([{"kind": "break"}])
    assert s["steps"][0]["code"] == "bad_program"


def test_budget_exceeded_on_runaway():
    s = winapi.run_program([
        {"kind": "repeat", "times": winapi._MAX_REPEAT, "body": [
            {"kind": "call", "api": "GetCurrentProcessId", "args": {}},
        ]},
    ])
    # 预算耗尽后停止，trace 不会无限增长
    assert len(s["steps"]) <= winapi._MAX_PROGRAM_STEPS + 5
    errs = [st for st in s["steps"] if st.get("code") == "budget_exceeded"]
    assert errs, "runaway loop must be reported"


def test_program_api_error_keeps_running():
    s = winapi.run_program([
        {"kind": "call", "api": "BogusAPI", "args": {}},
        {"kind": "call", "api": "GetCurrentProcessId", "args": {}},
    ])
    assert s["steps"][0]["code"] == "not_callable"
    assert s["steps"][1]["ret"]["value"] > 0


def test_empty_and_bad_shapes():
    with pytest.raises(NtlabError):
        winapi.run_program([])
    with pytest.raises(NtlabError):
        winapi.run_program("not-a-list")
