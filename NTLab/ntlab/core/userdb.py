"""用户数据库（SQLite）：保存编排程序与 PE 导出库缓存。

位于 data/user.db（随仓库忽略）。所有访问经模块级锁串行化；sqlite3 连接
以 check_same_thread=False 打开、由锁保护。
"""

from __future__ import annotations

import json
import sqlite3
import threading
import time
from pathlib import Path
from typing import Any

from ntlab.core.errors import NtlabError

_DB_PATH = Path(__file__).resolve().parents[2] / "data" / "user.db"
_LOCK = threading.Lock()
_CONN: sqlite3.Connection | None = None


def _conn() -> sqlite3.Connection:
    global _CONN
    if _CONN is None:
        _DB_PATH.parent.mkdir(parents=True, exist_ok=True)
        _CONN = sqlite3.connect(_DB_PATH, check_same_thread=False)
        _CONN.row_factory = sqlite3.Row
        _CONN.executescript(
            """
            CREATE TABLE IF NOT EXISTS programs (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE,
                program_json TEXT NOT NULL,
                asm_text TEXT NOT NULL DEFAULT '',
                created_at REAL NOT NULL,
                updated_at REAL NOT NULL
            );
            CREATE TABLE IF NOT EXISTS library_meta (
                key TEXT PRIMARY KEY,
                value TEXT
            );
            CREATE TABLE IF NOT EXISTS library_modules (
                name TEXT PRIMARY KEY,
                path TEXT NOT NULL,
                dll_name TEXT,
                count INTEGER NOT NULL
            );
            CREATE TABLE IF NOT EXISTS library_exports (
                module TEXT NOT NULL,
                name TEXT NOT NULL,
                ordinal INTEGER NOT NULL,
                rva INTEGER NOT NULL,
                trust TEXT NOT NULL,
                PRIMARY KEY (module, name)
            );
            """
        )
        _CONN.commit()
    return _CONN


# ---------------- 编排程序 ----------------

def save_program(name: str, program: list[dict], asm_text: str = "") -> dict:
    """按名称 upsert 保存编排。"""
    now = time.time()
    with _LOCK:
        c = _conn()
        cur = c.execute(
            "INSERT INTO programs(name, program_json, asm_text, created_at, updated_at) "
            "VALUES(?, ?, ?, ?, ?) "
            "ON CONFLICT(name) DO UPDATE SET program_json=excluded.program_json, "
            "asm_text=excluded.asm_text, updated_at=excluded.updated_at",
            (name, json.dumps(program, ensure_ascii=False), asm_text, now, now),
        )
        row = c.execute("SELECT * FROM programs WHERE name=?", (name,)).fetchone()
        c.commit()
    return _program_row(row)


def list_programs() -> list[dict]:
    with _LOCK:
        rows = _conn().execute(
            "SELECT * FROM programs ORDER BY updated_at DESC").fetchall()
    return [_program_row(r) for r in rows]


def get_program(pid: int) -> dict:
    with _LOCK:
        row = _conn().execute("SELECT * FROM programs WHERE id=?", (pid,)).fetchone()
    if row is None:
        raise NtlabError("not_found", f"program {pid} does not exist")
    return _program_row(row)


def delete_program(pid: int) -> None:
    with _LOCK:
        c = _conn()
        cur = c.execute("DELETE FROM programs WHERE id=?", (pid,))
        c.commit()
        if cur.rowcount == 0:
            raise NtlabError("not_found", f"program {pid} does not exist")


def _program_row(row: sqlite3.Row) -> dict:
    return {
        "id": row["id"],
        "name": row["name"],
        "program": json.loads(row["program_json"]),
        "asm_text": row["asm_text"],
        "created_at": row["created_at"],
        "updated_at": row["updated_at"],
    }


# ---------------- PE 导出库缓存 ----------------

def save_library(modules: dict[str, dict], built_at: float) -> None:
    """整库写入（单事务覆盖旧缓存）。"""
    with _LOCK:
        c = _conn()
        c.execute("BEGIN")
        try:
            c.execute("DELETE FROM library_modules")
            c.execute("DELETE FROM library_exports")
            c.executemany(
                "INSERT INTO library_modules(name, path, dll_name, count) VALUES(?, ?, ?, ?)",
                [(m["name"], m["path"], m.get("dll_name"), m["count"])
                 for m in modules.values()],
            )
            c.executemany(
                "INSERT INTO library_exports(module, name, ordinal, rva, trust) "
                "VALUES(?, ?, ?, ?, ?)",
                [(mod, item["name"], item["ordinal"], item["rva"], item["trust"])
                 for mod, m in modules.items() for item in m["exports"]],
            )
            c.execute(
                "INSERT INTO library_meta(key, value) VALUES('built_at', ?) "
                "ON CONFLICT(key) DO UPDATE SET value=excluded.value",
                (str(built_at),),
            )
            c.commit()
        except Exception:
            c.rollback()
            raise


def load_library() -> dict[str, dict] | None:
    """读取缓存的整库；无缓存返回 None。"""
    with _LOCK:
        c = _conn()
        mods = {r["name"]: r["path"] for r in
                c.execute("SELECT name, path FROM library_modules").fetchall()}
        if not mods:
            return None
        modules: dict[str, dict] = {
            name: {"name": name, "path": path, "exports": [], "count": 0,
                   "dll_name": None}
            for name, path in mods.items()
        }
        counts = {r["name"]: r["count"] for r in
                  c.execute("SELECT name, count, dll_name FROM library_modules").fetchall()}
        dll_names = {r["name"]: r["dll_name"] for r in
                     c.execute("SELECT name, dll_name FROM library_modules").fetchall()}
        by_module: dict[str, list[dict]] = {name: [] for name in modules}
        for r in c.execute(
                "SELECT module, name, ordinal, rva, trust FROM library_exports").fetchall():
            by_module[r["module"]].append({
                "name": r["name"], "ordinal": r["ordinal"],
                "rva": r["rva"], "trust": r["trust"],
            })
    for name, m in modules.items():
        m["exports"] = sorted(by_module[name], key=lambda x: x["name"].lower())
        m["count"] = counts.get(name, len(m["exports"]))
        m["dll_name"] = dll_names.get(name)
    return modules


def library_built_at() -> float | None:
    with _LOCK:
        row = _conn().execute(
            "SELECT value FROM library_meta WHERE key='built_at'").fetchone()
    return float(row["value"]) if row else None
