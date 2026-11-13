# 调研报告 E：UI 调用逻辑 / 性能优化 / UXD（2026-11-13）

> 约定：`[FW]` = framework 提供（仓库内无 framework 源码，仅 NuGet 引用）；`[本项目]` = osu.Game 封装。路径相对 `p:/Dev/osu`，行号实测。

---

## E1. 调用逻辑

**DI 基座**。`[FW]` `Cached/Resolved/BackgroundDependencyLoader`、`DependencyContainer.CacheAs`、`Bindable/BindTo/UnbindFrom/BeginLease/GetBoundCopy`、`ScreenStack`、`LoadComponentAsync`、`DrawablePool`、`LifetimeEntry`。`[本项目]` `OsuGameBase.cs:77` 以 `[Cached(typeof(OsuGameBase))]` 为根，`:283-301` 缓存 Realm/RulesetStore/Storage/LargeTextureStore/OnlineAssetCachingStore/LocalConfig，`:307-308` 把 SkinManager 缓存为 `ISkinSource`，`:376-379` 把 `NonNullableBindable<WorkingBeatmap>` 双重 CacheAs，`:404-436` 缓存 GlobalActionContainer 与 KeyBindingStore。`OsuGame.cs:98` 再 `[Cached(typeof(OsuGame))]`、`:356-359` 自建容器、`:416-424` 在 `[BackgroundDependencyLoader] load()` 内 `CacheAs(osuLogo)`、`:1095/1167` CacheAs idleTracker / ScreenFooter。

**单向绑定流**。`OsuGameBase.cs:188-201` 定义全局 Ruleset / SelectedMods / AvailableMods；`OsuGame.cs:427-443` config↔Ruleset、`:445` UserActivity、`:447-453` config↔Skin、`:455-460` PlayingState 暂停导入、`:466-467` Mods/Beatmap 变更；切屏 `:1826-1836` `UnbindFrom` 旧屏 / `BindTo` 新屏的 `OverlayActivationMode` 与 `Activity`，`:1308-1311` 非 All 即 `CloseAllOverlays()`，`:1841-1844` 按 `HideOverlaysOnEnter` 处理。

**屏幕 lease**。`OsuScreen.cs:99-103` 暴露局部 Beatmap/Ruleset/Mods、`:111` `CreateLeasedDependencies`、`:115-135` 构造 `OsuScreenDependencies`；`OsuScreenDependencies.cs:23-57` 用 `BeginLease(false/true)` 或 `GetBoundCopy()` 建只读单向副本，`DisallowExternalBeatmapRulesetChanges` 决定是否独占；`OsuScreenStack.cs:45-58` 在 push 时**同步**创建依赖以保证 lease 状态 sane。

**叠层容器与 overlay 状态机**。`OsuGame.cs:155-169` 六个 overlay 容器字段；`:1108-1161` 层级 `ScreenOffsetContainer → ScalingContainer(ExcludeOverlays) → {ScreenStack, logoContainer, footerBasedOverlayContent, PopoverContainer/ScreenStackFooter}`，平行 `overlayOffsetContainer → {overlayContent, leftFloating, rightFloating}`，最上 `topMostOverlayContent`；`:247-250` 三个列表；`:279-332` `IOverlayManager.RegisterBlockingOverlay/Show/HideBlockingOverlay` + `updateBlockingOverlayFade()`；`:339-347` `CloseAllOverlays()`；`:1268-1306` 三组互斥（侧栏 / informational / 全屏）；`:1440-1456` `showOverlayAboveOthers()` 用 `Depth = -Clock.CurrentTime` 提层，已 present 则不改深度。

**三层导航**。主屏栈 `OsuScreenStack.cs:11,28-32`（内嵌 `ParallaxContainer → BackgroundScreenStack`）；背景栈 `OsuScreen.cs:146-148,227-234,254-255`；`BackgroundScreenStack.cs:25-39` 相等则拒绝 push 以复用背景（`:31` 首屏禁用入场动画）；子屏栈示例 `Screens/OnlinePlay/Matchmaking/Match/ScreenMatchmaking.ScreenStack.cs:42,63,101,110`。`IPerformFromScreenRunner.cs:18` / `OsuGame.cs:1005-1032` / `PerformFromMenuRunner.cs:22,49-58` 每 200ms `checkCanComplete` 逐层 Exit 到合法屏，`:64-68` Cancel/Expire。

