/* PE Viewer page: path input + drag-drop upload + tabbed detail. */

import { apiGet, apiPost, apiUpload } from "../api.js";
import { swapContent } from "../motion.js";

/** WinAPI 试验台白名单（模块级缓存）；导出/导入表据此提供「试验台」跳转。 */
let _callableNames = null;
function callableNames() {
  if (!_callableNames) {
    _callableNames = apiGet("/api/winapi/catalog")
      .then((c) => new Set(c.apis.map((a) => a.name)))
      .catch(() => new Set());
  }
  return _callableNames;
}

const QUICK_FILES = [
  "C:\\Windows\\System32\\ntdll.dll",
  "C:\\Windows\\System32\\kernel32.dll",
  "C:\\Windows\\System32\\user32.dll",
  "C:\\Windows\\System32\\ntoskrnl.exe",
  "C:\\Windows\\System32\\notepad.exe",
];

const RECENT_KEY = "ntlab.recent-pe";

function loadRecent() {
  try { return JSON.parse(localStorage.getItem(RECENT_KEY)) ?? []; } catch { return []; }
}

function pushRecent(path) {
  const list = loadRecent().filter((p) => p !== path);
  list.unshift(path);
  localStorage.setItem(RECENT_KEY, JSON.stringify(list.slice(0, 5)));
}

