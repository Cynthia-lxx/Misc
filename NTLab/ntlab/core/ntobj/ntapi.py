"""ctypes declarations for the NT object-manager APIs we use.

Read-only contract of this module:
  - NtOpenDirectoryObject (DIRECTORY_QUERY only)
  - NtQueryDirectoryObject
  - NtQuerySymbolicLinkObject
  - NtClose
No create/write/delete API is ever declared or called.
"""

from __future__ import annotations

import ctypes
from ctypes import wintypes

ntdll = ctypes.WinDLL("ntdll", use_last_error=True)

# ---- basic types -----------------------------------------------------------

ULONG = ctypes.c_ulong
USHORT = ctypes.c_ushort
HANDLE = wintypes.HANDLE
PVOID = ctypes.c_void_p


class UNICODE_STRING(ctypes.Structure):
    _fields_ = [
        ("Length", USHORT),
        ("MaximumLength", USHORT),
        ("Buffer", ctypes.POINTER(ctypes.c_ushort)),
    ]


class OBJECT_ATTRIBUTES(ctypes.Structure):
    _fields_ = [
        ("Length", ULONG),
        ("RootDirectory", HANDLE),
        ("ObjectName", ctypes.POINTER(UNICODE_STRING)),
        ("Attributes", ULONG),
        ("SecurityDescriptor", PVOID),
        ("SecurityQualityOfService", PVOID),
    ]


class OBJECT_DIRECTORY_INFORMATION(ctypes.Structure):
    """Array of {Name, TypeName} pairs, terminated by an entry with Name.Length == 0."""

    _fields_ = [
        ("Name", UNICODE_STRING),
        ("TypeName", UNICODE_STRING),
    ]


# ---- constants -------------------------------------------------------------

STATUS_SUCCESS = 0x00000000
STATUS_NO_MORE_ENTRIES = 0x8000001A
STATUS_MORE_ENTRIES = 0x00000105
STATUS_BUFFER_TOO_SMALL = 0xC0000023
STATUS_BUFFER_OVERFLOW = 0x80000005
STATUS_ACCESS_DENIED = 0xC0000022
STATUS_OBJECT_NAME_NOT_FOUND = 0xC0000034
STATUS_OBJECT_PATH_NOT_FOUND = 0xC000003A
STATUS_INVALID_INFO_CLASS = 0xC0000003
STATUS_INVALID_HANDLE = 0xC0000008

DIRECTORY_QUERY = 0x0002
DIRECTORY_TRAVERSE = 0x0001
# 实测（Win11）：仅 DIRECTORY_QUERY 打开后 NtQueryDirectoryObject 返回
# STATUS_ACCESS_DENIED；QUERY|TRAVERSE 组合才可枚举。
DIRECTORY_QUERY_ACCESS = DIRECTORY_QUERY | DIRECTORY_TRAVERSE
OBJ_CASE_INSENSITIVE = 0x00000040

STATUS_NAMES = {
    STATUS_ACCESS_DENIED: "STATUS_ACCESS_DENIED",
    STATUS_OBJECT_NAME_NOT_FOUND: "STATUS_OBJECT_NAME_NOT_FOUND",
    STATUS_OBJECT_PATH_NOT_FOUND: "STATUS_OBJECT_PATH_NOT_FOUND",
    STATUS_INVALID_INFO_CLASS: "STATUS_INVALID_INFO_CLASS",
    STATUS_INVALID_HANDLE: "STATUS_INVALID_HANDLE",
    STATUS_NO_MORE_ENTRIES: "STATUS_NO_MORE_ENTRIES",
    STATUS_BUFFER_TOO_SMALL: "STATUS_BUFFER_TOO_SMALL",
    STATUS_BUFFER_OVERFLOW: "STATUS_BUFFER_OVERFLOW",
    0xC0000033: "STATUS_OBJECT_NAME_INVALID",
    0xC00000AD: "STATUS_NAME_TOO_LONG",
    0xC000010A: "STATUS_PROCESS_IS_TERMINATING",
    0xC00000BB: "STATUS_NOT_SUPPORTED",
}

STATUS_MESSAGES = {
    STATUS_ACCESS_DENIED: "拒绝访问（该目录需要特权，或被系统保护）",
    STATUS_OBJECT_NAME_NOT_FOUND: "对象名不存在",
    STATUS_OBJECT_PATH_NOT_FOUND: "对象路径不存在",
    STATUS_INVALID_INFO_CLASS: "信息类无效",
    STATUS_INVALID_HANDLE: "句柄无效",
    0xC0000033: "对象名格式非法",
    0xC00000AD: "名字过长",
    0xC00000BB: "该对象类型不支持此查询",
}

# ---- prototypes ------------------------------------------------------------

ntdll.NtOpenDirectoryObject.argtypes = [
    ctypes.POINTER(HANDLE),          # DirectoryHandle
    ctypes.c_ulong,                  # DesiredAccess
    ctypes.POINTER(OBJECT_ATTRIBUTES),
]
ntdll.NtOpenDirectoryObject.restype = ctypes.c_ulong  # NTSTATUS

ntdll.NtQueryDirectoryObject.argtypes = [
    HANDLE,                          # DirectoryHandle
    PVOID,                           # Buffer
    ULONG,                           # Length
    wintypes.BOOL,                   # ReturnSingleEntry
    wintypes.BOOL,                   # RestartScan
    ctypes.POINTER(ULONG),           # Context (index)
    ctypes.POINTER(ULONG),           # ReturnLength
]
ntdll.NtQueryDirectoryObject.restype = ctypes.c_ulong

ntdll.NtQuerySymbolicLinkObject.argtypes = [
    HANDLE,                          # LinkHandle
    ctypes.POINTER(UNICODE_STRING),  # LinkTarget
    ctypes.POINTER(ULONG),           # ReturnedLength
]
ntdll.NtQuerySymbolicLinkObject.restype = ctypes.c_ulong

ntdll.NtOpenSymbolicLinkObject.argtypes = [
    ctypes.POINTER(HANDLE),          # LinkHandle
    ctypes.c_ulong,                  # DesiredAccess
    ctypes.POINTER(OBJECT_ATTRIBUTES),
]
ntdll.NtOpenSymbolicLinkObject.restype = ctypes.c_ulong

ntdll.NtClose.argtypes = [HANDLE]
ntdll.NtClose.restype = ctypes.c_ulong

SYMBOLIC_LINK_QUERY = 0x0001


# ---- helpers ---------------------------------------------------------------

def status_name(status: int) -> str:
    return STATUS_NAMES.get(status & 0xFFFFFFFF, f"0x{status & 0xFFFFFFFF:08X}")


def status_message(status: int) -> str:
    s = status & 0xFFFFFFFF
    return STATUS_MESSAGES.get(s, f"NT status {status_name(s)}")
