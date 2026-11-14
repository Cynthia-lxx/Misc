/* SPDX-License-Identifier: MIT
 * Derived from ppy/osu (MIT) — see 00-overview/LICENSE-MIT.txt and 00-overview/licence-audit.json
 * Behaviour structure from:
 *   osu.Game/Graphics/UserInterface/OsuButton.cs:133-176
 *   osu.Game/Graphics/UserInterface/ShearedButton.cs:191-249
 *   osu.Game/Graphics/UserInterfaceV2/FormControlBackground.cs:42-93 (four visual states)
 * Rewritten as a native ES module Custom Element; no upstream code, assets or branding.
 */

const STYLE_URL = new URL('./ui-button.css', import.meta.url);

const FALLBACK_STYLES = `
  :host { display: inline-block; }
  button {
    height: var(--control-height, 40px);
    padding: 0 16px;
    border: var(--border-width, 2px) solid var(--ui-border, #444);
    border-radius: var(--corner-radius, 5px);
    background: var(--ui-surface-raised, #333);
    color: var(--ui-text, #fff);
    font: inherit;
    cursor: pointer;
  }
  button:disabled { opacity: 0.5; cursor: not-allowed; }
`;

/**
 * @typedef {Object} UiButtonPressDetail
 * @property {string} label
 * @property {string|null} i18nKey
 * @property {string} variant
 */

export class UiButtonElement extends HTMLElement {
  static get observedAttributes() {
    return ['label', 'variant', 'disabled', 'data-i18n-key', 'data-theme'];
  }

  constructor() {
    super();
    this.attachShadow({ mode: 'open' });

    this._style = document.createElement('style');
    this._style.textContent = FALLBACK_STYLES;

    this._button = document.createElement('button');
    this._button.setAttribute('part', 'button');
    this._button.setAttribute('type', 'button');

    const slot = document.createElement('slot');
    this._labelFallback = document.createElement('span');
    this._labelFallback.className = 'label';
    slot.appendChild(this._labelFallback);
    this._button.appendChild(slot);

    this.shadowRoot.append(this._style, this._button);

    this._onPress = (event) => {
      if (this._button.disabled) {
        event.preventDefault();
        event.stopImmediatePropagation();
        return;
      }
      /** @type {CustomEvent<UiButtonPressDetail>} */
      const detail = new CustomEvent('ui-press', {
        bubbles: true,
        composed: true,
        detail: {
          label: this._resolvedLabel(),
          i18nKey: this.getAttribute('data-i18n-key'),
          variant: this.getAttribute('variant') || 'default',
        },
      });
      // Visible click feedback: the CSS declares :host([data-flash]) animation,
      // but nothing used to set it, so clicks appeared to do nothing.
      this.dataset.flash = '';
      window.setTimeout(() => { delete this.dataset.flash; }, 500);
      this.dispatchEvent(detail);
    };
    this._button.addEventListener('click', this._onPress);
  }

  connectedCallback() {
    this._loadStyles();
    this._sync();
    // 语言包切换后重解析 data-i18n-key 标签（宿主 app 分发 ui-i18n-change）。
    this._onI18nChange = () => this._sync();
    document.addEventListener('ui-i18n-change', this._onI18nChange);
  }

  disconnectedCallback() {
    this._button.removeEventListener('click', this._onPress);
    document.removeEventListener('ui-i18n-change', this._onI18nChange);
  }

  attributeChangedCallback() {
    if (this.isConnected) this._sync();
  }

  /** Load the sibling CSS file into the shadow root; keep the inline fallback on failure. */
  async _loadStyles() {
    if (this._stylesLoaded) return;
    this._stylesLoaded = true;
    try {
      const response = await fetch(STYLE_URL);
      if (response.ok) this._style.textContent = await response.text();
    } catch {
      /* file:// or offline: inline fallback stays in place */
    }
  }

  _resolvedLabel() {
    const explicit = this.getAttribute('label');
    if (explicit) return explicit;

    const key = this.getAttribute('data-i18n-key');
    const dictionary = globalThis.UIReference?.i18n;
    if (key && dictionary && typeof dictionary[key] === 'string') return dictionary[key];

    return this.textContent?.trim() || '';
  }

  _sync() {
    const label = this._resolvedLabel();
    this._labelFallback.textContent = label;

    const variant = this.getAttribute('variant') || 'default';
    this._button.dataset.variant = variant;
    this._button.disabled = this.hasAttribute('disabled');
    this._button.setAttribute('aria-disabled', String(this._button.disabled));

    if (label) {
      this._button.setAttribute('aria-label', label);
      this._labelFallback.hidden = Boolean(this.textContent?.trim());
    } else {
      this._button.removeAttribute('aria-label');
      this._labelFallback.hidden = false;
    }
  }

  /** Local theme override; also notifies the host so a global switch can be applied. */
  setTheme(name) {
    if (!name) return;
    this.dataset.theme = name;
    this.dispatchEvent(new CustomEvent('ui-theme-change', {
      bubbles: true,
      composed: true,
      detail: { theme: name },
    }));
  }

  get theme() {
    return this.dataset.theme || null;
  }
}

if (typeof customElements !== 'undefined' && !customElements.get('ui-button')) {
  customElements.define('ui-button', UiButtonElement);
}

export default UiButtonElement;
