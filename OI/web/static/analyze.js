/* Analysis view controller. Shares the page (and the log panel) with console.js
   through window.__oiConsole; renders the local report, the optional run
   measurement and the budgeted one-shot AI summary. No framework. */

const $ = (sel) => document.querySelector(sel);
const byId = (id) => document.getElementById(id);

const VERDICT_TEXT = {
  green: ['通过', 'G'],
  yellow: ['注意', '!'],
  red: ['风险', 'R'],
  unknown: ['未知', '?'],
};
const DIM_META = {
  memory: ['内存', '全局/栈/堆估算与预算阈值'],
  complexity: ['复杂度', '循环嵌套与递归（启发式估算）'],
  style: ['风格冗余', '规则源：OI.py SYSTEM_PROMPT 十一条'],
  performance: ['性能', '常见本地反模式与热点'],
};
const DIM_ORDER = ['memory', 'complexity', 'style', 'performance'];

const view = {
  files: [],
  toolchain: {},
  budget: null,
  report: null,
  code: '',
  busy: false,
};

/* ------------------------------------------------------------ utilities */
async function api(path, options = {}) {
  const res = await fetch(path, { headers: { 'Content-Type': 'application/json' }, ...options });
  let data;
  try {
    data = await res.json();
  } catch {
    throw new Error(`HTTP ${res.status}：响应不是 JSON`);
  }
  if (!res.ok || data.ok === false) throw new Error(data.error || `HTTP ${res.status}`);
  return data;
}

function host() {
  return window.__oiConsole || { log() {}, warn() {}, pulse() {}, state: {} };
}

function fmtBytes(value) {
  if (typeof value !== 'number' || !isFinite(value)) return '—';
  const units = ['B', 'KiB', 'MiB', 'GiB'];
  let size = value;
  let index = 0;
  while (size >= 1024 && index < units.length - 1) {
    size /= 1024;
    index += 1;
  }
  return (index ? size.toFixed(2) : String(size)) + ' ' + units[index];
}

function fmtValue(f) {
  if (f.value === null || f.value === undefined) return '—';
  if (f.unit === 'bytes') return fmtBytes(f.value);
  return String(f.value);
}

function evidenceButtons(f) {
  return f.evidence.map((n) => `<button type="button" class="an-evbtn" data-ln="${n}" title="跳到第 ${n} 行">${n}</button>`).join('');
}

function findingsOf(dimension) {
  return (view.report?.findings || []).filter((f) => f.dimension === dimension);
}

/* ------------------------------------------------------------ controls */
async function refreshFiles() {
  try {
    const data = await api('/api/analyze/files');
    view.files = data.files || [];
    view.toolchain = data.toolchain || {};
    const select = byId('an-file');
    select.replaceChildren();
    view.files.forEach((item) => {
      const option = document.createElement('option');
      option.value = item.path;
      option.textContent = `${item.name} · ${fmtBytes(item.bytes)}`;
      select.appendChild(option);
    });
    const ok = view.toolchain.available;
    byId('an-toolchain-badge').textContent = ok ? '内置 GCC 可用' : '工具链缺失';
    byId('an-toolchain-badge').className = `oc-badge ${ok ? 'ok' : 'bad'}`;
  } catch (err) {
    host().warn?.(`题解列表载入失败: ${err.message}`);
  }
}

async function refreshBudget() {
  try {
    const data = await api('/api/analyze/enabled');
    view.budget = data;
    byId('an-ai-toggle').setAttribute('label', `AI: ${data.enabled ? '开' : '关'}`);
    byId('an-ai-toggle').dataset.active = data.enabled ? 'true' : 'false';
    renderBudgetLine();
  } catch (err) {
    host().warn?.(`预算状态载入失败: ${err.message}`);
  }
}

function renderBudgetLine() {
  const budget = view.budget;
  if (!budget) return;
  const fill = byId('an-budget-fill');
  const ratio = budget.cap ? Math.min(1, budget.used / budget.cap) : 0;
  fill.style.width = `${(ratio * 100).toFixed(1)}%`;
  byId('an-budget').textContent = `额度 ${budget.used}/${budget.cap} 当量`;
  const status = view.report?.ai_summary?.status;
  const note = {
    ok: `本次消耗 ${budget.per_summary} 当量 · 已用 ${budget.used}/${budget.cap}`,
    cached: '同一份代码与报告已汇总过，未消耗额度',
    disabled: 'AI 汇总已关闭，本地报告不受影响',
    skipped: budget.state || '没有 API Key，AI 汇总已跳过',
    budget_exhausted: '额度已耗尽；可在设置中调高或清空后重试',
    failed: 'AI 调用失败，本地报告不受影响',
    pending: '等待汇总…',
  }[status] || `单次汇总消耗 ${budget.per_summary} 当量，上限 ${budget.cap} 当量`;
  byId('an-budget-note').textContent = note;
}

