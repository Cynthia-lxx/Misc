/* prompts.js — style-prompt template manager for the settings view.

   Borrowed from Persona_Manager: {{name | fallback}} interpolation with
   missing-token visibility, content-hash revision for optimistic concurrency,
   and a live preview that renders even unsaved (dirty) editor text. Variable
   values are persisted server-side (store var_values) and merged on save.
   Built-ins ("standard", "original") remain immutable — recoverability is
   guaranteed by design, so no override/reset layer is needed. */

const byId = (id) => document.getElementById(id);
const S = () => window.__oiConsole;

const TOKEN_RE = /\{\{\s*([A-Za-z_][A-Za-z0-9_]*)\s*(?:\|\s*([\s\S]*?)\s*)?\}\}/g;

let templates = [];
let activeId = '';
let varValues = {};   // server-side persisted values
let saveTimer = null;

function current() {
  return templates.find((t) => t.id === byId('prompt-select').value) || null;
}

/* Client mirror of prompt_store.render_text — same semantics, zero latency. */
function renderText(text, values) {
  const missing = [];
  const rendered = (text || '').replace(TOKEN_RE, (raw, name, fallback) => {
    const value = values[name];
    if (value === undefined || value === '') {
      if (fallback !== undefined && fallback !== '') return fallback;
      missing.push(name);
      return raw;
    }
    return value;
  });
  return { rendered, missing };
}

function declaredVars(t) {
  const out = [];
  const seen = new Set();
  (t.vars || []).forEach((v) => {
    const name = typeof v === 'string' ? v : v.name;
    if (name && !seen.has(name)) {
      seen.add(name);
      out.push({ name, desc: (typeof v === 'object' && v.desc) || '' });
    }
  });
  (t.text || '').replace(TOKEN_RE, (raw, name) => {
    if (!seen.has(name)) { seen.add(name); out.push({ name, desc: '' }); }
    return raw;
  });
  return out;
}

/* ------------------------------------------------------------ rendering UI */
function fillSelect() {
  const sel = byId('prompt-select');
  sel.replaceChildren();
  templates.forEach((t) => {
    const opt = document.createElement('option');
    opt.value = t.id;
    opt.textContent = (t.builtin ? '★ ' : '') + t.name;
    sel.appendChild(opt);
  });
  sel.value = activeId;
}

function buildVarPanel(t) {
  const wrap = byId('prompt-vars');
  wrap.replaceChildren();
  const vars = declaredVars(t);
  if (!vars.length) {
    const hint = document.createElement('span');
    hint.className = 'an-limit-hint';
    hint.textContent = '此模板没有变量';
    wrap.appendChild(hint);
    return;
  }
  vars.forEach((v) => {
    const label = document.createElement('label');
    label.className = 'prompt-var';
    const name = document.createElement('code');
    name.textContent = v.name;
    name.title = v.desc || `变量 ${v.name}`;
    const input = document.createElement('input');
    input.type = 'text';
    input.value = varValues[v.name] || '';
    input.placeholder = v.desc || '留空 = 内联默认值';
    input.dataset.var = v.name;
    input.addEventListener('input', () => {
      varValues[v.name] = input.value;   // local for live preview
      renderPreview();
      scheduleSaveVars();
    });
    label.append(name, input);
    wrap.appendChild(label);
  });
}