**输入链**。`OsuGameBase.cs:410-429` SafeArea → Scaling → `GlobalActionContainer` → GlobalCursorDisplay → OsuTooltipContainer，另 `Add(new TouchInputInterceptor())`、`:640` 返回 `OsuUserInputManager`。`GlobalActionContainer.cs:15,17,35-45,83-85,87-125,274,640` 按 global>editor>ingame>songselect>audio>overlay 优先级。`OsuUserInputManager.cs:11-42` 绑定 PlayingState、右键改 `MouseButtonEventManager`（EnableDrag=true / EnableClick=false）。`TouchInputInterceptor.cs:22-62` 覆写 `Handle(UIEvent)` 按 MouseEvent/TouchEvent 写 `Static.TouchInputActive`。hover/press 集中在 `Graphics/UserInterface/`（31 文件）与 `Carousel.ScrollContainer.cs:292-339`。

**皮肤查找链**。`[FW] ISkinSource`；`Skinning/ISkin.cs:15-57` 四类 lookup；`SkinProvidingContainer.cs:28,74-84` `CreateChildDependencies` 取 ParentSource 并把自身 `CacheAs<ISkinSource>(this)`，`:100-181` 逐源 fallback 父源，`:196-214 SetSources`；`SkinnableDrawable.cs:15,44-56,76-100` `SkinChanged()` → `skin.GetDrawableComponent(lookup)` → 失败 `CreateDefault()` → `Cached scaling.Invalidate()`；`SkinReloadableDrawable.cs:16,30-41,71-85` 皮肤变更走 Scheduler；`BeatmapSkinProvidingContainer` / `RulesetSkinProvidingContainer` 为叠层特化。

## E2. 组件加载与渲染优化

