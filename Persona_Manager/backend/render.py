"""Rendering of template variables and assembly of the final system prefix.

Generalised from Coopanion's prompt prefix pipeline. A template references
variables with ``{{ name }}`` or ``{{ name | fallback }}``. The rendered value
is the first of: a provided value, the inline fallback, or the token left as-is
(so a missing value is visible and never silently dropped).
"""
import re
from datetime import date

# {{ name }} or {{ name | fallback with spaces }}
REF_RE = re.compile(r"\{\{\s*([\w.\-]+)\s*(?:\|([^}]*))?\}\}")


def references(text: str):
    """List every variable reference in ``text`` with its span."""
    out = []
    for m in REF_RE.finditer(text):
        out.append({
            "name": m.group(1),
            "fallback": (m.group(2) or "").strip() or None,
            "token": m.group(0),
            "from": m.start(),
            "to": m.end(),
        })
    return out


def render(text: str, values: dict) -> str:
    """Replace every ``{{ name | fallback }}`` in ``text`` using ``values``."""
    def sub(m: re.Match) -> str:
        name = m.group(1)
        fallback = (m.group(2) or "").strip()
        if name in values and values[name] not in (None, ""):
            return str(values[name])
        if fallback:
            return fallback
        return m.group(0)          # keep the token so the gap is visible
    return REF_RE.sub(sub, text)


def used_variables(text: str) -> list[str]:
    """Names referenced in ``text``, de-duplicated, in order of appearance."""
    seen, out = set(), []
    for ref in references(text):
        if ref["name"] not in seen:
            seen.add(ref["name"])
            out.append(ref["name"])
    return out


def builtin_values() -> dict:
    """Values that are always available without user configuration."""
    return {
        "date": date.today().isoformat(),
        "year": str(date.today().year),
    }