/* ------------------------------------------------------------ analyse */
function currentBody() {
  const kind = byId('an-source').value;
  const body = { run: byId('an-measure').checked, stdin: byId('an-stdin').value };
  if (kind === 'output') {
    const code = host().state?.code || '';
    if (!code) throw new Error('输出区还没有代码，先在工作台生成或载入历史');
    body.code = code;
  } else if (kind === 'workspace') {
    const file = byId('an-file').value;
    if (!file) throw new Error('工作区没有可选的 .cpp 题解');
    body.path = file;
  } else {
    const path = byId('an-path').value.trim();
    if (!path) throw new Error('请填写文件路径');
    body.path = path;
  }
  return body;
}

async function analyze() {
  if (view.busy) return;
  let body;
  try {
    body = currentBody();
  } catch (err) {
    host().warn?.(err.message);
    return;
  }
  view.busy = true;
  byId('an-run').toggleAttribute('disabled', true);
  byId('an-cache-badge').textContent = '分析中…';
  byId('an-cache-badge').className = 'oc-badge info';
  host().log?.(`analyze → ${byId('an-source').value}${body.run ? ' · 实测' : ''}`);
  try {
    const data = await api('/api/analyze', { method: 'POST', body: JSON.stringify(body) });
    view.report = data.report;
    view.code = data.code || '';
    view.budget = data.budget || view.budget;
    renderAll(Boolean(data.cached));
    host().log?.(`报告 ${data.report.verdict} · ${data.report.findings.length} findings · ${data.cached ? '缓存命中' : '新分析'}`, 'ok');
    await autoSummary(data.report.digest);
  } catch (err) {
    byId('an-cache-badge').textContent = '失败';
    byId('an-cache-badge').className = 'oc-badge bad';
    host().warn?.(`分析失败: ${err.message}`);
  } finally {
    view.busy = false;
    byId('an-run').removeAttribute('disabled');
  }
}

async function reanalyze() {
  try {
    const data = await api('/api/analyze/cache', { method: 'DELETE' });
    host().log?.(`缓存已清空 (${data.cleared} 份)`);
  } catch (err) {
    host().warn?.(`清缓存失败: ${err.message}`);
  }
  await analyze();
}

/* ------------------------------------------------------------ render */
function renderAll(cached) {
  renderVerdict(cached);
  renderDims();
  renderAssumptions();
  renderMeasurements();
  renderEvidence();
  renderBudgetLine();
}

function renderVerdict(cached) {
  const report = view.report;
  if (!report) return;
  const [label, mark] = VERDICT_TEXT[report.verdict] || VERDICT_TEXT.unknown;
  const badge = byId('an-verdict-badge');
  badge.textContent = mark;
  badge.className = `an-verdict-badge ${report.verdict}`;
  byId('an-verdict-title').textContent = `${label} · ${report.source.name || report.source.kind}`;
  const meta = [];
  meta.push(report.verdict_reason);
  if (report.unknown_count) meta.push(`${report.unknown_count} 项 unknown`);
  if (cached) meta.push('缓存命中');
  byId('an-verdict-reason').textContent = meta.join(' · ');

  const memory = findingsOf('memory').find((f) => f.key === 'memory.peak_bytes');
  const complexity = findingsOf('complexity').find((f) => f.key === 'complexity.estimated_order');
  const style = findingsOf('style').find((f) => f.key === 'style.issue_count');
  const perf = findingsOf('performance').filter((f) => f.verdict !== 'green' && f.verdict !== 'unknown');
  const overview = byId('an-overview');
  overview.replaceChildren();
  [
    ['峰值内存', memory && memory.value != null ? fmtBytes(memory.value) : '未知'],
    ['复杂度', complexity?.value ?? '未知'],
    ['风格问题', style?.value ?? '—'],
    ['性能提示', perf.length],
  ].forEach(([labelText, value]) => {
    const cell = document.createElement('div');
    const strong = document.createElement('b');
    const caption = document.createElement('span');
    strong.textContent = String(value);
    caption.textContent = labelText;
    cell.append(strong, caption);
    overview.appendChild(cell);
  });

  const cacheBadge = byId('an-cache-badge');
  cacheBadge.textContent = cached ? '缓存命中' : '新分析';
  cacheBadge.className = `oc-badge ${cached ? '' : 'ok'}`;
}

