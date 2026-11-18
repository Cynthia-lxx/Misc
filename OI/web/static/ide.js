/* ide.js — the cloud IDE controller: CodeMirror editor, run (stdout/stderr
   split), save/copy/open, key metrics + AI side panel, and streaming AI
   generation into the editor. Debug dock lives in debug.js. */

import { createEditor } from './editor.js';
import { currentLimits } from './analyze.js';
import { t } from './i18n.js';

const byId = (id) => document.getElementById(id);
const shell = () => window.__oiConsole;

let editorApi = null;
let runAbort = null;
let lastReport = null;
let analysisBusy = false;

const VERDICT_CLASS = { green: 'green', yellow: 'yellow', red: 'red', unknown: 'unknown' };
const VERDICT_TEXT = { green: '通过', yellow: '风险', red: '问题', unknown: '未知' };
const DIM_LABEL = { memory: '内存', complexity: '复杂度', style: '风格冗余', performance: '性能' };
const DIM_ORDER = ['memory', 'complexity', 'style', 'performance'];

/* ------------------------------------------------------------ editor */
async function initEditor() {
  const initial = byId('ide-editor').dataset.initial || '';
  editorApi = await createEditor(byId('ide-editor'), {
    value: initial,
    onSave: () => saveCode(),
    onGotoRequest: () => window.dispatchEvent(new CustomEvent('oi-goto')),
    onToggleBreakpoint: (line, added) => window.dispatchEvent(
      new CustomEvent('oi-bp-changed', { detail: { line, added } }),
    ),
  });
  window.__oiIde = {
    editor: editorApi,
    getCode: () => editorApi.getValue(),
    setCode: (code) => {
      editorApi.setValue(code ?? '');
      lastReport = null;
      renderMetrics(null);
    },
    getFilename: () => byId('ide-filename').value.trim() || 'solution.cpp',
    gotoLine: (n) => editorApi.gotoLine(n),
    saveCode,
    generate: handleGenerate,
  };
  window.dispatchEvent(new CustomEvent('oi-editor-ready'));
  return editorApi;
}

window.addEventListener('oi-load-code', (event) => {
  const code = (event.detail && event.detail.code) ?? '';
  if (editorApi) editorApi.setValue(code);
});

/* ------------------------------------------------------------ run */
function setRunBadge(text, kind = '') {
  const badge = byId('ide-run-badge');
  badge.textContent = text;
  badge.className = `oc-badge ${kind}`;
}

function humanBytes(n) {
  if (n == null) return '—';
  if (n >= 1024 * 1024) return `${(n / 1024 / 1024).toFixed(2)} MiB`;
  return `${(n / 1024).toFixed(1)} KiB`;
}

function showDockTab(name) {
  byId('ide-dock').querySelectorAll('[data-dtab]').forEach((b) => {
    b.classList.toggle('is-active', b.dataset.dtab === name);
  });
  const panes = { stdin: 'dock-stdin', stdout: 'dock-stdout', stderr: 'dock-stderr', debug: 'dock-debug' };
  Object.entries(panes).forEach(([key, id]) => {
    byId(id).classList.toggle('is-hidden', key !== name);
  });
}
window.__ideShowDockTab = showDockTab;

