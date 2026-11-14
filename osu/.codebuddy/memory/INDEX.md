# osu! (lazer) 源码调研索引（2026-11-13）

对 `p:/Dev/osu`（= `P:/Dev/osu`）的**全仓架构**与 **UI / VFX 专题**两轮深度调研。工作区无独立 `.git`（`git rev-parse --show-toplevel` → `P:/Dev`，`osu/` 是父仓下的未跟踪源码快照），故不能追溯 osu! 自身提交历史。

- 调研方式：两轮共 12 个只读探索子代理分主题深挖 + 主代理亲自核验关键文件（`OsuColour.cs`、`OverlayColourProvider.cs`、`Loader.cs`、`TrianglesV2.cs`、`LogoAnimation.cs`、`ModFlashlight.cs`、`PositionAndColourVertex.cs`、`osu!.res` 文件头）+ PowerShell 定量统计。结论均附相对仓库根的路径与行号。
- 分析性质：**静态源码分析**，未 `dotnet build`、未 `dotnet test`、未运行游戏、未截图；视觉效果判定留给实际界面。
- 版本判定：`global.json:3` SDK 10.0.100；`Directory.Build.props:4` C# 14；`osu.Game.csproj:41-42` `ppy.osu.Framework 2026.1111.0` / `ppy.osu.Game.Resources 2026.1112.1`；`Ruleset.cs:58` `CURRENT_RULESET_API_VERSION = "2026.818.0"` → **2026-11 前后的较新 lazer 快照**。

## 报告清单

| 文件 | 主题 | 覆盖 |
|---|---|---|
| `2026-11-13-osu-repo-architecture.md` | A · 全仓架构 | 工程拓扑/依赖、framework 边界、启动与 DI、屏幕与 overlay、谱面→判定→计分主链、四规则集、Realm/在线/皮肤、Tournament/平台/Benchmarks/Templates、四条端到端流程、测试与 CI、架构评估 |
| `2026-11-13-osu-ui-layout-colour.md` | B · 排版与配色 | 47 个容器、缩放链、界面常量、两代组件库、皮肤布局 JSON、`OsuColour` 四层色板、`OverlayColourProvider`、语义映射/渐变工具、字体图标、视觉语言公式与风险 |
| `2026-11-13-osu-motion-vfx-pipeline.md` | C · 动效 + VFX 底层（**重点**） | Transform/Easing 定量与时长约定、`ShaderPrecompiler` 15 次 Load、shader 资源链路、UBO/自定义 DrawNode、逐个 shader 实现、加法混合/辉光/模糊、粒子系统、VFX 范式 |
| `2026-11-13-osu-vfx-scenes.md` | D · 场景级 VFX（**重点**） | Intro/Menu/背景/选歌/判定管线/四规则集/视觉 mod/HUD/结算与 Fail/光标与叠层/皮肤介入，Top10 可借鉴实现与阅读顺序 |
| `2026-11-13-osu-callchain-perf-uxd.md` | E · 调用逻辑 / 优化 / UXD | DI 与 bindable lease、overlay 状态机、输入与皮肤查找链、异步加载/池化/批量绘制/帧稳定/GC、UXD 反馈闭环与用户旅程、无障碍缺口、P0/P1/P2 改进清单 |

## 定量速查

| 指标 | 值 |
|---|---|
| 全仓文件（排除 obj/bin/.git） | 5,656；`.cs` 4,928 / **602,717 行**；`.csproj` 35 |
| UI 代码 | `Graphics` 214/26,390 行、`Screens` 749/123,086、`Overlays` 523/61,241、`Skinning` 86/9,915、`Localisation` 99/8,082 → **1,671 文件 / 约 228,700 行（38%）** |
| API 密度 | Transform 2,622；`Easing.*` 2,408（OutQuint 约 8 成）；混合/遮罩/缓冲 1,257；Glow 726；时间序列 503；池化/异步/失效 506；shader/自定义渲染 138 |
| 测试 | 约 6,075 处 `[Test]/[TestCase]/[TestFixture]`；`.osu` 163、`.osk` 35、`.json` 119；TODO/FIXME/HACK 注释 149 |

## 五条核心结论