export async function mount(el) {
  el.innerHTML = `
    <div id="pe-top">
      <div class="page-head pe-top-extra">
        <h1>PE 分析器<span class="page-hint" title="输入本地 PE 文件路径，或直接拖一个 exe/dll/sys 进来">?</span></h1>
      </div>
      <div class="panel">
        <div style="display:flex; gap:10px; align-items:center">
          <ui-textbox id="pe-path" style="flex:1" placeholder="C:\\Windows\\System32\\kernel32.dll"></ui-textbox>
          <ui-button id="pe-go" variant="accent">解析</ui-button>
          <ui-button id="pe-fold" title="收起文件选择区">▲</ui-button>
        </div>
        <div class="pe-top-extra">
          <div class="chips" id="pe-chips"></div>
          <div class="chips" id="pe-recent"></div>
        </div>
      </div>
      <div class="drop-zone pe-top-extra" id="pe-drop" style="margin-top:14px">
        或把 PE 文件拖到这里（≤ 64 MB）
      </div>
    </div>
    <div id="pe-error"></div>
    <div id="pe-result" class="pe-result"></div>`;

  const pathBox = el.querySelector("#pe-path");
  const result = el.querySelector("#pe-result");
  const errBox = el.querySelector("#pe-error");

  function addChip(container, path, removable) {
    const chip = document.createElement("button");
    chip.className = "chip";
    chip.textContent = path.split("\\").pop();
    chip.title = path;
    chip.addEventListener("click", () => { pathBox.value = path; parse(path); });
    if (removable) {
      chip.addEventListener("contextmenu", (e) => {
        e.preventDefault();
        const list = loadRecent().filter((p) => p !== path);
        localStorage.setItem(RECENT_KEY, JSON.stringify(list));
        renderChips();
      });
    }
    container.appendChild(chip);
  }

  function renderChips() {
    const quick = el.querySelector("#pe-chips");
    const recent = el.querySelector("#pe-recent");
    quick.replaceChildren();
    for (const f of QUICK_FILES) addChip(quick, f, false);
    const rec = loadRecent();
    recent.replaceChildren();
    if (rec.length) {
      const label = document.createElement("span");
      label.className = "table-meta";
      label.style.margin = "0 6px 0 0";
      label.textContent = "最近：";
      recent.appendChild(label);
      for (const p of rec) addChip(recent, p, true);
    }
  }
  renderChips();

  el.querySelector("#pe-go").addEventListener("click", () => parse(pathBox.value));
  pathBox.addEventListener("ui-text-commit", (e) => parse(e.detail?.value ?? pathBox.value));
  pathBox.addEventListener("keydown", (e) => { if (e.key === "Enter") parse(pathBox.value); });

  // 顶部文件选择区折叠：解析成功后自动收起（淡出→收起→淡入的动效），为结果区让出垂直空间
  const topArea = el.querySelector("#pe-top");
  const foldBtn = el.querySelector("#pe-fold");
  let topAnimating = false;
  function setTopCollapsed(collapsed) {
    if (topAnimating || topArea.classList.contains("collapsed") === collapsed) return;
    topAnimating = true;
    const extras = topArea.querySelectorAll(".page-head, .pe-top-extra");
    if (collapsed) {
      extras.forEach((n) => n.classList.add("pe-fade-out"));
      setTimeout(() => {
        topArea.classList.add("collapsed");
        extras.forEach((n) => n.classList.remove("pe-fade-out"));
        topAnimating = false;
      }, 150);
    } else {
      topArea.classList.remove("collapsed");
      extras.forEach((n) => n.classList.add("pe-fade-in"));
      setTimeout(() => {
        extras.forEach((n) => n.classList.remove("pe-fade-in"));
        topAnimating = false;
      }, 260);
    }
    foldBtn.setAttribute("label", collapsed ? "▼" : "▲");
    foldBtn.setAttribute("title", collapsed ? "展开文件选择区" : "收起文件选择区");
  }
  foldBtn.addEventListener("click", () => setTopCollapsed(!topArea.classList.contains("collapsed")));

  const drop = el.querySelector("#pe-drop");
  ["dragover", "dragenter"].forEach((ev) => drop.addEventListener(ev, (e) => {
    e.preventDefault(); drop.classList.add("hover");
  }));
  ["dragleave", "drop"].forEach((ev) => drop.addEventListener(ev, (e) => {
    e.preventDefault(); drop.classList.remove("hover");
  }));
  drop.addEventListener("drop", async (e) => {
    const file = e.dataTransfer?.files?.[0];
    if (!file) return;
    if (file.size > 64 * 1024 * 1024) return showErr("文件超过 64 MB 上限，请直接输入路径解析");
    setLoading(true);
    try {
      const r = await apiUpload("/api/pe/upload", file);
      parse(r.path);
    } catch (err) { showErr(err); setLoading(false); }
  });

  function showErr(err) {
    errBox.innerHTML = `<div class="error-box">
      <span class="code">${err.code ?? "error"}</span> — ${esc(err.message ?? String(err))}
      ${err.detail?.status ? `<div class="code">${esc(err.detail.status)} ${esc(err.detail.name ?? "")}</div>` : ""}
    </div>`;
  }

  function clearErr() { errBox.innerHTML = ""; }

  function setLoading(on) {
    drop.style.opacity = on ? "0.4" : "";
    if (on) result.innerHTML = `<div class="loading-row"><ui-spinner></ui-spinner> 正在解析…</div>`;
  }

  /* ---------- 导航历史（跨 PE / 跨标签，Alt+← / Alt+→ 返回与前进） ---------- */
  let parsedPath = null;      // 最近一次 renderResult 的文件路径
  let showTabFn = null;       // renderResult 注册的标签切换入口
  const navStack = [];
  let navPos = -1;
  let navSwitching = false;   // 返回/前进触发的渲染不重复入栈

  const navBar = document.createElement("div");
  navBar.className = "pe-navbar";
  const navLabel = document.createElement("span");
  navLabel.className = "pe-nav-count";
  const backBtn = peNavBtn(-1, "返回", () => navGo(-1));
  const forwardBtn = peNavBtn(1, "前进", () => navGo(1));
  navBar.append(backBtn, forwardBtn, navLabel);
  function updateNav() {
    backBtn.disabled = navPos <= 0;
    forwardBtn.disabled = navPos >= navStack.length - 1;
    navLabel.textContent = navStack.length ? `${navPos + 1} / ${navStack.length}` : "";
  }
  updateNav();
  function pushNav(entry) {
    if (navSwitching) return;
    const cur = navStack[navPos];
    if (cur && cur.path === entry.path && cur.tab === entry.tab && cur.addr === entry.addr) return;
    navStack.length = navPos + 1;
    navStack.push(entry);
    navPos = navStack.length - 1;
    updateNav();
  }
  function navGo(dir) {
    const next = navPos + dir;
    if (next < 0 || next >= navStack.length) return;
    navPos = next;
    updateNav();
    navSwitching = true;
    gotoEntry(navStack[navPos]).finally(() => { navSwitching = false; });
  }
  async function gotoEntry(entry) {
    try {
      if (entry.path !== parsedPath || !showTabFn) {
        pathBox.value = entry.path;
        await parse(entry.path, { push: false });
      }
      if (showTabFn) showTabFn(entry.tab ?? "summary", entry.addr ?? null);
    } catch (err) { showErr(err); }
  }
  const peKeyHandler = (e) => {
    if (!e.altKey) return;
    const t = e.target;
    if (t && typeof t.closest === "function" && t.closest("input,textarea")) return;
    if (e.key === "ArrowLeft") { e.preventDefault(); navGo(-1); }
    else if (e.key === "ArrowRight") { e.preventDefault(); navGo(1); }
  };
  if (window.__ntlabPeNavKey) document.removeEventListener("keydown", window.__ntlabPeNavKey);
  window.__ntlabPeNavKey = peKeyHandler;
  document.addEventListener("keydown", peKeyHandler);

  async function openDll(name) {
    clearErr();
    try {
      const r = await apiGet(`/api/pe/resolve?name=${encodeURIComponent(name)}`);
      if (!r.path) {
        showErr({ code: "not-found", message: `系统目录中未找到模块 ${name}（api-ms-* 转发 DLL 无实体文件，可分析其宿主 DLL）` });
        return;
      }
      pathBox.value = r.path;
      parse(r.path);
    } catch (err) { showErr(err); }
  }

  async function parse(path, { push = true } = {}) {
    if (!path || !path.trim()) return;
    clearErr();
    setLoading(true);
    try {
      const summary = await apiPost("/api/pe/parse", { path: path.trim() });
      pushRecent(path.trim());
      renderChips();
      await renderResult(summary, path.trim());
      setTopCollapsed(true);
      if (push) pushNav({ path: path.trim(), tab: "summary", addr: null });
    } catch (err) {
      result.innerHTML = "";
      showErr(err);
    } finally {
      setLoading(false);
    }
  }

  async function renderResult(s, path) {
    parsedPath = path;
    const tabs = document.createElement("ui-tabs");
    tabs.setAttribute("value", "summary");
    tabs.setAttribute("strip", "");   // osu OsuTabControl：整行底线 500ms 跟随激活项
    const body = document.createElement("div");
    body.className = "pe-tabbody";
    tabs.append(
      tab("summary", "概览"), tab("headers", "DOS/文件头"), tab("imports", "导入表"),
      tab("exports", "导出表"), tab("resources", "资源"), tab("signature", "签名/PDB"),
      tab("strings", "Strings"), tab("hex", "Hex"), tab("disasm", "反汇编"));
    const views = { summary: () => summaryView(s), headers: () => headersView(path) };

    function onCellActivate(e) {
      const { kind, value } = e.detail;
      if (kind === "playground") {
        location.hash = `#/playground?api=${encodeURIComponent(value)}`;
        return;
      }
      // linkOf 传来的 value 是数字；字符串则按 hex 解析
      const v = typeof value === "number" ? value : parseInt(String(value), 16);
      if (kind === "dll") openDll(value);
      else if (kind === "hex") navigate("hex", v);
      else if (kind === "disasm") navigate("disasm", v);
    }

    function showTab(v, addr = null) {
      tabs.setAttribute("value", v);
      if (views[v]) { swapContent(body, views[v]()); return; }
      swapContent(body, spinnerRow("加载中…"));
      (async () => {
        let t = null;
        const url = (ep) => `/api/pe/${ep}?path=${encodeURIComponent(path)}`;
        if (v === "imports") t = await tableView(url("imports"), 300, importCols(await callableNames()),
          (row) => { if (row.iat_rva != null) navigate("hex", row.iat_rva); }, onCellActivate);
        else if (v === "exports") t = await tableView(url("exports"), 300, exportCols(await callableNames()),
          (row) => { if (row.rva) navigate("disasm", row.rva); }, onCellActivate);
        else if (v === "resources") t = await tableView(url("resources"), 300, resCols(), null, onCellActivate);
        else if (v === "signature") t = await signatureView(path);
        else if (v === "strings") t = await stringsView(path, navigate);
        else if (v === "hex") t = hexView(path, addr ?? 0);
        else if (v === "disasm") t = await disasmView(path, addr ?? entryRva(s), { onOpenDll: openDll });
        swapContent(body, t);
      })().catch((err) => swapContent(body, errRow(err)));
    }

    function navigate(tab, addr) {
      pushNav({ path, tab, addr });
      showTab(tab, addr);
    }
    showTabFn = showTab;

    tabs.addEventListener("ui-select", (e) => showTab(e.detail.value));
    body.replaceChildren(views.summary());
    const wrap = document.createElement("div");
    wrap.className = "pe-result-wrap";
    // 视图内部 jump 派发的 ui-pe-nav 冒泡到此 → 入历史栈
    wrap.addEventListener("ui-pe-nav", (e) => {
      pushNav({ path, tab: e.detail.tab, addr: e.detail.addr });
    });
    wrap.append(navBar, tabs, body);
    swapContent(result, wrap, { outMs: 80, inMs: 380 });
  }
}

