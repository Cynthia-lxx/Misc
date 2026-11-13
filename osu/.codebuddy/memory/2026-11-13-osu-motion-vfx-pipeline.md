# 调研报告 C：动效系统 + 图形渲染 / VFX 底层（2026-11-13，重点）

> 静态分析 + 主代理亲自核验（`Loader.cs`、`TrianglesV2.cs`、`LogoAnimation.cs`、`ModFlashlight.cs`、`PositionAndColourVertex.cs`、`osu!.res` 文件头）。路径相对 `p:/Dev/osu`。

---

## C1. 定量

| API 类别（全仓 `.cs` 正则计数） | 命中 |
|---|---|
| Transform 族（`FadeIn/FadeOut/FadeTo/ScaleTo/MoveTo*/RotateTo/ResizeTo/TransformTo/FlashColour`） | 2,622 |
| `Easing.*` | 2,408（`OutQuint` ≥404/494 ≈ 8 成） |
| 混合/遮罩/缓冲（`BlendingParameters`/`Additive`/`BufferedContainer`/`Masking`） | 1,257（其中 Additive 119+） |
| Glow / EdgeEffect | 726 |
| 时间序列（`Delay/Then/Loop/BeginAbsoluteSequence/BeginDelayedSequence`） | 503 |
| 池化/异步/失效（`DrawablePool`/`LoadComponentAsync`/`AlwaysPresent`/`Invalidate(`） | 506 |
| shader/自定义渲染（`IShader`/`ShaderManager`/`BindUniformBlock`/`CreateQuadBatch`/`CreateDrawNode`/`Uniform*`/自定义顶点） | 138 |

细项：`FadeIn` ≥277、`FadeOut` ≥359、`FadeTo` ≥214、`CreateDrawNode()` **18 处**、`IVertexBatch/CreateQuadBatch` 12 文件、`BindUniformBlock` 7 处。

## C2. 动效基础与编排范式

全部动效由 framework `Drawable` 的 `Transform/TransformSequence` 驱动，**无独立动画引擎、无全局时长/缓动 token**。

- 主流写法：链式 `FadeIn().MoveToY().Delay(delay).FadeIn()`（`Overlays/Mods/ModSelectOverlay.cs:485-507`、`SettingsPanel.cs:255-259`、`ShearedButton.cs:202-249`）；`.Then()` 两段式（`OsuButton.cs:152-154`、`Nub.cs:91-97`、`IntroScreen.cs:268-270`）；开场用 `BeginAbsoluteSequence/BeginDelayedSequence` 时间轴（`IntroTriangles.cs:195-261`）；非标准属性用 `TransformTo(nameof(...))`（`ShearedButton.cs:192`、`Nub.cs:171`、`IntroTriangles.cs:209/241`）。
- `FinishTransforms(true)` 在 `LoadComplete` 把初始状态吸附到位（`OsuAnimatedButton.cs:96`、`ShearedButton.cs:139`、`IntroScreen.cs:338`）；`AlwaysPresent` 只用于 alpha=0 仍需布局/输入（`Toolbar.cs:51,59`、`Nub.cs:50`、`OsuLogo.cs:235`），常配 `IsPresent => base.IsPresent || Scheduler.HasPendingTasks` 保活（`BackgroundScreen.cs:20`、`DialogOverlay.cs:38`、`NotificationOverlay.cs:167`）。
- **全仓没有 Transform `Flush()`**；取消靠 `ClearTransforms()` + `DelayUntilTransformsFinished()`（`OsuLogo.cs:258-275`）。

**时长/缓动约定（惯例而非 token）**：

| 场景 | 时长 | 缓动 |
|---|---|---|
| 短反馈（hover/focus/caret/nub） | 40–200ms | OutQuint |
| 标准转场（overlay / 背景 / 屏幕） | 300–600ms | 进场 OutQuint；退场混用 OutQuint / InQuint / OutExpo / InOutQuart |
| 入场编排（Intro / Wave / 弹性） | 600–1000ms | OutQuint / OutSine / OutElastic |
| 长时序（评级圆环 accuracy 填充） | 3000ms | OutPow10 |

