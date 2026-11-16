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
    "module_list": [],      # 轻量模块清单（扫描中增量追加）
    "by_name": {},          # name(lower) -> [(module, export), ...]
    "total_exports": 0,     # 运行总量（增量维护，避免每次响应全量求和）
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
        _STATE.update(status="scanning", done=0, total=len(files), error=None,
                      modules={}, module_list=[], by_name={}, total_exports=0)

    # 增量提交：每解析出一个 DLL 立即进入 _STATE，前端随到随显示
    for i, path in enumerate(files):
        try:
            pe = pefile.PE(str(path), fast_load=True)
            try:
                # fast_load 下 DATA_DIRECTORY 已可读：无导出目录的文件直接跳过，避免昂贵解析
                dd = pe.OPTIONAL_HEADER.DATA_DIRECTORY
                exp_idx = pefile.DIRECTORY_ENTRY["IMAGE_DIRECTORY_ENTRY_EXPORT"]
                if dd and len(dd) > exp_idx and dd[exp_idx].VirtualAddress:
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
                        module = {
                            "name": stem, "path": str(path),
                            "exports": sorted(items, key=lambda x: x["name"].lower()),
                            "count": len(items),
                            "dll_name": exp.name.decode("utf-8", "replace") if exp and exp.name else None,
                        }
                        with _LOCK:
                            _STATE["modules"][stem] = module
                            _STATE["module_list"].append(
                                {"name": stem, "path": str(path), "count": len(items),
                                 "dll_name": module["dll_name"]})
                            _STATE["total_exports"] += len(items)
                            for item in items:
                                _STATE["by_name"].setdefault(
                                    item["name"].lower(), []).append((stem, item))
            finally:
                pe.close()
        except Exception:
            pass  # 单个不可读 PE 不影响整库
        with _LOCK:
            _STATE["done"] = i + 1
    with _LOCK:
        _STATE["module_list"].sort(key=lambda x: x["name"])
        _STATE["status"] = "ready"
        _STATE["built_at"] = time.time()
        _STATE["done"] = len(files)
        modules_snapshot = dict(_STATE["modules"])
        built_at = _STATE["built_at"]
    try:
        from ntlab.core import userdb
        userdb.save_library(modules_snapshot, built_at)   # 缓存整库，重启免扫描
    except Exception:
        pass  # 缓存失败不影响库可用性


def start_scan(force: bool = False) -> dict:
    """idle 时优先从用户数据库载入缓存；无缓存或 force 才启动后台扫描。

    注意：_LOCK 不可重入——锁内一律调用 _status_locked()，不得调用 status()。
    """
    global _WORKER
    with _LOCK:
        if _STATE["status"] == "scanning":
            return _status_locked()
        if _STATE["status"] == "ready" and not force:
            return _status_locked()
        if _STATE["status"] == "idle" and not force and _load_cached_library():
            return _status_locked()
        _STATE.update(status="scanning", done=0, total=0, error=None)
        _WORKER = threading.Thread(target=_scan, name="ntlab-pe-library-scan", daemon=True)
        _WORKER.start()
    return _status_locked()


def _load_cached_library() -> bool:
    """从 user.db 载入导出库缓存到 _STATE；成功返回 True。"""
    try:
        from ntlab.core import userdb
        modules = userdb.load_library()
    except Exception:
        return False
    if not modules:
        return False
    by_name: dict[str, list] = {}
    module_list = []
    total = 0
    for name, m in modules.items():
        module_list.append({"name": name, "path": m["path"], "count": m["count"],
                            "dll_name": m.get("dll_name")})
        total += m["count"]
        for item in m["exports"]:
            by_name.setdefault(item["name"].lower(), []).append((name, item))
    module_list.sort(key=lambda x: x["name"])
    built = 0.0
    try:
        from ntlab.core import userdb
        built = userdb.library_built_at() or 0.0
    except Exception:
        pass
    _STATE.update(modules=modules, module_list=module_list, by_name=by_name,
                  total_exports=total, status="ready", built_at=built,
                  done=len(module_list), total=len(module_list))
    return True


def status(since: int = 0) -> dict:
    """库状态。`since` 为客户端已收到的模块数，返回其后的增量清单（分页拉取）。"""
    with _LOCK:
        return _status_locked(since)


def _status_locked(since: int = 0) -> dict:
    """status() 的持锁版本（调用方必须已持有 _LOCK）。"""
    ml = _STATE["module_list"]
    return {
        "status": _STATE["status"],
        "progress": {"done": _STATE["done"], "total": _STATE["total"]},
        "total_modules": len(ml),
        "modules": ml[max(0, since):],
        "total_exports": _STATE["total_exports"],
        "built_at": _STATE["built_at"],
        "error": _STATE["error"],
    }


def module_exports(name: str, offset: int = 0, limit: int = 0) -> dict:
    """模块导出分页：offset 起、limit 条（0 = 全部）。"""
    with _LOCK:
        m = _STATE["modules"].get((name or "").lower())
        if m is None:
            raise NtlabError("unknown_module", f"module {name!r} not found in PE library")
        exps = m["exports"]
        off = max(0, int(offset))
        chunk = exps[off:off + int(limit)] if int(limit) > 0 else exps[off:]
        return {
            "name": m["name"], "path": m["path"], "exports": chunk,
            "total": len(exps), "offset": off,
        }


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
