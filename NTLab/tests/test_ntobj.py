"""ObjDir core smoke tests (read-only NT namespace browsing)."""

from __future__ import annotations

import pytest

from ntlab.core import ntobj
from ntlab.core.errors import InvalidPathError, NtStatusError


def test_path_validation():
    with pytest.raises(InvalidPathError):
        ntobj.list_directory("Device")            # no leading backslash
    with pytest.raises(InvalidPathError):
        ntobj.list_directory("\\a\x00b")          # NUL byte
    with pytest.raises(InvalidPathError):
        ntobj.validate_path("x" * 2000)


def test_nonexistent_directory():
    with pytest.raises(NtStatusError) as exc:
        ntobj.list_directory("\\__ntlab_definitely_not_here__")
    assert "STATUS_" in exc.value.nt_name


def test_root_directory_enumerates():
    r = ntobj.list_directory("\\")
    names = {e["name"] for e in r["entries"]}
    assert "BaseNamedObjects" in names
    assert "Device" in names
    for e in r["entries"]:
        assert e["type"]
        assert isinstance(e["is_directory"], bool)


def test_base_named_objects_enumerates():
    r = ntobj.list_directory("\\BaseNamedObjects")
    assert len(r["entries"]) > 0
    types = {e["type"] for e in r["entries"]}
    assert types  # typically Event/Section/Mutant/Semaphore…


def test_well_known_roots_shape():
    r = ntobj.well_known_roots()
    paths = {root["path"] for root in r["roots"]}
    assert "\\" in paths
    for root in r["roots"]:
        assert "accessible" in root


def test_symlink_query_known_dlls():
    """\\KnownDlls\\kernel32.dll? — KnownDlls entries are sections, not symlinks.
    Use a guaranteed symlink if present; skip gracefully otherwise."""
    r = ntobj.list_directory("\\GLOBAL??")
    symlinks = [e for e in r["entries"] if e["type"] == "SymbolicLink"]
    if not symlinks:
        pytest.skip("no symlink visible in \\GLOBAL?? (restricted session)")
    target = ntobj.query_symbolic_link_target("\\GLOBAL??\\" + symlinks[0]["name"])
    assert target.startswith("\\Device\\")
