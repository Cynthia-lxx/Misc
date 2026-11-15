"""WinAPI playground core: whitelisted ctypes call runner + call queue.

Whitelist is enforced here — the API layer only forwards JSON dicts. Calls
execute inside the local NTLab server process, which binds 127.0.0.1 only
(single-user tool). Every callable is declared with a parameter schema and a
side-effect level ("safe" = no persistent effect, "observable" = user-visible
effect such as sound / cursor / modal dialog / a short sleep).

Call queue: a list of {api, args} steps executed in order. Any string argument
may reference an earlier step's result: "$N" (whole result), "$N.ret" or
"$N.out.<field>" — that is what makes multi-call sequences meaningful.
"""

from __future__ import annotations

import ast
import ctypes
import re
import time
import uuid
from dataclasses import dataclass
from ctypes import wintypes
from typing import Any

from ntlab.core.errors import NtlabError

# ---------------------------------------------------------------- structures

class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]


class RECT(ctypes.Structure):
    _fields_ = [
        ("left", ctypes.c_long), ("top", ctypes.c_long),
        ("right", ctypes.c_long), ("bottom", ctypes.c_long),
    ]


class MEMORYSTATUSEX(ctypes.Structure):
    _fields_ = [
        ("dwLength", wintypes.DWORD),
        ("dwMemoryLoad", wintypes.DWORD),
        ("ullTotalPhys", ctypes.c_ulonglong),
        ("ullAvailPhys", ctypes.c_ulonglong),
        ("ullTotalPageFile", ctypes.c_ulonglong),
        ("ullAvailPageFile", ctypes.c_ulonglong),
        ("ullTotalVirtual", ctypes.c_ulonglong),
        ("ullAvailVirtual", ctypes.c_ulonglong),
        ("ullAvailExtendedVirtual", ctypes.c_ulonglong),
    ]


class OSVERSIONINFOW(ctypes.Structure):
    _fields_ = [
        ("dwOSVersionInfoSize", wintypes.DWORD),
        ("dwMajorVersion", wintypes.DWORD),
        ("dwMinorVersion", wintypes.DWORD),
        ("dwBuildNumber", wintypes.DWORD),
        ("dwPlatformId", wintypes.DWORD),
        ("szCSDVersion", wintypes.WCHAR * 128),
    ]


_OUT_STRUCTS = {
    "POINT": POINT,
    "RECT": RECT,
    "MEMORYSTATUSEX": MEMORYSTATUSEX,
    "OSVERSIONINFOW": OSVERSIONINFOW,
}

# ---------------------------------------------------------------- spec model

@dataclass(frozen=True)
class Param:
    name: str
    type: str              # int | hex | str | bool | ptr | out:<Struct> | outint | buf | bin
    desc: str = ""
    default: Any = None
    optional: bool = False
    hidden: bool = False   # 运行器自动供给（如缓冲区长度），不出现在 UI 输入里


@dataclass(frozen=True)
class ApiSpec:
    name: str
    module: str            # dll name without extension
    desc: str
    params: tuple[Param, ...] = ()
    ret: str = "int"       # int | bool | handle | str | NTSTATUS | void
    side: str = "safe"     # safe | observable
    note: str = ""


