"""Lightweight C++ lexer for OI-style sources.

Not a parser: comments/strings are masked (positions preserved), tokens carry
1-based line numbers, and a brace stack yields scope depth, loop nesting,
function bodies and array declarations. Anything it cannot resolve is reported
as unknown by the estimators - it never guesses.
"""
from __future__ import annotations

import ast
import dataclasses
import re
from typing import Any

TOKEN_RE = re.compile(
    r"[A-Za-z_]\w*|\d+\.?\d*(?:[eE][+-]?\d+)?|::|->|<<|>>|<=|>=|==|!=|&&|\|\||\+\+|--|[^\s]"
)
FUNC_RE = re.compile(r"\b([A-Za-z_]\w*)\s+([A-Za-z_]\w*)\s*\(([^)]*)\)\s*\{")
ARRAY_DECL_RE = re.compile(
    r"\b(?P<type>unsigned\s+long\s+long|long\s+long|unsigned\s+int|unsigned\s+long|long\s+double|"
    r"[A-Za-z_]\w*(?:::\w+)?)\s+"
    r"(?P<decls>[^;()]+?)\s*;"
)
ARRAY_NAME_RE = re.compile(r"(?P<name>[A-Za-z_]\w*)\s*(?P<dims>(?:\[[^]\n]*\]\s*)+)")
DIM_RE = re.compile(r"\[([^]\n]*)\]")
DEFINE_RE = re.compile(r"#\s*define\s+([A-Za-z_]\w*)\s+([0-9eE+().*/ -]+)\b")
CONST_RE = re.compile(r"\bconst\s+(?:int|long\s+long)\s+([A-Za-z_]\w*)\s*=\s*([0-9eE+().*/ -]+?)\s*;")
CONTROL_WORDS = {"if", "for", "while", "switch", "catch", "return", "else", "do", "sizeof", "new", "delete"}
MAX_BRACE_DEPTH = 64
MAX_TOKENS = 200000


@dataclasses.dataclass(frozen=True)
class Token:
    kind: str
    text: str
    line: int
    col: int


@dataclasses.dataclass(frozen=True)
class ArrayDecl:
    name: str
    type_name: str
    dims: tuple
    line: int
    global_scope: bool
    elements: int | None
    reason: str


@dataclasses.dataclass(frozen=True)
class LoopInfo:
    kind: str
    line: int
    depth: int
    header: str
    bound: str


@dataclasses.dataclass(frozen=True)
class FunctionInfo:
    name: str
    line: int
    end_line: int
    body: str


@dataclasses.dataclass
class Lexed:
    code: str
    lines: list
    masked: str
    tokens: list
    comments: list
    strings: list
    arrays: list
    loops: list
    functions: list
    constants: dict
    line_depth: dict
    max_loop_depth: int
    recursion: list
    depth_limit_hit: bool
    truncated: bool


def _mask(code: str):
    out = list(code)
    comments: list[tuple[int, str]] = []
    strings: list[tuple[int, str]] = []
    i, line, n = 0, 1, len(code)
    while i < n:
        ch = code[i]
        if ch == "\n":
            line += 1
            i += 1
            continue
        if ch == "/" and i + 1 < n and code[i + 1] == "/":
            end = code.find("\n", i)
            end = n if end < 0 else end
            comments.append((line, code[i:end]))
            for k in range(i, end):
                out[k] = " "
            i = end
            continue
        if ch == "/" and i + 1 < n and code[i + 1] == "*":
            close = code.find("*/", i + 2)
            end = n if close < 0 else close + 2
            segment = code[i:end]
            comments.append((line, segment))
            for k in range(i, end):
                if code[k] != "\n":
                    out[k] = " "
            line += segment.count("\n")
            i = end
            continue
        if ch in ('"', "'"):
            quote, j = ch, i + 1
            while j < n:
                if code[j] == "\\":
                    j += 2
                    continue
                if code[j] == quote:
                    j += 1
                    break
                if code[j] == "\n":
                    break
                j += 1
            strings.append((line, code[i:j]))
            for k in range(i, j):
                if code[k] != "\n":
                    out[k] = " "
            i = j
            continue
        i += 1
    return "".join(out), comments, strings