function renderDims() {
  const wrap = byId('an-dims');
  wrap.replaceChildren();
  DIM_ORDER.forEach((dimension) => {
    const items = findingsOf(dimension);
    const worst = ['red', 'yellow', 'green', 'unknown'].find((v) => items.some((f) => f.verdict === v)) || 'unknown';
    const [title, subtitle] = DIM_META[dimension];
    const details = document.createElement('details');
    details.className = 'an-dim-card';
    details.open = true;
    const summary = document.createElement('summary');
    summary.innerHTML = `<span class="an-dim-dot ${worst}"></span><span>${title}</span><small>${subtitle}</small>`;
    const body = document.createElement('div');
    body.className = 'an-dim-body';
    if (!items.length) {
      const empty = document.createElement('div');
      empty.className = 'an-empty';
      empty.textContent = '无发现';
      body.appendChild(empty);
    } else {
      const table = document.createElement('table');
      table.className = 'an-table';
      table.innerHTML = '<thead><tr><th>指标</th><th>判定</th><th>值</th><th>依据</th></tr></thead>';
      const tbody = document.createElement('tbody');
      items.forEach((f) => {
        const row = document.createElement('tr');
        row.innerHTML =
          `<td>${f.key}</td>` +
          `<td class="an-verdict-cell ${f.verdict}">${f.verdict}<br><span class="an-conf ${f.confidence}">${f.confidence}</span></td>` +
          `<td class="an-v">${fmtValue(f)}<br>${evidenceButtons(f)}</td>` +
          `<td class="an-r">${f.reason}</td>`;
        tbody.appendChild(row);
      });
      table.appendChild(tbody);
      body.appendChild(table);
    }
    details.append(summary, body);
    wrap.appendChild(details);
  });
}

function renderAssumptions() {
  const wrap = byId('an-assumptions');
  wrap.replaceChildren();
  const entries = view.report?.assumptions || [];
  if (!entries.length) {
    wrap.classList.add('is-hidden');
    return;
  }
  wrap.classList.remove('is-hidden');
  entries.forEach((entry) => {
    const chip = document.createElement('span');
    chip.innerHTML = `${entry.key}=<b>${entry.value}</b> · ${entry.source}`;
    wrap.appendChild(chip);
  });
}

function renderMeasurements() {
  const measure = view.report?.measurements || {};
  const badge = byId('an-measure-badge');
  const kv = byId('an-measure-kv');
  const log = byId('an-measure-log');
  kv.replaceChildren();
  log.textContent = '';

  const addRow = (key, value) => {
    const dt = document.createElement('dt');
    const dd = document.createElement('dd');
    dt.textContent = key;
    dd.textContent = String(value);
    kv.append(dt, dd);
  };

  if (measure.status === 'ok') {
    badge.textContent = '实测完成';
    badge.className = 'oc-badge ok';
    addRow('编译耗时', `${measure.compile_ms} ms`);
    addRow('运行耗时', `${measure.wall_ms} ms`);
    addRow('峰值内存', measure.peak_bytes ? fmtBytes(measure.peak_bytes) : '未采样到');
    addRow('退出码', measure.exit_code);
    if (measure.segments?.status === 'ok') {
      addRow('text/data/bss', `${measure.segments.text} / ${measure.segments.data} / ${measure.segments.bss} B`);
    }
    addRow('编译器', measure.compiler);
  } else if (measure.status === 'timeout') {
    badge.textContent = '运行超时';
    badge.className = 'oc-badge bad';
    addRow('说明', measure.reason || 'wall timeout');
    addRow('编译耗时', `${measure.compile_ms} ms`);
  } else if (measure.status === 'failed') {
    badge.textContent = '实测失败';
    badge.className = 'oc-badge bad';
    addRow('阶段', measure.phase || '—');
    addRow('原因', measure.reason || '—');
  } else {
    badge.textContent = measure.status === 'skipped' ? '未启用实测' : '—';
    badge.className = 'oc-badge';
    addRow('说明', measure.reason || '勾选「实测」后重新分析');
  }
  if (measure.stdout) log.textContent += `stdout:\n${measure.stdout}\n`;
  if (measure.stderr) log.textContent += `stderr:\n${measure.stderr}\n`;
  if (!log.textContent) log.textContent = '（无输出）';
}

async function autoSummary(digest) {
  byId('an-ai-badge').textContent = '汇总中…';
  byId('an-ai-badge').className = 'oc-badge info';
  try {
    const data = await api('/api/analyze/summary', { method: 'POST', body: JSON.stringify({ digest }) });
    renderAi(data.ai_summary);
  } catch (err) {
    renderAi({ status: 'failed', reason: err.message, text: '', budget: view.budget });
  }
}