_CATALOG: tuple[ApiSpec, ...] = (
    # ---- kernel32: process / system info
    ApiSpec("GetCurrentProcessId", "kernel32", "当前进程 PID", ret="int"),
    ApiSpec("GetCurrentThreadId", "kernel32", "当前线程 TID", ret="int"),
    ApiSpec("GetTickCount64", "kernel32", "系统启动以来的毫秒数", ret="int"),
    ApiSpec("GetCommandLineW", "kernel32", "服务器进程的命令行", ret="str"),
    ApiSpec("GetCurrentDirectoryW", "kernel32", "当前工作目录（文本在出参 out.text）",
            params=(
                Param("n", "int", "", 0, True, True),
                Param("out", "buf", "缓冲区", 1024),
            ), ret="int"),
    ApiSpec("GetComputerNameW", "kernel32", "计算机名",
            params=(
                Param("out", "buf", "缓冲区", 256),
                Param("n", "outint", "长度指针（LPDWORD）", 256),
            ), ret="bool"),
    ApiSpec("GlobalMemoryStatusEx", "kernel32", "物理/虚拟内存状态",
            params=(Param("st", "out:MEMORYSTATUSEX", "内存状态"),),
            ret="bool", note="出参包含 ullTotalPhys / ullAvailPhys 等"),
    # ---- kernel32: file / handle
    ApiSpec("GetFileAttributesW", "kernel32", "文件属性位",
            params=(Param("path", "str", "完整路径"),), ret="int",
            note="0xFFFFFFFF 表示无效（文件不存在）"),
    ApiSpec("CreateFileW", "kernel32", "打开文件/设备获取句柄", side="observable",
            params=(
                Param("path", "str", "完整路径"),
                Param("access", "hex", "GENERIC_READ=0x80000000", 0x80000000),
                Param("share", "hex", "FILE_SHARE_READ=1", 1),
                Param("sa", "ptr", "SECURITY_ATTRIBUTES，恒为 NULL"),
                Param("disposition", "int", "OPEN_EXISTING=3", 3),
                Param("flags", "hex", "FILE_ATTRIBUTE_NORMAL=0x80", 0x80),
                Param("template", "ptr", "模板句柄，恒为 NULL"),
            ), ret="handle",
            note="拿到句柄后可用 CloseHandle($N.ret) 关闭"),
    ApiSpec("CloseHandle", "kernel32", "关闭句柄", side="observable",
            params=(Param("handle", "hex", "句柄，可用 $N.ret 引用"),), ret="bool"),
    # ---- kernel32: 系统行为
    ApiSpec("Beep", "kernel32", "蜂鸣", side="observable",
            params=(Param("freq", "int", "Hz", 800), Param("dur", "int", "ms，≤3000", 300)),
            ret="bool"),
    ApiSpec("Sleep", "kernel32", "阻塞当前线程", side="observable",
            params=(Param("ms", "int", "≤5000", 1000),), ret="void"),
    # ---- user32
    ApiSpec("MessageBoxW", "user32", "弹窗（需人工点掉，阻塞队列）", side="observable",
            params=(
                Param("hwnd", "hex", "父窗口，通常 0", 0),
                Param("text", "str", "内容"),
                Param("caption", "str", "标题", "NTLab"),
                Param("type", "hex", "MB_OK=0 / MB_OKCANCEL=1 / MB_ICONINFORMATION=0x40", 0),
            ), ret="int"),
    ApiSpec("GetCursorPos", "user32", "鼠标屏幕坐标",
            params=(Param("pt", "out:POINT", "鼠标坐标"),), ret="bool"),
    ApiSpec("SetCursorPos", "user32", "移动鼠标指针", side="observable",
            params=(Param("x", "int", ""), Param("y", "int", "")), ret="bool"),
    ApiSpec("GetSystemMetrics", "user32", "系统度量（0=屏宽 1=屏高 …）",
            params=(Param("index", "int", "SM_CXSCREEN=0"),), ret="int"),
    ApiSpec("GetForegroundWindow", "user32", "前台窗口句柄", ret="handle"),
    ApiSpec("FindWindowW", "user32", "按类名/标题找窗口",
            params=(
                Param("cls", "str", "窗口类名，可为 null", None, True),
                Param("title", "str", "窗口标题，可为 null", None, True),
            ), ret="handle"),
    ApiSpec("GetWindowTextW", "user32", "窗口标题（文本在出参 out.text）",
            params=(
                Param("hwnd", "hex", "窗口句柄"),
                Param("out", "buf", "缓冲区", 256),
                Param("n", "int", "", 0, True, True),
            ), ret="int"),
    ApiSpec("GetWindowRect", "user32", "窗口矩形",
            params=(Param("rect", "out:RECT", "矩形"),), ret="bool"),
    ApiSpec("GetWindowThreadProcessId", "user32", "窗口所属线程/进程",
            params=(Param("hwnd", "hex", "窗口句柄"),), ret="int"),
    # ---- ntdll
    ApiSpec("RtlGetVersion", "ntdll", "真实系统版本",
            params=(Param("ver", "out:OSVERSIONINFOW", "版本信息"),), ret="NTSTATUS"),
    ApiSpec("NtQuerySystemInformation", "ntdll", "系统信息（SystemBasicInformation=0 等）",
            params=(
                Param("cls", "int", "SYSTEMINFORMATIONCLASS"),
                Param("buf", "bin", "输出缓冲区字节数 ≤65536", 4096),
                Param("len", "int", "", 0, True, True),
                Param("retlen", "ptr", "ReturnLength 指针，恒为 NULL"),
            ), ret="NTSTATUS", note="出参 buf 为原始字节 hex（前 256 B）"),
    ApiSpec("RtlNtStatusToDosError", "ntdll", "NTSTATUS → Win32 错误码",
            params=(Param("status", "hex", "如 0xC0000005"),), ret="int"),
)

