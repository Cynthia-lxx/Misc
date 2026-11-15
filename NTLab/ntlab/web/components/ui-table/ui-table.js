/* SPDX-License-Identifier: GPL-3.0-or-later
 * ui-table — sortable data table styled after the vendor ui-* kit.
 *
 * Usage:
 *   table.columns = [{ key, label, type: 'text'|'hex'|'num'|'mono', width? }]
 *   table.rows = [{...}, ...]
 *   table.addEventListener('ui-row-activate', e => e.detail.row)
 */

const STYLE = `
  :host { display: block; font: inherit; }
  .wrap { overflow: auto; border: 1px solid var(--ui-border, #2a2a2a); border-radius: 8px; }
  table { border-collapse: collapse; width: 100%; font-size: 12.5px; }
  thead th {
    position: sticky; top: 0; z-index: 1;
    text-align: left; padding: 8px 12px;
    background: var(--ui-surface-raised, #262626);
    color: var(--ui-text-muted, #888);
    font-size: 11px; font-weight: 700; letter-spacing: 0.06em; text-transform: uppercase;
    cursor: pointer; user-select: none;
    border-bottom: 1px solid var(--ui-border, #2a2a2a);
    white-space: nowrap;
  }
  thead th:hover { color: var(--ui-accent); }
  thead th .arrow { margin-left: 5px; color: var(--ui-accent); font-size: 9px; }
  tbody td {
    padding: 6px 12px; border-bottom: 1px solid color-mix(in srgb, var(--ui-border) 55%, transparent);
    white-space: nowrap; max-width: 420px; overflow: hidden; text-overflow: ellipsis;
  }
  tbody tr { transition: background 90ms ease-out; }
  tbody tr:hover { background: color-mix(in srgb, var(--ui-accent) 7%, transparent); }
  td.num, th.num { text-align: right; }
  td.mono, td.hex { font-family: "Cascadia Code", Consolas, monospace; font-size: 12px; }
  td.hex { color: var(--ui-accent-weak, #7fb2cc); }
  td .dim { color: var(--ui-text-muted, #888); }
  .empty { padding: 18px; text-align: center; color: var(--ui-text-muted, #888); font-size: 13px; }
`;

const hex = (v) => (typeof v === "number" && Number.isFinite(v)) ? `0x${v.toString(16).toUpperCase()}` : (v ?? "");
const fmt = (v, type) => type === "hex" ? hex(v) : (v ?? "");

export class UiTableElement extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    this._columns = [];
    this._rows = [];
    this._sortKey = null;
    this._sortDir = 1;
    const style = document.createElement("style");
    style.textContent = STYLE;
    this._wrap = document.createElement("div");
    this._wrap.className = "wrap";
    this.shadowRoot.append(style, this._wrap);
  }

  set columns(cols) { this._columns = cols ?? []; this._render(); }
  get columns() { return this._columns; }

  set rows(rows) { this._rows = rows ?? []; this._render(); }
  get rows() { return this._rows; }

  _sorted() {
    if (!this._sortKey) return this._rows;
    const col = this._columns.find((c) => c.key === this._sortKey);
    const num = col && (col.type === "num" || col.type === "hex");
    return [...this._rows].sort((a, b) => {
      let x = a[this._sortKey], y = b[this._sortKey];
      if (num) return ((x ?? 0) - (y ?? 0)) * this._sortDir;
      x = String(x ?? ""); y = String(y ?? "");
      return x.localeCompare(y) * this._sortDir;
    });
  }

  _render() {
    if (!this._columns.length) {
      this._wrap.replaceChildren();
      return;
    }
    if (!this._rows.length) {
      const empty = document.createElement("div");
      empty.className = "empty";
      empty.textContent = "（无数据）";
      this._wrap.replaceChildren(empty);
      return;
    }
    const table = document.createElement("table");
    const thead = document.createElement("thead");
    const trh = document.createElement("tr");
    for (const col of this._columns) {
      const th = document.createElement("th");
      if (col.type === "num" || col.type === "hex") th.classList.add("num");
      th.textContent = col.label;
      if (this._sortKey === col.key) {
        const arrow = document.createElement("span");
        arrow.className = "arrow";
        arrow.textContent = this._sortDir > 0 ? "▲" : "▼";
        th.appendChild(arrow);
      }
      th.addEventListener("click", () => {
        if (this._sortKey === col.key) this._sortDir *= -1;
        else { this._sortKey = col.key; this._sortDir = 1; }
        this._render();
      });
      trh.appendChild(th);
    }
    thead.appendChild(trh);
    table.appendChild(thead);

    const tbody = document.createElement("tbody");
    for (const row of this._sorted()) {
      const tr = document.createElement("tr");
      for (const col of this._columns) {
        const td = document.createElement("td");
        if (col.type === "num" || col.type === "hex") td.classList.add("num");
        if (col.type === "mono" || col.type === "hex") td.classList.add(col.type === "hex" ? "hex" : "mono");
        const value = fmt(row[col.key], col.type);
        if (value === "" || value == null) {
          const dim = document.createElement("span");
          dim.className = "dim";
          dim.textContent = "—";
          td.appendChild(dim);
        } else {
          td.textContent = String(value);
        }
        tr.appendChild(td);
      }
      tr.addEventListener("dblclick", () => this.dispatchEvent(new CustomEvent("ui-row-activate", {
        bubbles: true, composed: true, detail: { row },
      })));
      tbody.appendChild(tr);
    }
    table.appendChild(tbody);
    this._wrap.replaceChildren(table);
  }
}

if (typeof customElements !== "undefined" && !customElements.get("ui-table")) {
  customElements.define("ui-table", UiTableElement);
}
export default UiTableElement;
