# 调研报告 D：场景级 VFX / 光效落地全景（2026-11-13，重点）

> 与报告 C（shader/DrawNode 底层）互补，本篇聚焦「哪里用了、什么效果、怎么编排」。路径相对 `p:/Dev/osu`。

---

## D1. 场景 → 触发 → 视觉表现 → 实现

| 场景 | 触发 | 视觉表现 | 实现路径 |
|---|---|---|---|
| **启动 Intro** | 进入/恢复 | 全黑背景渐亮、logo 居中缩放后旋转 20° 淡出、音轨 2.6s 淡入 / 3s 退出 | `osu.Game/Screens/Menu/IntroScreen.cs:95-98,288-300,303-319,321-357`（`FadeColour`/`VolumeTo`/`LogoArriving`） |
| **Triangles 时间轴** | 按绝对拍点 | 「welcome to osu!」逐字、GlitchingTriangles 抖动、规则集图标缩放散开、lazerLogo 描边生长，收尾全屏加性白闪 | `IntroTriangles.cs:195-260`（`BeginAbsoluteSequence`+`BeginDelayedSequence`）、`:264-280`（`GameWideFlash`：白 Box + Additive + `FadeOutFromOne(1000)`）、`:352-379`；圣诞版 `Seasonal/IntroChristmas.cs:159-244` 按 `getTimeForBeat(-16..-2)` 逐拍递进 |
| **Logo 节拍/频谱** | 每拍 / kiai | 心跳缩放、ripple 扩散淡出、kiai 时 `flashLayer` 与频谱提亮、三角 `Velocity` 随声道振幅增加；空闲阻尼回落；点击白闪 1.5s、hover 弹性放大 | `OsuLogo.cs:291-347,364-380,400-426`；`LogoVisualisation.cs:99-145,194-245` 50ms 采样 256 频段 → 200 根径向加性 bar、5 圈叠加、逐帧衰减、kiai ×1 / 非 ×0.5 |
| **常驻三角背景** | 每帧 | 上浮三角形粒子，速度/密度/明暗可调 | `Graphics/Backgrounds/Triangles.cs:25-31,113-118`、`TrianglesV2.cs:102-149` |
| **菜单编排** | 每拍 / 左右声道 | logo 代理到 `logoTarget`、按钮扇形展开、侧边加性渐变按左右声道分拍闪（kiai 每 2 拍交替） | `MainMenu.cs:181-190,229-234,278-296,341-377`、`MenuSideFlashes.cs:101-142`、`KiaiMenuFountains.cs`、`ButtonSystem.cs:63-74,134-137`（LogoTrackingContainer） |
| **背景层** | 切屏 / 切谱面 | 8 图轮换 800ms 交叉淡入、季节/皮肤优先；切谱面时旧图 250ms 淡出、新图 500ms `FadeInFromZero`，模糊/暗化由 Bindable 驱动 | `Screens/Backgrounds/BackgroundScreenDefault.cs:128-196`、`BackgroundScreenBeatmap.cs:104-118,168-203` |
| **选歌** | 滚动 / 选中 / 过滤 | double 精度 `DrawYPosition` 整体偏移、过滤后 expire 旧面板、`DrawablePool` 复用；选中加性 selectionLayer + `FadeEdgeEffectTo` + X 位移；`PulsatingBox`（BeatSyncedContainer）按层级每 N 拍脉冲；Wedge 三段横向渐变（含 Additive）；难度数字滚动 + `ForStarDifficulty` 背景/字色渐变；在线状态色驱动边框/glow/三角；背景斜向暗化 + `FadeInFromZero`；四层 Wave 错峰移入 | `Graphics/Carousel/Carousel.cs:167-173,406-418`、`Carousel.ScrollContainer.cs:64-101`、`CarouselItem.cs:14-67`、`Screens/Select/Panel.cs:135-164,170-205,331-385`、`WedgeBackground.cs:29-50`、`Beatmaps/Drawables/StarRatingDisplay.cs:157-172`、`PanelGroupRankedStatus.cs:153-191`、`PanelSetBackground.cs:94-125,163-173`、`BeatmapTitleWedge.cs:209-216`、`FilterControl.cs:368-375`、`Graphics/Containers/WaveContainer.cs:125-138,184-191` |
| **Mod 面板** | preselected / 类型 | Hollow Glow 描边，按 `ForModType` 取 Accent | `Overlays/Mods/ModPanel.cs:51-76`、`ShearedOverlayContainer.cs:134-160` |
| **判定统一管线** | 每次 hit/miss | 从池取 `DrawableJudgement`，`Apply()` 绑结果与屏幕坐标，`PrepareForUse()` 用 `BeginAbsoluteSequence(Result.TimeAbsolute)` 清 transform 后跑 `ApplyHitAnimations/ApplyMissAnimations`，以皮肤组件 `LatestTransformEndTime` 定 LifetimeEnd；判定体为 `SkinnableDrawable(SkinComponentLookup<HitResult>)`，换肤自动重播；默认 miss 放大 1.6→1、下坠 100px、旋转 40° | `osu.Game/Rulesets/Judgements/DrawableJudgement.cs:88-150,166-191`、`DefaultJudgementPiece.cs:39-54` |
| **osu! 玩法** | 命中 / spinner | 命中加性 `SkinnableLighting` 0.8→1.2 放大、FadeIn 200 / Delay 200 / FadeOut 1000（受 HitLighting 开关）；Argon 按等级喷 3~8 个加性圆环随机方向飞散；FollowPoint Glow+Additive、Argon 粉红渐变双 chevron；Spinner bonus `FlashColour` + 放大淡出 | `osu.Game.Rulesets.Osu/Objects/Drawables/DrawableOsuJudgement.cs:29-89`、`Skinning/Argon/ArgonJudgementPiece.cs:101-168`、`Objects/Drawables/Connections/FollowPoint.cs:30-48`、`Skinning/Argon/ArgonFollowPoint.cs:17-36`、`Skinning/Default/DefaultSpinner.cs:101-110`、`Skinning/Argon/ArgonSpinner.cs:95-104` |
| **视觉 mod（osu!）** | combo / break / 按键 | Flashlight 用 uniform + DrawNode 画暗场圆孔，combo≥100/200 收缩、break 放大；Bloom 光标随 combo 放大至 10×；Bubbles 池化气泡按 combo 膨胀后 pop；Depth 透视 z 缩放；Traceable 只留描边；Hidden 按 TimePreempt 分段 FadeOut；Blinds 随血量开合黑幕 | `osu.Game/Rulesets/Mods/ModFlashlight.cs:56,148-199,256-309`、`Osu/Mods/OsuModBloom.cs:68-85`、`OsuModBubbles.cs:94-115,186-207`、`OsuModDepth.cs:99-160`、`OsuModTraceable.cs:38-78`、`OsuModHidden.cs:70-183`、`OsuModBlinds.cs:160-206` |
| **Taiko** | 命中 / swell / kiai | 池化 `HitExplosion` 加性圆盘 ScaleTo 3 / FadeOut 500，rim/centre 分色，strong 二次 ResizeTo；Swell 按敲击数扩张光环 + 完成 1.4× 淡出；Kiai 皮肤叠层闪 | `UI/HitExplosion.cs:74-97`、`Skinning/Default/DefaultHitExplosion.cs:68-79`、`DefaultSwell.cs:131-175`、`Skinning/LegacyKiaiFlashingDrawable.cs:46-55` |
| **Catch** | 接果 / hyper dash | `HitExplosionEntry` 生成 4 层 EdgeEffect Glow + 随机旋转方向光晕；Hyper dash 整体 `FadeColour` 变红，拖尾/残影三容器按皮肤色分色 | `UI/Catcher.cs:224,285-335,402-403`、`Skinning/Default/DefaultHitExplosion.cs:83-123`、`UI/CatcherTrailDisplay.cs:51-108` |
| **Mania** | 按键 / 命中 / hold | keyIcon 弹性放大 + Glow；命中 4 个加性 Glow 圆环横向拉伸淡出；Hold 按住加性层按 160ms 节拍 Loop 闪 | `UI/Components/DefaultKeyArea.cs:83-124`、`UI/DefaultHitExplosion.cs:54-166`、`Skinning/Argon/ArgonHoldNoteHittingLayer.cs:24-55` |
| **HUD** | combo / 血量 / 误差 | combo 增长弹 1.1×、断连闪红 2s；血条主条 + glow/受伤残留双层 Damp + Flash；低血量左右加性红渐变；HitError 彩点池化入列；判定计数滚动 | `Screens/Play/HUD/ArgonComboCounter.cs:43-58`、`ArgonHealthDisplay.cs:157-209`、`FailingLayer.cs:50-108`、`HitErrorMeters/ColourHitErrorMeter.cs:62-67,181-199`、`JudgementCounter/JudgementCounter.cs:32-77` |
| **结算 / 评级** | 游玩结束 | 评级圆环整圈 ScaleTo 0→1、accuracy 3s `Easing.OutPow10` 填充、分段等级环 800ms 揭示、RankBadge 延时出现；V2 把 D 环拆两半保证 Z 序压盖；Rank 用 rankSprite + additive 叠层、评级色 glow、通过/失败红闪；按等级播 applause；展开时背景 Gray 淡变 | `Screens/Ranking/Expanded/Accuracy/AccuracyCircle.cs:216-300`、`GradedCircles.cs:66-87`、`RankingV2/Argon/GradedCirclesV2.cs:42-55,58-85`、`ArgonScoreRankDisplay.cs:238-296,344-346`、`ArgonTotalScorePanel.cs:259`、`Ranking/ResultsScreen.cs:259-309,455-487` |
| **Fail / 暂停 / 截图** | 失败 / 退出 | 音轨降频/低通、红闪、画面缩至 0.85 旋转 1° 变灰、存活物件随机旋转下坠 400px；`SkipOverlay` FlashColour；截图隐藏光标 3 帧后取帧并发通知 | `Screens/Play/FailAnimationContainer.cs:111-153,221-229`、`Player.cs:1125-1161`、`PauseOverlay.cs:13`、`SkipOverlay.cs:430`、`Graphics/ScreenshotManager.cs:182-205,257-268` |
| **光标 / 全局叠层** | hover / press / 通知 | `GlobalCursorDisplay` 每帧从 `HoveredDrawables` 找 `IProvideCursor` 覆盖者切 Hide/Show；`MenuCursorContainer` 按下缩 0.9、加性层 800ms 渐显、拖动按方向旋转、松手 OutElastic 回弹；玩法光标另有 ripple / Legacy 粒子 / Argon 拖尾；Notification Wave 位移 + FadeEdgeEffectTo；Dialog dimLayer 0.5 淡入；Toolbar 渐变 2.5s 淡入 / 200ms 淡出 + 上下滑入 | `Graphics/Cursor/GlobalCursorDisplay.cs:60-92`、`MenuCursorContainer.cs:156-184,203-234,250-289`、`OsuGame.cs:1070,1756,1839`、`Osu/UI/Cursor/CursorRippleVisualiser.cs:41-55`、`Osu/Skinning/Legacy/LegacyCursorParticles.cs:139`、`Osu/Skinning/Argon/ArgonCursorTrail.cs`、`Overlays/NotificationOverlay.cs:190,232-247`、`DialogOverlay.cs:160-172`、`Toolbar/Toolbar.cs:226-246,289-298`、`ToolbarNotificationButton.cs:75`、`Overlays/MedalAnimation.cs:198` |

