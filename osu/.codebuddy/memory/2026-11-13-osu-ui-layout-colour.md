# 调研报告 B：UI 排版 / 组件库 / 配色与视觉语言（2026-11-13）

> 静态分析；路径相对 `p:/Dev/osu`。UI 代码规模：`Graphics` 214 文件 26,390 行、`Screens` 749/123,086、`Overlays` 523/61,241、`Skinning` 86/9,915、`Localisation` 99/8,082。

---

## B1. 布局底层模型与缩放链

全部 UI 是 framework 的 **Drawable 树**：`Container`/`CompositeDrawable` 的 `Content` 重定向 + `Anchor/Origin` 九宫格 + `RelativeSizeAxes`/`AutoSizeAxes` + `FillFlowContainer`/`GridContainer`；`Padding`（父内缩）/`Margin`（子外距）/`Spacing`（流间距）职责严格分离；`FillMode.Fit|Fill` 只用于精灵/封面（`Users/Drawables/DrawableAvatar.cs:29`、`Graphics/Backgrounds/Background.cs:39`）；`GridContainer` 的 `Dimension(GridSizeMode.Relative, ..., maxSize)` 做限宽响应式。

缩放链（业务屏不重复适配）：

```
SafeAreaContainer
  → ScalingContainer(Everything)           // DrawSizePreservingFillContainer 固定逻辑 1024×768
    → GlobalActionContainer → GlobalCursorDisplay → OsuTooltipContainer
      → ScalingContainer(ExcludeOverlays)  // ScreenStack / Footer
      → ScalingContainer(Gameplay)         // HUD 层
```

`Graphics/Containers/ScalingContainer.cs:25,70-129,171-250`：内层 `DrawSizePreservingFillContainer` 固定逻辑坐标，`UIScale` 以 `Scale = s, Size = 1/s` 抵消实现等比缩放而不改布局；外层 `SizeableAlwaysInputContainer` 管相对定位、`corner_radius=10`、光标/平板限制；`:215-230` **仅在非单位尺寸或存在安全区时开 `Masking/CornerRadius`**（`:227` 注释自承全栈只应做一次）；`:262` `LayoutValue(Invalidation.RequiredParentSizeToFit)` 缓存 cursorRect。`ScalingMode` 四档 Off/Everything/ExcludeOverlays/Gameplay（`Configuration/ScalingMode.cs:9-22`）。SafeArea 由 `SafeAreaOverrideEdges` + `ISafeArea.SafeAreaPadding` 驱动；移动端强制 UIScale 默认值，横竖屏由 `SongSelect` 按 `DrawWidth/DrawHeight` 算 `widescreenBonusWidth` 重设三列 min/max。

## B2. `Graphics/Containers/` 47 文件归类

| 类别 | 代表 | 机制 |
|---|---|---|
| 缩放/变换 | `ScalingContainer`、`ParallaxContainer`、`UprightAspectMaintainingContainer`、`ConstrainedIconContainer`、`ShearAligningWrapper` | 视差 0.02 且同步放大 `1+|amount|` 防露边（`ParallaxContainer.cs:18-31,59`）；逆矩阵抵消父级旋转/剪切/缩放（`UprightAspectMaintainingContainer.cs:16,49`）；按父高 × SHEAR 补左 Padding（`ShearAligningWrapper.cs:18,47`） |
| 覆盖层动效 | `WaveContainer`（4 层旋转波浪 + Masking + 音效）、`OsuFocusedOverlayContainer`、`ExpandingContainer`、`HoldToConfirmContainer`、`ShakeContainer`、`UserDimContainer`、`BeatSyncedContainer` | `WaveContainer.cs:17,19,75,123`；`OsuFocusedOverlayContainer.cs:18,26,74,115` |
| 滚动/分区 | `OsuScrollContainer`（10px 条、r5、加法混合）、`UserTrackingScrollContainer`、`SectionsContainer<T>`、`ReverseChildIDFillFlowContainer`、`SelectionCycleFillFlowContainer`、`OsuRearrangeableListContainer/Item` | `SectionsContainer.cs:22,49,130,163,208,245,311` 的 ExpandableHeader/FixedHeader/Footer 与 0.1 处判定选中 |
| 文本 | `OsuTextFlowContainer`、`LinkFlowContainer`、`Markdown/*`（21 文件） | `LinkFlowContainer.cs:23,36,88,139,144` 把链接编译器从 `FlowingChildren` 剔除以免破坏 AutoSize |
| 其他 | `LogoTrackingContainer`（跨屏 logo 代理）、`OsuClickableContainer`/`OsuHoverContainer` | `LogoTrackingContainer.cs:18,37` |

