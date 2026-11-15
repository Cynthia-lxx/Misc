/* PE Viewer page: path input + drag-drop upload + tabbed detail. */

import { apiGet, apiPost, apiUpload } from "../api.js";

const QUICK_FILES = [
  "C:\\Windows\\System32\\ntdll.dll",
  "C:\\Windows\\System32\\kernel32.dll",
  "C:\\Windows\\System32\\user32.dll",
  "C:\\Windows\\System32\\ntoskrnl.exe",
  "C:\\Windows\\System32\\notepad.exe",
];

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
    </div>
    <div class="drop-zone" id="pe-drop" style="margin-top:14px">
      或把 PE 文件拖到这里（≤ 64 MB，会上传到本地 server 解析）
    </div>
    <div id="pe-error"></div>
    <div id="pe-result" style="margin-top:18px"></div>`;

  const pathBox = el.querySelector("#pe-path");
  const result = el.querySelector("#pe-result");
  const errBox = el.querySelector("#pe-error");

  for (const f of QUICK_FILES) {
    const chip = document.createElement("button");
    chip.className = "chip";
    chip.textContent = f.split("\\").pop();
    chip.title = f;
    chip.addEventListener("click", () => { pathBox.value = f; parse(f); });
    el.querySelector("#pe-chips").appendChild(chip);
  }

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
    body.style.marginTop = "14px";
    tabs.append(
      tab("summary", "概览"), tab("headers", "DOS/文件头"), tab("imports", "导入表"),
      tab("exports", "导出表"), tab("resources", "资源"), tab("signature", "签名/PDB"));
    const views = { summary: () => summaryView(s), headers: () => headersView(path) };
    async function selectTab(e) {
      const v = e.detail.value;
      if (views[v]) { body.replaceChildren(views[v]()); return; }
      body.replaceChildren(spinnerRow("加载中…"));
      try {
        if (v === "imports") body.replaceChildren(await tableView(`/api/pe/imports?path=${encodeURIComponent(path)}`, 250, importCols()));
        else if (v === "exports") body.replaceChildren(await tableView(`/api/pe/exports?path=${encodeURIComponent(path)}`, 250, exportCols()));
        else if (v === "resources") body.replaceChildren(await tableView(`/api/pe/resources?path=${encodeURIComponent(path)}`, 500, resCols()));
        else if (v === "signature") body.replaceChildren(await signatureView(path));
      } catch (err) { body.replaceChildren(errRow(err)); }
    }
    tabs.addEventListener("ui-select", selectTab);
    result.replaceChildren(tabs, body);
    body.replaceChildren(views.summary());
  }
}

function tab(value, label) {
  const t = document.createElement("span");
  t.setAttribute("data-value", value);
  t.setAttribute("label", label);
  return t;
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

async function tableView(url, limit, cols) {
  const data = await apiGet(`${url}${url.includes("?") ? "&" : "?"}offset=0&limit=${limit}`);
  const table = document.createElement("ui-table");
  table.columns = cols;
  table.rows = data.items;
  const head = document.createElement("p");
  head.style.cssText = "color:var(--ui-text-muted);margin:0 0 10px;font-size:12.5px";
  head.textContent = `共 ${data.total} 条，显示前 ${Math.min(limit, data.total)} 条（双击行无操作；排序点表头）`;
  const wrap = document.createElement("div");
  wrap.append(head, table);
  return wrap;
}

function importCols() {
  return [
    { key: "dll", label: "DLL", type: "mono" },
    { key: "function", label: "函数" },
    { key: "ordinal", label: "Ordinal", type: "num" },
    { key: "hint", label: "Hint", type: "hex" },
    { key: "iat_rva", label: "IAT RVA", type: "hex" },
  ];
}

function exportCols() {
  return [
    { key: "ordinal", label: "Ordinal", type: "num" },
    { key: "name", label: "导出名", type: "mono" },
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