_CATALOG_BY_NAME = {s.name: s for s in _CATALOG}

_SESSIONS: list[dict] = []
_SESSIONS_MAX = 12

_LIBS: dict[str, ctypes.WinDLL] = {}


def catalog() -> dict:
    return {
        "apis": [
            {
                "name": s.name, "module": s.module, "desc": s.desc,
                "side": s.side, "ret": s.ret, "note": s.note,
                "params": [
                    {"name": p.name, "type": p.type, "desc": p.desc,
                     "default": p.default, "optional": p.optional}
                    for p in s.params
                ],
            }
            for s in _CATALOG
        ],
    }


def callable_names() -> set[str]:
    return set(_CATALOG_BY_NAME)


# ---------------------------------------------------------------- runner

def _lib(module: str) -> ctypes.WinDLL:
    if module not in _LIBS:
        _LIBS[module] = ctypes.WinDLL(module, use_last_error=True)
    return _LIBS[module]


def _is_missing(v: Any) -> bool:
    return v is None or (isinstance(v, str) and v.strip() == "")


def _resolve_ref(v: Any, refs: list[dict]) -> Any:
    """"$N" / "$N.ret" / "$N.out.x" → 前面步骤的原始值（refs 是运行期视图）。"""
    if not (isinstance(v, str) and v.startswith("$")):
        return v
    parts = v[1:].split(".")
    try:
        idx = int(parts[0]) - 1
    except ValueError:
        raise NtlabError("bad_ref", f"invalid reference {v!r} (use $<step index>)")
    if idx < 0 or idx >= len(refs):
        raise NtlabError("bad_ref", f"reference {v!r} points to step {idx + 1}, which has not run yet")
    node: Any = refs[idx]
    for key in parts[1:]:
        if isinstance(node, dict) and key in node:
            node = node[key]
        else:
            raise NtlabError("bad_ref", f"reference {v!r}: no such key {key!r}")
    # 终点解包：__raw__ 包装 → 原始值；ctypes 简单类型 → .value
    if isinstance(node, dict) and "__raw__" in node:
        node = node["__raw__"]
    elif hasattr(node, "value") and not isinstance(node, (int, str, bytes, bool)):
        node = node.value
    return node


def _convert(p: Param, v: Any, refs: list[dict]) -> Any:
    v = _resolve_ref(v, refs)
    if p.type.startswith("out:"):
        st = _OUT_STRUCTS[p.type[4:]]()
        if hasattr(st, "dwLength"):
            st.dwLength = ctypes.sizeof(st)
        if hasattr(st, "dwOSVersionInfoSize"):
            st.dwOSVersionInfoSize = ctypes.sizeof(st)
        return st
    if p.type == "buf":
        n = max(8, min(int(v) if v is not None else 512, 65536))
        return ctypes.create_unicode_buffer(n)
    if p.type == "bin":
        n = max(16, min(int(v) if v is not None else 4096, 65536))
        return ctypes.create_string_buffer(n)
    if p.type == "outint":
        return wintypes.DWORD(int(v) if v is not None and not _is_missing(v) else 0)
    if p.type == "ptr":
        return None  # 恒为 NULL 的指针参数
    if _is_missing(v):
        if p.optional:
            return None  # 可选参数 → NULL / 0
        raise NtlabError("missing_param", f"parameter {p.name!r} is required")
    if p.type == "int":
        try:
            return int(v, 0) if isinstance(v, str) else int(v)
        except (TypeError, ValueError):
            raise NtlabError("bad_param", f"parameter {p.name!r} expects an integer, got {v!r}")
    if p.type == "hex":
        try:
            n = int(v, 0) if isinstance(v, str) else int(v)
        except (TypeError, ValueError):
            raise NtlabError("bad_param", f"parameter {p.name!r} expects hex/decimal, got {v!r}")
        if n < 0:
            n &= (1 << 64) - 1
        return ctypes.c_ulonglong(n)
    if p.type == "str":
        return str(v)
    if p.type == "bool":
        return bool(v)
    raise NtlabError("bad_param", f"unsupported param type {p.type!r}")


