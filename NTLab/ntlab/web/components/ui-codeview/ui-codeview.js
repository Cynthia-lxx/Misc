/* SPDX-License-Identifier: GPL-3.0-or-later
 * ui-codeview — hand-rolled virtualized code renderer (IDA disassembly flavor).
 *
 * Fixed line-height, absolute-positioned render window, only visible rows
 * exist in the DOM → smooth over 100k+ lines. The page supplies rows via
 * resetRows()/appendRows() and an async `loadMore()` callback fired when the
 * user scrolls near the bottom (incremental paging, never big-bang loads).
 *
 * Row shape (disasm): { rva, bytes, mnemonic, op, isData? }
 * renderItem() can be overridden for other code views.
 * Clicking the address cell dispatches "ui-code-jump" {detail:{rva}}.
 */

const LINE_H = 20;

const STYLE = `
  :host { display: block; font: inherit; }
  .viewport {
    height: 100%; overflow-y: auto; overflow-x: hidden;
    border: 1px solid var(--ui-border, #2a2a2a); border-radius: 8px;
    background: var(--ui-surface-sunken, #111);
    font-family: var(--ntlab-font-mono, "Cascadia Code", Consolas, "Microsoft YaHei", monospace);
    font-size: 12px;
    position: relative;
  }
  .spacer { position: relative; width: 100%; }
  .cl {
    position: absolute; left: 0; right: 0; height: ${LINE_H}px;
    display: flex; align-items: center; gap: 0;
    white-space: nowrap; padding: 0 10px; cursor: default;
    transition: background 80ms ease-out;
  }
  .cl:hover { background: color-mix(in srgb, var(--ui-accent) 8%, transparent); }
  .cl.cur { background: color-mix(in srgb, var(--ui-accent) 16%, transparent); }
  .addr {
    flex: 0 0 96px; color: var(--ui-text-muted, #888); text-align: left;
    cursor: pointer;
  }
  .addr:hover { color: var(--ui-accent); text-decoration: underline; }
  .bytes { flex: 0 0 150px; color: var(--ui-text-muted, #666); overflow: hidden; }
  .mnem { flex: 0 0 90px; font-weight: 600; }
  .ops  { flex: 1; overflow: hidden; text-overflow: ellipsis; color: var(--ui-text, #ddd); }
  .mnem-ctl { color: #ff66aa; }   /* call / jmp / loop */
  .mnem-ret { color: #ffcc22; }
  .mnem-stack { color: #8c66ff; }
  .mnem-db  { color: var(--ui-text-muted, #666); font-style: italic; }
  .num { color: #66ccff; }
  .num[data-jump] { cursor: pointer; }
  .num[data-jump]:hover { text-decoration: underline; color: var(--ui-accent, #4aa3ff); }
  .iat { color: #66cc99; cursor: pointer; margin-left: 10px; }
  .iat:hover { text-decoration: underline; }
  .reg { color: #8c66ff; }
  .empty { padding: 16px; color: var(--ui-text-muted, #888); }
`;

const REGEX_WORD = /[a-z]+/i;

function esc(s) {
  return String(s).replace(/[&<>"]/g, (c) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;",
  }[c]));
}

function colorOps(op, row = null) {
  // rip-relative 地址 → 先占位并算出绝对目标；裸 hex 立即数 → 可点击（有行上下文时）；
  // 寄存器 → .reg。占位符先换出，避免内部 0x… 被二次包装。
  const rips = [];
  let html = esc(op).replace(/\[rip\s*([+-])\s*(0x[0-9a-fA-F]+)\]/gi, (m, sign, hx) => {
    const idx = rips.length;
    let target = null;
    if (row && Number.isFinite(row.rva) && Number.isFinite(row.size)) {
      target = row.rva + row.size + (sign === "-" ? -parseInt(hx, 16) : parseInt(hx, 16));
    }
    rips.push({ sign, hx, target });
    return `\u0000${idx}\u0000`;
  });
  const canJump = row && Number.isFinite(row.rva);
  html = html.replace(/0x[0-9a-fA-F]+/g, (m) => {
    const attr = canJump ? ` data-jump="${parseInt(m.slice(2), 16).toString(16)}"` : "";
    return `<span class="num"${attr}>${m}</span>`;
  });
  html = html.replace(/\u0000(\d+)\u0000/g, (m, idx) => {
    const r = rips[+idx];
    if (r.target != null) {
      return `[rip${r.sign}${r.hx}] `
        + `<span class="num" data-jump="${r.target.toString(16)}">→0x${r.target.toString(16).toUpperCase()}</span>`;
    }
    return `[rip${r.sign}${r.hx}]`;
  });
  html = html.replace(/\b(r[a-z0-9]{1,3}|e[a-z]{2}[xji]|[a-d][xh]l|sil|dil|sp|bp)\b/gi,
    (m) => `<span class="reg">${m}</span>`);
  return html;
}

function mnemClass(m) {
  if (/^(call|jmp|j[a-z]{2,3}|loop)/.test(m)) return "mnem-ctl";
  if (/^ret/.test(m)) return "mnem-ret";
  if (/^(push|pop|leave|enter)$/.test(m)) return "mnem-stack";
  if (/^db$/.test(m)) return "mnem-db";
  return "";
}

