/* SPDX-License-Identifier: GPL-3.0-or-later
 * Motion helpers — WAAPI orchestration that mirrors ntlab/web/motion.css.
 * Durations are multiplied by --motion-scale so reduced-motion users get
 * instant swaps with zero JS branching at call sites.
 */

export function motionScale() {
  const v = parseFloat(getComputedStyle(document.documentElement).getPropertyValue("--motion-scale"));
  return Number.isFinite(v) ? v : 1;
}

const dur = (ms) => ms * motionScale();

/** Animate out oldEl, replace with newEl (node or factory), animate in.
 * animationend bubbles — only react to the element's OWN animation. */
function onceOwnAnimation(el, name, cb) {
  const handler = (e) => {
    if (e.target !== el || e.animationName !== name) return;
    el.removeEventListener("animationend", handler);
    cb();
  };
  el.addEventListener("animationend", handler);
}

/** Animate out oldEl, replace with newEl (node or factory), animate in. */
export function swapContent(host, makeNew, { outMs = 120, inMs = 300 } = {}) {
  const old = host.firstElementChild;
  const scale = motionScale();
  const finish = () => {
    const next = typeof makeNew === "function" ? makeNew() : makeNew;
    host.replaceChildren(next);
    if (scale > 0) {
      next.classList.add("swap-in");
      onceOwnAnimation(next, "ntl-swap-in", () => next.classList.remove("swap-in"));
    }
  };
  if (old && scale > 0) {
    old.classList.add("swap-out");
    let done = false;
    onceOwnAnimation(old, "ntl-swap-out", () => { done = true; finish(); });
    // safety net if the animation never fires (e.g. hidden ancestor)
    setTimeout(() => { if (!done) finish(); }, dur(outMs) + 80);
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
  const scale = motionScale();
  el.classList.add("page-entering");
  if (scale === 0) {
    el.classList.remove("page-entering");
    return;
  }
  onceOwnAnimation(el, "ntl-page-in", () => el.classList.remove("page-entering"));
  // safety net
  setTimeout(() => el.classList.remove("page-entering"), dur(220) + 120);
}

/** Exit animation; resolves when done (or immediately under reduced motion). */
export function exitPage(el) {
  return new Promise((resolve) => {
    const scale = motionScale();
    if (!el.firstElementChild || scale === 0) return resolve();
    const inner = el.firstElementChild;
    inner.classList.add("page-exiting");
    let done = false;
    const finish = () => { if (!done) { done = true; resolve(); } };
    onceOwnAnimation(inner, "ntl-page-out", finish);
    setTimeout(finish, dur(180) + 80);
  });
}
