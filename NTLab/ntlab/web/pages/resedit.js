/* 资源修改器 — 独立页（与 PE 分析器同级）。
 *
 * 打开 PE → ui-tree 树形浏览资源（类型 → 名称 → 语言，带缩进）→
 * 右侧预览/编辑（文本带语法高亮、图标/位图渲染、二进制 hex）→
 * 修改先入暂存区，「保存」（原地，自动 .bak）/「另存为」一次性重建 .rsrc。
 * 文件管理体系独立：自己的路径输入 / 拖放上传 / 打包下载。
 */

import { apiGet, apiUpload, apiSend } from "../api.js";
import { query } from "../router.js";

function esc(s) {
  return String(s).replace(/[&<>"]/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;",
  }[c]));
}

function fsize(n) {
  if (n >= 1024 * 1024) return `${(n / 1024 / 1024).toFixed(1)} MB`;
  if (n >= 1024) return `${(n / 1024).toFixed(1)} KB`;
  return `${n} B`;
}

/* ---------------- 字节工具 ---------------- */

function b64FromBytes(bytes) {
  let bin = "";
  const CHUNK = 0x8000;
  for (let i = 0; i < bytes.length; i += CHUNK)
    bin += String.fromCharCode.apply(null, bytes.subarray(i, i + CHUNK));
  return btoa(bin);
}

function bytesFromB64(b64) {
  const bin = atob(b64);
  const out = new Uint8Array(bin.length);
  for (let i = 0; i < bin.length; i++) out[i] = bin.charCodeAt(i);
  return out;
}

function concatBytes(a, b) {
  const out = new Uint8Array(a.length + b.length);
  out.set(a, 0);
  out.set(b, a.length);
  return out;
}

function errBoxEl(message) {
  const d = document.createElement("div");
  d.className = "error-box";
  d.textContent = message;
  return d;
}

function utf16leEncode(text) {
  const out = new Uint8Array(text.length * 2);
  const dv = new DataView(out.buffer);
  for (let i = 0; i < text.length; i++) dv.setUint16(i * 2, text.charCodeAt(i), true);
  return out;
}

async function fetchResourceBytes(path, type, name, lang) {
  const url = `/api/pe/resources/extract?path=${encodeURIComponent(path)}`
    + `&type=${encodeURIComponent(String(type))}&name=${encodeURIComponent(String(name))}&lang=${lang}`;
  const res = await fetch(url);
  if (!res.ok) {
    const data = await res.json().catch(() => ({}));
    throw new Error(data?.error?.message || `HTTP ${res.status}`);
  }
  return new Uint8Array(await res.arrayBuffer());
}

/* ---------------- 格式检测 ---------------- */

/** 文本启发式：UTF-16LE 零字节分布 + 可打印率。返回 {text, utf16} 或 null。 */
function tryText(bytes) {
  if (!bytes.length) return null;
  const n = Math.min(bytes.length, 512);
  let zeros = 0;
  for (let i = 1; i < n; i += 2) if (bytes[i] === 0) zeros++;
  const utf16 = bytes.length % 2 === 0 && zeros > n / 3;
  let text;
  try {
    text = utf16
      ? new TextDecoder("utf-16le").decode(bytes)
      : new TextDecoder("utf-8", { fatal: false }).decode(bytes);
  } catch { return null; }
  const m = Math.min(text.length, 800);
  if (!m) return null;
  let pr = 0;
  for (let i = 0; i < m; i++) {
    const c = text.charCodeAt(i);
    if (c === 9 || c === 10 || c === 13 || (c >= 32 && c !== 0xfffd)) pr++;
  }
  if (pr / m < 0.85) return null;
  return { text: text.replace(/\u0000+$/, "").replace(/\r\n/g, "\n"), utf16 };
}

/** DIB 头解析（BITMAPINFOHEADER）；不足 40 字节头返回 null。 */
function parseDib(b) {
  if (b.length < 40) return null;
  const dv = new DataView(b.buffer, b.byteOffset, b.byteLength);
  const biSize = dv.getUint32(0, true);
  if (biSize < 40) return null;
  return {
    size: biSize,
    width: dv.getInt32(4, true),
    height: dv.getInt32(8, true),
    bitcount: dv.getUint16(14, true),
    clrUsed: dv.getUint32(32, true),
  };
}

function palBytes(dib) {
  const n = dib.clrUsed || (dib.bitcount <= 8 ? 1 << dib.bitcount : 0);
  return n * 4;
}

/** 裸 DIB → BMP Blob（补 BITMAPFILEHEADER）。 */
function dibToBmpBlob(b) {
  const dib = parseDib(b);
  if (!dib) return null;
  const head = new Uint8Array(14);
  const dv = new DataView(head.buffer);
  dv.setUint8(0, 0x42); dv.setUint8(1, 0x4d);                       // 'BM'
  dv.setUint32(2, 14 + b.length, true);
  dv.setUint32(10, 14 + dib.size + palBytes(dib), true);            // offBits
  return new Blob([head, b], { type: "image/bmp" });
}