| 层面 | 手段 | 证据 |
|---|---|---|
| 启动加载 | `loadComponentSingleFile<T>()` 先 `CacheAs` 再用 `asyncLoadStream` **串行链式**加载约 40 个 overlay（注释「限制单线程避免打满所有核心」），经 `ScheduledDelegate` 在更新线程调 `[FW] LoadComponentAsync`，10ms 轮询防 disposed | `OsuGame.cs:1164-1261,1551-1615` |
| Shader | 三路并发 `LoadComponentAsync` + 每帧一枚编译 + 全就绪门控 | `Loader.cs:67-106,114-158` |
| 资源 | `WorkingBeatmap.cs:36-60` `Lazy<Storyboard>/Lazy<ISkin>`、`:90-102` BeginAsyncLoad/CancelAsyncLoad、`:112-120` 换轨回收 waveform、`:143-156 TryTransferTrack` 跨难度复用；`OnlineAssetCachingStore.cs:27-49` 用 LargeTextureStore 包 onlineStore（`:51-81` 基于 realm 的磁盘缓存被整段注释禁用）；`OsuGameBase.cs:292-301,475-489` LargeTextureStore + `AddTextureSource(CreateOnlineStore())` + `InitialiseFonts()`；`PlayerLoader.cs:559-563,761` | 同上 |
| 批量绘制 | `BarGraph.cs:39,55,93,98,104-189` 只在数据/动画变化时 `Invalidate(Invalidation.DrawNode)`，单 DrawNode 内循环 `renderer.DrawQuad` 画全部柱；`SegmentedGraph.cs:22,80-86,155,194-248` 段合并 + `:218` 过滤亚像素段 + 单批次；`LineGraph.cs:89-120` `LayoutValue(Invalidation.DrawSize)` 缓存顶点 | 同上 |
| 遮罩/缓冲 | `ScalingContainer.cs:215-230` 仅非单位尺寸或安全区时开 `Masking/CornerRadius`（`:227` 注释承认全栈只应做一次）、`:262` `LayoutValue` 缓存 cursorRect；`Carousel.cs:318 Masking=false`；`Background.cs:19,50-92` 按需 `BufferedContainer(cachedFrameBuffer:true)` + `RedrawOnScale=false` + `FrameBufferScale` 降采样；`SongSelect.cs:325-326,866` 由 `SongSelectBackgroundBlur` 控制 20→0；`GlowingDrawable.cs:13` 直接继承 BufferedContainer | 同上 |
| Lifetime / 池化 | `HitObjectLifetimeEntry.cs:16,60-77,85-117,127-132`（`InitialLifetimeOffset=10000` 仅为延迟 Apply 的优化、`KeepAlive` 强制 ±∞）；`Pooling/PooledDrawableWithLifetimeContainer.cs:22,61,78-97,105-139,158-166` 由 `LifetimeEntryManager.Update` 只在 alive 时 `GetDrawable()`；`PoolableDrawableWithLifetime.cs:16,27-38,47-72,79-86,123-156` Apply/Free 与 Lifetime 联动；`AlwaysPresent=true` 用于 `BeatmapCardContent.cs:96`、`PlayButton.cs:45`、`LegacyDefaultComboCounter.cs:63,76` | 同上 |
| Carousel 虚拟化 | `Carousel.cs:93-98` DisplayableItems vs VisibleItems、`:247-251` 建议 DrawablePool、`:261-262` 只查 on-screen、`:359-425 performFilter` 全异步可取消、`:400-411` 过滤后 expirePanel、`:855-902` 重排 Y 并 `SetLayoutHeight`（`:894-896` 注释：多数 item 未加载仍可滚动）；`Carousel.ScrollContainer.cs:25-35` 注释「超大纵向列表内部用 double 精度 Y」、`:42-102` 自建 Panels 层 + `DrawYPosition` 自定义布局（`:55-61` 避免与 ScrollContent 反馈）；`BeatmapCarousel.cs:813-819,872-890` **7 个 DrawablePool**（100×4、11、9、8）、`:933-956 GetDrawableForDisplay` | 同上 |
| 帧稳定 | `FrameStabilityContainer.cs:19-25` 注释「保证每时钟秒 ≥50 帧」、`[Cached] IGameplayClock/IFrameStableClock`、`:34 max_catchup_milliseconds=10`、`:107-125 UpdateSubTree` do/while catch-up、`:127-209` BASS 异常跳变 >500ms 忽略 / `isCatchingUp>200ms`、`:254-279` 60fps 步长限幅；`GameplayClockContainer.cs:17-21,52,76-104,106-167` | 同上 |
| GC 会话 | `HighPerformanceSessionManager.cs:12-57` Interlocked 计数 + `GCSettings.LatencyMode=LowLatency` + `GC.Collect(0)`，退出恢复；`PlayerLoader.cs:176,191,399,445,699-701,708,744-765` 在 push 前 BeginSession、results/Dispose 结束 | 同上 |
| 画质降级 | `OsuConfigManager.cs:335-419` `DimLevel/BlurLevel/ShowStoryboard` `:343-347`、`HUDVisibilityMode` `:353`、`ShowFpsDisplay` `:396`、`SongSelectBackgroundBlur` `:400`、`ScreenshotFormat` `:405`、`BeatmapSkins/Colours/Hitsounds` `:407-409`、`PreferNoVideo` `:413`、Scaling 系列 `:414-419`（blur 默认关、BlurLevel 0–1 步进 0.01） | 同上 |
| 基准 | `osu.Game.Benchmarks/` 13 个 BenchmarkDotNet 用例**均为 CPU 算法**（`BenchmarkCarouselFilter.cs:12`、`BenchmarkBeatmapParsing.cs`、`BenchmarkHitObject.cs`、`BenchmarkDifficultyCalculation.cs`、`BenchmarkRealmReads.cs`、`BenchmarkGeometryUtils.cs` 等），**无 UI/DrawNode 绘制专项**；绘制回归靠 `osu.Game.Tests/Visual` 与 `Graphics/UserInterface/FPSCounter.cs` | — |

**优化取舍总结**：① 异步且节流（串行加载链、Loader 三路并发 + shader 每帧一枚、WorkingBeatmap Lazy/BeginAsyncLoad）；② 缓存（LargeTextureStore、在线资产、字体、Lazy Storyboard/Skin、TryTransferTrack、背景栈相等复用）；③ 池化/生命周期（7 个 DrawablePool、LifetimeEntryManager）；④ 帧稳定（60fps 限幅 + 10ms catch-up + GC LowLatency）；⑤ 批量绘制（单 DrawNode 多 DrawQuad）；⑥ 按需遮罩/缓冲；⑦ 画质降级开关。