`OutElastic/OutElasticHalf` 专用于按压回弹与弹窗；`InOutSine` 仅 11 处且多用于按拍脉冲（`OsuTextBox.cs:394`）。

## C3. 组件级反馈 / 转场 / 开场编排

- **音效与视觉解耦**：`Graphics/UserInterface/HoverSounds.cs:21-46` 只挂采样（hover 随机音高 ±0.02）、`HoverClickSounds.cs:49-58` 补 click/禁用采样；`HoverSampleDebounceComponent.cs:30-44` 全局去重。`OsuButton.cs:133,141,148-176` hover 40→800ms 两段、点击 `FadeOutFromOne(500)`、按下 `ScaleTo(0.9, 4000)` / 松开 `ScaleTo(1, 1000, OutElastic)`；`ShearedButton.cs:139-249` 150ms 颜色状态 + 800ms flash + 240ms/OutCubic Footer 入场；`OsuAnimatedButton.cs:91-133` 加 `FlashColour(...,800)`。
- **输入控件**：`OsuTextBox.cs:258-276,302-319,323,348-395` focus 只切 3px 边框、caret 60/200ms、按拍 `FadeTo(0.7).FadeTo(0.4, beatLength, InOutSine)`、键盘采样 15ms 去抖；`OsuSliderBar.cs:61-83` 30ms 去抖并按值变调；`Nub.cs:64-104,162-177` 靠 `Glowing` 触发 40→800ms Glow。
- **容器**：`OsuFocusedOverlayContainer.cs:22-24,115-155` 只管 pop-in/out 采样与 `OverlayActivation` 门控；`WaveContainer.cs:19-24,123-142,184-191` 800/500ms + OutSine/InSine 四波错位；`ShearedOverlayContainer.cs:132-174` 400ms 进 / 500ms 出并联动 Footer；`ParallaxContainer.cs:20-29,59-93` 是**每帧阻尼插值而非 Transform**（位置 100ms、缩放 1000ms、幅度 `0.02 × MenuParallaxScale × BackgroundParallaxAmount`）。
- **屏幕/overlay 转场**：`BackgroundScreen.cs:16-22,47-88` 固定 500ms、位移 50px、进 InOutQuart / 出 OutExpo，首屏由 `BackgroundScreenStack.cs:30-31` 关掉入场动画；`Toolbar.cs:40,51,59,241-246,287-299` 500ms（PopIn 仅 125ms、PopOut InQuint）；`NotificationOverlay.cs:40,230-248` 600ms + `FadeEdgeEffectTo`；`DialogOverlay.cs:151-173` 遮罩 500ms + 100ms duck；`PopupDialog.cs:27-28,294-300,324-354` `ENTER/EXIT_DURATION=500`、ring `OutElasticHalf(750)`；`SettingsPanel.cs:36,170-204,221-259` 600ms + 侧栏 40ms 逐项错峰；`ModSelectOverlay.cs:477-577` 400/500ms + 每列 30ms 错峰并同步声像；`WizardOverlay.cs:160-182,277-285` 0.99→1 缩放 400ms。`CloseAllOverlays()`（`OsuGame.cs:339-347`）只是逐个 `Hide()`，**不等动画完成**；`PerformFromMenuRunner.cs:58,70-100` 靠 200ms 轮询。
- **开场编排**：`Screens/Loader.cs:40-63,67-106,117-158` 三路并发 `LoadComponentAsync`（ShaderPrecompiler / IntroScreen / LoadingSpinner）+ `checkIfLoaded()` 门控，spinner 延迟 200ms；按 `IntroSequence` 选 Circles/Welcome/Triangles，季节 intro 由 `SeasonalUIConfig.ENABLED`（`:14`，当前 false）控制。`IntroScreen.cs:64,190-194,213-292,321-381` 3000ms outro、8s 卡死兜底、音量 `.VolumeTo().Then().VolumeTo()`；`IntroTriangles.cs:171-280,361-380` 200/400/700/900/1060 → 1450/1650/1850 → 2080(+920) → 3000ms 绝对时间轴，末尾 `GameWideFlash` 1000ms、Glitch 三角 22ms 生成 / 120ms 淡出；`IntroCircles.cs:22-24,40-60` 600ms 起曲、2900ms 进菜单；`IntroWelcome.cs:28,84-101,156-167` 2142ms 主序列。`OsuLogo.cs:36,84-95,258-275,291-359,385-452` 300ms 过渡 + 按拍缩放/ripple/kiai flash、`PlayIntro()` 3150+200ms、hover 500 OutElastic、按下 1000/松开 500 OutElastic、拖拽 800 OutElastic。`LogoVisualisation.cs:31-56,122-145` 不是 Transform，而是 50ms 采样 + 每帧衰减。`MainMenu.cs:52,254-296` 按钮 `FadeInFromZero(500)`、logo 100ms、sideFlashes `Delay(300).FadeIn(64, InQuint)`。

