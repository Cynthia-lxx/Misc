/* OI Code Console — shell controller: theme/motion, log, routing, history,
   settings, global shortcuts. The IDE itself lives in ide.js (editor, run,
   debug, generation), overlays in overlays.js, debug dock in debug.js. */

const $ = (sel) => document.querySelector(sel);
const $$ = (sel) => Array.from(document.querySelectorAll(sel));
const byId = (id) => document.getElementById(id);

import { applyDom, getLang, setLang, t } from './i18n.js';

const THEMES = ['red', 'orange', 'lime', 'green', 'aquamarine', 'blue', 'purple', 'plum', 'pink'];
const THEME_HUE = { red: 0, orange: 45, lime: 90, green: 125, aquamarine: 160, blue: 200, purple: 255, plum: 320, pink: 333 };
const VIEW_META = {
  ide: ['IDE', '以代码为中心：编辑、运行、调试与关键指标同屏'],
  analysis: ['分析报告', '内存 / 复杂度 / 风格冗余 / 性能四类体检，纯本地分析 + 一次短 AI 汇总'],
  history: ['会话历史', '服务端持久化的完整对话，与 CLI 的 /show 同源'],
  settings: ['设置', 'API 与生成参数写入 ~/.oicoder_config.json，CLI 同步生效'],
  cheatsheet: ['命令速查', '原 OI.py 命令在控制台中的落点，以及快捷键'],
};

const state = {
  config: {},
  status: {},
  messages: [],
  logCount: 0,
  clearArmed: false,
  pulseTimer: null,
  codeLoaded: false,
};

/* ------------------------------------------------------------ utilities */
async function api(path, options = {}) {
  const res = await fetch(path, {
    headers: { 'Content-Type': 'application/json' },
    ...options,
  });
  let data;
  try {
    data = await res.json();
  } catch {
    throw new Error(`HTTP ${res.status}：响应不是 JSON`);
  }
  if (!res.ok || data.ok === false) throw new Error(data.error || `HTTP ${res.status}`);
  return data;
}

