/* Tiny hash router with page exit/enter orchestration (motion.js). */

import { enterPage, exitPage } from "./motion.js";

const routes = new Map();

export function register(name, page) {
  routes.set(name, page);
}

export function current() {
  const hash = location.hash.replace(/^#\/?/, "");
  return (hash.split("/")[0] || "home").toLowerCase();
}

let navigating = false;

export async function render(content) {
  const name = current();
  const page = routes.get(name) ?? routes.get("home");
  document.querySelectorAll("#nav a").forEach((a) => {
    a.classList.toggle("active", a.dataset.route === name);
  });
  await exitPage(content);
  content.replaceChildren();
  const el = document.createElement("div");
  // page-entering 在 mount 之前就挂上：page-waiting 暂停动画、page-hidden
  // 锁 opacity:0 —— mount 中的中间态（如 loading spinner）绝不以全亮先被
  // 绘制；起播空窗期（摘 paused → animationstart 之间的延迟）也由
  // page-hidden 继续锁定，enterPage 在 animationstart 时才摘除。
  el.className = `page page--${name} page-entering page-waiting page-hidden`;
  content.appendChild(el);
  await page.mount(el);
  enterPage(el);
}

export function start(content) {
  window.addEventListener("hashchange", () => {
    if (navigating) return;
    navigating = true;
    render(content).finally(() => { navigating = false; });
  });
  return render(content);
}
