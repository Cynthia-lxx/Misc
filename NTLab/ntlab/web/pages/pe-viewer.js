/* PE Viewer page: path input + drag-drop upload + tabbed detail. */

import { apiGet, apiPost, apiUpload } from "../api.js";
import { swapContent } from "../motion.js";

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
    <div class="page-head">
      <h1>PE 解剖器</h1>
      <p>输入本地 PE 文件路径，或直接拖一个 exe/dll/sys 进来。</p>
    </div>
    <div class="panel">
      <div style="display:flex; gap:10px; align-items:center">
        <ui-textbox id="pe-path" style="flex:1" placeholder="C:\\Windows\\System32\\kernel32.dll"></ui-textbox>
        <ui-button id="pe-go" variant="accent">解析</ui-button>
      </div>
      <div class="chips" id="pe-chips"></div>
      <div class="chips" id="pe-recent"></div>
    </div>
    <div class="drop-zone" id="pe-drop" style="margin-top:14px">
      或把 PE 文件拖到这里（≤ 64 MB，会上传到本地 server 解析）
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

  async function parse(path) {
    if (!path || !path.trim()) return;
    clearErr();
    setLoading(true);
    try {
      const summary = await apiPost("/api/pe/parse", { path: path.trim() });
      pushRecent(path.trim());
      renderChips();
      await renderResult(summary, path.trim());
    } catch (err) {
      result.innerHTML = "";
      showErr(err);
    } finally {
      setLoading(false);
    }
  }

  async function renderResult(s, path) {
    const tabs = document.createElement("ui-tabs");
    tabs.setAttribute("value", "summary");
    const body = document.createElement("div");
    body.className = "pe-tabbody";
    tabs.append(
      tab("summary", "概览"), tab("headers", "DOS/文件头"), tab("imports", "导入表"),
      tab("exports", "导出表"), tab("resources", "资源"), tab("signature", "签名/PDB"),
      tab("strings", "Strings"), tab("hex", "Hex"), tab("disasm", "反汇编"));
    const views = { summary: () => summaryView(s), headers: () => headersView(path) };
    async function selectTab(e) {
      const v = e.detail.value;
      if (views[v]) { swapContent(body, views[v]()); return; }
      swapContent(body, spinnerRow("加载中…"));
      try {
        if (v === "imports") await tableView(`/api/pe/imports?path=${encodeURIComponent(path)}`, 500, importCols()).then((t) => swapContent(body, t));
        else if (v === "exports") await tableView(`/api/pe/exports?path=${encodeURIComponent(path)}`, 2000, exportCols(), (row) => {
          if (row.rva) {
            tabs.setAttribute("value", "disasm");
            disasmView(path, row.rva).then((t) => swapContent(body, t));
          }
        }).then((t) => swapContent(body, t));
        else if (v === "resources") await tableView(`/api/pe/resources?path=${encodeURIComponent(path)}`, 1000, resCols()).then((t) => swapContent(body, t));
        else if (v === "signature") await signatureView(path).then((t) => swapContent(body, t));
        else if (v === "strings") await stringsView(path).then((t) => swapContent(body, t));
        else if (v === "hex") swapContent(body, hexView(path));
        else if (v === "disasm") swapContent(body, await disasmView(path, entryRva(s)));
      } catch (err) { swapContent(body, errRow(err)); }
    }
    tabs.addEventListener("ui-select", selectTab);
    body.replaceChildren(views.summary());
    const wrap = document.createElement("div");
    wrap.className = "pe-result-wrap";
    wrap.append(tabs, body);
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

async function tableView(url, limit, cols, onRowActivate) {
  const data = await apiGet(`${url}${url.includes("?") ? "&" : "?"}offset=0&limit=${limit}`);
  const table = document.createElement("ui-table");
  table.setAttribute("fill", "");
  table.columns = cols;

  const meta = document.createElement("p");
  meta.className = "table-meta";

  const applyFilter = () => {
    const q = input.value.trim().toLowerCase();
    const rows = !q ? data.items : data.items.filter((row) =>
      Object.values(row).some((v) => v != null && String(v).toLowerCase().includes(q)));
    table.rows = rows;
    meta.textContent = q
      ? `匹配 ${rows.length} / ${data.total} 条（过滤 “${input.value.trim()}”）`
      : `共 ${data.total} 条${data.total > limit ? `，已载入前 ${Math.min(limit, data.total)} 条` : ""}（排序点表头；表格内滚动）`;
  };

  const input = document.createElement("input");
  input.className = "filter-input";
  input.type = "search";
  input.placeholder = "过滤（名称 / DLL / RVA…）";
  input.addEventListener("input", applyFilter);

  if (onRowActivate) table.addEventListener("ui-row-activate", (e) => onRowActivate(e.detail.row));

  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";
  wrap.append(input, meta, table);
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
  if (mono) i.style.fontFamily = '"Cascadia Code", Consolas, monospace';
  return i;
}

function toolBtn(label, onClick, variant) {
  const b = document.createElement("ui-button");
  b.setAttribute("label", label);
  if (variant) b.setAttribute("variant", variant);
  b.addEventListener("click", onClick);
  return b;
}

async function stringsView(path) {
  const data = await apiGet(`/api/pe/strings?path=${encodeURIComponent(path)}&offset=0&limit=1000`);
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
  const applyFilter = () => {
    const q = input.value.trim().toLowerCase();
    const rows = !q ? data.items : data.items.filter((r) => r.text.toLowerCase().includes(q));
    table.rows = rows;
    meta.textContent = q
      ? `匹配 ${rows.length} / ${data.total} 条字符串`
      : `共 ${data.total} 条字符串（ASCII + UTF-16LE，≥5 可打印字符）${data.total > 1000 ? "，已载入前 1000 条" : ""}`;
  };
  const input = document.createElement("input");
  input.className = "filter-input";
  input.type = "search";
  input.placeholder = "搜索字符串内容…";
  input.addEventListener("input", applyFilter);

  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";
  wrap.append(input, meta, table);
  applyFilter();
  return wrap;
}

function hexView(path, startRva = 0) {
  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";

  let rva = Math.max(0, startRva);
  const PAGE = 256;

  const rvaInput = toolInput("RVA（hex，如 1A000）", rva.toString(16), { width: "160px" });
  const searchInput = toolInput("搜索文本…", "", { mono: false, width: "200px" });
  const hexInput = toolInput("或字节序列（hex，4d 5a）", "", { width: "200px" });
  const meta = document.createElement("p");
  meta.className = "table-meta";
  const table = document.createElement("ui-table");
  table.setAttribute("fill", "");
  table.columns = [
    { key: "rva", label: "RVA", type: "hex" },
    { key: "file_offset", label: "File Offset", type: "hex" },
    { key: "hex", label: "字节", type: "mono" },
    { key: "ascii", label: "ASCII", type: "mono" },
  ];

  async function jump(target, { announce = "" } = {}) {
    rva = Math.max(0, target | 0);
    const r = await apiGet(`/api/pe/hex?path=${encodeURIComponent(path)}&rva=${rva}&size=${PAGE}`);
    table.rows = r.rows;
    rvaInput.value = rva.toString(16);
    meta.textContent = `RVA 0x${rva.toString(16).toUpperCase()} · File Offset 0x${r.file_offset.toString(16).toUpperCase()}`
      + ` · 节 ${r.section ?? "—"}${announce ? ` · ${announce}` : ""}`;
  }

  async function doSearch() {
    const text = searchInput.value.trim();
    const hexq = hexInput.value.trim();
    const q = hexq || text;
    const mode = hexq ? "hex" : "text";
    if (!q) return;
    const r = await apiGet(`/api/pe/search?path=${encodeURIComponent(path)}&q=${encodeURIComponent(q)}&mode=${mode}&start=${rva}&limit=50`);
    if (!r.matches.length) { meta.textContent = `从 0x${rva.toString(16).toUpperCase()} 起未找到 “${q}”`; return; }
    const m = r.matches[0];
    await jump(m.file_offset === m.rva ? m.rva : m.file_offset, {});
    // 文件偏移与 RVA 对头区不同：hex() 只接受 rva，header 命中时用 off 视图
    if (m.rva == null) {
      const rr = await apiGet(`/api/pe/hex?path=${encodeURIComponent(path)}&off=${m.file_offset}&size=${PAGE}`);
      table.rows = rr.rows;
      meta.textContent = `命中 File Offset 0x${m.file_offset.toString(16).toUpperCase()}（头部区域）· 共 ${r.total} 处命中，再点「下一个」继续`;
      return;
    }
    meta.textContent = `命中 RVA 0x${m.rva.toString(16).toUpperCase()} · 共 ${r.total} 处命中，再点「下一个」继续`;
    searchState = r.matches;
    searchIdx = 0;
  }

  let searchState = [];
  let searchIdx = 0;

  const bar = toolbarRow(
    toolBtn("◀", () => jump(rva - PAGE).catch(() => {})),
    rvaInput,
    toolBtn("跳转", async () => { await jump(parseInt(rvaInput.value, 16) || 0).catch((e) => { meta.textContent = e.message; }); }, "accent"),
    toolBtn("▶", () => jump(rva + PAGE).catch(() => {})),
    searchInput,
    hexInput,
    toolBtn("搜索", doSearch, "accent"),
    toolBtn("下一个", async () => {
      if (!searchState.length) return doSearch();
      searchIdx = (searchIdx + 1) % searchState.length;
      const m = searchState[searchIdx];
      if (m.rva == null) return;
      await jump(m.rva);
      meta.textContent = `命中 ${searchIdx + 1}/${searchState.length} · RVA 0x${m.rva.toString(16).toUpperCase()}`;
    }),
  );

  rvaInput.addEventListener("keydown", (e) => { if (e.key === "Enter") bar.querySelector("ui-button") && jump(parseInt(rvaInput.value, 16) || 0).catch((x) => { meta.textContent = x.message; }); });

  wrap.append(bar, meta, table);
  jump(startRva).catch((e) => { meta.textContent = e.message; });
  return wrap;
}

async function disasmView(path, startRva = 0) {
  const wrap = document.createElement("div");
  wrap.style.cssText = "display:flex;flex-direction:column;height:100%;min-height:0";

  let rva = Math.max(0, startRva | 0);
  const COUNT = 200;

  const rvaInput = toolInput("RVA（hex）", rva.toString(16), { width: "160px" });
  const xrefBox = document.createElement("div");
  xrefBox.className = "table-meta";
  xrefBox.style.marginBottom = "6px";
  const meta = document.createElement("p");
  meta.className = "table-meta";
  const table = document.createElement("ui-table");
  table.setAttribute("fill", "");
  table.columns = [
    { key: "rva", label: "RVA", type: "hex" },
    { key: "bytes", label: "字节", type: "mono" },
    { key: "mnemonic", label: "指令", type: "mono" },
    { key: "op", label: "操作数", type: "mono" },
    { key: "section", label: "节" },
  ];

  async function render() {
    const r = await apiGet(`/api/pe/disasm?path=${encodeURIComponent(path)}&rva=${rva}&count=${COUNT}`);
    table.rows = r.items;
    rvaInput.value = rva.toString(16);
    meta.textContent = `RVA 0x${rva.toString(16).toUpperCase()} 起 ${r.items.length} 条`
      + ` · ImageBase 0x${r.image_base.toString(16).toUpperCase()}`
      + ` · 上一页/下一页翻页`;
    try {
      const x = await apiGet(`/api/pe/xrefs?path=${encodeURIComponent(path)}&rva=${rva}`);
      xrefBox.replaceChildren();
      const label = document.createElement("span");
      label.textContent = x.total_refs
        ? `交叉引用：此地址被 ${x.total_refs} 处 call/jmp（点 RVA 跳转）：`
        : "交叉引用：此地址没有已知的 call/jmp 落点";
      label.style.color = "var(--ui-text-muted)";
      xrefBox.appendChild(label);
      x.callers.slice(0, 12).forEach((c) => {
        const a = document.createElement("a");
        a.href = "#";
        a.className = "mono";
        a.style.cssText = "color:var(--ui-accent);margin-right:8px;text-decoration:none";
        a.textContent = `0x${c.rva.toString(16).toUpperCase()}`;
        a.addEventListener("click", (ev) => {
          ev.preventDefault();
          rva = c.rva;
          render();
        });
        xrefBox.appendChild(a);
      });
    } catch { xrefBox.textContent = ""; }
  }

  const bar = toolbarRow(
    toolBtn("◀", () => { rva = Math.max(0, rva - COUNT * 4); render(); }),
    rvaInput,
    toolBtn("跳转", () => { const v = parseInt(rvaInput.value, 16); if (Number.isFinite(v)) { rva = v; render(); } }, "accent"),
    toolBtn("▶", () => { rva = rva + COUNT * 4; render(); }),
    toolBtn("入口点", () => { rva = startRva; render(); }),
  );
  rvaInput.addEventListener("keydown", (e) => { if (e.key === "Enter") { const v = parseInt(rvaInput.value, 16); if (Number.isFinite(v)) { rva = v; render(); } } });

  wrap.append(bar, xrefBox, meta, table);
  render();
  return wrap;
}

function importCols() {
  return [
    { key: "dll", label: "DLL", type: "mono" },
    { key: "function", label: "函数" },
    { key: "demangled", label: "反修饰名", type: "mono" },
    { key: "ordinal", label: "Ordinal", type: "num" },
    { key: "hint", label: "Hint", type: "hex" },
    { key: "iat_rva", label: "IAT RVA", type: "hex" },
  ];
}

function exportCols() {
  return [
    { key: "ordinal", label: "Ordinal", type: "num" },
    { key: "name", label: "导出名", type: "mono" },
    { key: "demangled", label: "反修饰名", type: "mono" },
    { key: "rva", label: "RVA", type: "hex" },
    { key: "forwarder", label: "转发" },
  ];
}

function resCols() {
  return [
    { key: "type", label: "类型" },
    { key: "name", label: "名称" },
    { key: "language", label: "语言", type: "num" },
    { key: "size", label: "大小", type: "num" },
    { key: "rva", label: "RVA", type: "hex" },
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
