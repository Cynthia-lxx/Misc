# -*- coding: utf-8 -*-
"""
第三方依赖审计：扫描"脱水后保留文件集"的实际顶层 import，
输出被使用的第三方包集合，用于精简 requirements.txt。

用法:
    python dep_audit.py <ComfyDL_UI 根目录>
"""
import ast
import os
import re
import sys

ROOT = sys.argv[1] if len(sys.argv) > 1 else r"p:\Dev\ComfyUI_Refs\ComfyDL_UI"

# 项目内顶层包/模块（非第三方）
PROJECT_TOPS = {
    "comfy", "comfy_execution", "comfy_api", "comfy_extras", "comfy_api_nodes",
    "app", "api_server", "middleware", "utils", "comfy_config", "alembic_db",
    "nodes", "server", "execution", "folder_paths", "node_helpers",
    "latent_preview", "protocol", "comfyui_version", "cuda_malloc",
    "hook_breaker_ac10a0", "main", "custom_nodes", "importlib", "json",
}

# comfy/ 下被删除的生成子目录
DEL_DIRS = {
    "ldm", "text_encoders", "k_diffusion", "sd1_tokenizer", "audio_encoders",
    "image_encoders", "extra_samplers", "taesd", "cldm", "t2i_adapter",
    "weight_adapter", "background_removal",
}

# comfy/ 下被删除的生成单文件（§6.2A manifest）
DEL_FILES = {
    "diffusers_load.py", "diffusers_convert.py", "samplers.py", "sample.py",
    "sampler_helpers.py", "controlnet.py", "clip_vision.py", "clip_model.py",
    "sd1_clip.py", "sdxl_clip.py", "lora.py", "lora_convert.py", "model_base.py",
    "model_detection.py", "model_sampling.py", "latent_formats.py", "conds.py",
    "context_windows.py", "supported_models.py", "supported_models_base.py",
    "pixel_space_convert.py", "gligen.py", "bg_removal_model.py", "rmsnorm.py",
    "nested_tensor.py",
}

# comfy_extras 保留名单（§4.3 manifest）
EXTRAS_KEEP = {
    "nodes_mask.py", "nodes_images.py", "nodes_post_processing.py",
    "nodes_compositing.py", "nodes_rebatch.py", "nodes_resolution.py",
    "nodes_color.py", "nodes_logic.py", "nodes_math.py", "nodes_primitive.py",
    "nodes_number_convert.py", "nodes_nop.py", "nodes_curve.py",
    "nodes_string.py", "nodes_text.py", "nodes_latent.py", "nodes_toolkit.py",
}

# 整个目录排除（脱水后整删或非运行代码）
SKIP_DIRS = {"comfy_api_nodes", "tests", "tests-unit", "script_examples",
             "blueprints", "__pycache__", "penv", ".git", "input", "output",
             "models", "docs", "node_modules"}


def is_project_mod(top: str) -> bool:
    if top in PROJECT_TOPS:
        return True
    # 以 comfy 开头但非项目模块（如 comfy_kitchen 以 comfy_ 开头）——注意 comfy_kitchen/comfy_aimdo 是第三方！
    if top in ("comfy_kitchen", "comfy_aimdo"):
        return False
    return False


def third_party_imports(filepath: str) -> set:
    """提取文件顶层 import 的第三方包名。"""
    try:
        with open(filepath, "r", encoding="utf-8", errors="replace") as f:
            tree = ast.parse(f.read(), filename=filepath)
    except (SyntaxError, UnicodeDecodeError):
        return set()
    tops = set()
    for node in ast.walk(tree):
        if isinstance(node, ast.Import):
            for alias in node.names:
                top = alias.name.split(".")[0]
                if not is_project_mod(top) and top != "__future__":
                    tops.add(top)
        elif isinstance(node, ast.ImportFrom):
            mod = node.module or ""
            top = mod.split(".")[0]
            if not is_project_mod(top) and top != "__future__":
                tops.add(top)
    return tops


def walk_keep_files(root: str):
    """按脱水后保留规则遍历文件。"""
    files = []
    # 根目录 *.py
    for f in os.listdir(root):
        if f.endswith(".py") and f not in DEL_FILES:
            files.append(os.path.join(root, f))
    # 根目录 yaml.example / ini
    for extra in ("extra_model_paths.yaml.example", "alembic.ini"):
        p = os.path.join(root, extra)
        if os.path.exists(p):
            files.append(p)

    def walk(pkg_dir, project_prefix, skip_dirs=(), is_comfy=False):
        for dirpath, dirnames, filenames in os.walk(pkg_dir):
            dirnames[:] = [d for d in dirnames
                           if d not in SKIP_DIRS and d not in skip_dirs and not d.startswith(".")]
            for fn in filenames:
                if not fn.endswith(".py"):
                    continue
                full = os.path.join(dirpath, fn)
                rel = os.path.relpath(full, pkg_dir).replace("\\", "/")
                if is_comfy:
                    # comfy 包：排除生成子目录与生成单文件
                    top = rel.split("/")[0]
                    if top in DEL_DIRS:
                        continue
                    if rel in DEL_FILES or fn in DEL_FILES:
                        continue
                files.append(full)

    walk(os.path.join(root, "comfy"), "comfy", is_comfy=True)
    for sub in ("comfy_execution", "comfy_api", "app", "api_server",
                "middleware", "utils", "comfy_config", "alembic_db"):
        p = os.path.join(root, sub)
        if os.path.isdir(p):
            walk(p, sub)
    # comfy_extras 保留名单
    p = os.path.join(root, "comfy_extras")
    if os.path.isdir(p):
        for fn in EXTRAS_KEEP:
            fp = os.path.join(p, fn)
            if os.path.exists(fp):
                files.append(fp)
    return files


def main():
    files = walk_keep_files(ROOT)
    all_tops = {}
    for fp in files:
        for top in third_party_imports(fp):
            all_tops.setdefault(top, set()).add(os.path.relpath(fp, ROOT))
    print(f"扫描文件数: {len(files)}")
    print("第三方顶层包使用情况:")
    for top in sorted(all_tops):
        print(f"  {top}  <- {len(all_tops[top])} 文件")
        # 列出前 6 个使用文件
        for fp in sorted(all_tops[top])[:6]:
            print(f"      {fp}")
    print("\n[汇总] 用到的第三方包:")
    print(" ".join(sorted(all_tops)))


if __name__ == "__main__":
    main()