def _struct_to_dict(st: ctypes.Structure) -> dict:
    out = {}
    for fname, ftype in st._fields_:
        val = getattr(st, fname)
        if isinstance(val, ctypes.Structure):
            out[fname] = _struct_to_dict(val)
        elif isinstance(ftype, ctypes.Array):
            out[fname] = str(val)
        else:
            out[fname] = int(val) if isinstance(val, int) else val
    return out


def _restype_of(spec: ApiSpec):
    return {
        "int": ctypes.c_int,
        "bool": ctypes.c_int,
        "handle": ctypes.c_void_p,
        "str": ctypes.c_wchar_p,
        "NTSTATUS": ctypes.c_uint32,
        "void": None,
    }[spec.ret]


def _ntstatus_info(status: int) -> dict:
    """NTSTATUS → {code, dos, message}（借 ntdll/kernel32 自己解码）。"""
    info: dict[str, Any] = {"code": f"0x{status & 0xFFFFFFFF:08X}"}
    try:
        dos = _lib("ntdll").RtlNtStatusToDosError(ctypes.c_uint32(status))
        info["dos"] = dos
        msg = ctypes.create_unicode_buffer(512)
        n = _lib("kernel32").FormatMessageW(
            0x1000, None, dos, 0, msg, len(msg), None)  # FORMAT_MESSAGE_FROM_SYSTEM
        info["message"] = msg.value.strip() if n else None
    except Exception:
        pass
    return info


def _render_ret(spec: ApiSpec, ret: Any) -> Any:
    if spec.ret in ("int", "bool"):
        return {"value": int(ret), "hex": f"0x{int(ret) & 0xFFFFFFFF:X}"}
    if spec.ret == "handle":
        v = int(ret) if ret else 0
        return {"value": v, "hex": f"0x{v:X}", "valid": v != 0}
    if spec.ret == "NTSTATUS":
        st = int(ret) & 0xFFFFFFFF
        return {"value": st, "hex": f"0x{st:08X}", "success": st < 0x80000000,
                "ntstatus": _ntstatus_info(st)}
    return ret


def _ref_out_view(val: Any) -> Any:
    """出参 → 可引用树（结构体字段 / 缓冲区文本 / DWORD 原始值）。"""
    if isinstance(val, ctypes.Structure):
        return _struct_to_dict(val)
    if isinstance(val, ctypes.Array) and val._type_ is ctypes.c_wchar:
        return {"text": val.value, "len_chars": len(val.value)}
    if isinstance(val, ctypes.Array):
        raw = bytes(val)[:256]
        return {"hex": raw.hex(" "), "size": len(raw)}
    if hasattr(val, "value"):
        return {"__raw__": int(val.value)}
    return {"__raw__": val}


