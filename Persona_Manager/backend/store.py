"""Storage layer: built-in templates + file-based user overrides.

Generalised from Coopanion's seed / applyPersona / SHA-256 identity logic:

* The shipped manifest is the read-only baseline (``origin=builtin``).
* A user edit is written to ``data/overrides/<key>.md`` (``origin=user``).
* ``revision`` is the SHA-256 of the effective content with LF line endings.
  It powers optimistic concurrency: a save must carry the revision it was
  loaded at, otherwise the call is rejected (HTTP 409).
* A preset is applied by writing its text into the ``role`` override, exactly
  like Coopanion copies a persona into CONSTITUTION.md. Once edited, the
  template is no longer recognised as any built-in preset.
"""
import hashlib
import json
from pathlib import Path

import config
import render


class ConflictError(Exception):
    """Raised when a save is based on a stale revision."""


def _read_text(path: Path) -> str | None:
    try:
        return path.read_text(encoding="utf-8")
    except FileNotFoundError:
        return None


def _hash(text: str) -> str:
    return hashlib.sha256(text.replace("\r\n", "\n").encode("utf-8")).hexdigest()


def _manifest() -> dict:
    return json.loads(config.MANIFEST.read_text(encoding="utf-8"))


def _override_path(key: str) -> Path:
    return config.OVERRIDES / f"{key}.md"


def _config() -> dict:
    raw = config.CONFIG_FILE.read_text(encoding="utf-8") if config.CONFIG_FILE.exists() else "{}"
    try:
        return json.loads(raw)
    except json.JSONDecodeError:
        return {}


def _write_config(data: dict) -> None:
    config.ensure_data()
    config.CONFIG_FILE.write_text(json.dumps(data, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")


def _tpl_defs() -> list[dict]:
    return _manifest()["templates"]


def _tpl_def(key: str) -> dict | None:
    return next((t for t in _tpl_defs() if t["key"] == key), None)


def template_doc(key: str) -> dict | None:
    """Return the effective PromptDoc for ``key`` (override wins over built-in)."""
    definition = _tpl_def(key)
    if definition is None:
        return None
    generated = bool(definition.get("generated"))
    override = _read_text(_override_path(key)) if not generated else None
    content = override if override is not None else definition["content"]
    origin = "user" if override is not None else definition.get("origin", "builtin")
    return {
        "key": key,
        "title": definition.get("title", key),
        "description": definition.get("description", ""),
        "role": definition.get("role", "custom"),
        "origin": origin,
        "generated": generated,
        "editable": not generated,
        "content": content,
        "revision": _hash(content),
        "vars": definition.get("vars", []),
    }


def list_templates() -> list[dict]:
    return [template_doc(t["key"]) for t in _tpl_defs()]


def save_template(key: str, content: str, base_revision: str | None) -> dict:
    """Persist an override with optimistic concurrency. Returns the new doc."""
    definition = _tpl_def(key)
    if definition is None:
        raise KeyError(key)
    if definition.get("generated"):
        raise ValueError("generated templates are read-only")
    config.ensure_data()
    current = template_doc(key)
    if base_revision is not None and base_revision != current["revision"]:
        raise ConflictError("template changed since it was loaded")
    _override_path(key).write_text(content.replace("\r\n", "\n"), encoding="utf-8")
    return template_doc(key)


def reset_template(key: str) -> dict | None:
    """Remove a user override, restoring the built-in content."""
    if _tpl_def(key) is None:
        return None
    path = _override_path(key)
    if path.exists():
        path.unlink()
    return template_doc(key)


def variable_values() -> dict:
    """Merge built-in variable values with user overrides and auto values."""
    values = render.builtin_values()
    for var in _manifest().get("variables", []):
        values[var["name"]] = var.get("value", "")
    values.update(_config().get("variables", {}))
    # 'date' is always auto: an empty configured value falls back to builtin.
    if not values.get("date"):
        values["date"] = render.builtin_values()["date"]
    return values


def list_variables() -> list[dict]:
    values = variable_values()
    out = []
    for var in _manifest().get("variables", []):
        out.append({
            "name": var["name"],
            "description": var.get("description", ""),
            "origin": var.get("origin", "builtin"),
            "value": values.get(var["name"], ""),
        })
    return out


def set_variable_values(values: dict) -> dict:
    data = _config()
    current = dict(data.get("variables", {}))
    for name, value in values.items():
        if isinstance(name, str):
            current[name] = "" if value is None else str(value)
    data["variables"] = current
    _write_config(data)
    return list_variables()


def list_presets() -> list[dict]:
    presets = []
    for preset in _manifest().get("presets", []):
        presets.append({"id": preset["id"], "title": preset.get("title", preset["id"])})
    return presets


def active_preset() -> str | None:
    """Which preset the effective 'role' template equals, or None if edited."""
    doc = template_doc("role")
    if doc is None:
        return None
    digest = doc["revision"]
    for preset in _manifest().get("presets", []):
        if _hash(preset["content"]) == digest:
            return preset["id"]
    return None


def apply_preset(preset_id: str) -> dict | None:
    """Write a preset's text into the 'role' override."""
    preset = next((p for p in _manifest().get("presets", []) if p["id"] == preset_id), None)
    if preset is None:
        return None
    config.ensure_data()
    _override_path("role").write_text(preset["content"].replace("\r\n", "\n"), encoding="utf-8")
    return {"id": preset_id, "title": preset.get("title", preset_id)}


def build_prefix(values: dict | None = None) -> dict:
    """Render every template in order into segments and the joined system prompt."""
    vals = variable_values() if values is None else {**render.builtin_values(), **values}
    segments = []
    for definition in _tpl_defs():
        doc = template_doc(definition["key"])
        text = render.render(doc["content"], vals)
        segments.append({
            "key": doc["key"],
            "title": doc["title"],
            "role": doc["role"],
            "origin": doc["origin"],
            "generated": doc["generated"],
            "editable": doc["editable"],
            "content": text,
            "vars": doc["vars"],
        })
    prompt = "\n\n".join(s["content"] for s in segments if s["content"].strip())
    return {"prompt": prompt, "segments": segments, "variables": vals}
