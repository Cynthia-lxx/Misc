r"""生成 NTLab 应用图标（仅构建期使用，运行时不需要本脚本与依赖）。

输入：ntlab/resources/icon-black.svg（黑）与 icon-white.svg（白），
输出：ntlab/resources/app.ico —— 白色图形置于深色圆角底板上，
浅色/深色桌面与任务栏均可辨识；另输出 preview.png 供人工核验。

用法：.venv\Scripts\python.exe scripts\make_app_icon.py
依赖：pip install svglib reportlab pillow
"""

from __future__ import annotations

import io
import sys
from pathlib import Path

from PIL import Image, ImageDraw
from reportlab.graphics import renderPM
from svglib.svglib import svg2rlg

RES = Path(__file__).resolve().parents[1] / "ntlab" / "resources"
TILE_BG = (26, 27, 30)          # 与应用深色主题一致的圆角底板
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
    """深色圆角底板 + 白色图形（留 12% 外边距）。"""
    img = Image.new("RGBA", (px, px), (0, 0, 0, 0))
    d = ImageDraw.Draw(img)
    radius = max(2, px // 5)                       # 底板圆角
    d.rounded_rectangle([0, 0, px - 1, px - 1], radius=radius, fill=TILE_BG + (255,))
    inner = int(px * 0.76)                          # 图形占底板 76%
    offset = (px - inner) // 2
    glyph = render_svg(svg, inner * 2).resize((inner, inner), Image.LANCZOS)
    img.alpha_composite(glyph, (offset, offset))
    return img


def main() -> None:
    white = RES / "icon-white.svg"
    if not white.is_file():
        sys.exit(f"缺少 {white}")
    base = build_tile(white, 256)
    base.save(RES / "app.ico", sizes=[(s, s) for s in SIZES])
    base.save(RES / "preview.png")
    # 16px 下的抗锯齿细化：用 LANCZOS 逐级缩
    ico_sizes = {(s, s) for s in SIZES}
    for s in (16, 24, 32):
        small = base.resize((s, s), Image.LANCZOS)
        ico_sizes.discard((s, s))
    print(f"OK: {RES / 'app.ico'} + preview.png ({', '.join(map(str, SIZES))} px)")


if __name__ == "__main__":
    main()
