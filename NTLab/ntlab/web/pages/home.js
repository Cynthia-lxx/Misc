/* Home — tool navigation hub.
 *
 * 布局：hero 横幅 → 2×2 主工具卡（PE 分析器 / 资源修改器 / ObjDir / 试验台）
 *      → 底部「客户端与设置」条（调试入口 + 当前运行形态）。
 */

import { apiGet } from "../api.js";

export async function mount(el) {
  el.innerHTML = "";
  const frag = document.createElement("div");
  frag.innerHTML = `
    <div class="hero">
      <div class="glow"></div>
      <h1>Windows 系统底层实验室</h1>
      <p class="sub">解析 PE 文件的头部、节表、导入导出、资源与签名；浏览 NT 对象管理器命名空间；
      白名单内试调 Win32/NT API。</p>
    </div>
    <div class="tool-grid">
      <a class="tool-card" href="#/pe">
        <div class="ico">▤</div>
        <h2>PE 分析器</h2>
        <p>解析任意 exe / dll / sys：DOS 头、节表、导入导出表、资源树、Authenticode 签名与
        PDB 路径，附带反汇编与字符串搜索。</p>
        <div class="go">进入 →</div>
      </a>
      <a class="tool-card" href="#/resedit">
        <div class="ico">◫</div>
        <h2>资源修改器</h2>
        <p>Resource Hacker 式的 PE 资源编辑：树形浏览（类型 → 名称 → 语言），文本 / Hex / 图像
        预览即编辑，保存时原地备份或另存重建。</p>
        <div class="go">进入 →</div>
      </a>
      <a class="tool-card" href="#/objdir">
        <div class="ico">⌘</div>
        <h2>ObjDir — NT 命名空间浏览器</h2>
        <p>遍历对象管理器的目录树：\\Device、\\BaseNamedObjects、\\KernelObjects 等
        目录下的设备、互斥体、事件与符号链接。</p>
        <div class="go">进入 →</div>
      </a>
      <a class="tool-card" href="#/playground">
        <div class="ico">⚡</div>
        <h2>WinAPI 试验台</h2>
        <p>白名单内执行 Win32/NT API 调用：队列编排、结果引用、NTSTATUS 与
        GetLastError 渲染，以及 x86-64 汇编组装。</p>
        <div class="go">进入 →</div>
      </a>
    </div>
    <a class="dbg-entry" href="#/debug">
      <span class="ico">⚙</span>
      <span class="dbg-entry-txt">
        <b>调试</b> — 服务器端口、终端彩色输出、程序终端开关等运行参数
      </span>
      <span class="dbg-entry-state" id="home-dbg-state">…</span>
      <span class="go">打开 →</span>
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
    const mode = data.effective?.is_client ? "内嵌客户端" : "浏览器模式";
    s.textContent = `${mode}${data.effective?.current_port != null ? ` · 端口 ${data.effective.current_port}` : ""}`;
  }).catch(() => {});
}
