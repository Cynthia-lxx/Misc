/* NTLab SPA bootstrap: import vendor elements, register pages, start router. */

// vendor custom elements (ES modules self-register on import)
import "./vendor/components/ui-button/ui-button.js";
import "./vendor/components/ui-tabs/ui-tabs.js";
import "./vendor/components/ui-textbox/ui-textbox.js";
import "./vendor/components/ui-spinner/ui-spinner.js";
import "./vendor/components/ui-scroll/ui-scroll.js";

// first-party elements
import "./components/ui-tree/ui-tree.js";
import "./components/ui-table/ui-table.js";
import "./components/ui-codeview/ui-codeview.js";

import { apiGet } from "./api.js";
import { current, register, render, start } from "./router.js";
import { applyDom, getLang, setLang, t } from "./i18n.js";
import * as home from "./pages/home.js";
import * as peViewer from "./pages/pe-viewer.js";
import * as objdir from "./pages/objdir.js";
import * as playground from "./pages/playground.js";
import * as resedit from "./pages/resedit.js";
import * as debug from "./pages/debug.js";

register("home", home);
register("pe", peViewer);
register("objdir", objdir);
register("playground", playground);
register("resedit", resedit);
register("debug", debug);

/* ---- 侧栏底部：主题色点 + 语言切换 ---- */
const THEMES = [
  { name: "red", hue: 0 }, { name: "orange", hue: 45 }, { name: "lime", hue: 90 },
  { name: "green", hue: 125 }, { name: "aquamarine", hue: 160 }, { name: "blue", hue: 200 },
  { name: "purple", hue: 255 }, { name: "plum", hue: 320 }, { name: "pink", hue: 333 },
];

function initSidebarControls() {
  const dots = document.getElementById("theme-dots");
  const langBtn = document.getElementById("lang-btn");
  if (!dots || !langBtn) return;

  const sync = () => {
    const cur = document.documentElement.dataset.theme;
    dots.querySelectorAll(".theme-dot").forEach((d) =>
      d.classList.toggle("active", d.dataset.theme === cur));
    langBtn.textContent = getLang() === "zh" ? "EN" : "中";
  };

  THEMES.forEach(({ name, hue }) => {
    const dot = document.createElement("button");
    dot.type = "button";
    dot.className = "theme-dot";
    dot.dataset.theme = name;
    dot.title = name[0].toUpperCase() + name.slice(1);
    dot.style.background = `hsl(${hue} 100% 70%)`;
    dot.addEventListener("click", () => {
      document.documentElement.dataset.theme = name;
      localStorage.setItem("ntlab-theme", name);
      sync();
    });
    dots.appendChild(dot);
  });

  langBtn.addEventListener("click", () => {
    setLang(getLang() === "zh" ? "en" : "zh");
    sync();
  });

  document.addEventListener("ui-i18n-change", sync);
  sync();

  // 语言切换后重挂有译文的页面（home/debug），其他页面暂为中文原文无需重挂
  document.addEventListener("ui-i18n-change", () => {
    const name = current();
    if (name === "home" || name === "debug") render(content);
  });
}

applyDom();
initSidebarControls();

const content = document.getElementById("content");
start(content);

// version info (nav stagger indices are inlined in index.html —
// post-hoc JS assignment changes animation-delay and restarts finished nav animations)
apiGet("/api/system/info").then((info) => {
  const ver = document.getElementById("ver");
  if (ver) ver.textContent = `NTLab v${info.ntlab} · Win build ${info.build}`;
}).catch(() => {});
