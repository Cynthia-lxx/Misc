/* 资源修改器 — 独立页（与 PE 分析器同级）。
 *
 * 打开 PE → ui-tree 树形浏览资源（类型 → 名称 → 语言，带缩进）→
 * 右侧预览即编辑（Resource Hacker 式）：文本直接是带行号/语法着色/当前行高亮的
 * 代码编辑器；二进制是虚拟滚动的 Hex 表格（hex/ascii 双列就地编辑，改动字节高亮）；
 * 图像是 Mask Editor 式画布。修改实时入暂存区，「保存」（原地，自动 .bak）/
 * 「另存为」一次性重建 .rsrc。文件管理体系独立：路径输入 / 拖放上传 / 打包下载。
 */

import { apiSend } from "../api.js";
import { query } from "../router.js";
import { openImageEditor } from "./resedit-imgedit.js";
import { apiGetProgress, fetchBytesProgress, fetchBlobProgress, apiUploadProgress, createLoadbar } from "../progress.js";

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

async function fetchResourceBytes(path, type, name, lang, onProgress) {
  const url = `/api/pe/resources/extract?path=${encodeURIComponent(path)}`
    + `&type=${encodeURIComponent(String(type))}&name=${encodeURIComponent(String(name))}&lang=${lang}`;
  return fetchBytesProgress(url, onProgress);
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

/* ---------------- hex 只读视图（截断 4 KB，> 64 KB 时兜底） ---------------- */

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
        <h1>资源修改器<span class="page-hint" title="Resource Hacker 式的 PE 资源编辑器：树形浏览，预览即编辑（文本=代码编辑器 / 二进制=Hex 表格 / 图像=涂抹画布），改动实时入暂存区，保存（原地，自动 .bak）/另存为，导出单个资源或整包 zip">?</span></h1>
        <p>打开 PE 文件 → 树形浏览资源（类型 → 名称 → 语言）→ 在预览中直接编辑 → 保存（原地，自动 .bak 备份）/ 另存为</p>
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
      <div id="resed-load"></div>
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
  const loadHost = el.querySelector("#resed-load");
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

  /** 在 #resed-load 容器里跑一个带进度的加载条；fn(报告进度的回调) 执行加载。 */
  async function runLoadbar(label, fn) {
    const bar = createLoadbar(label);
    loadHost.replaceChildren(bar.el);
    bar.indeterminate();                       // 进度上报前先以不定长模式展示
    try {
      const result = await fn((loaded, total) => bar.set(loaded, total));
      bar.finish();
      return result;
    } catch (err) {
      bar.el.remove();
      throw err;
    }
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
      const r = await runLoadbar(`读取资源表 ${path.split("\\").pop()}`,
        (onProgress) => apiGetProgress("/api/pe/resources", { path }, onProgress));
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

  function stageReplace(meta, bytes, note, silent) {
    staged.set(keyOf(meta), {
      entry: {
        type: meta.type, name: String(meta.name), lang: meta.lang,
        data_b64: b64FromBytes(bytes),
      },
      label: `${meta.type}/${meta.name}/lang${meta.lang}${note ? `（${note}）` : ""}`,
    });
    updateDirtyUI();
    if (!silent && currentLeafId) showLeaf(currentLeafId);   // 刷新预览头部的暂存徽标
  }

  function showLeaf(leafId) {
    currentLeafId = leafId;
    const meta = leafMeta.get(leafId);
    const stagedItem = staged.get(keyOf(meta));
    previewEl.innerHTML = '<p class="table-meta">读取资源数据…</p>';
    (async () => {
      // 暂存优先：编辑器/预览显示待写入内容，而非磁盘上的旧数据
      const bytes = stagedItem
        ? bytesFromB64(stagedItem.entry.data_b64)
        : await runLoadbar(`读取 ${meta.type}/${meta.name}`,
          (onProgress) => fetchResourceBytes(currentPath, meta.type, meta.name, meta.lang, onProgress));
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

    // 编辑器静默暂存后就地更新头部徽标（不重渲染，避免编辑器光标/焦点丢失）
    const markStaged = (label) => {
      if (!title.querySelector(".resed-dirty")) {
        const b = document.createElement("span");
        b.className = "resed-dirty";
        b.textContent = "已暂存修改";
        title.appendChild(b);
      }
      undo.style.display = "";
      info.textContent = `${fsize(bytes.length)} · codepage ${meta.codepage ?? 0} · 暂存：${label}`;
    };

    const body = document.createElement("div");
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
            openImageEditor({
              meta, blob, outFormat: view.outFormat, origBytes: bytes,
              onApply: (bytes2, note) => stageReplace(meta, bytes2, note),
            });
          });
          ops.appendChild(ed);
        }
      } else {
        appendHex(body, bytes, "（无法解析为图像）");
      }
    } else if (view.kind === "text") {
      // 预览即编辑：语法着色 + 行号 + 当前行高亮，输入实时暂存
      body.appendChild(makeCodeEditor(meta, view, markStaged));
    } else if (bytes.length <= 65536) {
      // 预览即编辑：虚拟滚动 Hex 表格（hex/ascii 双列就地编辑）
      body.appendChild(makeHexEditor(meta, bytes, markStaged));
    } else {
      appendHex(body, bytes, `（${fsize(bytes.length)} 超出内联 Hex 编辑上限 64 KB，仅展示前 4 KB）`);
      const hint = document.createElement("span");
      hint.className = "resed-trunc";
      hint.textContent = "（过大，内联 Hex 编辑仅限 ≤ 64 KB；可导出后用外部工具改，或从文件替换）";
      ops.appendChild(hint);
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

  /* ---------- 代码编辑器（预览即编辑）：行号槽 + 高亮层 + 透明 textarea ---------- */

  function makeCodeEditor(meta, view, markStaged) {
    const wrap = document.createElement("div");
    wrap.className = "resed-code";
    const bodyEl = document.createElement("div");
    bodyEl.className = "resed-code-body";
    const gutter = document.createElement("div");
    gutter.className = "resed-gutter";
    const main = document.createElement("div");
    main.className = "resed-code-main";
    const pre = document.createElement("pre");
    const ta = document.createElement("textarea");
    ta.value = view.text;
    ta.spellcheck = false;
    main.append(pre, ta);
    bodyEl.append(gutter, main);
    const status = document.createElement("div");
    status.className = "resed-code-status";
    wrap.append(bodyEl, status);

    const label = view.utf16 ? "UTF-16LE 文本" : "UTF-8 文本";
    const byteLen = (s) => view.utf16
      ? s.length * 2
      : new TextEncoder().encode(s).length;
    let lines = [""];
    let timer = null;

    const layout = () => {
      lines = ta.value.split("\n");
      let g = "";
      for (let i = 1; i <= lines.length; i++) g += `<div class="gline">${i}</div>`;
      gutter.innerHTML = g;
      // 末尾补一个换行：textarea 的尾随换行也占一行，pre 不补会矮一行
      pre.innerHTML = highlightMarkup(ta.value, view.hl) + "\n";
    };

    const caret = () => {
      const p = ta.selectionStart;
      let acc = 0;
      for (let i = 0; i < lines.length; i++) {
        if (p <= acc + lines[i].length) return { ln: i, col: p - acc };
        acc += lines[i].length + 1;
      }
      return { ln: lines.length - 1, col: 0 };
    };

    const updateStatus = () => {
      const { ln, col } = caret();
      const prev = gutter.querySelector(".gline.cur");
      if (prev) prev.classList.remove("cur");
      const gl = gutter.children[ln];
      if (gl) gl.classList.add("cur");
      status.textContent = `行 ${ln + 1}，列 ${col + 1} · ${lines.length} 行`
        + ` · ${byteLen(ta.value)} 字节（${label}）`
        + " · 输入自动暂存，Ctrl+Enter 立即暂存（「保存/另存为」后写盘）";
    };

    const applyNow = () => {
      clearTimeout(timer);
      timer = null;
      stageReplace(meta,
        view.utf16 ? utf16leEncode(ta.value) : new TextEncoder().encode(ta.value),
        label, true);
      markStaged(label);
    };

    ta.addEventListener("input", () => {
      layout();
      updateStatus();
      clearTimeout(timer);
      timer = setTimeout(applyNow, 600);
    });
    ta.addEventListener("scroll", () => {
      pre.scrollTop = ta.scrollTop;
      pre.scrollLeft = ta.scrollLeft;
      gutter.scrollTop = ta.scrollTop;
    });
    ["keyup", "click", "focus"].forEach((ev) => ta.addEventListener(ev, updateStatus));
    ta.addEventListener("keydown", (e) => {
      if ((e.ctrlKey || e.metaKey) && e.key === "Enter") {
        e.preventDefault();
        applyNow();
      }
    });

    layout();
    updateStatus();
    return wrap;
  }

  /* ---------- Hex 编辑器（预览即编辑）：虚拟滚动表格，hex/ascii 双列就地编辑 ---------- */

  function makeHexEditor(meta, bytes, markStaged) {
    const ROWH = 20;
    const buf = bytes.slice();                 // 可写副本
    const mod = new Uint8Array(buf.length);    // 1 = 该字节已修改（高亮）
    let modCount = 0;
    const rows = Math.ceil(buf.length / 16);

    const wrap = document.createElement("div");
    wrap.className = "hexed-wrap";
    const vp = document.createElement("div");
    vp.className = "hexed";
    const spacer = document.createElement("div");
    spacer.className = "hexed-spacer";
    spacer.style.height = `${rows * ROWH}px`;
    vp.appendChild(spacer);
    const status = document.createElement("div");
    status.className = "hexed-status";
    wrap.append(vp, status);

    const hexByte = (i) => buf[i].toString(16).padStart(2, "0");
    const ascText = (i) => {
      const c = buf[i];
      return c >= 32 && c < 127 ? String.fromCharCode(c) : ".";
    };

    // 只渲染可视 ±3 行，绝对定位行容器由 spacer 撑出真实滚动高度；
    // clientHeight 为 0（尚未布局）时按默认 420px 兜底，避免首屏下方空白
    const renderRows = () => {
      const vis = Math.ceil((vp.clientHeight || 420) / ROWH) + 1;
      const first = Math.max(0, Math.floor(vp.scrollTop / ROWH) - 3);
      const last = Math.min(rows, first + vis + 6);
      let html = "";
      for (let r = first; r < last; r++) {
        const off = r * 16;
        html += `<div class="hexed-row" style="top:${r * ROWH}px">`
          + `<span class="hexed-off">${off.toString(16).padStart(8, "0")}</span>`;
        for (let c = 0; c < 16; c++) {
          const i = off + c;
          html += i < buf.length
            ? `<span class="hexed-c${mod[i] ? " mod" : ""}" contenteditable="plaintext-only"`
              + ` spellcheck="false" data-i="${i}">${hexByte(i)}</span>`
            : `<span class="hexed-pad"></span>`;
        }
        html += `<span class="hexed-gap"></span>`;
        for (let c = 0; c < 16; c++) {
          const i = off + c;
          html += i < buf.length
            ? `<span class="hexed-a${mod[i] ? " mod" : ""}" contenteditable="plaintext-only"`
              + ` spellcheck="false" data-i="${i}">${esc(ascText(i))}</span>`
            : `<span class="hexed-pad"></span>`;
        }
        html += `</div>`;
      }
      spacer.innerHTML = html;
    };

    let raf = 0;
    vp.addEventListener("scroll", () => {
      if (!raf) raf = requestAnimationFrame(() => { raf = 0; renderRows(); });
    });

    const findCell = (i, hexCls) =>
      spacer.querySelector(`${hexCls ? ".hexed-c" : ".hexed-a"}[data-i="${i}"]`);

    const focusCell = (el) => {
      if (!el) return;
      el.focus();
      const r = document.createRange();
      r.selectNodeContents(el);
      r.collapse(false);
      const sel = getSelection();
      sel.removeAllRanges();
      sel.addRange(r);
    };

    const markMod = (i) => {
      if (!mod[i]) { mod[i] = 1; modCount++; }
    };

    const updStatus = () => {
      status.textContent = modCount
        ? `${modCount} / ${buf.length} 字节已修改 · 已自动暂存`
        + " · Ctrl+Enter 立即暂存（「保存/另存为」后写盘）"
        : `点击任意 Hex 或 ASCII 单元格直接修改（字节数不变，0 / ${buf.length} 已修改）`;
    };

    let timer = null;
    const stage = (immediate) => {
      if (!modCount) return;
      clearTimeout(timer);
      const doStage = () => {
        const note = `Hex 编辑，${modCount} 字节改动`;
        stageReplace(meta, buf.slice(), note, true);
        markStaged(note);
      };
      if (immediate) doStage();
      else timer = setTimeout(doStage, 400);
    };

    // 事件委托：单元格 input → 清洗/写回 buf → 同步镜像列 → 焦点右移
    spacer.addEventListener("input", (e) => {
      const cell = e.target.closest?.("[contenteditable]");
      if (!cell) return;
      const i = Number(cell.dataset.i);
      if (Number.isNaN(i)) return;
      if (cell.classList.contains("hexed-c")) {
        const t = cell.textContent.toLowerCase().replace(/[^0-9a-f]/g, "").slice(-2);
        cell.textContent = t;
        if (t.length === 2) {
          buf[i] = parseInt(t, 16);
          markMod(i);
          cell.classList.add("mod");
          const a = findCell(i, false);
          if (a) { a.textContent = ascText(i); a.classList.add("mod"); }
          updStatus();
          stage(false);
          focusCell(findCell(i + 1, true));
        }
      } else {
        const ch = cell.textContent.slice(-1);
        const code = ch.charCodeAt(0);
        if (ch && code >= 32 && code < 256) {
          buf[i] = code;
          markMod(i);
          cell.textContent = ch;
          cell.classList.add("mod");
          const h = findCell(i, true);
          if (h) { h.textContent = hexByte(i); h.classList.add("mod"); }
          updStatus();
          stage(false);
          focusCell(findCell(i + 1, false));
        } else {
          cell.textContent = ascText(i);   // 无效输入回弹
        }
      }
    });

    spacer.addEventListener("keydown", (e) => {
      const cell = e.target.closest?.("[contenteditable]");
      if (!cell) return;
      if (e.key === "Enter" || ((e.ctrlKey || e.metaKey) && e.key === "Enter")) {
        e.preventDefault();
        stage(true);
        return;
      }
      const i = Number(cell.dataset.i);
      const isHex = cell.classList.contains("hexed-c");
      const nav = (ni, nh) => { e.preventDefault(); focusCell(findCell(ni, nh)); };
      if (e.key === "ArrowRight") nav(i + 1, isHex);
      else if (e.key === "ArrowLeft") nav(i - 1, isHex);
      else if (e.key === "ArrowDown") nav(i + 16, isHex);
      else if (e.key === "ArrowUp") nav(i - 16, isHex);
      else if (e.key === "Tab") { e.preventDefault(); focusCell(findCell(i, !isHex)); }
    });

    // 首渲染时机：挂载前 clientHeight 为 0，rAF 在无头/后台标签下也可能过早；
    // ResizeObserver 首次布局回调 + rAF 兜底
    const ro = new ResizeObserver(() => {
      if (!vp.isConnected) { ro.disconnect(); return; }
      renderRows();
    });
    ro.observe(vp);
    requestAnimationFrame(() => renderRows());
    updStatus();
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
      const r = await runLoadbar(inPlace ? "写回原文件（自动 .bak）" : "另存为新文件",
        async () => apiSend("POST", "/api/pe/resources/replace", {
          path: currentPath, entries, in_place: inPlace, out_path: outPath,
        }));
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
  packBtn.addEventListener("click", async () => {
    if (!currentPath) return;
    try {
      const blob = await runLoadbar("打包下载全部资源",
        (onProgress) => fetchBlobProgress(`/api/pe/resources/pack?path=${encodeURIComponent(currentPath)}`, onProgress));
      const url = URL.createObjectURL(blob);
      const a = document.createElement("a");
      a.href = url;
      a.download = currentPath.split("\\").pop().replace(/\.[^.]+$/, "") + ".zip";
      document.body.appendChild(a); a.click(); a.remove();
      window.setTimeout(() => URL.revokeObjectURL(url), 5000);
    } catch (err) {
      showErr(err);
    }
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
      const r = await runLoadbar(`上传 ${file.name}（${fsize(file.size)}）`,
        (onProgress) => apiUploadProgress("/api/pe/upload", file, onProgress));
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