## C4. 光效型动效与开关缺口

`FlashColour` 常配 Additive 层与 `FadeInFromZero/FadeOutFromOne`（`OsuAnimatedButton.cs:119`、`ToolbarButton.cs:174-193`、`PopupDialog.cs:294-300`、`SkipOverlay.cs:430`、`ArgonScoreRankDisplay.cs:296,344-346`、`SupporterDisplay.cs:134 .Loop()`）；`EdgeEffect/FadeEdgeEffectTo` 两段式 40→800ms Glow/Shadow（`Nub.cs:64-104`、`ShearedNub.cs:125,205-211`、`NotificationOverlay.cs:234/247`、`SettingsPanel.cs:174/195`、`Panel.cs:357`、`DefaultHealthDisplay.cs:116-118`）。**仓库无 Shimmer/Scanline 文件**；Ripple 仅 `osu.Game.Rulesets.Osu/UI/Cursor/CursorRippleVisualiser.cs:41-55` 与 OsuLogo 的 ripple 精灵；季节光效在 `Seasonal/MainMenuSeasonalLighting.cs`、`SeasonalMenuSideFlashes.cs`。

**关键缺失**：全仓 `ReduceMotion` / `prefers-reduced-motion` / `UIAnimation` **0 命中**，**没有全局动效降级、没有系统级 reduced-motion 读取**。只有逐项开关：`MenuParallaxScale` 0–2（0 即禁用，`GeneralSettings.cs:38-44`）、`CursorRotation`、`IntroSequence`、`SeasonalBackgroundMode`、`MenuBackgroundSource`、`MenuVoice/MenuMusic/MenuTips`、`UIHoldActivationDelay`、`ShowStoryboard/HitLighting/StarFountains`（`OsuConfigManager.cs:139-141,197-202,377-380,421,430`）；旧 `MenuParallax` 仅在迁移中保留（`OsuGame.cs:1400-1404`）。`HighPerformanceSessionManager.cs:20-57` 只切 `GCLatencyMode.LowLatency` + `GC.Collect(0)`（由 `PlayerLoader.cs:701,748,765` 在游戏加载期使用），**不影响任何动画**。

**动效不一致**：① 时长全是字面量或局部 const（同类转场 400/500/600/800 并存）；② 缓动不成对（进场几乎全 OutQuint，退场混用 OutQuint/InQuint/OutExpo/In/InSine）；③ `Flush()` 未用于 Transform；④ overlay `Hide()` 与屏幕切换无统一「等动画结束」栅栏，`PerformFromMenuRunner` 靠 200ms 轮询而非 `LatestTransformEndTime`（仅 `ShearedButton.cs:236` 用了该模式）；⑤ 淡入、弹性、闪光、开场序列均不可关闭。

## C5. shader 资源链路（VFX 基础设施核心）

**`osu.Game/osu!.res` 判定**（主代理实测）：156,596 字节，头 `00 00 00 00 20 00 00 00 FF FF 00 00 FF FF 00 00` 为自定义二进制，非 GLSL 文本；全仓文本检索 `osu!.res` **0 命中**；`osu.Game.csproj` 无任何 `EmbeddedResource`，`Directory.Build.props:22` 只内嵌 `Resources\**\*.*` 而 `osu.Game/Resources` 目录**不存在** → 随快照签入、**当前无代码/MSBuild 引用的历史资源存档**，不要误当 shader 源。