## B3. 具体界面排版常量

| 界面 | 结构与常量 | 证据 |
|---|---|---|
| MainMenu | `ParallaxContainer → LogoTrackingContainer → ButtonArea`；`BUTTON_WIDTH=140 / WEDGE_WIDTH=20` 楔形按钮 + 底部 FillFlow | `Screens/Menu/ButtonSystem.cs:36,38,72,100`、`MainMenu.cs:106,111` |
| SongSelect | `mainContent(Bottom=Footer.HEIGHT) → ContextMenu → PopoverContainer → GridContainer 三列`；左列 `Shear=SHEAR(0.2)` + 负 Padding 隐藏圆角、`ShearAligningWrapper` 包 TitleWedge/DetailsArea；右列 `BeatmapCarousel`(BleedTop/Bottom) + FilterControl；`CORNER_RADIUS_HIDE_OFFSET=20` | `Screens/Select/SongSelect.cs:82,83,187,192,207,221,239,245,282,298,417` |
| Carousel 面板 | `ICarouselPanel.DrawYPosition`（double 精度）托管定位、`CarouselItem.DEFAULT_HEIGHT=45`、`Panel.CORNER_RADIUS=10`、PanelBeatmap(Set)/Group 池化 | `Graphics/Carousel/ICarouselPanel.cs:14,39`、`CarouselItem.cs:14,47` |
| Footer | `ScreenStackFooter` 绑定 `OsuScreen.FooterContent` → `ScreenFooter(HEIGHT=50)`；GridContainer 左 AutoSize/右填充；`SCREEN_EDGE_MARGIN(12)+padding(60)`、Spacing 7；右侧 `ShearedButton` | `Screens/Footer/ScreenFooter.cs:26,42,44,95,112,131,142`、`ScreenStackFooter.cs:15,44,97` |
| Settings | `SettingsPanel(PANEL_WIDTH=400, CONTENT_PADDING 12/22, CONTENT_MARGINS=20)` → `SettingsSectionsContainer : SectionsContainer<SettingsSection>`（搜索框作 FixedHeader、波浪阴影沿用 WaveContainer 常量）；`ITEM_SPACING=14 / V2=4` | `Overlays/SettingsPanel.cs:29,31,34,43,48,87,110,127,146,297,307` |
| ModSelect | `ShearedOverlayContainer(PADDING=14)`；上留 `ShearedOverlayHeader.HEIGHT(70+2×14)`、下留 `Footer.HEIGHT+14`；`ColumnScrollContainer → ColumnFlowContainer`（水平、Shear、Margin 70、列间 10）→ `ModColumn` 内部 `-SHEAR` 反向抵消、`panel.Shear=0`；`BUTTON_WIDTH=200` | `Overlays/Mods/ShearedOverlayContainer.cs:21,23,75,92,97,142`、`ModSelectOverlay.cs:39,41,143,162,172,186,341,703,747`、`ModColumn.cs:102,109,155` |
| ShearedButton | H50 / r7 / border2 | `Graphics/UserInterface/ShearedButton.cs:18,20,21,22,85,89` |
| HUD | `HUDOverlay` 全屏 + `HUDComponentsContainer : SkinnableContainer(MainHUDComponents)`（+ ruleset 版/Playfield 层）+ 三个固定 FillFlow（Margin/Spacing 10、AlwaysPresent），`ShowHud` 统一显隐 | `Screens/Play/HUDOverlay.cs:35,88,98,116,130,137,436,444` |
| Results | `ResultsScreen` GridContainer + VerticalScrollContainer + ScorePanelList/StatisticsPanel（`screen_height=768-TwoLayerButton.SIZE_EXTENDED.Y`）；`RankingV2` 改为单一 `SkinnableContainer(Results)` 承载 Argon/Extended/Legacy 三套面板 | `Screens/Ranking/ResultsScreen.cs:39,42,105,108,115`、`RankingV2/SoloResultsScreenV2.cs:16,34,36` |
| Editor | Top bar 40px、ScreenContainer `Padding Top=40/Bottom=50`、BottomBar、`EditorScreenSwitcherControl` 右下；Mode 切 Compose/Design/Timing/Setup/Verify，Compose 再分 Timeline 区 | `Screens/Edit/Editor.cs:69,355,364,370,374,465,474,1159,1711` |

