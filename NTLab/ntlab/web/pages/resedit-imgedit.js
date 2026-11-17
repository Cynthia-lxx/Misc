/* 图像编辑器 Overlay —— ShearedOverlayContainer 式（对齐 osu.Game/Overlays/Mods/ShearedOverlayContainer.cs）。
 *
 * 结构：遮罩 Background6@0.75 → sheared header 卡片（圆角14 双层、标题/描述/关闭）→
 * 主区（左工具列 / 画布 / 右图层面板+调色盘）→ 底部操作条。
 * 入场：整体 fade 400ms OutQuint，header MoveToY(0)，三面板 ±700px 交替 30ms 错峰
 * （对齐 ModSelectOverlay.PopIn）；出场 500ms 反向。Esc / 点击遮罩关闭。
 *
 * 编辑模型：图层（PhotoShop 式，base 图像 + 贴图层，可导入/剪贴板粘贴/拖放）；
 * 工具：画笔 / 铅笔（硬边小半径）/ 荧光笔（低 alpha）/ 二值笔（阈值无 AA）/ 直线 /
 * 矩形 / 椭圆（空心/实心）/ 吸管；半径与不透明度按工具独立记忆；调色盘：
 * HSL 色轮 + S/L 方块、RGBA 数值、色号输入、屏幕取色器（EyeDropper）、透明色（抠图）；
 * 画布自动适配尺寸，滚轮缩放（指向光标）/ 空格或中键平移；撤销栈。
 */

import { t } from "../i18n.js";

const QUART_IN = 'cubic-bezier(0.55, 0.055, 0.675, 0.19)';  // Easing.InQuad

/* ---------------- 小工具 ---------------- */

function el(tag, cls, text) {
  const d = document.createElement(tag);
  if (cls) d.className = cls;
  if (text != null) d.textContent = text;
  return d;
}

function svg(pathD, viewBox = '0 0 24 24') {
  const ns = 'http://www.w3.org/2000/svg';
  const svg = document.createElementNS(ns, 'svg');
  svg.setAttribute('viewBox', viewBox);
  const p = document.createElementNS(ns, 'path');
  p.setAttribute('d', pathD);
  svg.appendChild(p);
  return svg;
}

function clamp(v, lo, hi) { return Math.min(hi, Math.max(lo, v)); }

function hslToRgb(h, s, l) {
  h = ((h % 360) + 360) % 360;
  const c = (1 - Math.abs(2 * l - 1)) * s;
  const x = c * (1 - Math.abs(((h / 60) % 2) - 1));
  const m = l - c / 2;
  let r = 0, g = 0, b = 0;
  if (h < 60) [r, g, b] = [c, x, 0];
  else if (h < 120) [r, g, b] = [x, c, 0];
  else if (h < 180) [r, g, b] = [0, c, x];
  else if (h < 240) [r, g, b] = [0, x, c];
  else if (h < 300) [r, g, b] = [x, 0, c];
  else [r, g, b] = [c, 0, x];
  return {
    r: Math.round((r + m) * 255),
    g: Math.round((g + m) * 255),
    b: Math.round((b + m) * 255),
  };
}

function rgbToHsl(r, g, b) {
  r /= 255; g /= 255; b /= 255;
  const max = Math.max(r, g, b), min = Math.min(r, g, b);
  const l = (max + min) / 2;
  if (max === min) return { h: 0, s: 0, l };
  const d = max - min;
  const s = d / (1 - Math.abs(2 * l - 1));
  let h = 0;
  if (max === r) h = 60 * (((g - b) / d) % 6);
  else if (max === g) h = 60 * ((b - r) / d + 2);
  else h = 60 * ((r - g) / d + 4);
  return { h: ((h % 360) + 360) % 360, s, l };
}

function rgbCss(rgb, a = 1) {
  return `rgba(${rgb.r}, ${rgb.g}, ${rgb.b}, ${a})`;
}

function cssColor(state) {
  if (state.erase) return 'rgba(0, 0, 0, 1)';
  return rgbCss(hslToRgb(state.h, state.s, state.l), state.a);
}

/** ImageData(RGBA) → 32bpp 自底向上 DIB；withMask 时附 AND mask（ICON/CURSOR）。 */
function encodeDib(imageData, withMask) {
  const W = imageData.width, H = imageData.height, d = imageData.data;
  const stride = ((W + 31) >> 5) << 2;
  const maskBytes = withMask ? stride * H : 0;
  const buf = new ArrayBuffer(40 + W * 4 * H + maskBytes);
  const dv = new DataView(buf);
  dv.setUint32(0, 40, true);
  dv.setInt32(4, W, true);
  dv.setInt32(8, withMask ? H * 2 : H, true);
  dv.setUint16(12, 1, true);
  dv.setUint16(14, 32, true);
  dv.setUint32(16, 0, true);
  dv.setUint32(20, W * 4 * H + maskBytes, true);
  const u8 = new Uint8Array(buf);
  let p = 40;
  for (let y = H - 1; y >= 0; y--) {
    for (let x = 0; x < W; x++) {
      const i = (y * W + x) * 4;
      u8[p++] = d[i + 2]; u8[p++] = d[i + 1]; u8[p++] = d[i]; u8[p++] = d[i + 3];
    }
  }
  if (withMask) {
    for (let y = H - 1; y >= 0; y--) {
      for (let x = 0; x < W; x++) {
        if (d[(y * W + x) * 4 + 3] < 128) u8[p + (x >> 3)] |= (0x80 >> (x & 7));
      }
      p += stride;
    }
  }
  return new Uint8Array(buf);
}

function loadImageEl(blob) {
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => resolve(img);
    img.onerror = () => reject(new Error(t('图像解码失败')));
    img.src = URL.createObjectURL(blob);
  });
}

/* ---------------- 工具定义 ---------------- */

const TOOLS = [
  { id: 'brush',       label: '画笔',    radius: 6,  alpha: 1,    icon: 'M4.5 19.5c0-2 .8-3.2 2.4-3.7L17.5 5.2 19.8 7.5 9.3 18c-.6 1.6-1.8 2.4-3.7 2.4zM14.5 8.2l2.3 2.3' },
  { id: 'pencil',      label: '铅笔',    radius: 1,  alpha: 1,    icon: 'M4 20l1-4L16.5 4.5 19.5 7.5 8 19l-4 1zM14 7l3 3' },
  { id: 'highlighter', label: '荧光笔',  radius: 18, alpha: 0.3,  icon: 'M6 18h12M4 21h16M8.5 15.5 17.5 4l3.5 2.5-8 10.5H7z' },
  { id: 'binary',      label: '二值笔',  radius: 4,  alpha: 1,    icon: 'M5 5h6v6H5zM13 5h6v6h-6zM5 13h6v6H5zM13 13h6v6h-6z' },
  { id: 'line',        label: '直线',    radius: 3,  alpha: 1,    icon: 'M4 20 20 4M4 20h3M4 17v3M20 4h-3M17 4h3v3' },
  { id: 'rect',        label: '矩形',    radius: 3,  alpha: 1,    icon: 'M4 6h16v12H4z' },
  { id: 'ellipse',     label: '椭圆',    radius: 3,  alpha: 1,    icon: 'M12 6c4.4 0 8 2.7 8 6s-3.6 6-8 6-8-2.7-8-6 3.6-6 8-6z' },
  { id: 'picker',      label: '吸管',    radius: 0,  alpha: 1,    icon: 'M19 3.5a2.1 2.1 0 0 0-3 0L13.5 6l1.5 1.5L17.5 5zM13 7.5 5.5 15 4 20l5-1.5L16.5 11' },
];

