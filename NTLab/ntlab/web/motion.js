/* SPDX-License-Identifier: GPL-3.0-or-later
 * Motion helpers — WAAPI orchestration that mirrors ntlab/web/motion.css.
 * Durations are multiplied by --motion-scale so reduced-motion users get
 * instant swaps with zero JS branching at call sites.
 */

// ---------------------------------------------------------------- debug
// VERBOSE 探针：localStorage["ntlab-motion-debug"]="0" 可关闭。
// 输出所有 ntl-* 动画生命周期事件、transition 事件与 motion.js 打点。
export const MOTION_DEBUG = localStorage.getItem("ntlab-motion-debug") !== "0";
const T0 = performance.now();
const ts = () => `@${(performance.now() - T0).toFixed(0)}ms`;

function tagOf(el) {
  if (!(el instanceof Element)) return String(el?.constructor?.name ?? el);
  const id = el.id ? `#${el.id}` : "";
  const cls = el.classList.length ? `.${[...el.classList].join(".")}` : "";
  const shadow = el.shadowRoot ? " (shadow-host)" : "";
  return `<${el.tagName.toLowerCase()}${id}${cls}>${shadow}`;
}

if (MOTION_DEBUG) {
  const ANIM_EVENTS = ["animationstart", "animationend", "animationcancel"];
  const TRANS_EVENTS = ["transitionstart", "transitionend", "transitioncancel", "transitionrun"];
  const log = (msg) => console.debug(`[NTLab:motion] ${msg}`);
  log(`instrumentation on ${ts()}`);
  for (const type of ANIM_EVENTS) {
    document.addEventListener(type, (e) => {
      const name = e.animationName ?? "";
      if (String(name).startsWith("ntl-")) log(`${type} "${name}" ← ${tagOf(e.target)} ${ts()}`);
    }, true);
  }
  for (const type of TRANS_EVENTS) {
    document.addEventListener(type, (e) => {
      log(`${type} "${e.propertyName}" ← ${tagOf(e.target)} ${ts()}`);
    }, true);
  }
  // class 变更观察（只看 #content 子树，含旧值）
  const content = document.getElementById("content");
  if (content) {
    new MutationObserver((muts) => {
      for (const m of muts) {
        log(`class ${tagOf(m.target)}: "${m.oldValue}" → "${m.target.getAttribute("class")}" ${ts()}`);
      }
    }).observe(content, { subtree: true, attributes: true, attributeFilter: ["class"], attributeOldValue: true });
  }
  // 布局位移探针：「闪一下」最常见元凶是 layout shift（如滚动条出现挤压内容）
  if (typeof PerformanceObserver !== "undefined") {
    try {
      const po = new PerformanceObserver((list) => {
        for (const entry of list.getEntries()) {
          if (entry.hadRecentInput) continue;
          const sources = (entry.sources ?? []).map((s) => tagOf(s.node)).slice(0, 6).join(", ");
          log(`LAYOUT-SHIFT value=${entry.value.toFixed(4)} ← [${sources || "no-sources"}] ${ts()}`);
        }
      });
      po.observe({ type: "layout-shift", buffered: false });
    } catch { /* unsupported */ }
  }
  // 闪帧视觉采样：入场关键点截图（手动对比用）
  (window).__ntlMotionLog = { log, ts, tagOf };
}

// ---------------------------------------------------------------- helpers

export function motionScale() {
  const v = parseFloat(getComputedStyle(document.documentElement).getPropertyValue("--motion-scale"));
  return Number.isFinite(v) ? v : 1;
}

const dur = (ms) => ms * motionScale();

/** Animate out oldEl, replace with newEl (node or factory), animate in.
 * animationend bubbles — only react to the element's OWN animation. */
function onceOwnAnimation(el, name, cb) {
  const dbg = (msg) => { if (MOTION_DEBUG) (window).__ntlMotionLog?.log?.(`${msg} ${ts()}`); };
  const handler = (e) => {
    if (e.target !== el) {
      dbg(`onceOwn(${name}) IGNORED bubbled "${e.animationName}" from ${tagOf(e.target)} on ${tagOf(el)}`);
      return;
    }
    if (e.animationName !== name) {
      dbg(`onceOwn(${name}) IGNORED own-anim "${e.animationName}" on ${tagOf(el)}`);
      return;
    }
    dbg(`onceOwn(${name}) MATCHED on ${tagOf(el)}`);
    el.removeEventListener("animationend", handler);
    cb();
  };
  el.addEventListener("animationend", handler);
}

