/* SPDX-License-Identifier: MIT
 * Preview application shell: hash router, global controls, API client, event log.
 * No framework, no build step — native ES modules served by preview_server.py.
 */

import { VIEWS, VIEW_ORDER, VIEW_META } from './views.js';
import '../components/ui-button/ui-button.js';
import { animate, stagger } from '../05-motion/effects.js';

const configEl = document.getElementById('pv-config');
const config = (() => { try { return JSON.parse(configEl?.textContent || '{}'); } catch { return {}; } })();
const root = document.documentElement;
const $ = (sel) => document.querySelector(sel);

const state = {
  theme: root.dataset.theme || config.theme || 'blue',
  lang: root.lang || config.lang || 'en',
  rate: 1,
  reduce: false,
  i18n: {},
  distFiles: config.files || [],
  server: false,
};

/* ---------------- event log ---------------- */
const logList = $('#pv-log-list');
const logCount = $('#pv-log-count');
let logTotal = 0;
function log(text, level = 'info') {
  logTotal += 1;
  logCount.textContent = String(logTotal);
  if (!logList) return;
  const li = document.createElement('li');
  li.className = level;
  li.textContent = `${new Date().toLocaleTimeString()} · ${text}`;
  logList.prepend(li);
  while (logList.children.length > 120) logList.lastElementChild.remove();
}
const logPanel = $('#pv-log-panel');
function toggleLogPanel(force) {
  if (!logPanel) return;
  const open = typeof force === 'boolean' ? force : !logPanel.classList.contains('is-open');
  logPanel.classList.toggle('is-open', open);
}
$('#pv-log-toggle')?.addEventListener('click', () => toggleLogPanel());
$('#pv-log-close')?.addEventListener('click', () => toggleLogPanel(false));
$('#pv-log-clear')?.addEventListener('click', () => { logList.replaceChildren(); logTotal = 0; logCount.textContent = '0'; });

/* ---------------- API ---------------- */
async function api(path, fallbackPath) {
  try {
    const response = await fetch(`/api/${path}`, { cache: 'no-store' });
    if (response.ok) return await response.json();
  } catch { /* static mode */ }
  if (fallbackPath) {
    try {
      const response = await fetch(fallbackPath, { cache: 'no-store' });
      if (response.ok) return await response.json();
    } catch { /* ignore */ }
  }
  return null;
}

async function loadI18n(code) {
  const pack = await api(`i18n/${code}`, `i18n/${code}.json`);
  state.i18n = pack || {};
  globalThis.UIReference = { i18n: state.i18n };
  return pack;
}

/* ---------------- global controls ---------------- */
function applyMotionScale() {
  root.style.setProperty('--motion-scale', state.reduce ? '0' : String(1 / state.rate));
}
function setTheme(name) {
  state.theme = name;
  root.dataset.theme = name;
  const select = $('#pv-theme');
  if (select) select.value = name;
  window.dispatchEvent(new CustomEvent('pv-theme-change', { detail: { theme: name } }));
  log(`theme → ${name}`);
}
function setLang(code) {
  state.lang = code;
  root.lang = code;
  const select = $('#pv-lang');
  if (select) select.value = code;
  loadI18n(code).then((pack) => log(`language → ${code} (${Object.keys(pack || {}).length} keys)`));
}
function setReduce(value) {
  state.reduce = Boolean(value);
  root.dataset.reduceMotion = state.reduce ? 'true' : 'false';
  applyMotionScale();
  const button = $('#pv-reduce');
  if (button) {
    button.setAttribute('label', `Reduced motion: ${state.reduce ? 'on' : 'off'}`);
    button.dataset.active = String(state.reduce);
  }
  log(`reduced motion → ${state.reduce ? 'on' : 'off'}`);
}
function setRate(value) {
  state.rate = Number(value) || 1;
  applyMotionScale();
  log(`speed → ${state.rate}×`);
}

$('#pv-theme')?.addEventListener('change', (e) => setTheme(e.target.value));
$('#pv-lang')?.addEventListener('change', (e) => setLang(e.target.value));
$('#pv-rate')?.addEventListener('change', (e) => setRate(e.target.value));
$('#pv-reduce')?.addEventListener('click', () => setReduce(!state.reduce));

/* ---------------- router ---------------- */
const nav = $('#pv-nav');
const viewHost = $('#pv-view');
let cleanup = null;

function buildNav() {
  nav.innerHTML = VIEW_ORDER.map((id, index) =>
    `<a href="#/${id}" data-view="${id}"><span class="pv-nav-idx">${String(index + 1).padStart(2, '0')}</span>${VIEW_META[id].title}</a>`).join('');
}

