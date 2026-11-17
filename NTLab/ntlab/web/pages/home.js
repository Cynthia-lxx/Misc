/* Home — tool navigation hub.
 *
 * 布局：hero 横幅 → 2×2 主工具卡（PE 分析器 / 资源修改器 / ObjDir / 试验台）
 *      → 底部「客户端与设置」条（调试入口 + 当前运行形态）。
 * 文案走 i18n 词典；语言切换由 app.js 统一重挂当前页。
 */

import { apiGet } from "../api.js";
import { t } from "../i18n.js";

export async function mount(el) {
  el.innerHTML = "";
  const frag = document.createElement("div");
  frag.innerHTML = `
    <div class="hero">
      <div class="glow"></div>
      <h1>${t("home.hero.title")}</h1>
      <p class="sub">${t("home.hero.sub")}</p>
    </div>
    <div class="tool-grid">
      <a class="tool-card" href="#/pe">
        <div class="ico">▤</div>
        <h2>${t("home.card.pe.title")}</h2>
        <p>${t("home.card.pe.desc")}</p>
        <div class="go">${t("home.go")}</div>
      </a>
      <a class="tool-card" href="#/resedit">
        <div class="ico">◫</div>
        <h2>${t("home.card.resedit.title")}</h2>
        <p>${t("home.card.resedit.desc")}</p>
        <div class="go">${t("home.go")}</div>
      </a>
      <a class="tool-card" href="#/objdir">
        <div class="ico">⌘</div>
        <h2>${t("home.card.objdir.title")}</h2>
        <p>${t("home.card.objdir.desc")}</p>
        <div class="go">${t("home.go")}</div>
      </a>
      <a class="tool-card" href="#/playground">
        <div class="ico">⚡</div>
        <h2>${t("home.card.playground.title")}</h2>
        <p>${t("home.card.playground.desc")}</p>
        <div class="go">${t("home.go")}</div>
      </a>
    </div>
    <a class="dbg-entry" href="#/debug">
      <span class="ico">⚙</span>
      <span class="dbg-entry-txt"><b>${t("nav.debug")}</b> — ${t("home.dbg.entry")}</span>
      <span class="dbg-entry-state" id="home-dbg-state">…</span>
      <span class="go">${t("home.go")}</span>
    </a>`;

  // stagger the tool cards (grid rhythm: 60ms/card)
  const grid = frag.querySelector(".tool-grid");
  grid.classList.add("motion-stagger");
  grid.dataset.stagger = "grid";
  [...grid.children].forEach((card, i) => card.style.setProperty("--ui-index", i));
  el.appendChild(frag);

  // 底部条：显示当前运行形态（内嵌客户端 / 浏览器）与监听端口
  apiGet("/api/app/settings").then((data) => {
    const s = frag.querySelector("#home-dbg-state");
    if (!s) return;
    const mode = data.effective?.is_client ? t("dbg.info.client") : t("dbg.info.server");
    s.textContent = mode + (data.effective?.current_port != null
      ? ` · :${data.effective.current_port}` : "");
  }).catch(() => {});
}
