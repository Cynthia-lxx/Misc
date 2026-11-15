/* Generic AI content template manager — vanilla JS client.
 * Mirrors Coopanion's prompt editor ideas (segmented templates, dirty tracking,
 * optimistic concurrency, variable chips) without any framework dependency.
 */
"use strict";

const I18N = {
  zh: {
    appTitle: "AI 内容模板管理", appSubtitle: "为任何基于 LLM API 的工具编辑系统提示词模板",
    presetLabel: "角色预设", reload: "刷新", saveAll: "全部保存", templates: "模板段",
    editorHint: "按顺序拼接为完整系统提示词 · 灰色生成段由工具在运行时填充",
    variables: "模板变量", variablesHint: "模板中使用 {{name}} 或 {{name|默认值}}",
    preview: "最终提示词预览", copy: "复制", reset: "恢复默认",
    dirty: "未保存", saved: "已保存", saving: "保存中…",
    loadFail: "加载失败", saveFail: "保存失败", copied: "已复制", copyFail: "复制失败",
    conflict: "内容已被其他位置修改，请刷新后再保存",
    role_role: "角色", role_behavior: "行为", role_output: "输出", role_context: "上下文",
    origin_user: "已修改", origin_builtin: "内置", origin_generated: "生成",
    reloadOk: "已刷新", presetOk: "预设已应用", varsOk: "变量已更新",
  },
  en: {
    appTitle: "AI Content Template Manager", appSubtitle: "Edit system prompt templates for any LLM API tool",
    presetLabel: "Preset", reload: "Reload", saveAll: "Save all", templates: "Template segments",
    editorHint: "Joined in order into the full system prompt · grey generated segments are filled by the tool at runtime",
    variables: "Template variables", variablesHint: "Use {{name}} or {{name|fallback}} in templates",
    preview: "Final prompt preview", copy: "Copy", reset: "Reset",
    dirty: "Unsaved", saved: "Saved", saving: "Saving…",
    loadFail: "Load failed", saveFail: "Save failed", copied: "Copied", copyFail: "Copy failed",
    conflict: "Content changed elsewhere — reload before saving",
    role_role: "Role", role_behavior: "Behavior", role_output: "Output", role_context: "Context",
    origin_user: "Edited", origin_builtin: "Built-in", origin_generated: "Generated",
    reloadOk: "Reloaded", presetOk: "Preset applied", varsOk: "Variables updated",
  },
};
let lang = (navigator.language || "zh").toLowerCase().startsWith("zh") ? "zh" : "en";
const T = (k) => (I18N[lang] || I18N.en)[k] || k;

const $ = (s, root = document) => root.querySelector(s);
const el = (tag, cls, text) => {
  const n = document.createElement(tag);
  if (cls) n.className = cls;
  if (text !== undefined) n.textContent = text;
  return n;
};

const state = {
  templates: [],
  variables: [],
  presets: [],
  activePreset: null,
  docs: new Map(),      // key -> { content, revision } last loaded / saved
  dirty: new Map(),     // key -> current edited text
  collapsed: new Set(),
  varsValues: {},
  saving: false,
};

function applyI18n() {
  document.querySelectorAll("[data-i18n]").forEach((node) => {
    node.textContent = T(node.getAttribute("data-i18n"));
  });
}

async function api(path, options = {}) {
  const res = await fetch(path, {
    ...options,
    headers: { "Content-Type": "application/json", ...(options.headers || {}) },
  });
  let data;
  try { data = await res.json(); } catch { data = {}; }
  if (!res.ok) {
    const err = new Error(data.error || `HTTP ${res.status}`);
    err.status = res.status;
    throw err;
  }
  return data;
}

function toast(message, isError = false) {
  const box = $("#toast");
  box.textContent = message;
  box.classList.toggle("error", isError);
  box.classList.add("show");
  clearTimeout(toast._t);
  toast._t = setTimeout(() => box.classList.remove("show"), 2200);
}

