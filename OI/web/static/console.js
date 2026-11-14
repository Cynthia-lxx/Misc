/* OI Code Console — front-end controller. No framework, no build step. */

const $ = (sel) => document.querySelector(sel);
const $$ = (sel) => Array.from(document.querySelectorAll(sel));
const byId = (id) => document.getElementById(id);

const THEMES = ['red', 'orange', 'lime', 'green', 'aquamarine', 'blue', 'purple', 'plum', 'pink'];
const THEME_HUE = { red: 0, orange: 45, lime: 90, green: 125, aquamarine: 160, blue: 200, purple: 255, plum: 320, pink: 333 };
const VIEW_META = {
  workbench: ['工作台', '粘贴题面或输入修改要求，生成符合个人风格的 C++14 代码'],
  history: ['会话历史', '服务端持久化的完整对话，与 CLI 的 /show 同源'],
  settings: ['设置', 'API 与生成参数写入 ~/.oicoder_config.json，CLI 同步生效'],
  cheatsheet: ['命令速查', '原 OI.py 命令在控制台中的落点，以及快捷键'],
};
const SAMPLE = `【题目】最大子段和
给定一个长度为 n 的整数序列 a，求一个连续子段，使其元素之和最大。

【输入】
第一行一个整数 n (1 <= n <= 100000)。
第二行 n 个整数 ai (-10000 <= ai <= 10000)。

【输出】
一行一个整数，表示最大子段和。

【样例输入】
5
1 -2 3 4 -1

【样例输出】
7`;

const state = {
  config: {},
  status: {},
  messages: [],
  raw: '',
  code: '',
  tab: 'code',
  generating: false,
  controller: null,
  startedAt: 0,
  logCount: 0,
  clearArmed: false,
  pulseTimer: null,
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
  byId('log-btn').setAttribute('label', `日志 ${state.logCount}`);
  byId('foot-left').textContent = message;
}