def _tokenize(masked: str):
    tokens: list[Token] = []
    truncated = False
    for lineno, line in enumerate(masked.split("\n"), 1):
        for match in TOKEN_RE.finditer(line):
            text = match.group()
            if len(tokens) >= MAX_TOKENS:
                truncated = True
                return tokens, truncated
            if text[0].isalpha() or text[0] == "_":
                kind = "ident"
            elif text[0].isdigit():
                kind = "number"
            else:
                kind = "punct"
            tokens.append(Token(kind, text, lineno, match.start() + 1))
    return tokens, truncated


def _line_depths(tokens, line_count, masked):
    depth_by_line: dict[int, int] = {}
    depth = 0
    current = 1
    hit = False
    for token in tokens:
        while current < token.line:
            depth_by_line[current] = depth
            current += 1
        if token.text == "{":
            depth += 1
            if depth > MAX_BRACE_DEPTH:
                hit = True
        elif token.text == "}":
            depth = max(0, depth - 1)
    while current <= line_count:
        depth_by_line[current] = depth
        current += 1
    return depth_by_line, hit


def _eval_dim(expr: str, constants: dict) -> int | None:
    expression = expr.strip().replace("^", "**")
    if not expression or len(expression) > 120:
        return None
    try:
        tree = ast.parse(expression, mode="eval")
    except (SyntaxError, ValueError):
        return None

    def walk(node):
        if isinstance(node, ast.Expression):
            return walk(node.body)
        if isinstance(node, ast.Constant) and isinstance(node.value, (int, float)):
            return int(node.value)
        if isinstance(node, ast.Name) and node.id in constants:
            return int(constants[node.id])
        if isinstance(node, ast.UnaryOp) and isinstance(node.op, (ast.UAdd, ast.USub)):
            value = walk(node.operand)
            return None if value is None else (value if isinstance(node.op, ast.UAdd) else -value)
        if isinstance(node, ast.BinOp) and isinstance(node.op, (ast.Add, ast.Sub, ast.Mult, ast.Div, ast.FloorDiv)):
            left, right = walk(node.left), walk(node.right)
            if left is None or right is None:
                return None
            if isinstance(node.op, ast.Add):
                return left + right
            if isinstance(node.op, ast.Sub):
                return left - right
            if isinstance(node.op, ast.Mult):
                return left * right
            if right == 0:
                return None
            return int(left / right)
        return None

    value = walk(tree)
    return value if isinstance(value, int) and 0 <= value <= 10**12 else None


def _constants(masked: str, code: str) -> dict:
    found: dict[str, int] = {}
    for pattern in (DEFINE_RE, CONST_RE):
        for match in pattern.finditer(masked):
            value = _eval_dim(match.group(2), found)
            if value is not None and 0 <= value <= 10**9:
                found[match.group(1)] = value
    return found


def _arrays(masked: str, line_depth: dict, constants: dict):
    arrays: list[ArrayDecl] = []
    for lineno, line in enumerate(masked.split("\n"), 1):
        for match in ARRAY_DECL_RE.finditer(line):
            type_name = " ".join(match.group("type").split())
            for raw in match.group("decls").split(","):
                named = ARRAY_NAME_RE.search(raw.strip())
                if not named:
                    continue
                dims = tuple(d.strip() for d in DIM_RE.findall(named.group("dims")))
                if not dims:
                    continue
                elements, reason = 1, ""
                for dim in dims:
                    value = _eval_dim(dim, constants)
                    if value is None:
                        elements, reason = None, f"dimension '{dim}' is not statically known"
                        break
                    elements *= value
                arrays.append(ArrayDecl(
                    name=named.group("name"),
                    type_name=type_name,
                    dims=dims,
                    line=lineno,
                    global_scope=line_depth.get(lineno, 0) == 0,
                    elements=elements,
                    reason=reason,
                ))
    return arrays