function renderPreview() {
  const t = current();
  if (!t) return;
  const { rendered, missing } = renderText(byId('prompt-text').value, varValues);
  const pre = byId('prompt-preview');
  pre.replaceChildren();
  // highlight unresolved {{tokens}} in the rendered output
  const parts = rendered.split(/(\{\{\s*[A-Za-z_][A-Za-z0-9_]*\s*(?:\|[\s\S]*?)?\}\})/g);
  for (const part of parts) {
    if (/^\{\{/.test(part)) {
      const mark = document.createElement('mark');
      mark.textContent = part;
      pre.appendChild(mark);
    } else if (part) {
      pre.appendChild(document.createTextNode(part));
    }
  }
  const note = byId('prompt-preview-note');
  if (missing.length) {
    note.textContent = `缺失变量：${[...new Set(missing)].join(', ')}（生成时将原样保留）`;
    note.style.color = 'var(--state-warning, #ffcc22)';
  } else {
    note.textContent = '全部变量已解析';
    note.style.color = '';
  }
}

function fillEditor() {
  const t = current();
  const text = byId('prompt-text');
  const name = byId('prompt-name');
  const hint = byId('prompt-active-hint');
  if (!t) return;
  text.value = t.text || '';
  text.readOnly = Boolean(t.builtin);
  name.value = t.builtin ? '' : t.name;
  name.readOnly = Boolean(t.builtin);
  const isActive = t.id === activeId;
  hint.textContent = t.builtin
    ? '内置模板不可编辑；可「另存为新模板」后修改副本'
    : `自定义模板${isActive ? ' · 当前激活' : ''}`;
  buildVarPanel(t);
  renderPreview();
}

/* ------------------------------------------------------------ data flow */
async function reload() {
  const data = await S().api('/api/prompts');
  templates = data.templates || [];
  activeId = data.active || '';
  varValues = data.var_values || {};
  fillSelect();
  fillEditor();
}

async function post(payload) {
  return S().api('/api/prompts', { method: 'POST', body: JSON.stringify(payload) });
}

function scheduleSaveVars() {
  clearTimeout(saveTimer);
  saveTimer = setTimeout(async () => {
    try {
      const data = await post({ action: 'save_vars', values: varValues });
      varValues = data.var_values || varValues;
    } catch (err) {
      S().warn(`变量保存失败: ${err.message}`);
    }
  }, 400);
}

async function onActivate() {
  const t = current();
  if (!t) return;
  await post({ action: 'activate', id: t.id });
  await reload();
  S().setStatus(`已激活模板「${t.name}」`, 'ok');
  S().log(`提示词模板切换 → ${t.name}（新生成即生效）`, 'ok');
}

async function onSave() {
  const t = current();
  if (!t || t.builtin) return;
  try {
    const data = await post({
      action: 'update',
      id: t.id,
      name: byId('prompt-name').value.trim(),
      text: byId('prompt-text').value,
      base_revision: t.revision,
    });
    await reload();
    S().setStatus('模板已保存', 'ok');
    if (data && data.revision) { /* revision refreshed via reload */ }
  } catch (err) {
    if (String(err.message).includes('409') || String(err.message).includes('冲突')) {
      S().warn('保存冲突：模板已被其他窗口修改。已为你重新载入最新内容，请合并后再保存。');
      await reload();
    } else {
      S().warn(`保存失败: ${err.message}`);
    }
  }
}

async function onNew() {
  const text = byId('prompt-text').value.trim();
  if (!text) {
    S().warn('模板内容为空');
    return;
  }
  const name = byId('prompt-name').value.trim() || `自定义 ${new Date().toLocaleString()}`;
  await post({ action: 'save_new', name, text });
  await reload();
  S().setStatus(`已另存并激活「${name}」`, 'ok');
  S().log(`新提示词模板「${name}」已保存并激活`, 'ok');
}

async function onDelete() {
  const t = current();
  if (!t || t.builtin) return;
  await post({ action: 'delete', id: t.id });
  await reload();
  S().log(`模板「${t.name}」已删除`, 'ok');
}

async function onCopy() {
  const { rendered } = renderText(byId('prompt-text').value, varValues);
  try {
    await navigator.clipboard.writeText(rendered);
    S().setStatus('渲染结果已复制到剪贴板', 'ok');
  } catch {
    S().warn('剪贴板不可用');
  }
}

/* ------------------------------------------------------------ wiring */
function init() {
  byId('prompt-select').addEventListener('change', fillEditor);
  byId('prompt-text').addEventListener('input', renderPreview);
  byId('prompt-activate').addEventListener('click', () => onActivate().catch((e) => S().warn(e.message)));
  byId('prompt-save').addEventListener('click', () => onSave().catch((e) => S().warn(e.message)));
  byId('prompt-new').addEventListener('click', () => onNew().catch((e) => S().warn(e.message)));
  byId('prompt-delete').addEventListener('click', () => onDelete().catch((e) => S().warn(e.message)));
  byId('prompt-copy').addEventListener('click', () => onCopy().catch((e) => S().warn(e.message)));
  reload().catch((e) => S().warn(`提示词模板载入失败: ${e.message}`));
}

if (window.__oiConsole) init();
else document.addEventListener('DOMContentLoaded', () => setTimeout(() => window.__oiConsole && init(), 0));
