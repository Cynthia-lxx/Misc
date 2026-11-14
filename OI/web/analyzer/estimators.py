"""Four estimator families. Each returns Findings and never raises outward.

Rules for `style` mirror OI.py SYSTEM_PROMPT (:91-118). Every number that
cannot be derived from the source is reported as unknown, never guessed.
"""
from __future__ import annotations

import re

from .lexer import Lexed
from .report import TYPE_SIZE_KEYS, finding, human_bytes

ESTIMATORS: list = []
MAX_FINDINGS_PER_FAMILY = 14


def register(dimension: str, key: str):
    def decorate(fn):
        ESTIMATORS.append((dimension, key, fn))
        return fn
    return decorate


def run_all(lexed: Lexed, assumptions) -> list:
    findings = []
    for dimension, key, estimator in ESTIMATORS:
        try:
            produced = list(estimator(lexed, assumptions))
        except Exception as exc:
            produced = [finding(
                dimension, key, "unknown", None, "", (),
                f"estimator error: {type(exc).__name__}", "unknown",
            )]
        findings.extend(produced[:MAX_FINDINGS_PER_FAMILY])
    return findings


def _budget_verdict(peak, limit, largest):
    if peak is None or not limit:
        return "unknown", "no_budget"
    if largest is not None and largest > limit:
        return "red", "single_exceeds_free"
    if peak >= limit * 0.9:
        return "red", "total_exceeds_budget"
    if peak < limit * 0.7:
        return "green", "below_70_free"
    return "yellow", "between"


@register("memory", "memory.arrays")
def memory_arrays(lx: Lexed, ass):
    global_bytes = 0
    stack_by_function: dict[int, int] = {}
    largest = 0
    largest_line = 0
    unknown_lines: list[int] = []
    array_lines: list[int] = []

    for arr in lx.arrays:
        array_lines.append(arr.line)
        size_key = TYPE_SIZE_KEYS.get(arr.type_name)
        if arr.elements is None or size_key is None:
            unknown_lines.append(arr.line)
            continue
        size = arr.elements * ass.get(size_key)
        if size > largest:
            largest, largest_line = size, arr.line
        if arr.global_scope:
            global_bytes += size
        else:
            owner = next(
                (fn.line for fn in lx.functions if fn.line <= arr.line <= fn.end_line),
                0,
            )
            stack_by_function[owner] = stack_by_function.get(owner, 0) + size

    stack_bytes = max(stack_by_function.values(), default=0)
    limit = ass.get("memory_limit_bytes")
    stack_limit = ass.get("stack_budget_bytes")

    heap_bytes = 0
    for match in re.finditer(r"\bnew\s+([A-Za-z_]\w*)\s*\[([^\]]+)\]", lx.masked):
        line = lx.masked.count("\n", 0, match.start()) + 1
        text = match.group(2).strip()
        if text.isdigit() and match.group(1) in TYPE_SIZE_KEYS:
            heap_bytes += int(text) * ass.get(TYPE_SIZE_KEYS[match.group(1)])
        else:
            unknown_lines.append(line)

    peak = global_bytes + stack_bytes + heap_bytes
    verdict, reason = _budget_verdict(peak if lx.arrays or heap_bytes else None, limit, largest)
    stack_verdict = "red" if stack_bytes >= stack_limit * 0.9 else ("yellow" if stack_bytes >= stack_limit * 0.7 else "green")
    out = [
        finding("memory", "memory.peak_bytes", verdict, peak if peak else None, "bytes",
                array_lines[:8], f"{reason}; peak = global + max stack frame + heap estimate ({human_bytes(peak)})", "estimated"),
        finding("memory", "memory.global_bytes", "green" if global_bytes < limit * 0.7 else "yellow",
                global_bytes, "bytes", [a.line for a in lx.arrays if a.global_scope][:8],
                "sum of statically sized global/static arrays", "estimated"),
        finding("memory", "memory.stack_bytes", stack_verdict, stack_bytes, "bytes",
                [a.line for a in lx.arrays if not a.global_scope][:8],
                f"largest single stack frame; budget {human_bytes(stack_limit)}", "estimated"),
        finding("memory", "memory.largest_single", "red" if largest > limit * 0.9 else "green",
                largest, "bytes", [largest_line] if largest_line else (),
                "largest single array declaration", "estimated"),
    ]
    if heap_bytes:
        out.append(finding("memory", "memory.heap_bytes", "green", heap_bytes, "bytes", (),
                           "statically sized new[] allocations only", "estimated"))
    if unknown_lines:
        out.append(finding("memory", "memory.unknown_arrays", "unknown", None, "count",
                           unknown_lines[:8],
                           f"{len(unknown_lines)} array declaration(s) have a symbolic dimension or unknown type size",
                           "unknown"))
    if lx.truncated or lx.depth_limit_hit:
        out.append(finding("memory", "memory.scan_limits", "unknown", None, "", (),
                           "token or brace depth limit reached; memory figures may be incomplete", "unknown"))
    return out