/** 裸 DIB → ICO Blob（包一层 ICONDIR + ICONDIRENTRY）。
 *  ICONDIR(6B): reserved@0 type@2 count@4；ICONDIRENTRY(16B) 基址 6：
 *  w@6 h@7 colors@8 reserved@9 planes@10 bitcount@12 size@14 offset@18。 */
function dibToIcoBlob(b) {
  const dib = parseDib(b);
  if (!dib) return null;
  const head = new Uint8Array(22);
  const dv = new DataView(head.buffer);
  dv.setUint16(2, 1, true);                                          // type = icon
  dv.setUint16(4, 1, true);                                          // count
  dv.setUint8(6, Math.min(255, dib.width) & 0xff);
  dv.setUint8(7, Math.min(255, Math.round(dib.height / 2)) & 0xff);  // ico 高 = DIB 高的一半
  dv.setUint8(8, Math.min(255, dib.clrUsed
    || (dib.bitcount <= 8 ? 1 << dib.bitcount : 0)) & 0xff);
  dv.setUint16(10, 1, true);                                         // planes
  dv.setUint16(12, dib.bitcount, true);
  dv.setUint32(14, b.length, true);                                  // bytesInRes
  dv.setUint32(18, 22, true);                                        // imageOffset
  return new Blob([head, b], { type: "image/x-icon" });
}

const IMAGE_TYPES = new Set(["ICON", "BITMAP", "GROUP_ICON", "CURSOR", "GROUP_CURSOR"]);

/** 字节 → hex 文本（16 字节/行，空格分隔）。 */
function hexLines(bytes, limit = 4096) {
  const n = Math.min(bytes.length, limit);
  const rows = [];
  for (let off = 0; off < n; off += 16) {
    const slice = bytes.subarray(off, Math.min(off + 16, n));
    rows.push([...slice].map((x) => x.toString(16).padStart(2, "0")).join(" "));
  }
  return rows.join("\n");
}

/** 宽松解析 hex 文本（容忍空白/冒号/逗号）；字节数为奇数时返回 null。 */
function parseHexText(text) {
  const h = String(text).toLowerCase().replace(/[^0-9a-f]/g, "");
  if (!h.length || h.length % 2) return null;
  const out = new Uint8Array(h.length / 2);
  for (let i = 0; i < out.length; i++) out[i] = parseInt(h.substr(i * 2, 2), 16);
  return out;
}

/** 加载 Blob 为 HTMLImageElement。 */
function loadImageEl(blob) {
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => resolve(img);
    img.onerror = () => reject(new Error("图像解码失败"));
    img.src = URL.createObjectURL(blob);
  });
}

/** ImageData(RGBA) → 32bpp 自底向上 DIB；withMask 时附 AND mask（ICON/CURSOR）。 */
function encodeDib(imageData, withMask) {
  const W = imageData.width, H = imageData.height, d = imageData.data;
  const stride = ((W + 31) >> 5) << 2;             // AND mask 行宽（4B 对齐）
  const maskBytes = withMask ? stride * H : 0;
  const buf = new ArrayBuffer(40 + W * 4 * H + maskBytes);
  const dv = new DataView(buf);
  dv.setUint32(0, 40, true);                        // biSize
  dv.setInt32(4, W, true);
  dv.setInt32(8, withMask ? H * 2 : H, true);       // ICON/CURSOR 的 DIB 高 = 像素 + mask
  dv.setUint16(12, 1, true);                        // planes
  dv.setUint16(14, 32, true);                       // bitcount（统一 32bpp BGRA）
  dv.setUint32(16, 0, true);                        // BI_RGB
  dv.setUint32(20, W * 4 * H + maskBytes, true);    // sizeImage
  const u8 = new Uint8Array(buf);
  let p = 40;
  for (let y = H - 1; y >= 0; y--) {                // 自底向上
    for (let x = 0; x < W; x++) {
      const i = (y * W + x) * 4;
      u8[p++] = d[i + 2]; u8[p++] = d[i + 1]; u8[p++] = d[i]; u8[p++] = d[i + 3];
    }
  }
  if (withMask) {                                   // AND mask：alpha<128 → 位 1
    for (let y = H - 1; y >= 0; y--) {
      for (let x = 0; x < W; x++) {
        if (d[(y * W + x) * 4 + 3] < 128) u8[p + (x >> 3)] |= (0x80 >> (x & 7));
      }
      p += stride;
    }
  }
  return new Uint8Array(buf);
}