async function runCode() {
  const S = shell();
  const code = editorApi.getValue();
  if (!code.trim()) {
    S.warn(t('编辑器没有代码，先写点或让 AI 生成'));
    return;
  }
  runAbort = new AbortController();
  setRunBadge(t('运行中…'), 'warn');
  showDockTab('stdout');
  byId('dock-stdout').textContent = t('（编译中…）');
  byId('dock-stderr').textContent = '';
  try {
    const res = await fetch('/api/run', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        code,
        stdin: byId('dock-stdin').value,
        timeout: 8,
      }),
      signal: runAbort.signal,
    });
    const data = await res.json();
    if (!res.ok || data.ok === false) throw new Error(data.error || `HTTP ${res.status}`);
    byId('dock-stdout').textContent = data.stdout || t('（无输出）');
    byId('dock-stderr').textContent = data.stderr || (data.status === 'ok' ? '' : data.reason || '');
    byId('dock-meta').textContent =
      `exit ${data.exit_code ?? '—'} · ${data.wall_ms ?? '—'} ms · peak ${humanBytes(data.peak_bytes)}`
      + (data.compile_cached ? ` · ${t('缓存编译')}` : ` · ${t('编译 {ms} ms', { ms: data.compile_ms ?? '—' })}`);
    if (data.status === 'ok') {
      setRunBadge(t('通过 {ms} ms', { ms: data.wall_ms }), 'ok');
      S.pulse('PASSED');
      S.log(t('运行通过：exit 0，{ms} ms，峰值 {mem}', { ms: data.wall_ms, mem: humanBytes(data.peak_bytes) }), 'ok');
    } else if (data.status === 'timeout') {
      setRunBadge(t('超时'), 'warn');
      S.warn(t('运行超时（{reason}）', { reason: data.reason }));
    } else if (data.phase === 'compile') {
      setRunBadge(t('编译失败'), 'err');
      S.warn(t('编译失败，stderr 标签有详情'));
    } else {
      setRunBadge(t('退出码 {code}', { code: data.exit_code }), 'err');
      S.log(t('运行结束：exit {code}', { code: data.exit_code }), 'warn');
    }
    if (data.stderr) showDockTab('stderr');
  } catch (err) {
    if (err.name === 'AbortError') {
      setRunBadge(t('已停止'), 'warn');
      byId('dock-stdout').textContent += `\n${t('（已手动停止）')}`;
      S.log(t('运行已手动停止'), 'warn');
    } else {
      setRunBadge(t('失败'), 'err');
      S.warn(t('运行失败: {msg}', { msg: err.message }));
    }
  } finally {
    runAbort = null;
  }
}

function stopRun() {
  if (runAbort) runAbort.abort();
  window.dispatchEvent(new CustomEvent('oi-debug-stop'));
}

/* ------------------------------------------------------------ save / copy */
async function saveCode() {
  const S = shell();
  const filename = byId('ide-filename').value.trim() || 'solution.cpp';
  try {
    const data = await S.api('/api/save', {
      method: 'POST',
      body: JSON.stringify({ code: editorApi.getValue(), filename }),
    });
    S.setStatus(t('已保存 {path}', { path: data.path }), 'ok');
    S.log(t('已保存 → {path}', { path: data.path }), 'ok');
    S.pulse('SAVED');
  } catch (err) {
    S.warn(t('保存失败: {msg}', { msg: err.message }));
  }
}

async function copyCode() {
  const S = shell();
  const code = editorApi.getValue();
  if (!code.trim()) return;
  let ok = false;
  try {
    await navigator.clipboard.writeText(code);
    ok = true;
  } catch {
    const ta = document.createElement('textarea');
    ta.value = code;
    document.body.appendChild(ta);
    ta.select();
    ok = document.execCommand('copy');
    ta.remove();
  }
  if (ok) {
    S.setStatus(t('代码已复制到剪贴板'), 'ok');
    S.pulse('COPIED');
    S.log(t('代码已复制'), 'ok');
  } else {
    S.warn(t('剪贴板不可用'));
  }
  }

/* ------------------------------------------------------------ side metrics */
function worstVerdict(findings) {
  const rank = { red: 3, yellow: 2, green: 1, unknown: 0 };
  let worst = 'green';
  for (const f of findings) {
    if ((rank[f.verdict] ?? 0) > (rank[worst] ?? 0)) worst = f.verdict;
  }
  return worst;
}