**shader 源码在 NuGet 包里**：`osu.Game.csproj:41-42` 引用 `ppy.osu.Framework 2026.1111.0` 与 `ppy.osu.Game.Resources 2026.1112.1`；`OsuGameBase.cs:281` 把 `OsuResources.ResourceAssembly` 用 `DllResourceStore` 加进 `Game.Resources`；framework `ShaderManager` 从 `Shaders/` 命名空间读 GLSL 编译。规则集可覆盖：`Rulesets/UI/DrawableRulesetDependencies.cs:64-71,201-216` 用 `NamespacedResourceStore(resources, "Shaders")` 构造 `RulesetShaderManager`（`GetRawData/GetCachedShaderPart` 逐级回退全局 manager）；测试 `TestSceneRulesetDependencies.cs:57` 加载 ruleset 本地 `TestFragment`。本仓库 **不含** `.fs/.vs/.frag/.vert/.glsl`；`DistributedResourceStore`、`GLWrapper`、`GetShader`、`Uniform<` 全仓 0 命中 —— 均属 framework 内部。

**启动预编译**（亲自核验 `Screens/Loader.cs:117-158`）：`ShaderPrecompiler` 在 `[BackgroundDependencyLoader]` 里 `manager.Load(...)` **15 次** —— 3 组内置组合（`TEXTURE_2+TEXTURE`、`TEXTURE_2+BLUR`、`TEXTURE_3+TEXTURE`，`:126-128`）+ 12 具名自定义：`TriangleBorder`、`FastCircle`、`CircularProgress`、`ArgonBarPath`、`ArgonBarPathBackground`、`SaturationSelectorBackground`、`HueSelectorBackground`、`Path`、`PathPrepass`、`LogoAnimation`、`SpinnerGlow`、`CursorTrail`（`:130-143`，其中 `:141` 注释承认「Ruleset local shader usage 应移走」）。`:146-157` 每帧检查 `AllLoaded`（framework 每帧只编译一枚以保留绘制帧，注释 `:115-116`），全就绪才 `FinishedCompiling=true`；`:91-95` `checkIfLoaded()` **阻塞进入主界面**直到完成。

**完整链路**：

```
NuGet(framework + osu.Game.Resources) 的 GLSL / 纹理 / 字体
  → OsuGameBase.cs:281 DllResourceStore 注入
  → framework ShaderManager 编译（ruleset 可经 NamespacedResourceStore 覆盖）
  → Loader.cs:126-143 预编译清单 + 逐帧门控
  → Drawable 的 [BackgroundDependencyLoader] shaders.Load(...)
  → CreateDrawNode() / ApplyState() 快照状态
  → Draw(IRenderer): shader.Bind() + BindUniformBlock(UBO) + DrawQuad / CreateQuadBatch
  → framework 批次 → GPU → 屏幕
```

`Blending`/`Blur`/`EdgeEffect` 由 framework 在 Drawable 层生效。

## C6. 自定义渲染层（本仓库实现的部分）

`Graphics/OpenGL/` **只有一个类**：`Vertices/PositionAndColourVertex.cs:13-25`，`[StructLayout(LayoutKind.Sequential)]` + `IVertex` + `[VertexMember(2, Float)] Vector2 Position` / `[VertexMember(4, Float)] Color4 Colour`，唯一使用者 `ModFlashlight.cs:299-304`（已亲自核验全文）。其余自定义顶点在业务文件：`LogoAnimation.cs:115-138` 的 `LogoAnimationVertex`、`CursorTrail.cs:406-423` 的 `TexturedTrailVertex`。

