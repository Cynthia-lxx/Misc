# -*- coding: utf-8 -*-
"""
requirements.txt 逐项审计：对比脱水后保留文件的实际第三方 import，
输出每个依赖的使用文件列表（缺失=可剔除候选）。
"""
import ast
import os
import re
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"p:\Dev\ComfyUI_Refs\ComfyDL_UI"
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dep_audit import walk_keep_files, third_party_imports

# requirements.txt 中的包名 -> 顶层 import 名（映射）
REQ = [
    ("comfyui-frontend-package", ["comfyui_frontend_package"]),
    ("comfyui-workflow-templates", ["comfyui_workflow_templates"]),
    ("comfyui-embedded-docs", ["comfyui_embedded_docs"]),
    ("torch", ["torch"]),
    ("torchsde", ["torchsde"]),
    ("torchvision", ["torchvision"]),
    ("torchaudio", ["torchaudio"]),
    ("numpy", ["numpy"]),
    ("einops", ["einops"]),
    ("transformers", ["transformers"]),
    ("tokenizers", ["tokenizers"]),
    ("sentencepiece", ["sentencepiece"]),
    ("safetensors", ["safetensors"]),
    ("aiohttp", ["aiohttp"]),
    ("yarl", ["yarl"]),
    ("pyyaml", ["yaml"]),
    ("Pillow", ["PIL"]),
    ("scipy", ["scipy"]),
    ("tqdm", ["tqdm"]),
    ("psutil", ["psutil"]),
    ("alembic", ["alembic"]),
    ("SQLAlchemy", ["sqlalchemy"]),
    ("filelock", ["filelock"]),
    ("av", ["av"]),
    ("comfy-kitchen", ["comfy_kitchen"]),
    ("comfy-aimdo", ["comfy_aimdo"]),
    ("requests", ["requests"]),
    ("simpleeval", ["simpleeval"]),
    ("blake3", ["blake3"]),
    # 非必要
    ("kornia", ["kornia"]),
    ("spandrel", ["spandrel"]),
    ("pydantic", ["pydantic"]),
    ("pydantic-settings", ["pydantic_settings"]),
    ("PyOpenGL", ["OpenGL"]),
    ("comfy-angle", ["comfy_angle", "comfy_angle_utils"]),
]


def main():
    files = walk_keep_files(ROOT)
    usage = {}
    for fp in files:
        for top in third_party_imports(fp):
            usage.setdefault(top, set()).add(os.path.relpath(fp, ROOT))

    print(f"{'依赖包':<28}{'状态':<6}使用文件")
    print("-" * 100)
    for name, tops in REQ:
        hits = set()
        for t in tops:
            hits |= usage.get(t, set())
        if hits:
            print(f"{name:<28}使用  {len(hits)} 文件")
            for fp in sorted(hits)[:8]:
                print(f"    {fp}")
        else:
            print(f"{name:<28}未使用  (建议剔除/注释)")
    print("\n[额外] 保留文件中出现但 requirements.txt 未列出的第三方包:")
    known = set()
    for _, tops in REQ:
        known |= set(tops)
    stdlib = set(__import__("sys").stdlib_module_names)
    extra = {t for t in usage if t not in known and t not in stdlib
             and not t.startswith("_") and t not in ("abc",)}
    for t in sorted(extra):
        print(f"    {t}: {len(usage[t])} 文件")


if __name__ == "__main__":
    main()