@register("complexity", "complexity.loops")
def complexity_loops(lx: Lexed, ass):
    out = []
    depth = lx.max_loop_depth
    variable_loops = [loop for loop in lx.loops if loop.bound == "variable"]
    constant_only = bool(lx.loops) and not variable_loops
    if not lx.loops:
        out.append(finding("complexity", "complexity.estimated_order", "unknown", None, "", (),
                           "no loop detected; recursion or library calls may still dominate", "unknown"))
    else:
        order = "O(1)" if constant_only else ("O(n)" if depth == 1 else f"O(n^{depth})")
        verdict = "green" if constant_only or depth <= 2 else ("yellow" if depth == 3 else "red")
        out.append(finding("complexity", "complexity.estimated_order", verdict, order, "order",
                           [loop.line for loop in lx.loops[:8]],
                           f"heuristic from {len(lx.loops)} loop(s), max nesting {depth}; bounded constants treated as O(1)",
                           "heuristic"))
        out.append(finding("complexity", "complexity.loop_depth", verdict, depth, "levels",
                           [loop.line for loop in lx.loops[:8]],
                           "maximum brace-nested loop depth", "heuristic"))
        if depth >= 3:
            hotspots = [loop.line for loop in lx.loops if loop.depth == depth]
            out.append(finding("complexity", "complexity.hotspot", "yellow", len(hotspots), "lines",
                               hotspots[:8], f"innermost loop body at nesting {depth}", "heuristic"))
    if lx.recursion:
        out.append(finding("complexity", "complexity.recursion", "unknown", None, "",
                           [line for _, line in lx.recursion][:8],
                           "self-recursion detected; depth and order depend on input", "unknown"))
    sorts = [i for i, line in enumerate(lx.lines, 1) if re.search(r"\b(sort|stable_sort|lower_bound|upper_bound|nth_element)\s*\(", line)]
    if sorts:
        out.append(finding("complexity", "complexity.library_ops", "green", len(sorts), "calls",
                           sorts[:8], "standard O(n log n) / O(log n) library calls detected", "estimated"))
    if lx.truncated:
        out.append(finding("complexity", "complexity.scan_limit", "unknown", None, "", (),
                           "token limit reached; complexity estimate may be incomplete", "unknown"))
    return out


