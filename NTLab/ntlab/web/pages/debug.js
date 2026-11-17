/* 调试 — 客户端/服务器运行参数 + 终端实时镜像。
 *
 * 保存后生效路径：
 * - 内嵌客户端（pywebview）：window.pywebview.api.applySettings 热应用
 *   （端口变更 → 重启服务线程；终端开关 → 以 pythonw/python 重启整个程序）
 * - 浏览器模式：设置落盘，重启 NTLab 后生效
 *
 * 终端镜像：SSE(/api/app/logs/stream) 拉取 ANSI 彩色日志行 → 渲染成彩色 span，
 * 等宽代码字体、回放最近 200 行后实时追加。文案走 i18n（语言切换由 app.js 重挂本页）。
 */

import { apiGet, apiSend } from "../api.js";
import { t } from "../i18n.js";

function esc(s) {
  return String(s).replace(/[&<>"]/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;",
  }[c]));
}

const HINT =
  "运行参数：服务器端口、程序终端开关（开=python.exe 带控制台，关=pythonw.exe 无终端静默运行）、" +
  "纯服务器模式下自动开浏览器，以及终端输出的实时彩色镜像。" +
  "内嵌客户端内保存即热应用；浏览器模式下需重启 NTLab。设置持久化于 data/settings.json。";

/* ANSI SGR → CSS 类（uvicorn 调色板：levelprefix / 状态码着色） */
const ANSI_COLOR = {
  30: "c-gray", 31: "c-red", 32: "c-green", 33: "c-yellow", 34: "c-blue",
  35: "c-magenta", 36: "c-cyan", 37: "c-white",
  90: "c-gray", 91: "c-red", 92: "c-green", 93: "c-yellow", 94: "c-blue",
  95: "c-magenta", 96: "c-cyan", 97: "c-white",
};

