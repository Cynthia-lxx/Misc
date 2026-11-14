# UIReference 抽离库 · 会话进度（2026-11-14）

工作目录：`p:/Dev/osu/UIReference`（源码）+ `dist/`（装配产物，59 文件）+ `preview_server.py`（默认 ：8190，服务 DIST）。

## 预览站"吃自己的狗粮"完成（2026-11-14）

用户指令："让预览器本身的动效、排版、组件风格也使用这套UI库。" 已全部落地并回归通过：

- `preview.css` 整体重写：排版走 `--font-size-heading2/caption1/caption2`、颜色清硬编码（`#06202c`→`--gray-0`、状态色→`--state-success/warning/error`、canvas→`--gray-1`）、时长统一 `--pv-t-fast/normal = calc(var(--duration-*)*var(--motion-scale,1))` + `--pv-ease`；日志面板去 `hidden` 改 `.is-open`（visibility+opacity+transform 过渡，默认收起）；导航 active 左侧 3px 指示条；工具栏/日志头 `ui-button` 跑 32px 小号（`--control-height:32px; --border-width:1px`）；boot 入场 `@keyframes pv-enter`（sidebar/top）；reduced-motion 通配（`data-reduce-motion` + media query 双覆盖，含 `!important`）。
- `app.js`：import `../components/ui-button/ui-button.js` 与 `../05-motion/effects.js` 的 `animate/stagger`；`toggleLogPanel(force)` class 化（键盘 L 键直调，因 ui-button 的 click 在 shadow 内，host.click() 不会传导）；`setReduce` 改 `setAttribute('label',…)` + `dataset.active`；`staggerEntrance(viewHost)` 挂在 renderRoute 成功后——注意 **effects.js 的 `motionScale()` 只认 reduceMotion，不认 rate**，故 stagger 的 duration/delay 按 `1/state.rate` 手动预缩放。
- `views.js buttonRow()` → `<ui-button id label>`；view-theme/view-motion/view-effects/view-data/view-components 里残留的原生 `.pv-btn` 全部换 `ui-button`。所有视图绑定用 `event.currentTarget`（=ui-button 宿主），composed click 穿 shadow，兼容无需改。
- `ui-button.css` 新增 `:host([data-active='true'])` / `button[data-active='true']` 高亮态（accent 混色背景）。

## 两个必须记住的坑

1. **命名 token 撞 CSS**：`naming-map.json` 有裸 token `"top: 2" → "primary: 2"`；新 CSS 里 `top: 20%` 会被前缀误伤。规避法：用 `inset-block` 等逻辑属性替代 `top:`。写 preview 系文件后必须跑 `python 10-templates/apply-naming.py --dry-run` 确认 `files changed=0`。
2. **模块路径以 dist 装配布局为准**：`preview/*.js` 里引组件用 `../components/<id>/...`、motion 用 `../05-motion/...`（不是源码树的 `04-components/`），server 只服务 dist。

## 回归基线（全绿）

`assemble.py --check` → `validation: ok, previewViews: 7`；naming dry-run 0 改动；浏览器（playwright-cli `--browser=chromium`，系统 Chrome 不存在，默认 browser=open 会失败）断言：工具栏 ui-button 化（shadow 高 32）、reduce 切换 label/active/reduceMotion 联动、日志面板 `.is-open` opacity=1、7 视图卡片挂载 + stagger `getAnimations()` 计数正常、purple 主题 accent 跟随 `hsl(255…)`、console 仅 favicon 与 `/api/presets`（静态回退，`ctx.json` 带 fallbackPath）两处既有噪音。

## 动效丰富化：路由切换 + 每视图签名入场（2026-11-14 追加）

用户反馈"预览器基本只有淡入、切换没动效"后，对齐上游模式补齐：

- **路由切换 = ScreenStack push/pop**（app.js `renderRoute`）：按 `VIEW_ORDER` 序号差算方向，旧视图整体 `translateX(-34×dir)` 滑出（InQuint 160ms）→ 挂载 → 主机 `translateX(+46×dir)` 滑入（OutQuint 340ms）。新增 `routeToken` 守卫防快速连切竞态；退场用 `fill:'forwards'` + `getAnimations().cancel()` 清残留。
- **每视图签名入场**（`views.js` VIEW_META 新增 `enter` 字段 + app.js `playEntrance`）：overview=rise / theme=fade / components=up / motion=x / layers=scale / effects=elastic(OutElastic) / data=shear(复用库原语 `shearSlide` ±90px 交替)。全部 WAAPI，时长按 `1/rate` 手动预缩放（effects.js `motionScale()` 只认 reduce）。
- **日志面板改横向滑入**（translateX 20px，对齐上游通知从侧边滑入）。
- 回归：boot 主机滑入 1 anim；effects 途中采样 `cardScale=0.9, opacity=0`（elastic 起点命中）；反向切换 650ms 时主机入场动画在跑；reduce 开启后切换 0 动画。**命名 token 又中两次**：注释里的 `ModSelect` 是裸 token（已改写"斜切面板"）——写含上游类名的注释前先想 naming。
- 观感判定留给用户；若嫌退场 160ms 等待拖慢切换，可把 `await` 退场改为不等待并行滑动。

## 遗留待办

- overlay variant 队列错位修复与 sheared 安全区（`ui-reveal-overlay`）已在本会话早前完成，方向检验 5/5 PASS，但尚未做长时稳定性观察。
- 观感判定留给用户；如需继续：给 ui-select / ui-panel 等其余组件也进预览器 chrome、或把 `.pv-input`/`select` 换成 kit 组件（目前仅按钮组件化了）。