function setStatus(message, kind = '') {
  const el = byId('status-line');
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

function flash() {
  const layer = document.createElement('div');
  layer.className = 'ui-additive-flash';
  byId('output-card').appendChild(layer);
  setTimeout(() => layer.remove(), 500);
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
  if (!quiet) log(`theme → ${name}`);
}

function setReducedMotion(on, quiet = false) {
  document.documentElement.dataset.reduceMotion = on ? 'true' : 'false';
  localStorage.setItem('oi-reduce', on ? '1' : '0');
  const label = `动效: ${on ? '关' : '开'}`;
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
  if (!VIEW_META[name]) name = 'workbench';
  $$('.oc-nav a').forEach((a) => a.classList.toggle('is-active', a.dataset.view === name));
  $$('.oc-view-panel').forEach((p) => p.classList.toggle('is-hidden', p.dataset.panel !== name));
  byId('view-title').textContent = VIEW_META[name][0];
  byId('view-desc').textContent = VIEW_META[name][1];
  byId('foot-right').textContent = `${state.config.model || 'deepseek-chat'} · ${state.messages.length} msgs · hash-route`;
  if (name === 'history') loadHistory();
  if (name === 'settings') fillConfigForm();
}

/* ------------------------------------------------------------ C++ highlighting */
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

function renderCode(code) {
  const view = byId('code-view');
  if (!code) {
    view.classList.add('is-hidden');
    view.innerHTML = '';
    return;
  }
  view.innerHTML = code.split('\n').map((line, i) =>
    `<span class="oc-line"><i class="oc-ln">${i + 1}</i><span class="oc-lc">${highlightCpp(line) || ' '}</span></span>`
  ).join('');
  if (state.generating) view.scrollTop = view.scrollHeight;
}

function setTab(tab) {
  state.tab = tab;
  $$('.oc-tabs button').forEach((b) => b.classList.toggle('is-active', b.dataset.tab === tab));
  byId('code-view').classList.toggle('is-hidden', tab !== 'code' || !state.code);
  byId('code-empty').classList.toggle('is-hidden', tab !== 'code' || !!state.code);
  byId('raw-view').classList.toggle('is-hidden', tab !== 'raw');
  byId('rules-view').classList.toggle('is-hidden', tab !== 'rules');
}

function renderOutput(text) {
  state.raw = text || '';
  state.code = extractCode(state.raw) || '';
  renderCode(state.code);
  byId('raw-view').textContent = state.raw || '（暂无响应）';
  byId('meta-chars').textContent = String(state.raw.length);
  byId('meta-lines').textContent = state.code ? String(state.code.split('\n').length) : '—';
  const badge = byId('code-badge');
  badge.className = 'oc-badge ' + (state.code ? 'ok' : state.raw ? 'info' : '');
  badge.textContent = state.code ? `已提取 ${state.code.split('\n').length} 行` : state.raw ? '纯文本响应' : '无代码';
  setTab(state.tab);
}

/* ------------------------------------------------------------ generation */
function setWorking(on) {
  byId('output-card').classList.toggle('is-working', on);
  byId('generate-btn').toggleAttribute('disabled', on);
  byId('stop-btn').toggleAttribute('disabled', !on);
  byId('generate-btn').setAttribute('label', on ? '生成中…' : '生成代码');
  byId('output-sub').textContent = on ? '模型正在输出，代码会实时出现在这里…' : '最近的生成结果';
}

function elapsed() {
  if (!state.startedAt) return '—';
  return `${((performance.now() - state.startedAt) / 1000).toFixed(1)} s`;
}

async function generate() {
  if (state.generating) return;
  const input = byId('prompt').value.trim();
  if (!input) { warn('请先输入题面或修改要求'); byId('prompt').focus(); return; }

  const newTopic = byId('new-topic').checked;
  const temperature = Number(byId('temperature').value);
  state.controller = new AbortController();
  state.generating = true;
  state.startedAt = performance.now();
  state.raw = '';
  state.code = '';
  renderOutput('');
  setTab('code');
  setWorking(true);
  setStatus('正在连接模型…');
  log(`generate → ${state.config.model || 'deepseek-chat'} · ${newTopic ? '新话题' : '延续上下文'} · T=${temperature}`);

  let accumulated = '';
  let raf = 0;
  const flush = () => { raf = 0; renderOutput(accumulated); };

  try {
    const res = await fetch('/api/generate', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ input, new_topic: newTopic, temperature }),
      signal: state.controller.signal,
    });
    if (!res.ok) {
      const detail = await res.json().catch(() => ({}));
      throw new Error(detail.error || `HTTP ${res.status}`);
    }
    const reader = res.body.getReader();
    const decoder = new TextDecoder();
    let buffer = '';

    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      buffer += decoder.decode(value, { stream: true });
      let end;
      while ((end = buffer.indexOf('\n\n')) !== -1) {
        const frame = buffer.slice(0, end);
        buffer = buffer.slice(end + 2);
        const line = frame.split('\n').find((l) => l.startsWith('data: '));
        if (!line) continue;
        let evt;
        try { evt = JSON.parse(line.slice(6)); } catch { continue; }
        if (evt.type === 'start') {
          byId('meta-model').textContent = evt.model;
          setStatus(`已连接 ${evt.model}，等待首个 token…`);
        } else if (evt.type === 'delta') {
          accumulated += evt.text;
          if (!raf) raf = requestAnimationFrame(flush);
        } else if (evt.type === 'error') {
          throw new Error(evt.message);
        } else if (evt.type === 'done') {
          accumulated = evt.content || accumulated;
        }
      }
    }
    if (raf) cancelAnimationFrame(raf);
    renderOutput(accumulated);
    byId('meta-time').textContent = elapsed();
    if (state.code) {
      setStatus(`完成 · ${state.code.split('\n').length} 行代码 · ${elapsed()}`, 'ok');
      log(`生成完成 · ${state.code.split('\n').length} 行 · ${accumulated.length} 字符`, 'ok');
      pulse('DONE');
      flash();
    } else {
      setStatus('完成，但未检测到代码（见「原始响应」）', 'ok');
      log('生成完成 · 纯文本响应', 'ok');
      pulse('TEXT', 'info');
    }
  } catch (err) {
    byId('meta-time').textContent = elapsed();
    if (err.name === 'AbortError') {
      setStatus('已停止生成', 'error');
      renderOutput(accumulated);
      log('生成已中止', 'warn');
      pulse('STOP', 'warning');
    } else {
      setStatus(`错误: ${err.message}`, 'error');
      log(`生成失败: ${err.message}`, 'error');
      pulse('ERROR', 'error');
    }
  } finally {
    state.generating = false;
    state.controller = null;
    setWorking(false);
    await Promise.allSettled([loadHistory(), refreshStatus()]);
  }
}