function tab(value, label) {
  const t = document.createElement("span");
  t.setAttribute("data-value", value);
  t.setAttribute("label", label);
  return t;
}

function entryRva(s) {
  return s.entry_point
    || (s.sections.find((x) => x.flags.includes("EXECUTE")) || {}).virtual_address
    || 0;
}

function spinnerRow(text) {
  const d = document.createElement("div");
  d.className = "loading-row";
  d.innerHTML = `<ui-spinner></ui-spinner> ${esc(text)}`;
  return d;
}

function errRow(err) {
  const d = document.createElement("div");
  d.className = "error-box";
  d.innerHTML = `<span class="code">${esc(err.code ?? "error")}</span> — ${esc(err.message ?? String(err))}`;
  return d;
}

function kvPanel(title, pairs) {
  const div = document.createElement("div");
  div.className = "panel";
  div.style.marginBottom = "14px";
  const h = document.createElement("h3");
  h.textContent = title;
  h.style.margin = "0 0 12px";
  const dl = document.createElement("dl");
  dl.className = "kv";
  for (const [k, v] of pairs) {
    const dt = document.createElement("dt");
    dt.textContent = k;
    const dd = document.createElement("dd");
    dd.textContent = v == null || v === "" ? "—" : String(v);
    dl.append(dt, dd);
  }
  div.append(h, dl);
  return div;
}

