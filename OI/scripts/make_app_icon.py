r"""生成 OI 助手应用图标（仅构建期使用，运行时不需要本脚本与依赖）。

与 NTLab scripts/make_app_icon.py 同一方法：
输入 C:\Users\wjz-s\Downloads\code.svg（复制为 scripts/icon.svg 做存档），
输出 web/static/assets/app.ico —— 图形置于深色圆角底板上（GUI 是深色主题），
浅色/深色桌面与任务栏均可辨识；另输出 preview.png 供人工核验。

用法：P:\Dev\NTLab\.venv\Scripts\python.exe scripts\make_app_icon.py
依赖：pip install svglib reportlab pillow（NTLab venv 已具备）
"""

from __future__ import annotations

import io
import shutil
import sys
from pathlib import Path

from PIL import Image, ImageDraw
from reportlab.graphics import renderPM
from svglib.svglib import svg2rlg

SCRIPTS = Path(__file__).resolve().parent
ASSETS = SCRIPTS.parent / "web" / "static" / "assets"
SOURCE = Path(r"C:\Users\wjz-s\Downloads\code.svg")
ARCHIVE = SCRIPTS / "icon.svg"          # SVG 存档进仓库（Downloads 易丢）
TILE_BG = (20, 20, 20)                  # 与 OI 深色主题一致的圆角底板
SIZES = [16, 24, 32, 48, 64, 128, 256]


def render_svg(svg_path: Path, px: int) -> Image.Image:
    """SVG → 透明底 RGBA 光栅图（正方形）。"""
    drawing = svg2rlg(str(svg_path))
    k = px / drawing.width
    drawing.width = drawing.height = px
    drawing.scale(k, k)
    png = renderPM.drawToString(drawing, fmt="PNG", bg=0)
    img = Image.open(io.BytesIO(png)).convert("RGBA")
    if img.size != (px, px):
        img = img.resize((px, px), Image.LANCZOS)
    return img


def build_tile(svg: Path, px: int) -> Image.Image:
    """深色圆角底板 + 原色图形（留 12% 外边距）。

    与 NTLab 的差异：code.svg 自带配色，不强制染白（作者设计优先）。
    """
    img = Image.new("RGBA", (px, px), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    radius = max(2, px // 5)
    d.rounded_rectangle([0, 0, px - 1, px - 1], radius=radius, fill=TILE_BG + (255,))
    inner = int(px * 0.76)
    offset = (px - inner) // 2
    glyph = render_svg(svg, inner * 2).resize((inner, inner), Image.LANCZOS)
    img.alpha_composite(glyph, (offset, offset))
    return img


def main() -> None:
    src = SOURCE if SOURCE.is_file() else ARCHIVE
    if not src.is_file():
        sys.exit(f"缺少 {SOURCE}（也未有存档 {ARCHIVE}）")
    if src == SOURCE and not ARCHIVE.is_file():
        shutil.copy2(SOURCE, ARCHIVE)   # SVG 存档进仓库
    base = build_tile(src, 256)
    ASSETS.mkdir(parents=True, exist_ok=True)
    base.save(ASSETS / "app.ico", sizes=[(s, s) for s in SIZES])
    base.save(ASSETS / "icon-preview.png")
    print(f"OK: {ASSETS / 'app.ico'} + icon-preview.png ({', '.join(map(str, SIZES))} px)")


if __name__ == "__main__":
    main()
