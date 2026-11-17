/* i18n — 轻量双语（zh 默认 / en）。
 *
 * 约定与 vendor 组件一致：
 * - globalThis.UIReference.i18n = 当前词典（组件按 data-i18n-key 自解析）
 * - document 派发 "ui-i18n-change"（组件监听并重解析标签）
 * - 应用侧用 t(key) 取词、applyDom() 刷新 [data-i18n-key] 文本
 * - 语言持久化 localStorage("ntlab-lang")，切换即时生效、刷新保持
 */

const DICTS = {
  zh: {
    "nav.home": "首页",
    "nav.pe": "PE 分析器",
    "nav.objdir": "ObjDir",
    "nav.resedit": "资源修改器",
    "nav.playground": "试验台",
    "nav.debug": "调试",

    "home.hero.title": "Windows 系统底层实验室",
    "home.hero.sub": "解析 PE 文件的头部、节表、导入导出、资源与签名；浏览 NT 对象管理器命名空间；白名单内试调 Win32/NT API。",
    "home.card.pe.title": "PE 分析器",
    "home.card.pe.desc": "解析任意 exe / dll / sys：DOS 头、节表、导入导出表、资源树、Authenticode 签名与 PDB 路径，附带反汇编与字符串搜索。",
    "home.card.resedit.title": "资源修改器",
    "home.card.resedit.desc": "Resource Hacker 式的 PE 资源编辑：树形浏览（类型 → 名称 → 语言），文本 / Hex / 图像预览即编辑，保存时原地备份或另存重建。",
    "home.card.objdir.title": "ObjDir — NT 命名空间浏览器",
    "home.card.objdir.desc": "遍历对象管理器的目录树：\\Device、\\BaseNamedObjects、\\KernelObjects 等目录下的设备、互斥体、事件与符号链接。",
    "home.card.playground.title": "WinAPI 试验台",
    "home.card.playground.desc": "白名单内执行 Win32/NT API 调用：队列编排、结果引用、NTSTATUS 与 GetLastError 渲染，以及 x86-64 汇编组装。",
    "home.go": "进入 →",
    "home.dbg.entry": "调试 — 服务器端口、终端彩色输出、程序终端开关等运行参数",

    "dbg.title": "调试",
    "dbg.sub": "客户端 / 服务器运行参数。内嵌客户端内保存即热应用；浏览器模式重启 NTLab 后生效。",
    "dbg.port": "服务器端口",
    "dbg.port.listening": "当前监听 {port}",
    "dbg.console": "程序终端",
    "dbg.console.note": "开 = python.exe 带控制台；关 = pythonw.exe 无终端静默运行（切换时程序会自动重启）",
    "dbg.browser": "自动打开浏览器",
    "dbg.browser.note": "仅纯服务器模式（main.py --server）启动时生效；内嵌客户端忽略",
    "dbg.mirror": "终端输出镜像",
    "dbg.mirror.note": "在下方以等宽代码字体实时显示服务端日志（颜色与终端一致）",
    "dbg.save": "保存并应用",
    "dbg.term.head": "服务输出 · 实时 · 回放最近 200 行",
    "dbg.info.console": "控制台",
    "dbg.info.attached": "已附加",
    "dbg.info.detached": "未附加",
    "dbg.info.mode": "运行形态",
    "dbg.info.client": "内嵌客户端",
    "dbg.info.server": "浏览器/服务器",
    "dbg.info.settingsfile": "设置文件",
    "dbg.saved.browser": "已保存——浏览器模式下重启 NTLab 后生效",
    "dbg.saved.applying": "已保存，客户端热应用中…",
    "dbg.readfail": "读取失败：{err}（若为 404：当前服务是旧版本，重启 NTLab 后即可）",
    "dbg.savefail": "保存失败：{err}",
    "dbg.savefail404": "（当前服务是旧版本代码，重启 NTLab 即可）",
    "dbg.port.bad": "端口必须是 1-65535 的整数",
    "dbg.on": "开",
    "dbg.off": "关",
    "dbg.live": "实时",
  },

  en: {
    "nav.home": "Home",
    "nav.pe": "PE Analyzer",
    "nav.objdir": "ObjDir",
    "nav.resedit": "Resource Editor",
    "nav.playground": "Playground",
    "nav.debug": "Debug",

    "home.hero.title": "Windows Internals Laboratory",
    "home.hero.sub": "Inspect PE headers, sections, imports/exports, resources and signatures; browse the NT object manager namespace; invoke Win32/NT APIs inside a whitelist.",
    "home.card.pe.title": "PE Analyzer",
    "home.card.pe.desc": "Analyze any exe / dll / sys: DOS header, sections, import/export tables, resource tree, Authenticode signature and PDB path — with disassembly and string search.",
    "home.card.resedit.title": "Resource Editor",
    "home.card.resedit.desc": "Resource Hacker style PE resource editing: tree browsing (type → name → language), text / hex / image preview-is-editor, in-place backup or rebuild on save.",
    "home.card.objdir.title": "ObjDir — NT Namespace Browser",
    "home.card.objdir.desc": "Walk the object manager directory tree: devices, mutexes, events and symbolic links under \\Device, \\BaseNamedObjects, \\KernelObjects and more.",
    "home.card.playground.title": "WinAPI Playground",
    "home.card.playground.desc": "Run whitelisted Win32/NT API calls: call queues, result references, NTSTATUS & GetLastError rendering, plus x86-64 assembly.",
    "home.go": "Open →",
    "home.dbg.entry": "Debug — server port, terminal mirror and other runtime options",

    "dbg.title": "Debug",
    "dbg.sub": "Client / server runtime options. Applied hot inside the desktop client; restart NTLab in browser mode.",
    "dbg.port": "Server port",
    "dbg.port.listening": "listening on {port}",
    "dbg.console": "Program console",
    "dbg.console.note": "On = python.exe with console; Off = pythonw.exe silent (the app restarts itself when toggled)",
    "dbg.browser": "Auto-open browser",
    "dbg.browser.note": "Plain server mode (main.py --server) only; ignored by the desktop client",
    "dbg.mirror": "Terminal mirror",
    "dbg.mirror.note": "Live server log below, monospace and colored exactly like the terminal",
    "dbg.save": "Save & apply",
    "dbg.term.head": "Server output · live · last 200 lines replayed",
    "dbg.info.console": "Console",
    "dbg.info.attached": "attached",
    "dbg.info.detached": "not attached",
    "dbg.info.mode": "Mode",
    "dbg.info.client": "desktop client",
    "dbg.info.server": "browser/server",
    "dbg.info.settingsfile": "Settings file",
    "dbg.saved.browser": "Saved — restart NTLab to take effect in browser mode",
    "dbg.saved.applying": "Saved, applying in client…",
    "dbg.readfail": "Load failed: {err} (404 = the running server is an older build; restart NTLab)",
    "dbg.savefail": "Save failed: {err}",
    "dbg.savefail404": " (the running server is an older build; restarting NTLab fixes it)",
    "dbg.port.bad": "Port must be an integer between 1 and 65535",
    "dbg.on": "On",
    "dbg.off": "Off",
    "dbg.live": "live",
  },
};

let lang = localStorage.getItem("ntlab-lang") === "en" ? "en" : "zh";

export function t(key, params) {
  let s = DICTS[lang][key] ?? DICTS.zh[key] ?? key;
  if (params) {
    for (const [k, v] of Object.entries(params)) s = s.replaceAll(`{${k}}`, String(v));
  }
  return s;
}

export function getLang() {
  return lang;
}

/** 切换语言：更新 UIReference 全局词典 → 刷 [data-i18n-key] → 派发组件级刷新事件。 */
export function setLang(next) {
  if (!DICTS[next] || next === lang) return lang;
  lang = next;
  localStorage.setItem("ntlab-lang", lang);
  globalThis.UIReference = { ...(globalThis.UIReference ?? {}), i18n: DICTS[lang] };
  applyDom();
  document.dispatchEvent(new CustomEvent("ui-i18n-change"));
  return lang;
}

/** 把 [data-i18n-key] 元素的 textContent 刷成当前语言。 */
export function applyDom(root = document) {
  root.querySelectorAll("[data-i18n-key]").forEach((el) => {
    el.textContent = t(el.dataset.i18nKey);
  });
}

globalThis.UIReference = { ...(globalThis.UIReference ?? {}), i18n: DICTS[lang] };