function esc(s) {
  return String(s).replace(/[&<>"']/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  }[c]));
}

function summaryView(s) {
  const wrap = document.createElement("div");
  wrap.appendChild(kvPanel("基本信息", [
    ["文件", s.file.path],
    ["大小", `${(s.file.size / 1024).toFixed(1)} KB`],
    ["架构", `${s.machine.name} (0x${s.machine.value.toString(16)})`],
    ["格式", s.magic.name],
    ["类型", s.is_dll ? "DLL" : "EXE"],
    ["子系统", `${s.subsystem.name}`],
    ["入口点 RVA", `0x${s.entry_point.toString(16).toUpperCase()}`],
    ["镜像基址", `0x${s.image_base.toString(16).toUpperCase()}`],
    ["链接器", `${s.linker.major}.${s.linker.minor}`],
    ["OS 版本", s.os_version],
    ["校验和", `声明 0x${s.checksum.stated.toString(16).toUpperCase()} / 实际 0x${(s.checksum.actual ?? 0).toString(16).toUpperCase()}${s.checksum.stated === s.checksum.actual ? " ✓" : " ✗"}`],
  ]));
  const table = document.createElement("ui-table");
  table.columns = [
    { key: "name", label: "节" },
    { key: "virtual_size", label: "VirtualSize", type: "hex" },
    { key: "virtual_address", label: "VirtualAddress", type: "hex" },
    { key: "raw_size", label: "RawSize", type: "hex" },
    { key: "entropy", label: "熵", type: "num" },
    { key: "flags", label: "属性" },
  ];
  table.rows = s.sections.map((sec) => ({ ...sec, flags: sec.flags.join(" · ") }));
  wrap.appendChild(table);
  return wrap;
}

async function headersView(path) {
  const h = await apiGet("/api/pe/headers", { path });
  const wrap = document.createElement("div");
  wrap.appendChild(kvPanel("DOS 头", Object.entries(h.dos_header).map(([k, v]) => [k, typeof v === "number" ? `0x${v.toString(16).toUpperCase()} (${v})` : v])));
  wrap.appendChild(kvPanel("文件头 (FILE_HEADER)", Object.entries(h.file_header)));
  return wrap;
}

async function tableView(url, pageLimit, cols, onRowActivate, onCellActivate) {
  const sep = url.includes("?") ? "&" : "?";
  let items = [];
  let total = 0;
  let offset = 0;

  const first = await apiGet(`${url}${sep}offset=0&limit=${pageLimit}`);
  items = first.items;
  total = first.total;
  offset = items.length;

  const table = document.createElement("ui-table");
  table.setAttribute("fill", "");
  table.columns = cols;
  if (onCellActivate) table.addEventListener("ui-cell-activate", onCellActivate);

  const meta = document.createElement("p");
  meta.className = "table-meta";

  const cs = caseToggle();
  const applyFilter = () => {
    const raw = input.value.trim();
    const q = cs._box.checked ? raw : raw.toLowerCase();
    const norm = (s) => (cs._box.checked ? String(s) : String(s).toLowerCase());
    table.rows = q
      ? items.filter((row) => Object.values(row).some((v) => v != null && norm(v).includes(q)))
      : items;
    meta.textContent = q ? `匹配 ${table.rows.length} 条` : "";
  };
  cs._box.addEventListener("change", applyFilter);

  const input = document.createElement("input");
  input.className = "filter-input";
  input.type = "search";
  input.placeholder = "过滤（名称 / DLL / RVA…）";
  input.addEventListener("input", applyFilter);

  // 增量分页：滚动到底自动取下一页（onLoadMore 契约见 ui-table）
  table.onLoadMore = async () => {
    if (offset >= total) return null;
    const r = await apiGet(`${url}${sep}offset=${offset}&limit=${pageLimit}`);
    items.push(...r.items);
    offset += r.items.length;
    applyFilter();
    return []; // applyFilter 已整体替换行，避免双份追加
  };

  if (onRowActivate) table.addEventListener("ui-row-activate", (e) => onRowActivate(e.detail.row));

  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";
  cs.style.marginBottom = "8px";
  wrap.append(input, cs, meta, table);
  applyFilter();
  return wrap;
}

/* ---------------- IDA-flavored views ---------------- */

function toolbarRow(...children) {
  const bar = document.createElement("div");
  bar.style.cssText = "display:flex;gap:8px;align-items:center;flex-wrap:wrap;margin-bottom:10px;flex:0 0 auto";
  bar.append(...children);
  return bar;
}

function toolInput(placeholder, value, { mono = true, width = "180px" } = {}) {
  const i = document.createElement("input");
  i.className = "filter-input";
  i.style.width = width;
  i.style.marginBottom = "0";
  i.placeholder = placeholder;
  i.value = value ?? "";
  if (mono) i.style.fontFamily = 'var(--ntlab-font-mono, "Cascadia Code", Consolas, "Microsoft YaHei", monospace)';
  return i;
}

function toolBtn(label, onClick, variant) {
  const b = document.createElement("ui-button");
  b.setAttribute("label", label);
  if (variant) b.setAttribute("variant", variant);
  b.addEventListener("click", onClick);
  return b;
}

/** osu! ScreenBackButton / PageSelectorPrevNextButton 风格：细体文字 + SVG chevron。 */
function peNavBtn(dir, label, onClick) {
  const b = document.createElement("button");
  b.type = "button";
  b.className = "ntl-action-btn pe-nav-btn";
  b.dataset.dir = dir < 0 ? "back" : "forward";
  b.title = dir < 0 ? "返回历史 (Alt+←)" : "前进历史 (Alt+→)";
  const chevron = `<svg viewBox="0 0 16 16" aria-hidden="true"><path d="${dir < 0 ? "M10 3L5 8l5 5" : "M6 3l5 5-5 5"}"/></svg>`;
  b.innerHTML = dir < 0 ? `${chevron}<span>${label}</span>` : `<span>${label}</span>${chevron}`;
  b.addEventListener("click", onClick);
  return b;
}

/** "Aa" case-sensitivity checkbox（默认不区分大小写，勾选后区分）。 */
function caseToggle() {
  const label = document.createElement("label");
  label.style.cssText =
    "display:flex;align-items:center;gap:5px;font-size:12px;cursor:pointer;user-select:none;"
    + "color:var(--ui-text-muted,#888);white-space:nowrap;margin-bottom:0";
  const box = document.createElement("input");
  box.type = "checkbox";
  label._box = box;
  label.append(box, document.createTextNode("Aa 区分大小写"));
  return label;
}

async function stringsView(path, navigate = null) {
  const sep = path.includes("?") ? "&" : "?";
  const base = `/api/pe/strings?path=${encodeURIComponent(path)}`;
  let items = [];
  let total = 0;
  let offset = 0;

  const first = await apiGet(`${base}&offset=0&limit=300`);
  items = first.items;
  total = first.total;
  offset = items.length;

  const table = document.createElement("ui-table");
  table.setAttribute("fill", "");
  table.columns = [
    { key: "file_offset", label: "File Offset", type: "hex" },
    { key: "rva", label: "RVA", type: "hex" },
    { key: "section", label: "节" },
    { key: "size", label: "长度", type: "num" },
    { key: "encoding", label: "编码" },
    { key: "text", label: "内容", type: "mono" },
  ];

  const meta = document.createElement("p");
  meta.className = "table-meta";
  const cs = caseToggle();
  const applyFilter = () => {
    const raw = input.value.trim();
    const q = cs._box.checked ? raw : raw.toLowerCase();
    const norm = (s) => (cs._box.checked ? s : s.toLowerCase());
    table.rows = q
      ? items.filter((r) => norm(r.text).includes(q))
      : items;
    meta.textContent = q ? `匹配 ${table.rows.length} 条` : "";
  };
  cs._box.addEventListener("change", applyFilter);
  const input = document.createElement("input");
  input.className = "filter-input";
  input.type = "search";
  input.placeholder = "搜索字符串内容…";
  input.addEventListener("input", applyFilter);

  table.onLoadMore = async () => {
    if (offset >= total) return null;
    const r = await apiGet(`${base}&offset=${offset}&limit=300`);
    items.push(...r.items);
    offset += r.items.length;
    applyFilter();
    return [];
  };

  if (navigate) {
    table.addEventListener("ui-row-activate", (e) => {
      const off = e.detail.row?.file_offset;
      if (off != null) navigate("hex", off);
    });
  }

  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";
  cs.style.marginBottom = "8px";
  wrap.append(input, cs, meta, table);
  applyFilter();
  return wrap;
}

function hexView(path, startRva = 0) {
  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";

  const CHUNK = 2048; // bytes per incremental fetch（≤4096 服务端上限）
  let nextOff = 0;    // 下一段的 file offset；-1 表示到底
  const fsize = (v) => `0x${v.toString(16).toUpperCase()}`;

  const offInput = toolInput("File Offset / RVA（hex）", Math.max(0, startRva | 0).toString(16), { width: "180px" });
  const searchInput = toolInput("搜索文本…", "", { mono: false, width: "180px" });
  const hexInput = toolInput("或字节序列（hex）", "", { width: "160px" });
  const meta = document.createElement("p");
  meta.className = "table-meta";

  const view = document.createElement("ui-codeview");
  view.style.cssText = "flex:1;min-height:0";
  view.renderItem = (row) => {
    const cur = row.file_offset === (view._currentRva ?? -1) ? " cur" : "";
    return `<span class="addr${cur}" data-jump="${row.file_offset.toString(16)}">0x${row.file_offset.toString(16).toUpperCase()}</span>`
      + `<span class="addr" style="cursor:default;color:var(--ui-text-muted,#666)">0x${(row.rva ?? 0).toString(16).toUpperCase()}</span>`
      + `<span class="bytes" style="flex:0 0 440px">${esc(row.hex ?? "")}</span>`
      + `<span class="ops">${esc(row.ascii ?? "")}</span>`;
  };

  async function jump(off, push = true) {
    off = Math.max(0, off | 0);
    const r = await apiGet(`/api/pe/hex?path=${encodeURIComponent(path)}&off=${off}&size=${CHUNK}`);
    nextOff = r.file_offset + r.rows.length * 16;
    if (nextOff >= r.file_size) nextOff = -1;
    view._currentRva = r.file_offset;
    view.resetRows(r.rows, { currentRva: r.file_offset });
    offInput.value = r.file_offset.toString(16);
    meta.textContent = `RVA ${fsize(r.rva)} · File Offset ${fsize(r.file_offset)} · 节 ${r.section ?? "—"}`
      + ` · 滚动到底自动续载 · 点击左侧地址跳转`;
    if (push) view.dispatchEvent(new CustomEvent("ui-pe-nav", { bubbles: true, detail: { tab: "hex", addr: off } }));
  }

  view.loadMore = async () => {
    if (nextOff < 0) return null;
    const r = await apiGet(`/api/pe/hex?path=${encodeURIComponent(path)}&off=${nextOff}&size=${CHUNK}`);
    const atEnd = r.rows.length * 16 < CHUNK || nextOff + r.rows.length * 16 >= r.file_size;
    nextOff = atEnd ? -1 : r.file_offset + r.rows.length * 16;
    return r.rows;
  };

  view.addEventListener("ui-code-jump", (e) => {
    jump(parseInt(e.detail.value, 16)).catch((err) => { meta.textContent = err.message; });
  });

  let searchState = [];
  let searchIdx = 0;

  const searchCs = caseToggle();

  async function doSearch() {
    const text = searchInput.value.trim();
    const hexq = hexInput.value.trim();
    const q = hexq || text;
    const mode = hexq ? "hex" : "text";
    if (!q) return;
    const startAt = Math.max(0, (view._currentRva ?? 0) + 1);
    const r = await apiGet(`/api/pe/search?path=${encodeURIComponent(path)}&q=${encodeURIComponent(q)}&mode=${mode}`
      + `&start=${startAt}&limit=100&case_sensitive=${searchCs._box.checked}`);
    if (!r.matches.length) {
      meta.textContent = `从 ${fsize(startAt)} 起未找到 “${q}”`;
      return;
    }
    searchState = r.matches;
    searchIdx = 0;
    await jump(searchState[0].file_offset);
    meta.textContent = `命中 ${searchState.length} 处（第 1 处，点「下一个」继续）`;
  }

  const bar = toolbarRow(
    offInput,
    toolBtn("跳转", () => {
      const v = parseInt(offInput.value, 16);
      if (Number.isFinite(v)) jump(v).catch((err) => { meta.textContent = err.message; });
    }, "accent"),
    searchInput,
    hexInput,
    searchCs,
    toolBtn("搜索", () => doSearch().catch((err) => { meta.textContent = err.message; }), "accent"),
    toolBtn("下一个", () => {
      if (!searchState.length) return doSearch().catch((err) => { meta.textContent = err.message; });
      searchIdx = (searchIdx + 1) % searchState.length;
      const m = searchState[searchIdx];
      jump(m.file_offset)
        .then(() => { meta.textContent = `命中 ${searchState.length} 处 · 第 ${searchIdx + 1} 处`; })
        .catch((err) => { meta.textContent = err.message; });
    }),
  );
  offInput.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      const v = parseInt(offInput.value, 16);
      if (Number.isFinite(v)) jump(v).catch((err) => { meta.textContent = err.message; });
    }
  });

  wrap.append(bar, meta, view);
  jump(startRva | 0, false).catch((err) => { meta.textContent = err.message; });
  return wrap;
}

