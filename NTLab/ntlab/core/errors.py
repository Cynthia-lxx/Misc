"""Unified error model for NTLab core modules.

core 层唯一异常类型；api 层负责转 HTTP 响应。
"""

from __future__ import annotations

from typing import Any


class NtlabError(Exception):
    """Base error carrying a machine-readable code and human-readable message."""

    def __init__(self, code: str, message: str, detail: dict[str, Any] | None = None) -> None:
        super().__init__(message)
        self.code = code
        self.message = message
        self.detail = detail or {}

    def to_dict(self) -> dict[str, Any]:
        payload: dict[str, Any] = {"code": self.code, "message": self.message}
        if self.detail:
            payload["detail"] = self.detail
        return payload


class NotFoundError(NtlabError):
    def __init__(self, message: str, detail: dict[str, Any] | None = None) -> None:
        super().__init__("not_found", message, detail)


class InvalidPathError(NtlabError):
    def __init__(self, message: str, detail: dict[str, Any] | None = None) -> None:
        super().__init__("invalid_path", message, detail)


class ParseError(NtlabError):
    def __init__(self, message: str, detail: dict[str, Any] | None = None) -> None:
        super().__init__("parse_error", message, detail)


class NtStatusError(NtlabError):
    """An NT API call returned a non-success status."""

    def __init__(self, status: int, name: str, message: str) -> None:
        super().__init__("nt_status", message, {"status": f"0x{status & 0xFFFFFFFF:08X}", "name": name})
        self.status = status
        self.nt_name = name