def run_call(api: str, args: dict[str, Any], refs: list[dict]) -> dict:
    spec = _CATALOG_BY_NAME.get(api)
    if spec is None:
        raise NtlabError("not_callable", f"{api!r} is not in the whitelist; see /api/winapi/catalog")
    fn = getattr(_lib(spec.module), spec.name)
    fn.restype = _restype_of(spec)

    supplied = dict(args or {})
    sent: dict[str, Any] = {}
    out_names: list[str] = []
    vals: dict[str, Any] = {}
    # 第一遍：转换可见参数
    for p in spec.params:
        if p.hidden:
            continue
        v = supplied.get(p.name, p.default)
        if p.type.startswith("out:") or p.type in ("outint", "buf", "bin"):
            out_names.append(p.name)
        vals[p.name] = _convert(p, v, refs)
        sent[p.name] = v
    # 第二遍：hidden 参数（缓冲区长度等）由运行器供给
    for p in spec.params:
        if p.hidden:
            buf_len = next((len(vals[q.name]) for q in spec.params
                            if q.type in ("buf", "bin") and q.name in vals), 0)
            vals[p.name] = buf_len
    cargs: list[Any] = []
    for p in spec.params:
        val = vals[p.name]
        # 结构体/DWORD 出参必须 byref 传递（直传会按值拷贝，API 收到无效指针/数据）
        if p.type.startswith("out:") or p.type == "outint":
            val = ctypes.byref(val)
        cargs.append(val)

    t0 = time.perf_counter()
    try:
        ret = fn(*cargs)
    except OSError as exc:
        refs.append({"ret": {"__raw__": 0}, "out": {}})  # 保持步号对齐
        return {"api": api, "module": spec.module, "args": sent,
                "error": f"OSError: {exc}", "duration_ms": round((time.perf_counter() - t0) * 1000, 2)}
    step: dict[str, Any] = {
        "api": api, "module": spec.module, "side": spec.side, "args": sent,
        "duration_ms": round((time.perf_counter() - t0) * 1000, 2),
    }
    step["ret"] = _render_ret(spec, ret)
    if spec.ret == "bool":
        step["ret"]["last_error"] = ctypes.get_last_error()
    out = {}
    for name in out_names:
        val = vals[name]
        if isinstance(val, ctypes.Structure):
            out[name] = _struct_to_dict(val)
        elif isinstance(val, ctypes.Array) and val._type_ is ctypes.c_wchar:
            out[name] = {"text": val.value, "len_chars": len(val.value)}
        elif isinstance(val, ctypes.Array):
            raw = bytes(val)[:256]
            out[name] = {"hex": raw.hex(" "), "size": len(raw)}
        else:  # outint → DWORD 实例
            out[name] = {"value": int(val.value)}
    if out:
        step["out"] = out
    # 运行期引用视图：ret = 渲染值 + __raw__（$N.ret 解包原始值，$N.ret.value 取渲染字段）
    rendered = _render_ret(spec, ret)
    entry_ret = rendered if isinstance(rendered, dict) else {"text": str(rendered)}
    entry_ret["__raw__"] = int(ret) if ret is not None else 0
    refs.append({
        "ret": entry_ret,
        "out": {name: _ref_out_view(vals[name]) for name in out_names},
    })
    return step


def run_queue(calls: list[dict]) -> dict:
    """按顺序执行调用队列；单步失败记录 error 并继续。"""
    if not isinstance(calls, list) or not calls:
        raise NtlabError("empty_queue", "queue must be a non-empty list of {api, args}")
    if len(calls) > 64:
        raise NtlabError("queue_too_long", "queue is limited to 64 steps")
    results: list[dict] = []
    refs: list[dict] = []
    for i, call in enumerate(calls):
        if not isinstance(call, dict) or "api" not in call:
            raise NtlabError("bad_queue", f"step {i + 1} must be an object with an 'api' field")
        try:
            results.append(run_call(call["api"], call.get("args") or {}, refs))
        except NtlabError as exc:
            results.append({"api": call.get("api"), "error": exc.message, "code": exc.code})
            refs.append({})  # 保持步号对齐
    session = {
        "id": uuid.uuid4().hex[:12],
        "ts": time.time(),
        "steps": results,
        "ok": sum(1 for r in results if "error" not in r),
        "failed": sum(1 for r in results if "error" in r),
    }
    _SESSIONS.append(session)
    while len(_SESSIONS) > _SESSIONS_MAX:
        _SESSIONS.pop(0)
    return session


def sessions() -> list[dict]:
    return list(reversed(_SESSIONS))


# ---------------------------------------------------------------- program runner（编排：顺序/选择/循环/跳转）

class _BreakSignal(Exception):
    pass


class _ContinueSignal(Exception):
    pass