async function disasmView(path, startRva = 0, { onOpenDll = null } = {}) {
  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";

  const COUNT = 200;
  const PAGE = 200;
  let nextRva = Math.max(0, startRva | 0);

  const rvaInput = toolInput("RVA（hex）", nextRva.toString(16), { width: "160px" });
  const xrefBox = document.createElement("div");
  xrefBox.className = "table-meta";
  xrefBox.style.marginBottom = "6px";
  const meta = document.createElement("p");
  meta.className = "table-meta";
  meta.style.flex = "0 0 auto";

  const view = document.createElement("ui-codeview");
  view.style.cssText = "flex:1;min-height:0";

  async function fetchPage(rva) {
    return apiGet(`/api/pe/disasm?path=${encodeURIComponent(path)}&rva=${rva}&count=${COUNT}`);
  }

  // 增量分页：滚动到底自动续载下一页；nextRva < 0 表示到底
  view.loadMore = async () => {
    if (nextRva < 0) return null;
    const r = await fetchPage(nextRva);
    const atEnd = r.items.length < PAGE || r.next_rva <= nextRva || r.next_rva >= r.image_size;
    nextRva = atEnd ? -1 : r.next_rva;
    return r.items;
  };

  async function jump(target, push = true) {
    nextRva = target;
    const r = await fetchPage(target);
    const atEnd = r.items.length < PAGE || r.next_rva <= target || r.next_rva >= r.image_size;
    nextRva = atEnd ? -1 : r.next_rva;
    view.resetRows(r.items, { currentRva: target });
    rvaInput.value = target.toString(16);
    meta.textContent = `ImageBase 0x${r.image_base.toString(16).toUpperCase()}`
      + ` · 滚动到底自动续载 · 点击行内地址跳转 · Alt+← 返回`;
    updateXrefs(target);
    if (push) view.dispatchEvent(new CustomEvent("ui-pe-nav", { bubbles: true, detail: { tab: "disasm", addr: target } }));
  }

  async function updateXrefs(rva) {
    xrefBox.replaceChildren();
    const label = document.createElement("span");
    label.textContent = "交叉引用：查询中…";
    xrefBox.appendChild(label);
    try {
      const x = await apiGet(`/api/pe/xrefs?path=${encodeURIComponent(path)}&rva=${rva}`);
      xrefBox.replaceChildren();
      const hint = document.createElement("span");
      hint.textContent = x.total_refs
        ? `交叉引用：被 ${x.total_refs} 处 call/jmp（点击跳到调用处）：`
        : "交叉引用：此地址没有已知的 call/jmp 落点";
      hint.style.color = "var(--ui-text-muted)";
      xrefBox.appendChild(hint);
      x.callers.slice(0, 12).forEach((c) => {
        const a = document.createElement("a");
        a.href = "#";
        a.className = "mono";
        a.style.cssText = "color:var(--ui-accent);margin-right:8px;text-decoration:none";
        a.textContent = `0x${c.rva.toString(16).toUpperCase()}`;
        a.addEventListener("click", (ev) => { ev.preventDefault(); jump(c.rva); });
        xrefBox.appendChild(a);
      });
    } catch {
      xrefBox.replaceChildren();
    }
  }

  const bar = toolbarRow(
    rvaInput,
    toolBtn("跳转", () => {
      const v = parseInt(rvaInput.value, 16);
      if (Number.isFinite(v)) jump(v);
    }, "accent"),
    toolBtn("入口点", () => jump(startRva)),
  );
  rvaInput.addEventListener("keydown", (e) => {
    if (e.key === "Enter") {
      const v = parseInt(rvaInput.value, 16);
      if (Number.isFinite(v)) jump(v);
    }
  });
  view.addEventListener("ui-code-jump", (e) => {
    jump(parseInt(e.detail.value, 16)).catch((err) => { meta.textContent = err.message; });
  });
  view.addEventListener("ui-code-open", (e) => {
    if (onOpenDll) onOpenDll(e.detail.value);
  });

  wrap.append(bar, xrefBox, meta, view);
  jump(startRva, false).catch((err) => { meta.textContent = err.message; });
  return wrap;
}