**潜在风险**：(a) 单文件过大（`OsuGame.cs` 78KB、`BeatmapCarousel.cs` 54KB、`Carousel.cs` 50KB），串行加载仍可能单个组件长帧，Loader 三路并发与启动抢资源；(b) 层级过深（SafeArea→Scaling→GlobalAction→Cursor→Tooltip→ScreenOffset→ScreenContainer→ScreenStack→Parallax→BackgroundStack），`RelativeSizeAxes=Both` 层层传递导致 layout invalidation 级联（`ScalingContainer.cs:227` 自承）；(c) blur/buffered 滥用（`Background.cs:62` 每帧双 pass 模糊、`SongSelect.cs:866` blur=20、全仓 700+ 处 `Masking`/`AlwaysPresent`）；(d) `OnlineAssetCachingStore.cs:51-81` 磁盘缓存被禁用，只剩 framework 内存纹理缓存；(e) `WorkingBeatmap.cs:207-208` 注释承认 Beatmap getter 每帧走 async 偏贵；(f) `PerformFromMenuRunner` 每 200ms 轮询 + 深层退出 O(n) 绑定解绑；(g) 无 UI/DrawNode 基准。

## E3. UXD：交互反馈闭环

- **视觉 + 音效是组件化默认能力**：`Graphics/UserInterface/HoverSounds.cs:21,36-45` 只挂采样（hover 随机音高 0.02）、`HoverClickSounds.cs:42-58` 补 click/禁用采样；全库 53 个控件复用；`HoverSampleDebounceComponent.cs:18,30-44` 用 `SessionStatics.Static.LastHoverSoundPlaybackTime` 全局去重且滚动按键按下时不播；`OsuMenuSamples.cs:27-30,58-68` + `OsuMenu.cs:46,68-86` 按 open/sub-open/close 优先级合并播放。
- **Tooltip**：`Graphics/Cursor/OsuTooltipContainer.cs:27,39-55,94-114` 已局部可见时缩短延迟、内容去重、变更时 FlashColour、最大宽 500、阴影圆角、120ms 插值跟随；`ToolbarButton.cs:38-48,160-169` 主/副两行 tooltip + `HotkeyDisplay`，hover 提亮、click 白色 Flash（`:174-193`）。
- **焦点/键盘**：`GlobalActionContainer.cs:35-45,87-125` 全局键位（Up/Down 选择、Space/Enter 确认、Escape/鼠标侧键返回、Ctrl+O 设置）；`OsuFocusedOverlayContainer.cs:26,58-62,78-109,121-126,129-155` 统一 Back 关闭 + 屏外 mouse-up 关闭 + pop 采样 + 预览音停止；`OsuTextBox.cs:86-89,106-108,258-275` 焦点加 3px 边框、焦点色/提交色、光标移动与选字采样；`OsuDropdown.cs:41-46,166-190` Back 关闭 + 方向键预选；`OsuContextMenu.cs:40-46` 右键菜单 click 采样。
- **拖拽导入 / 触屏**：`OsuGame.cs:361-412,377` 因单次触发数百次回调，用 **100ms 防抖 + 批量 Import**；`Input/TouchInputInterceptor.cs:24-63` 透传位置输入仅写 `Static.TouchInputActive`；`osu.Android/OsuGameAndroid.cs:32,68-85,91` 改缩放目标尺寸、隐藏光标、按屏幕锁定/竖屏方向。

## E4. UXD：全局状态一致性与用户旅程

