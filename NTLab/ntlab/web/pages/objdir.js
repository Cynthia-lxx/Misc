/* ObjDir — NT object-manager namespace browser: lazy tree (left) + detail (right). */

import { apiGet } from "../api.js";

export async function mount(el) {
  el.innerHTML = `
    <div class="page-head">
      <h1>ObjDir</h1>
      <p>NT 对象管理器名字空间。点击目录节点懒加载展开；右侧显示对象详情。全程只读。</p>
    </div>
    <div class="objdir">
      <div class="panel objdir-left"><div class="loading-row"><ui-spinner></ui-spinner> 正在探测根目录…</div></div>
      <div class="objdir-right"><div class="panel" id="obj-detail">选择左侧一个对象查看详情。</div></div>
    </div>`;

  const left = el.querySelector(".objdir-left");
  const detail = el.querySelector("#obj-detail");

  let roots;
  try {
    roots = await apiGet("/api/ntobj/roots");
  } catch (err) {
    left.innerHTML = `<div class="error-box">${esc(err.message)}</div>`;
    return;
  }

  const tree = document.createElement("ui-tree");
  tree.data = roots.roots.map((r) => ({
    id: r.path,
    label: r.path,
    expandable: r.accessible,
    badge: r.accessible ? "目录" : "受限",
    badgeClass: r.accessible ? "directory" : "",
    accessible: r.accessible,
  }));
  tree.loadChildren = async (node) => {
    if (!node.accessible) return [];
    const r = await apiGet("/api/ntobj/list", { path: node.id });
    return r.entries.map((e) => ({
      id: joinPath(node.id, e.name),
      label: e.name,
      type: e.type,
      expandable: e.is_directory,
      badge: e.is_symlink ? "链接" : (e.is_directory ? "目录" : e.type),
      badgeClass: e.is_symlink ? "symlink" : (e.is_directory ? "directory" : "other"),
    }));
  };
  tree.addEventListener("ui-tree-select", async (e) => {
    const node = e.detail.node;
    await showDetail(node);
  });
  left.replaceChildren(tree);

  async function showDetail(node) {
    detail.replaceChildren(spinnerRow());
    try {
      const info = await apiGet("/api/ntobj/object", { path: node.id, type: node.type ?? "" });
      const wrap = document.createElement("div");
      const head = document.createElement("div");
      head.style.cssText = "display:flex;align-items:center;gap:10px;margin-bottom:14px";
      const h2 = document.createElement("h2");
      h2.textContent = node.id;
      h2.style.cssText = "margin:0;font-size:18px;word-break:break-all;flex:1";
      const badge = document.createElement("span");
      badge.className = `badge t-${(node.type ?? "other").toLowerCase().startsWith("dir") ? "directory"
        : (node.type ?? "").toLowerCase() === "symboliclink" ? "symlink" : "other"}`;
      badge.textContent = node.type ?? "对象";
      head.append(badge, h2);
      wrap.appendChild(head);

      const body = document.createElement("div");
      if ((node.type ?? "") === "SymbolicLink") {
        body.innerHTML = info.target
          ? `<div class="panel"><div class="kv"><dt>链接目标</dt><dd>${esc(info.target)}</dd></div></div>`
          : `<div class="error-box">无法读取链接目标：${esc(info.target_error?.message ?? "未知错误")}</div>`;
      } else if (node.expandable) {
        body.innerHTML = `<div class="loading-row"><ui-spinner></ui-spinner> 正在枚举子对象…</div>`;
        try {
          const r = await apiGet("/api/ntobj/list", { path: node.id });
          const table = document.createElement("ui-table");
          table.columns = [
            { key: "name", label: "对象名", type: "mono" },
            { key: "type", label: "类型" },
            { key: "kind", label: "种类" },
          ];
          table.rows = r.entries.map((e2) => ({
            ...e2,
            kind: e2.is_directory ? "可展开" : e2.is_symlink ? "符号链接" : "叶子",
          }));
          table.addEventListener("ui-row-activate", (ev) => {
            const row = ev.detail.row;
            if (row.is_directory || row.is_symlink) {
              // navigate by selecting in tree is complex; jump detail directly
              showDetail({ id: joinPath(node.id, row.name), type: row.type, expandable: row.is_directory });
            }
          });
          body.replaceChildren(table);
        } catch (err) {
          body.innerHTML = `<div class="error-box"><span class="code">${esc(err.detail?.status ?? "")}</span> ${esc(err.message)}</div>`;
        }
      } else {
        body.innerHTML = `<div class="panel"><div class="kv"><dt>类型</dt><dd>${esc(node.type ?? "未知")}</dd>
          <dt>路径</dt><dd>${esc(node.id)}</dd></div></div>`;
      }
      wrap.appendChild(body);
      detail.replaceChildren(wrap);
    } catch (err) {
      detail.innerHTML = `<div class="error-box"><span class="code">${esc(err.detail?.status ?? err.code ?? "")}</span> ${esc(err.message)}</div>`;
    }
  }
}

function joinPath(base, name) {
  if (base === "\\") return "\\" + name;
  return base + "\\" + name;
}

function spinnerRow() {
  const d = document.createElement("div");
  d.className = "loading-row";
  d.innerHTML = `<ui-spinner></ui-spinner> 加载中…`;
  return d;
}

function esc(s) {
  return String(s).replace(/[&<>"']/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;", "'": "&#39;",
  }[c]));
}
