"""自动发现的 Windows PE 导出函数库（供 WinAPI 试验台左侧积木库使用）。

设计边界：
- 目录数据来自本机 System32 / SysWOW64 下的 DLL/EXE 导出表，后台线程扫描并缓存于进程内存。
- 信任分级只影响 UI 提示；真正的硬拦截在 core/winapi.py 的动态调用入口再次执行。
- blocked 类别（写/删/进程/注册表写等）永远不会进入子进程调用。
"""

from __future__ import annotations

import os
import re
import threading
import time
from pathlib import Path
from typing import Any

from ntlab.core.errors import NtlabError

# 硬拦截：写/删除/重命名、注册表写、进程线程控制、内存写、模块加载、系统关机等。
_BLOCKED = re.compile(
    r"(?i)(?:^|_)(?:create|write|delete|remove|rename|replace|truncate|erase|clear|set|"
    r"terminate|exit|kill|suspend|resume|inject|hook|shutdown|logoff|power|"
    r"loadlibrary|freelibrary|getprocaddress|virtualprotect|virtualalloc|virtualfree|"
    r"heapalloc|heapfree|globalalloc|globalfree|localalloc|localfree|"
    r"shellexecute|winexec|system|duplicatehandle|mapview|unmapview|flushview|"
    r"openprocess|openthread|createthread|createprocess|createfile|"
    r"reg(?:set|create|delete|rename|save|restore)|"
    r"nt(?:write|set|create|delete|terminate|openprocess|openfile|mapview|"
    r"allocate|free|protect|raiseuserfault)|"
    r"zw(?:write|set|create|delete|terminate))",
)

# 半启用：非只读命名（无法从名字确认安全），UI 用暗色提示用户谨慎。
_GENERIC_OK = re.compile(r"(?i)^(?:get|query|is|enumerate|find|lookup|rtlget|rtlntdll|format|compare|wc|lstr)")

_STATE: dict[str, Any] = {
    "status": "idle",       # idle | scanning | ready
    "done": 0,
    "total": 0,
    "modules": {},          # name(lower) -> {name,path,exports,count}
    "module_list": [],      # 供 /library 直接复用的轻量模块清单
    "by_name": {},          # name(lower) -> [(module, export), ...]
    "built_at": 0.0,
    "error": None,
}
_LOCK = threading.Lock()
_WORKER: threading.Thread | None = None


def classify(name: str) -> str:
    """返回 generic（只读样命名）/ restricted（不确定）/ blocked（硬拦截）。"""
    if not name:
        return "restricted"
    if _BLOCKED.search(name):
        return "blocked"
    return "generic" if _GENERIC_OK.match(name) else "restricted"


def is_blocked(name: str) -> bool:
    return classify(name) == "blocked"


def scan_roots() -> list[Path]:
    # 只扫描与服务器进程同位数的 System32：SysWOW64 是 32 位 DLL，无法在 64 位进程加载。
    root = Path(os.environ.get("SystemRoot", r"C:\Windows"))
    return [root / "System32"]


def _scan() -> None:
    import pefile

    roots = [p for p in scan_roots() if p.is_dir()]
    files: list[Path] = []
    for root in roots:
        files.extend(root.glob("*.dll"))
        files.extend(root.glob("*.exe"))
    files = sorted({str(p).lower(): Path(p) for p in files}.values())
    with _LOCK:
        _STATE.update(status="scanning", done=0, total=len(files), error=None)

    modules: dict[str, dict] = {}
    by_name: dict[str, list] = {}
    for i, path in enumerate(files):
        try:
            pe = pefile.PE(str(path), fast_load=True)
            try:
                # fast_load 下 DATA_DIRECTORY 已可读：无导出目录的文件直接跳过，避免昂贵解析
                dd = pe.OPTIONAL_HEADER.DATA_DIRECTORY
                exp_idx = pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]
                if not dd or len(dd) <= exp_idx or not dd[exp_idx].VirtualAddress:
                    with _LOCK:
                        _STATE["done"] = i + 1
                    continue
                pe.parse_data_directories(
                    directories=[pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]])
                exp = getattr(pe, "DIRECTORY_ENTRY_EXPORT", None)
                items: list[dict] = []
                if exp is not None:
                    for sym in exp.symbols:
                        if not sym.name:
                            continue  # 纯序号导出没有可调用的名字
                        name = sym.name.decode("utf-8", "replace")
                        items.append({
                            "name": name,
                            "ordinal": sym.ordinal,
                            "rva": sym.address,
                            "trust": classify(name),
                        })
                if items:
                    stem = path.stem.lower()
                    modules[stem] = {
                        "name": stem, "path": str(path),
                        "exports": sorted(items, key=lambda x: x["name"].lower()),
                        "count": len(items),
                        "dll_name": exp.name.decode("utf-8", "replace") if exp and exp.name else None,
                    }
                    for item in items:
                        by_name.setdefault(item["name"].lower(), []).append((stem, item))
            finally:
                pe.close()
        except Exception:
            continue  # 单个不可读 PE 不影响整库
        with _LOCK:
            _STATE["done"] = i + 1
    with _LOCK:
        module_list = [
            {"name": m["name"], "path": m["path"], "count": m["count"],
             "dll_name": m["dll_name"]}
            for m in sorted(modules.values(), key=lambda x: x["name"])
        ]
        _STATE.update(modules=modules, module_list=module_list, by_name=by_name,
                      status="ready", built_at=time.time(), done=len(files))


def start_scan(force: bool = False) -> dict:
    """启动后台扫描；已在扫描中则直接返回进度。"""
    global _WORKER
    with _LOCK:
        if _STATE["status"] == "scanning":
            return status()
        if _STATE["status"] == "ready" and not force:
            return status()
        _STATE.update(status="scanning", done=0, total=0, error=None)
        _WORKER = threading.Thread(target=_scan, name="ntlab-pe-library-scan", daemon=True)
        _WORKER.start()
    return status()


def status(include_modules: bool = True) -> dict:
    with _LOCK:
        return {
            "status": _STATE["status"],
            "progress": {"done": _STATE["done"], "total": _STATE["total"]},
            "modules": _STATE["module_list"] if include_modules else [],
            "total_exports": sum(m["count"] for m in _STATE["modules"].values()),
            "built_at": _STATE["built_at"],
            "error": _STATE["error"],
        }


def module_exports(name: str) -> dict:
    with _LOCK:
        m = _STATE["modules"].get((name or "").lower())
        if m is None:
            raise NtlabError("unknown_module", f"module {name!r} not found in PE library")
        return {"name": m["name"], "path": m["path"], "exports": m["exports"]}


def find_export(name: str, module: str | None = None) -> dict | None:
    """动态调用解析：(module, name) 精确匹配优先，否则按名字查第一个。"""
    with _LOCK:
        if module:
            m = _STATE["modules"].get(module.lower())
            if m:
                for item in m["exports"]:
                    if item["name"].lower() == name.lower():
                        return {"module": m["name"], "path": m["path"], **item}
            return None
        hits = _STATE["by_name"].get(name.lower())
        if not hits:
            return None
        mod, item = hits[0]
        return {"module": mod, "path": _STATE["modules"][mod]["path"], **item}


def resolve_path(module: str) -> str:
    """worker 加载用的 DLL 绝对路径；未扫描到时退回模块名由系统解析。"""
    with _LOCK:
        m = _STATE["modules"].get((module or "").lower())
        return m["path"] if m else module
