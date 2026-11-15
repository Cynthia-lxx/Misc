/* overlays.js — overlay layer for the OI IDE: file open, save-as, AI prompt,
   goto-line and the command palette. One shared layer; Esc closes it via
   'oi-overlay-close' dispatched by console.js. */

const byId = (id) => document.getElementById(id);
const S = () => window.__oiConsole;

let currentCleanup = null;

function openOverlay(title, subtitle, buildBody, cleanup) {
  const layer = byId('ov-layer');
  const box = byId('ov-box');
  box.replaceChildren();
  const head = document.createElement('div');
  head.className = 'ov-head';
  const text = document.createElement('div');
  const h2 = document.createElement('h2');
  h2.textContent = title;
  text.appendChild(h2);
  if (subtitle) {
    const p = document.createElement('p');
    p.textContent = subtitle;
    text.appendChild(p);
  }
  const close = document.createElement('button');
  close.type = 'button';
  close.className = 'oc-btn';
  close.textContent = '×';
  close.addEventListener('click', closeOverlay);
  head.append(text, close);
  const body = document.createElement('div');
  body.className = 'ov-body';
  buildBody(body, closeOverlay);
  box.append(head, body);
  layer.classList.remove('is-hidden');
  currentCleanup = cleanup || null;
}

function closeOverlay() {
  byId('ov-layer').classList.add('is-hidden');
  if (currentCleanup) {
    try { currentCleanup(); } catch { /* noop */ }
    currentCleanup = null;
  }
}

/* ------------------------------------------------------------ file open */
async function openFileOverlay() {
  const s = S();
  s.log('打开文件列表…');
  let files = [];
  try {
    const data = await s.api('/api/analyze/files');
    files = data.files || [];
  } catch (err) {
    s.warn(`读取工作区文件列表失败: ${err.message}`);
    return;
  }
  openOverlay('打开文件', `工作区 .cpp × ${files.length}（Esc 关闭）`, (body, done) => {
    const search = document.createElement('input');
    search.type = 'text';
    search.className = 'ov-input';
    search.placeholder = '筛选文件名…';
    const list = document.createElement('div');
    list.className = 'ov-file-list';

    function render(filter) {
      list.replaceChildren();
      const needle = (filter || '').toLowerCase();
      files
        .filter((f) => !needle || f.toLowerCase().includes(needle))
        .slice(0, 200)
        .forEach((name) => {
          const btn = document.createElement('button');
          btn.type = 'button';
          btn.className = 'ov-file-item';
          const span = document.createElement('span');
          span.textContent = name;
          const meta = document.createElement('small');
          meta.textContent = '载入 → 编辑器';
          btn.append(span, meta);
          btn.addEventListener('click', () => loadFile(name, done));
          list.appendChild(btn);
        });
      if (!list.children.length) {
        const empty = document.createElement('p');
        empty.className = 'oc-empty';
        empty.textContent = '没有匹配的文件';
        list.appendChild(empty);
      }
    }
    search.addEventListener('input', () => render(search.value));
    render('');
    body.append(search, list);
    setTimeout(() => search.focus(), 30);
  });
}

async function loadFile(path, done) {
  const s = S();
  try {
    const data = await s.api('/api/read-file', {
      method: 'POST',
      body: JSON.stringify({ path }),
    });
    window.dispatchEvent(new CustomEvent('oi-load-code', { detail: { code: data.code } }));
    byId('ide-filename').value = path;
    s.log(`已载入 ${path}（${(data.code || '').split('\n').length} 行）`, 'ok');
    s.pulse('LOADED', 'info');
    location.hash = 'ide';
    done();
  } catch (err) {
    s.warn(`读取 ${path} 失败: ${err.message}`);
  }
}

/* ------------------------------------------------------------ save as */
function saveOverlay() {
  openOverlay('保存代码', '相对路径基于工作区；自动补 .cpp', (body, done) => {
    const input = document.createElement('input');
    input.type = 'text';
    input.className = 'ov-input';
    input.value = byId('ide-filename').value.trim() || 'solution.cpp';
    const row = document.createElement('div');
    row.className = 'ov-row';
    const save = document.createElement('button');
    save.type = 'button';
    save.className = 'oc-btn sheared';
    save.textContent = '保存';
    const saveAndClose = async () => {
      byId('ide-filename').value = input.value.trim() || 'solution.cpp';
      await window.__oiIde.saveCode();
      done();
    };
    save.addEventListener('click', saveAndClose);
    input.addEventListener('keydown', (e) => { if (e.key === 'Enter') saveAndClose(); });
    row.append(input, save);
    body.append(row);
    setTimeout(() => { input.focus(); input.select(); }, 30);
  });
}