## D2. 皮肤对 VFX 的介入（所有特效都可被替换）

`Skinning/SkinnableDrawable.cs:76-100` 以 lookup 向 `ISkinSource` 索取，缺失回落 `CreateDefault()`；`SkinnableSprite.cs:69-90` 取贴图/帧动画；`SkinnableSound.cs:122-173` 把 SampleInfo 转池化 `PoolableSkinnableSample`；`SkinTransformer.cs:29-37` 允许逐方法改写 lookup 链。

Legacy 映射：`osu.Game.Rulesets.Osu/Skinning/Legacy/OsuLegacySkinTransformer.cs:44,168-282` 把 `hitcircle`、`followpoint`、`sliderfollowcircle/b`、`approachcircle`、`spinner-top/background`、`cursor-ripple`、`star2` 映射为 Drawable，`hit0/hit50/hit100/hit300` 由 `Skinning/LegacyJudgementPieceNew.cs`/`Old.cs` 承担；`Skinning/LegacySkin.cs:176-304,383-440` 管 Mania `LightingN/LightingL/ExplosionImage` 与旧式 HUD。内置皮肤：`Skinning/ArgonSkin.cs:93-312`（ArgonComboCounter、ArgonHealthDisplay、Results 组件）、`TrianglesSkin.cs:59-202`（Default* HUD）。SkinEditor 可改 `Skinning/GlobalSkinnableContainers.cs:11-24` 四区（HUD / SongSelect / Playfield / Results）内 `ISerialisableDrawable` 的位置、尺寸、锚点与 `[SettingSource]` 参数（如 WireframeOpacity、BarHeight）→ `Overlays/SkinEditor/SkinEditor.cs:379-447`。

