/* Home — tool navigation hub. */

export async function mount(el) {
  el.innerHTML = "";
  const frag = document.createElement("div");
  frag.innerHTML = `
    <div class="hero">
      <div class="glow"></div>
      <h1>Windows 系统底层实验室</h1>
      <p class="sub">把 ntdll、ntoskrnl、对象管理器这些「看不见的房间」打开给你看。
      全部工具只读、本地运行——这些系统机制本来就该被看见，而不是只存在于逆向工程的故事里。</p>
    </div>
    <div class="tool-grid">
      <a class="tool-card" href="#/objdir">
        <div class="ico">⌘</div>
        <h2>ObjDir — NT 名字空间浏览器</h2>
        <p>遍历对象管理器的目录树：\\Device、\\BaseNamedObjects、\\KernelObjects…看看设备、
        互斥体、事件、符号链接在内核里长什么样，以及谁链接到谁。</p>
        <div class="go">进入 →</div>
      </a>
      <a class="tool-card" href="#/pe">
        <div class="ico">▤</div>
        <h2>PE 解剖器</h2>
        <p>拖入任意 exe / dll / sys：DOS 头、节表、导入导出表、资源、Authenticode 签名与
        PDB 路径——每一块二进制的「骨骼与血管」。</p>
        <div class="go">进入 →</div>
      </a>
      <div class="tool-card" style="opacity:0.55; cursor:default">
        <div class="ico">◆</div>
        <h2>WinAPI Playground</h2>
        <p>白名单内交互式调用 Win32/NT API，实时渲染返回值、GetLastError 与 NTSTATUS。（Phase 2）</p>
        <div class="go" style="color:var(--ui-text-muted)">敬请期待</div>
      </div>
    </div>`;
  el.appendChild(frag);
}
