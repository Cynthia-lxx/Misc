/* Tiny hash router. Routes map #/xxx → { mount(el), unmount?(el) }. */

const routes = new Map();

export function register(name, page) {
  routes.set(name, page);
}

export function current() {
  const hash = location.hash.replace(/^#\/?/, "");
  return (hash.split("/")[0] || "home").toLowerCase();
}

export async function render(content) {
  const name = current();
  const page = routes.get(name) ?? routes.get("home");
  document.querySelectorAll("#nav a").forEach((a) => {
    a.classList.toggle("active", a.dataset.route === name);
  });
  content.replaceChildren();
  const el = document.createElement("div");
  el.className = "page";
  content.appendChild(el);
  await page.mount(el);
}

export function start(content) {
  window.addEventListener("hashchange", () => render(content));
  return render(content);
}