function importCols(names) {
  return [
    { key: "dll", label: "DLL", type: "link", linkOf: (r) => ({ kind: "dll", value: r.dll }) },
    { key: "function", label: "函数" },
    { key: "demangled", label: "反修饰名", type: "mono" },
    { key: "ordinal", label: "Ordinal", type: "num" },
    { key: "hint", label: "Hint", type: "hex" },
    { key: "iat_rva", label: "IAT RVA", type: "link", linkOf: (r) => ({ kind: "hex", value: r.iat_rva }) },
    { key: "function", label: "试验台", type: "link", linkOf: (r) => names.has(r.function) ? { kind: "playground", value: r.function } : null },
  ];
}

function exportCols(names) {
  return [
    { key: "ordinal", label: "Ordinal", type: "num" },
    { key: "name", label: "导出名", type: "mono" },
    { key: "demangled", label: "反修饰名", type: "mono" },
    { key: "rva", label: "RVA", type: "link", linkOf: (r) => ({ kind: "disasm", value: r.rva }) },
    { key: "forwarder", label: "转发" },
    { key: "name", label: "试验台", type: "link", linkOf: (r) => names.has(r.name) ? { kind: "playground", value: r.name } : null },
  ];
}

function resCols() {
  return [
    { key: "type", label: "类型" },
    { key: "name", label: "名称" },
    { key: "language", label: "语言", type: "num" },
    { key: "size", label: "大小", type: "num" },
    { key: "rva", label: "RVA", type: "link", linkOf: (r) => ({ kind: "hex", value: r.rva }) },
  ];
}