function ansiToHtml(text) {
  const parts = String(text).split(/\x1b\[([0-9;]*)m/);
  let out = "", color = "", bold = false;
  const wrap = (seg) => {
    const cls = [color, bold ? "c-bold" : ""].filter(Boolean).join(" ");
    return cls ? `<span class="${cls}">${seg}</span>` : seg;
  };
  for (let i = 0; i < parts.length; i++) {
    if (i % 2 === 0) {
      if (parts[i]) out += wrap(esc(parts[i]));
    } else {
      for (const code of parts[i].split(";").filter(Boolean)) {
        if (code === "0") { color = ""; bold = false; }
        else if (code === "1" || code === "22") bold = code === "1";
        else if (code === "39") color = "";
        else if (ANSI_COLOR[code]) color = ANSI_COLOR[code];
      }
    }
  }
  return out;
}

export async function mount(el) {
  // 路由层没有 unmount 钩子：重进页面先关掉上一次的镜像流
  if (window.__dbgES) { window.__dbgES.close(); window.__dbgES = null; }

  el.innerHTML = "";
  const frag = document.createElement("div");
  frag.innerHTML = `
    <div class="page-head">
      <h1>${t("dbg.title")}<span class="page-hint" title="${esc(HINT)}">?</span></h1>
      <p>${t("dbg.sub")}</p>
    </div>
    <div class="panel dbg-panel">
      <div class="dbg-row">
        <div class="dbg-label">${t("dbg.port")}</div>
        <div class="dbg-ctl">
          <input id="dbg-port" class="dbg-input" inputmode="numeric" maxlength="5" spellcheck="false">
          <span class="dbg-note" id="dbg-port-note"></span>
        </div>
      </div>
      <div class="dbg-row">
        <div class="dbg-label">${t("dbg.console")}</div>
        <div class="dbg-ctl">
          <label class="dbg-switch"><input type="checkbox" id="dbg-console"><i></i><span id="dbg-console-txt"></span></label>
          <span class="dbg-note">${t("dbg.console.note")}</span>
        </div>
      </div>
      <div class="dbg-row">
        <div class="dbg-label">${t("dbg.browser")}</div>
        <div class="dbg-ctl">
          <label class="dbg-switch"><input type="checkbox" id="dbg-browser"><i></i><span>${t("dbg.on")}</span></label>
          <span class="dbg-note">${t("dbg.browser.note")}</span>
        </div>
      </div>
      <div class="dbg-row">
        <div class="dbg-label">${t("dbg.mirror")}</div>
        <div class="dbg-ctl">
          <label class="dbg-switch"><input type="checkbox" id="dbg-mirror"><i></i><span>${t("dbg.live")}</span></label>
          <span class="dbg-note">${t("dbg.mirror.note")}</span>
        </div>
      </div>
      <div class="dbg-actions">
        <button id="dbg-save" class="dbg-save">${t("dbg.save")}</button>
        <span id="dbg-msg" class="dbg-msg"></span>
      </div>
      <div class="dbg-info" id="dbg-info"></div>
      <div class="dbg-term-wrap" id="dbg-term-wrap">
        <div class="dbg-term-head">${t("dbg.term.head")}<span class="live-dot"></span></div>
        <div class="dbg-term" id="dbg-term"></div>
      </div>
    </div>`;
  el.appendChild(frag);

  const port = frag.querySelector("#dbg-port");
  const consoleChk = frag.querySelector("#dbg-console");
  const consoleTxt = frag.querySelector("#dbg-console-txt");
  const browser = frag.querySelector("#dbg-browser");
  const mirror = frag.querySelector("#dbg-mirror");
  const portNote = frag.querySelector("#dbg-port-note");
  const msg = frag.querySelector("#dbg-msg");
  const info = frag.querySelector("#dbg-info");
  const saveBtn = frag.querySelector("#dbg-save");
  const termWrap = frag.querySelector("#dbg-term-wrap");
  const term = frag.querySelector("#dbg-term");

  consoleChk.addEventListener("change", () => {
    consoleTxt.textContent = consoleChk.checked ? t("dbg.off") : t("dbg.on");
  });

  function pushLine(text) {
    const div = document.createElement("div");
    div.className = "term-line";
    div.innerHTML = ansiToHtml(text) || "&nbsp;";
    term.appendChild(div);
    while (term.children.length > 800) term.firstChild.remove();
    term.scrollTop = term.scrollHeight;
  }

  function startMirror() {
    if (window.__dbgES) return;
    termWrap.style.display = "";
    const es = new EventSource("/api/app/logs/stream");
    es.onmessage = (e) => {
      try {
        const { text } = JSON.parse(e.data);
        pushLine(text);
      } catch { /* 忽略坏帧 */ }
    };
    es.onerror = () => { /* EventSource 自动重连 */ };
    window.__dbgES = es;
  }

  function stopMirror() {
    if (window.__dbgES) { window.__dbgES.close(); window.__dbgES = null; }
    termWrap.style.display = "none";
  }

  const apply = (data) => {
    const s = data.settings;
    port.value = String(s.port);
    consoleChk.checked = !!s.no_console;
    consoleTxt.textContent = consoleChk.checked ? t("dbg.off") : t("dbg.on");
    browser.checked = !!s.open_browser;
    mirror.checked = s.mirror_terminal !== false;
    portNote.textContent = data.effective?.current_port != null
      ? t("dbg.port.listening", { port: data.effective.current_port }) : "";
    info.innerHTML =
      `${t("dbg.info.console")}：<b>${data.effective?.console_attached
        ? t("dbg.info.attached") : t("dbg.info.detached")}</b>` +
      ` · ${t("dbg.info.mode")}：<b>${data.effective?.is_client
        ? t("dbg.info.client") : t("dbg.info.server")}</b>` +
      ` · ${t("dbg.info.settingsfile")}：<code>data/settings.json</code>`;
    if (mirror.checked) startMirror(); else stopMirror();
  };

  try {
    apply(await apiGet("/api/app/settings"));
  } catch (err) {
    msg.className = "dbg-msg err";
    msg.textContent = t("dbg.readfail", { err: err?.message ?? err });
  }

  async function save() {
    const p = Number(port.value);
    if (!Number.isInteger(p) || p < 1 || p > 65535) {
      msg.className = "dbg-msg err";
      msg.textContent = t("dbg.port.bad");
      return;
    }
    const payload = {
      port: p,
      no_console: consoleChk.checked,
      open_browser: browser.checked,
      mirror_terminal: mirror.checked,
    };
    saveBtn.disabled = true;
    msg.className = "dbg-msg";
    try {
      await apiSend("POST", "/api/app/settings", payload);
      if (!mirror.checked) stopMirror(); else startMirror();
      const api = window.pywebview?.api;
      if (api?.applySettings) {
        msg.textContent = t("dbg.saved.applying");
        const r = await api.applySettings(JSON.stringify(payload));
        msg.className = r?.ok === false ? "dbg-msg err" : "dbg-msg ok";
        msg.textContent = r?.message ?? t("dbg.saved.applying");
      } else {
        msg.className = "dbg-msg ok";
        msg.textContent = t("dbg.saved.browser");
      }
    } catch (err) {
      msg.className = "dbg-msg err";
      msg.textContent = t("dbg.savefail", { err: err?.message ?? err })
        + (/404/.test(err?.message ?? "") ? t("dbg.savefail404") : "");
    } finally {
      saveBtn.disabled = false;
    }
  }

  saveBtn.addEventListener("click", save);
  port.addEventListener("keydown", (e) => { if (e.key === "Enter") save(); });
}