/* ---------------- Overlay ---------------- */

export function openImageEditor({ meta, blob, outFormat, origBytes, onApply }) {
  return new Promise((resolve) => {
    const overlay = el('div', 'imged-overlay');
    overlay.setAttribute('role', 'dialog');
    overlay.setAttribute('aria-label', t('图像编辑器'));

    /* ---------- 骨架 ---------- */
    const backdrop = el('div', 'imged-backdrop');
    const root = el('div', 'imged-root');

    // header 卡片（ShearedOverlayHeader：underlay + content 双层圆角）
    const header = el('div', 'imged-header');
    const headerUnder = el('div', 'imged-header-under');
    const headerCard = el('div', 'imged-header-card');
    const titleBox = el('div', 'imged-header-title');
    titleBox.append(
      el('h2', null, t('图像编辑器')),
      el('p', null, t('{type} / {name}（{lang}）— 修改实时绘制在图层上，「应用」后进入暂存区',
        { type: meta.type, name: meta.name, lang: meta.lang })),
    );
    const closeBtn = el('button', 'imged-close');
    closeBtn.type = 'button';
    closeBtn.setAttribute('aria-label', t('关闭'));
    closeBtn.append(svg('M6 6l12 12M18 6 6 18'));
    headerCard.append(titleBox, closeBtn);
    header.append(headerUnder, headerCard);

    // 主区：一块大的 sheared 底板（纯装饰，允许出血）+ 内部直立小分区区块。
    // 不给小面板各自斜切：垂直排布下窄窗口的可用安全宽度趋近零，斜切角必越界被裁。
    const main = el('div', 'imged-main');
    const plate = el('div', 'imged-plate');
    const toolsSection = el('div', 'imged-section imged-tools');
    toolsSection.style.setProperty('--i', '0');
    const viewport = el('div', 'imged-viewport');
    viewport.style.setProperty('--i', '1');
    const rightCol = el('div', 'imged-rightcol');
    const layersSection = el('div', 'imged-section imged-layers');
    layersSection.style.setProperty('--i', '1');
    const paletteSection = el('div', 'imged-section imged-palette');
    paletteSection.style.setProperty('--i', '2');
    rightCol.append(layersSection, paletteSection);
    main.append(plate, toolsSection, viewport, rightCol);

    // footer
    const footer = el('div', 'imged-footer');
    const footerLeft = el('div', 'imged-footer-left');
    const footerRight = el('div', 'imged-footer-right');
    footer.append(footerLeft, footerRight);

    root.append(header, main, footer);
    overlay.append(backdrop, root);
    document.body.appendChild(overlay);

    /* ---------- 画布栈 ---------- */
    const stage = el('div', 'imged-stage');
    const checker = el('div', 'imged-checker');
    const previewCanvas = document.createElement('canvas');   // 形状拖拽预览
    previewCanvas.className = 'imged-preview';
    const ghostCanvas = document.createElement('canvas');     // 半透明笔迹实时预览（最终一次合成，避免重叠段变深）
    ghostCanvas.className = 'imged-ghost';
    const brushRing = el('div', 'imged-brush-ring');
    viewport.append(stage, brushRing);
    stage.append(checker, previewCanvas);

    const zoombar = el('div', 'imged-zoombar');
    viewport.appendChild(zoombar);

    let docW = 0, docH = 0;
    let layers = [];            // { name, canvas, ctx, visible, opacity } 底 → 顶
    let activeIdx = 0;
    let layerSeq = 0;
    let tool = 'brush';
    const toolState = new Map(TOOLS.filter(t => t.radius > 0).map(t => [t.id, { radius: t.radius, alpha: t.alpha }]));
    const color = { h: 0, s: 0, l: 0, a: 1, erase: false };   // 默认红，方便第一时间看到笔迹
    color.h = 358; color.s = 0.85; color.l = 0.58;
    const recent = [];
    const undoStack = [];
    let zoom = 1, panX = 0, panY = 0;
    let strokeLayer = document.createElement('canvas');
    let strokeCtx = null;
    let drawing = false, spacePan = false, panDrag = false, lastPan = null;
    let shapeStart = null, shiftKey = false;
    let applied = false;

    const activeLayer = () => layers[activeIdx];

    /* ---------- 初始化：加载原图为底层 ---------- */
    (async () => {
      const img = await loadImageEl(blob);
      docW = img.naturalWidth;
      docH = img.naturalHeight;
      checker.style.width = `${docW}px`;
      checker.style.height = `${docH}px`;
      previewCanvas.width = docW; previewCanvas.height = docH;
      ghostCanvas.width = docW; ghostCanvas.height = docH;
      strokeLayer.width = docW; strokeLayer.height = docH;
      strokeCtx = strokeLayer.getContext('2d');
      addLayerFromImage(img, t('原图'), true);
      previewCtx = previewCanvas.getContext('2d');
      ghostCtx = ghostCanvas.getContext('2d');
      overlay.classList.add('open');           // PopIn：400ms OutQuint
      // 适配视图必须在 overlay 可见（display 恢复）且布局完成之后，否则 rect=0
      requestAnimationFrame(() => requestAnimationFrame(fitView));
    })().catch((err) => {
      headerCard.querySelector('p').textContent = t('加载失败：{err}', { err: err.message });
    });

    let previewCtx = null;
    let ghostCtx = null;

    /* ---------- 图层 ---------- */
    function addLayerFromImage(imgOrCanvas, name, asBase = false) {
      const canvas = document.createElement('canvas');
      canvas.width = docW; canvas.height = docH;
      const ctx = canvas.getContext('2d');
      if (imgOrCanvas) {
        const w = imgOrCanvas.naturalWidth || imgOrCanvas.width;
        const h = imgOrCanvas.naturalHeight || imgOrCanvas.height;
        ctx.drawImage(imgOrCanvas, Math.round((docW - w) / 2), Math.round((docH - h) / 2));
      }
      const layer = { name, canvas, ctx, visible: true, opacity: 1, thumb: null };
      if (asBase) {
        layers.push(layer);
        activeIdx = layers.length - 1;
      } else {
        layers.splice(activeIdx + 1, 0, layer);
        activeIdx += 1;
        layer.name = t('图层 {n}', { n: ++layerSeq });
      }
      syncLayerDOM();
      renderLayerPanel();
    }

    function addLayerFromBlob(blob2, name) {
      loadImageEl(blob2).then((img) => {
        addLayerFromImage(img, name);
        pushToast(t('已导入贴图 {w}×{h}', { w: img.naturalWidth, h: img.naturalHeight }));
      }).catch(() => pushToast(t('图像解码失败')));
    }

    function addEmptyLayer() {
      addLayerFromImage(null, t('图层 {n}', { n: ++layerSeq }));
    }

    function deleteActiveLayer() {
      if (layers.length <= 1) return pushToast(t('至少保留一个图层'));
      snapshotForUndo();
      layers.splice(activeIdx, 1);
      activeIdx = Math.min(activeIdx, layers.length - 1);
      syncLayerDOM();
      renderLayerPanel();
    }

    function moveLayer(dir) {
      const j = activeIdx + dir;
      if (j < 0 || j >= layers.length) return;
      [layers[activeIdx], layers[j]] = [layers[j], layers[activeIdx]];
      activeIdx = j;
      syncLayerDOM();
      renderLayerPanel();
    }

    function mergeDown() {
      if (activeIdx === 0) return pushToast(t('底层无法向下合并'));
      snapshotForUndo();
      const top = layers[activeIdx];
      const low = layers[activeIdx - 1];
      low.ctx.globalAlpha = top.opacity;
      low.ctx.drawImage(top.canvas, 0, 0);
      low.ctx.globalAlpha = 1;
      layers.splice(activeIdx, 1);
      activeIdx -= 1;
      syncLayerDOM();
      renderLayerPanel();
    }

    function syncLayerDOM() {
      // 按数组顺序（底 → 顶）重新挂载，保证绘制顺序：checker → 图层… → ghost → 形状预览
      for (const layer of layers) {
        layer.canvas.className = 'imged-layer';
        layer.canvas.style.opacity = layer.visible ? layer.opacity : 0;
        stage.appendChild(layer.canvas);
      }
      stage.appendChild(ghostCanvas);
      stage.appendChild(previewCanvas);
    }

    function renderLayerPanel() {
      listEl.replaceChildren();
      // 顶 → 底显示（PhotoShop 习惯）
      for (let i = layers.length - 1; i >= 0; i--) {
        const layer = layers[i];
        const item = el('div', 'imged-layer-item' + (i === activeIdx ? ' active' : ''));
        item.style.setProperty('--i', String(layers.length - 1 - i));
        const eye = el('button', 'imged-eye' + (layer.visible ? ' on' : ''));
        eye.type = 'button';
        eye.title = layer.visible ? t('隐藏图层') : t('显示图层');
        eye.append(svg(layer.visible
          ? 'M2 12s3.5-6 10-6 10 6 10 6-3.5 6-10 6-10-6-10-6zM12 9.5a2.5 2.5 0 1 0 0 5 2.5 2.5 0 0 0 0-5z'
          : 'M3 3l18 18M10 5.3A9.8 9.8 0 0 1 12 5c6.5 0 10 7 10 7a15 15 0 0 1-3.2 3.9M6.6 6.6A15 15 0 0 0 2 12s3.5 7 10 7c1.5 0 2.9-.3 4.1-.9M9.9 9.9a2.5 2.5 0 0 0 3.5 3.5'));
        eye.addEventListener('click', (e) => {
          e.stopPropagation();
          layer.visible = !layer.visible;
          syncLayerDOM();
          renderLayerPanel();
        });
        const thumb = document.createElement('canvas');
        thumb.className = 'imged-thumb';
        thumb.width = 44; thumb.height = 30;
        layer.thumb = thumb;
        drawThumb(layer);
        const nameBox = el('div', 'imged-layer-name');
        nameBox.append(el('div', null, layer.name), el('div', 'imged-layer-dim', `${docW}×${docH} · ${Math.round(layer.opacity * 100)}%`));
        item.append(eye, thumb, nameBox);
        item.addEventListener('click', () => {
          activeIdx = i;
          renderLayerPanel();
        });
        listEl.appendChild(item);
      }
      // 选中层不透明度
      layerOpacitySlider.setAttribute('value', String(Math.round(activeLayer().opacity * 100)));
      layerOpacityLabel.textContent = t('图层不透明度 {n}%', { n: Math.round(activeLayer().opacity * 100) });
      // 按钮禁用态
      mergeBtn.classList.toggle('disabled', activeIdx === 0);
      delBtn.classList.toggle('disabled', layers.length <= 1);
    }

    function drawThumb(layer) {
      const t = layer.thumb;
      if (!t) return;
      const ctx = t.getContext('2d');
      ctx.clearRect(0, 0, t.width, t.height);
      const s = Math.min(t.width / docW, t.height / docH);
      ctx.imageSmoothingEnabled = true;
      ctx.drawImage(layer.canvas, (t.width - docW * s) / 2, (t.height - docH * s) / 2, docW * s, docH * s);
    }

    function refreshThumbs() {
      for (const layer of layers) drawThumb(layer);
    }

    /* ---------- 调色盘 ---------- */
    const WHEEL = 148, SL_W = 148, SL_H = 92;

    const wheelCanvas = document.createElement('canvas');
    wheelCanvas.className = 'imged-wheel';
    wheelCanvas.width = WHEEL; wheelCanvas.height = WHEEL;
    const svCanvas = document.createElement('canvas');
    svCanvas.className = 'imged-sv';
    svCanvas.width = SL_W; svCanvas.height = SL_H;

    const swatchPreview = el('div', 'imged-swatch-preview');
    const swatchChecker = el('div', 'imged-swatch-checker');
    swatchPreview.appendChild(swatchChecker);
    const swatchFill = el('div', 'imged-swatch-fill');
    swatchPreview.appendChild(swatchFill);

    const transparentBtn = el('button', 'imged-transparent-swatch');
    transparentBtn.type = 'button';
    transparentBtn.title = t('透明色（橡皮）：涂抹处 alpha 清零，用于抠图');
    transparentBtn.append(el('span', null, t('透明')));

    const screenPickBtn = el('button', 'imged-mini-btn');
    screenPickBtn.type = 'button';
    screenPickBtn.textContent = t('屏幕取色');
    screenPickBtn.title = window.EyeDropper ? t('从屏幕任意位置取色') : t('需要 Chromium/Edge 的 EyeDropper API');

    const rgbInputs = ['r', 'g', 'b', 'a'].map((k) => {
      const box = el('label', 'imged-num');
      box.append(el('span', null, k.toUpperCase()));
      const input = document.createElement('input');
      input.type = 'number'; input.min = 0; input.max = 255; input.step = 1;
      input.addEventListener('change', () => {
        const v = clamp(Math.round(Number(input.value) || 0), 0, 255);
        input.value = String(v);
        const rgb = rgbFromInputs();
        if (k === 'a') color.a = v / 255;
        else {
          const hsl = rgbToHsl(rgb.r, rgb.g, rgb.b);
          color.h = hsl.h; color.s = hsl.s; color.l = hsl.l;
        }
        color.erase = false;
        syncPalette();
      });
      box.appendChild(input);
      return { key: k, box, input };
    });

    function rgbFromInputs() {
      const get = (k) => Number(rgbInputs.find((x) => x.key === k).input.value) || 0;
      return { r: get('r'), g: get('g'), b: get('b') };
    }

    const hexInput = document.createElement('input');
    hexInput.type = 'text';
    hexInput.className = 'imged-hex';
    hexInput.placeholder = '#RRGGBB / #RRGGBBAA';
    hexInput.spellcheck = false;
    hexInput.addEventListener('change', () => {
      const m = /^#?([0-9a-f]{6})([0-9a-f]{2})?$/i.exec(hexInput.value.trim());
      if (!m) { pushToast(t('色号格式：#RRGGBB 或 #RRGGBBAA')); return; }
      const r = parseInt(m[1].slice(0, 2), 16);
      const g = parseInt(m[1].slice(2, 4), 16);
      const b = parseInt(m[1].slice(4, 6), 16);
      const hsl = rgbToHsl(r, g, b);
      color.h = hsl.h; color.s = hsl.s; color.l = hsl.l;
      if (m[2]) color.a = parseInt(m[2], 16) / 255;
      color.erase = false;
      syncPalette();
    });

    const recentRow = el('div', 'imged-recent');

    /* ---------- 工具列 ---------- */
    const toolButtons = new Map();
    const radiusSlider = document.createElement('ui-slider');
    radiusSlider.setAttribute('min', '1');
    radiusSlider.setAttribute('max', '64');
    radiusSlider.setAttribute('step', '1');
    const radiusLabel = el('div', 'imged-slider-label');
    const alphaSlider = document.createElement('ui-slider');
    alphaSlider.setAttribute('min', '2');
    alphaSlider.setAttribute('max', '100');
    alphaSlider.setAttribute('step', '1');
    const alphaLabel = el('div', 'imged-slider-label');

    function syncToolSliders() {
      const st = toolState.get(tool);
      if (st) {
        radiusSlider.style.display = '';
        radiusLabel.style.display = '';
        radiusSlider.setAttribute('value', String(st.radius));
        radiusLabel.textContent = t('笔刷半径 {n}px', { n: st.radius });
        alphaSlider.style.display = '';
        alphaLabel.style.display = '';
        alphaSlider.setAttribute('value', String(Math.round(st.alpha * 100)));
        alphaLabel.textContent = t('不透明度 {n}%', { n: Math.round(st.alpha * 100) });
        alphaLabel.style.display = tool === 'binary' || tool === 'pencil' ? 'none' : '';
        alphaSlider.style.display = tool === 'binary' || tool === 'pencil' ? 'none' : '';
      } else {
        radiusSlider.style.display = radiusLabel.style.display = 'none';
        alphaSlider.style.display = alphaLabel.style.display = 'none';
      }
    }

    /* ---------- DOM 装配 ---------- */
    // 工具列
    const toolGrid = el('div', 'imged-tool-grid');
    for (const tl of TOOLS) {
      const b = el('button', 'imged-tool' + (tl.id === tool ? ' active' : ''));
      b.type = 'button';
      b.title = t(tl.label);
      b.style.setProperty('--i', String(TOOLS.indexOf(tl)));
      b.append(svg(tl.icon));
      b.appendChild(el('span', 'imged-tool-tip', t(tl.label)));
      b.addEventListener('click', () => {
        tool = tl.id;
        shapeStart = null;
        previewCtx?.clearRect(0, 0, docW, docH);
        toolGrid.querySelectorAll('.imged-tool').forEach((x) => x.classList.remove('active'));
        b.classList.add('active');
        shapeModeBox.classList.toggle('show', tool === 'rect' || tool === 'ellipse');
        syncToolSliders();
        updateRingSize();
      });
      toolButtons.set(tl.id, b);
      toolGrid.appendChild(b);
    }
    const shapeModeBox = el('div', 'imged-shape-mode');
    const hollowBtn = el('button', 'imged-mini-btn');
    hollowBtn.type = 'button'; hollowBtn.textContent = t('空心');
    const solidBtn = el('button', 'imged-mini-btn sel');
    solidBtn.type = 'button'; solidBtn.textContent = t('实心');
    let shapeSolid = true;
    hollowBtn.addEventListener('click', () => { shapeSolid = false; hollowBtn.classList.add('sel'); solidBtn.classList.remove('sel'); });
    solidBtn.addEventListener('click', () => { shapeSolid = true; solidBtn.classList.add('sel'); hollowBtn.classList.remove('sel'); });
    shapeModeBox.append(hollowBtn, solidBtn);

    toolsSection.append(
      el('div', 'imged-panel-title', t('工具')),
      toolGrid,
      shapeModeBox,
      radiusLabel, radiusSlider,
      alphaLabel, alphaSlider,
    );

    // 图层面板
    const listEl = el('div', 'imged-layer-list');
    const addBtn = el('button', 'imged-mini-btn');
    addBtn.type = 'button'; addBtn.textContent = t('新建图层');
    const importBtn = el('button', 'imged-mini-btn');
    importBtn.type = 'button'; importBtn.textContent = t('导入贴图');
    const mergeBtn = el('button', 'imged-mini-btn');
    mergeBtn.type = 'button'; mergeBtn.textContent = t('向下合并');
    const delBtn = el('button', 'imged-mini-btn');
    delBtn.type = 'button'; delBtn.textContent = t('删除图层');
    const layerOpacitySlider = document.createElement('ui-slider');
    layerOpacitySlider.setAttribute('min', '0');
    layerOpacitySlider.setAttribute('max', '100');
    layerOpacitySlider.setAttribute('step', '1');
    const layerOpacityLabel = el('div', 'imged-slider-label');
    const layerBtnRow = el('div', 'imged-btn-row');
    layerBtnRow.append(addBtn, importBtn, mergeBtn, delBtn);
    layersSection.append(
      el('div', 'imged-panel-title', t('图层')),
      listEl,
      layerOpacitySlider, layerOpacityLabel,
      layerBtnRow,
    );

    // 调色盘
    const colorTop = el('div', 'imged-color-top');
    colorTop.append(wheelCanvas, svCanvas);
    const previewRow = el('div', 'imged-preview-row');
    previewRow.append(swatchPreview, transparentBtn, screenPickBtn);
    const rgbRow = el('div', 'imged-rgb-row');
    for (const { box } of rgbInputs) rgbRow.appendChild(box);
    paletteSection.append(
      el('div', 'imged-panel-title', t('颜色')),
      colorTop,
      previewRow,
      rgbRow, hexInput,
      el('div', 'imged-slider-label', t('最近使用')),
      recentRow,
    );

    // footer
    const undoBtn = el('button', 'imged-mini-btn');
    undoBtn.type = 'button'; undoBtn.textContent = t('撤销 (Ctrl+Z)');
    const fitBtn = el('button', 'imged-mini-btn');
    fitBtn.type = 'button'; fitBtn.textContent = t('适配视图');
    const zoomLabel = el('span', 'imged-zoom-label', '100%');
    const cancelBtn = el('button', 'imged-cancel');
    cancelBtn.type = 'button'; cancelBtn.textContent = t('取消');
    const applyBtn = el('button', 'imged-apply');
    applyBtn.type = 'button'; applyBtn.textContent = t('应用为资源内容');
    footerLeft.append(undoBtn, fitBtn, zoomLabel);
    footerRight.append(cancelBtn, applyBtn);

    /* ---------- 撤销 ---------- */
    function snapshotForUndo() {
      const layer = activeLayer();
      undoStack.push({ idx: activeIdx, data: layer.ctx.getImageData(0, 0, docW, docH) });
      if (undoStack.length > 25) undoStack.shift();
    }

    function undo() {
      const s = undoStack.pop();
      if (!s) return pushToast(t('没有可撤销的操作'));
      layers[s.idx].ctx.putImageData(s.data, 0, 0);
      refreshThumbs();
    }

    /* ---------- 合成管线 ----------
     * strokeLayer：本笔划遮罩（最终颜色 / 擦除时为白），alpha 1；
     * - 不透明工具（binary/pencil 阈值、α≥1 画笔）：逐段增量合成（幂等，实时可见）；
     * - 半透明工具（荧光笔、低 α 画笔、透明色）：ghost 层实时预览（CSS opacity），
     *   pointerup 一次性合成 —— 避免逐段合成导致重叠段反复加深（Photoshop 同款语义）。 */
    function effAlpha() {
      const st = toolState.get(tool) ?? { alpha: 1 };
      return color.erase ? color.a : clamp(color.a * st.alpha, 0, 1);
    }

    function isThresholdTool() {
      return tool === 'binary' || tool === 'pencil';
    }

    function strokeColor() {
      return color.erase ? '#ffffff' : rgbCss(hslToRgb(color.h, color.s, color.l), 1);
    }

    function compositeStroke(dirty) {
      const layer = activeLayer();
      const ctx = layer.ctx;
      if (color.erase) {
        ctx.save();
        ctx.globalCompositeOperation = 'destination-out';
        ctx.globalAlpha = color.a;
        ctx.drawImage(strokeLayer, 0, 0);
        ctx.restore();
      } else if (isThresholdTool()) {
        // 阈值管线：mask alpha ≥ 128 → 当前色全不透明（无抗锯齿硬边）
        const x0 = Math.max(0, dirty.x), y0 = Math.max(0, dirty.y);
        const x1 = Math.min(docW, dirty.x + dirty.w), y1 = Math.min(docH, dirty.y + dirty.h);
        if (x1 <= x0 || y1 <= y0) return;
        const mask = strokeCtx.getImageData(x0, y0, x1 - x0, y1 - y0);
        const out = new ImageData(x1 - x0, y1 - y0);
        const rgb = hslToRgb(color.h, color.s, color.l);
        for (let i = 0; i < mask.data.length; i += 4) {
          if (mask.data[i + 3] >= 128) {
            out.data[i] = rgb.r; out.data[i + 1] = rgb.g; out.data[i + 2] = rgb.b;
            out.data[i + 3] = 255;
          }
        }
        const tmp = document.createElement('canvas');
        tmp.width = x1 - x0; tmp.height = y1 - y0;
        tmp.getContext('2d').putImageData(out, 0, 0);
        ctx.drawImage(tmp, x0, y0);
      } else {
        ctx.save();
        ctx.globalAlpha = effAlpha();
        ctx.drawImage(strokeLayer, 0, 0);
        ctx.restore();
      }
      ctx.globalAlpha = 1;
      ctx.globalCompositeOperation = 'source-over';
    }

    function strokeSeg(a, b, radius) {
      const col = strokeColor();
      for (const c of [strokeCtx, ghostCtx]) {
        c.strokeStyle = col;
        c.fillStyle = col;
        c.lineWidth = Math.max(1, radius * 2);
        c.lineCap = 'round';
        c.lineJoin = 'round';
        if (a.x === b.x && a.y === b.y) {
          // 零长度线段不渲染圆帽点：显式画圆
          c.beginPath();
          c.arc(a.x, a.y, Math.max(0.5, radius), 0, Math.PI * 2);
          c.fill();
        } else {
          c.beginPath();
          c.moveTo(a.x, a.y);
          c.lineTo(b.x, b.y);
          c.stroke();
        }
      }
    }

    /* ---------- 指针交互 ---------- */
    function toDoc(e) {
      const r = stage.getBoundingClientRect();
      return { x: (e.clientX - r.left) / zoom, y: (e.clientY - r.top) / zoom };
    }

    function constrainShift(a, b) {
      if (!shiftKey) return b;
      if (tool === 'line') {
        const dx = b.x - a.x, dy = b.y - a.y;
        const ang = Math.round(Math.atan2(dy, dx) / (Math.PI / 4)) * (Math.PI / 4);
        const len = Math.hypot(dx, dy);
        return { x: a.x + Math.cos(ang) * len, y: a.y + Math.sin(ang) * len };
      }
      const s = Math.max(Math.abs(b.x - a.x), Math.abs(b.y - a.y));
      return {
        x: a.x + Math.sign(b.x - a.x || 1) * s,
        y: a.y + Math.sign(b.y - a.y || 1) * s,
      };
    }

    function drawShapePreview(a, b) {
      previewCtx.clearRect(0, 0, docW, docH);
      const rgb = hslToRgb(color.h, color.s, color.l);
      const st = toolState.get(tool);
      previewCtx.fillStyle = color.erase ? 'rgba(255,255,255,.55)' : rgbCss(rgb, color.a);
      previewCtx.strokeStyle = color.erase ? 'rgba(255,255,255,.9)' : rgbCss(rgb, color.a);
      previewCtx.lineWidth = Math.max(1, st.radius * 2);
      previewCtx.lineCap = 'round';
      previewCtx.lineJoin = 'round';
      previewCtx.beginPath();
      if (tool === 'line') {
        previewCtx.moveTo(a.x, a.y);
        previewCtx.lineTo(b.x, b.y);
        previewCtx.stroke();
        return;
      }
      if (tool === 'rect') {
        previewCtx.rect(Math.min(a.x, b.x), Math.min(a.y, b.y), Math.abs(b.x - a.x), Math.abs(b.y - a.y));
      } else if (tool === 'ellipse') {
        previewCtx.ellipse((a.x + b.x) / 2, (a.y + b.y) / 2, Math.abs(b.x - a.x) / 2, Math.abs(b.y - a.y) / 2, 0, 0, Math.PI * 2);
      }
      if (shapeSolid) previewCtx.fill(); else previewCtx.stroke();
    }

    function commitShape(a, b) {
      const st = toolState.get(tool);
      const radius = st.radius;
      const col = strokeColor();
      strokeCtx.fillStyle = col;
      strokeCtx.strokeStyle = col;
      strokeCtx.lineWidth = Math.max(1, radius * 2);
      strokeCtx.lineCap = 'round';
      strokeCtx.lineJoin = 'round';
      strokeCtx.beginPath();
      if (tool === 'line') {
        strokeCtx.moveTo(a.x, a.y);
        strokeCtx.lineTo(b.x, b.y);
        strokeCtx.stroke();
      } else if (tool === 'rect') {
        strokeCtx.rect(Math.min(a.x, b.x), Math.min(a.y, b.y), Math.abs(b.x - a.x), Math.abs(b.y - a.y));
        if (shapeSolid) strokeCtx.fill(); else strokeCtx.stroke();
      } else {
        strokeCtx.ellipse((a.x + b.x) / 2, (a.y + b.y) / 2, Math.abs(b.x - a.x) / 2, Math.abs(b.y - a.y) / 2, 0, 0, Math.PI * 2);
        if (shapeSolid) strokeCtx.fill(); else strokeCtx.stroke();
      }
      compositeStroke({
        x: Math.min(a.x, b.x) - radius, y: Math.min(a.y, b.y) - radius,
        w: Math.abs(b.x - a.x) + radius * 4, h: Math.abs(b.y - a.y) + radius * 4,
      });
      afterStroke();
    }

    function clearStrokeBuffers() {
      strokeCtx.clearRect(0, 0, docW, docH);
      ghostCtx.clearRect(0, 0, docW, docH);
      ghostCanvas.style.opacity = '';
    }

    function afterStroke() {
      clearStrokeBuffers();
      refreshThumbs();
      if (!color.erase) {
        const key = `${Math.round(color.h)},${color.s.toFixed(2)},${color.l.toFixed(2)},${Math.round(color.a * 100)}`;
        const i = recent.indexOf(key);
        if (i >= 0) recent.splice(i, 1);
        recent.unshift(key);
        if (recent.length > 8) recent.pop();
        renderRecent();
      }
    }

    let ghostMode = false;   // 本笔划走 ghost 预览 + 收尾一次合成

    viewport.addEventListener('pointerdown', (e) => {
      if (!docW || e.target.closest('.imged-zoombar')) return;
      if (e.button === 1 || spacePan) {       // 中键 / 空格 平移
        panDrag = true; lastPan = { x: e.clientX, y: e.clientY };
        try { viewport.setPointerCapture(e.pointerId); } catch { /* 合成事件/无活动指针 */ }
        e.preventDefault();
        return;
      }
      if (e.button !== 0) return;
      const p = toDoc(e);
      if (tool === 'picker') {
        pickColorAt(p);
        return;
      }
      const st = toolState.get(tool);
      drawing = true;
      snapshotForUndo();
      try { viewport.setPointerCapture(e.pointerId); } catch { /* 合成事件/无活动指针 */ }
      if (tool === 'line' || tool === 'rect' || tool === 'ellipse') {
        shapeStart = p;
        return;
      }
      ghostMode = !isThresholdTool() && effAlpha() < 0.999;
      if (ghostMode) {
        ghostCtx.clearRect(0, 0, docW, docH);
        ghostCanvas.style.opacity = String(effAlpha());
      }
      strokeSeg(p, p, st.radius);
      if (!ghostMode) compositeStroke({ x: p.x - st.radius, y: p.y - st.radius, w: st.radius * 2, h: st.radius * 2 });
      lastPt = p;
    });

    let lastPt = null;

    viewport.addEventListener('pointermove', (e) => {
      moveRing(e);
      if (panDrag) {
        panX += e.clientX - lastPan.x;
        panY += e.clientY - lastPan.y;
        lastPan = { x: e.clientX, y: e.clientY };
        applyTransform();
        return;
      }
      if (!drawing) return;
      const p = toDoc(e);
      if (shapeStart) {
        drawShapePreview(shapeStart, constrainShift(shapeStart, p));
        return;
      }
      const st = toolState.get(tool);
      strokeSeg(lastPt ?? p, p, st.radius);
      if (ghostMode) return;                       // 半透明：收尾一次合成
      const minX = Math.min(lastPt?.x ?? p.x, p.x) - st.radius - 2;
      const minY = Math.min(lastPt?.y ?? p.y, p.y) - st.radius - 2;
      const w = Math.abs((lastPt?.x ?? p.x) - p.x) + st.radius * 2 + 4;
      const h = Math.abs((lastPt?.y ?? p.y) - p.y) + st.radius * 2 + 4;
      compositeStroke({ x: minX, y: minY, w, h });
      lastPt = p;
    });

    viewport.addEventListener('pointerup', (e) => {
      if (panDrag) { panDrag = false; return; }
      if (!drawing) return;
      drawing = false;
      if (shapeStart) {
        const b = constrainShift(shapeStart, toDoc(e));
        previewCtx.clearRect(0, 0, docW, docH);
        commitShape(shapeStart, b);
        shapeStart = null;
        return;
      }
      if (ghostMode) compositeStroke({ x: 0, y: 0, w: docW, h: docH });
      ghostMode = false;
      afterStroke();
      lastPt = null;
    });

    /* ---------- 吸管 ---------- */
    function pickColorAt(p) {
      const m = document.createElement('canvas');
      m.width = docW; m.height = docH;
      const mctx = m.getContext('2d');
      for (const layer of layers) {
        if (!layer.visible) continue;
        mctx.globalAlpha = layer.opacity;
        mctx.drawImage(layer.canvas, 0, 0);
      }
      const d = mctx.getImageData(clamp(Math.floor(p.x), 0, docW - 1), clamp(Math.floor(p.y), 0, docH - 1), 1, 1).data;
      if (d[3] === 0) {
        color.erase = true;
        pushToast(t('取到透明像素 — 已切换到透明色'));
      } else {
        const hsl = rgbToHsl(d[0], d[1], d[2]);
        color.h = hsl.h; color.s = hsl.s; color.l = hsl.l; color.a = d[3] / 255;
        color.erase = false;
      }
      syncPalette();
    }

    /* ---------- 缩放 / 平移 ---------- */
    const zoombarLabel = el('span', 'imged-zoom-label', '100%');

    function applyTransform() {
      stage.style.transform = `translate(${panX}px, ${panY}px) scale(${zoom})`;
      const text = `${Math.round(zoom * 100)}%`;
      zoomLabel.textContent = text;
      zoombarLabel.textContent = text;
      stage.classList.toggle('pixelated', zoom >= 2);
      updateRingSize();
    }

    function fitView() {
      const r = viewport.getBoundingClientRect();
      zoom = clamp(Math.min(r.width / docW, r.height / docH) * 0.85, 0.05, 24);
      panX = (r.width - docW * zoom) / 2;
      panY = (r.height - docH * zoom) / 2;
      applyTransform();
    }

    viewport.addEventListener('wheel', (e) => {
      if (!docW) return;
      e.preventDefault();
      const r = viewport.getBoundingClientRect();
      const mx = e.clientX - r.left, my = e.clientY - r.top;
      const k = Math.exp(-e.deltaY * 0.0015);
      const nz = clamp(zoom * k, 0.05, 24);
      panX = mx - (mx - panX) * (nz / zoom);
      panY = my - (my - panY) * (nz / zoom);
      zoom = nz;
      applyTransform();
    }, { passive: false });

    zoombar.append(zoomOutBtn(), zoombarLabel, zoomInBtn(), fitBtn2(), zoom100Btn());

    function zoomOutBtn() { const b = el('button', 'imged-zoom-btn', '−'); b.type = 'button'; b.addEventListener('click', () => setZoom(zoom / 1.25)); return b; }
    function zoomInBtn() { const b = el('button', 'imged-zoom-btn', '+'); b.type = 'button'; b.addEventListener('click', () => setZoom(zoom * 1.25)); return b; }
    function fitBtn2() { const b = el('button', 'imged-zoom-btn', t('适配')); b.type = 'button'; b.addEventListener('click', fitView); return b; }
    function zoom100Btn() { const b = el('button', 'imged-zoom-btn', '1:1'); b.type = 'button'; b.addEventListener('click', () => setZoom(1)); return b; }

    function setZoom(nz) {
      nz = clamp(nz, 0.05, 24);
      const r = viewport.getBoundingClientRect();
      const cx = r.width / 2, cy = r.height / 2;
      panX = cx - (cx - panX) * (nz / zoom);
      panY = cy - (cy - panY) * (nz / zoom);
      zoom = nz;
      applyTransform();
    }

    // fitBtn（footer 里那个）复用 fitView
    fitBtn.addEventListener('click', fitView);

    /* ---------- 笔刷光标环（rAF 平滑跟随） ---------- */
    let ringTarget = { x: 0, y: 0 }, ringPos = { x: 0, y: 0 }, ringVisible = false;
    function moveRing(e) {
      const r = viewport.getBoundingClientRect();
      ringTarget.x = e.clientX - r.left;
      ringTarget.y = e.clientY - r.top;
      if (!ringVisible) { ringPos = { ...ringTarget }; ringVisible = true; brushRing.classList.add('on'); }
    }
    function updateRingSize() {
      const st = toolState.get(tool);
      const d = st ? Math.max(6, st.radius * 2 * zoom) : 8;
      brushRing.style.width = `${d}px`;
      brushRing.style.height = `${d}px`;
    }
    viewport.addEventListener('pointerleave', () => { ringVisible = false; brushRing.classList.remove('on'); });
    (function ringLoop() {
      if (!overlay.isConnected) return;
      ringPos.x += (ringTarget.x - ringPos.x) * 0.4;
      ringPos.y += (ringTarget.y - ringPos.y) * 0.4;
      brushRing.style.transform = `translate(${ringPos.x}px, ${ringPos.y}px) translate(-50%, -50%)`;
      requestAnimationFrame(ringLoop);
    })();

    /* ---------- 键盘 ---------- */
    const onKeyDown = (e) => {
      if (e.key === 'Escape') { requestClose(); return; }
      if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'z') { e.preventDefault(); undo(); return; }
      if (e.key === ' ' && !spacePan) { spacePan = true; e.preventDefault(); return; }
      if (e.key === 'Shift') shiftKey = true;
    };
    const onKeyUp = (e) => {
      if (e.key === ' ') spacePan = false;
      if (e.key === 'Shift') shiftKey = false;
    };
    document.addEventListener('keydown', onKeyDown, true);
    document.addEventListener('keyup', onKeyUp, true);

    /* ---------- 粘贴 / 拖放导入 ---------- */
    const onPaste = (e) => {
      if (e.target.closest('input, textarea, [contenteditable]')) return;
      const item = [...(e.clipboardData?.items ?? [])].find((x) => x.type.startsWith('image/'));
      if (item) {
        e.preventDefault();
        addLayerFromBlob(item.getAsFile(), t('贴图 {n}', { n: ++layerSeq }));
      }
    };
    document.addEventListener('paste', onPaste);
    viewport.addEventListener('dragover', (e) => { e.preventDefault(); viewport.classList.add('dragging'); });
    viewport.addEventListener('dragleave', () => viewport.classList.remove('dragging'));
    viewport.addEventListener('drop', (e) => {
      e.preventDefault();
      viewport.classList.remove('dragging');
      const file = e.dataTransfer?.files?.[0];
      if (file && file.type.startsWith('image/')) addLayerFromBlob(file, t('贴图 {n}', { n: ++layerSeq }));
    });

    /* ---------- 屏幕取色 ---------- */
    screenPickBtn.addEventListener('click', async () => {
      if (!window.EyeDropper) return pushToast(t('当前浏览器不支持 EyeDropper，请用画布内吸管工具'));
      try {
        const res = await new EyeDropper().open();
        const r = parseInt(res.sRGBHex.slice(1, 3), 16);
        const g = parseInt(res.sRGBHex.slice(3, 5), 16);
        const b = parseInt(res.sRGBHex.slice(5, 7), 16);
        const hsl = rgbToHsl(r, g, b);
        color.h = hsl.h; color.s = hsl.s; color.l = hsl.l;
        color.erase = false;
        syncPalette();
        pushToast(t('取色 {color}', { color: res.sRGBHex.toUpperCase() }));
      } catch { /* 用户取消 */ }
    });

    /* ---------- 调色盘渲染 ---------- */
    function drawWheel() {
      const ctx = wheelCanvas.getContext('2d');
      const c = WHEEL / 2, rOut = c - 4, rIn = c - 22;
      ctx.clearRect(0, 0, WHEEL, WHEEL);
      for (let i = 0; i < 360; i += 1) {
        const a0 = (i - 0.5) * Math.PI / 180, a1 = (i + 1.5) * Math.PI / 180;
        ctx.beginPath();
        ctx.arc(c, c, rOut, a0, a1);
        ctx.arc(c, c, rIn, a1, a0, true);
        ctx.closePath();
        ctx.fillStyle = rgbCss(hslToRgb(i, 1, 0.5));
        ctx.fill();
      }
      // 当前 hue 指针
      const ang = (color.h - 90) * Math.PI / 180;
      const px = c + ((rIn + rOut) / 2) * Math.cos(ang);
      const py = c + ((rIn + rOut) / 2) * Math.sin(ang);
      ctx.beginPath();
      ctx.arc(px, py, 7, 0, Math.PI * 2);
      ctx.strokeStyle = '#fff';
      ctx.lineWidth = 2.5;
      ctx.stroke();
      ctx.strokeStyle = 'rgba(0,0,0,.55)';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.arc(px, py, 8.5, 0, Math.PI * 2);
      ctx.stroke();
    }

    function drawSV() {
      const ctx = svCanvas.getContext('2d');
      const img = ctx.createImageData(SL_W, SL_H);
      for (let y = 0; y < SL_H; y++) {
        for (let x = 0; x < SL_W; x++) {
          const s = x / (SL_W - 1), l = 1 - y / (SL_H - 1);
          const rgb = hslToRgb(color.h, s, l);
          const i = (y * SL_W + x) * 4;
          img.data[i] = rgb.r; img.data[i + 1] = rgb.g; img.data[i + 2] = rgb.b; img.data[i + 3] = 255;
        }
      }
      ctx.putImageData(img, 0, 0);
      // S/L 指针
      const px = color.s * (SL_W - 1);
      const py = (1 - color.l) * (SL_H - 1);
      ctx.beginPath();
      ctx.arc(px, py, 6, 0, Math.PI * 2);
      ctx.strokeStyle = '#fff';
      ctx.lineWidth = 2;
      ctx.stroke();
      ctx.strokeStyle = 'rgba(0,0,0,.55)';
      ctx.lineWidth = 1;
      ctx.beginPath();
      ctx.arc(px, py, 7.5, 0, Math.PI * 2);
      ctx.stroke();
    }

    wheelCanvas.addEventListener('pointerdown', (e) => {
      try { wheelCanvas.setPointerCapture(e.pointerId); } catch { /* 合成事件 */ }
      pickHue(e);
      const move = (ev) => pickHue(ev);
      const up = () => {
        wheelCanvas.removeEventListener('pointermove', move);
        wheelCanvas.removeEventListener('pointerup', up);
      };
      wheelCanvas.addEventListener('pointermove', move);
      wheelCanvas.addEventListener('pointerup', up);
    });
    function pickHue(e) {
      const r = wheelCanvas.getBoundingClientRect();
      const dx = e.clientX - r.left - WHEEL / 2, dy = e.clientY - r.top - WHEEL / 2;
      color.h = ((Math.atan2(dy, dx) * 180 / Math.PI + 90) % 360 + 360) % 360;
      color.erase = false;
      syncPalette();
    }

    svCanvas.addEventListener('pointerdown', (e) => {
      try { svCanvas.setPointerCapture(e.pointerId); } catch { /* 合成事件 */ }
      const move = (ev) => pickSL(ev);
      const up = () => {
        svCanvas.removeEventListener('pointermove', move);
        svCanvas.removeEventListener('pointerup', up);
      };
      svCanvas.addEventListener('pointermove', move);
      svCanvas.addEventListener('pointerup', up);
      pickSL(e);
    });
    function pickSL(e) {
      const r = svCanvas.getBoundingClientRect();
      color.s = clamp((e.clientX - r.left) / (SL_W - 1), 0, 1);
      color.l = clamp(1 - (e.clientY - r.top) / (SL_H - 1), 0, 1);
      color.erase = false;
      syncPalette();
    }

    transparentBtn.addEventListener('click', () => {
      color.erase = !color.erase;
      syncPalette();
      pushToast(color.erase ? t('已选透明色：涂抹将清除 alpha（抠图）') : t('已回到普通颜色'));
    });

    function syncPalette() {
      const rgb = hslToRgb(color.h, color.s, color.l);
      drawWheel();
      drawSV();
      swatchFill.style.background = color.erase
        ? 'transparent'
        : rgbCss(rgb, color.a);
      swatchPreview.classList.toggle('erase', color.erase);
      for (const { key, input } of rgbInputs) {
        input.value = String(key === 'a' ? Math.round(color.a * 255) : rgb[key]);
      }
      const hex = `#${[rgb.r, rgb.g, rgb.b].map((v) => v.toString(16).padStart(2, '0')).join('')}`
        + (color.a < 1 ? `#${Math.round(color.a * 255).toString(16).padStart(2, '0')}` : '');
      if (document.activeElement !== hexInput) hexInput.value = hex.toUpperCase();
      transparentBtn.classList.toggle('sel', color.erase);
      syncToolSliders();
      updateRingColor();
    }

    function renderRecent() {
      recentRow.replaceChildren();
      for (const key of recent) {
        const [h, s, l, a] = key.split(',').map(Number);
        const sw = el('button', 'imged-recent-swatch');
        sw.type = 'button';
        sw.style.background = rgbCss(hslToRgb(h, s, l), a / 100);
        sw.title = `H${h} S${Math.round(s * 100)} L${Math.round(l * 100)} A${a}%`;
        sw.style.setProperty('--i', String(recent.indexOf(key)));
        sw.addEventListener('click', () => {
          color.h = h; color.s = s; color.l = l; color.a = a / 100;
          color.erase = false;
          syncPalette();
        });
        recentRow.appendChild(sw);
      }
    }

    function updateRingColor() {
      brushRing.style.borderColor = color.erase ? 'rgba(255,255,255,.9)' : rgbCss(hslToRgb(color.h, color.s, color.l), Math.max(0.4, color.a));
    }

    /* ---------- slider 事件 ---------- */
    radiusSlider.addEventListener('ui-change', (e) => {
      const st = toolState.get(tool);
      if (!st) return;
      st.radius = clamp(Math.round(e.detail?.value ?? st.radius), 1, 64);
      radiusLabel.textContent = t('笔刷半径 {n}px', { n: st.radius });
      updateRingSize();
    });
    alphaSlider.addEventListener('ui-change', (e) => {
      const st = toolState.get(tool);
      if (!st) return;
      st.alpha = clamp((e.detail?.value ?? st.alpha * 100) / 100, 0.02, 1);
      alphaLabel.textContent = t('不透明度 {n}%', { n: Math.round(st.alpha * 100) });
    });
    layerOpacitySlider.addEventListener('ui-change', (e) => {
      const layer = activeLayer();
      layer.opacity = clamp((e.detail?.value ?? 0) / 100, 0, 1);
      layerOpacityLabel.textContent = t('图层不透明度 {n}%', { n: Math.round(layer.opacity * 100) });
      syncLayerDOM();
      renderLayerPanel();
    });

    /* ---------- 图层按钮 ---------- */
    addBtn.addEventListener('click', addEmptyLayer);
    delBtn.addEventListener('click', deleteActiveLayer);
    mergeBtn.addEventListener('click', mergeDown);
    const upBtn = el('button', 'imged-mini-btn', t('上移')); upBtn.type = 'button';
    upBtn.addEventListener('click', () => moveLayer(1));
    const downBtn = el('button', 'imged-mini-btn', t('下移')); downBtn.type = 'button';
    downBtn.addEventListener('click', () => moveLayer(-1));
    layerBtnRow.append(upBtn, downBtn);

    const fileInput = document.createElement('input');
    fileInput.type = 'file';
    fileInput.accept = 'image/*';
    fileInput.style.display = 'none';
    importBtn.addEventListener('click', () => fileInput.click());
    fileInput.addEventListener('change', () => {
      const file = fileInput.files?.[0];
      fileInput.value = '';
      if (file) addLayerFromBlob(file, t('贴图 {n}', { n: ++layerSeq }));
    });
    overlay.appendChild(fileInput);

    /* ---------- toast ---------- */
    let toastTimer = 0;
    function pushToast(msg) {
      let toastEl = overlay.querySelector('.imged-toast');
      if (!toastEl) {
        toastEl = el('div', 'imged-toast');
        overlay.appendChild(toastEl);
      }
      toastEl.textContent = msg;
      toastEl.classList.remove('show');
      void toastEl.offsetWidth;      // 重启动画
      toastEl.classList.add('show');
      window.clearTimeout(toastTimer);
      toastTimer = window.setTimeout(() => toastEl.classList.remove('show'), 1800);
    }

    /* ---------- 关闭 / 应用 ---------- */
    function requestClose() {
      if (undoStack.length && !applied && !confirm(t('有未应用的修改，确定关闭编辑器？'))) return;
      closeOverlay(null);
    }

    function closeOverlay(result) {
      document.removeEventListener('keydown', onKeyDown, true);
      document.removeEventListener('keyup', onKeyUp, true);
      document.removeEventListener('paste', onPaste);
      overlay.classList.add('closing');       // PopOut：500ms OutQuint
      window.setTimeout(() => {
        overlay.remove();
        resolve(result);
      }, 520);
    }

    closeBtn.addEventListener('click', () => requestClose());
    backdrop.addEventListener('click', () => requestClose());
    cancelBtn.addEventListener('click', () => requestClose());
    undoBtn.addEventListener('click', undo);

    applyBtn.addEventListener('click', () => {
      const merged = document.createElement('canvas');
      merged.width = docW; merged.height = docH;
      const mctx = merged.getContext('2d');
      for (const layer of layers) {
        if (!layer.visible) continue;
        mctx.globalAlpha = layer.opacity;
        mctx.drawImage(layer.canvas, 0, 0);
      }
      mctx.globalAlpha = 1;
      let bytes2;
      const note = t('图层编辑（{n} 层）', { n: layers.length });
      if (outFormat === 'png') {
        merged.toBlob((b) => {
          b.arrayBuffer().then((buf) => {
            applied = true;
            onApply(new Uint8Array(buf), t('{note} → PNG', { note }));
            closeOverlay(new Uint8Array(buf));
          });
        }, 'image/png');
        return;
      }
      const data = mctx.getImageData(0, 0, docW, docH);
      const dib = encodeDib(data, outFormat !== 'dib-bmp');
      bytes2 = outFormat === 'dib-cursor'
        ? concatBytes(origBytes.subarray(0, 4), dib)   // 保留 hotspot 2 word
        : dib;
      applied = true;
      onApply(bytes2, t('{note} → 32bpp DIB', { note }));
      closeOverlay(bytes2);
    });

    /* ---------- 启动 ---------- */
    updateRingSize();
    function concatBytes(a, b) {
      const out = new Uint8Array(a.length + b.length);
      out.set(a, 0);
      out.set(b, a.length);
      return out;
    }

    syncPalette();
    syncToolSliders();
    renderRecent();
    applyTransform();
  });
}