function renderMetrics(report) {
  const wrap = byId('ide-metrics');
  const badge = byId('ide-verdict-badge');
  wrap.replaceChildren();
  if (!report) {
    badge.className = 'an-verdict-badge unknown';
    badge.textContent = '?';
    byId('ide-verdict-title').textContent = t('未分析');
    byId('ide-verdict-reason').textContent = t('点击「分析 A」体检当前代码');
    return;
  }
  const verdict = report.verdict || 'unknown';
  badge.className = `an-verdict-badge ${VERDICT_CLASS[verdict] || 'unknown'}`;
  badge.textContent = t(VERDICT_TEXT[verdict]) || '?';
  byId('ide-verdict-title').textContent =
    t('{verdict} · {n} 项发现', { verdict: t(VERDICT_TEXT[verdict] || '未知'), n: report.findings.length })
    + (report.unknown_count ? ` · ${report.unknown_count} unknown` : '');
  byId('ide-verdict-reason').textContent = verdict === 'green'
    ? t('本地体检未发现问题')
    : t('详见「完整报告」视图；unknown 表示静态无法确定，不做猜测');

  for (const dim of DIM_ORDER) {
    const list = report.findings.filter((f) => f.dimension === dim);
    const row = document.createElement('div');
    row.className = 'ide-metric-row';
    const badgeEl = document.createElement('span');
    badgeEl.className = `oc-badge v-${worstVerdict(list)}`;
    badgeEl.textContent = t(DIM_LABEL[dim]);
    const detail = document.createElement('code');
    detail.textContent = list.length
      ? list.map((f) => f.key.split('.')[1] || f.key).slice(0, 3).join(' · ')
      : '—';
    const count = document.createElement('span');
    count.textContent = `${list.length}`;
    row.append(badgeEl, detail, count);
    wrap.appendChild(row);
  }

  // measured vs problem limits (when the user filled them in the analysis view)
  const measure = report.measurements || {};
  const limits = report.limits || {};
  if (measure.status === 'ok' && (limits.time_ms || limits.memory_mb)) {
    const cells = [];
    if (limits.time_ms) {
      cells.push(`${t('时 {a}/{b}ms{mark}', { a: measure.wall_ms, b: limits.time_ms, mark: measure.time_exceeded ? t(' ✗超时') : ' ✓' })}`);
    }
    if (limits.memory_mb && measure.peak_bytes) {
      cells.push(`${t('存 {a}/{b}MiB{mark}', { a: (measure.peak_bytes / 1048576).toFixed(2), b: limits.memory_mb, mark: measure.memory_exceeded ? t(' ✗超限') : ' ✓' })}`);
    }
    const row = document.createElement('div');
    row.className = 'ide-metric-row';
    const badgeEl = document.createElement('span');
    badgeEl.className = `oc-badge v-${measure.limit_verdict === 'fail' ? 'red' : 'green'}`;
    badgeEl.textContent = t('限制');
    const detail = document.createElement('code');
    detail.textContent = cells.join(' · ');
    const count = document.createElement('span');
    count.textContent = measure.limit_verdict === 'fail' ? '!' : '✓';
    row.append(badgeEl, detail, count);
    wrap.appendChild(row);
  }

  const ai = report.ai_summary || {};
  renderAiSummary(ai, report);
}

function renderAiSummary(ai, report) {
  const list = byId('ide-ai-list');
  list.replaceChildren();
  const items = [];
  if (ai.text) items.push(...ai.text.split(/\n+/).filter((s) => s.trim()));
  if (ai.status && ai.status !== 'ok' && ai.reason) items.push(`(${ai.reason})`);
  if (!items.length && ai.status === 'ok') items.push(t('（无建议）'));
  for (const item of items.slice(0, 6)) {
    const li = document.createElement('li');
    li.textContent = item;
    list.appendChild(li);
  }
  const budget = ai.budget || report?.ai_summary?.budget || {};
  if (budget.cap != null) {
    byId('ide-ai-fill').style.width = `${Math.min(100, (budget.used / budget.cap) * 100)}%`;
    byId('ide-ai-note').textContent = t('额度 {used}/{cap} 当量 · 单次 0.1 · {status}', { used: budget.used, cap: budget.cap, status: ai.status || '—' });
  }
}

