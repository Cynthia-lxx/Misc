"""Prompt template store: built-in + user style profiles for code generation.

Persisted to web/data/prompt_templates.json (UTF-8, no BOM). The active
template is used by /api/generate as the system prompt; "original" always
resolves to OI.SYSTEM_PROMPT so it tracks the CLI. Thread-safe via a lock.
"""
from __future__ import annotations

import json
import threading
import time
from pathlib import Path

STORE_PATH = Path(__file__).resolve().parent / "data" / "prompt_templates.json"
_LOCK = threading.RLock()  # re-entrant: create/update/activate/delete call load() while held

# 标准 OI 风格：面向「能 AC、可读、不 UB」的正式题解，而非个人手癖。
STANDARD_TEMPLATE = """你是一名资深 OI（信息学竞赛）选手。根据题目描述输出一份可以直接编译并通过评测的 C++14 题解。

## 总则
- 输出完整、可直接提交的代码；除用户明确要求外不要解释。
- 行为以题面为准；读题时先确认数据范围、时空限制与特殊边界，再选算法。
- 算法必须满足复杂度约束：n ≥ 1e5 时禁止 O(n^2) 及以上；不确定时优先选渐进复杂度更低且实现不易错的方案。

## 结构与头文件
- 第一行 #include <bits/stdc++.h>，随后 using namespace std;（竞赛惯例）。
- 文件首行写注释：// [题号] [算法] 时间 O(?) 空间 O(?)，最多一行。
- 逻辑全部置于 main；可复用的小函数放 main 之前，避免过度抽象与模板元编程。

## 命名
- 循环用 i/j/k；规模用 n/m/q；答案用 ans；计数用 cnt；前缀和用 pre/s；图用 g、边表用 e。
- 多字母变量用完整小写单词或公认缩写（dp, fa, vis, deg, mx, mn），禁止无意义组合名。
- 单字母仅限循环变量、临时交换与惯用记号；占位变量用 _。

## 数值与正确性
- 数组规模用 const int N = 1e5 + 5; 形式：与数据范围一致并留 5% 裕量；严禁硬编码裸数字。
- 有溢出风险（求和/乘积可能超 2^31）必须用 long long，输出用 %lld 或 cout；开大数组前估算内存不超过题面空间限制。
- 变量必须初始化；数组下标 0/1 基准全程一致；浮点比较用 eps。
- 禁止一切 UB：不越界访问、不依赖求值顺序、不做负数/超宽位移、不用未定义类型转换。

## 输入输出
- 统一用 cin/cout 或 scanf/printf 其一，不混用。
- 输入量 ≥ 1e5 时关闭同步：ios::sync_with_stdio(false); cin.tie(nullptr);
- 换行统一 '\\n'，不用 endl；严格按题面格式输出（空格、换行、精度、大小写）。

## 风格
- 4 空格缩进，一行一条语句；大括号 K&R（{ 跟随行尾）。
- 注释只解释「为什么」与算法分块（如 // 离散化、// 建反图），不复述代码；行数越少越贵，可读性优先。
- 优先朴素正确的实现；确需卡常（快读、手写栈、位运算）时必须注释理由。
- 禁止保留调试输出（cerr/cout 调试行）、危险宏（#define int long long、#define N 1000005 遮蔽）与死代码。

## 提交前自检（内部执行，不输出）
- 边界：最小/最大规模、单元素、全相同、极端值；多组输入是否读到 EOF。
- 与样例心算对照一遍再输出。
"""


def _defaults() -> dict:
    return {
        "active": "standard",
        "templates": [
            {"id": "standard", "name": "标准 OI 风格（推荐）", "builtin": True, "text": STANDARD_TEMPLATE},
            {"id": "original", "name": "用户原版（OI.py）", "builtin": True, "text": ""},
        ],
    }


def _original_text() -> str:
    """Resolve the CLI's system prompt at call time so it always tracks OI.py.

    Prefer the module the server already imported (import OI as oi); a fresh
    `import oi` would re-execute OI.py and can fail inside request threads."""
    import sys  # noqa: PLC0415
    try:
        mod = sys.modules.get("OI")
        if mod is not None and hasattr(mod, "SYSTEM_PROMPT"):
            return mod.SYSTEM_PROMPT
        import OI  # noqa: PLC0415
        return OI.SYSTEM_PROMPT
    except Exception:
        return ""


def load() -> dict:
    with _LOCK:
        data = _defaults()
        if STORE_PATH.is_file():
            try:
                stored = json.loads(STORE_PATH.read_text(encoding="utf-8"))
                templates = {t["id"]: t for t in data["templates"]}
                for t in stored.get("templates", []):
                    if t.get("id") and not t["id"] in templates:
                        templates[t["id"]] = {
                            "id": str(t["id"]),
                            "name": str(t.get("name") or t["id"])[:60],
                            "builtin": False,
                            "text": str(t.get("text") or ""),
                        }
                    elif t.get("id") in templates and t.get("text"):
                        # keep builtin texts persisted only when non-empty
                        pass
                data["templates"] = list(templates.values())
                if stored.get("active") in {t["id"] for t in data["templates"]}:
                    data["active"] = stored["active"]
            except Exception:
                pass  # corrupt store falls back to defaults, never crashes the API
        return data


def _save_locked(data: dict) -> None:
    STORE_PATH.parent.mkdir(parents=True, exist_ok=True)
    STORE_PATH.write_text(json.dumps(data, ensure_ascii=False, indent=2), encoding="utf-8")


def resolve_text(template: dict) -> str:
    """Builtin 'original' tracks OI.py; everything else uses its stored text."""
    if template.get("id") == "original":
        return _original_text()
    return template.get("text") or ""


def active_text() -> str:
    data = load()
    for t in data["templates"]:
        if t["id"] == data["active"]:
            text = resolve_text(t)
            if text:
                return text
    return _original_text()  # last resort: the CLI prompt


def create(name: str, text: str) -> dict:
    with _LOCK:
        data = load()
        tid = "p%d" % int(time.time() * 1000)
        entry = {"id": tid, "name": (name or "未命名模板")[:60], "builtin": False, "text": text}
        data["templates"].append(entry)
        data["active"] = tid
        _save_locked(data)
        return entry


def update(tid: str, name: str | None, text: str | None) -> dict | None:
    with _LOCK:
        data = load()
        for t in data["templates"]:
            if t["id"] == tid and not t.get("builtin"):
                if name is not None:
                    t["name"] = (name or t["name"])[:60]
                if text is not None:
                    t["text"] = text
                _save_locked(data)
                return t
        return None


def activate(tid: str) -> bool:
    with _LOCK:
        data = load()
        if any(t["id"] == tid for t in data["templates"]):
            data["active"] = tid
            _save_locked(data)
            return True
        return False


def delete(tid: str) -> bool:
    with _LOCK:
        data = load()
        rest = [t for t in data["templates"] if t["id"] == tid and not t.get("builtin")]
        if not rest:
            return False
        data["templates"] = [t for t in data["templates"] if t["id"] != tid]
        if data["active"] == tid:
            data["active"] = data["templates"][0]["id"] if data["templates"] else "standard"
        _save_locked(data)
        return True
