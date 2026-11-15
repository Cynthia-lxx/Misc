"""NT object-manager namespace browsing core (ctypes, read-only)."""

from .browser import list_directory, object_detail, well_known_roots, validate_path, query_symbolic_link_target

__all__ = ["list_directory", "object_detail", "well_known_roots", "validate_path", "query_symbolic_link_target"]
