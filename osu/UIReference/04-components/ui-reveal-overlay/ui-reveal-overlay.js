/* SPDX-License-Identifier: MIT
 * Derived from ppy/osu (MIT) — see 00-overview/LICENSE-MIT.txt
 * Ported from Graphics/Containers/WaveContainer.cs:19-192,
 * Overlays/WaveOverlayContainer.cs, Overlays/Mods/ShearedOverlayContainer.cs:132-174,
 * Overlays/SettingsPanel.cs:170-204, Overlays/Dialog/PopupDialog.cs:324-354.
 *
 * Five overlay reveal geometries with real enter/exit transitions.
 * Visibility is gated by `data-visible` (visibility/pointer-events), NOT by
 * `display:none`: switching display and transform in the same style
 * recalculation gives the browser no before-change style, so transitions are
 * skipped entirely. `open()` waits for the shadow stylesheet, then forces a
 * layout frame before flipping `data-open`; `close()` waits for the running
 * animations to finish before hiding.
 */

const STYLE_URL = new URL('./ui-reveal-overlay.css', import.meta.url);

const REVEAL_LAYERS = [
  { rotation: 13, y: -930, anchor: 'centre' },
  { rotation: -7, y: -560, anchor: 'right' },
  { rotation: 4, y: -390, anchor: 'centre' },
  { rotation: -2, y: -220, anchor: 'right' },
];

const FALLBACK_STYLES = `
  :host { position: fixed; inset: 0; display: block; z-index: 2000;
          visibility: hidden; pointer-events: none; }
  :host([data-visible='true']) { visibility: visible; pointer-events: auto; }
  .panel { position: absolute; inset: 0; background: var(--ui-surface, #2a2a2a); }
`;

const nextFrame = () => new Promise((resolve) => requestAnimationFrame(() => requestAnimationFrame(resolve)));
const delay = (ms) => new Promise((resolve) => window.setTimeout(resolve, ms));

export class UiRevealOverlayElement extends HTMLElement {
  static get observedAttributes() { return ['variant', 'data-activation', 'data-theme']; }

  constructor() {
    super();
    this.attachShadow({ mode: 'open' });
    this._style = document.createElement('style');
    this._style.textContent = FALLBACK_STYLES;

    this._dim = document.createElement('div');
    this._dim.className = 'dim';

    this._panel = document.createElement('div');
    this._panel.className = 'panel';
    this._panel.setAttribute('part', 'panel');
    this._panel.dataset.open = 'false';

    this._waves = document.createElement('div');
    this._waves.className = 'waves';
    this._waves.setAttribute('aria-hidden', 'true');
    for (const [index, layer] of REVEAL_LAYERS.entries()) {
      const el = document.createElement('div');
      el.className = 'wave';
      el.dataset.anchor = layer.anchor;
      el.style.setProperty('--ui-wave-rotation', `${layer.rotation}deg`);
      el.style.setProperty('--ui-wave-y', `${layer.y}px`);
      el.style.setProperty('--ui-wave-index', String(index));
      this._waves.appendChild(el);
    }

    this._content = document.createElement('div');
    this._content.className = 'content';
    this._content.appendChild(document.createElement('slot'));

    this._panel.append(this._waves, this._content);
    this.shadowRoot.append(this._style, this._dim, this._panel);

    this._onDimClick = () => this.close();
    this._onKeyDown = (event) => { if (event.key === 'Escape') this.close(); };
    this._dim.addEventListener('click', this._onDimClick);

    this._stylesReady = null;
    this._hideToken = 0;
  }

  connectedCallback() {
    this._stylesReady = this._loadStyles();
    if (!this.hasAttribute('variant')) this.setAttribute('variant', 'wave');
    document.addEventListener('keydown', this._onKeyDown);
  }

  disconnectedCallback() {
    this._dim.removeEventListener('click', this._onDimClick);
    document.removeEventListener('keydown', this._onKeyDown);
  }

  attributeChangedCallback(name, oldValue, newValue) {
    if (name === 'variant' && oldValue && oldValue !== newValue && this.dataset?.open === 'true') {
      this.replay();
    }
  }

  async _loadStyles() {
    try {
      const response = await fetch(STYLE_URL);
      if (response.ok) this._style.textContent = await response.text();
    } catch { /* offline: inline fallback stays */ }
  }

  get variant() { return this.getAttribute('variant') || 'wave'; }
  get activation() { return this.getAttribute('data-activation') || 'all'; }
  get isOpen() { return this.dataset.open === 'true'; }

  async open() {
    if (this.activation === 'disabled') return false;
    if (this._stylesReady) await this._stylesReady;
    if (this.isOpen) { this.replay(); return true; }

    this._hideToken += 1;
    this.dataset.visible = 'true';
    await nextFrame(); // give the browser a before-change style to transition from

    this.dataset.open = 'true';
    this._panel.dataset.open = 'true';
    this.dispatchEvent(new CustomEvent('ui-overlay-open', {
      bubbles: true, composed: true, detail: { variant: this.variant },
    }));
    return true;
  }

  /** Re-run the enter animation from its first frame. */
  replay() {
    for (const animation of this.shadowRoot.getAnimations?.() || []) animation.cancel();
    this.dataset.open = 'false';
    this._panel.dataset.open = 'false';
    void this._panel.offsetWidth; // force synchronous reflow
    this.dataset.open = 'true';
    this._panel.dataset.open = 'true';
  }

  close() {
    if (!this.isOpen && this.dataset.visible !== 'true') return;
    this.dataset.open = 'false';
    this._panel.dataset.open = 'false';
    this.dispatchEvent(new CustomEvent('ui-overlay-close', {
      bubbles: true, composed: true, detail: { variant: this.variant },
    }));

    const token = ++this._hideToken;
    const running = this.shadowRoot.getAnimations?.() || [];
    const finished = running.length
      ? Promise.allSettled(running.map((animation) => animation.finished))
      : Promise.resolve();
    Promise.race([finished, delay(1000)]).then(() => {
      if (token === this._hideToken && !this.isOpen) this.dataset.visible = 'false';
    });
  }

  toggle() { return this.isOpen ? (this.close(), false) : this.open(); }

  /** Current transition count — used by the preview site as a regression assertion. */
  get animationCount() { return (this.shadowRoot.getAnimations?.() || []).length; }

  setTheme(name) { if (name) this.dataset.theme = name; return this; }

  unmount() {
    document.removeEventListener('keydown', this._onKeyDown);
    this.remove();
  }
}

if (typeof customElements !== 'undefined' && !customElements.get('ui-reveal-overlay')) {
  customElements.define('ui-reveal-overlay', UiRevealOverlayElement);
}

export default UiRevealOverlayElement;
