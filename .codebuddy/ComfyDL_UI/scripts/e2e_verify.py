#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ComfyDL_UI dehydration task-6 end-to-end workflow verification (§7-3).

Scenario: LoadImage -> one ComfyDL Image Tools node (IMAGE->IMAGE) -> SaveImage,
submitted via the real HTTP prompt API. Asserts prompt success and that a PNG
lands in the output directory.

This script lives in the workspace .codebuddy (not part of the repo) and may be
re-run at any time. Exit code 0 = pass.
"""
import json
import os
import subprocess
import sys
import time
import uuid
from pathlib import Path

import requests

ROOT = Path(r"P:\Dev\ComfyUI_Refs\ComfyDL_UI")
PY = ROOT / "penv" / "Scripts" / "python.exe"
PORT = 8188
INPUT_IMG = "verify_e2e.png"
OUT_PREFIX = "verify_e2e_out"
SYS_LOG = Path(os.environ.get("TEMP", ".")) / "comfydl_e2e_server.log"
API = f"http://127.0.0.1:{PORT}"


def wait_server(timeout=180.0):
    t0 = time.time()
    while time.time() - t0 < timeout:
        try:
            r = requests.get(f"{API}/system_stats", timeout=3)
            if r.status_code == 200:
                return
        except requests.RequestException:
            pass
        if server.poll() is not None:
            tail = SYS_LOG.read_text(encoding="utf-8", errors="replace") if SYS_LOG.exists() else ""
            raise SystemExit(f"server exited early rc={server.returncode}\n{tail[-4000:]}")
        time.sleep(2)
    raise SystemExit("server not ready within timeout")


def widget_value(spec):
    t = spec[0]
    meta = spec[1] if len(spec) > 1 else {}
    if t == "INT":
        return meta.get("default", 1)
    if t == "FLOAT":
        return meta.get("default", 0.5)
    if t == "BOOLEAN":
        return meta.get("default", True)
    if t == "STRING":
        return meta.get("default", "")
    if isinstance(t, list):  # combo
        if meta.get("default") in t:
            return meta["default"]
        return t[0]
    return None  # not a widget (external slot or unsupported)


def pick_image_node(obj_info):
    """Pick a ComfyDL (Cdl*) IMAGE->IMAGE node whose non-IMAGE required inputs are all widgets."""
    candidates = []
    for cls, info in obj_info.items():
        if not cls.startswith("Cdl"):
            continue
        outs = info.get("output", [])
        if "IMAGE" not in outs:
            continue
        req = info.get("input", {}).get("required", {})
        img_names = [n for n, spec in req.items() if spec[0] == "IMAGE"]
        if len(img_names) != 1:
            continue
        plan = {n: widget_value(spec) for n, spec in req.items() if spec[0] != "IMAGE"}
        if any(v is None for v in plan.values()):
            continue
        candidates.append((cls, img_names[0], plan))
    if not candidates:
        return None
    by_name = {c: (i, p) for c, i, p in candidates}
    if "CdlImageGrayscale" in by_name:
        cls, img_in, plan = "CdlImageGrayscale", *by_name["CdlImageGrayscale"]
    else:
        cls, img_in, plan = candidates[0]
    return cls, img_in, plan


def main():
    global server
    (ROOT / "input").mkdir(exist_ok=True)

    # 1) prepare test image
    from PIL import Image
    img = Image.new("RGB", (64, 48), (20, 120, 220))
    img.save(ROOT / "input" / INPUT_IMG)

    # 2) boot server
    with SYS_LOG.open("w", encoding="utf-8", errors="replace") as lf:
        server = subprocess.Popen(
            [str(PY), "main.py", "--cpu", "--port", str(PORT)],
            cwd=str(ROOT), stdout=lf, stderr=subprocess.STDOUT,
        )
    wait_server()

    # 3) pick node from live registry
    obj_info = requests.get(f"{API}/object_info").json()
    picked = pick_image_node(obj_info)
    assert picked, "no ComfyDL IMAGE->IMAGE node found"
    node_cls, image_in, plan = picked
    print(f"picked node: {node_cls}  image_input={image_in}  widget_plan={plan}")

    # 4) submit prompt
    graph = {
        "1": {"class_type": "LoadImage", "inputs": {"image": INPUT_IMG}},
        "2": {"class_type": node_cls, "inputs": {image_in: ["1", 0], **plan}},
        "3": {"class_type": "SaveImage", "inputs": {"images": ["2", 0], "filename_prefix": OUT_PREFIX}},
    }
    cid = str(uuid.uuid4())
    r = requests.post(f"{API}/prompt", json={"prompt": graph, "client_id": cid}, timeout=30)
    payload = r.json()
    if "prompt_id" not in payload:
        raise SystemExit(f"prompt rejected: {payload}")

    pid = payload["prompt_id"]
    t0 = time.time()
    hist = None
    while time.time() - t0 < 180:
        hist = requests.get(f"{API}/history/{pid}", timeout=10).json()
        rec = hist.get(pid)
        if rec and rec.get("status", {}).get("completed"):
            break
        time.sleep(2)
    assert hist and pid in hist, f"prompt not completed: {list(hist)}"

    st = hist[pid]["status"]
    print(f"prompt {pid} completed={st.get('completed')} status_str={st.get('status_str')}")
    outputs = hist[pid].get("outputs", {})
    imgs = []
    for node_out in outputs.values():
        for image in node_out.get("images", []):
            imgs.append(ROOT / "output" / image["filename"])
    print(f"output images: {[p.name for p in imgs]}")
    assert imgs and all(p.exists() and p.stat().st_size > 0 for p in imgs), "output PNG missing/empty"
    print("E2E PASS")
    return 0


if __name__ == "__main__":
    server = None
    try:
        sys.exit(main())
    finally:
        if server is not None and server.poll() is None:
            server.terminate()
            try:
                server.wait(timeout=10)
            except subprocess.TimeoutExpired:
                server.kill()