## D3. 最值得借鉴的 10 个 VFX 实现

| # | 路径:行号 | 理由 |
|---|---|---|
| 1 | `osu.Game/Rulesets/Judgements/DrawableJudgement.cs:113-150` | 绝对时间序列 + 池化 + 皮肤组件 transform 自动定 lifetime，判定特效最干净的骨架 |
| 2 | `osu.Game/Screens/Menu/LogoVisualisation.cs:99-145,194-245` | 音频频域 → 径向几何的 DrawNode 自绘，加性叠加与衰减分离 |
| 3 | `osu.Game/Screens/Menu/OsuLogo.cs:291-347` | 一个 `OnNewBeat` 同时编排缩放、波纹、闪光、粒子速度，节奏驱动范例 |
| 4 | `osu.Game/Screens/Menu/IntroTriangles.cs:195-280` | 多层 `BeginDelayedSequence` 时间轴 + 收尾全屏加性白闪，开场编排模板 |
| 5 | `osu.Game/Screens/Menu/MenuSideFlashes.cs:101-142` | 按左右声道振幅 / kiai 奇偶拍驱动双侧加性渐变，成本低氛围强 |
| 6 | `osu.Game/Screens/Select/Panel.cs:170-205` | `BeatSyncedContainer` + DepthLayer 决定闪光间隔，让列表整体「随音乐呼吸」 |
| 7 | `osu.Game.Rulesets.Osu/Skinning/Argon/ArgonJudgementPiece.cs:101-168` | 判定等级映射粒子数量/距离，随机方向 + 加性圆环飞散 |
| 8 | `osu.Game/Screens/Play/FailAnimationContainer.cs:111-153,221-229` | 视觉、音频滤波、物件逐个坠落同步的一次性转场容器 |
| 9 | `osu.Game/Screens/Ranking/Expanded/Accuracy/AccuracyCircle.cs:216-300` | 圆环填充、分段等级环、徽章与音效频率同驱的长时序结算动画 |
| 10 | `osu.Game/Skinning/SkinnableDrawable.cs:76-100` | lookup + 默认回落 + 换肤重建，让所有 VFX 天然可被皮肤替换 |