export class UiCodeViewElement extends HTMLElement {
  constructor() {
    super();
    this.attachShadow({ mode: "open" });
    /** async () => rows[] | null — return null to stop incremental loading */
    this.loadMore = null;
    /** (row, index) => HTML string for one line (default: disasm layout) */
    this.renderItem = (row) => {
      const cur = row.rva === this._currentRva ? " cur" : "";
      const iat = row.iat
        ? `<span class="iat" data-open="${esc(row.iat.split("!")[0])}">; ${esc(row.iat)}</span>`
        : "";
      return `<span class="addr${cur}" data-jump="${row.rva.toString(16)}">0x${row.rva.toString(16).toUpperCase()}</span>`
        + `<span class="bytes">${esc(row.bytes ?? "")}</span>`
        + `<span class="mnem ${mnemClass(row.mnemonic)}">${esc(row.mnemonic ?? "")}</span>`
        + `<span class="ops">${colorOps(row.op ?? "", row)}${iat}</span>`;
    };
    this._rows = [];
    this._loadingMore = false;
    this._exhausted = false;
    this._currentRva = null;
    this._raf = 0;

    const style = document.createElement("style");
    style.textContent = STYLE;
    this._viewport = document.createElement("div");
    this._viewport.className = "viewport";
    this._spacer = document.createElement("div");
    this._spacer.className = "spacer";
    this._rowsEl = document.createElement("div");
    this._spacer.append(this._rowsEl);
    this._viewport.append(this._spacer);
    this.shadowRoot.append(style, this._viewport);

    this._viewport.addEventListener("scroll", () => {
      if (!this._raf) {
        this._raf = requestAnimationFrame(() => { this._raf = 0; this._renderWindow(); });
      }
      this._maybeLoadMore();
    });
    this._rowsEl.addEventListener("click", (e) => {
      const jumpEl = e.target.closest("[data-jump]");
      if (jumpEl) {
        this.dispatchEvent(new CustomEvent("ui-code-jump", {
          bubbles: true, composed: true, detail: { value: jumpEl.dataset.jump },
        }));
        return;
      }
      const openEl = e.target.closest("[data-open]");
      if (openEl) {
        this.dispatchEvent(new CustomEvent("ui-code-open", {
          bubbles: true, composed: true, detail: { value: openEl.dataset.open },
        }));
      }
    });
  }

  /** Replace the whole buffer (used on jump). */
  resetRows(rows, { currentRva = null } = {}) {
    this._rows = rows ?? [];
    this._currentRva = currentRva;
    this._viewport.scrollTop = 0;
    this._renderWindow();
  }

  /** Append rows (incremental load); height grows, window re-renders. */
  appendRows(rows) {
    this._rows.push(...(rows ?? []));
    this._renderWindow();
  }

  get rows() { return this._rows; }

  /** Scroll the row with the given RVA into view (must be loaded). */
  scrollToRva(rva) {
    this._currentRva = rva;
    // binary search (rows are rva-ascending for disasm buffers)
    let lo = 0, hi = this._rows.length - 1, hit = -1;
    while (lo <= hi) {
      const mid = (lo + hi) >> 1;
      if (this._rows[mid].rva === rva) { hit = mid; break; }
      if (this._rows[mid].rva < rva) lo = mid + 1; else hi = mid - 1;
    }
    if (hit >= 0) {
      this._viewport.scrollTop = Math.max(0, hit * LINE_H - this._viewport.clientHeight / 2);
    }
    this._renderWindow();
  }

  _renderWindow() {
    if (!this._rows.length) {
      this._spacer.style.height = "0px";
      this._rowsEl.innerHTML = `<div class="empty">（空）</div>`;
      return;
    }
    this._spacer.style.height = this._rows.length * LINE_H + "px";
    const first = Math.max(0, Math.floor(this._viewport.scrollTop / LINE_H) - 10);
    const visible = Math.ceil(this._viewport.clientHeight / LINE_H) + 30;
    const last = Math.min(this._rows.length, first + visible);
    let html = "";
    for (let i = first; i < last; i++) {
      html += `<div class="cl" style="top:${i * LINE_H}px">${this.renderItem(this._rows[i], i)}</div>`;
    }
    this._rowsEl.innerHTML = html;
  }

  _maybeLoadMore() {
    if (!this.loadMore || this._loadingMore || this._exhausted) return;
    const el = this._viewport;
    if (el.scrollTop + el.clientHeight >= el.scrollHeight - 300) {
      this._loadMore();
    }
  }

  async _loadMore() {
    this._loadingMore = true;
    try {
      const more = await this.loadMore();
      if (more == null) { this._exhausted = true; return; }
      if (more.length) this.appendRows(more);
    } finally {
      this._loadingMore = false;
      // 追加后可能仍不满足滚动条件（页太小），继续补页
      this._maybeLoadMore();
    }
  }
}

if (typeof customElements !== "undefined" && !customElements.get("ui-codeview")) {
  customElements.define("ui-codeview", UiCodeViewElement);
}
export default UiCodeViewElement;
