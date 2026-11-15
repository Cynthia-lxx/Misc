/* Home — tool navigation hub. */

export async function mount(el) {
  el.innerHTML = "";
  const frag = document.createElement("div");
  frag.innerHTML = `
    <div class="hero">
      <div class="glow"></div>
      <h1>Windows 系统底层实验室</h1>
      <p class="sub">浏览 NT 对象管理器命名空间，解析 PE 文件的头部、节表、导入导出、资源与签名。</p>
    </div>
    <div class="tool-grid">
      <a class="tool-card" href="#/objdir">
        <div class="ico">⌘</div>
        <h2>ObjDir — NT 命名空间浏览器</h2>
        <p>遍历对象管理器的目录树：\\Device、\\BaseNamedObjects、\\KernelObjects 等
        目录下的设备、互斥体、事件与符号链接。</p>
        <div class="go">进入 →</div>
      </a>
      <a class="tool-card" href="#/pe">
        <div class="ico">▤</div>
        <h2>PE 分析器</h2>
        <p>解析任意 exe / dll / sys：DOS 头、节表、导入导出表、资源、Authenticode 签名与
        PDB 路径。</p>
        <div class="go">进入 →</div>
      </a>
      <a class="tool-card" href="#/playground">
        <div class="ico">⚡</div>
        <h2>WinAPI 试验台</h2>
        <p>白名单内执行 Win32/NT API 调用：队列编排、结果引用、NTSTATUS 与
        GetLastError 渲染，以及 x86-64 汇编组装。</p>
        <div class="go">进入 →</div>
      </a>
    </div>`;
  // stagger the tool cards (grid rhythm: 60ms/card)
  const grid = frag.querySelector(".tool-grid");
  grid.classList.add("motion-stagger");
  grid.dataset.stagger = "grid";
  [...grid.children].forEach((card, i) => card.style.setProperty("--ui-index", i));
  el.appendChild(frag);
}