**一致性约束**：5px 圆角 / 40px 控件高 / 50px footer / 7–14px 间距 / 10px HUD 边距；斜切必成对（父 `Shear`、子 `-SHEAR`/`Shear=0`/`UprightAspectMaintainingContainer`/`ShearAligningWrapper`）；常量集中在各文件 const；缩放集中在 DrawSizePreservingFillContainer。

## B4. 皮肤布局：官方 UI 也可被用户重排

`Skinning/GlobalSkinnableContainers.cs:11` 四个 target（MainHUDComponents / SongSelect / Playfield / Results）+ 可选 Ruleset 组成 `GlobalSkinnableContainerLookup.cs:13,18,24`；`Skin.cs:133-150,266-306` 遍历读 `{target}.json`，解析为 `SkinLayoutInfo.cs:37,42,48`（按 ruleset 分组的 `SerialisedDrawableInfo[]`），兼容旧纯数组、命名空间迁移与版本 migration。`SerialisedDrawableInfo.cs:28-51,62,100` 字段：`Type/Position/Rotation/Scale/Width/Height/Anchor/Origin/UsesFixedAnchor/Settings(snake_case Bindable)/Children`，AutoSize 轴不写尺寸；`SerialisableDrawableExtensions.cs:14,16,36,58` 依 `CanBeMoved/Rotated/Scaled` 权限回写。`SkinnableContainer.cs:22,34,46,51,56,70` 的 `Reload()` 从皮肤取组件（优先用户覆盖 lookup）并 `LoadComponentAsync` + `CanBePlaced` 过滤；`Overlays/SkinEditor/SkinEditor.cs:44,48,81,169,588,633,639` 用 BlueprintContainer / SelectionRotation / ScaleHandler / 组件工具箱 / 撤销栈可视化编辑，保存走 `CreateSerialisedInfo()` → `skins.Save` → `SkinImporter.cs:221,227` 写回 JSON。

## B5. 两代组件库

- **`Graphics/UserInterface/`（87）一代**：直接派生 framework 控件 —— `OsuButton.cs:22,62,76,78,85`（H40/r5/Hover/Flash/音效）、`OsuTextBox.cs:27,35,37,73`（H40/r5/Caret 3）、`OsuCheckbox.cs:18,42` + `Nub.cs:19,21,23`（15/50 圆形）、`OsuSliderBar.cs:16` → `RoundedSliderBar`/`ShearedSliderBar`/`RangeSlider`、`ProgressBar.cs:14`、`SearchTextBox.cs:11`、`OsuDropdown.cs:27`、`OsuTabControl`/`OsuMenu`、`TwoLayerButton`、`Bar.cs:13`/`BarGraph`/`LineGraph`/`SegmentedGraph`、各种 Counter、`LoadingSpinner`/`LoadingLayer`、`HoverSounds`、`DialogButton`、`PageSelector`、`Hotkey`、`FPSCounter`。
- **`Graphics/UserInterfaceV2/`（39）二代**：统一底座 `IFormControl.cs:13` + `FormControlBackground.cs:17,19,20,42,47,51,93`（r5、CornerExponent 2.5、Border 2.5、**Normal/Disabled/Hovered/Focused 四态** + `FlashOnCommit/FlashOnInputError`）；`FormButton/TextBox/NumberBox/PasswordTextBox/Dropdown/SliderBar/CheckBox(SwitchButton 56×16)/FileSelector/ColourPalette` + `LabelledDrawable.cs:16` 标签包装；`RoundedButton.cs:21,52` 仍继承 OsuButton 但换 TrianglesV2 渐变；另有 `OsuPopover.cs:20`、`OsuDirectorySelector`/`OsuFileSelector`、`OsuHSVColourPicker`/`OsuHexColourPicker`/`OsuSwatchColourPicker`、`ReportDialog`、`ShearedDropdown.cs:24`。差异：V1 各控件自绘背景音效；V2 抽出视觉状态机 + 表单语义 + `IHasFilterTerms` 过滤接口，配色统一由 `[Cached] OverlayColourProvider` 注入。
- **Sprites / Cursor**：`Graphics/Sprites/OsuSpriteText.cs:9,17`、`GlowingSpriteText`、`TruncatingSpriteText`/`SizePreservingSpriteText`、`LogoAnimation.cs:21,26,45`（自定义 shader）；`Graphics/Cursor/GlobalCursorDisplay.cs:19,43`（`IProvideCursor`）→ `MenuCursorContainer.cs:21,41` → `OsuTooltipContainer.cs:18,29,31` / `OsuContextMenuContainer`。