/* ---------- rendering ---------- */
function renderTemplates() {
  const list = $("#template-list");
  list.replaceChildren();
  for (const tpl of state.templates) {
    const key = tpl.key;
    const doc = state.docs.get(key) || { content: tpl.content, revision: tpl.revision };
    const collapsed = state.collapsed.has(key);

    const card = el("div", "tpl" + (collapsed ? " collapsed" : ""));
    card.dataset.key = key;

    const head = el("div", "tpl-head");
    const chevron = el("span", "tpl-chevron", "▾");
    const title = el("span", "tpl-title", tpl.title);
    const role = el("span", `tpl-role role-${tpl.role}`, T(`role_${tpl.role}`) || tpl.role);
    head.append(chevron, title, role);

    const originCls = tpl.generated ? "generated" : (tpl.origin === "user" ? "user" : "builtin");
    const originText = tpl.generated ? T("origin_generated") : (tpl.origin === "user" ? T("origin_user") : T("origin_builtin"));
    head.append(el("span", `badge ${originCls}`, originText));
    head.append(el("span", "tpl-spacer"));
    const dirtyFlag = el("span", "tpl-dirty", "● " + T("dirty"));
    head.append(dirtyFlag);
    card.classList.toggle("dirty", state.dirty.has(key));
    head.addEventListener("click", () => {
      if (state.collapsed.has(key)) state.collapsed.delete(key); else state.collapsed.add(key);
      card.classList.toggle("collapsed");
    });
    card.append(head);

    const body = el("div", "tpl-body");
    body.append(el("div", "tpl-desc", tpl.description || ""));

    if (tpl.vars && tpl.vars.length) {
      const chips = el("div", "tpl-desc");
      for (const name of tpl.vars) {
        const chip = el("span", "var-chip", `{{${name}}}`);
        chip.title = name;
        chip.addEventListener("click", () => $("#variable-list")?.querySelector(`[data-var="${CSS.escape(name)}"] input`)?.focus());
        chips.append(chip);
      }
      body.append(chips);
    }

    const textarea = el("textarea");
    textarea.value = state.dirty.has(key) ? state.dirty.get(key) : doc.content;
    textarea.spellcheck = false;
    if (tpl.generated) textarea.readOnly = true;
    textarea.addEventListener("input", () => {
      if (tpl.generated) return;
      const text = textarea.value;
      if (text === doc.content) state.dirty.delete(key); else state.dirty.set(key, text);
      card.classList.toggle("dirty", state.dirty.has(key));
      updateSaveButton();
      updatePreviewLocally();
    });
    body.append(textarea);

    if (!tpl.generated) {
      const actions = el("div", "tpl-actions");
      const reset = el("button", "btn tiny ghost", T("reset"));
      reset.disabled = tpl.origin !== "user";
      reset.addEventListener("click", async () => {
        try {
          const out = await api("/api/templates/reset", { method: "POST", body: JSON.stringify({ key }) });
          applyDoc(out.template);
          state.dirty.delete(key);
          toast(T("reloadOk"));
          renderTemplates();
          updatePreviewLocally();
        } catch (e) { toast(e.message, true); }
      });
      actions.append(reset);
      body.append(actions);
    }
    card.append(body);
    list.append(card);
  }
}

function renderVariables() {
  const list = $("#variable-list");
  list.replaceChildren();
  for (const v of state.variables) {
    const row = el("div", "var-row");
    row.dataset.var = v.name;
    row.append(el("label", "", `{{${v.name}}}`));
    row.append(el("div", "var-desc", v.description || ""));
    const input = el("input");
    input.type = "text";
    input.value = v.value || "";
    input.placeholder = v.origin === "generated" ? "(auto)" : "";
    if (v.origin === "generated") input.readOnly = true;
    input.addEventListener("change", async () => {
      state.varsValues[v.name] = input.value;
      try {
        const out = await api("/api/vars", { method: "POST", body: JSON.stringify({ values: { [v.name]: input.value } }) });
        state.variables = out.variables;
        toast(T("varsOk"));
        updatePreviewLocally();
      } catch (e) { toast(e.message, true); }
    });
    row.append(input);
    list.append(row);
  }
}

