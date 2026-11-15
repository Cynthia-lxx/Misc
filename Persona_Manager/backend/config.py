"""Paths and constants for the generic AI content template manager.

This module has no dependency on any agent / desktop-pet concept. It only
describes where built-in templates and user overrides live on disk.
"""
import os
from pathlib import Path

BACKEND = Path(__file__).resolve().parent
ROOT = BACKEND.parent                      # Persona_Manager project root
DEFAULTS = BACKEND / "defaults"            # shipped templates (read-only baseline)
MANIFEST = DEFAULTS / "manifest.json"
DATA = ROOT / "data"                       # runtime user data (created on first run)
OVERRIDES = DATA / "overrides"             # one <key>.md per user-edited template
CONFIG_FILE = DATA / "config.json"         # variable values + misc settings
FRONTEND = ROOT / "frontend"

HOST = os.environ.get("PERSONA_MANAGER_HOST", "127.0.0.1")
PORT = int(os.environ.get("PERSONA_MANAGER_PORT", "18765"))

API_TEMPLATES = "/api/templates"
API_PREFIX = "/api/templates/prefix"
API_RESET = "/api/templates/reset"
API_VARS = "/api/vars"
API_PRESETS = "/api/presets"
API_ROLE_APPLY = "/api/role/apply"
API_PROMPT = "/api/prompt"                 # the call an LLM tool hits to get its system prompt
API_RELOAD = "/api/reload"
API_HEALTH = "/api/health"


def ensure_data() -> None:
    """Create the runtime data directories if they do not exist yet."""
    OVERRIDES.mkdir(parents=True, exist_ok=True)
    DATA.mkdir(parents=True, exist_ok=True)
