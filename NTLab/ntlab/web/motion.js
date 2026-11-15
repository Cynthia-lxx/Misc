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

/** Animate out oldEl, replace with newEl (node or factory), animate in. */
export function swapContent(host, makeNew, { outMs = 120, inMs = 300 } = {}) {
  const old = host.firstElementChild;
  const scale = motionScale();
  const finish = () => {
    const next = typeof makeNew === "function" ? makeNew() : makeNew;
    host.replaceChildren(next);
    if (scale > 0) {
      next.classList.add("swap-in");
      next.addEventListener("animationend", () => next.classList.remove("swap-in"), { once: true });
    }
  };
  if (old && scale > 0) {
    old.classList.add("swap-out");
    old.addEventListener("animationend", finish, { once: true });
    // safety net if animationend never fires (e.g. display:none ancestor)
    setTimeout(() => { if (old.isConnected) finish(); }, dur(outMs) + 80);
  } else {
    finish();
  }
}

/** Assign --ui-index to children so .motion-stagger / ui-table rows can stagger. */
export function assignIndices(container, selector = ":scope > *") {
  container.querySelectorAll(selector).forEach((el, i) => el.style.setProperty("--ui-index", i));
}

/** Fade-and-slide entrance for a freshly rendered container (router pages). */
export function enterPage(el) {
  const scale = motionScale();
  el.classList.add("page-entering");
  el.addEventListener("animationend", () => el.classList.remove("page-entering"), { once: true });
  // stagger first-level blocks
  [...el.children].forEach((child, i) => child.style.setProperty("--ui-index", i));
  el.classList.add("motion-stagger");
  if (scale === 0) {
    el.classList.remove("page-entering", "motion-stagger");
  }
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
    inner.addEventListener("animationend", finish, { once: true });
    setTimeout(finish, dur(180) + 80);
  });
}
