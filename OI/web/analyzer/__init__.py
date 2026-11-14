"""Pure-local C++ analysis engine for the OI console (no web dependencies)."""
from .engine import analyse, cache_size, clear_cache, list_cpp_files, resolve_code
from .report import REPORT_VERSION, AnalysisReport, Finding

__all__ = [
    "analyse", "cache_size", "clear_cache", "list_cpp_files", "resolve_code",
    "REPORT_VERSION", "AnalysisReport", "Finding",
]