## B6. 色板四层（`Graphics/OsuColour.cs`）

| 层 | 内容 | 行号 |
|---|---|---|
| Legacy 五组 | Purple/Pink/Blue/Yellow/Green 各 Lighter→Darker（另有 Alternative）；注释指向 osu-web `colors.less` | `:308-339` |
| 六组灰色系 | GreySky / GreySeaFoam / GreyCyan / GreyLime / GreyViolet / GreyCarmine，各 5 档 + 一个高饱和原色，暗色 UI 底色 | `:341-381` |
| 中性灰阶 | `Gray0`(#000)…`GrayF`(#fff)，另有 `Gray(float/byte)` | `:20-21,383-398` |
| "Basic" 主题 | 8 组 × 5 档（Pink/Purple/Blue/Green/Lime/Orange/DarkOrange/Red）；`0` 最亮、`1` 品牌主色、`4` 最深；Figma `Asset/Colours?node-id=1838:3` | `:400-457` |

其余：`:305-306` 团队红蓝、`:459-475` `B5` 内容底色 / Red 判定色 / `ChatBlue` / `ContextMenuGray` / `DangerousButtonColour`。外部对齐链接：`:153` osu-web `layout.less#L36-L50`、`:260` Figma Tier-Colours、`:308` `colors.less`、`OverlayColourScheme.cs:25` osu-web `helpers.php#L1620-L1628`。

## B7. 主题 Provider：一个 Hue 派生整套层级

`Overlays/OverlayColourProvider.cs:20-70`（已亲自核验）：只存 `Hue`，`getColour(s,l) = Color4.FromHSL(Hue/360, s, l)`。

```
Colour0 = (1, 0.8) → Colour4 = (0.4, 0.3)
Highlight1 = (1, 0.7)
Light1~4 (s=0.4, l=0.8→0.5) / Dark1~6 (s=0.2, l=0.35→0.1) / Background1~6 (s=0.1, l=0.4→0.1)
Content1/2 (s=0.4, l=1/0.9) / Foreground1 (s=0.1, l=0.6)
```

`OverlayColourScheme.cs:8-19,23-58` 9 个方案映射 0°(Red)…333°(Pink)。`OsuColour.cs:404-407` 明确分工：**跟随视图主题用 provider，刻意偏离周围色相的固定语义色用 `OsuColour` 常量**。

**关键限制**：`OsuConfigManager.cs:133,224,408,439` 只有 `BeatmapColours`/`ComboColourNormalisationAmount`，**没有全局 scheme 设置**；各屏构造时硬编码（SongSelect Blue `SongSelect.cs:116`、Player Purple `Player.cs:128`、OnlinePlay Plum、UserProfile Pink、SettingsPanel Purple `SettingsPanel.cs:75`、Editor Aquamarine），仅 `ScreenFooter.cs:70,391` 退出 overlay 重置 Aquamarine、`UserProfileOverlay.cs:57,127,146-148` 用在线 `ProfileHue` 覆盖。`ChangeColourScheme()` `:61-68` 注释直言不广播、所有 drawable 需手动刷新。`SettingsColour.cs:64-76` 只是单色拾取控件。

## B8. 数据 → 颜色：查表 / 渐变 / 感知亮度

- **星级**：`:23-31` `STAR_DIFFICULTY_DEFINED_COLOUR_CUTOFF=6.5`（黑底可辨上限）、`STAR_DIFFICULTY_TEXT_GRADIENT_CUTOFF=9.0`；`:33-48` 13 段 `STAR_DIFFICULTY_SPECTRUM`（0.1 灰蓝 → 9/10 黑）、`:50-57` `STAR_DIFFICULTY_TEXT_SPECTRUM`；`ForStarDifficulty()` `:62` 按两位小数线性采样，文字 `<6.5` 黑 75% / `6.5~9` `Orange1` / `≥9` 黄→紫渐变（`:67-76`）。
- **离散状态查表**：`ForRank()` `:81-109`（XH/X `#de31ae`、S `#02b5c3`、A `#88da20`、B `#e3b130`、C `#ff8e5d`、D `#ff5a5a`、F `#3f3f3f`）；`ForHitResult()` `:114-145`（Miss 红 / Meh 黄 / Ok 绿 / Good 青绿 / Great 蓝）；`ForModType()` `:192-217`（Automation→Blue1、DifficultyIncrease→Red1、Reduction→Lime1、Conversion→Purple1、Fun→Pink1、System→Yellow）；`ForBeatmapSetOnlineStatus()` `:155-187`（Ranked `b3ff66`、Loved `ff66ab`、Qualified `66ccff`…，`null` 回退 provider）；`ForRoomCategory/Status` `:222-256`；`ForRankingTier()` `:262-291` 除 Iron 外全 `ColourInfo.GradientVertical`。
- **感知亮度**：`ForegroundTextColourFor()` `:297-303` 用 YIQ `0.299R+0.587G+0.114B`，`>0.5` → `Gray(0.2)` 否则 `Gray(0.9)`；`Graphics/HSPAColour.cs:9,12-14,44-49,95-200` 实现 HSPA↔RGB（`P=√(0.299R²+0.587G²+0.114B²)`），唯一业务调用是 `Rulesets/Objects/Drawables/DrawableHitObject.cs:580-586` 把 combo 色归一化到 `P=0.6`。
- **渐变工具**：`Utils/ColourUtils.cs:18-35` `SampleFromLinearGradient` 按 `(position, colour)` 停靠点 + `Interpolation.ValueAt`；`ColourInfo.Gradient*` 用于 `WedgeBackground.cs:35,49`、`Users/CoverBackground.cs:60`、`UserListPanel.cs:35`、`Screens/Select/FilterControl.DifficultyRangeSlider.cs:66,176,178`（把星数 spectrum 铺成 GradientHorizontal）。典型上色：`StarRatingDisplay.cs:169-172`、`Overlays/Mods/ModPanel.cs:53,66-72` / `ModColumn.cs:123` / `Rulesets/UI/ModIcon.cs:242,267`、`Profile/Header/Components/LevelBadge.cs:91`、`HUD/DefaultHealthDisplay.cs:46-66,122-125`、`ArgonHealthDisplay.cs:214`、`ArgonSongProgressBar.cs:72,89,98,147-152`。

## B9. 皮肤 / 规则集配色

`Skinning/SkinConfiguration.cs:13-22,31-44,46-72` 实现 `IHasComboColours/IHasCustomColours`（默认 4 色 combo、`CustomColours` 字符串键字典）；`Beatmaps/Formats/LegacyDecoder.cs:23,95-96,110-120,130-151,180` 解析 `[Colours]` 的 `Combo1..Combo8` 与任意键（MenuGlow、InputOverlayText、SliderBall、HyperDash…）；`Skinning/LegacySkin.cs:88-132,210,296,307,325-332,337-350,496` 用 `GlobalSkinColours.cs:6-10`/`SkinComboColourLookup`/`SkinCustomColourLookup` 分发；`LegacyColourCompatibility.cs:22-44` 复刻 stable 的零 alpha→1 与 alpha 翻倍。默认皮肤：`DefaultLegacySkin.cs:15-21,45-51`（classic 4 色 + 蓝 SliderBall）、`ArgonSkin.cs:53-73`（6 色，从 index 1 起保持绿蓝红黄）、`ArgonProSkin.cs:12-48`（继承不覆色）、`TrianglesSkin.cs:205-233`（fallback）、`RetroSkin.cs:63-71`（四色 + 蓝 SpinnerApproachCircle）；`Utils/BackgroundComboColourExtractor.cs:20-57` 用 Wu 量化从谱面背景提色并 clamp 亮度 50~220。规则集：Catch `Skinning/CatchSkinColour.cs:6-22`（HyperDash/Fruit/AfterImage）；Mania `LegacyManiaSkinConfiguration.cs:23,79-106` 列色键 + `UI/Column.cs:59` `Bindable<Color4>` 驱动 `Darken(5)` + 垂直渐变；osu `Skinning/OsuSkinColour.cs:6-13`；`Graphics/IHasAccentColour.cs:15-18,26-36` 与 `IHasDisplayColour.cs:12-18`（含 `FadeAccent()`）。

## B10. 字体 / 图标 / 文本

`Graphics/OsuFont.cs:16,21-57,62-79,90-101,108-141,156-162,165-206`：实际字体 **Torus / Torus-Alternate / Venera / Inter**（非 Torvus，快照内无 .ttf/.otf 二进制）；Torus 无 Medium 回退 Regular、斜体一律禁用。层级 Title 32 Alternate / Subtitle 28 / Heading1 22 Bold / Heading2 18 SemiBold / Body 16 / Caption1 14 / Caption2 12；`FontWeight` 300~900；数字用 Venera Bold。`OsuSpriteText.cs:11-21` 默认 `Shadow=true` + `OsuFont.Default`（**无文本描边**）；`TruncatingSpriteText.cs:13-27` 截断 + tooltip 全文；`GlowingSpriteText.cs:71` 关阴影改辉光；`DialogButton.cs:186-187` 用 `Black.Opacity(0.1f)` 阴影。图标双轨：`Graphics/OsuIcon.cs:19-25,769-770` 的 `FONT_NAME="Icons"` 纹理族 + 大量 framework `FontAwesome`（`StarRatingDisplay.cs:130`）。`Localisation/` 99 个 `*Strings.cs` 按功能及 Catch/Mania/Taiko/HUD 分组，`Language.cs` 含简/繁中。

## B11. 视觉语言总结与一致性风险

**公式**：暗色底（Gray 阶 / 六组灰系 / Background1~6）+ 语义色（枚举查表）+ 主题 provider（单 Hue 经 HSL 派生整套层级）+ 皮肤覆盖（skin.ini 的 Combo/CustomColours 覆盖 gameplay 层）；连续量用渐变采样，可读性靠 YIQ/HSPA 感知亮度保证。

**风险**：① 同色在 `OsuColour` 常量与 provider 双份定义（仅注释约定），HSL 派生值与 hex 不完全相等，易漂移；② 无全局 scheme 设置、各 screen 硬编码 hue、`ChangeColourScheme()` 不广播，跨视图拼接易色相断裂；③ Legacy / 灰系 / Basic 三套色板并存且命名相近（Green/GreenLight/Lime/GreyLime）误用概率高；④ 星级 ≥9 压黑需额外文字色与 Darken 补偿，深色皮肤叠加可读性依赖手工调参；⑤ `CustomColours` 是字符串键字典无编译期校验；⑥ FontAwesome 与自研 Icons 混用且字体资源不在快照内，一致性靠调用点自觉。

**可借鉴**：① 薄包装容器（`Content`/`InternalChild` 重定向）封装特定排版问题；② 斜切必成对；③ 逻辑分辨率固定 + 单一缩放点，业务屏零适配；④ 皮肤把布局外置 JSON 但用 `ISerialisableDrawable` 权限白名单守住视觉一致性；⑤ 「视图主题用 provider / 语义色用常量」的注释式契约；⑥ 数据→颜色映射集中在一个类，连续量用渐变采样、对比度用感知亮度反推。