/** Animate out oldEl, replace with newEl (node or factory), animate in. */
export function swapContent(host, makeNew, { outMs = 120, inMs = 300 } = {}) {
  const dbg = (msg) => { if (MOTION_DEBUG) (window).__ntlMotionLog?.log?.(`swap[${tagOf(host)}] ${msg} ${ts()}`); };
  const old = host.firstElementChild;
  const scale = motionScale();
  dbg(`start (old=${old ? tagOf(old) : "none"}, scale=${scale})`);
  const finish = () => {
    const next = typeof makeNew === "function" ? makeNew() : makeNew;
    host.replaceChildren(next);
    dbg(`content replaced → ${tagOf(next)}`);
    if (scale > 0) {
      next.classList.add("swap-in");
      onceOwnAnimation(next, "ntl-swap-in", () => {
        next.classList.remove("swap-in");
        dbg("swap-in class removed (own anim done)");
      });
    }
  };
  if (old && scale > 0) {
    old.classList.add("swap-out");
    let done = false;
    onceOwnAnimation(old, "ntl-swap-out", () => { done = true; dbg("swap-out done (animend)"); finish(); });
    // safety net if the animation never fires (e.g. hidden ancestor)
    setTimeout(() => { if (!done) { dbg("swap-out done (safety timeout)"); finish(); } }, dur(outMs) + 80);
  } else {
    finish();
  }
}

/** Assign --ui-index to children so .motion-stagger / ui-table rows can stagger. */
export function assignIndices(container, selector = ":scope > *") {
  container.querySelectorAll(selector).forEach((el, i) => el.style.setProperty("--ui-index", i));
}

/** Entrance for a freshly rendered page. The container only does a subtle
 * fade; each page adds its own flavour via .page--<name> rules in motion.css
 * (per-region directions/rhythms, per motion.json). */
export function enterPage(el) {
  const dbg = (msg) => { if (MOTION_DEBUG) (window).__ntlMotionLog?.log?.(`enterPage ${msg} ${ts()}`); };
  const scale = motionScale();
  // router 已在 mount 前挂好 page-entering(page-waiting + page-hidden)；
  // 这里摘除暂停起播；page-hidden 继续锁 opacity:0 直到动画真正起播，
  // 消灭「摘除 paused → animationstart」空窗期的全亮闪帧。
  if (scale === 0) {
    el.classList.remove("page-entering", "page-waiting", "page-hidden");
    el.closest("#content")?.classList.add("ready");
    dbg("reduced motion → classes removed immediately");
    return;
  }
  dbg(`start page-in on ${tagOf(el)} (scale=${scale}, waiting=${el.classList.contains("page-waiting")})`);
  // 强制一次样式失效，确保 paused→running 从第 0 帧起播
  void el.offsetWidth;
  el.classList.remove("page-waiting");
  // .ready 解除 CSS 层的 #content 预隐藏；此刻 page-hidden 仍锁 opacity 0，
  // 动画起播前不会漏亮（含首帧早于模块执行的场景）
  el.closest("#content")?.classList.add("ready");
  dbg(".ready added (content still locked by page-hidden)");
  onceOwnAnimation(el, "ntl-page-in", () => {
    el.classList.remove("page-entering", "page-hidden");
    dbg("page-in done → page-entering/page-hidden removed");
  });
  // safety net：动画迟迟不起播或没跑完时兜底显形
  setTimeout(() => {
    if (el.classList.contains("page-entering") || el.classList.contains("page-hidden")) {
      el.classList.remove("page-entering", "page-hidden");
      dbg("classes removed (safety timeout)");
    }
  }, dur(220) + 200);
}

/** Exit animation; resolves when done (or immediately under reduced motion). */
export function exitPage(el) {
  return new Promise((resolve) => {
    const dbg = (msg) => { if (MOTION_DEBUG) (window).__ntlMotionLog?.log?.(`exitPage ${msg} ${ts()}`); };
    const scale = motionScale();
    if (!el.firstElementChild || scale === 0) { dbg("skip (empty or reduced)"); return resolve(); }
    const inner = el.firstElementChild;
    inner.classList.add("page-exiting");
    dbg(`add .page-exiting on ${tagOf(inner)}`);
    let done = false;
    const finish = () => { if (!done) { done = true; dbg("exit resolved"); resolve(); } };
    onceOwnAnimation(inner, "ntl-page-out", finish);
    setTimeout(finish, dur(180) + 80);
  });
}
