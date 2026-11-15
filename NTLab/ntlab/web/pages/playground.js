/* WinAPI 试验台：白名单调用卡 + 调用队列 + 会话时间线 + x86-64 汇编（仅组装）。
 *
 * 后端白名单见 ntlab/core/winapi.py；参数引用 $N / $N.ret / $N.out.<field>
 * 由运行器在服务端解析。调用在本服务器进程内执行（127.0.0.1 单用户工具）。
 */

import { apiGet, apiPost } from "../api.js";
import { query } from "../router.js";

function esc(s) {
  return String(s).replace(/[&<>"']/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  }[c]));
}

/** 不需要用户输入的参数种类（出参 / 自动长度 / 恒 NULL 指针）。 */
const AUTO_PARAM = (p) => p.hidden
  || p.type.startsWith("out:")
  || ["buf", "bin", "outint", "ptr"].includes(p.type);

const hexOf = (v) => `0x${Number(v).toString(16).toUpperCase()}`;

/* ---------------- 脚本解析：每行 Func(arg, …) ---------------- */

function tokenizeArgs(s) {
  const toks = [];
  let cur = "";
  let q = null;
  for (const ch of s) {
    if (q) {
      if (ch === q) q = null;
      cur += ch;
    } else if (ch === '"' || ch === "'") {
      q = ch;
      cur += ch;
    } else if (ch === ",") {
      if (cur.trim()) toks.push(cur.trim());
      cur = "";
    } else {
      cur += ch;
    }
  }
  if (cur.trim()) toks.push(cur.trim());
  return toks;
}

function parseScript(text, apis) {
  const byName = new Map(apis.map((a) => [a.name, a]));
  const steps = [];
  const errs = [];
  for (const [ln, raw] of text.split("\n").entries()) {
    const line = raw.trim();
    if (!line || line.startsWith("#")) continue;
    const m = line.match(/^(\w+)\s*\((.*)\)\s*;?$/s);
    if (!m) { errs.push(`第 ${ln + 1} 行：格式应为 Func(arg, …)`); continue; }
    const spec = byName.get(m[1]);
    if (!spec) { errs.push(`第 ${ln + 1} 行：${m[1]} 不在白名单`); continue; }
    const toks = tokenizeArgs(m[2]);
    const vis = spec.params.filter((p) => !AUTO_PARAM(p));
    const args = {};
    vis.forEach((p, i) => {
      if (i >= toks.length) return;
      let t = toks[i];
      if (/^".*"$/.test(t) || /^'.*'$/.test(t)) t = t.slice(1, -1);
      else if (t === "null") t = null;
      args[p.name] = t;
    });
    steps.push({ api: spec.name, args, sig: `${spec.name}(${toks.join(", ")})` });
  }
  return { steps, errs };
}

/* ---------------- 页面 ---------------- */

export async function mount(el) {
  el.innerHTML = `
    <div class="page-head">
      <h1>WinAPI 试验台<span class="page-hint" title="白名单内的 API 在本服务器进程中执行；参数可写 $N / $N.ret / $N.out.字段 引用第 N 步的结果；汇编视图只组装不执行">?</span></h1>
      <p>白名单调用 · 队列编排 · NTSTATUS / GetLastError 渲染 · x86-64 汇编组装</p>
    </div>
    <div class="playground">
      <div class="pg-left panel">
        <input class="filter-input" id="pg-search" placeholder="搜索 API / 模块…">
        <div class="pg-catalog" id="pg-catalog"></div>
      </div>
      <div class="pg-right">
        <ui-tabs id="pg-tabs">
          <span data-value="call" label="调用卡"></span>
          <span data-value="queue" label="队列"></span>
          <span data-value="asm" label="汇编"></span>
        </ui-tabs>
        <div class="pg-body" id="pg-body"></div>
      </div>
    </div>`;

  const cat = await apiGet("/api/winapi/catalog");
  const apis = cat.apis;

  const bodyEl = el.querySelector("#pg-body");
  const tabsEl = el.querySelector("#pg-tabs");
  const searchEl = el.querySelector("#pg-search");
  const catalogEl = el.querySelector("#pg-catalog");

  let selected = null;                       // 当前选中的 spec
  let queue = [];                            // [{api, args, sig}]
  let session = null;                        // 最近一次执行结果
  let sessions = [];                         // 历史会话（服务端内存）

  const tab = (value, label) => {
    const t = document.createElement("span");
    t.setAttribute("data-value", value);
    t.setAttribute("label", label);
    return t;
  };

  /* ---- 目录 ---- */
  function renderCatalog(filter = "") {
    const q = filter.trim().toLowerCase();
    const list = q
      ? apis.filter((a) => a.name.toLowerCase().includes(q) || a.module.toLowerCase().includes(q))
      : apis;
    catalogEl.replaceChildren(...list.map((a) => {
      const d = document.createElement("div");
      d.className = "pg-item" + (selected && selected.name === a.name ? " sel" : "");
      d.innerHTML = `<span class="nm">${esc(a.name)}</span>`
        + `<span class="mod">${esc(a.module)}</span>`
        + `<span class="pg-badge${a.side === "observable" ? " warn" : ""}">`
        + `${a.side === "observable" ? "有副作用" : "safe"}</span>`;
      d.addEventListener("click", () => {
        selected = a;
        renderCatalog(filter);
        if (currentTab === "call") swap(renderCallCard());
      });
      return d;
    }));
    if (!list.length) {
      const d = document.createElement("div");
      d.className = "empty";
      d.textContent = "（无匹配 API）";
      catalogEl.replaceChildren(d);
    }
  }
  searchEl.addEventListener("input", () => renderCatalog(searchEl.value));

  /* ---- 调用卡 ---- */
  function argInputs(spec) {
    const rows = [];
    for (const p of spec.params.filter((p) => !AUTO_PARAM(p))) {
      const row = document.createElement("div");
      row.className = "row";
      const def = p.default != null ? String(p.default) : "";
      row.innerHTML = `<label>${esc(p.name)} <span class="dim">(${esc(p.type)})</span></label>`;
      const input = document.createElement("input");
      input.className = "filter-input";
      input.style.width = "260px";
      input.value = def;
      input.placeholder = p.desc || p.type;
      input.dataset.param = p.name;
      row.appendChild(input);
      rows.push(row);
    }
    return { rows };
  }

  function collectArgs(spec, root) {
    const args = {};
    for (const input of root.querySelectorAll("input[data-param]")) {
      const v = input.value.trim();
      if (v !== "") args[input.dataset.param] = v;
    }
    return args;
  }

  function renderRet(part) {
    if (part == null) return "—";
    if (typeof part !== "object") return esc(String(part));
    let s = `${esc(String(part.value))} <span class="dim">(${esc(part.hex ?? "")})</span>`;
    if (part.ntstatus) {
      s += ` · ${part.success ? "STATUS_SUCCESS" : "失败"}`;
      if (part.ntstatus.message) s += ` — ${esc(part.ntstatus.message)}`;
    }
    if (part.last_error) s += ` · GetLastError=${esc(String(part.last_error))}`;
    if (part.valid === false) s += ' <span class="err">INVALID</span>';
    return s;
  }

  function renderStepCard(step, idx) {
    const d = document.createElement("div");
    d.className = "pg-step-card";
    const head = `<h4>#${idx + 1} ${esc(step.module ?? "")}!${esc(step.api ?? "?")}`
      + `<span class="dur">${step.duration_ms != null ? `${step.duration_ms} ms` : ""}</span></h4>`;
    if (step.error) {
      d.innerHTML = head + `<div class="err">${esc(step.error)}</div>`;
      return d;
    }
    const rows = [];
    for (const [k, v] of Object.entries(step.args ?? {})) {
      rows.push(`<tr><td class="k">${esc(k)}</td><td class="v">${esc(v == null ? "null" : String(v))}</td></tr>`);
    }
    rows.push(`<tr><td class="k">返回</td><td class="v">${renderRet(step.ret)}</td></tr>`);
    for (const [k, fields] of Object.entries(step.out ?? {})) {
      for (const [fk, fv] of Object.entries(fields)) {
        rows.push(`<tr><td class="k">${esc(k)}.${esc(fk)}</td><td class="v">${esc(String(fv))}</td></tr>`);
      }
    }
    d.innerHTML = head + `<table>${rows.join("")}</table>`;
    return d;
  }

  function renderSession(session) {
    const wrap = document.createElement("div");
    if (!session) {
      wrap.innerHTML = '<p class="table-meta">尚无执行结果。选择左侧 API，或在队列标签中编排后执行。</p>';
      return wrap;
    }
    const h = document.createElement("h3");
    h.textContent = `会话 ${session.id} · 成功 ${session.ok} / 失败 ${session.failed}`;
    h.style.margin = "0 0 10px";
    wrap.append(h, ...session.steps.map((s, i) => renderStepCard(s, i)));
    return wrap;
  }

  function renderCallCard(result = null) {
    const wrap = document.createElement("div");
    if (!selected) {
      wrap.innerHTML = '<p class="table-meta">从左侧选择一个 API 构建调用。</p>';
      return wrap;
    }
    const a = selected;
    const panel = document.createElement("div");
    panel.className = "panel pg-card";
    panel.style.marginBottom = "12px";
    const head = document.createElement("h3");
    head.innerHTML = `${esc(a.module)}!<b>${esc(a.name)}</b> `
      + `<span class="pg-badge${a.side === "observable" ? " warn" : ""}">${a.side === "observable" ? "有副作用" : "safe"}</span>`;
    const desc = document.createElement("p");
    desc.className = "table-meta";
    desc.textContent = a.desc + (a.note ? ` · ${a.note}` : "");
    panel.append(head, desc);

    const { rows } = argInputs(a);
    if (rows.length) panel.append(...rows);
    else {
      const noArgs = document.createElement("p");
      noArgs.className = "table-meta";
      noArgs.textContent = "（无输入参数，出参自动生成）";
      panel.appendChild(noArgs);
    }

    const btns = document.createElement("div");
    btns.style.cssText = "display:flex;gap:8px;margin:10px 0";
    const runBtn = document.createElement("ui-button");
    runBtn.setAttribute("label", "立即执行");
    runBtn.setAttribute("variant", "accent");
    const addBtn = document.createElement("ui-button");
    addBtn.setAttribute("label", "加入队列");
    btns.append(runBtn, addBtn);
    panel.appendChild(btns);

    const hint = document.createElement("p");
    hint.className = "table-meta";
    hint.textContent = "参数可写 $N / $N.ret / $N.out.字段 引用队列中第 N 步的结果";
    panel.appendChild(hint);

    const out = document.createElement("div");
    if (result) out.appendChild(renderSession(result.session));

    runBtn.addEventListener("click", async () => {
      const args = collectArgs(a, panel);
      try {
        const s = await apiPost("/api/winapi/run", { calls: [{ api: a.name, args }] });
        out.replaceChildren(renderSession(s));
      } catch (err) {
        out.innerHTML = `<div class="error-box"><span class="code">${esc(err.code)}</span> — ${esc(err.message)}</div>`;
      }
    });
    addBtn.addEventListener("click", () => {
      const args = collectArgs(a, panel);
      const sig = `${a.name}(${Object.entries(args).map(([k, v]) => `${k}=${v}`).join(", ")})`;
      queue.push({ api: a.name, args, sig });
      tabsEl.setAttribute("value", "queue");
      swap(queueView());
    });

    wrap.append(panel, out);
    return wrap;
  }

  /* ---- 队列 ---- */
  function queueView() {
    const wrap = document.createElement("div");
    const panel = document.createElement("div");
    panel.className = "panel pg-steps";
    panel.style.marginBottom = "12px";
    const h = document.createElement("h3");
    h.textContent = `调用队列（${queue.length} 步）`;
    panel.appendChild(h);

    const list = document.createElement("div");
    queue.forEach((st, i) => {
      const row = document.createElement("div");
      row.className = "step";
      row.innerHTML = `<span class="no">#${i + 1}</span><span class="sig">${esc(st.sig)}</span>`;
      const del = document.createElement("ui-button");
      del.setAttribute("label", "移除");
      del.addEventListener("click", () => { queue.splice(i, 1); swap(queueView()); });
      row.appendChild(del);
      list.appendChild(row);
    });
    if (!queue.length) {
      const empty = document.createElement("p");
      empty.className = "table-meta";
      empty.textContent = "队列为空：在调用卡中「加入队列」，或在下方写脚本。";
      list.appendChild(empty);
    }
    panel.appendChild(list);

    const btns = document.createElement("div");
    btns.style.cssText = "display:flex;gap:8px;margin:10px 0";
    const runBtn = document.createElement("ui-button");
    runBtn.setAttribute("label", "执行队列");
    runBtn.setAttribute("variant", "accent");
    runBtn.addEventListener("click", async () => {
      if (!queue.length) return;
      try {
        const s = await apiPost("/api/winapi/run", {
          calls: queue.map((q) => ({ api: q.api, args: q.args })),
        });
        session = s;
      } catch (err) {
        session = { id: "error", steps: [{ api: "queue", error: err.message }], ok: 0, failed: 1 };
      }
      swap(queueView());
    });
    const clrBtn = document.createElement("ui-button");
    clrBtn.setAttribute("label", "清空");
    clrBtn.addEventListener("click", () => { queue = []; session = null; swap(queueView()); });
    btns.append(runBtn, clrBtn);
    panel.appendChild(btns);

    const scriptLabel = document.createElement("h3");
    scriptLabel.textContent = "脚本 → 队列（每行一个调用）";
    scriptLabel.style.margin = "14px 0 8px";
    const ta = document.createElement("textarea");
    ta.className = "filter-input";
    ta.style.cssText = "width:100%;height:110px;font-family:var(--ntlab-font-mono,'Cascadia Code',Consolas,'Microsoft YaHei',monospace);resize:vertical";
    ta.placeholder = "# 每行一个调用；参数可写 $N.ret 引用第 N 步结果\nCreateFileW(C:\\Windows\\win.ini)\nCloseHandle($1.ret)\nMessageBoxW(0, \"队列执行完成\", \"NTLab\", 0x40)";
    const parseBtn = document.createElement("ui-button");
    parseBtn.setAttribute("label", "解析并加入队列");
    parseBtn.style.marginTop = "8px";
    parseBtn.addEventListener("click", () => {
      const { steps, errs } = parseScript(ta.value, apis);
      queue.push(...steps);
      if (errs.length) {
        session = { id: "脚本解析", steps: errs.map((e) => ({ api: "script", error: e })), ok: 0, failed: errs.length };
      }
      swap(queueView());
    });
    panel.append(scriptLabel, ta, parseBtn);
    wrap.appendChild(panel);

    if (session) {
      const h2 = document.createElement("h3");
      h2.textContent = "最近一次执行";
      h2.style.margin = "0 0 10px";
      wrap.append(h2, renderSession(session));
    }
    return wrap;
  }

  /* ---- ASM ---- */
  function asmView() {
    const wrap = document.createElement("div");
    const panel = document.createElement("div");
    panel.className = "panel";
    panel.style.marginBottom = "12px";
    const h = document.createElement("h3");
    h.textContent = "x86-64 汇编（仅组装与回读校验，绝不执行）";
    panel.appendChild(h);
    const ta = document.createElement("textarea");
    ta.className = "filter-input";
    ta.style.cssText = "width:100%;height:130px;font-family:var(--ntlab-font-mono,'Cascadia Code',Consolas,'Microsoft YaHei',monospace);resize:vertical";
    ta.placeholder = "mov rax, rcx\nxor rax, rbx\nret";
    const btn = document.createElement("ui-button");
    btn.setAttribute("label", "汇编");
    btn.setAttribute("variant", "accent");
    btn.style.marginTop = "8px";
    const out = document.createElement("div");
    out.style.marginTop = "10px";
    btn.addEventListener("click", async () => {
      try {
        const r = await apiPost("/api/winapi/asm", { text: ta.value });
        const lines = r.roundtrip.map((l) =>
          `<div>${esc(l.mnemonic)} ${esc(l.op)}</div>`).join("");
        out.innerHTML = `<p class="table-meta">${r.size} 字节：${esc(r.bytes_hex)}</p><div class="pg-steps">${lines}</div>`;
      } catch (err) {
        out.innerHTML = `<div class="error-box"><span class="code">${esc(err.code)}</span> — ${esc(err.message)}</div>`;
      }
    });
    panel.append(ta, btn, out);
    wrap.appendChild(panel);
    return wrap;
  }

  /* ---- tabs ---- */
  let currentTab = "call";
  function swap(node) { bodyEl.replaceChildren(node); }
  function showTab(v) {
    currentTab = v;
    tabsEl.setAttribute("value", v);
    if (v === "call") swap(renderCallCard());
    else if (v === "queue") swap(queueView());
    else if (v === "asm") swap(asmView());
  }
  tabsEl.append(tab("call", "调用卡"), tab("queue", "队列"), tab("asm", "汇编"));
  tabsEl.addEventListener("ui-select", (e) => showTab(e.detail.value));

  /* ---- 会话历史入口 ---- */
  const histBtn = document.createElement("ui-button");
  histBtn.setAttribute("label", "历史会话");
  histBtn.style.cssText = "margin-top:10px;margin-bottom:8px";
  histBtn.addEventListener("click", async () => {
    const r = await apiGet("/api/winapi/sessions");
    sessions = r.sessions ?? [];
    if (!sessions.length) {
      swap(Object.assign(document.createElement("div"),
        { innerHTML: '<p class="table-meta">服务器内存中还没有历史会话。</p>' }));
      return;
    }
    const wrap = document.createElement("div");
    const h = document.createElement("h3");
    h.textContent = `历史会话（最近 ${sessions.length} 次，仅存内存）`;
    wrap.append(h, ...sessions.map((s) => renderSession(s)));
    swap(wrap);
  });
  el.querySelector(".page-head").appendChild(histBtn);

  /* ---- 启动 ---- */
  renderCatalog();
  showTab("call");

  // PE 分析器跳转：#/playground?api=CreateFileW
  const pre = query().get("api");
  if (pre) {
    const spec = apis.find((a) => a.name.toLowerCase() === pre.toLowerCase());
    if (spec) {
      selected = spec;
      renderCatalog(searchEl.value);
      tabsEl.setAttribute("value", "call");
      showTab("call");
    }
  }
}