# 表达式求值：ast 白名单，杜绝函数调用/属性访问/导入等任意代码
_EXPR_ALLOWED = (
    ast.Expression, ast.BoolOp, ast.And, ast.Or,
    ast.UnaryOp, ast.Not, ast.USub, ast.UAdd,
    ast.Compare, ast.Eq, ast.NotEq, ast.Lt, ast.LtE, ast.Gt, ast.GtE,
    ast.BinOp, ast.Add, ast.Sub, ast.Mult, ast.Div, ast.FloorDiv, ast.Mod,
    ast.Name, ast.Load, ast.Constant,
)

_REF_EXPR_RE = re.compile(r"\$([0-9]+)((?:\.[A-Za-z_][A-Za-z0-9_]*)*)")

_MAX_PROGRAM_STEPS = 2000
_MAX_REPEAT = 10000


def _eval_expr(code: str, names: dict, what: str) -> Any:
    try:
        tree = ast.parse(code, mode="eval")
    except SyntaxError as exc:
        raise NtlabError("bad_expr", f"{what}语法错误: {exc}") from exc
    for node in ast.walk(tree):
        if not isinstance(node, _EXPR_ALLOWED):
            raise NtlabError("bad_expr", f"{what}包含不允许的语法 {type(node).__name__}")
    try:
        return eval(compile(tree, "<expr>", "eval"), {"__builtins__": {}}, dict(names))
    except NtlabError:
        raise
    except Exception as exc:
        raise NtlabError("bad_expr", f"{what}求值失败: {exc}") from exc


def _bind_expr_refs(expr: str, refs: list[dict]) -> tuple[str, dict]:
    """把 $N.path 引用预先求值并绑定到合法标识符，返回 (改写后的表达式, 变量表)。"""
    names: dict[str, Any] = {}

    def repl(m: re.Match) -> str:
        val = _resolve_ref(m.group(0), refs)
        var = "ref_" + m.group(1) + (m.group(2).replace(".", "_") if m.group(2) else "")
        names[var] = val
        return var

    e = _REF_EXPR_RE.sub(repl, expr)
    # C 风格逻辑运算符 → Python 关键字（不支持 C 的 ! 取反，请用 not）
    e = re.sub(r"&&", " and ", e)
    e = re.sub(r"\|\|", " or ", e)
    return e, names


def _spend(budget: list[int]) -> None:
    budget[0] -= 1
    if budget[0] < 0:
        raise NtlabError(
            "budget_exceeded",
            f"程序执行步数超过 {_MAX_PROGRAM_STEPS} 上限（循环失控？）",
        )


def _exec_call_node(node: dict, refs: list[dict], trace: list, where: str) -> None:
    api = node.get("api")
    slot = int(node.get("_slot", 0))
    try:
        step = run_call(api, node.get("args") or {}, refs)
        step["where"] = where
        trace.append(step)
    except NtlabError as exc:
        refs.append({"ret": {"__raw__": 0}, "out": {}})
        trace.append({"api": api, "error": exc.message, "code": exc.code, "where": where})
    # refs 对齐到该 call 块的静态槽位（循环多轮时覆盖旧结果）
    entry = refs.pop() if refs else {"ret": {"__raw__": 0}, "out": {}}
    while len(refs) < slot:
        refs.append({"ret": {"__raw__": 0}, "out": {}})
    if slot == len(refs):
        refs.append(entry)
    else:
        refs[slot] = entry


def _run_nodes(nodes: Any, refs: list[dict], trace: list, budget: list[int], where: str) -> None:
    if not isinstance(nodes, list):
        return
    for i, node in enumerate(nodes):
        _spend(budget)
        if not isinstance(node, dict):
            raise NtlabError("bad_program", "program 块必须是对象")
        kind = node.get("kind", "call")
        here = f"{where}/{i + 1}"
        if kind == "call":
            _exec_call_node(node, refs, trace, here)
        elif kind == "repeat":
            _run_repeat(node, refs, trace, budget, here)
        elif kind == "if":
            _run_if(node, refs, trace, budget, here)
        elif kind == "break":
            raise _BreakSignal()
        elif kind == "continue":
            raise _ContinueSignal()
        else:
            trace.append({"api": kind, "error": f"未知块类型 {kind!r}", "code": "bad_program", "where": here})