- `OverlayActivation` 三态（`Overlays/OverlayActivation.cs:6-11`）由当前屏绑定：`StartupScreen`/游玩中/主菜单退出时 Disabled（`Player.cs:86,561`、`MainMenu.cs:440`），`Player`/`Results` 为 UserTriggered（`ResultsScreen.cs:48`）；`IOverlayManager.cs:18-42` + `OsuGame.cs:283-318` 的 blocking overlay 把 `ScreenContainer` 变暗至 `Gray(0.5)`；`CloseAllOverlays()` `OsuGame.cs:339-347`。
- `DialogOverlay.cs:30-31,38-45,82-128,151-158,175-191` 自带 dim、Push 时先关旧框、Select 默认触发 OK、音乐 ducking；`NotificationOverlay.cs:132-146,175-203,254-269` toast tray + 分区、重要通知闪窗口、采样防抖，刻意 **250ms 延迟开启避免淹没用户**；`ProgressNotification.cs:109-161,237-239,266-281` 四态配色 + spinner，Close 需先 `CancelRequested`。
- `PerformFromMenuRunner.cs:58,79-81,136-184` 每 200ms 轮询退回菜单，遇确认框阻塞并对「框消失但屏未变」判定为中断、发 `ActionInterruptedByDialog`；`ScreenFooter.cs:299-311` 用「同时只允许一个 sheared overlay，否则抛异常」强约束；`ScreenStackFooter.cs:101-153` 按 None/Legacy/Argon 三态切换返回与操作区。
- **首次运行**：`OsuConfigManager.cs:176` `ShowFirstRunSetup` 默认 true；`FirstRunSetupOverlay.cs:39-44,54-69,72-97` 5~6 步（欢迎→UI 缩放→谱面→可选 stable 导入→行为），只在到达主菜单后展示，退出时可发「随时点击恢复」通知，走完才置 false；`WizardOverlay.cs:127-158,203-231,258-274` 返回、Select=下一步、按钮显示进度文案、200ms 后才显示 loading；`IntroScreen.cs:214-218` 8s 卡死兜底；`LoginOverlay.cs:75-82` pop-in 即 ChangeFocus；`AccountCreationOverlay.cs:111-131` 随 APIState.Online 自动隐藏；`MenuTipDisplay.cs:129-225` 30 条可关闭引导提示。
- **Settings IA**：Overlay→Panel→Section→Subsection 四级，固定 5+5+规则集动态分区，`SearchContainer` 过滤 + 常驻聚焦搜索，Section 首次点击只滚动居中并淡化非当前区（`SettingsOverlay.cs:73-105`、`SettingsPanel.cs:59,139,185-186,236,298-324`、`SettingsSection.cs:141-179`）。
- **游玩链**：`PlayerLoader.cs:136-143,310-312,385-390,679-706` `ReadyForGameplay` 要求「鼠标悬停/空闲、无拖拽、无 focused overlay」，含癫痫警告、静音/低电量提示；`ModSelectOverlay.cs:84-90,216-217,244-273` 文本搜索 + 精确缩写/名称预选 + 每次交互清空搜索 + 自定义/预设列。
- **Editor**：`Editor.cs:94-101,479-480,542-546,587-611,642-654,796-806,888-912` hash 比对 `HasUnsavedChanges`、退出/删除必弹 `PromptForSaveDialog`、重复退出 `Flash()`、Undo/Redo/Save 绑平台键并按 `CanUndo/CanRedo` 置灰、保存 toast；`EditorChangeHandler.cs:17-42,85-107` 50 状态上限；`SkinEditor.cs:182-188,315-345,518-548` 同样带 Undo/Clone/拖放。
- **多人房间**：`RoomStatusPill.cs:31-76` 每秒刷新状态文案与配色；`MultiplayerRoomSounds.cs:26-55` 加入/离开/被踢/换房主各有音效；`MultiplayerRoomPanel.cs:59-66` 仅 host/referee 显示 Change settings。

## E5. UXD：无障碍、个性化与设计规范

**明确缺失**：全库 `ReduceMotion` = 0，`\b(motion|colourblind|colorblind|high contrast|contrast ratio)\b` = 0 → **无 reduced-motion、无高对比/色盲配置**。可用「动效降级」只有零散开关：`MenuParallaxScale` 0–2 且 0 显示 Disabled（`GeneralSettings.cs:40-43`、`OsuConfigManager.cs:139-141`）、`IntroSequence`、`SeasonalBackgroundMode`（Sometimes/Always/Never）、MenuTips 开关（`MainMenuSettings.cs:31-69`、`SeasonalBackgroundLoader.cs:84-88`）。UI 缩放 0.8–1.6（移动端 0.8–1.1）+ `PlatformAction.ZoomIn/Out/Default`（`OsuConfigManager.cs:192-195`、`OsuGame.cs:1699-1709`、`ScalingContainer.cs:100-115`），并作为首次运行第 2 步（`FirstRunSetup/ScreenUIScale.cs:34-54`）。`UIHoldActivationDelay` 默认 200ms 长按防误触（`OsuConfigManager.cs:197`、`GeneralSettings.cs:45-51`，11 处 HoldToConfirm 组件）。本地化 99 个 `*Strings.cs` + `Language.cs`（含简/繁中，Crowdin = osu-web 项目，`README.md:11,137`）。键位可改且持久化 Realm，含禁用滚轮、冲突自动清空（`RealmKeyBindingStore.cs:96-118,164-199`）。SkinEditor 自由度极高（组件工具箱、拖放、锚点、设置侧栏、撤销）。