def _header(tokens, index: int) -> str:
    parts: list[str] = []
    inner = 0
    for token in tokens[index:index + 80]:
        if token.text == "(":
            inner += 1
        elif token.text == ")":
            inner -= 1
            if inner == 0:
                break
        if inner > 0 and token.text != "(":
            parts.append(token.text)
    return " ".join(parts)


def _bound(header: str) -> str:
    if not header:
        return "unknown"
    if re.search(r"[<>]=?\s*\d", header):
        return "constant"
    if re.search(r"[<>]=?\s*[A-Za-z_]", header) or re.search(r"\b(n|m|N|M|k|K)\b", header):
        return "variable"
    return "unknown"


def _loops(tokens):
    loops: list[LoopInfo] = []
    active: list[int] = []
    braces: list[tuple[int, bool]] = []
    pending: tuple[str, int, str] | None = None
    depth = 0
    for index, token in enumerate(tokens):
        if token.kind == "ident" and token.text in ("for", "while"):
            pending = (token.text, token.line, _header(tokens, index + 1))
        elif token.text == "{":
            depth += 1
            if pending:
                kind, line, header = pending
                loops.append(LoopInfo(kind, line, len(active) + 1, header, _bound(header)))
                active.append(depth)
                braces.append((depth, True))
                pending = None
            else:
                braces.append((depth, False))
        elif token.text == "}":
            if braces:
                opened, is_loop = braces.pop()
                if is_loop and active and active[-1] == opened:
                    active.pop()
            depth = max(0, depth - 1)
        elif token.text == ";" and pending:
            kind, line, header = pending
            loops.append(LoopInfo(kind, line, len(active) + 1, header, _bound(header)))
            pending = None
    return loops


def _functions(masked: str, lines: list):
    functions: list[FunctionInfo] = []
    for match in FUNC_RE.finditer(masked):
        return_type, name = match.group(1), match.group(2)
        if return_type in CONTROL_WORDS or name in CONTROL_WORDS:
            continue
        start = masked.count("\n", 0, match.start()) + 1
        open_at = masked.find("{", match.start(), match.end())
        if open_at < 0:
            continue
        depth, close_at = 0, None
        for pos in range(open_at, len(masked)):
            if masked[pos] == "{":
                depth += 1
            elif masked[pos] == "}":
                depth -= 1
                if depth == 0:
                    close_at = pos
                    break
        end = masked.count("\n", 0, close_at) + 1 if close_at else len(lines)
        functions.append(FunctionInfo(name, start, end, masked[open_at:close_at or len(masked)]))
    return functions


def lex(code: str) -> Lexed:
    lines = code.split("\n")
    masked, comments, strings = _mask(code)
    tokens, truncated = _tokenize(masked)
    line_depth, depth_limit_hit = _line_depths(tokens, len(lines), masked)
    constants = _constants(masked, code)
    arrays = _arrays(masked, line_depth, constants)
    loops = _loops(tokens)
    functions = _functions(masked, lines)
    recursion = [
        (fn.name, fn.line)
        for fn in functions
        if re.search(r"\b" + re.escape(fn.name) + r"\s*\(", fn.body[1:])
    ]
    return Lexed(
        code=code,
        lines=lines,
        masked=masked,
        tokens=tokens,
        comments=comments,
        strings=strings,
        arrays=arrays,
        loops=loops,
        functions=functions,
        constants=constants,
        line_depth=line_depth,
        max_loop_depth=max((loop.depth for loop in loops), default=0),
        recursion=recursion,
        depth_limit_hit=depth_limit_hit,
        truncated=truncated,
    )