async function signatureView(path) {
  const r = await apiGet("/api/pe/signature", { path });
  const wrap = document.createElement("div");
  const sig = r.signature;
  wrap.appendChild(kvPanel("Authenticode 签名目录", [
    ["存在", sig.present ? "是" : "否"],
    ["文件偏移", sig.rva_file_offset ? `0x${sig.rva_file_offset.toString(16).toUpperCase()}` : "—"],
    ["长度", sig.size ?? "—"],
    ["证书类型", sig.certificate_type ?? "—"],
    ["说明", sig.present ? "仅显示 WIN_CERTIFICATE 头信息；签名链校验可用 signtool verify / WinVerifyTrust" : "该文件没有嵌入签名"],
  ]));
  wrap.appendChild(kvPanel("PDB / 调试信息", r.pdb ? [
    ["PDB 路径", r.pdb.path],
    ["GUID", r.pdb.guid ?? "—"],
    ["Age", r.pdb.age ?? "—"],
  ] : [["PDB", "未找到 CodeView 记录"]]));
  if (r.debug_entries.length) {
    const table = document.createElement("ui-table");
    table.columns = [
      { key: "type", label: "Debug 类型", type: "num" },
      { key: "size_of_data", label: "大小", type: "num" },
      { key: "rva", label: "RVA", type: "hex" },
    ];
    table.rows = r.debug_entries;
    const h = document.createElement("h3");
    h.textContent = `调试目录（${r.debug_entries.length} 项）`;
    h.style.cssText = "margin:16px 0 10px";
    wrap.append(h, table);
  }
  return wrap;
}