**设计规范来源**：`CONTRIBUTING.md:64,94` 主观视觉改动或大任务**必须先开 discussion/issue** 并指向 **Figma Master Library**；`:38-42` 主观反馈先走 discussion；`:71` 要求视觉测试；`README.md:44` 称 ruleset 复用 general UX；`OsuColour.cs:153,260,308,402-407` 对齐 osu-web `layout.less`/`colors.less` 与 Figma Tier-Colours/Asset-Colours，并区分 `OsuColour` 常量与 provider 主题色的使用语境；`CONTRIBUTING.md:5-7` 的 **AI/LLM 辅助贡献一律关闭并锁定**同样作用于设计提案，意味着设计流程强制人工、先讨论后实现。

**UXD 原则**：反馈是「可组合的基类」而非逐屏手写；全局状态由单一 `OverlayActivation` + `IOverlayManager` 收敛；危险/未保存操作强制确认且可撤销；文案全量 `LocalisableString`；视觉对齐 osu-web/Figma 且改动需先讨论。

**做得好**：hover 音全局防抖、拖拽 100ms 批量防抖、通知 250ms 防淹没、Editor 哈希式未保存判定 + 50 步撤销、Dialog 键盘默认按钮、Wizard 可恢复、Tooltip 自适应延迟、长按确认与键位冲突自清理、色值可溯源。

**隐患（均有代码证据）**：① 无 `ReduceMotion`，tooltip/footer/wizard 大量 300–600ms Easing 变换无全局降级；② 无高对比/色盲配置，房间状态、进度色依赖红/蓝/黄（`ProgressNotification.cs:237-239`）；③ 层级深（Settings 四级、footer 三模式、blocking overlay 例外，`OsuFocusedOverlayContainer.cs:28-32` 自注「临时方案」）；④ overlay 互斥靠运行时抛异常约束；⑤ `PerformFromMenuRunner` 靠 200ms 轮询 + 推测确认框是否「用户取消」（`:158-174`），启发式容错；⑥ 首次设置 5~6 步且强制回主菜单；⑦ `OsuFocusedOverlayContainer.cs:124` 留有 TODO：overlay 被 Disabled 拦截时「缺少视觉/听觉反馈」；⑧ 通知采样 popin/popout 共用防抖，代码自承可能漏播（`NotificationOverlay.cs:254-255`）。

## E6. 总体评价与改进优先级

**强项**：插件化/组件化彻底；视觉语言有单一真源；VFX 用 CPU 几何 + UBO + 加法混合 + 皮肤覆盖在极低 draw call 下做出高密度效果；启动预编译 + 池化 + 帧稳定 + GC 会话构成完整性能策略；皮肤 JSON 让用户能重排官方 UI。

