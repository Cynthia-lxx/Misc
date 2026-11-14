/* SPDX-License-Identifier: MIT — Component gallery with live assertions. */
import { escapeHtml, buttonRow, on, setStatus, table } from './views.js';
import '../components/ui-button/ui-button.js';
import '../components/ui-textbox/ui-textbox.js';
import '../components/ui-slider/ui-slider.js';
import '../components/ui-dropdown/ui-dropdown.js';
import '../components/ui-tabs/ui-tabs.js';
import '../components/ui-checkbox/ui-checkbox.js';
import '../components/ui-scroll/ui-scroll.js';
import '../components/ui-breadcrumb/ui-breadcrumb.js';
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
          <h2>ui-textbox</h2>
          <p>聚焦 3px 描边、逐字坠落入场、光标节拍脉冲、Enter 提交闪烁、非法输入红闪；<code>search</code> / <code>number</code> 变体。</p>
          <div class="pv-row">
            <ui-textbox id="cp-tb-main" placeholder="type here…"></ui-textbox>
            <ui-textbox variant="search" data-i18n-key="ui.user_search"></ui-textbox>
            <ui-textbox variant="number" value="42"></ui-textbox>
            <ui-textbox placeholder="disabled" disabled></ui-textbox>
          </div>
          <div class="pv-row"><span class="pv-note" id="cp-tb-status">change 0 · commit 0 · error 0</span></div>
        </section>

        <section class="pv-card">
          <h2>ui-slider</h2>
          <p>nub 250ms OutQuint 跟随、hover/drag 辉光、30ms 节流 tick 脉冲、双击恢复默认、方向键 / Shift×10 / PgUp·PgDn / Home·End。</p>
          <div class="pv-row">
            <ui-slider id="cp-sl-main" min="0" max="100" step="1" value="70"></ui-slider>
            <output id="cp-sl-out">70</output>
          </div>
          <div class="pv-row">
            <ui-slider min="0" max="1" step="0.05" value="0.5"></ui-slider>
            <ui-slider min="0" max="100" value="30" disabled></ui-slider>
          </div>
          <div class="pv-row"><span class="pv-note" id="cp-sl-status">changes: 0</span></div>
        </section>

        <section class="pv-card">
          <h2>ui-dropdown</h2>
          <p>菜单 300ms OutQuint 淡入+高度过渡、悬停条 100ms 入 / 600ms 出、悬停 chevron 滑入、头部 chevron 翻转、方向键预选 / Enter 选择 / Esc 关闭、长标签截断省略。</p>
          <div class="pv-row">
            <ui-dropdown id="cp-dd-main" placeholder="choose…">
              <ui-option value="wave" label="Wave 四层旋转"></ui-option>
              <ui-option value="sheared" label="Sheared 斜切"></ui-option>
              <ui-option value="slide-x" label="Slide-X 侧栏"></ui-option>
              <ui-option value="pop" label="Pop 环形弹性揭示"></ui-option>
            </ui-dropdown>
            <span class="pv-badge" id="cp-dd-value">value: —</span>
            <ui-dropdown disabled placeholder="disabled"></ui-dropdown>
          </div>
        </section>

        <section class="pv-card">
          <h2>ui-tabs</h2>
          <p>悬停/激活底栏 1px 淡入 500ms OutQuint、激活加粗白字 / 未激活 accent 字、<code>strip</code> 属性启用底线跟随、方向键 / Home / End。</p>
          <div class="pv-row">
            <ui-tabs id="cp-tabs-main" value="all" strip>
              <ui-tab value="all" label="All"></ui-tab>
              <ui-tab value="recent" label="Recently Played"></ui-tab>
              <ui-tab value="favourite" label="Favourites"></ui-tab>
              <ui-tab value="group" label="Group"></ui-tab>
            </ui-tabs>
          </div>
          <div class="pv-row"><span class="pv-badge" id="cp-tabs-value">value: all</span></div>
        </section>

        <section class="pv-card">
          <h2>ui-checkbox</h2>
          <p>nub 靠右、选中 OutElasticHalf 弹出（width 38→50 / 内环 3→8.5）、未选 OutQuint 收回、悬停辉光、空格 / 回车切换。</p>
          <div class="pv-row">
            <ui-checkbox id="cp-cb-main" label="Parallax" checked></ui-checkbox>
            <ui-checkbox label="Interface voices"></ui-checkbox>
            <ui-checkbox label="Disabled" disabled checked></ui-checkbox>
            <ui-checkbox data-i18n-key="ui.rotate_cursor"></ui-checkbox>
          </div>
          <div class="pv-row"><span class="pv-badge" id="cp-cb-value">checked: true</span></div>
        </section>

        <section class="pv-card">
          <h2>ui-scroll</h2>
          <p>自绘滚动条（宽 10、圆角 5、最小拇指 30）：悬停 GrayF、拖拽 accent 高亮、轨道点击绝对滚动、原生滚动条隐藏。</p>
          <ui-scroll id="cp-scroll-main" style="height:180px">
            <div class="pv-code" style="max-height:none">scroll me —
              ${Array.from({ length: 14 }, (_, i) => `\nline ${String(i + 1).padStart(2, '0')} — The quick brown fox jumps over the lazy dog.`).join('')}
            </div>
          </ui-scroll>
        </section>

        <section class="pv-card">
          <h2>ui-breadcrumb</h2>
          <p>选中之后的项 500ms OutQuint 淡出+横向缩至 0.8、之前的项带分隔 chevron、激活白字加粗+底栏。</p>
          <div class="pv-row">
            <ui-breadcrumb id="cp-crumb-main" value="all">
              <ui-crumb value="all" label="All"></ui-crumb>
              <ui-crumb value="favourite" label="Favourites"></ui-crumb>
              <ui-crumb value="recent" label="Recently Played"></ui-crumb>
              <ui-crumb value="group" label="Group"></ui-crumb>
            </ui-breadcrumb>
          </div>
          <div class="pv-row"><span class="pv-badge" id="cp-crumb-value">value: all</span></div>
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
            ['cp-ov-axis', '连续切换方向检验'],
          ])}
          <div class="pv-row">
            <span class="pv-badge" id="cp-ov-badge">not tested</span>
            <span class="pv-note" id="cp-ov-state">overlay: closed</span>
            <label class="pv-note"><input type="checkbox" id="cp-ov-safe"> 显示斜切内容安全区</label>
          </div>
          <div class="pv-code" id="cp-ov-diag">点击任一 variant 开始检验。</div>
          <div id="cp-ov-axis-out" style="margin-top:8px"></div>
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
        <ui-button id="cp-ov-inner-close" label="Close"></ui-button>
      </ui-reveal-overlay>`;

    /* ---- ui-button ---- */
    let presses = 0;
    const onPress = () => { presses += 1; setStatus(root, `presses: ${presses}`); };
    root.addEventListener('ui-press', onPress);

    /* ---- ui-textbox ---- */
    const tbStatus = root.querySelector('#cp-tb-status');
    const tbEvents = { change: 0, commit: 0, error: 0 };
    const tbSync = () => { tbStatus.textContent = `change ${tbEvents.change} · commit ${tbEvents.commit} · error ${tbEvents.error}`; };
    for (const name of ['ui-text-change', 'ui-text-commit', 'ui-text-error']) {
      const kind = name.split('-')[2];
      root.addEventListener(name, (event) => {
        tbEvents[kind] += 1;
        tbSync();
        ctx.log(`textbox ${name}: ${event.detail.value}`);
      });
    }

    /* ---- ui-slider ---- */
    const slider = root.querySelector('#cp-sl-main');
    const sliderOut = root.querySelector('#cp-sl-out');
    const sliderStatus = root.querySelector('#cp-sl-status');
    let sliderChanges = 0;
    slider.addEventListener('ui-change', (event) => {
      sliderChanges += 1;
      sliderOut.textContent = String(event.detail.value);
      const nubAnims = slider.shadowRoot.querySelector('.nub').getAnimations().length;
      sliderStatus.textContent = `changes: ${sliderChanges} · nub anims: ${nubAnims}`;
      ctx.log(`slider → ${event.detail.value}`);
    });

    /* ---- ui-dropdown ---- */
    const dropdown = root.querySelector('#cp-dd-main');
    const ddValue = root.querySelector('#cp-dd-value');
    dropdown.addEventListener('ui-select', (event) => {
      ddValue.textContent = `value: ${event.detail.value}`;
      ddValue.className = 'pv-badge ok';
      ctx.log(`dropdown select: ${event.detail.value}`);
    });

    /* ---- ui-tabs / ui-checkbox ---- */
    const tabs = root.querySelector('#cp-tabs-main');
    const tabsValue = root.querySelector('#cp-tabs-value');
    tabs.addEventListener('ui-select', (event) => {
      tabsValue.textContent = `value: ${event.detail.value}`;
      ctx.log(`tabs select: ${event.detail.value}`);
    });
    const checkbox = root.querySelector('#cp-cb-main');
    const cbValue = root.querySelector('#cp-cb-value');
    checkbox.addEventListener('ui-change', (event) => {
      cbValue.textContent = `checked: ${event.detail.checked}`;
      ctx.log(`checkbox → ${event.detail.checked}`);
    });

    /* ---- ui-breadcrumb ---- */
    const breadcrumb = root.querySelector('#cp-crumb-main');
    const crumbValue = root.querySelector('#cp-crumb-value');
    breadcrumb.addEventListener('ui-select', (event) => {
      crumbValue.textContent = `value: ${event.detail.value}`;
      ctx.log(`breadcrumb select: ${event.detail.value}`);
    });

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

    const showVariant = async (id) => {
      ctx.setReduce(false);
      overlay.removeAttribute('data-activation');
      await overlay.setVariant(id);
      root.querySelector('#cp-ov-variant').textContent = id;
      await overlay.open();
    };

    for (const [id] of VARIANTS) {
      root.querySelector(`#cp-ov-${id}`).addEventListener('click', async () => {
        await showVariant(id);
        await probe(`open(${id})`);
      });
    }
    on(root, '#cp-ov-close', 'click', async () => {
      overlay.close();
      await probe('close (exit)', 120);
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
    on(root, '#cp-ov-safe', 'change', (event) => {
      overlay.setSafeZone(event.currentTarget.checked);
      ctx.log(`sheared safe zone → ${event.currentTarget.checked}`);
    });

    /* Regress the reported bug: switching variants must never reuse the
     * previous geometry's axis for the first enter. */
    const EXPECTED_AXIS = { wave: 'contentY', sheared: 'plateY', 'slide-x': 'X', 'slide-y': 'Y', pop: 'scale' };
    let axisBusy = false;
    on(root, '#cp-ov-axis', 'click', async (event) => {
      if (axisBusy) return;
      axisBusy = true;
      event.currentTarget.dataset.active = 'true';
      const out = root.querySelector('#cp-ov-axis-out');
      out.innerHTML = '<span class="pv-note">依次切换 5 个 variant 并采样入场方向…</span>';
      const rows = [];
      for (const [id] of VARIANTS) {
        await showVariant(id);
        await new Promise((resolve) => window.setTimeout(resolve, 130));
        const shadow = overlay.shadowRoot;
        const panel = new DOMMatrix(getComputedStyle(shadow.querySelector('.panel')).transform);
        const plate = new DOMMatrix(getComputedStyle(shadow.querySelector('.plate')).transform);
        const content = new DOMMatrix(getComputedStyle(shadow.querySelector('.content')).transform);
        let actual = 'none';
        if (id === 'pop') actual = panel.a < 0.99 ? 'scale' : 'none';
        else if (id === 'wave') actual = Math.abs(content.f) > 20 ? 'contentY' : 'none';
        else if (id === 'sheared') actual = Math.abs(plate.f) > 20 ? 'plateY' : 'none';
        else actual = Math.abs(panel.e) > Math.abs(panel.f) ? 'X' : 'Y';
        rows.push([
          `<code>${escapeHtml(id)}</code>`,
          `<span class="pv-note">${EXPECTED_AXIS[id]}</span>`,
          escapeHtml(actual),
          `${Math.round(panel.e)} / ${Math.round(panel.f)}`,
          `<span class="pv-badge ${actual === EXPECTED_AXIS[id] ? 'ok' : 'bad'}">${actual === EXPECTED_AXIS[id] ? 'PASS' : 'FAIL'}</span>`,
        ]);
        await overlay.close();
      }
      out.innerHTML = table(['variant', '期望轴', '实测轴', 'tx / ty', '结论'], rows);
      const passed = rows.filter((row) => row[4].includes('PASS')).length;
      setBadge(`方向检验 ${passed}/${VARIANTS.length} PASS`, passed === VARIANTS.length);
      ctx.log(`variant axis regression: ${passed}/${VARIANTS.length} PASS`);
      axisBusy = false;
      event.currentTarget.dataset.active = 'false';
    });

    let timer = null;
    on(root, '#cp-ov-cycle', 'click', (event) => {
      if (timer) { clearInterval(timer); timer = null; event.currentTarget.dataset.active = 'false'; return; }
      event.currentTarget.dataset.active = 'true';
      let index = 0;
      let busy = false;
      const step = async () => {
        if (busy) return;
        busy = true;
        const [id] = VARIANTS[index % VARIANTS.length];
        await showVariant(id);
        await probe(`cycle(${id})`, 120);
        index += 1;
        busy = false;
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
