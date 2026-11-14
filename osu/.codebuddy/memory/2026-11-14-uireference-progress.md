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

## 组件扩容 + i18n 扩容（2026-11-14 追加 2）

用户指令"继续 UI 资源的榨取与翻译"，本轮产出：

- **ui-textbox**（04-components/ui-textbox/）：依据 `OsuTextBox.cs:73-93,258-282,321-397` + `SearchTextBox.cs:15-19` + `OsuNumberBox.cs`。聚焦 3px 描边、Background5/4/Commit 三态背景（→ --ui-surface-sunken/surface/accent-weak）、逐字坠落入场（镜像 span 测宽 + transient overlay，≤3 字符/次、池上限 6）、光标 500ms InOutSine 节拍脉冲、Enter 提交闪烁、number 变体非法输入红闪+回退+ui-text-error、search 变体高 35。宿主 mousedown 转发焦点到内部 input。
- **ui-slider**（04-components/ui-slider/）：依据 `RoundedSliderBar.cs:62-129,190-205` + `Nub.cs:82-105,162-178` + `OsuSliderBar.cs:61-86`。accent/背景双箱 5px 轨道、nub 50×15 永久 filled、left 250ms OutQuint（拖拽中禁用过渡）、hover/drag/focus 辉光（40ms 闪+800ms 回落）、30ms 节流 nub tick 脉冲（WAAPI）、双击重置初始值（`_defaultLocked` 只在 connectedCallback 锁基准——`_sync` 曾用当前值覆盖 `_default` 导致重置失效，已修）、方向键/Shift×10/PgUp·PgDn/Home·End、aria slider 三值、tooltip 跟随。
- **i18n 29→90 键**（en/zh-CN ×2，UTF-8 LF，`_inbox/gen-i18n.py` 生成）：CommonStrings/UserInterfaceStrings/DialogStrings/ContextMenuStrings 四源；新增 dialog.*、context.* 命名空间；剔除与既有键映射冲突的 beatmaps/scores/skins/mod_presets（→items/results/themes/presets 已存在）。**i18n 键名与文案都在命名表的替换范围内**：`intro→entrance`（ui.entrance_sequence）、`beatmap→item`、`谱面→条目`、`主菜单→首页` 等——生成 i18n/写注释前先对照 naming-map.json 第 175-192/295 行词表，直接写映射后的中性词，dry-run 才能归零。
- **i18n 热切换**：app.js `setLang` 在 loadI18n 后派发 `document` 级 `ui-i18n-change`；ui-button / ui-textbox 监听并 `_sync`（此前语言切换后 data-i18n-key 文案不更新）。
- 组件数 5→7，组件视图卡片 5→7；装配 `validation: ok`、naming dry-run 0、7 视图挂载、回归断言全绿（textbox change/commit/error 计数、slider 72→70 重置、i18n 占位"用户搜索"）。
- `_inbox/gen-i18n.py` 保留为 i18n 再生成脚本；`dump-strings.py`/`strings-dump.txt` 已删。

## ui-dropdown + 表格溢出修复（2026-11-14 追加 3）

用户点名"下拉菜单/列表的动效样式"与"表格内容超出卡片边界"（截图：theme 视图 settings 表"作用"列被卡片右缘裁切）：

- **ui-dropdown**（04-components/ui-dropdown/，组件 7→8）：依据 `OsuDropdown.cs:79-127,247-254,326-335,346-462`。菜单开合 = 淡入/淡出 300ms OutQuint + 高度 0↔scrollHeight WAAPI（UpdateSize 复刻，菜单 absolute 定位故高度动画不影响布局——注意不要加 `height:0 !important` 的关闭兜底，会掐死退场动画）；选项悬停条 100ms 入/600ms 出（CSS 双 transition-duration 技巧）、悬停 chevron -3px→0 滑入 400ms/退场 200ms In、头部 chevron scaleY(-1) 翻转、标签 TruncatingSpriteText 省略、方向键预选/Enter/Esc/外点关闭、disabled alpha 0.3。选项用未注册的 `<ui-option value label>` 子元素声明。
- **表格溢出**：preview.css 给 `.pv-table th/td`、`.pv-table td code`、`.pv-kv dt/dd` 加 `overflow-wrap: anywhere` + `.pv-table max-width:100%`——`--theme-hue` 这类不可断 token 自动换行，卡片 `scrollWidth <= clientWidth` 全部成立（theme/data 视图实测 fits=true）。
- 命名 dry-run 0；组件视图 7→8 卡；断言：开启动画 1 running、menuH 133 中程采样、chevron 已翻转、选择后 badge 更新且菜单关闭。
- 教训：run-code 输出的 `### Result` 在整个输出的**前段**，用 `-Last N` 截取会误判为失败；`page.$eval` 的 `$` 会被 PowerShell 当变量展开，改用 `page.evaluate`。

## 资源普查 + ui-tabs/ui-checkbox（2026-11-14 追加 4）

用户指令"查漏补缺，没有则继续抽取"。普查结论：库内全部源文件（8→10 组件、motion/VFX 全链、themes/tokens/i18n/audit）均已入预览器，无遗漏 → 进入继续抽取：

- **ui-tabs**（04-components/ui-tabs/）：依据 `OsuTabControl.cs:31-53,88-101,121-146,177-213`。间距 10、tab 文字 14px 上下边距 5、悬停/激活底栏 1px 淡入 500ms OutQuint（离开淡出）、激活加粗白字/未激活 accent、`strip` 属性启用整行底线（left/width 500ms OutQuint 跟随激活项——strip 与 .tabs 同级，`:host` 需 position:relative）、方向键/Home/End、横向溢出滚动。选项用未注册的 `<ui-tab value label>` 声明。
- **ui-checkbox**（04-components/ui-checkbox/）：依据 `OsuCheckbox.cs:42-113` + `Nub.cs:82-105,162-178`。nub 靠右、标签 padding-right 60、选中 width 38→50 + 内环 3→8.5 OutElasticHalf 200ms、未选反向 OutQuint、悬停辉光（40ms 闪）、切换脉冲 WAAPI、role=switch、空格/回车。
- **两个新坑**：① `??` 与 `||` 链式混用是 SyntaxError（`a ?? (b) || c` 也报错），拆成多行赋值；② 演示数据里的 `value="multi"` 撞命名 token `"multi"→"group"`——演示/测试数据用词也要对照 naming-map。
- 组件 8→10，组件视图 8 卡；`validation: ok`、naming 0、断言全绿（tabs 加粗/底栏渐入/strip 58px 跟随、cb 41px 中程宽/3 anims、i18n 标签热更新）。

## 遗留待办

- overlay variant 队列错位修复与 sheared 安全区（`ui-reveal-overlay`）已在本会话早前完成，方向检验 5/5 PASS，但尚未做长时稳定性观察。
- 观感判定留给用户；如需继续：给 ui-select / ui-panel 等其余组件也进预览器 chrome、或把 `.pv-input`/`select` 换成 kit 组件（目前仅按钮组件化了）。