全仓 `CreateDrawNode()` 18 处、`IVertexBatch/CreateQuadBatch` 12 文件、`BindUniformBlock` 7 处。全部基于 framework `IRenderer` 抽象（`DrawQuad`、`CreateQuadBatch`、`CreateUniformBuffer`），**无直接 GL 调用**。两类：① 纯批量几何（`BarGraph`、`SegmentedGraph`、`LogoVisualisation`、`Triangles*`）；② shader + UBO 驱动（`Triangles*`、`LogoAnimation`、`GhostIcon`、`ArgonBar*`、`ModFlashlight`、`CursorTrail`、`ArgonSpinnerProgressArc`）。`SmokeSegment`（烟雾分段）与 `CentreMarker`（时间轴三角）也是自定义 DrawNode，但只用默认 texture shader。

## C7. 逐个 shader / DrawNode 实现（VFX 核心清单）

| 实现 | shader | UBO / 机制 | 效果与差异 | 关键行号 |
|---|---|---|---|---|
| `Backgrounds/Triangles.cs` | `TEXTURE_2 + TriangleBorder` | `m_BorderData{Thickness,TexelSize}`（`TriangleBorderData.cs:9-15`），`Thickness=1` 实心、`TexelSize=0` | 按面积定量 `AimCount=W*H*0.002/scale²`、越界**原地重置**、逐粒子 `ColourShade`、反序绘制、alpha³ 淡出 | `:117,130-133,161,185-187,313-322,339-340,411` |
| `Backgrounds/TrianglesV2.cs` | 同上 | `Thickness=0.02` **描边**、`texelSize=1.5/quad.Height` 抗锯齿 | 按宽度定量 `AimCount=W*0.02*SpawnRatio`、越界 `RemoveAt`、每帧增量补充、`ColourInfo.Interpolate(quad)` 上色；**更省、无逐粒子颜色/缩放状态** | `:79,102-152,191-283`（已亲验 `:76-81,90-151`） |
| `Sprites/LogoAnimation.cs` | 自定义 VS+FS `LogoAnimation` | `m_AnimationData{Progress}`；`SpriteDrawNode` 重写 `BindUniformResources`，`Blit` 用 `vertexAction` 把输出重定向到自定义 quad batch | logo 程序化动画，`AnimationProgress` 变更只 `Invalidate(Invalidation.DrawInfo)` | `:26,39,45-105,115-138`（已亲验 `:23-27,31-45,69-104`） |
| `Graphics/GhostIcon.cs` | `Ghost` FS + `WhitePixel` | `m_GhostParameters{Time}`，2000ms 周期、每帧失效 DrawNode | 无贴图、手写 4 个 `TexturedVertex2D` + `BlendRange` 抗锯齿的纯程序化图形 | `:28,33,40,43-45,60-129,131-136` |
| `HUD/ArgonHealthDisplayParts/ArgonHealthDisplayBar.cs` | `ArgonBarPath` | `m_ArgonBarPathParameters{Bar/Glow 双色, GlowPortion, Size, ProgressRange, PathRadius}` | 圆角路径血条 + 发光/脉冲；背景 `ArgonHealthDisplayBackground.cs:21,45-68` 用 `ArgonBarPathBackground`；`ArgonHealthDisplay.cs:102,111` 叠加加法混合 | `:99,102,142-178` |
| `Rulesets/Mods/ModFlashlight.cs` | `PositionAndColour` VS + 子类 FS（osu/catch/taiko = `CircularFlashlight`，mania = `RectangularFlashlight`） | `m_FlashlightParameters{Position,Size,Dim,Smoothness}` | 黑色遮罩 + 柔边光锥；combo≥100/200 收缩（`GetComboScaleFor` `:190-198`）、break ×2.5（`:188`）、`PlayfieldDrawInfoTracker` 跟随缩放（`:348-368`）；尺寸常量注释对齐 osu-stable-reference `:184-185` | `:113,136,148-199,256-339,348-368`（已亲验 `:113,133-137,144-198`） |
| `Select/BeatmapMetadataWedge.FailRetryDisplay.cs` | `FastCircle` | `DampContinuously` 平滑数据 | 给 100 根柱画圆角，**避开 Masking 并合成单批次** | `:105,118,126,166-225` |
| `Menu/LogoVisualisation.cs` | **非自定义 shader**（`:91` 默认 texture） | 200 柱 × 5 圈、`IVertexBatch<TexturedVertex2D>(100,10)`、加法混合、`inflationPercentage` 抗锯齿 | 50ms 采样 256 频段 → 径向频谱，逐帧衰减、kiai ×1 / 非 ×0.5 | `:77,91,99-145,194-245` |
| `Osu/UI/Cursor/CursorTrail.cs` | `CursorTrail` VS | `m_CursorTrailParameters` | 光标拖尾淡出 | `:102,228,288-302,406-423` |
| `Osu/Skinning/Argon/ArgonSpinnerProgressArc.cs` | `SpinnerGlow` + `Blur.KernelSize(50)` | 继承 `CircularProgressDrawNode` | spinner 进度弧加性辉光 | `:101,107,141-169` |
| `UserInterface/SegmentedGraph.cs` / `BarGraph.cs` | **不是自定义 shader**（默认 `TEXTURE_2`/`TEXTURE`） | 单 DrawNode 内循环 `renderer.DrawQuad` | `SegmentedGraph.cs:118-153,250-257` tier 分层 + `ColourInfo.Interpolate/ApplyChild`；`BarGraph.cs:104-189,242-252` 插值动画 + 最小 1.5px 抗锯齿 | `SegmentedGraph.cs:73,155,194-258`；`BarGraph.cs:78,104-189,242-252` |