## D4. 关键路径清单（前缀 `p:/Dev/osu/`）

- 开场/菜单：`osu.Game/Screens/Menu/{IntroScreen.cs:95,288,303,321, IntroTriangles.cs:195,264,352, OsuLogo.cs:291,350,400, LogoVisualisation.cs:99,194, MainMenu.cs:181,278,341, ButtonSystem.cs:63,134, MenuSideFlashes.cs:101}`、`osu.Game/Seasonal/IntroChristmas.cs:159,223`、`osu.Game/Graphics/Backgrounds/{Triangles.cs:25,113, TrianglesV2.cs:102,141}`。
- 背景层：`osu.Game/Screens/Backgrounds/{BackgroundScreenDefault.cs:128,137, BackgroundScreenBeatmap.cs:104,168}`。
- 选歌：`osu.Game/Graphics/Carousel/{Carousel.cs:167,406, Carousel.ScrollContainer.cs:64,94, CarouselItem.cs:14}`、`osu.Game/Screens/Select/{Panel.cs:135,170,331, PanelSetBackground.cs:94,163, PanelGroupRankedStatus.cs:153, WedgeBackground.cs:29, BeatmapTitleWedge.cs:209, FilterControl.cs:368}`、`osu.Game/Beatmaps/Drawables/StarRatingDisplay.cs:157`、`osu.Game/Overlays/Mods/ModPanel.cs:62`。
- 游玩：`osu.Game/Rulesets/{Judgements/DrawableJudgement.cs:88,113,154, Mods/ModFlashlight.cs:56,148,256}`、`osu.Game.Rulesets.Osu/{Objects/Drawables/DrawableOsuJudgement.cs:29,73, Skinning/Argon/ArgonJudgementPiece.cs:101,147, Objects/Drawables/Connections/FollowPoint.cs:30, Mods/OsuModBubbles.cs:94,186, Mods/OsuModBloom.cs:68, Mods/OsuModDepth.cs:99, Mods/OsuModTraceable.cs:38, Mods/OsuModHidden.cs:70, Mods/OsuModBlinds.cs:160}`。
- Taiko/Catch/Mania：`osu.Game.Rulesets.Taiko/{UI/HitExplosion.cs:74, Skinning/Default/DefaultHitExplosion.cs:68, DefaultSwell.cs:131}`、`osu.Game.Rulesets.Catch/{UI/Catcher.cs:224,285,402, UI/CatcherTrailDisplay.cs:51, Skinning/Default/DefaultHitExplosion.cs:83}`、`osu.Game.Rulesets.Mania/{UI/DefaultHitExplosion.cs:54,147, UI/Components/DefaultKeyArea.cs:83,113, Skinning/Argon/ArgonHoldNoteHittingLayer.cs:24}`。
- HUD：`osu.Game/Screens/Play/HUD/{ArgonComboCounter.cs:43, ArgonHealthDisplay.cs:157,188, FailingLayer.cs:50,101, HitErrorMeters/ColourHitErrorMeter.cs:62,181, JudgementCounter/JudgementCounter.cs:32}`。
- 结算/转场：`osu.Game/Screens/{Ranking/Expanded/Accuracy/AccuracyCircle.cs:216, Ranking/GradedCircles.cs:66, RankingV2/Argon/GradedCirclesV2.cs:42,58, RankingV2/Argon/ArgonScoreRankDisplay.cs:238,344, Ranking/ResultsScreen.cs:259,455, Play/FailAnimationContainer.cs:111,221, Play/Player.cs:1125}`、`osu.Game/Graphics/ScreenshotManager.cs:182,257`。
- 光标/叠层：`osu.Game/Graphics/Cursor/{GlobalCursorDisplay.cs:60, MenuCursorContainer.cs:156,203,250}`、`osu.Game/Overlays/{NotificationOverlay.cs:190,232, DialogOverlay.cs:160, Toolbar/Toolbar.cs:226,289, ToolbarNotificationButton.cs:75, Dialog/PopupDialog.cs:294}`。
- 皮肤：`osu.Game/Skinning/{SkinnableDrawable.cs:76, SkinnableSprite.cs:69, SkinnableSound.cs:122, SkinTransformer.cs:29, LegacyKiaiFlashingDrawable.cs:46, LegacySkin.cs:176,383, ArgonSkin.cs:93,111,283, TrianglesSkin.cs:59, GlobalSkinnableContainers.cs:11}`、`osu.Game/Overlays/SkinEditor/SkinEditor.cs:379,441`、`osu.Game.Rulesets.Osu/Skinning/Legacy/OsuLegacySkinTransformer.cs:44,168,212,276`。

## D5. 阅读顺序建议（VFX 优先）

`Screens/Loader.cs:65-158` → `Graphics/Backgrounds/TrianglesV2.cs` → `Graphics/Sprites/LogoAnimation.cs` → `Rulesets/Mods/ModFlashlight.cs` → `Rulesets/Judgements/DrawableJudgement.cs` → `Screens/Menu/LogoVisualisation.cs` + `Graphics/ParticleSpewer.cs` → `Screens/Menu/IntroTriangles.cs` + `OsuLogo.cs` → `Skinning/SkinnableDrawable.cs` + `ArgonSkin.cs` → 场景表 D1 按需展开。
