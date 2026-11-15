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

const hexOf = (v) => `0x${Number(v).toString(16).toUpperCase()}`; // eslint-disable-line
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
    steps.push({ kind: "call", api: spec.name, args, sig: `${spec.name}(${toks.join(", ")})` });
  }
  return { steps, errs };
}

/* ---------------- 页面 ---------------- */

export async function mount(el) {
  el.innerHTML = `
    <div class="page-head">
      <h1>WinAPI 试验台<span class="page-hint" title="白名单内的 API 在本服务器进程中执行；编排区支持循环/条件/跳出块；参数可写 $N / $N.ret / $N.out.字段 引用第 N 个调用块；汇编视图只组装不执行">?</span></h1>
      <p>白名单调用 · 积木编排（循环 / 条件 / 跳出） · NTSTATUS / GetLastError 渲染 · x86-64 汇编组装</p>
    </div>
    <div class="playground">
      <div class="pg-left panel">
        <input class="filter-input" id="pg-search" placeholder="搜索 API / 模块…">
        <div class="pg-catalog" id="pg-catalog"></div>
      </div>
      <div class="pg-right">
        <ui-tabs id="pg-tabs">
          <span data-value="call" label="调用卡"></span>
          <span data-value="queue" label="编排"></span>
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
  const pgLeft = el.querySelector(".pg-left");

  // 左栏 = 编排块的「家门口」：拖块到这里松手即移除
  pgLeft.addEventListener("dragover", (e) => {
    if (![...e.dataTransfer.types].includes("text/pg-move")) return;
    e.preventDefault();
    pgLeft.classList.add("pg-danger");
  });
  pgLeft.addEventListener("dragleave", () => pgLeft.classList.remove("pg-danger"));
  pgLeft.addEventListener("drop", (e) => {
    pgLeft.classList.remove("pg-danger");
    let fromPath = null;
    try { fromPath = JSON.parse(e.dataTransfer.getData("text/pg-move") || "null"); } catch { /* ignore */ }
    if (!Array.isArray(fromPath)) return;
    e.preventDefault();
    takeAt(fromPath);
    if (editingPath === JSON.stringify(fromPath)) editingPath = null;
    showTab("queue");
  });

  let selected = null;                       // 当前选中的 spec
  let program = [];                          // 编排程序树（块：call/repeat/if/break/continue）
  let session = null;                        // 最近一次执行结果
  let sessions = [];                         // 历史会话（服务端内存）
  let editingPath = null;                    // 正在编辑参数的 call 块路径（JSON 字符串）
  let dragState = null;                      // {arr, idx, mark, markEl}

  const specByName = new Map(apis.map((a) => [a.name, a]));

  /** 调用块的默认参数：取白名单默认值（出参/自动参数除外）。 */
  function defaultArgs(spec) {
    const args = {};
    for (const p of spec.params) {
      if (!AUTO_PARAM(p) && p.default != null) args[p.name] = p.default;
    }
    return args;
  }

  function sigOf(name, args) {
    const spec = specByName.get(name);
    // 按白名单参数顺序展示；未在 schema 中的键（不应出现）排在其后
    const keys = spec
      ? [...spec.params.filter((p) => !AUTO_PARAM(p)).map((p) => p.name),
         ...Object.keys(args).filter((k) => !spec.params.some((p) => p.name === k))]
      : Object.keys(args);
    return `${name}(${keys.filter((k) => args[k] != null).map((k) => `${k}=${args[k]}`).join(", ")})`;
  }

  function paramTooltip(p) {
    const def = p.default != null ? `默认 ${p.default}` : "无默认值";
    return `${p.name} (${p.type}) — ${p.desc || "无说明"}；${def}；可写 $N / $N.ret / $N.out.字段 引用第 N 个调用块`;
  }

  /* ---- 编排树操作：path = [idx, "body", idx, "then", idx, ...] ---- */
  function getArr(path) { // path 长度为偶数（不含末端索引）
    let arr = program;
    for (let k = 0; k < path.length; k += 2) arr = arr[path[k]][path[k + 1]];
    return arr;
  }
  function takeAt(path) {
    const parent = path.length === 1 ? program : getArr(path.slice(0, -1));
    return parent.splice(path[path.length - 1], 1)[0];
  }
  /** 程序中第 N 个 call 块（静态编号，DFS 序）。 */
  function countSlots() {
    let n = 0;
    const walk = (nodes) => {
      if (!Array.isArray(nodes)) return;
      for (const nd of nodes) {
        if (nd.kind === "call" || !nd.kind) nd._no = ++n;
        walk(nd.body); walk(nd.then); walk(nd.else);
      }
    };
    walk(program);
  }
  function makeCall(name) {
    const spec = specByName.get(name);
    const args = defaultArgs(spec);
    return { kind: "call", api: name, args, sig: sigOf(name, args) };
  }
  function makeBlock(kind) {
    if (kind === "repeat") return { kind, times: 3, body: [] };
    if (kind === "if") return { kind, cond: "$1.ret.value == 1", then: [], else: [] };
    return { kind };  // break / continue
  }

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
      // 拖拽：把 API 拎进右侧编排区（拖动开始时自动切到编排标签）
      d.draggable = true;
      d.addEventListener("dragstart", (e) => {
        e.dataTransfer.setData("text/pg-api", a.name);
        e.dataTransfer.effectAllowed = "copy";
        showTab("queue");
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
      + `<span class="dur">${[step.where, step.duration_ms != null ? `${step.duration_ms} ms` : ""]
        .filter(Boolean).join(" · ")}</span></h4>`;
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
      wrap.innerHTML = '<p class="table-meta">尚无执行结果。选择左侧 API，或在编排区搭好后执行。</p>';
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
    addBtn.setAttribute("label", "加入编排");
    btns.append(runBtn, addBtn);
    panel.appendChild(btns);

    const hint = document.createElement("p");
    hint.className = "table-meta";
    hint.textContent = "参数可写 $N / $N.ret / $N.out.字段 引用第 N 个调用块";
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
      program.push({ kind: "call", api: a.name, args, sig: sigOf(a.name, args) });
      tabsEl.setAttribute("value", "queue");
      swap(composeView());
    });

    wrap.append(panel, out);
    return wrap;
  }

  /* ---- 调用块的紧凑参数编辑器（点击块头展开） ---- */
  function buildEditor(node, slotNo) {
    const spec = specByName.get(node.api);
    const ed = document.createElement("div");
    ed.className = "pg-edit";
    const h = document.createElement("span");
    h.className = "pg-edit-h";
    h.textContent = `编辑调用 #${slotNo} ${node.api}`;
    ed.appendChild(h);
    for (const p of spec.params.filter((p) => !AUTO_PARAM(p))) {
      const fld = document.createElement("span");
      fld.className = "fld";
      const label = document.createElement("label");
      label.textContent = p.name;
      label.title = paramTooltip(p);
      const input = document.createElement("input");
      input.value = node.args[p.name] != null ? String(node.args[p.name]) : "";
      input.title = paramTooltip(p);
      input.placeholder = p.desc || p.type;
      input.addEventListener("input", () => {
        const v = input.value.trim();
        if (v === "") delete node.args[p.name];
        else node.args[p.name] = v;
        node.sig = sigOf(node.api, node.args);
        const blk = ed.closest(".pg-blk");
        const sigEl = blk && blk.querySelector(".sig");
        if (sigEl) sigEl.textContent = node.sig;
      });
      fld.append(label, input);
      ed.appendChild(fld);
    }
    return ed;
  }

  /* ---- 编排（积木式程序树） ---- */
  function clearDropMarks() {
    document.querySelectorAll(".pg-drop-before,.pg-drop-after,.pg-into")
      .forEach((n) => n.classList.remove("pg-drop-before", "pg-drop-after", "pg-into"));
  }
  function isProgDrag(e) {
    return [...e.dataTransfer.types].some((t) => t === "text/pg-api" || t === "text/pg-move");
  }

  function doDrop(e) {
    const t = dragState;
    dragState = null;
    clearDropMarks();
    if (!t) return;
    let node;
    const apiName = e.dataTransfer.getData("text/pg-api");
    if (apiName) {
      node = makeCall(apiName);
    } else {
      const fromPath = JSON.parse(e.dataTransfer.getData("text/pg-move"));
      node = takeAt(fromPath);
      const fromArr = fromPath.length === 1 ? program : getArr(fromPath.slice(0, -1));
      const fromIdx = fromPath[fromPath.length - 1];
      if (fromArr === t.arr && fromIdx < t.idx) t.idx--;
    }
    if (node) t.arr.splice(t.idx, 0, node);
    swap(composeView());
  }

  /** 通用投放区：getTarget(e) → {arr, idx, mark, markEl}。 */
  function attachDropZone(el, getTarget) {
    el.addEventListener("dragover", (e) => {
      if (!isProgDrag(e)) return;
      e.preventDefault();
      e.stopPropagation();
      e.dataTransfer.dropEffect = e.dataTransfer.types.includes("text/pg-api") ? "copy" : "move";
      const t = getTarget(e);
      if (!t) return;
      dragState = t;
      clearDropMarks();
      t.markEl.classList.add(t.mark);
    });
    el.addEventListener("drop", (e) => {
      if (!isProgDrag(e)) return;
      e.preventDefault();
      e.stopPropagation();
      doDrop(e);
    });
  }

  function renderBlock(node, path) {
    const kind = node.kind || "call";
    const blk = document.createElement("div");
    blk.className = "pg-blk pg-" + (kind === "call" ? "call" : kind);
    blk.dataset.path = JSON.stringify(path);
    const hd = document.createElement("div");
    hd.className = "hd";
    blk.appendChild(hd);
    const isEditing = editingPath === JSON.stringify(path);

    // 拖拽（块头为手柄）+ 投放判定：上/中/下三分
    hd.draggable = !isEditing;
    hd.addEventListener("dragstart", (e) => {
      e.dataTransfer.setData("text/pg-move", JSON.stringify(path));
      e.dataTransfer.effectAllowed = "move";
      blk.classList.add("dragging");
    });
    hd.addEventListener("dragend", () => { blk.classList.remove("dragging"); clearDropMarks(); });
    attachDropZone(blk, (e) => {
      const rc = blk.getBoundingClientRect();
      const rel = (e.clientY - rc.top) / rc.height;
      const last = path[path.length - 1];
      if (containerKeyFor(node)) {
        if (rel < 0.3) return { arr: getArr(path.slice(0, -1)), idx: last, mark: "pg-drop-before", markEl: blk };
        if (rel > 0.7) return { arr: getArr(path.slice(0, -1)), idx: last + 1, mark: "pg-drop-after", markEl: blk };
        const carr = node[containerKeyFor(node)];
        return { arr: carr, idx: carr.length, mark: "pg-into", markEl: blk };
      }
      const idx = rel < 0.5 ? last : last + 1;
      const mark = rel < 0.5 ? "pg-drop-before" : "pg-drop-after";
      return { arr: getArr(path.slice(0, -1)), idx, mark, markEl: blk };
    });

    if (kind === "call") {
      hd.innerHTML = `<span class="no">#${node._no}</span><span class="sig">${esc(node.sig)}</span>`;
      hd.title = "点击编辑参数 · 拖动调整位置 · 拖回左侧列表移除";
      const del = document.createElement("ui-button");
      del.setAttribute("label", "移除");
      del.addEventListener("click", (e) => {
        e.stopPropagation();
        takeAt(path);
        if (editingPath === JSON.stringify(path)) editingPath = null;
        swap(composeView());
      });
      hd.appendChild(del);
      hd.addEventListener("click", () => {
        editingPath = isEditing ? null : JSON.stringify(path);
        swap(composeView());
      });
      if (isEditing) blk.appendChild(buildEditor(node, node._no));
      return blk;
    }

    if (kind === "repeat") {
      hd.innerHTML = `<span class="kw">重复</span>`;
      const inp = document.createElement("input");
      inp.className = "pg-cond";
      inp.value = String(node.times ?? 3);
      inp.title = "循环次数：整数或表达式，可用 $N 引用（如 $2.ret.value）";
      inp.addEventListener("input", () => { node.times = inp.value.trim(); });
      hd.append(inp, mkText("次"));
      blk.appendChild(mkContainer(node, path, "body"));
      return blk;
    }

    if (kind === "if") {
      hd.innerHTML = `<span class="kw">如果</span>`;
      const inp = document.createElement("input");
      inp.className = "pg-cond";
      inp.value = String(node.cond ?? "");
      inp.title = "条件表达式：比较/逻辑运算（支持 && ||），$N.path 引用，如 $1.ret.value == 1";
      inp.addEventListener("input", () => { node.cond = inp.value; });
      hd.appendChild(inp);
      const thenWrap = document.createElement("div");
      thenWrap.className = "pg-branch";
      thenWrap.append(mkText("那么"));
      thenWrap.appendChild(mkContainer(node, path, "then"));
      blk.appendChild(thenWrap);
      if (Array.isArray(node.else)) {
        const elseWrap = document.createElement("div");
        elseWrap.className = "pg-branch";
        elseWrap.append(mkText("否则"));
        elseWrap.appendChild(mkContainer(node, path, "else"));
        blk.appendChild(elseWrap);
      } else {
        const addElse = document.createElement("ui-button");
        addElse.setAttribute("label", "+ 否则");
        addElse.addEventListener("click", () => { node.else = []; swap(composeView()); });
        blk.appendChild(addElse);
      }
      return blk;
    }

    // break / continue
    hd.innerHTML = `<span class="kw">${kind === "break" ? "跳出循环" : "下一轮"}</span>`;
    hd.title = "只能出现在循环块内部";
    return blk;
  }

  function containerKeyFor(node) {
    if (node.kind === "repeat") return "body";
    if (node.kind === "if") return "then";
    return null;
  }

  function mkText(s) {
    const span = document.createElement("span");
    span.className = "kw";
    span.textContent = s;
    return span;
  }

  function mkContainer(node, path, key) {
    const arr = node[key] || [];
    const body = document.createElement("div");
    body.className = "pg-blk-body";
    renderBlocks(arr, body, [...path, key]);
    if (!arr.length) {
      const empty = document.createElement("p");
      empty.className = "table-meta";
      empty.textContent = "（把块拖到这里）";
      body.appendChild(empty);
    }
    attachDropZone(body, () => ({ arr, idx: arr.length, mark: "pg-into", markEl: body }));
    return body;
  }

  function renderBlocks(arr, host, pathPrefix) {
    arr.forEach((node, i) => host.appendChild(renderBlock(node, [...pathPrefix, i])));
  }

  function composeView() {
    countSlots();
    const wrap = document.createElement("div");
    const panel = document.createElement("div");
    panel.className = "panel pg-steps";
    panel.style.marginBottom = "12px";
    const h = document.createElement("h3");
    h.textContent = `编排（${program.length} 个顶层块）`;
    panel.appendChild(h);

    const list = document.createElement("div");
    renderBlocks(program, list, []);
    if (!program.length) {
      const empty = document.createElement("p");
      empty.className = "table-meta";
      empty.textContent = "编排为空：把左侧 API 拖进来，或用下方工具条加循环/条件块。";
      list.appendChild(empty);
    }
    panel.appendChild(list);

    // 根列表 = 投放区（追加到末尾）
    attachDropZone(list, () => ({ arr: program, idx: program.length, mark: "pg-into", markEl: list }));

    const btns = document.createElement("div");
    btns.style.cssText = "display:flex;gap:8px;margin:10px 0;flex-wrap:wrap";
    const add = (label, kind) => {
      const b = document.createElement("ui-button");
      b.setAttribute("label", label);
      b.addEventListener("click", () => { program.push(makeBlock(kind)); swap(composeView()); });
      return b;
    };
    const runBtn = document.createElement("ui-button");
    runBtn.setAttribute("label", "执行");
    runBtn.setAttribute("variant", "accent");
    runBtn.addEventListener("click", async () => {
      if (!program.length) return;
      try {
        session = await apiPost("/api/winapi/run", { program });
      } catch (err) {
        session = { id: "error", steps: [{ api: "program", error: err.message }], ok: 0, failed: 1 };
      }
      swap(composeView());
    });
    const clrBtn = document.createElement("ui-button");
    clrBtn.setAttribute("label", "清空");
    clrBtn.addEventListener("click", () => { program = []; session = null; editingPath = null; swap(composeView()); });
    btns.append(
      runBtn, clrBtn,
      add("+ 循环", "repeat"), add("+ 条件", "if"),
      add("+ 跳出", "break"), add("+ 下一轮", "continue"),
    );
    panel.appendChild(btns);

    const scriptLabel = document.createElement("h3");
    scriptLabel.textContent = "脚本 → 编排（每行一个调用，追加到末尾）";
    scriptLabel.style.margin = "14px 0 8px";
    const ta = document.createElement("textarea");
    ta.className = "filter-input";
    ta.style.cssText = "width:100%;height:110px;font-family:var(--ntlab-font-mono,'Cascadia Code',Consolas,'Microsoft YaHei',monospace);resize:vertical";
    ta.placeholder = "# 每行一个调用；参数可写 $N.ret 引用第 N 个调用块\nCreateFileW(C:\\Windows\\win.ini)\nCloseHandle($1.ret)\nMessageBoxW(0, \"编排执行完成\", \"NTLab\", 0x40)";
    const parseBtn = document.createElement("ui-button");
    parseBtn.setAttribute("label", "解析并加入编排");
    parseBtn.style.marginTop = "8px";
    parseBtn.addEventListener("click", () => {
      const { steps, errs } = parseScript(ta.value, apis);
      program.push(...steps);
      if (errs.length) {
        session = { id: "脚本解析", steps: errs.map((e) => ({ api: "script", error: e })), ok: 0, failed: errs.length };
      }
      swap(composeView());
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
    else if (v === "queue") swap(composeView());
    else if (v === "asm") swap(asmView());
  }
  const tab = (value, label) => {
    const t = document.createElement("span");
    t.setAttribute("data-value", value);
    t.setAttribute("label", label);
    return t;
  };
  tabsEl.append(tab("call", "调用卡"), tab("queue", "编排"), tab("asm", "汇编"));
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