/** 卡片入场：stagger 库 + animate 库；时长/步长按 rate 预缩放（motionScale 只认 reduce 开关）。 */
function staggerEntrance(host) {
  if (state.reduce) return;
  const cards = host.querySelectorAll('.pv-card');
  if (!cards.length) return;
  const scale = 1 / state.rate;
  stagger(cards, (el, index) => animate(el, [
    { opacity: 0, transform: 'translateY(10px) scale(0.985)' },
    { opacity: 1, transform: 'none' },
  ], {
    duration: Math.max(1, Math.round(300 * scale)),
    delay: Math.round(40 * scale) * index,
    fill: 'both',
  }), { step: Math.round(40 * scale) });
}

async function renderRoute() {
  const id = (location.hash.replace(/^#\/?/, '') || 'overview').split(/[?&]/)[0];
  const view = VIEWS[id] ? id : 'overview';
  if (typeof cleanup === 'function') { try { cleanup(); } catch (e) { log(`cleanup: ${e.message}`, 'warn'); } }
  cleanup = null;
  viewHost.replaceChildren();
  $('#pv-view-title').textContent = VIEW_META[view].title;
  $('#pv-view-desc').textContent = VIEW_META[view].desc;
  for (const link of nav.querySelectorAll('[data-view]')) link.classList.toggle('is-active', link.dataset.view === view);
  $('#pv-foot-left').textContent = `view: ${view}`;
  try {
    cleanup = await VIEWS[view].mount(viewHost, ctx) || null;
    staggerEntrance(viewHost);
    log(`view → ${view}`);
  } catch (error) {
    console.error(`[preview] view ${view} failed`, error);
    log(`view ${view} failed: ${error.message}`, 'error');
    viewHost.innerHTML = `<section class="pv-card"><h2>视图加载失败</h2><p class="pv-code"></p></section>`;
    viewHost.querySelector('.pv-code').textContent = String(error.stack || error);
  }
}

window.addEventListener('hashchange', renderRoute);
window.addEventListener('keydown', (event) => {
  if (event.target instanceof HTMLInputElement || event.target instanceof HTMLSelectElement) return;
  const index = Number(event.key);
  if (index >= 1 && index <= VIEW_ORDER.length) location.hash = `#/${VIEW_ORDER[index - 1]}`;
  if (event.key.toLowerCase() === 'l') toggleLogPanel();
  if (event.key.toLowerCase() === 'r') setReduce(!state.reduce);
});

/* ---------------- global component events ---------------- */
const EVENT_LABELS = {
  'ui-press': (e) => `ui-press: ${e.detail?.label || ''}`,
  'ui-menu-press': (e) => `menu press: ${e.detail?.id}`,
  'ui-menu-state': (e) => `menu state → ${e.detail?.state}`,
  'ui-overlay-open': (e) => `overlay open: ${e.detail?.variant}`,
  'ui-overlay-close': (e) => `overlay close: ${e.detail?.variant}`,
  'ui-entrance-step': (e) => `entrance step: ${e.detail?.step?.action} @ ${e.detail?.step?.at}ms`,
  'ui-status-pulse': (e) => `status-pulse: ${e.detail?.type}`,
  'ui-select': (e) => `panel select: ${e.detail?.selected}`,
  'pv-log': (e) => String(e.detail?.text || ''),
};
for (const [name, format] of Object.entries(EVENT_LABELS)) {
  document.addEventListener(name, (event) => log(format(event), event.detail?.level));
}

/* ---------------- shared context for views ---------------- */
const ctx = {
  state, config, api, log, setTheme, setLang, setReduce, setRate,
  async json(name, fallback) { return api(name, fallback); },
  async text(path) {
    try { const r = await fetch(path, { cache: 'no-store' }); return r.ok ? await r.text() : ''; } catch { return ''; }
  },
};

window.PV = ctx;

/* ---------------- boot ---------------- */
async function boot() {
  buildNav();
  applyMotionScale();
  setReduce(false);
  await loadI18n(state.lang);
  $('#pv-foot-right').textContent = `${state.distFiles.length} dist files · hash-route · MIT-derived`;

  api('health').then((health) => {
    state.server = Boolean(health?.ok);
    const dot = $('#pv-server-dot');
    const status = $('#pv-server-status');
    const note = $('#pv-api-note');
    if (state.server) {
      dot.className = 'pv-dot ok';
      status.textContent = `preview server · :${health.port || ''} · ${health.files || 0} files`;
      note.textContent = `${health.endpoints?.length || 0} API endpoints online`;
      log(`preview server online (:${health.port})`);
    } else {
      dot.className = 'pv-dot warn';
      status.textContent = 'static mode (no API)';
      note.textContent = 'run preview_server.py for /api/*';
      log('no preview server; using static fallbacks', 'warn');
    }
  });

  await renderRoute();
}

boot();