function renderPresets() {
  const select = $("#preset-select");
  select.replaceChildren();
  for (const p of state.presets) {
    const opt = el("option", "", p.title);
    opt.value = p.id;
    select.append(opt);
  }
  if (state.activePreset) select.value = state.activePreset;
  select.onchange = async () => {
    try {
      await api("/api/role/apply", { method: "POST", body: JSON.stringify({ id: select.value }) });
      toast(T("presetOk"));
      await load();
    } catch (e) { toast(e.message, true); }
  };
}

function applyDoc(doc) {
  state.docs.set(doc.key, { content: doc.content, revision: doc.revision });
  const tpl = state.templates.find((t) => t.key === doc.key);
  if (tpl) { tpl.content = doc.content; tpl.revision = doc.revision; tpl.origin = doc.origin; }
}

function updateSaveButton() {
  $("#save-btn").textContent = state.dirty.size ? `${T("saveAll")} (${state.dirty.size})` : T("saveAll");
}

function renderPreview(text) { $("#prompt-preview").textContent = text || ""; }

function updatePreviewLocally() {
  // Reflect unsaved edits immediately by overlaying the server-rendered prefix.
  const values = { ...state.varsValues };
  for (const v of state.variables) if (!(v.name in values)) values[v.name] = v.value;
  const parts = [];
  for (const tpl of state.templates) {
    let text = state.dirty.has(tpl.key) ? state.dirty.get(tpl.key) : tpl.content;
    if (typeof text !== "string") continue;
    text = text.replace(/\{\{\s*([\w.\-]+)\s*(?:\|([^}]*))?\}\}/g, (m, name, fb) => {
      const val = values[name];
      if (val !== undefined && val !== null && val !== "") return String(val);
      if (fb && fb.trim()) return fb.trim();
      return m;
    });
    if (text.trim()) parts.push(text);
  }
  renderPreview(parts.join("\n\n"));
}

/* ---------- actions ---------- */
async function saveAll() {
  if (state.saving || state.dirty.size === 0) return;
  state.saving = true;
  updateSaveButton();
  toast(T("saving"));
  let handledConflict = false;
  try {
    for (const [key, content] of [...state.dirty]) {
      const doc = state.docs.get(key);
      try {
        const out = await api("/api/templates", {
          method: "POST",
          body: JSON.stringify({ key, content, baseRevision: doc ? doc.revision : null }),
        });
        applyDoc(out.template);
        if (state.dirty.get(key) === content) state.dirty.delete(key);
      } catch (e) {
        if (e.status === 409 && !handledConflict) { handledConflict = true; toast(T("conflict"), true); }
        else toast(e.message, true);
      }
    }
  } finally {
    state.saving = false;
    renderTemplates();
    updateSaveButton();
    await refreshPrefix();
  }
}

async function refreshPrefix() {
  try {
    const data = await api("/api/templates/prefix");
    renderPreview(data.prompt);
  } catch (e) { /* keep local preview */ }
}

async function load() {
  try {
    const [tplData, varData, presetData] = await Promise.all([
      api("/api/templates"), api("/api/vars"), api("/api/presets"),
    ]);
    state.templates = tplData.templates;
    state.docs.clear();
    state.dirty.clear();
    for (const t of state.templates) state.docs.set(t.key, { content: t.content, revision: t.revision });
    state.variables = varData.variables;
    state.varsValues = {};
    for (const v of state.variables) state.varsValues[v.name] = v.value;
    state.presets = presetData.presets;
    state.activePreset = presetData.active;

    renderTemplates();
    renderVariables();
    renderPresets();
    updateSaveButton();
    await refreshPrefix();
  } catch (e) {
    toast(`${T("loadFail")}: ${e.message}`, true);
  }
}

/* ---------- wiring ---------- */
applyI18n();
$("#save-btn").addEventListener("click", saveAll);
$("#reload-btn").addEventListener("click", async () => { await load(); toast(T("reloadOk")); });
$("#copy-btn").addEventListener("click", async () => {
  const text = $("#prompt-preview").textContent || "";
  try { await navigator.clipboard.writeText(text); toast(T("copied")); }
  catch { toast(T("copyFail"), true); }
});
document.addEventListener("keydown", (e) => {
  if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === "s") { e.preventDefault(); saveAll(); }
});
load();