async function analyzeCurrent() {
  if (analysisBusy) return;
  analysisBusy = true;
  const S = shell();
  setRunBadge(t('分析中…'), 'warn');
  try {
    const data = await S.api('/api/analyze', {
      method: 'POST',
      body: JSON.stringify({ code: editorApi.getValue(), run: false, limits: currentLimits() }),
    });
    lastReport = data.report;
    renderMetrics(lastReport);
    setRunBadge(data.cached ? t('分析（缓存）') : t('分析完成'), 'ok');
    S.log(t('IDE 分析完成：{verdict}，{n} 项发现', { verdict: t(VERDICT_TEXT[lastReport.verdict] || '未知'), n: lastReport.findings.length }), 'ok');
    window.dispatchEvent(new CustomEvent('oi-side-analyzed', { detail: { report: lastReport } }));
    // one short AI summary per analysis (server-side budget + dedup)
    if (lastReport.ai_summary && lastReport.ai_summary.status === 'pending') {
      try {
        const sdata = await S.api('/api/analyze/summary', {
          method: 'POST',
          body: JSON.stringify({ digest: lastReport.digest }),
        });
        lastReport.ai_summary = sdata.ai_summary;
        renderMetrics(lastReport);
      } catch { /* summary is optional; budget/degradation handled server-side */ }
    }
  } catch (err) {
    setRunBadge(t('分析失败'), 'err');
    S.warn(t('分析失败: {msg}', { msg: err.message }));
  } finally {
    analysisBusy = false;
  }
}

/* ------------------------------------------------------------ AI generation (SSE → editor) */
async function generateFromPrompt(prompt, signal) {
  const S = shell();
  let buffer = '';
  let stream = null;
  const res = await fetch('/api/generate', {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify({ input: prompt, new_topic: true }),
    signal,
  });
  if (!res.ok || !res.body) throw new Error(`HTTP ${res.status}`);

  const reader = res.body.getReader();
  const decoder = new TextDecoder();
  let sse = '';
  while (true) {
    const { value, done } = await reader.read();
    if (done) break;
    sse += decoder.decode(value, { stream: true });
    const parts = sse.split('\n\n');
    sse = parts.pop();
    for (const part of parts) {
      const dataLines = part.split('\n').filter((l) => l.startsWith('data:'));
      if (!dataLines.length) continue;
      const payload = dataLines.map((l) => l.slice(5).trim()).join('');
      if (payload === '[DONE]') continue;
      let evt;
      try { evt = JSON.parse(payload); } catch { continue; }
      if (evt.type === 'error') throw new Error(evt.message || t('生成失败'));
      if (evt.type === 'delta' && evt.text) buffer += evt.text;
      if (evt.type === 'done') {
        stream = evt;
        break;
      }
    }
    if (stream) break;
    if (buffer) {
      const code = S.extractCode(buffer);
      editorApi.setValue(code || buffer);
      editorApi.cm.execCommand('goDocEnd');
    }
  }
  if (!stream) throw new Error(t('生成流意外中断'));
  return stream.code || S.extractCode(stream.content || buffer) || buffer;
}

async function handleGenerate(prompt) {
  const S = shell();
  const controller = new AbortController();
  setRunBadge(t('AI 生成中…'), 'warn');
  showDockTab('stdout');
  byId('dock-stdout').textContent = t('（AI 正在把题解写入编辑器…）');
  try {
    const buffer = await generateFromPrompt(prompt, controller.signal);
    const code = S.extractCode(buffer);
    if (code) editorApi.setValue(code);
    setRunBadge(t('AI 生成完成'), 'ok');
    S.log(t('AI 生成完成：{n} 行', { n: (code || buffer).split('\n').length }), 'ok');
    S.pulse('GENERATED');
    await refreshStatusQuiet();
    analyzeCurrent();
  } catch (err) {
    if (err.name === 'AbortError') {
      setRunBadge(t('生成已停止'), 'warn');
      S.log(t('AI 生成已停止'), 'warn');
    } else {
      setRunBadge(t('生成失败'), 'err');
      S.warn(t('AI 生成失败: {msg}', { msg: err.message }));
    }
  }
}

async function refreshStatusQuiet() {
  try {
    const data = await window.__oiConsole.api('/api/status');
    const bridge = window.__oiConsole.state;
    bridge.config = data.config;
    bridge.messages = data.messages ?? bridge.messages;
  } catch { /* noop */ }
}