def _run_repeat(node: dict, refs: list[dict], trace: list, budget: list[int], where: str) -> None:
    expr = str(node.get("times", 0))
    code, names = _bind_expr_refs(expr, refs)
    n = int(_eval_expr(code, names, "repeat 次数"))
    n = max(0, min(n, _MAX_REPEAT))
    body = node.get("body")
    for it in range(1, n + 1):
        _spend(budget)
        try:
            _run_nodes(body, refs, trace, budget, f"{where}×{n}[{it}]")
        except _ContinueSignal:
            continue
        except _BreakSignal:
            break


def _run_if(node: dict, refs: list[dict], trace: list, budget: list[int], where: str) -> None:
    cond = str(node.get("cond", "")).strip()
    if not cond:
        raise NtlabError("bad_expr", "if 块缺少条件表达式")
    code, names = _bind_expr_refs(cond, refs)
    truth = bool(_eval_expr(code, names, "if 条件"))
    branch = node.get("then") if truth else node.get("else")
    _run_nodes(branch, refs, trace, budget, f"{where}({'then' if truth else 'else'})")


def run_program(program: list[dict]) -> dict:
    """执行编排程序：kind = call / repeat(times, body) / if(cond, then, else) / break / continue。

    $N 引用第 N 个调用块（按程序静态顺序编号；循环多轮时引用该块最近一次的结果）。
    总执行步数受 _MAX_PROGRAM_STEPS 预算约束，防止死循环拖垮服务器。
    """
    if not isinstance(program, list) or not program:
        raise NtlabError("empty_queue", "program must be a non-empty list of blocks")
    if len(program) > 256:
        raise NtlabError("queue_too_long", "program is limited to 256 top-level blocks")

    counter = [0]

    def annotate(nodes: Any) -> None:
        if not isinstance(nodes, list):
            return
        for node in nodes:
            if not isinstance(node, dict):
                continue
            if node.get("kind", "call") == "call":
                node["_slot"] = counter[0]
                counter[0] += 1
            for key in ("body", "then", "else"):
                annotate(node.get(key))

    annotate(program)

    refs: list[dict] = []
    trace: list[dict] = []
    budget = [_MAX_PROGRAM_STEPS]
    try:
        _run_nodes(program, refs, trace, budget, "main")
    except _BreakSignal:
        trace.append({"api": "break", "error": "break 出现在循环外", "code": "bad_program"})
    except _ContinueSignal:
        trace.append({"api": "continue", "error": "continue 出现在循环外", "code": "bad_program"})
    except NtlabError as exc:
        trace.append({"api": "program", "error": exc.message, "code": exc.code})

    session = {
        "id": uuid.uuid4().hex[:12],
        "ts": time.time(),
        "steps": trace,
        "ok": sum(1 for r in trace if "error" not in r),
        "failed": sum(1 for r in trace if "error" in r),
    }
    _SESSIONS.append(session)
    while len(_SESSIONS) > _SESSIONS_MAX:
        _SESSIONS.pop(0)
    return session


# ---------------------------------------------------------------- asm (assemble only, never execute)

def assemble(text: str) -> dict:
    """x86-64 汇编 → 字节（keystone），再经 capstone 反汇编回读校验。

    只做组装与展示，绝不执行——在服务器进程里执行外来机器码等同于自杀。
    """
    if not text or not text.strip():
        raise NtlabError("empty_asm", "assembly text is empty")
    try:
        import keystone as _ks
    except ImportError:
        raise NtlabError(
            "asm_unavailable",
            "keystone-engine is not installed; run: pip install keystone-engine",
        )
    from ntlab.core.pe.disasm import _md  # reuse configured capstone instance

    ks = _ks.Ks(_ks.KS_ARCH_X86, _ks.KS_MODE_64)
    try:
        enc, _count = ks.asm(text)
    except _ks.KsError as exc:
        raise NtlabError("asm_error", f"keystone failed: {exc}")
    data = bytes(enc)
    lines = [
        {"rva": off, "size": size, "bytes": raw.hex(" "), "mnemonic": mn, "op": ops}
        for off, size, raw, mn, ops in (
            (i.address, i.size, i.bytes, i.mnemonic, i.op_str)
            for i in _md.disasm(data, 0)
        )
    ]
    return {"bytes_hex": data.hex(" "), "size": len(data), "roundtrip": lines}