/* ------------------------------------------------------------ AI prompt */
function promptOverlay() {
  const s = S();
  openOverlay('AI 生成题解', '必用 AI · 题面 → 流式写入编辑器（1 次生成 ≈ 1 次代码生成用量，不带历史）', (body, done) => {
    const area = document.createElement('textarea');
    area.className = 'ov-prompt-area';
    area.placeholder = '粘贴题目描述（可含样例输入输出）…';
    const status = document.createElement('span');
    status.className = 'ov-generating';
    const row1 = document.createElement('div');
    row1.className = 'ov-row';
    const clipBtn = document.createElement('button');
    clipBtn.type = 'button';
    clipBtn.className = 'oc-btn';
    clipBtn.textContent = '读剪贴板';
    clipBtn.addEventListener('click', async () => {
      try {
        const text = await navigator.clipboard.readText();
        if (text) area.value = text;
      } catch {
        s.warn('无法读取剪贴板（浏览器权限）');
      }
    });
    const sampleBtn = document.createElement('button');
    sampleBtn.type = 'button';
    sampleBtn.className = 'oc-btn';
    sampleBtn.textContent = '示例题面';
    sampleBtn.addEventListener('click', () => {
      area.value = [
        '【题目】数列求和',
        '输入：第一行 n，第二行 n 个绝对值 ≤ 1e6 的整数。',
        '输出：一行，为这 n 个数的和。',
        '样例输入：',
        '3',
        '1 2 3',
        '样例输出：',
        '6',
        '数据范围：n ≤ 1e6。',
      ].join('\n');
    });
    const clearBtn = document.createElement('button');
    clearBtn.type = 'button';
    clearBtn.className = 'oc-btn';
    clearBtn.textContent = '清空';
    clearBtn.addEventListener('click', () => { area.value = ''; area.focus(); });
    row1.append(clipBtn, sampleBtn, clearBtn);

    const row2 = document.createElement('div');
    row2.className = 'ov-row';
    const go = document.createElement('button');
    go.type = 'button';
    go.className = 'oc-btn sheared';
    go.textContent = '生成 ⌃⏎';
    const stop = document.createElement('button');
    stop.type = 'button';
    stop.className = 'oc-btn';
    stop.textContent = '停止';
    stop.style.display = 'none';
    row2.append(go, stop, status);

    const controller = { abort: null };
    go.addEventListener('click', async () => {
      const prompt = area.value.trim();
      if (!prompt) { s.warn('题面是空的'); return; }
      const ac = new AbortController();
      controller.abort = () => ac.abort();
      go.disabled = true;
      stop.style.display = '';
      status.textContent = '生成中…';
      done(); // close overlay; streaming continues into the editor
      await window.__oiIde.generate(prompt, ac.signal);
    });
    stop.addEventListener('click', () => controller.abort && controller.abort());

    area.addEventListener('keydown', (e) => {
      if ((e.ctrlKey || e.metaKey) && e.key === 'Enter') go.click();
    });
    body.append(area, row1, row2);
    setTimeout(() => area.focus(), 30);
    currentCleanup = () => controller.abort && controller.abort();
  });
}

/* ------------------------------------------------------------ goto line */
function gotoOverlay() {
  openOverlay('跳转到行', 'Ctrl+G', (body, done) => {
    const input = document.createElement('input');
    input.type = 'text';
    input.className = 'ov-input';
    input.placeholder = '行号';
    input.addEventListener('keydown', (e) => {
      if (e.key === 'Enter') {
        const n = parseInt(input.value, 10);
        if (Number.isFinite(n) && n > 0) {
          window.__oiIde.gotoLine(n);
          done();
        }
      }
    });
    body.append(input);
    setTimeout(() => input.focus(), 30);
  });
}

