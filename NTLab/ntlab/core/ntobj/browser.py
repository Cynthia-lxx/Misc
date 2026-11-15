"""Directory enumeration / object detail over the NT object-manager namespace.

Read-only: every opened handle is closed in a finally block; no write-class
API is ever called.
"""

from __future__ import annotations

import ctypes

from ntlab.core.errors import InvalidPathError, NtStatusError, NotFoundError
from . import ntapi as nt

WELL_KNOWN_ROOTS = [
    "\\BaseNamedObjects",
    "\\KernelObjects",
    "\\Device",
    "\\KnownDlls",
    "\\KnownDlls32",
    "\\ObjectTypes",
    "\\RPC Control",
    "\\Sessions",
    "\\Windows",
    "\\GLOBAL??",
    "\\Security",
    "\\NLS",
]

_BUFFER_SIZE = 64 * 1024


def validate_path(path: str) -> str:
    p = path.strip()
    if not p.startswith("\\"):
        raise InvalidPathError("NT path must start with '\\'", {"got": path})
    if len(p) > 1024:
        raise InvalidPathError("path too long")
    if "\x00" in p:
        raise InvalidPathError("NUL byte in path")
    return p


def _make_object_attributes(path: str, root_handle: ctypes.c_void_p | None = None):
    name_buf = ctypes.create_unicode_buffer(path)
    us = nt.UNICODE_STRING(
        Length=len(path) * 2,
        MaximumLength=len(name_buf) * 2,
        Buffer=ctypes.cast(name_buf, ctypes.POINTER(ctypes.c_ushort)),
    )
    oa = nt.OBJECT_ATTRIBUTES(
        Length=ctypes.sizeof(nt.OBJECT_ATTRIBUTES),
        RootDirectory=root_handle,
        ObjectName=ctypes.pointer(us),
        Attributes=nt.OBJ_CASE_INSENSITIVE,
        SecurityDescriptor=None,
        SecurityQualityOfService=None,
    )
    # keep buffers referenced until the call completes
    oa._name_buf = name_buf  # type: ignore[attr-defined]
    oa._name_us = us  # type: ignore[attr-defined]
    return oa


def open_directory(path: str) -> ctypes.c_void_p:
    path = validate_path(path)
    handle = ctypes.c_void_p()
    oa = _make_object_attributes(path)
    status = nt.ntdll.NtOpenDirectoryObject(
        ctypes.byref(handle), nt.DIRECTORY_QUERY_ACCESS, ctypes.byref(oa))
    if status != nt.STATUS_SUCCESS:
        raise NtStatusError(status, nt.status_name(status), nt.status_message(status))
    return handle


def open_symbolic_link(path: str) -> ctypes.c_void_p:
    path = validate_path(path)
    handle = ctypes.c_void_p()
    oa = _make_object_attributes(path)
    status = nt.ntdll.NtOpenSymbolicLinkObject(
        ctypes.byref(handle), nt.SYMBOLIC_LINK_QUERY, ctypes.byref(oa))
    if status != nt.STATUS_SUCCESS:
        raise NtStatusError(status, nt.status_name(status), nt.status_message(status))
    return handle


def query_symbolic_link_target(path: str) -> str:
    handle = open_symbolic_link(path)
    try:
        buf = ctypes.create_unicode_buffer(1024)
        us = nt.UNICODE_STRING(
            Length=0,
            MaximumLength=1024 * 2,
            Buffer=ctypes.cast(buf, ctypes.POINTER(ctypes.c_ushort)),
        )
        returned = ctypes.c_ulong(0)
        status = nt.ntdll.NtQuerySymbolicLinkObject(
            handle, ctypes.byref(us), ctypes.byref(returned))
        if status not in (nt.STATUS_SUCCESS, nt.STATUS_BUFFER_OVERFLOW):
            raise NtStatusError(status, nt.status_name(status), nt.status_message(status))
        if us.Length == 0:
            return ""
        chars = min(us.Length // 2, 1023)
        return ctypes.wstring_at(us.Buffer, chars)
    finally:
        nt.ntdll.NtClose(handle)


def list_directory(path: str) -> dict:
    """Enumerate one directory. Returns {path, entries, truncated}."""
    handle = open_directory(path)
    entries: list[dict] = []
    truncated = False
    try:
        index = 0
        while True:
            buf = ctypes.create_string_buffer(_BUFFER_SIZE)
            restart = 1 if index == 0 else 0
            status = nt.ntdll.NtQueryDirectoryObject(
                handle,
                buf,
                _BUFFER_SIZE,
                True,                       # ReturnSingleEntry
                restart,
                ctypes.byref(ctypes.c_ulong(index)),
                ctypes.byref(ctypes.c_ulong(0)),
            )
            if status & 0xFFFFFFFF == nt.STATUS_NO_MORE_ENTRIES:
                break
            if status & 0xFFFFFFFF == nt.STATUS_MORE_ENTRIES:
                truncated = True
                break
            if status != nt.STATUS_SUCCESS:
                raise NtStatusError(status, nt.status_name(status), nt.status_message(status))

            info = ctypes.cast(buf, ctypes.POINTER(nt.OBJECT_DIRECTORY_INFORMATION)).contents
            if info.Name.Length == 0:
                break
            name = ctypes.wstring_at(info.Name.Buffer, info.Name.Length // 2)
            type_name = ctypes.wstring_at(info.TypeName.Buffer, info.TypeName.Length // 2)
            entries.append({
                "name": name,
                "type": type_name,
                "is_directory": type_name == "Directory",
                "is_symlink": type_name == "SymbolicLink",
            })
            index += 1
            if index >= 4096:  # hard safety stop
                truncated = True
                break
    finally:
        nt.ntdll.NtClose(handle)
    return {"path": path, "entries": entries, "truncated": truncated}


def well_known_roots() -> dict:
    """Probe curated roots; missing/denied ones are reported, not hidden."""
    roots = []
    for path in ["\\", *WELL_KNOWN_ROOTS]:
        try:
            h = open_directory(path)
            nt.ntdll.NtClose(h)
            roots.append({"path": path, "accessible": True})
        except NtStatusError as exc:
            roots.append({
                "path": path,
                "accessible": False,
                "status": exc.detail.get("status"),
                "status_name": exc.nt_name,
                "message": exc.message,
            })
    return {"roots": roots}


def object_detail(path: str, type_name: str | None = None) -> dict:
    """Detail for one object. Directories get entry counts on demand (client)."""
    path = validate_path(path)
    detail: dict = {"path": path, "name": path.rsplit("\\", 1)[-1] or "\\"}
    if type_name:
        detail["type"] = type_name

    if type_name == "SymbolicLink":
        try:
            detail["target"] = query_symbolic_link_target(path)
        except NtStatusError as exc:
            detail["target_error"] = {"status": exc.detail.get("status"), "message": exc.message}
    return detail