function renderAi(summary) {
  const badge = byId('an-ai-badge');
  const list = byId('an-ai-list');
  list.replaceChildren();
  if (summary.status === 'ok' || summary.status === 'cached') {
    const lines = (summary.text || '').split('\n').map((s) => s.trim()).filter(Boolean);
    lines.forEach((line) => {
      const li = document.createElement('li');
      li.textContent = line.replace(/^[\d.、\-)\s]+/, '');
      list.appendChild(li);
    });
    badge.textContent = summary.status === 'cached' ? '缓存汇总' : 'AI 汇总';
    badge.className = 'oc-badge ok';
  } else {
    const empty = document.createElement('li');
    empty.className = 'an-empty';
    empty.style.cssText = 'list-style:none;margin-left:-18px;';
    empty.textContent = {
      disabled: 'AI 汇总已关闭（仅本地报告）',
      skipped: '没有 API Key，已跳过 AI 汇总',
      budget_exhausted: 'AI 额度已耗尽',
      pending: '等待汇总…',
    }[summary.status] || `AI 汇总不可用（${summary.reason || summary.status}）`;
    list.appendChild(empty);
    badge.textContent = summary.status === 'disabled' ? '已关闭' : summary.status === 'skipped' ? '已跳过' : summary.status === 'budget_exhausted' ? '额度耗尽' : '失败';
    badge.className = 'oc-badge';
  }
  if (summary.budget) view.budget = summary.budget;
  renderBudgetLine();
}

function renderEvidence() {
  const report = view.report;
  const code = view.code || '';
  const pre = byId('an-code');
  const badge = byId('an-evidence-badge');
  if (!code) {
    pre.innerHTML = '<span class="oc-line"><span class="oc-lc">（无代码）</span></span>';
    badge.textContent = '';
    return;
  }
  const hits = new Set();
  (report?.findings || []).forEach((f) => f.evidence.forEach((n) => hits.add(n)));
  pre.innerHTML = code.split('\n').map((line, index) => {
    const hit = hits.has(index + 1) ? ' is-hit' : '';
    return `<span class="oc-line${hit}" data-ln="${index + 1}"><i class="oc-ln">${index + 1}</i><span class="oc-lc">${host().highlightCpp?.(line) || ' '}</span></span>`;
  }).join('');
  badge.textContent = hits.size ? `${hits.size} 行命中` : '';
  badge.className = `oc-badge ${hits.size ? 'info' : ''}`;
}

function gotoLine(lineNumber) {
  const line = byId('an-code')?.querySelector(`.oc-line[data-ln="${lineNumber}"]`);
  if (!line) return;
  line.scrollIntoView({ block: 'center', behavior: 'smooth' });
  line.classList.add('is-flash');
  setTimeout(() => line.classList.remove('is-flash'), 900);
}

/* ------------------------------------------------------------ wiring */
function init() {
  byId('an-source').addEventListener('change', (event) => {
    const kind = event.target.value;
    byId('an-file').classList.toggle('is-hidden', kind !== 'workspace');
    byId('an-path-wrap').classList.toggle('is-hidden', kind !== 'path');
    byId('an-stdin-row').classList.toggle('is-hidden', kind === 'output' && !byId('an-measure').checked);
  });
  byId('an-measure').addEventListener('change', (event) => {
    byId('an-stdin-row').classList.toggle('is-hidden', !event.target.checked && byId('an-source').value !== 'workspace');
  });
  byId('an-run').addEventListener('click', analyze);
  byId('an-rerun').addEventListener('click', reanalyze);
  byId('an-ai-toggle').addEventListener('click', async () => {
    try {
      const data = await api('/api/analyze/enabled', {
        method: 'POST',
        body: JSON.stringify({ enabled: !(view.budget?.enabled ?? true) }),
      });
      view.budget = data;
      renderBudgetLine();
      host().log?.(`AI 汇总 ${data.enabled ? '开启' : '关闭'}`);
    } catch (err) {
      host().warn?.(`切换 AI 失败: ${err.message}`);
    }
  });
  byId('an-cache-clear').addEventListener('click', reanalyze);
  byId('an-dims').addEventListener('click', (event) => {
    const button = event.target.closest('.an-evbtn');
    if (button) gotoLine(Number(button.dataset.ln));
  });
  byId('an-path').addEventListener('keydown', (event) => {
    if (event.key === 'Enter') analyze();
  });
  window.addEventListener('oi-analyze-current', () => {
    const source = byId('an-source');
    if (source.value !== 'output') {
      source.value = 'output';
      source.dispatchEvent(new Event('change'));
    }
    location.hash = 'analysis';
    analyze();
  });

  refreshFiles();
  refreshBudget();
  renderMeasurements();
}

init();
