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
  el.className = `page page--${name}`;
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
