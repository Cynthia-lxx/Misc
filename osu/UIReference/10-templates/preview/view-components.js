/* SPDX-License-Identifier: MIT — Component gallery with live assertions. */
import { escapeHtml, buttonRow, on, setStatus, table } from './views.js';
import '../components/ui-button/ui-button.js';
import { mount as mountPanel } from '../components/ui-panel/ui-panel.js';
import '../04-components/ui-action-rail/ui-action-rail.js';
import '../04-components/ui-reveal-overlay/ui-reveal-overlay.js';

const VARIANTS = [
  ['wave', 'Wave 四层旋转揭示'], ['sheared', 'Sheared 斜切下滑'],
  ['slide-x', 'Slide-X 侧栏'], ['slide-y', 'Slide-Y 底部'], ['pop', 'Pop 环形弹性'],
];

const MENU_ITEMS = [
  ['settings', '⚙', 'primary', 'primary', '#88da20', 'Settings'],
  ['detail', '▶', 'primary', 'primary', '#66ccff', 'Play'],
  ['secondary', '✎', 'primary', 'primary', '#ffcc22', 'Edit'],
  ['browse', '⌕', 'primary', 'primary', '#8c66ff', 'Browse'],
  ['solo', '●', 'detail', 'detail', '#66ccff', 'Solo'],
  ['group', '●●', 'detail', 'detail', '#ff66ab', 'Multi'],
  ['edit-map', '▤', 'secondary', 'secondary', '#ffcc22', 'Modify item'],
  ['edit-renderer', '◈', 'secondary', 'secondary', '#ff9966', 'Modify theme'],
];

