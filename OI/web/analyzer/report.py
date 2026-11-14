"""Analysis report data contract.

Field semantics follow ComfyDL_UI's comfy/profiling/engine.py (report carries
its own errors, explicit unknown, assumptions with source, peak not sum) but the
shape is reduced to what an OI C++ source needs. No web/UI concepts here.
"""
from __future__ import annotations

import dataclasses
from typing import Any

REPORT_VERSION = 1
VERDICTS = ("green", "yellow", "red", "unknown")
DIMENSIONS = ("memory", "complexity", "style", "performance")
CONFIDENCES = ("measured", "estimated", "heuristic", "unknown")

DEFAULT_ASSUMPTIONS: dict[str, int] = {
    "int_bytes": 4,
    "long_long_bytes": 8,
    "double_bytes": 8,
    "char_bytes": 1,
    "bool_bytes": 1,
    "pointer_bytes": 8,
    "stack_budget_bytes": 8 * 1024 * 1024,
    "memory_limit_bytes": 256 * 1024 * 1024,
    "default_n": 100000,
}

TYPE_SIZE_KEYS: dict[str, str] = {
    "int": "int_bytes",
    "unsigned": "int_bytes",
    "unsigned int": "int_bytes",
    "signed": "int_bytes",
    "short": "int_bytes",
    "long": "int_bytes",
    "unsigned long": "int_bytes",
    "long long": "long_long_bytes",
    "unsigned long long": "long_long_bytes",
    "double": "double_bytes",
    "long double": "double_bytes",
    "float": "int_bytes",
    "char": "char_bytes",
    "unsigned char": "char_bytes",
    "bool": "bool_bytes",
}

_RANK = {"red": 3, "yellow": 2, "green": 1, "unknown": 0}


class AssumptionSet:
    """Only fills genuine holes; overrides are user-supplied exact values."""

    def __init__(self, overrides: dict | None = None):
        self.overrides = {
            k: v for k, v in (overrides or {}).items()
            if k in DEFAULT_ASSUMPTIONS and isinstance(v, (int, float))
        }
        self.used: dict[str, tuple[Any, str]] = {}

    def get(self, key: str):
        if key in self.overrides:
            value, source = self.overrides[key], "override"
        else:
            value, source = DEFAULT_ASSUMPTIONS[key], "default"
        self.used[key] = (value, source)
        return value

    def entries(self) -> list[dict]:
        return [
            {"key": k, "value": v, "source": s}
            for k, (v, s) in sorted(self.used.items())
        ]

    def canonical(self) -> dict:
        return dict(sorted(self.overrides.items()))


@dataclasses.dataclass(frozen=True)
class Finding:
    dimension: str
    key: str
    verdict: str
    value: Any
    unit: str
    evidence: tuple
    reason: str
    confidence: str

    def as_dict(self) -> dict:
        return {
            "dimension": self.dimension,
            "key": self.key,
            "verdict": self.verdict,
            "value": self.value,
            "unit": self.unit,
            "evidence": list(self.evidence),
            "reason": self.reason,
            "confidence": self.confidence,
        }


@dataclasses.dataclass
class AnalysisReport:
    version: int
    digest: str
    source: dict
    verdict: str
    verdict_reason: str
    findings: list
    assumptions: list
    measurements: dict
    ai_summary: dict
    unknown_count: int
    error: str | None = None

    def as_dict(self) -> dict:
        return {
            "version": self.version,
            "digest": self.digest,
            "source": self.source,
            "verdict": self.verdict,
            "verdict_reason": self.verdict_reason,
            "findings": [f.as_dict() if isinstance(f, Finding) else f for f in self.findings],
            "assumptions": self.assumptions,
            "measurements": self.measurements,
            "ai_summary": self.ai_summary,
            "unknown_count": self.unknown_count,
            "error": self.error,
        }


def aggregate(findings: list[Finding]) -> tuple[str, str]:
    """Worst non-unknown wins; unknown only dominates when nothing is known."""
    if not findings:
        return "unknown", "empty"
    known = [f for f in findings if f.verdict in ("green", "yellow", "red")]
    if not known:
        return "unknown", "nothing_estimated"
    worst = max(known, key=lambda f: _RANK[f.verdict])
    return worst.verdict, worst.key if worst.verdict != "green" else "all_clear"


def human_bytes(value: Any) -> str:
    if not isinstance(value, (int, float)) or value < 0:
        return "unknown"
    units = ("B", "KiB", "MiB", "GiB")
    size = float(value)
    for unit in units:
        if size < 1024 or unit == units[-1]:
            return f"{size:.0f} {unit}" if unit == "B" else f"{size:.2f} {unit}"
        size /= 1024
    return "unknown"


def finding(dimension, key, verdict, value, unit, evidence, reason, confidence) -> Finding:
    return Finding(
        dimension=dimension,
        key=key,
        verdict=verdict if verdict in VERDICTS else "unknown",
        value=value,
        unit=unit,
        evidence=tuple(sorted(set(int(e) for e in evidence if isinstance(e, int) and e > 0))),
        reason=reason,
        confidence=confidence if confidence in CONFIDENCES else "unknown",
    )