@register("style", "style.rules")
def style_rules(lx: Lexed, ass):
    issues: list[tuple[str, int, str]] = []
    for lineno, line in enumerate(lx.lines, 1):
        if re.search(r"\busing\s+namespace\s+std\s*;", line):
            issues.append(("style.using_namespace_std", lineno, "规则 3：改用逐项 using std::名称"))
        if re.search(r"\bstd::endl\b", line):
            issues.append(("style.endl", lineno, "规则 7：endl 会强制刷新，OI 代码通常用 \"\\n\""))
        if re.search(r"\bcerr\b", line):
            issues.append(("style.debug_output", lineno, "规则 5：调试输出用 cerr，但尽量不要保留"))
        if re.search(r"\b(gcd|lcm)\s*\([^)]*\)\s*\{", line) and not re.search(r"\binline\b", line):
            issues.append(("style.inline_helper", lineno, "规则 6：gcd/lcm 等小函数应以 inline 定义"))
        for match in re.finditer(r"\b(?:int|long long|double|char|bool|auto)\s+([A-Za-z_]\w{7,})\b", line):
            name = match.group(1)
            if name not in {"max_retries", "answer", "result", "target", "length", "count", "value"}:
                issues.append(("style.long_name", lineno, f"规则 4：变量名 {name} 偏长，倾向单字母/功能缩写"))
        if re.search(r"#\s*include\s*<(?!bits/stdc\+\+\.h>)[^>]+>", line):
            issues.append(("style.include", lineno, "规则 2：统一用 #include<bits/stdc++.h>"))

    for lineno, text in lx.comments:
        stripped = text.strip()
        if stripped.startswith("//") and not stripped.endswith("喵"):
            issues.append(("style.comment_suffix", lineno, "规则 9：注释结尾要加「喵」"))
    comment_count = len([c for c in lx.comments if c[1].lstrip().startswith("//")])
    if comment_count > max(3, len(lx.lines) // 6):
        issues.append(("style.excess_comments", 1, "规则 8：移除所有非必要注释"))

    declarations: dict[str, int] = {}
    for lineno, line in enumerate(lx.masked.split("\n"), 1):
        for match in re.finditer(r"\b(?:int|long long|double|char|bool)\s+([A-Za-z_]\w*)\s*[;,=]", line):
            declarations.setdefault(match.group(1), lineno)
    for name, lineno in list(declarations.items())[:80]:
        if len(name) > 1 and len(re.findall(r"\b" + re.escape(name) + r"\b", lx.masked)) <= 1:
            issues.append(("style.unused_variable", lineno, f"变量 {name} 声明后未再出现（启发式）"))

    out = [
        finding("style", "style.issue_count",
                "red" if len(issues) >= 5 else ("yellow" if issues else "green"),
                len(issues), "issues", (),
                "rule source: OI.py SYSTEM_PROMPT (:91-118)", "heuristic")
    ]
    for key, lineno, reason in issues[:MAX_FINDINGS_PER_FAMILY - 1]:
        out.append(finding("style", key, "yellow", None, "", (lineno,), reason, "heuristic"))
    if not lx.functions:
        out.append(finding("style", "style.no_main", "unknown", None, "", (),
                           "no function definition recognised; style rules may be incomplete", "unknown"))
    return out


@register("performance", "performance.rules")
def performance_rules(lx: Lexed, ass):
    findings = []
    io_lines = [i for i, line in enumerate(lx.lines, 1) if re.search(r"\b(cin|cout)\b", line)]
    has_fast_io = re.search(r"sync_with_stdio\s*\(\s*false\s*\)", lx.masked)
    if io_lines and not has_fast_io and len(io_lines) >= 2:
        findings.append(finding("performance", "performance.sync_io", "yellow", len(io_lines), "lines",
                                io_lines[:8], "多次 cin/cout 且未关闭同步；建议 ios::sync_with_stdio(false)", "heuristic"))
    endl_lines = [i for i, line in enumerate(lx.lines, 1) if re.search(r"\bstd::endl\b", line)]
    if endl_lines:
        findings.append(finding("performance", "performance.endl_flush", "yellow", len(endl_lines), "lines",
                                endl_lines[:8], "endl 触发 flush，热路径换成 \"\\n\"", "heuristic"))
    push_lines = [i for i, line in enumerate(lx.lines, 1) if re.search(r"\.push_back\s*\(", line)]
    if push_lines and "reserve" not in lx.masked:
        findings.append(finding("performance", "performance.vector_reserve", "yellow", len(push_lines), "lines",
                                push_lines[:8], "push_back 未见 reserve；已知上界时预分配可避免重分配", "heuristic"))
    concat_lines = [i for i, line in enumerate(lx.lines, 1) if re.search(r"\b\w+\s*\+=\s*[^;]*(\"|std::string|string)", line)]
    if concat_lines:
        findings.append(finding("performance", "performance.string_concat", "yellow", len(concat_lines), "lines",
                                concat_lines[:8], "字符串拼接在循环内可能反复分配", "heuristic"))
    expensive_lines = [i for i, line in enumerate(lx.lines, 1)
                       if re.search(r"\b(pow|sqrt|log|exp|sin|cos)\s*\(", line)]
    if expensive_lines and lx.max_loop_depth >= 2:
        findings.append(finding("performance", "performance.math_in_loop", "yellow", len(expensive_lines), "lines",
                                expensive_lines[:8], f"浮点数学调用出现在嵌套循环（深度 {lx.max_loop_depth}）中", "heuristic"))
    if lx.recursion:
        findings.append(finding("performance", "performance.recursive_stack", "yellow", len(lx.recursion), "calls",
                                [line for _, line in lx.recursion][:8], "递归深度不受静态控制，可能爆栈或重复计算", "heuristic"))
    if lx.max_loop_depth >= 3:
        hot = [loop.line for loop in lx.loops if loop.depth >= 3]
        findings.append(finding("performance", "performance.hot_loop", "yellow", len(hot), "lines",
                                hot[:8], "三层及以上循环是主要调优目标", "heuristic"))
    if not findings:
        findings.append(finding("performance", "performance.no_hotspot", "green", 0, "issues", (),
                                "未发现常见本地性能反模式（仅启发式，不替代实测）", "heuristic"))
    return findings
