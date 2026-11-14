"""Local analysis engine: aggregation, signature cache, workspace enumeration.

The engine never raises on malformed input: failures become an `error` field
and unknown Findings, matching ComfyDL_UI comfy/profiling/engine.py:25-26.
"""
from __future__ import annotations

import hashlib
import json
import os
import threading
from collections import OrderedDict
from pathlib import Path

from . import report as R
from .estimators import run_all
from .lexer import lex

ROOT_DIR = Path(__file__).resolve().parents[2]
CACHE_LIMIT = 8
_CACHE: OrderedDict[str, R.AnalysisReport] = OrderedDict()
_CACHE_LOCK = threading.Lock()
MAX_CODE_BYTES = 2 * 1024 * 1024


def list_cpp_files(root: Path | None = None, limit: int = 300) -> list[dict]:
    base = Path(root or ROOT_DIR)
    items = []
    for path in sorted(base.glob("*.cpp")):
        try:
            stat = path.stat()
        except OSError:
            continue
        items.append({
            "name": path.name,
            "path": path.name,
            "bytes": stat.st_size,
            "mtime": int(stat.st_mtime),
        })
        if len(items) >= limit:
            break
    return items


def resolve_code(body: dict) -> tuple[str, dict]:
    """Accept inline code or a workspace-relative .cpp path."""
    inline = body.get("code")
    if isinstance(inline, str) and inline.strip():
        return inline, {"kind": "output", "name": "当前输出"}

    name = str(body.get("path") or "").strip()
    if not name:
        raise ValueError("需要 code 或 path")
    path = Path(name)
    if not path.is_absolute():
        path = ROOT_DIR / path
    path = path.resolve()
    try:
        if os.path.commonpath([str(path), str(ROOT_DIR.resolve())]) != str(ROOT_DIR.resolve()):
            raise ValueError("路径超出工作区")
    except ValueError as exc:
        raise ValueError("路径超出工作区") from exc
    if path.suffix.lower() != ".cpp" or not path.is_file():
        raise ValueError("不是工作区内的 .cpp 文件")
    if path.stat().st_size > MAX_CODE_BYTES:
        raise ValueError("文件超过 2 MB")
    return path.read_text(encoding="utf-8", errors="replace"), {
        "kind": "file", "path": path.name, "name": path.name,
    }


def signature(code: str, assumptions: dict | None, run: bool, stdin: str) -> str:
    payload = {
        "v": R.REPORT_VERSION,
        "code": hashlib.sha256(code.encode("utf-8", "replace")).hexdigest(),
        "assumptions": dict(sorted((assumptions or {}).items())),
        "run": bool(run),
        "stdin": hashlib.sha256(stdin.encode("utf-8", "replace")).hexdigest(),
    }
    return hashlib.sha256(json.dumps(payload, sort_keys=True, separators=(",", ":")).encode()).hexdigest()


def analyse(code: str, source: dict, assumptions: dict | None = None,
            run: bool = False, stdin: str = "") -> tuple[R.AnalysisReport, bool]:
    """Return (report, cache_hit). Reports carrying an error are not cached."""
    key = signature(code, assumptions, run, stdin)
    with _CACHE_LOCK:
        cached = _CACHE.get(key)
        if cached is not None:
            _CACHE.move_to_end(key)
            return cached, True

    ass = R.AssumptionSet(assumptions)
    error = None
    try:
        lexed = lex(code)
        findings = run_all(lexed, ass)
    except Exception as exc:
        error = f"lexer failure: {type(exc).__name__}"
        findings = [R.finding("memory", "engine.error", "unknown", None, "", (), error, "unknown")]

    verdict, reason = R.aggregate(findings)
    rep = R.AnalysisReport(
        version=R.REPORT_VERSION,
        digest=hashlib.sha256(code.encode("utf-8", "replace")).hexdigest(),
        source=source,
        verdict=verdict,
        verdict_reason=reason,
        findings=findings,
        assumptions=ass.entries(),
        measurements={"status": "skipped", "reason": "run not requested"},
        ai_summary={"status": "pending", "text": "", "budget_used": 0, "budget_cap": 0},
        unknown_count=sum(1 for f in findings if f.verdict == "unknown"),
        error=error,
    )
    if error is None:
        with _CACHE_LOCK:
            _CACHE[key] = rep
            _CACHE.move_to_end(key)
            while len(_CACHE) > CACHE_LIMIT:
                _CACHE.popitem(last=False)
    return rep, False


def clear_cache() -> int:
    with _CACHE_LOCK:
        count = len(_CACHE)
        _CACHE.clear()
    return count


def cache_size() -> int:
    with _CACHE_LOCK:
        return len(_CACHE)
