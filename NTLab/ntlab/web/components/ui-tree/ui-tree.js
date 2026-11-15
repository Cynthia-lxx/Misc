/* SPDX-License-Identifier: GPL-3.0-or-later
 * ui-tree — lazy-load tree view styled after the vendor ui-* kit.
 *
 * Usage:
 *   const tree = document.createElement('ui-tree');
 *   tree.loadChildren = async (node) => [{ id, label, type, expandable, badge }]
 *   tree.data = [{ id, label, type, expandable, badge }, ...]   // roots
 *   tree.addEventListener('ui-tree-select', (e) => e.detail.node)
 */

const STYLE = `
  :host { display: block; font: inherit; }
  .tree { list-style: none; margin: 0; padding: 0; }
  ul.tree ul { list-style: none; margin: 0; padding: 0 0 0 16px; }
  .row {
    display: flex; align-items: center; gap: 6px;
    padding: 5px 8px; margin: 0 4px; border-radius: 5px;
    cursor: pointer; user-select: none;
    transition: background 100ms ease-out;
  }
  .row:hover { background: color-mix(in srgb, var(--ui-accent) 10%, transparent); }
  .row.selected { background: color-mix(in srgb, var(--ui-accent) 18%, transparent); }
  .twisty {
    width: 14px; flex: 0 0 auto; text-align: center;
    color: var(--ui-text-muted, #888); font-size: 10px;
    transition: transform 150ms cubic-bezier(0.22, 1, 0.36, 1);
  }
  .twisty.leaf { visibility: hidden; }
  .row.open .twisty:not(.leaf) { transform: rotate(90deg); }
  .label {
    flex: 1; min-width: 0; overflow: hidden; text-overflow: ellipsis; white-space: nowrap;
    font-family: "Cascadia Code", Consolas, monospace; font-size: 12.5px;
  }
  .badge {
    flex: 0 0 auto; font-size: 9.5px; font-weight: 700; letter-spacing: 0.03em;
    padding: 1px 6px; border-radius: 3px;
    color: var(--ui-text-muted, #888);
    background: rgba(255,255,255,0.06);
  }
  .badge.directory { color: #66ccff; background: rgba(102, 204, 255, 0.12); }
  .badge.symlink { color: #ff66aa; background: rgba(255, 102, 170, 0.12); }
  .kids.loading { opacity: 0.5; }
  .kids > li {
    animation: tree-row-in calc(240ms * var(--motion-scale, 1)) cubic-bezier(0.16, 1, 0.3, 1) both;
    animation-delay: calc(20ms * var(--ui-index, 0) * var(--motion-scale, 1));
  }
  @keyframes tree-row-in { from { opacity: 0; transform: translateX(-10px); } to { opacity: 1; transform: translateX(0); } }
  .err { color: #ff8899; font-size: 12px; padding: 4px 8px 4px 36px; }
`;

export class UiTreeElement extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this._roots = [];
    /** async (node) => NodeSpec[] */
    this.loadChildren = null;
    this._selectedId = null;
    const style = document.createElement("style");
    style.textContent = STYLE;
    this._ul = document.createElement("ul");
    this._ul.className = "tree";
    this.shadowRoot.append(style, this._ul);
  }

  /** NodeSpec: { id, label, type, badge?, expandable?, children? } */
  set data(nodes) {
    this._roots = nodes.map((n) => ({ ...n, depth: 0 }));
    this._render();
  }

  get selected() {
    return this._selectedId;
  }

  _render() {
    this._ul.replaceChildren();
    for (const node of this._roots) this._ul.appendChild(this._makeNode(node, this._ul));
  }

  _makeNode(node, parentEl) {
    const li = document.createElement("li");
    const row = document.createElement("div");
    row.className = "row";
    row.dataset.id = node.id;
    if (node.id === this._selectedId) row.classList.add("selected");

    const twisty = document.createElement("span");
    twisty.className = "twisty" + (node.expandable ? "" : " leaf");
    twisty.textContent = "▸";

    const label = document.createElement("span");
    label.className = "label";
    label.textContent = node.label;
    if (node.id) label.title = node.id;

    row.append(twisty, label);

    if (node.badge) {
      const badge = document.createElement("span");
      badge.className = "badge " + (node.badgeClass ?? "");
      badge.textContent = node.badge;
      row.appendChild(badge);
    }

    const kids = document.createElement("ul");
    kids.className = "kids";
    li.append(row, kids);

    node._row = row;
    node._twisty = twisty;
    node._kidsEl = kids;
    node._open = false;

    row.addEventListener("click", () => this._onRowClick(node));
    return li;
  }

  async _onRowClick(node) {
    this._select(node);
    if (!node.expandable) return;
    if (node._open) {
      node._open = false;
      node._row.classList.remove("open");
      node._kidsEl.replaceChildren();
      return;
    }
    node._open = true;
    node._row.classList.add("open");
    node._kidsEl.classList.add("loading");
    try {
      const children = this.loadChildren ? await this.loadChildren(node) : [];
      node._kidsEl.classList.remove("loading");
      node._kidsEl.replaceChildren();
      // cap stagger so huge directories (500+ entries) don't feel laggy
      children.forEach((child, i) => { child._staggerIndex = Math.min(i, 24); });
      for (const child of children) {
        const li = this._makeNode(child, node._kidsEl);
        li.style.setProperty("--ui-index", child._staggerIndex);
        node._kidsEl.appendChild(li);
      }
      if (!children.length) {
        const empty = document.createElement("div");
        empty.className = "err";
        empty.style.color = "var(--ui-text-muted, #888)";
        empty.textContent = "（空目录）";
        node._kidsEl.appendChild(empty);
      }
    } catch (err) {
      node._kidsEl.classList.remove("loading");
      const msg = document.createElement("div");
      msg.className = "err";
      msg.textContent = err.message || String(err);
      node._kidsEl.replaceChildren(msg);
    }
  }

  _select(node) {
    this.shadowRoot.querySelectorAll(".row.selected").forEach((r) => r.classList.remove("selected"));
    node._row?.classList.add("selected");
    this._selectedId = node.id;
    this.dispatchEvent(new CustomEvent("ui-tree-select", {
      bubbles: true, composed: true, detail: { node: { ...node } },
    }));
  }
}

if (typeof customElements !== "undefined" && !customElements.get("ui-tree")) {
  customElements.define("ui-tree", UiTreeElement);
}
export default UiTreeElement;