**归属界线**：GLSL 源码、`ShaderManager`/`IRenderer`/`ShaderPrecompiler` 编译调度、`BufferedContainer`/`Blur`/`EdgeEffect`/`CircularProgress`/`BlendingParameters`/`TexturedVertex2D`/`IVertexBatch` 均由 **framework + 资源 NuGet 包提供**；上表 C# 文件（DrawNode、UBO 布局、粒子状态机、几何生成、shader 名绑定）才是**本仓库实现**。

## C8. 光效工具箱（framework 能力在本项目的用法）

| 手段 | 命中 | 代表 |
|---|---|---|
| `BlendingParameters.Additive` | 119+ | 粒子、频谱、HUD、季节光效、判定闪光 |
| `EdgeEffect` / `FadeEdgeEffectTo` | 726 类命中 | `UserInterfaceV2/OsuPopover.cs:40`、`ShearedSliderBar.cs:149-163`、`Nub.cs:64-101` |
| `BufferedContainer` + `Blur` | 按需创建 | `Graphics/Backgrounds/Background.cs:44-91`（`cachedFrameBuffer:true`、`RedrawOnScale=false`、`FrameBufferScale` 降采样并同比缩 BlurSigma、`BlurTo` 变换）、`Graphics/Sprites/GlowingDrawable.cs:13-37`（`Blur.KernelSize` 膨胀 quad、`EffectColour`）、`GlowingSpriteText.cs:63-64` |
| `FlashColour` | 多处 | 见 C4 |
| `ColourInfo.Gradient*` / `Masking` / `CircularProgress` | 大量 | `CircularProgress` 30 处，shader 在 `Loader.cs:132` 预编译 |

**无后处理 Bloom / Sharpen**（`OsuModBloom` 是放大光标的 gameplay mod，不是渲染后处理），`EffectPlacement` 未检索到使用 —— 光效是「逐 Drawable 加性层 + shader」堆出来的，不是全屏后处理管线。`TextureStore/LargeTextureStore` 见 `OsuGameBase.cs:292-294`、`DrawableRulesetDependencies.cs:64-65`、`Background.cs:44`。

## C9. 粒子与几何 VFX

