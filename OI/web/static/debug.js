/* debug.js — debug dock controller: gdbserver-backed sessions via /api/debug*.
   Syncs breakpoint gutter ↔ session, renders variables/watch, highlights the
   current line, streams live stdout/stderr into the dock panes. */

import { t } from './i18n.js';

const byId = (id) => document.getElementById(id);
const S = () => window.__oiConsole;

let sid = null;
let pollTimer = null;
let lastState = null;

const STATUS_TEXT = {
  running: '运行中',
  stopped: '已停止',
  exited: '已退出',
  error: '错误',
  dead: '已结束',
  starting: '启动中',
};

function dbgBadge(text, kind = '') {
  const el = byId('dbg-status');
  el.textContent = text;
  el.className = `oc-badge ${kind}`;
}

function setBusy(busy) {
  ['dbg-continue', 'dbg-next', 'dbg-step', 'dbg-finish'].forEach((id) => {
    byId(id).toggleAttribute('disabled', busy);
  });
}

async function callDebug(action, extra = {}) {
  const data = await S().api('/api/debug/command', {
    method: 'POST',
    body: JSON.stringify({ session: sid, action, ...extra }),
  });
  render(data.state);
  return data.state;
}

async function startDebug() {
  const s = S();
  if (sid) {
    s.warn(t('已有调试会话在运行，先结束（Esc 或「结束调试」）'));
    return;
  }
  const ide = window.__oiIde;
  const code = ide.getCode();
  if (!code.trim()) {
    s.warn(t('编辑器没有代码'));
    return;
  }
  const breakpoints = ide.editor.getBreakpoints();
  const stdin = byId('dock-stdin').value;
  dbgBadge(t('编译+附加中…'), 'warn');
  setBusy(true);
  window.__ideShowDockTab('debug');
  try {
    const data = await s.api('/api/debug/start', {
      method: 'POST',
      body: JSON.stringify({ code, stdin, breakpoints }),
    });
    sid = data.session;
    render(data.state);
    byId('ide-stop').removeAttribute('disabled');
    s.log(t('调试会话 {sid} 启动：{reason}', { sid, reason: data.state.reason || data.state.status }), 'ok');
  } catch (err) {
    dbgBadge(t('启动失败'), 'err');
    setBusy(false);
    s.warn(t('调试启动失败: {msg}', { msg: err.message }));
  }
}

