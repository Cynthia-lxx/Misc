/* prompts.js — style-prompt template manager for the settings view.
   Built-ins ("standard", "original") are immutable; user templates can be
   created, edited, activated and deleted. The active template feeds
   /api/generate as the system prompt. */

const byId = (id) => document.getElementById(id);
const S = () => window.__oiConsole;

let templates = [];
let activeId = '';

function current() {
  return templates.find((t) => t.id === byId('prompt-select').value) || null;
}

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
}

async function reload() {
  const data = await S().api('/api/prompts');
  templates = data.templates || [];
  activeId = data.active || '';
  fillSelect();
  fillEditor();
}

async function post(payload) {
  return S().api('/api/prompts', { method: 'POST', body: JSON.stringify(payload) });
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
  await post({ action: 'update', id: t.id, name: byId('prompt-name').value.trim(), text: byId('prompt-text').value });
  await reload();
  S().setStatus('模板已保存', 'ok');
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

function init() {
  byId('prompt-select').addEventListener('change', fillEditor);
  byId('prompt-activate').addEventListener('click', () => onActivate().catch((e) => S().warn(e.message)));
  byId('prompt-save').addEventListener('click', () => onSave().catch((e) => S().warn(e.message)));
  byId('prompt-new').addEventListener('click', () => onNew().catch((e) => S().warn(e.message)));
  byId('prompt-delete').addEventListener('click', () => onDelete().catch((e) => S().warn(e.message)));
  reload().catch((e) => S().warn(`提示词模板载入失败: ${e.message}`));
}

if (window.__oiConsole) init();
else document.addEventListener('DOMContentLoaded', () => setTimeout(() => window.__oiConsole && init(), 0));