export default {
  async mount(root, ctx) {
    const manifest = await ctx.json('manifest', 'manifest.json');
    const components = manifest?.components || [];

    root.innerHTML = `
      <div class="pv-grid">
        <section class="pv-card">
          <h2>ui-button</h2>
          <p>Custom Element + Shadow DOM；四态、sheared/danger 变体、i18n 标签、点击 <code>data-flash</code> 闪光。</p>
          <div class="pv-row">
            <ui-button label="Default"></ui-button>
            <ui-button variant="sheared" label="Sheared"></ui-button>
            <ui-button variant="danger" label="Danger"></ui-button>
            <ui-button label="Disabled" disabled></ui-button>
            <ui-button data-i18n-key="common.back"></ui-button>
          </div>
          <div class="pv-row"><span class="pv-note" data-status>presses: 0</span></div>
        </section>

        <section class="pv-card">
          <h2>ui-panel</h2>
          <p>纯 DOM 工厂，导出 <code>mount / unmount / setTheme</code>；选中发光与节拍脉冲。</p>
          ${buttonRow([
            ['cp-panel-mount', 'mount'], ['cp-panel-unmount', 'unmount'],
            ['cp-panel-selected', 'toggle selected'], ['cp-panel-beat', 'toggle beat'],
            ['cp-panel-theme', 'setTheme(pink)'],
          ])}
          <div id="cp-panel-host" style="margin-top:10px"></div>
          <div class="pv-row"><span class="pv-note" id="cp-panel-state">handle: —</span></div>
        </section>

        <section class="pv-card" style="grid-column:1/-1">
          <h2>ui-action-rail</h2>
          <p>锚点右侧横排楔形按钮：4 种宽度状态、<code>-20px</code> 负间距、<code>Shear 0.2 / -0.2</code> 反切、节拍驱动 hover、加性按压层、键盘导航、hub 追踪。</p>
          <ui-action-rail id="cp-menu">
            ${MENU_ITEMS.map(([id, icon, min, max, colour, label]) =>
              `<span data-menu-item="${id}" data-icon="${icon}" data-min="${min}" data-max="${max}" data-colour="${colour}">${escapeHtml(label)}</span>`).join('')}
          </ui-action-rail>
          ${buttonRow([
            ['cp-state-initial', 'Initial'], ['cp-state-primary', 'Primary'], ['cp-state-detail', 'Play'],
            ['cp-state-group', 'Multi'], ['cp-state-secondary', 'Edit'],
            ['cp-state-transition', 'Transition'], ['cp-state-hidden', 'Exit'],
          ])}
          <div class="pv-row"><strong class="pv-note" id="cp-menu-state">state: initial</strong><span class="pv-note">hover 楔形按钮查看 160→240px OutElastic 与图标节拍心跳</span></div>
        </section>

        <section class="pv-card" style="grid-column:1/-1">
          <h2>ui-reveal-overlay · 入/出场动效检验</h2>
          <p>5 种几何 variant。<b>关键回归断言</b>：打开后读取 <code>shadowRoot.getAnimations()</code>，必须 &gt; 0 才算真的播放了过渡；关闭后等动画结束才隐藏。</p>
          ${buttonRow(VARIANTS.map(([id, label]) => [`cp-ov-${id}`, label]))}
          ${buttonRow([
            ['cp-ov-close', 'Close（退场）'], ['cp-ov-replay', 'Replay（重播入场）'],
            ['cp-ov-cycle', '自动轮换'], ['cp-ov-disabled', 'activation=disabled'],
          ])}
          <div class="pv-row">
            <span class="pv-badge" id="cp-ov-badge">not tested</span>
            <span class="pv-note" id="cp-ov-state">overlay: closed</span>
          </div>
          <div class="pv-code" id="cp-ov-diag">点击任一 variant 开始检验。</div>
        </section>

        <section class="pv-card" style="grid-column:1/-1">
          <h2>清单</h2>
          ${table(['组件', '形态', '挂载点', '导出', '来源'], components.map((c) => [
            `<code>${escapeHtml(c.id)}</code>`, escapeHtml(c.kind),
            `<code>${escapeHtml(c.mount)}</code>`,
            `<span class="pv-note">${escapeHtml((c.exports || []).join(', '))}</span>`,
            `<span class="pv-note">${escapeHtml((c.source || [])[0] || '')}</span>`,
          ]))}
        </section>
      </div>

      <ui-reveal-overlay id="cp-overlay" variant="wave">
        <h2>Overlay content</h2>
        <p>当前 variant：<strong id="cp-ov-variant">wave</strong>。按 Esc 或点击遮罩关闭。</p>
        <p class="pv-note">Wave 四层 rotation 13/-7/4/-2、finalY -930/-560/-390/-220；content relative Y 200%→0。</p>
        <button type="button" class="pv-btn" id="cp-ov-inner-close">Close</button>
      </ui-reveal-overlay>`;

    /* ---- ui-button ---- */
    let presses = 0;
    const onPress = () => { presses += 1; setStatus(root, `presses: ${presses}`); };
    root.addEventListener('ui-press', onPress);

    /* ---- ui-panel ---- */
    let panel = null;
    const panelHost = root.querySelector('#cp-panel-host');
    const panelState = root.querySelector('#cp-panel-state');
    const syncPanel = () => { panelState.textContent = `handle: ${panel ? 'mounted' : '—'}`; };
    const mountHandler = () => {
      if (panel) panel.unmount();
      panel = mountPanel(panelHost, {
        title: 'Panel component', description: 'mount / unmount / setTheme',
        selected: true, strings: ctx.state.i18n,
      });
      panel.element.addEventListener('ui-select', () => ctx.log('panel clicked'));
      syncPanel();
    };
    mountHandler();
    on(root, '#cp-panel-mount', 'click', mountHandler);
    on(root, '#cp-panel-unmount', 'click', () => { panel?.unmount(); panel = null; syncPanel(); });
    on(root, '#cp-panel-selected', 'click', () => {
      if (!panel) return;
      panel.update({ selected: panel.element.dataset.selected !== 'true' });
    });
    on(root, '#cp-panel-beat', 'click', () => {
      if (!panel) return;
      panel.update({ beat: panel.element.dataset.beat !== 'true' });
    });
    on(root, '#cp-panel-theme', 'click', () => panel?.setTheme('pink'));

    /* ---- ui-action-rail ---- */
    const menu = root.querySelector('#cp-menu');
    const menuState = root.querySelector('#cp-menu-state');
    for (const button of root.querySelectorAll('[id^="cp-state-"]')) {
      button.addEventListener('click', () => menu.setState(button.id.replace('cp-state-', '')));
    }
    const onMenuState = (event) => { menuState.textContent = `state: ${event.detail.state}`; };
    const onMenuPress = (event) => { menuState.textContent = `pressed: ${event.detail.id} @ ${event.detail.state}`; };
    menu.addEventListener('ui-menu-state', onMenuState);
    menu.addEventListener('ui-menu-press', onMenuPress);
    window.setTimeout(() => menu.setState('primary'), 150);

    /* ---- ui-reveal-overlay ---- */
    const overlay = root.querySelector('#cp-overlay');
    const badge = root.querySelector('#cp-ov-badge');
    const diag = root.querySelector('#cp-ov-diag');
    const ovState = root.querySelector('#cp-ov-state');
    const setBadge = (text, ok) => {
      badge.textContent = text;
      badge.className = `pv-badge ${ok === true ? 'ok' : ok === false ? 'bad' : ''}`;
    };
    const probe = async (label, waitMs = 140) => {
      await new Promise((resolve) => window.setTimeout(resolve, waitMs));
      const count = overlay.animationCount;
      const visible = getComputedStyle(overlay).visibility;
      const open = overlay.isOpen;
      diag.textContent = `${label}\nrunning transitions: ${count}\nvisibility: ${visible}\ndata-open: ${open}\nvariant: ${overlay.variant}\nassert: ${count > 0 ? 'PASS (transition is running)' : 'FAIL (no CSS transition produced)'}`;
      setBadge(`${label}: ${count > 0 ? 'PASS' : 'FAIL'} (${count} anim)`, count > 0);
      ovState.textContent = `overlay: ${open ? overlay.variant + ' / open' : 'closed'}`;
      ctx.log(`${label}: ${count} transitions`);
    };

    for (const [id] of VARIANTS) {
      root.querySelector(`#cp-ov-${id}`).addEventListener('click', async () => {
        ctx.setReduce(false);
        overlay.removeAttribute('data-activation');
        overlay.setAttribute('variant', id);
        root.querySelector('#cp-ov-variant').textContent = id;
        await overlay.open();
        await probe(`open(${id})`);
      });
    }
    on(root, '#cp-ov-close', 'click', async () => {
      overlay.close();
      await probe('close (exit)', 120);
      window.setTimeout(() => {
        diag.textContent += `\nafter animation: visibility=${getComputedStyle(overlay).visibility}`;
      }, 950);
    });
    on(root, '#cp-ov-inner-close', 'click', () => overlay.close());
    on(root, '#cp-ov-replay', 'click', async () => {
      if (!overlay.isOpen) await overlay.open(); else overlay.replay();
      await probe('replay', 120);
    });
    on(root, '#cp-ov-disabled', 'click', async () => {
      overlay.setAttribute('data-activation', 'disabled');
      const ok = await overlay.open();
      diag.textContent = `activation=disabled → open() returned ${ok}；组件被 OverlayActivation 门控，符合上游 OsuFocusedOverlayContainer 行为。`;
      setBadge('gated', null);
      overlay.removeAttribute('data-activation');
    });

    let timer = null;
    on(root, '#cp-ov-cycle', 'click', (event) => {
      if (timer) { clearInterval(timer); timer = null; event.currentTarget.dataset.active = 'false'; return; }
      event.currentTarget.dataset.active = 'true';
      let index = 0;
      const step = async () => {
        const [id] = VARIANTS[index % VARIANTS.length];
        overlay.setAttribute('variant', id);
        root.querySelector('#cp-ov-variant').textContent = id;
        await overlay.open();
        await probe(`cycle(${id})`, 120);
        index += 1;
      };
      step();
      timer = window.setInterval(step, 2600);
    });

    const onTheme = (event) => {
      panel?.setTheme(event.detail.theme);
      menu.setTheme(event.detail.theme);
      overlay.setTheme(event.detail.theme);
    };
    window.addEventListener('pv-theme-change', onTheme);
    document.addEventListener('ui-overlay-close', () => { ovState.textContent = 'overlay: closed'; });

    return () => {
      if (timer) clearInterval(timer);
      window.removeEventListener('pv-theme-change', onTheme);
      root.removeEventListener('ui-press', onPress);
      menu.removeEventListener('ui-menu-state', onMenuState);
      menu.removeEventListener('ui-menu-press', onMenuPress);
      panel?.unmount();
      overlay.unmount();
    };
  },
};