function render(state) {
  if (!state) return;
  lastState = state;
  const st = state.status;
  dbgBadge(t(STATUS_TEXT[st] || st) || st, st === 'stopped' ? 'ok' : st === 'running' ? 'warn' : st === 'exited' ? '' : st === 'error' ? 'err' : '');
  byId('dbg-reason').textContent = state.reason || '';

  // current line highlight
  window.__oiIde.editor.setDebugLine(st === 'stopped' ? state.line : 0);

  // variables
  const vars = byId('dbg-vars');
  vars.replaceChildren();
  (state.variables || []).forEach((v) => {
    const dt = document.createElement('dt');
    dt.textContent = v.type ? `${v.name} : ${v.type}` : v.name;
    const dd = document.createElement('dd');
    dd.textContent = v.value;
    vars.append(dt, dd);
  });
  if (!state.variables || !state.variables.length) {
    const dt = document.createElement('dt');
    const dd = document.createElement('dd');
    dt.textContent = t('（无局部变量）');
    dd.textContent = t('运行到断点处后显示');
    vars.append(dt, dd);
  }

  // watch
  const watch = byId('dbg-watch');
  watch.replaceChildren();
  (state.watch || []).forEach((w) => {
    const dt = document.createElement('dt');
    dt.textContent = w.expr;
    const dd = document.createElement('dd');
    dd.textContent = w.error ? `${w.value}` : w.value;
    if (w.error) dd.style.color = 'var(--state-warning, #ffcc22)';
    const del = document.createElement('button');
    del.type = 'button';
    del.className = 'oc-btn';
    del.textContent = '×';
    del.style.marginLeft = '6px';
    del.addEventListener('click', () => callDebug('watch_del', { expr: w.expr }));
    dt.append(del);
    watch.append(dt, dd);
  });

  // live output panes
  if (state.stdout) byId('dock-stdout').textContent = state.stdout || t('（暂无输出）');
  if (state.stderr) byId('dock-stderr').textContent = state.stderr || '';
  byId('dock-meta').textContent = st === 'exited'
    ? `exit ${state.exit_code ?? '—'} · ${t('调试结束')}`
    : t('行 {line} · {status}', { line: state.line || '—', status: t(STATUS_TEXT[st] || st) });

  // gutter sync (server-side truth after adds/deletes)
  if (state.breakpoints) {
    const current = window.__oiIde.editor.getBreakpoints();
    const same = current.length === state.breakpoints.length
      && current.every((n, i) => n === state.breakpoints[i]);
    if (!same) window.__oiIde.editor.setBreakpoints(state.breakpoints);
  }

  // poll while running so async *stopped (breakpoint hit) is picked up
  clearInterval(pollTimer);
  if (st === 'running') {
    pollTimer = setInterval(async () => {
      try {
        const data = await S().api('/api/debug/command', {
          method: 'POST',
          body: JSON.stringify({ session: sid, action: 'state' }),
        });
        if (data.state.status !== 'running') render(data.state);
      } catch {
        clearInterval(pollTimer);
      }
    }, 1200);
  } else {
    setBusy(false);
    if (st === 'exited' || st === 'error' || st === 'dead') {
      clearInterval(pollTimer);
      sid = null;
      byId('ide-stop').setAttribute('disabled', '');
      if (st === 'exited') S().pulse('DONE', 'info');
    }
  }
}

async function stopDebug() {
  if (!sid) return;
  clearInterval(pollTimer);
  const current = sid;
  sid = null;
  setBusy(false);
  window.__oiIde.editor.setDebugLine(0);
  dbgBadge(t('未调试'), '');
  byId('ide-stop').setAttribute('disabled', '');
  try {
    await S().api('/api/debug/stop', {
      method: 'POST',
      body: JSON.stringify({ session: current }),
    });
    S().log(t('调试会话已结束'), 'ok');
  } catch (err) {
    S().log(t('结束调试时出错: {msg}', { msg: err.message }), 'warn');
  }
}

function init() {
  const s = S();
  byId('ide-debug').addEventListener('click', startDebug);
  byId('dbg-stop').addEventListener('click', stopDebug);
  byId('dbg-continue').addEventListener('click', () => callDebug('continue'));
  byId('dbg-next').addEventListener('click', () => callDebug('next'));
  byId('dbg-step').addEventListener('click', () => callDebug('step'));
  byId('dbg-finish').addEventListener('click', () => callDebug('finish'));
  byId('watch-add').addEventListener('click', () => {
    const expr = byId('watch-input').value.trim();
    if (!expr) return;
    byId('watch-input').value = '';
    callDebug('watch_add', { expr });
  });
  byId('watch-input').addEventListener('keydown', (e) => {
    if (e.key === 'Enter') byId('watch-add').click();
  });

  window.addEventListener('oi-debug', startDebug);
  window.addEventListener('oi-debug-stop', stopDebug);
  window.addEventListener('oi-dbg-next', () => sid && callDebug('next'));
  window.addEventListener('oi-dbg-step', () => sid && callDebug('step'));
  window.addEventListener('oi-bp-changed', (event) => {
    if (!sid) return;
    const { line, added } = event.detail || {};
    if (!line) return;
    callDebug(added ? 'bp_add' : 'bp_del', { line });
  });
  window.addEventListener('hashchange', () => {
    if (location.hash !== '#ide' && sid && lastState && lastState.status === 'running') {
      // keep polling while hidden; no-op
    }
  });
  s.log(t('调试台就绪（gdbserver + GDB/MI）'), 'ok');
}

if (window.__oiConsole) init();
else document.addEventListener('DOMContentLoaded', () => setTimeout(() => window.__oiConsole && init(), 0));
