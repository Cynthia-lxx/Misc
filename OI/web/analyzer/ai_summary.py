"""One short, single-turn AI summary per analysis, under a hard budget.

Deliberately decoupled from oi.history: no prior messages, no code re-sending,
no writes to the shared CLI history. Any failure degrades to a complete local
report (ai_summary.status describes why).
"""
from __future__ import annotations

import hashlib
import json
import threading
from collections import OrderedDict

from openai import OpenAI

EQUIVALENT_PER_SUMMARY = 0.1  # one summary ~= 0.1 of a full code generation
DEFAULT_CAP = 2.0             # conservative end of the user's 2..10 range
MAX_CAP = 10.0
MAX_TOKENS = 512
MAX_SUMMARY_CHARS = 1400
MAX_BRIEF_CHARS = 3200
REQUEST_TIMEOUT = 20.0

SYSTEM = (
    "你是 OI C++ 代码诊断助手。根据本地静态分析的结构化摘要，用中文给出 3 到 5 条"
    "最值得优先处理的建议，每条一行、不超过 40 字，按影响排序。不要复述指标，"
    "不要写代码，不要客套。标为 unknown 的项不得给出确定结论。"
)


class Budget:
    def __init__(self):
        self._lock = threading.RLock()  # re-entrant: set_enabled/reserve call state() while held
        self.enabled = True
        self.cap = DEFAULT_CAP
        self.used = 0.0
        self.calls = 0
        self.seen: OrderedDict[str, dict] = OrderedDict()

    def set_enabled(self, value: bool) -> dict:
        with self._lock:
            self.enabled = bool(value)
            return self.state()

    def set_cap(self, value) -> dict:
        with self._lock:
            self.cap = max(DEFAULT_CAP, min(float(value or DEFAULT_CAP), MAX_CAP))
            return self.state()

    def state(self) -> dict:
        with self._lock:
            return {
                "enabled": self.enabled,
                "used": round(self.used, 3),
                "cap": round(self.cap, 3),
                "remaining": round(max(0.0, self.cap - self.used), 3),
                "unit": "code_generation_equivalent",
                "per_summary": EQUIVALENT_PER_SUMMARY,
                "calls": self.calls,
            }

    def cached(self, digest: str):
        with self._lock:
            if digest in self.seen:
                self.seen.move_to_end(digest)
                return dict(self.seen[digest])
        return None

    def reserve(self, digest: str, result: dict) -> dict:
        with self._lock:
            self.used = round(min(self.cap, self.used + EQUIVALENT_PER_SUMMARY), 3)
            self.calls += 1
            self.seen[digest] = dict(result)
            self.seen.move_to_end(digest)
            while len(self.seen) > 16:
                self.seen.popitem(last=False)
            return self.state()


BUDGET = Budget()


def _brief(report) -> str:
    data = report.as_dict()
    lines = []
    for item in data["findings"][:24]:
        value = item["value"]
        if isinstance(value, (int, float)) and value > 10000:
            value = f"{value:,}"
        lines.append(
            f"{item['dimension']}/{item['key']}: {item['verdict']}/{item['confidence']} "
            f"={value}{item['unit']} | {item['reason'][:110]}"
        )
    measurement = data.get("measurements") or {}
    brief = {
        "verdict": data["verdict"],
        "reason": data["verdict_reason"],
        "unknown": data["unknown_count"],
        "assumptions": len(data["assumptions"]),
        "measure": {
            "status": measurement.get("status"),
            "wall_ms": measurement.get("wall_ms"),
            "peak_bytes": measurement.get("peak_bytes"),
        },
        "findings": lines,
    }
    text = json.dumps(brief, ensure_ascii=False, separators=(",", ":"))
    return text[:MAX_BRIEF_CHARS]


def summarize(report, config: dict) -> dict:
    """Return {status, text, budget}; never raises."""
    state = BUDGET.state()
    digest = hashlib.sha256(
        (report.digest + _brief(report)).encode("utf-8", "replace")
    ).hexdigest()
    if not state["enabled"]:
        return {"status": "disabled", "text": "", "budget": state}
    cached = BUDGET.cached(digest)
    if cached:
        cached["status"] = "cached"
        cached["budget"] = BUDGET.state()
        return cached
    if state["remaining"] < EQUIVALENT_PER_SUMMARY:
        return {"status": "budget_exhausted", "text": "", "budget": state}
    api_key = config.get("api_key")
    if not api_key:
        return {"status": "skipped", "reason": "no_api_key", "text": "", "budget": state}

    try:
        client = OpenAI(
            api_key=api_key,
            base_url=config.get("base_url"),
            timeout=REQUEST_TIMEOUT,
            max_retries=0,  # single-shot by design; retries would stretch the wait
        )
        response = client.chat.completions.create(
            model=config.get("model") or "deepseek-chat",
            messages=[
                {"role": "system", "content": SYSTEM},
                {"role": "user", "content": _brief(report)},
            ],
            max_tokens=MAX_TOKENS,
            temperature=0.2,
            stream=False,
        )
        text = (response.choices[0].message.content or "").strip()[:MAX_SUMMARY_CHARS]
        if not text:
            return {"status": "failed", "reason": "empty response", "text": "", "budget": state}
        result = {"status": "ok", "text": text}
        budget = BUDGET.reserve(digest, result)
        return {**result, "budget": budget}
    except Exception as exc:
        return {
            "status": "failed", "reason": type(exc).__name__,
            "text": "", "budget": state,
        }


def set_enabled(value: bool) -> dict:
    return BUDGET.set_enabled(value)


def set_cap(value) -> dict:
    return BUDGET.set_cap(value)


def state() -> dict:
    return BUDGET.state()