/** 内容形态：image / text(xml|ini|plain) / hex。outFormat = 内联编辑的重编码目标。 */
function detectView(bytes, typeLabel) {
  if (bytes.length >= 8 && bytes[0] === 0x89 && bytes[1] === 0x50 && bytes[2] === 0x4e && bytes[3] === 0x47)
    return { kind: "image", mime: "image/png", outFormat: "png" };
  if (typeLabel === "BITMAP") {
    const dib = parseDib(bytes);
    if (dib) return { kind: "image", blob: dibToBmpBlob(bytes), outFormat: "dib-bmp" };
  }
  if (typeLabel === "ICON" || typeLabel === "CURSOR") {
    const dib = parseDib(bytes);
    if (dib) return {
      kind: "image", blob: dibToIcoBlob(bytes),
      outFormat: typeLabel === "CURSOR" ? "dib-cursor" : "dib-icon",
    };
  }
  const t = tryText(bytes);
  if (t) {
    const s = t.text.trimStart();
    let hl = "plain";
    if (/^<\?xml|^</i.test(s)) hl = "xml";
    else if (/^(\[[^\]]+\]\s*$|[\w.]+\s*=)/m.test(s) && !/[\u4e00-\u9fff]{30,}/.test(s)) hl = "ini";
    return { kind: "text", ...t, hl };
  }
  return { kind: "hex" };
}

/* ---------------- 语法高亮 ---------------- */

function highlightMarkup(text, hl) {
  const e = esc(text);
  if (hl === "xml") {
    return e
      .replace(/(&lt;!--[\s\S]*?--&gt;)/g, '<span class="hl-com">$1</span>')
      .replace(/(&lt;\/?)([A-Za-z][\w:.-]*)/g, '$1<span class="hl-tag">$2</span>')
      .replace(/([A-Za-z][\w:.-]*)=(&quot;.*?&quot;)/g,
        '<span class="hl-attr">$1</span>=<span class="hl-str">$2</span>');
  }
  if (hl === "ini") {
    return e
      .replace(/^(\[[^\]]*\])(\s*)$/gm, '<span class="hl-tag">$1</span>$2')
      .replace(/^([A-Za-z][\w. ]*)(=)(.*)$/gm,
        '<span class="hl-attr">$1</span>$2<span class="hl-str">$3</span>');
  }
  return e;
}

/* ---------------- hex 视图（客户端渲染，截断 4 KB） ---------------- */

function hexDumpHTML(bytes) {
  const LIMIT = 4096;
  const n = Math.min(bytes.length, LIMIT);
  let html = "";
  for (let off = 0; off < n; off += 16) {
    const slice = bytes.subarray(off, Math.min(off + 16, n));
    const hex = [...slice].map((x) => x.toString(16).padStart(2, "0")).join(" ");
    const hexPad = hex.padEnd(47, " ");
    const ascii = [...slice].map((x) => (x >= 32 && x < 127 ? String.fromCharCode(x) : ".")).join("");
    html += `<div class="resed-hexrow"><span class="hx-off">${off.toString(16).padStart(8, "0")}</span>`
      + `<span class="hx-bytes">${esc(hexPad)}</span><span class="hx-ascii">${esc(ascii)}</span></div>`;
  }
  if (bytes.length > LIMIT)
    html += `<div class="resed-hexrow resed-trunc">…（其余 ${bytes.length - LIMIT} 字节省略，可导出后用外部工具查看）</div>`;
  return html;
}

/* ---------------- 页面 ---------------- */

