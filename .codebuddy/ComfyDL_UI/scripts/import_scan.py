"""
import_scan.py — ComfyUI 脱水改造的静态 import 依赖分析脚本

从入口文件（main.py / server.py / execution.py / nodes.py）出发，用 AST 解析
全部 .py 文件的 import 语句，做 BFS 闭包分析，输出：

  --closure      入口依赖闭合集（硬性保留候选，KEEP）
  --candidates   不在闭合集内的模块（删除候选，DELETE）
  --report json  全量分类报告，供 dehydrate_manifest.md 生成参考

用法:
  python import_scan.py <project_root> [--entry a.py,b.py] [--closure]
                                          [--candidates] [--report out.json]
"""
import ast
import argparse
import json
import os
import sys
from collections import deque

# 目录级忽略：虚拟环境、git、字节码缓存等
SKIP_DIRS = {"penv", ".git", "__pycache__", "node_modules", "venv", "env"}


def iter_py(root):
    """递归产出所有 .py 文件路径（跳过忽略目录）。"""
    for dirpath, dirnames, filenames in os.walk(root):
        dirnames[:] = [d for d in dirnames if d not in SKIP_DIRS]
        for fn in filenames:
            if fn.endswith(".py"):
                yield os.path.join(dirpath, fn)


def module_of(path, root):
    """
    绝对路径 -> 模块点路径。
    comfy/__init__.py -> comfy
    comfy/sd.py       -> comfy.sd
    """
    rel = os.path.relpath(path, root)
    rel = rel.replace(os.sep, "/")
    if rel.endswith("/__init__.py"):
        return rel[: -len("/__init__.py")].replace("/", ".")
    if rel.endswith(".py"):
        return rel[:-3].replace("/", ".")
    return None


def parse_import_targets(path, mod, project_modules):
    """
    解析一个文件的所有 import 目标，映射为 project_modules 中的精确模块名集合。
    支持绝对导入（含父包前缀）与相对导入（按当前模块包深度解析）。
    """
    targets = set()
    try:
        with open(path, encoding="utf-8") as f:
            tree = ast.parse(f.read())
    except (OSError, SyntaxError, ValueError):
        return targets

    parts = mod.split(".")
    is_init = os.path.basename(path) == "__init__.py"
    depth = len(parts) - (0 if is_init else 1)  # 当前模块所在包的深度

    def add_candidates(name):
        """将 import 目标拆成完整名+各级父包，取与 project_modules 的交集。"""
        segs = name.split(".")
        for i in range(len(segs), 0, -1):
            cand = ".".join(segs[:i])
            if cand in project_modules:
                targets.add(cand)

    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            for a in node.names:
                add_candidates(a.name)
        elif isinstance(node, ast.ImportFrom):
            if node.level:
                pkg_depth = depth - (node.level - 1)
                if node.module:
                    full = parts[:pkg_depth] + [node.module]
                    add_candidates(".".join(full))
                else:
                    base = ".".join(parts[:pkg_depth])
                    if base:
                        if base in project_modules:
                            targets.add(base)
                        # from . import x —— x 通常是同包模块，直接尝试拼接
                        for name in node.names:
                            cand = f"{base}.{name}"
                            if cand in project_modules:
                                targets.add(cand)
            elif node.module:
                add_candidates(node.module)
    return targets


def build_graph(root):
    """module -> set(依赖的 project 内模块)。"""
    project_modules = set()
    for path in iter_py(root):
        m = module_of(path, root)
        if m:
            project_modules.add(m)
    graph = {}
    for path in iter_py(root):
        m = module_of(path, root)
        if m:
            graph[m] = parse_import_targets(path, m, project_modules)
    return graph


def closure(entries, graph):
    """BFS 闭包：入口模块集合可达的项目内模块集合。"""
    seen = set(e for e in entries if e in graph)
    q = deque(seen)
    while q:
        mod = q.popleft()
        for target in graph.get(mod, ()):
            if target not in seen:
                seen.add(target)
                q.append(target)
    return seen


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("project_root", help="ComfyUI 项目根目录")
    ap.add_argument("--entry", default="main.py,server.py,execution.py,nodes.py",
                    help="入口模块（逗号分隔，.py 省略）")
    ap.add_argument("--closure", action="store_true", help="打印闭合集")
    ap.add_argument("--candidates", action="store_true", help="打印删除候选")
    ap.add_argument("--report", help="输出 JSON 报告路径")
    args = ap.parse_args()

    root = os.path.abspath(args.project_root)
    if not os.path.isdir(root):
        print(f"[ERR] 目录不存在: {root}", file=sys.stderr)
        sys.exit(1)

    graph = build_graph(root)
    entries = []
    for e in args.entry.split(","):
        e = e.strip()
        if not e:
            continue
        if not e.endswith(".py"):
            e += ".py"
        m = module_of(os.path.join(root, e), root)
        if m in graph:
            entries.append(m)
    entries = list(dict.fromkeys(entries))

    kept = closure(entries, graph)
    project = set(graph.keys())
    candidates = sorted(project - kept)
    kept_sorted = sorted(kept)

    print(f"扫描模块总数: {len(project)}")
    print(f"入口: {entries}")
    print(f"闭合集（保留候选）: {len(kept_sorted)}")
    print(f"删除候选: {len(candidates)}")

    if args.closure:
        print("\n===== KEEP 闭合集 =====")
        for m in kept_sorted:
            print(m)

    if args.candidates:
        print("\n===== DELETE 候选 =====")
        for m in candidates:
            print(m)

    if args.report:
        payload = {
            "root": root,
            "entries": entries,
            "keep_closure": kept_sorted,
            "delete_candidates": candidates,
            "graph": {m: sorted(v) for m, v in sorted(graph.items())},
        }
        with open(args.report, "w", encoding="utf-8") as f:
            json.dump(payload, f, ensure_ascii=False, indent=2)
        print(f"\n报告已写入: {args.report}")


if __name__ == "__main__":
    main()