1. **没有独立动画引擎，仓库内也没有 shader 源码**：动效 = framework `TransformSequence`；GLSL 源码在 NuGet `ppy.osu.Framework` / `ppy.osu.Game.Resources`（`osu.Game.csproj:41-42`）。本仓库的 VFX 精华是 **C# 侧 shader 绑定、UBO 结构、自定义 DrawNode 与几何生成**（`.frag/.vert/.glsl` 全仓 0 个）。
2. **启动期强制 shader 预编译**：`Screens/Loader.cs:117-158` 的 `ShaderPrecompiler` 共 15 次 `manager.Load(...)`（3 组内置 + 12 具名自定义），framework 每帧只编译一枚、全部 `IsLoaded` 才放行主界面（`Loader.cs:89-106`）。
3. **VFX 骨架 = 池化 + 绝对时间序列 + 皮肤可替换**：`Rulesets/Judgements/DrawableJudgement.cs:88-150` 用 `BeginAbsoluteSequence(Result.TimeAbsolute)` 对齐回放/帧稳定时钟，皮肤组件 `LatestTransformEndTime` 自动定 lifetime。
4. **节奏是一等公民**：`BeatSyncedContainer`、`OsuLogo.OnNewBeat`、`MenuSideFlashes` 左右声道、`LogoVisualisation` 256 频段→200 加性频谱柱，音频数据直接变视觉脉冲。
5. **两个明显缺口**：`ReduceMotion`/`prefers-reduced-motion`/色盲/高对比 **全仓 0 命中**；配色方案无全局切换，`OverlayColourProvider.cs:61-68` 的 `ChangeColourScheme()` 明确不广播。

## 风险红线 / 约定（勿踩）

- **AI 贡献政策**：`CONTRIBUTING.md:5-7` 与 `README.md:139` —— 任何由 AI/LLM 创建、撰写或辅助的 issue/PR 通常**直接关闭并锁定**；主观视觉改动须先开 discussion（`CONTRIBUTING.md:64`，指向 Figma Master Library）。此约束也作用于设计提案。
- **禁止 Release 配置跑游戏/测试**（`CONTRIBUTING.md:73`，唯一例外 benchmark）：Debug 才指向隔离的 `dev.ppy.sh` 全栈环境，Release 可能损坏本地 Realm 库。
- `osu.Game/osu!.res`（156,596 字节，自定义二进制，头 `00 00 00 00 20 00 00 00 FF FF...`）**全仓 0 代码引用**，`osu.Game.csproj` 无 EmbeddedResource、`osu.Game/Resources` 目录不存在 → 判为历史资源存档，**不要误当作 shader 源**。
- shader/字体/纹理源码在外部 NuGet 包；要做自定义 VFX 应参照 `Rulesets/UI/DrawableRulesetDependencies.cs:71` 的 `NamespacedResourceStore(resources, "Shaders")` 本地覆盖模式。
- 编译期硬约束：`CodeAnalysis/BannedSymbols.txt:1-27`（禁 `object.Equals`、`Task.Wait/Result`、framework `SpriteText`、Realm 原生订阅等），`osu.globalconfig:108` 把 RS0030 设为 error，`.editorconfig:33-94` 命名规则 error 级。

## 待办 / 未覆盖

- 未编译、未测试、未运行界面，故"能否在当前环境构建通过"与视觉美观判定均留白。
- 无独立 `.git` → 无提交历史/演进分析；`osu!.res` 的确切来源与是否可清理待查。
- 若续查：① `TrianglesV2` + `ModFlashlight` 的 UBO/DrawNode 逐行拆解；② Intro 绝对时间轴整理成可复用动效编排表；③ 从这套 VFX 提炼可移植到自家项目的实现要点；④ 补 UI/DrawNode 绘制专项基准（`osu.Game.Benchmarks/` 现有 13 个用例全是 CPU 算法）。

（状态：两轮调研完成并落盘，2026-11-13；主报告 5 份 + 本索引。）

> 增补：`UIReference` 抽离库/预览站的实施进度见 `2026-11-14-uireference-progress.md`（2026-11-14 起会话记录，含命名 token 与 dist 路径两个坑、回归基线）。