export async function mount(el) {
  el.innerHTML = `
    <div id="resed-root">
      <div class="page-head">
        <h1>资源修改器<span class="page-hint" title="Resource Hacker 式的 PE 资源编辑器：树形浏览、预览、编辑，保存/另存为，导出单个资源或整包 zip">?</span></h1>
        <p>打开 PE 文件 → 树形浏览资源（类型 → 名称 → 语言）→ 预览或编辑 → 保存（原地，自动 .bak 备份）/ 另存为</p>
      </div>
      <div class="panel">
        <div style="display:flex; gap:10px; align-items:center; flex-wrap:wrap">
          <ui-textbox id="resed-path" style="flex:1; min-width:260px"
            placeholder="C:\\Windows\\System32\\notepad.exe"></ui-textbox>
          <ui-button id="resed-open" variant="accent">打开</ui-button>
          <ui-button id="resed-save" disabled title="把暂存的修改写回原文件（自动生成 .bak）">保存</ui-button>
          <ui-button id="resed-saveas" disabled title="把暂存的修改写入新文件">另存为</ui-button>
          <button class="ntl-action-btn" id="resed-pack" title="全部资源打包为 zip 下载">打包下载 (.zip)</button>
        </div>
        <div class="resed-meta" id="resed-meta"></div>
      </div>
      <div class="drop-zone resed-drop" id="resed-drop" style="margin-top:14px">
        或把 PE 文件拖到这里（≤ 64 MB）
      </div>
      <div id="resed-error"></div>
      <div id="resed-work">
        <div class="resed-split">
          <div class="resed-left" id="resed-left">
            <p class="table-meta">尚未打开文件。</p>
          </div>
          <div class="resed-right" id="resed-preview">
            <p class="table-meta">从左侧树中选择一个资源。</p>
          </div>
        </div>
      </div>
    </div>`;

  const pathBox = el.querySelector("#resed-path");
  const openBtn = el.querySelector("#resed-open");
  const saveBtn = el.querySelector("#resed-save");
  const saveasBtn = el.querySelector("#resed-saveas");
  const packBtn = el.querySelector("#resed-pack");
  const metaEl = el.querySelector("#resed-meta");
  const drop = el.querySelector("#resed-drop");
  const errBox = el.querySelector("#resed-error");
  const workEl = el.querySelector("#resed-work");
  const leftEl = el.querySelector("#resed-left");
  const previewEl = el.querySelector("#resed-preview");

  let currentPath = null;          // 当前打开的 PE
  let manifest = [];               // /api/pe/resources items
  let treeEl = null;
  let kidsMap = new Map();         // 节点 id → 子节点 spec[]
  let leafMeta = new Map();        // 叶子节点 id → {type,name,lang,size,codepage}
  const staged = new Map();        // key → { entry, label }
  let currentLeafId = null;

  function showErr(err) {
    errBox.innerHTML = `<div class="error-box">
      <span class="code">${esc(err?.code ?? "error")}</span> — ${esc(err?.message ?? String(err))}</div>`;
  }
  function clearErr() { errBox.innerHTML = ""; }

  function updateDirtyUI() {
    const n = staged.size;
    saveBtn.disabled = !currentPath || n === 0;
    saveasBtn.disabled = !currentPath || n === 0;
    metaEl.textContent = currentPath
      ? `${currentPath} — ${manifest.length} 项资源` + (n ? ` · ${n} 项已暂存待写入` : "")
      : "";
  }

  function triggerDownload(url) {
    const a = document.createElement("a");
    a.href = url; a.download = "";
    document.body.appendChild(a); a.click(); a.remove();
  }

  /* ---------- 打开 / 树形构建 ---------- */

  async function openPath(path) {
    if (!path || !path.trim()) return;
    if (staged.size && !confirm("当前文件有未保存的修改，切换文件将丢弃，继续？")) return;
    staged.clear();
    currentLeafId = null;
    clearErr();
    previewEl.innerHTML = '<p class="table-meta">加载中…</p>';
    try {
      const r = await apiGet("/api/pe/resources", { path });
      currentPath = path;
      manifest = r.items ?? [];
      if (!manifest.length) {
        leftEl.innerHTML = '<p class="table-meta">该文件没有资源（无 .rsrc 节）。</p>';
        previewEl.innerHTML = '<p class="table-meta">—</p>';
        updateDirtyUI();
        return;
      }
      buildTree();
      updateDirtyUI();
      previewEl.innerHTML = '<p class="table-meta">从左侧树中选择一个资源。</p>';
    } catch (err) {
      currentPath = null;
      manifest = [];
      updateDirtyUI();
      showErr(err);
    }
  }

  function buildTree() {
    // 类型 → 名称 → 语言
    const types = new Map(); // type → Map(name → items[])
    for (const it of manifest) {
      if (!types.has(it.type)) types.set(it.type, new Map());
      const names = types.get(it.type);
      if (!names.has(it.name)) names.set(it.name, []);
      names.get(it.name).push(it);
    }
    kidsMap = new Map();
    leafMeta = new Map();

    const roots = [];
    for (const [type, names] of types) {
      const tid = `T:${type}`;
      const nameNodes = [];
      let count = 0;
      for (const [name, items] of names) {
        const nid = `N:${type}|${name}`;
        count += items.length;
        if (items.length === 1) {
          const it = items[0];
          const meta = { type, name, lang: it.lang_id, size: it.size, codepage: it.codepage };
          leafMeta.set(nid, meta);
          nameNodes.push({
            id: nid, label: String(name), type: "leaf",
            badge: fsize(it.size), expandable: false,
          });
        } else {
          kidsMap.set(nid, items.map((it) => {
            const lid = `L:${type}|${name}|${it.lang_id}`;
            leafMeta.set(lid, { type, name, lang: it.lang_id, size: it.size, codepage: it.codepage });
            return {
              id: lid, label: `语言 ${it.lang_id}（${it.language}/${it.sublanguage}）`,
              type: "leaf", badge: fsize(it.size), expandable: false,
            };
          }));
          nameNodes.push({
            id: nid, label: String(name), type: "name",
            badge: `${items.length} 语言`, expandable: true,
          });
        }
      }
      kidsMap.set(tid, nameNodes);
      roots.push({
        id: tid, label: type, type: "type",
        badge: `${count} 项`, badgeClass: "directory", expandable: true,
      });
    }
    treeEl = document.createElement("ui-tree");
    treeEl.data = roots;
    treeEl.loadChildren = async (node) => kidsMap.get(node.id) ?? [];
    treeEl.addEventListener("ui-tree-select", (e) => {
      const node = e.detail.node;
      if (leafMeta.has(node.id)) showLeaf(node.id);
    });
    leftEl.replaceChildren(treeEl);
  }

  /* ---------- 预览 / 编辑 ---------- */

  function keyOf(m) { return `${m.type}\u0001${m.name}\u0001${m.lang}`; }

  function stageReplace(meta, bytes, note) {
    staged.set(keyOf(meta), {
      entry: {
        type: meta.type, name: String(meta.name), lang: meta.lang,
        data_b64: b64FromBytes(bytes),
      },
      label: `${meta.type}/${meta.name}/lang${meta.lang}${note ? `（${note}）` : ""}`,
    });
    updateDirtyUI();
    if (currentLeafId) showLeaf(currentLeafId);   // 刷新预览头部的暂存徽标
  }

  function showLeaf(leafId) {
    currentLeafId = leafId;
    const meta = leafMeta.get(leafId);
    const stagedItem = staged.get(keyOf(meta));
    previewEl.innerHTML = '<p class="table-meta">读取资源数据…</p>';
    (async () => {
      const bytes = await fetchResourceBytes(currentPath, meta.type, meta.name, meta.lang);
      const view = detectView(bytes, meta.type);
      renderPreview(meta, bytes, view, stagedItem);
    })().catch((err) => {
      previewEl.innerHTML = "";
      const d = document.createElement("div");
      d.className = "error-box";
      d.innerHTML = `<span class="code">${esc(err?.code ?? "error")}</span> — ${esc(err?.message ?? String(err))}`;
      previewEl.appendChild(d);
    });
  }

  function renderPreview(meta, bytes, view, stagedItem) {
    previewEl.replaceChildren();
    const head = document.createElement("div");
    head.className = "resed-head";
    const title = document.createElement("div");
    title.className = "resed-title";
    title.innerHTML = `<b>${esc(meta.type)}</b> / <b>${esc(String(meta.name))}</b>`
      + ` / 语言 ${meta.lang}`
      + (stagedItem ? '<span class="resed-dirty">已暂存修改</span>' : "");
    const info = document.createElement("div");
    info.className = "table-meta";
    info.textContent = `${fsize(bytes.length)} · codepage ${meta.codepage ?? 0}`
      + (stagedItem ? ` · 暂存：${stagedItem.label}` : "");
    const ops = document.createElement("div");
    ops.className = "resed-ops";

    const dl = document.createElement("a");
    dl.href = "#"; dl.textContent = "导出资源";
    dl.addEventListener("click", (e) => {
      e.preventDefault();
      triggerDownload(`/api/pe/resources/extract?path=${encodeURIComponent(currentPath)}`
        + `&type=${encodeURIComponent(String(meta.type))}&name=${encodeURIComponent(String(meta.name))}`
        + `&lang=${meta.lang}`);
    });

    const rep = document.createElement("a");
    rep.href = "#"; rep.textContent = "从文件替换";
    rep.addEventListener("click", (e) => {
      e.preventDefault();
      pickReplaceFile(meta);
    });

    const undo = document.createElement("a");
    undo.href = "#"; undo.textContent = "撤销修改";
    undo.style.display = stagedItem ? "" : "none";
    undo.addEventListener("click", (e) => {
      e.preventDefault();
      staged.delete(keyOf(meta));
      updateDirtyUI();
      showLeaf(currentLeafId);
    });

    const body = document.createElement("div");
    const enterEdit = (builder) => {
      const orig = [...body.children];
      const cancel = () => body.replaceChildren(...orig);
      body.replaceChildren(builder(cancel));
    };
    ops.append(dl, rep, undo);
    head.append(title, info, ops);
    previewEl.appendChild(head);
    previewEl.appendChild(body);

    if (view.kind === "image") {
      if (view.blob || view.mime) {
        const blob = view.blob || new Blob([bytes], { type: view.mime });
        const url = URL.createObjectURL(blob);
        const img = document.createElement("img");
        img.className = "resed-img";
        img.src = url;
        img.onload = () => {
          info.textContent += ` · ${img.naturalWidth}×${img.naturalHeight}`;
        };
        body.appendChild(img);
        const note = document.createElement("p");
        note.className = "table-meta";
        note.textContent = meta.type === "BITMAP" ? "（裸 DIB 已包上 BMP 文件头渲染）"
          : "（资源数据按图标/位图渲染）";
        body.appendChild(note);
        if (view.outFormat) {
          const ed = document.createElement("a");
          ed.href = "#"; ed.textContent = "编辑图像";
          ed.addEventListener("click", (e) => {
            e.preventDefault();
            enterEdit((cancel) => makeImageEditor(meta, blob, view.outFormat, bytes, cancel));
          });
          ops.appendChild(ed);
        }
      } else {
        appendHex(body, bytes, "（无法解析为图像）");
      }
    } else if (view.kind === "text") {
      const pre = document.createElement("pre");
      pre.className = "resed-hl";
      pre.innerHTML = highlightMarkup(view.text, view.hl);
      body.appendChild(pre);
      const ed = document.createElement("a");
      ed.href = "#"; ed.textContent = "编辑文本";
      ed.addEventListener("click", (e) => {
        e.preventDefault();
        enterEdit((cancel) => makeTextEditor(meta, view.text, view.utf16, view.hl, cancel));
      });
      ops.appendChild(ed);
    } else {
      appendHex(body, bytes);
      if (bytes.length <= 4096) {
        const ed = document.createElement("a");
        ed.href = "#"; ed.textContent = "Hex 编辑";
        ed.addEventListener("click", (e) => {
          e.preventDefault();
          enterEdit((cancel) => makeHexEditor(meta, bytes, cancel));
        });
        ops.appendChild(ed);
      } else {
        const hint = document.createElement("span");
        hint.className = "resed-trunc";
        hint.textContent = "（二进制过大，Hex 编辑仅限 ≤ 4 KB；可导出后用外部工具改，或从文件替换）";
        ops.appendChild(hint);
      }
    }
  }

  function appendHex(container, bytes, note) {
    if (note) {
      const p = document.createElement("p");
      p.className = "table-meta";
      p.textContent = note;
      container.appendChild(p);
    }
    const box = document.createElement("div");
    box.className = "resed-hex";
    box.innerHTML = hexDumpHTML(bytes);
    container.appendChild(box);
  }

  function editBarEl(applyLabel, onApply, cancel) {
    const bar = document.createElement("div");
    bar.className = "resed-editbar";
    const apply = document.createElement("button");
    apply.type = "button";
    apply.className = "ntl-action-btn";
    apply.textContent = applyLabel;
    apply.addEventListener("click", onApply);
    const cancelBtn = document.createElement("button");
    cancelBtn.type = "button";
    cancelBtn.className = "ntl-action-btn";
    cancelBtn.textContent = "取消";
    cancelBtn.addEventListener("click", cancel);
    bar.append(apply, cancelBtn);
    return bar;
  }

  function makeTextEditor(meta, text, utf16, hl, cancel) {
    const wrap = document.createElement("div");
    const hlWrap = document.createElement("div");
    hlWrap.className = "resed-hlwrap";
    const pre = document.createElement("pre");
    pre.innerHTML = highlightMarkup(text, hl);
    const ta = document.createElement("textarea");
    ta.value = text;
    ta.spellcheck = false;
    const sync = () => {
      pre.scrollTop = ta.scrollTop;
      pre.scrollLeft = ta.scrollLeft;
    };
    ta.addEventListener("input", () => {
      pre.innerHTML = highlightMarkup(ta.value, hl);
      sync();
    });
    ta.addEventListener("scroll", sync);
    hlWrap.append(pre, ta);
    const note = document.createElement("p");
    note.className = "table-meta";
    note.textContent = "应用后进入暂存区，仍需「保存」或「另存为」才写盘。";
    wrap.append(hlWrap, editBarEl("应用为资源内容", () => {
      const bytes = utf16 ? utf16leEncode(ta.value)
        : new TextEncoder().encode(ta.value);
      stageReplace(meta, bytes, utf16 ? "UTF-16LE 文本" : "UTF-8 文本");
    }, cancel), note);
    return wrap;
  }

  function makeHexEditor(meta, bytes, cancel) {
    const wrap = document.createElement("div");
    const ta = document.createElement("textarea");
    ta.className = "resed-editor";
    ta.style.display = "block";
    ta.value = hexLines(bytes);
    ta.spellcheck = false;
    const mirror = document.createElement("div");
    mirror.className = "resed-hl";
    const stat = document.createElement("p");
    stat.className = "table-meta";
    const refresh = () => {
      const parsed = parseHexText(ta.value);
      if (parsed) {
        stat.textContent = `解析：${parsed.length} 字节（原 ${bytes.length}；字节数允许变化）`;
        const LIMIT = 512;
        const n = Math.min(parsed.length, LIMIT);
        let s = "";
        for (let i = 0; i < n; i++) {
          const c = parsed[i];
          s += (c >= 32 && c < 127) ? String.fromCharCode(c) : ".";
          if ((i & 63) === 63) s += "\n";
        }
        mirror.textContent = s + (parsed.length > LIMIT ? `\n…（共 ${parsed.length} 字节）` : "");
        mirror.style.display = "";
      } else {
        stat.textContent = "解析失败：hex 字符数为奇数或不含有效 hex";
        mirror.style.display = "none";
      }
    };
    ta.addEventListener("input", refresh);
    refresh();
    wrap.append(ta, stat, mirror, editBarEl("应用为资源内容", () => {
      const parsed = parseHexText(ta.value);
      if (!parsed) return alert("hex 解析失败：请检查格式（每字节两位十六进制）");
      stageReplace(meta, parsed, "Hex 编辑");
    }, cancel));
    return wrap;
  }

  /** 图像编辑器（Mask Editor 式）：画布叠加涂抹层，左键画 / 右键擦。 */
  function makeImageEditor(meta, blob, outFormat, origBytes, cancel) {
    const wrap = document.createElement("div");
    const base = document.createElement("canvas");
    const ovl = document.createElement("canvas");
    ovl.className = "ovl";
    const wrapBox = document.createElement("div");
    wrapBox.className = "resed-canvas-wrap";
    wrapBox.append(base, ovl);
    let brush = 6;
    let color = "#ff4455";
    const undoStack = [];
    const tools = document.createElement("div");
    tools.className = "resed-imgtools";
    const swatches = ["#ff4455", "#ffffff", "#000000", "#44ff88"];
    const swEls = swatches.map((c, i) => {
      const b = document.createElement("button");
      b.type = "button";
      b.className = "swatch" + (i === 0 ? " sel" : "");
      b.style.background = c;
      b.title = c;
      b.addEventListener("click", () => {
        color = c;
        swEls.forEach((x) => x.classList.remove("sel"));
        b.classList.add("sel");
      });
      return b;
    });
    const sizeLabel = document.createElement("span");
    sizeLabel.textContent = `笔刷 ${brush}px`;
    const size = document.createElement("input");
    size.type = "range";
    size.min = 2; size.max = 48; size.value = brush;
    size.addEventListener("input", () => {
      brush = Number(size.value);
      sizeLabel.textContent = `笔刷 ${brush}px`;
    });
    const undoBtn = document.createElement("button");
    undoBtn.type = "button";
    undoBtn.className = "ntl-action-btn";
    undoBtn.textContent = "撤销";
    const clearBtn = document.createElement("button");
    clearBtn.type = "button";
    clearBtn.className = "ntl-action-btn";
    clearBtn.textContent = "清空涂抹";
    tools.append(...swEls, sizeLabel, size, undoBtn, clearBtn);

    (async () => {
      const img = await loadImageEl(blob);
      base.width = ovl.width = img.naturalWidth;
      base.height = ovl.height = img.naturalHeight;
      base.getContext("2d").drawImage(img, 0, 0);
    })().catch((err) => {
      wrap.replaceChildren(errBoxEl(err.message));
    });

    const pos = (e) => {
      const r = ovl.getBoundingClientRect();
      return {
        x: (e.clientX - r.left) * (ovl.width / r.width),
        y: (e.clientY - r.top) * (ovl.height / r.height),
      };
    };
    const snapshot = () => {
      undoStack.push(ovl.getContext("2d").getImageData(0, 0, ovl.width, ovl.height));
      if (undoStack.length > 25) undoStack.shift();
    };
    const stroke = (from, to, erase) => {
      const ctx = ovl.getContext("2d");
      ctx.save();
      ctx.globalCompositeOperation = erase ? "destination-out" : "source-over";
      const paint = erase ? "rgba(0,0,0,1)" : color + "8c";
      ctx.strokeStyle = paint;
      ctx.lineWidth = brush;
      ctx.lineCap = "round";
      ctx.lineJoin = "round";
      ctx.beginPath();
      ctx.moveTo(from.x, from.y);
      ctx.lineTo(to.x, to.y);
      ctx.stroke();
      ctx.restore();
    };
    let drawing = false;
    let eraseMode = false;
    let last = null;
    ovl.addEventListener("pointerdown", (e) => {
      if (!ovl.width) return;
      drawing = true;
      eraseMode = e.button === 2;
      ovl.setPointerCapture(e.pointerId);
      snapshot();
      last = pos(e);
      stroke(last, last, eraseMode);
    });
    ovl.addEventListener("pointermove", (e) => {
      if (!drawing) return;
      const p = pos(e);
      stroke(last, p, eraseMode);
      last = p;
    });
    ovl.addEventListener("pointerup", () => { drawing = false; });
    ovl.addEventListener("contextmenu", (e) => e.preventDefault());
    undoBtn.addEventListener("click", () => {
      const s = undoStack.pop();
      if (s) ovl.getContext("2d").putImageData(s, 0, 0);
    });
    clearBtn.addEventListener("click", () => {
      snapshot();
      ovl.getContext("2d").clearRect(0, 0, ovl.width, ovl.height);
    });

    const note = document.createElement("p");
    note.className = "table-meta";
    note.textContent = "左键涂抹 / 右键擦除；应用时合并进原图。"
      + (outFormat.startsWith("dib") ? "保存时重编码为 32bpp BGRA DIB（原位深不保留）。" : "");

    const onApply = async () => {
      if (!ovl.width) return alert("图像尚未加载完成");
      const merged = document.createElement("canvas");
      merged.width = ovl.width;
      merged.height = ovl.height;
      const ctx = merged.getContext("2d");
      ctx.drawImage(base, 0, 0);
      ctx.drawImage(ovl, 0, 0);
      let bytes2;
      if (outFormat === "png") {
        const blob2 = await new Promise((res) => merged.toBlob(res, "image/png"));
        bytes2 = new Uint8Array(await blob2.arrayBuffer());
      } else {
        const data = ctx.getImageData(0, 0, merged.width, merged.height);
        const dib = encodeDib(data, outFormat !== "dib-bmp");
        bytes2 = outFormat === "dib-cursor"
          ? concatBytes(origBytes.subarray(0, 4), dib)   // 保留 hotspot 2 word
          : dib;
      }
      stageReplace(meta, bytes2,
        outFormat === "png" ? "画布编辑 → PNG" : "画布编辑 → 32bpp DIB");
    };
    wrap.append(tools, wrapBox, note, editBarEl("应用为资源内容", onApply, cancel));
    return wrap;
  }

  function pickReplaceFile(meta) {
    const input = document.createElement("input");
    input.type = "file";
    input.style.display = "none";
    input.addEventListener("change", async () => {
      const file = input.files && input.files[0];
      input.remove();
      if (!file) return;
      const buf = new Uint8Array(await file.arrayBuffer());
      stageReplace(meta, buf, `来自 ${file.name}，${fsize(buf.length)}`);
    });
    document.body.appendChild(input);
    input.click();
  }

  /* ---------- 保存 / 另存为 / 打包 / 上传 ---------- */

  async function commit(inPlace) {
    const entries = [...staged.values()].map((s) => s.entry);
    if (!entries.length || !currentPath) return;
    let outPath = null;
    if (!inPlace) {
      outPath = prompt("另存为路径：",
        currentPath.replace(/\.(exe|dll|sys|ocx|scr|cpl|efi|mui|drv)$/i, ".patched$1"));
      if (!outPath) return;
    }
    saveBtn.disabled = saveasBtn.disabled = true;
    try {
      const r = await apiSend("POST", "/api/pe/resources/replace", {
        path: currentPath, entries, in_place: inPlace, out_path: outPath,
      });
      alert(`已写入 ${r.replaced.length} 项资源\n输出：${r.path}\n`
        + `.rsrc ${r.old_rsrc_size} → ${r.rsrc_size} 字节\nPE CheckSum: ${r.checksum}`
        + (r.backup ? `\n备份：${r.backup}` : "")
        + (r.grew ? "\n（文件已扩展，后续节已平移）" : ""));
      staged.clear();
      if (inPlace) await openPath(currentPath);
      else { metaEl.textContent = `已另存为 ${r.path}`; updateDirtyUI(); }
    } catch (err) {
      showErr(err);
      updateDirtyUI();
    }
  }

  openBtn.addEventListener("click", () => openPath(pathBox.value));
  pathBox.addEventListener("ui-text-commit", (e) => openPath(e.detail?.value ?? pathBox.value));
  pathBox.addEventListener("keydown", (e) => { if (e.key === "Enter") openPath(pathBox.value); });
  saveBtn.addEventListener("click", () => commit(true));
  saveasBtn.addEventListener("click", () => commit(false));
  packBtn.addEventListener("click", () => {
    if (!currentPath) return;
    triggerDownload(`/api/pe/resources/pack?path=${encodeURIComponent(currentPath)}`);
  });

  ["dragover", "dragenter"].forEach((ev) => drop.addEventListener(ev, (e) => {
    e.preventDefault(); drop.classList.add("hover");
  }));
  ["dragleave", "drop"].forEach((ev) => drop.addEventListener(ev, (e) => {
    e.preventDefault(); drop.classList.remove("hover");
  }));
  drop.addEventListener("drop", async (e) => {
    const file = e.dataTransfer?.files?.[0];
    if (!file) return;
    if (file.size > 64 * 1024 * 1024) return showErr({ message: "文件超过 64 MB 上限，请直接输入路径打开" });
    try {
      const r = await apiUpload("/api/pe/upload", file);
      pathBox.value = r.path;
      openPath(r.path);
    } catch (err) { showErr(err); }
  });

  // PE 分析器跳转：#/resedit?path=C:\...
  const prePath = query().get("path");
  if (prePath) {
    pathBox.value = prePath;
    openPath(prePath);
  }
}