/* ------------------------------------------------------------ data */
async function refreshStatus() {
  const data = await api('/api/status');
  state.status = data;
  state.config = data.config;
  byId('api-dot').className = `oc-dot ${data.config.api_key_set ? 'ok' : 'warn'}`;
  byId('api-status').textContent = data.config.api_key_set ? 'API Key 已配置' : 'API Key 未设置';
  byId('model-status').textContent = `model: ${data.config.model || '—'}`;
  byId('history-status').textContent = `history: ${data.history_count} 条`;
  byId('meta-workspace').textContent = data.workspace;
  byId('meta-model').textContent = data.config.model || '—';
  byId('rules-view').textContent = data.system_prompt;
  byId('foot-right').textContent = `${data.config.model || 'deepseek-chat'} · ${data.history_count} msgs · hash-route`;
  if (data.last_code && !state.code) {
    renderOutput(data.last_code);
    setStatus('已载入上次生成的代码', 'ok');
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
  const t = Number(c.temperature ?? 0.3);
  byId('temperature').value = String(t);
  byId('temperature-out').textContent = t.toFixed(1);

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
    btn.textContent = '载入到输出';
    btn.addEventListener('click', () => {
      setView('workbench');
      location.hash = 'workbench';
      renderOutput(msg.content);
      byId('output-sub').textContent = `历史消息 #${index + 1}`;
      log(`载入历史消息 #${index + 1}`, 'ok');
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
  const timeline = byId('timeline');
  timeline.replaceChildren();
  const recent = data.messages.slice(-8);
  if (!recent.length) {
    const empty = document.createElement('div');
    empty.className = 'oc-empty';
    empty.textContent = '暂无对话记录，生成一次代码后这里会出现消息。';
    timeline.appendChild(empty);
  } else {
    recent.forEach((msg, i) => timeline.appendChild(messageCard(msg, data.messages.length - recent.length + i)));
    timeline.scrollTop = timeline.scrollHeight;
  }
  byId('timeline-count').textContent = `${data.messages.length} 条`;
  byId('history-status').textContent = `history: ${data.messages.length} 条`;

  const list = byId('history-list');
  list.replaceChildren();
  if (!data.messages.length) {
    const empty = document.createElement('div');
    empty.className = 'oc-empty';
    empty.textContent = '暂无会话历史。';
    list.appendChild(empty);
  } else {
    data.messages.forEach((msg, i) => list.appendChild(messageCard(msg, i, true)));
  }
}

/* ------------------------------------------------------------ actions */
async function copyCode() {
  if (!state.code) { warn('没有可复制的代码'); return; }
  try {
    await navigator.clipboard.writeText(state.code);
    log('已复制到浏览器剪贴板', 'ok');
  } catch {
    await api('/api/clipboard', { method: 'POST', body: JSON.stringify({ action: 'write', text: state.code }) });
    log('已复制到系统剪贴板', 'ok');
  }
  setStatus('代码已复制', 'ok');
  pulse('COPIED', 'info');
}

async function saveCode() {
  if (!state.code) { warn('没有可保存的代码'); return; }
  const filename = byId('save-name').value.trim() || 'solution.cpp';
  try {
    const data = await api('/api/save', { method: 'POST', body: JSON.stringify({ filename, code: state.code }) });
    setStatus(`已保存 ${data.path}`, 'ok');
    log(`saved → ${data.path} (${data.bytes} B)`, 'ok');
    pulse('SAVED');
    flash();
  } catch (err) { warn(`保存失败: ${err.message}`); }
}

function downloadCode() {
  if (!state.code) { warn('没有可下载的代码'); return; }
  const name = byId('save-name').value.trim() || 'solution.cpp';
  const filename = name.split(/[\\/]/).pop().replace(/\.cpp$/i, '') + '.cpp';
  const url = URL.createObjectURL(new Blob([state.code], { type: 'text/plain;charset=utf-8' }));
  const a = document.createElement('a');
  a.href = url;
  a.download = filename;
  a.click();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
  log(`download → ${filename}`, 'ok');
  pulse('DOWNLOAD', 'info');
}

async function readClipboard() {
  try {
    const data = await api('/api/clipboard', { method: 'POST', body: JSON.stringify({ action: 'read' }) });
    if (!data.text.trim()) { warn('系统剪贴板为空'); return; }
    byId('prompt').value = data.text;
    log(`剪贴板 → 题面 (${data.text.length} 字符)`, 'ok');
    setStatus('已从系统剪贴板载入题面', 'ok');
  } catch (err) { warn(`读取剪贴板失败: ${err.message}`); }
}

async function readFilePath() {
  const path = byId('file-path').value.trim();
  if (!path) { warn('请先填写文件路径'); byId('file-path').focus(); return; }
  try {
    const data = await api('/api/read-file', { method: 'POST', body: JSON.stringify({ path }) });
    byId('prompt').value = data.content;
    log(`read → ${data.path} (${data.content.length} 字符)`, 'ok');
    setStatus(`已载入 ${data.path}`, 'ok');
  } catch (err) { warn(`读取失败: ${err.message}`); }
}

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
    setStatus('设置已保存', 'ok');
    log('配置已写入 ~/.oicoder_config.json', 'ok');
    pulse('SAVED');
  } catch (err) { warn(`保存设置失败: ${err.message}`); }
}

async function clearHistory() {
  const btn = byId('history-clear');
  if (!state.clearArmed) {
    state.clearArmed = true;
    btn.setAttribute('label', '再点一次确认');
    setTimeout(() => {
      state.clearArmed = false;
      btn.setAttribute('label', '清空历史');
    }, 3500);
    log('清空历史：等待二次确认', 'warn');
    return;
  }
  state.clearArmed = false;
  btn.setAttribute('label', '清空历史');
  await api('/api/history', { method: 'DELETE' });
  await Promise.allSettled([loadHistory(), refreshStatus()]);
  renderOutput('');
  setStatus('会话历史已清空', 'ok');
  log('会话历史已清空', 'ok');
  pulse('CLEARED', 'warning');
}

/* ------------------------------------------------------------ wiring */
function bind(id, handler) {
  const el = byId(id);
  if (el) el.addEventListener('click', handler);
}

function init() {
  initSwatches();
  setTheme(localStorage.getItem('oi-theme') || 'blue', true);
  setReducedMotion(localStorage.getItem('oi-reduce') === '1', true);

  $$('.oc-nav a').forEach((a) => a.addEventListener('click', (e) => {
    e.preventDefault();
    location.hash = a.dataset.view;
  }));
  window.addEventListener('hashchange', () => setView(location.hash.slice(1)));
  setView(location.hash.slice(1) || 'workbench');

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
    byId('log-btn').setAttribute('label', '日志 0');
  });

  $$('.oc-tabs button').forEach((b) => b.addEventListener('click', () => setTab(b.dataset.tab)));

  bind('generate-btn', generate);
  bind('stop-btn', () => state.controller?.abort());
  bind('copy-btn', copyCode);
  bind('save-btn', saveCode);
  bind('download-btn', downloadCode);
  bind('clipboard-btn', readClipboard);
  bind('file-btn', readFilePath);
  bind('sample-btn', () => { byId('prompt').value = SAMPLE; log('已填入示例题面'); byId('prompt').focus(); });
  bind('clear-prompt-btn', () => { byId('prompt').value = ''; byId('prompt').focus(); });
  byId('new-topic').addEventListener('change', (e) => {
    byId('mode-badge').textContent = e.target.checked ? '新话题' : '延续上下文';
    byId('mode-badge').className = `oc-badge ${e.target.checked ? 'info' : ''}`;
  });
  byId('temperature').addEventListener('input', (e) => {
    byId('temperature-out').textContent = Number(e.target.value).toFixed(1);
  });
  byId('file-input').addEventListener('change', async (e) => {
    const file = e.target.files?.[0];
    if (!file) return;
    byId('prompt').value = await file.text();
    log(`载入文件 → ${file.name}`, 'ok');
    setStatus(`已载入 ${file.name}`, 'ok');
    e.target.value = '';
  });
  byId('file-path').addEventListener('keydown', (e) => { if (e.key === 'Enter') readFilePath(); });
  byId('save-name').addEventListener('keydown', (e) => { if (e.key === 'Enter') saveCode(); });

  byId('config-form').addEventListener('submit', saveConfig);
  bind('config-save', saveConfig);
  bind('config-reload', async () => { await refreshStatus(); log('配置已重新载入'); });
  bind('history-refresh', loadHistory);
  bind('history-clear', clearHistory);

  document.addEventListener('keydown', (e) => {
    const tag = (e.target.tagName || '').toLowerCase();
    const typing = ['input', 'textarea', 'select'].includes(tag);
    if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') { e.preventDefault(); generate(); return; }
    if ((e.ctrlKey || e.metaKey) && e.key.toLowerCase() === 'l') { e.preventDefault(); toggleLog(); return; }
    if (e.key === 'Escape') {
      if (state.generating) state.controller?.abort();
      byId('log-panel').classList.remove('is-open');
      return;
    }
    if (typing || e.ctrlKey || e.metaKey || e.altKey) return;
    const keyMap = { w: 'workbench', h: 'history', s: 'settings', '?': 'cheatsheet' };
    const view = keyMap[e.key.toLowerCase()];
    if (view) { e.preventDefault(); location.hash = view; }
  });

  log('OI Code Console 已启动');
  Promise.allSettled([refreshStatus(), loadHistory()]).then((results) => {
    const failed = results.find((r) => r.status === 'rejected');
    if (failed) {
      log(`服务连接失败: ${failed.reason.message}`, 'error');
      byId('api-dot').className = 'oc-dot bad';
      byId('api-status').textContent = '服务未响应';
    } else {
      log('状态与历史已载入', 'ok');
    }
  });
}

init();