/* ------------------------------------------------------------ command palette */
function paletteOverlay() {
  const s = S();
  const commands = [
    { label: '运行代码', hint: 'F5', run: () => window.dispatchEvent(new CustomEvent('oi-run')) },
    { label: '启动调试', hint: 'F9', run: () => window.dispatchEvent(new CustomEvent('oi-debug')) },
    { label: '保存代码', hint: 'Ctrl+S', run: () => window.dispatchEvent(new CustomEvent('oi-save')) },
    { label: '打开文件…', hint: '', run: () => openFileOverlay() },
    { label: 'AI 生成题解…', hint: 'Ctrl+Enter', run: () => promptOverlay() },
    { label: '分析当前代码', hint: 'A', run: () => window.dispatchEvent(new CustomEvent('oi-analyze-request')) },
    { label: '查看完整报告', hint: '', run: () => { location.hash = 'analysis'; } },
    { label: '复制代码', hint: '', run: () => window.dispatchEvent(new CustomEvent('oi-copy')) },
    { label: '跳转到行…', hint: 'Ctrl+G', run: () => gotoOverlay() },
    { label: '打开会话历史', hint: 'H', run: () => { location.hash = 'history'; } },
    { label: '打开设置', hint: 'S', run: () => { location.hash = 'settings'; } },
    { label: '切换主题…', hint: '', run: () => {
      const current = document.documentElement.dataset.theme;
      const themes = ['red', 'orange', 'lime', 'green', 'aquamarine', 'blue', 'purple', 'plum', 'pink'];
      const next = themes[(themes.indexOf(current) + 1) % themes.length];
      document.getElementById('theme-select').value = next;
      document.getElementById('theme-select').dispatchEvent(new Event('change'));
      s.log(`theme → ${next}`);
    } },
    { label: '清空编辑器', hint: '', run: () => window.dispatchEvent(new CustomEvent('oi-load-code', { detail: { code: '' } })) },
  ];

  openOverlay('命令面板', 'Ctrl+K · ↑↓ 选择 · Enter 执行', (body, done) => {
    const input = document.createElement('input');
    input.type = 'text';
    input.className = 'ov-input';
    input.placeholder = '输入命令…';
    const list = document.createElement('div');
    list.className = 'ov-palette-list';
    let active = 0;
    let filtered = commands;

    function render() {
      list.replaceChildren();
      filtered.forEach((cmd, i) => {
        const btn = document.createElement('button');
        btn.type = 'button';
        btn.className = `ov-palette-item${i === active ? ' is-active' : ''}`;
        const span = document.createElement('span');
        span.textContent = cmd.label;
        const hint = document.createElement('small');
        hint.textContent = cmd.hint;
        btn.append(span, hint);
        btn.addEventListener('click', () => { done(); cmd.run(); });
        list.appendChild(btn);
      });
    }
    function move(delta) {
      if (!filtered.length) return;
      active = (active + delta + filtered.length) % filtered.length;
      render();
    }
    input.addEventListener('input', () => {
      const needle = input.value.trim().toLowerCase();
      filtered = commands.filter((c) => !needle || c.label.toLowerCase().includes(needle));
      active = 0;
      render();
    });
    input.addEventListener('keydown', (e) => {
      if (e.key === 'ArrowDown') { e.preventDefault(); move(1); }
      else if (e.key === 'ArrowUp') { e.preventDefault(); move(-1); }
      else if (e.key === 'Enter') {
        e.preventDefault();
        const cmd = filtered[active];
        if (cmd) { done(); cmd.run(); }
      }
    });
    render();
    body.append(input, list);
    setTimeout(() => input.focus(), 30);
  });
}

/* ------------------------------------------------------------ wiring */
function init() {
  window.addEventListener('oi-overlay-close', closeOverlay);
  window.addEventListener('oi-open-overlay', openFileOverlay);
  window.addEventListener('oi-save-overlay', saveOverlay);
  window.addEventListener('oi-prompt-overlay', promptOverlay);
  window.addEventListener('oi-palette', paletteOverlay);
  window.addEventListener('oi-goto', gotoOverlay);
  const isOpen = () => !byId('ov-layer').classList.contains('is-hidden');
  window.__oiOverlays = { openOverlay, closeOverlay, isOpen, promptOverlay };
  window.__oiConsole.log('overlay 层就绪', 'ok');
}

if (window.__oiConsole) init();
else document.addEventListener('DOMContentLoaded', () => setTimeout(() => window.__oiConsole && init(), 0));