- `Graphics/ParticleSpewer.cs`：固定容量环形缓冲 `perSecond*ceil(duration/1000)`（`:45,106-108`）、加法混合（`:43`）、`IsPresent` 依赖 Active（`:33,38`）、**位置/透明度/缩放/角度全是时间的纯函数**（`:230-244`），无逐帧物理 step；DrawNode 每帧 copy 数组并 `DrawQuad(...MultiplyAlpha)`（`:144,152-185`）；大跳帧限补发（`:82-87`）、忽略回放 seek 的「未来粒子」（`:161-163`）。
- `Graphics/ParticleExplosion.cs`：`Restart()` 重建 150 粒子 + `FadeOutFromOne`（`:45-53`），随机方向/距离/时长（`:128-133`），加法混合（`:32`），`Blit` 逐个 DrawQuad（`:90-116`）。
- 三角形背景性能上限统一取 `ushort.MaxValue/(VERTICES_PER_QUAD+2)`（`Triangles.cs:185`、`TrianglesV2.cs:139`），每帧 `Invalidate(Invalidation.DrawNode)`（`Triangles.cs:172`、`TrianglesV2.cs:118`），V1 用 `AsSpan()` 原地重置（`:191-203`）。
- 使用点：`Screens/Menu/StarFountain.cs:37`、`osu.Game.Rulesets.Osu/Skinning/Legacy/LegacyCursorParticles.cs:139`、`Skinning/LegacyJudgementPieceNew.cs:44`、`osu.Game.Tests/Visual/.../TestSceneParticleSpewer|Explosion.cs`。

## C10. VFX 设计范式提炼（最值得学）

1. **绝对时间轴而非逐帧状态机**：判定特效用 `BeginAbsoluteSequence(Result.TimeAbsolute)`，使回放 seek、帧稳定时钟、皮肤组件 lifetime 天然对齐。
2. **节奏作为动画时钟**：`BeatSyncedContainer` + `OnNewBeat` + kiai 标志，把音频振幅映射为缩放/闪光/粒子速度。
3. **成本极低的粒子**：时间纯函数 + 环形缓冲 + 单 DrawNode 批量 DrawQuad，无物理积分、无每粒子 Drawable。
4. **加法混合 + 受控发光**：119+ 处 Additive 与 `EdgeEffect`/`FlashColour` 组合出「光」，而非全屏后处理；按需 `BufferedContainer` + `FrameBufferScale` 降采样模糊。
5. **CPU 侧几何 + GPU 侧 UBO**：TrianglesV2、GhostIcon、LogoAnimation 把状态压成小 UBO，几何由 CPU 批量生成，避免逐物件 draw call。
6. **预编译换首帧稳定**：启动期把 12 个自定义 shader 全部编译完才进主界面。
7. **VFX 全皮肤化**：`SkinnableDrawable.cs:76-100` 的 lookup → 默认回落 → 换肤重建，让每个视觉特效都可被用户资产替换。

**复用价值排序**：① `TrianglesV2.cs`（shader+UBO+批量 quad+精确抗锯齿，最完整范式）② `ModFlashlight.cs`（自定义顶点 + UBO + 片元遮罩全链路）③ `LogoAnimation.cs`（自定义 VS/FS + vertexAction 改写批次）④ `ArgonHealthDisplayBar.cs`（UBO 传渐变/发光参数）⑤ `GhostIcon.cs`（无贴图程序化动画）⑥ `BeatmapMetadataWedge.FailRetryDisplay.cs`（FastCircle 圆角单批次）⑦ `ParticleSpewer.cs`/`ParticleExplosion.cs` ⑧ `LogoVisualisation.cs`（加法混合频谱）⑨ `Triangles.cs`（旧版参考）⑩ `BarGraph.cs`/`SegmentedGraph.cs`（纯 DrawQuad）。

## C11. 待办 / 注意

- 若要做自定义 VFX：**不要**找仓库里的 GLSL 或改 `osu!.res`；参照 `DrawableRulesetDependencies.cs:71,201-216` 的 `NamespacedResourceStore(resources, "Shaders")` 模式放本地 shader，并记得加进 `Loader.ShaderPrecompiler` 预编译清单，否则首帧会掉帧。
- `osu!.res` 的确切来源与是否可清理待查（无代码引用，疑为历史存档）。
- 动效侧最大改进空间：抽全局时长/缓动 token + reduced-motion 开关 + 转场「等动画结束」栅栏（详见报告 E 的 P0/P1 清单）。