/* ------------------------------------------------------------ wiring */
function switchSideTab(name) {
  byId('ide-side').querySelectorAll('[data-stab]').forEach((b) => {
    b.classList.toggle('is-active', b.dataset.stab === name);
  });
  byId('ide-side-metrics').classList.toggle('is-hidden', name !== 'metrics');
  byId('ide-side-ai').classList.toggle('is-hidden', name !== 'ai');
}

function init() {
  const S = shell();
  byId('ide-editor').dataset.initial = byId('ide-editor').dataset.initial || '';
  initEditor();

  byId('ide-run').addEventListener('click', runCode);
  byId('ide-stop').addEventListener('click', stopRun);
  byId('ide-save').addEventListener('click', saveCode);
  byId('ide-copy').addEventListener('click', copyCode);
  byId('ide-open').addEventListener('click', () => window.dispatchEvent(new CustomEvent('oi-open-overlay')));
  byId('ide-generate').addEventListener('click', () => window.dispatchEvent(new CustomEvent('oi-prompt-overlay')));
  byId('ide-palette').addEventListener('click', () => window.dispatchEvent(new CustomEvent('oi-palette')));
  byId('ide-full-report').addEventListener('click', () => { location.hash = 'analysis'; });
  /* AI 汇总开关：状态记在模块变量里（不再解析按钮文本——i18n 后文本会变） */
  let aiSummaryEnabled = null;
  const setAiToggleLabel = (enabled) => {
    byId('ide-ai-toggle').setAttribute('label', t(enabled ? 'AI 汇总: 开' : 'AI 汇总: 关'));
  };
  S.api('/api/analyze/enabled').then((d) => {
    aiSummaryEnabled = Boolean(d.enabled);
    setAiToggleLabel(aiSummaryEnabled);
  }).catch(() => {});
  document.addEventListener('ui-i18n-change', () => {
    if (aiSummaryEnabled != null) setAiToggleLabel(aiSummaryEnabled);
  });
  byId('ide-ai-toggle').addEventListener('click', async () => {
    const btn = byId('ide-ai-toggle');
    try {
      const data = await S.api('/api/analyze/enabled', {
        method: 'POST',
        body: JSON.stringify({ enabled: !aiSummaryEnabled }),
      });
      aiSummaryEnabled = Boolean(data.enabled);
      setAiToggleLabel(aiSummaryEnabled);
      S.log(t(data.enabled ? 'AI 自动汇总已开启（本地报告不受影响）' : 'AI 自动汇总已关闭（本地报告不受影响）'), 'ok');
      renderAiSummary({ status: data.enabled ? 'skipped' : 'disabled', reason: data.enabled ? '' : '已手动关闭', budget: data });
    } catch (err) {
      S.warn(t('切换 AI 失败: {msg}', { msg: err.message }));
    }
  });
  byId('ide-analyze').addEventListener('click', () => {
    location.hash = 'analysis';
    window.dispatchEvent(new CustomEvent('oi-analyze-current'));
  });

  byId('ide-side').querySelectorAll('[data-stab]').forEach((b) => {
    b.addEventListener('click', () => switchSideTab(b.dataset.stab));
  });
  byId('ide-dock').querySelectorAll('[data-dtab]').forEach((b) => {
    b.addEventListener('click', () => showDockTab(b.dataset.dtab));
  });

  window.addEventListener('oi-run', runCode);
  window.addEventListener('oi-generate', () => window.dispatchEvent(new CustomEvent('oi-prompt-overlay')));
  window.addEventListener('oi-analyze-request', analyzeCurrent);
  window.addEventListener('oi-analyze-quiet', analyzeCurrent);
  window.addEventListener('oi-save', saveCode);
  window.addEventListener('oi-copy', copyCode);

  S.log(t('IDE 控制器就绪（CodeMirror 5）'), 'ok');
}

function boot() {
  if (window.__oiConsole) init();
  else window.addEventListener('oi-console-ready', init, { once: true });
}

/* console.js defines the bridge synchronously before this module runs */
if (window.__oiConsole) init();
else document.addEventListener('DOMContentLoaded', () => setTimeout(boot, 0));