function escapeHtml(s) {
  return s.replace(/[&<>"']/g, (c) => ({ '&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;' })[c]);
}

function log(message, level = '') {
  const li = document.createElement('li');
  li.className = level;
  li.textContent = `[${new Date().toLocaleTimeString('zh-CN', { hour12: false })}] ${message}`;
  byId('log-list').appendChild(li);
  const list = byId('log-list');
  list.scrollTop = list.scrollHeight;
  state.logCount += 1;
  byId('log-btn').setAttribute('label', t('日志 {n}', { n: state.logCount }));
  setStatus(message);
}

function setStatus(message, kind = '') {
  const el = byId('status-line');
  if (!el) return;
  el.textContent = message;
  el.className = `oc-status-line ${kind}`;
}

function pulse(text, kind = 'success') {
  const host = byId('pulse-host');
  const el = byId('pulse-text');
  el.textContent = text;
  const extra = kind === 'error' ? ' ui-status-pulse--error'
    : kind === 'warning' ? ' ui-status-pulse--warning'
      : kind === 'info' ? ' ui-status-pulse--success' : '';
  el.className = `ui-status-pulse${extra}`;
  host.dataset.active = 'false';
  void host.offsetWidth;
  host.dataset.active = 'true';
  clearTimeout(state.pulseTimer);
  state.pulseTimer = setTimeout(() => { host.dataset.active = 'false'; }, 900);
}

function warn(message) {
  setStatus(message, 'error');
  log(message, 'warn');
  pulse('!', 'warning');
}

/* ------------------------------------------------------------ theme / motion */
function setTheme(name, quiet = false) {
  if (!THEMES.includes(name)) name = 'blue';
  document.documentElement.dataset.theme = name;
  localStorage.setItem('oi-theme', name);
  byId('theme-select').value = name;
  $$('.oc-swatch').forEach((b) => b.classList.toggle('is-active', b.dataset.theme === name));
  $$('.oc-theme-dot').forEach((d) => d.classList.toggle('active', d.dataset.theme === name));
  if (!quiet) log(`theme → ${name}`);
}

function setReducedMotion(on, quiet = false) {
  state.reduceMotion = on;
  document.documentElement.dataset.reduceMotion = on ? 'true' : 'false';
  localStorage.setItem('oi-reduce', on ? '1' : '0');
  const label = t(on ? '动效: 关' : '动效: 开');
  byId('reduce-btn').setAttribute('label', label);
  byId('theme-reduce').setAttribute('label', label);
  if (!quiet) log(`reduced motion → ${on ? 'on' : 'off'}`);
}

function initSwatches() {
  const wrap = byId('theme-swatches');
  THEMES.forEach((name) => {
    const btn = document.createElement('button');
    btn.type = 'button';
    btn.className = 'oc-swatch';
    btn.dataset.theme = name;
    const bar = document.createElement('i');
    bar.style.background = `linear-gradient(135deg, hsl(${THEME_HUE[name]} 100% 70%), hsl(${THEME_HUE[name]} 60% 40%))`;
    const caption = document.createElement('span');
    caption.textContent = name;
    btn.append(bar, caption);
    btn.addEventListener('click', () => setTheme(name));
    wrap.appendChild(btn);
  });
}

/* ------------------------------------------------------------ routing */
function setView(name) {
  if (!VIEW_META[name]) name = 'ide';
  $$('.oc-nav a').forEach((a) => a.classList.toggle('is-active', a.dataset.view === name));
  $$('.oc-view-panel').forEach((p) => p.classList.toggle('is-hidden', p.dataset.panel !== name));
  byId('view-title').textContent = t(VIEW_META[name][0]);
  byId('view-desc').textContent = t(VIEW_META[name][1]);
  byId('foot-right').textContent = `${state.config.model || 'deepseek-chat'} · ${state.messages.length} msgs · hash-route`;
  if (name === 'history') loadHistory();
  if (name === 'settings') fillConfigForm();
}

/* ------------------------------------------------------------ C++ highlighting (history preview) */
const CPP_KEYWORDS = new Set(('alignas alignof and and_eq asm auto bitand bitor bool break case catch char16_t char32_t class compl const constexpr const_cast continue decltype default delete do double dynamic_cast else enum explicit export extern false float for friend goto if inline int long mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected public register reinterpret_cast return short signed sizeof static static_assert static_cast struct switch template this thread_local throw true try typedef typeid typename union unsigned using virtual void volatile wchar_t while xor xor_eq std').split(' '));
const CPP_TYPES = new Set(('size_t ptrdiff_t string vector map set queue stack deque pair tuple array bitset unique_ptr shared_ptr priority_queue unordered_map unordered_set istream ostream ifstream ofstream stringstream int8_t int16_t int32_t int64_t uint8_t uint16_t uint32_t uint64_t').split(' '));
const TOKEN_RE = /(\/\/[^\n]*|\/\*[\s\S]*?\*\/)|("(?:\\.|[^"\\])*"|'(?:\\.|[^'\\])*')|(#[A-Za-z_]+)|\b(\d+(?:\.\d+)?(?:[eE][+-]?\d+)?[A-Za-z]*)\b|\b([A-Za-z_]\w*)\b|([{}()[\];,.<>+\-*/%=!&|^~?:]+)/g;

function highlightCpp(src) {
  let out = '';
  let last = 0;
  let match;
  TOKEN_RE.lastIndex = 0;
  while ((match = TOKEN_RE.exec(src)) !== null) {
    out += escapeHtml(src.slice(last, match.index));
    const [full, comment, string, pre, num, word] = match;
    let cls = '';
    if (comment) cls = 't-com';
    else if (string) cls = 't-str';
    else if (pre) cls = 't-pre';
    else if (num) cls = 't-num';
    else if (word) {
      if (CPP_KEYWORDS.has(word)) cls = 't-kw';
      else if (CPP_TYPES.has(word)) cls = 't-ty';
      else if (src[match.index + full.length] === '(') cls = 't-fn';
    } else cls = 't-op';
    out += cls ? `<span class="${cls}">${escapeHtml(full)}</span>` : escapeHtml(full);
    last = match.index + full.length;
  }
  out += escapeHtml(src.slice(last));
  return out;
}

function extractCode(text) {
  if (!text) return null;
  const fenced = text.match(/```(?:cpp|c\+\+|C\+\+)\s*\n([\s\S]*?)(?:\n```|$)/);
  if (fenced) return fenced[1].replace(/\n```[\s\S]*$/, '');
  const plain = text.match(/```\s*\n([\s\S]*?)(?:\n```|$)/);
  if (plain && /#include|using|int\s+main/.test(plain[1])) return plain[1];
  const idx = text.indexOf('#include');
  if (idx >= 0) return text.slice(idx).replace(/\n```[\s\S]*$/, '');
  return null;
}

/* ------------------------------------------------------------ data */
async function refreshStatus() {
  const data = await api('/api/status');
  state.status = data;
  state.config = data.config;
  byId('api-dot').className = `oc-dot ${data.config.api_key_set ? 'ok' : 'warn'}`;
  byId('api-status').textContent = data.config.api_key_set ? t('API Key 已配置') : t('API Key 未设置');
  byId('model-status').textContent = `model: ${data.config.model || '—'}`;
  byId('history-status').textContent = t('history: {n} 条', { n: data.history_count });
  byId('foot-right').textContent = `${data.config.model || 'deepseek-chat'} · ${data.history_count} msgs · hash-route`;
  if (data.last_code && !state.codeLoaded) {
    state.codeLoaded = true;
    window.dispatchEvent(new CustomEvent('oi-load-code', { detail: { code: extractCode(data.last_code) || '' } }));
    log(t('已载入上次生成的代码'), 'ok');
  }
  fillConfigForm();
  return data;
}

function fillConfigForm() {
  const c = state.config || {};
  byId('cfg-base-url').value = c.base_url || '';
  byId('cfg-model').value = c.model || '';
  byId('cfg-max-tokens').value = c.max_tokens ?? 8192;
  byId('cfg-temperature').value = c.temperature ?? 0.3;
  byId('cfg-api-key').value = '';

  const table = byId('config-table');
  table.replaceChildren();
  Object.entries(c).forEach(([key, value]) => {
    const dt = document.createElement('dt');
    const dd = document.createElement('dd');
    dt.textContent = key;
    dd.textContent = String(value);
    table.append(dt, dd);
  });
}

function messageCard(msg, index, full = false) {
  const el = document.createElement('div');
  el.className = `oc-msg ${msg.role === 'assistant' ? 'assistant' : 'user'}`;
  const head = document.createElement('div');
  head.className = 'oc-msg-head';
  const who = document.createElement('b');
  who.textContent = msg.role === 'assistant' ? 'Agent' : 'User';
  const num = document.createElement('span');
  num.textContent = `#${index + 1}`;
  head.append(who, num);
  const body = document.createElement('p');
  body.className = 'oc-msg-body';
  body.textContent = msg.role === 'assistant' ? (extractCode(msg.content) || msg.content) : msg.content;
  el.append(head, body);
  if (full && msg.role === 'assistant') {
    const row = document.createElement('div');
    row.className = 'oc-row';
    const btn = document.createElement('button');
    btn.type = 'button';
    btn.className = 'oc-btn';
    btn.textContent = t('载入到编辑器');
    btn.addEventListener('click', () => {
      setView('ide');
      location.hash = 'ide';
      const code = extractCode(msg.content) || '';
      window.dispatchEvent(new CustomEvent('oi-load-code', { detail: { code } }));
      log(t('载入历史消息 #{n} → 编辑器 ({m} 行)', { n: index + 1, m: code.split('\n').length }), 'ok');
      pulse('LOADED', 'info');
    });
    row.appendChild(btn);
    el.appendChild(row);
  }
  return el;
}

async function loadHistory() {
  const data = await api('/api/history');
  state.messages = data.messages;
  const list = byId('history-list');
  list.replaceChildren();
  if (!data.messages.length) {
    const empty = document.createElement('div');
    empty.className = 'oc-empty';
    empty.textContent = t('暂无会话历史。');
    list.appendChild(empty);
  } else {
    data.messages.forEach((msg, i) => list.appendChild(messageCard(msg, i, true)));
  }
  byId('history-status').textContent = t('history: {n} 条', { n: data.messages.length });
}

/* ------------------------------------------------------------ settings */
async function saveConfig(event) {
  if (event) event.preventDefault();
  const body = {
    api_key: byId('cfg-api-key').value.trim(),
    base_url: byId('cfg-base-url').value.trim(),
    model: byId('cfg-model').value.trim(),
    max_tokens: Number(byId('cfg-max-tokens').value),
    temperature: Number(byId('cfg-temperature').value),
  };
  try {
    const data = await api('/api/config', { method: 'POST', body: JSON.stringify(body) });
    state.config = data.config;
    fillConfigForm();
    await refreshStatus();
    setStatus(t('设置已保存'), 'ok');
    log(t('配置已写入 ~/.oicoder_config.json'), 'ok');
    pulse('SAVED');
  } catch (err) { warn(t('保存设置失败: {msg}', { msg: err.message })); }
}

async function clearHistory() {
  const btn = byId('history-clear');
  if (!state.clearArmed) {
    state.clearArmed = true;
    btn.setAttribute('label', t('再点一次确认'));
    setTimeout(() => {
      state.clearArmed = false;
      btn.setAttribute('label', t('清空历史'));
    }, 3500);
    log(t('清空历史：等待二次确认'), 'warn');
    return;
  }
  state.clearArmed = false;
  btn.setAttribute('label', t('清空历史'));
  await api('/api/history', { method: 'DELETE' });
  await Promise.allSettled([loadHistory(), refreshStatus()]);
  window.dispatchEvent(new CustomEvent('oi-load-code', { detail: { code: '' } }));
  setStatus(t('会话历史已清空'), 'ok');
  log(t('会话历史已清空'), 'ok');
  pulse('CLEARED', 'warning');
}

/* ------------------------------------------------------------ wiring */
function bind(id, handler) {
  const el = byId(id);
  if (el) el.addEventListener('click', handler);
}

/* ---- 侧栏底部：主题色点 + 语言切换（与 NTLab 同款） ---- */
function initSideControls() {
  const dots = byId('theme-dots');
  const langBtn = byId('lang-btn');
  if (!dots || !langBtn) return;
  THEMES.forEach((name) => {
    const dot = document.createElement('button');
    dot.type = 'button';
    dot.className = 'oc-theme-dot';
    dot.dataset.theme = name;
    dot.title = name[0].toUpperCase() + name.slice(1);
    dot.style.background = `linear-gradient(135deg, hsl(${THEME_HUE[name]} 100% 70%), hsl(${THEME_HUE[name]} 60% 40%))`;
    dot.addEventListener('click', () => setTheme(name));
    dots.appendChild(dot);
  });
  const syncLang = () => { langBtn.textContent = getLang() === 'zh' ? 'EN' : '中'; };
  langBtn.addEventListener('click', () => setLang(getLang() === 'zh' ? 'en' : 'zh'));
  document.addEventListener('ui-i18n-change', () => {
    syncLang();
    // 语言切换后：重刷视图标题/描述与动态标签（applyDom 已由 setLang 触发）
    setView(location.hash.slice(1) || 'ide');
    byId('log-btn').setAttribute('label', t('日志 {n}', { n: state.logCount }));
    setReducedMotion(state.reduceMotion === true, true);
  });
  syncLang();
}

/* ---- 服务器设置（端口/浏览器，独立配置文件，重启生效） ---- */
async function loadSrvSettings() {
  const d = await api('/api/app/settings');
  byId('srv-port').value = d.port;
  byId('srv-browser').checked = Boolean(d.open_browser);
}

async function saveSrvSettings(event) {
  if (event) event.preventDefault();
  const body = {
    port: Number(byId('srv-port').value),
    open_browser: byId('srv-browser').checked,
  };
  try {
    await api('/api/app/settings', { method: 'POST', body: JSON.stringify(body) });
    setStatus(t('服务器设置已保存'), 'ok');
    log(t('服务器设置已保存'), 'ok');
    pulse('SAVED');
  } catch (err) {
    warn(t('保存服务器设置失败: {msg}', { msg: err.message }));
  }
}

function init() {
  initSwatches();
  initSideControls();
  setTheme(localStorage.getItem('oi-theme') || 'blue', true);
  setReducedMotion(localStorage.getItem('oi-reduce') === '1', true);

  $$('.oc-nav a').forEach((a) => a.addEventListener('click', (e) => {
    e.preventDefault();
    location.hash = a.dataset.view;
  }));
  window.addEventListener('hashchange', () => setView(location.hash.slice(1)));
  setView(location.hash.slice(1) || 'ide');

  byId('theme-select').addEventListener('change', (e) => setTheme(e.target.value));
  byId('theme-reset').addEventListener('click', () => setTheme('blue'));
  bind('reduce-btn', () => setReducedMotion(document.documentElement.dataset.reduceMotion !== 'true'));
  bind('theme-reduce', () => setReducedMotion(document.documentElement.dataset.reduceMotion !== 'true'));

  const toggleLog = () => byId('log-panel').classList.toggle('is-open');
  bind('log-btn', toggleLog);
  bind('log-close', () => byId('log-panel').classList.remove('is-open'));
  bind('log-clear', () => {
    byId('log-list').replaceChildren();
    state.logCount = 0;
    byId('log-btn').setAttribute('label', t('日志 {n}', { n: 0 }));
  });

  byId('config-form').addEventListener('submit', saveConfig);
  bind('config-save', saveConfig);
  bind('config-reload', async () => { await refreshStatus(); log(t('配置已重新载入')); });
  bind('history-refresh', loadHistory);
  bind('history-clear', clearHistory);

  // ---- 服务器设置（端口 / 自动打开浏览器） ----
  byId('srv-form').addEventListener('submit', saveSrvSettings);
  bind('srv-save', saveSrvSettings);
  bind('srv-reload', async () => {
    await loadSrvSettings();
    log(t('服务器设置已重新载入'));
  });
  loadSrvSettings().catch((err) => log(t('保存服务器设置失败: {msg}', { msg: err.message }), 'warn'));

  // ---- global shortcuts (letters only outside inputs; F-keys always) ----
  window.addEventListener('keydown', (event) => {
    const inInput = /^(INPUT|TEXTAREA|SELECT)$/.test(document.activeElement?.tagName || '')
      || document.activeElement?.classList.contains('CodeMirror');

    if (event.key === 'F5') {
      event.preventDefault();
      window.dispatchEvent(new CustomEvent('oi-run'));
      return;
    }
    if (event.key === 'F9') {
      event.preventDefault();
      window.dispatchEvent(new CustomEvent('oi-debug'));
      return;
    }
    if (event.key === 'F10' || event.key === 'F11') {
      if (document.querySelector('#dock-debug:not(.is-hidden)')) event.preventDefault();
      window.dispatchEvent(new CustomEvent(event.key === 'F10' ? 'oi-dbg-next' : 'oi-dbg-step'));
      return;
    }
    if (event.key === 'Escape') {
      window.dispatchEvent(new CustomEvent('oi-overlay-close'));
      byId('log-panel').classList.remove('is-open');
      return;
    }
    if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 'k') {
      event.preventDefault();
      window.dispatchEvent(new CustomEvent('oi-palette'));
      return;
    }
    if ((event.ctrlKey || event.metaKey) && event.key.toLowerCase() === 'l') {
      event.preventDefault();
      toggleLog();
      return;
    }
    if ((event.ctrlKey || event.metaKey) && event.key === 'Enter') {
      event.preventDefault();
      window.dispatchEvent(new CustomEvent('oi-generate'));
      return;
    }
    if (inInput || event.ctrlKey || event.metaKey || event.altKey) return;
    const viewKey = { h: 'history', a: 'analysis', s: 'settings', '?': 'cheatsheet' }[event.key];
    if (viewKey) {
      event.preventDefault();
      location.hash = viewKey;
    }
  });

  (async () => {
    log(t('OI Code Console 启动（IDE 布局）'), 'ok');
    try {
      await refreshStatus();
      byId('api-dot').classList.add('ok');
    } catch (err) {
      warn(t('服务状态获取失败: {msg}', { msg: err.message }));
      byId('api-dot').classList.add('err');
    }
  })();
}

init();

/* bridge for sibling modules */
window.__oiConsole = {
  state,
  setView,
  log,
  warn,
  pulse,
  setStatus,
  api,
  escapeHtml,
  highlightCpp,
  extractCode,
  byId,
};