| 级别 | 问题 | 建议落点 |
|---|---|---|
| P0 | 无 reduced-motion / 动效强度开关 | `OsuConfigManager` 加全局系数，统一替换 `Easing`/duration 字面量；至少让 Intro、Flash、Wave、弹性回弹可降级 |
| P0 | 时长/缓动无 token、进退场不成对 | 抽 `OsuAnimation` 常量类（FAST 120 / NORMAL 300 / TRANSITION 500 / INTRO 800 + 成对 easing） |
| P1 | 主题色无全局切换、`ChangeColourScheme()` 不广播 | 把 Hue 做成 `Bindable<int>` 并让 provider 颜色属性可绑定 |
| P1 | `CloseAllOverlays()` / 切屏不等动画结束 | 用 `LatestTransformEndTime` 或 `DelayUntilTransformsFinished()` 建栅栏，替代 200ms 轮询 |
| P1 | shader 源码在外部 NuGet、`osu!.res` 无引用 | 自定义 VFX 按 `DrawableRulesetDependencies.cs:71` 的 `NamespacedResourceStore(...,"Shaders")` 放本地 shader 并加入 `Loader.ShaderPrecompiler`；`osu!.res` 待确认是否可清理 |
| P2 | 无 UI/DrawNode 绘制基准 | 在 `osu.Game.Benchmarks/` 补 DrawNode/几何生成用例 |
| P2 | overlay 阻断无反馈、通知采样漏播、色觉适配 | `OsuFocusedOverlayContainer.cs:124` TODO、`NotificationOverlay.cs:254-255`、`ProgressNotification.cs:237-239` |

## E7. 关键路径速查

| 主题 | 路径:行 |
|---|---|
| DI/绑定根 | `OsuGameBase.cs:77,188-201,283-301,307-308,376-379,404-436,475-489,640`；`OsuGame.cs:98,356-359,416-467,1095,1167` |
| overlay 状态机 | `OsuGame.cs:155-169,247-250,279-347,1108-1161,1268-1311,1440-1456,1826-1846` |
| 组件加载 | `OsuGame.cs:1164-1261,1551-1615`；`Loader.cs:67-106,114-158` |
| 屏幕 lease / 三层栈 | `OsuScreen.cs:99-135,146-148,227-234,254-255`；`OsuScreenDependencies.cs:23-57`；`OsuScreenStack.cs:11,28-32,45-58`；`BackgroundScreenStack.cs:25-39` |
| 输入 | `GlobalActionContainer.cs:15-45,87-125,274,640`；`OsuUserInputManager.cs:11-42`；`TouchInputInterceptor.cs:22-62` |
| 皮肤链 | `ISkin.cs:15-57`；`SkinProvidingContainer.cs:28,74-84,100-181,196-214`；`SkinnableDrawable.cs:15,44-56,76-100` |
| 批量绘制/缓冲 | `BarGraph.cs:39-189`、`SegmentedGraph.cs:22-248`、`LineGraph.cs:89-120`、`ScalingContainer.cs:215-230,262`、`Background.cs:19-92`、`GlowingDrawable.cs:13` |
| 生命周期/池 | `HitObjectLifetimeEntry.cs:16-132`、`PooledDrawableWithLifetimeContainer.cs:22-166`、`PoolableDrawableWithLifetime.cs:16-156`、`BeatmapCarousel.cs:813-819,872-890,933-956` |
| 帧稳定/GC | `FrameStabilityContainer.cs:19-331`、`GameplayClockContainer.cs:17-167`、`HighPerformanceSessionManager.cs:12-57`、`PlayerLoader.cs:699-765` |
| UXD 反馈 | `HoverSounds.cs:21-45`、`HoverSampleDebounceComponent.cs:18-44`、`OsuTooltipContainer.cs:27-114`、`ToolbarButton.cs:38-48,160-193`、`GlobalActionContainer.cs:87-125` |
| UXD 一致性 | `OverlayActivation.cs:6-11`、`IOverlayManager.cs:18-42`、`DialogOverlay.cs:30-191`、`NotificationOverlay.cs:132-269`、`ProgressNotification.cs:109-281`、`ScreenFooter.cs:299-311` |
| 用户旅程 | `FirstRunSetupOverlay.cs:39-97`、`WizardOverlay.cs:127-274`、`SettingsOverlay.cs:73-105`、`PlayerLoader.cs:679-706`、`Editor.cs:587-611`、`RoomStatusPill.cs:31-76` |
| 无障碍缺口 | `OsuConfigManager.cs:139-141,192-202`、`GeneralSettings.cs:40-51`、`MainMenuSettings.cs:31-69`（`ReduceMotion`/色盲/高对比全库 0 命中） |
| 规范 | `CONTRIBUTING.md:5-7,38-42,64,71,73,94`；`README.md:44,137,139` |

（状态：报告 E 完成，2026-11-13；与 A–D 及 `INDEX.md` 共同构成 osu 项目记忆。）
