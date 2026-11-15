# osu! (lazer) 快照 · 工作区记忆索引

本文件是 `p:/Dev/osu` 工作区项目记忆（LM）的主入口；遵守全局规则：项目记忆只写 LM，不写 GM。

## 指引

- **调研详录入口**：[INDEX.md](INDEX.md) —— 2026-11-13 两轮深度源码调研的总索引（全仓架构 / 排版配色 / 动效 VFX 链路 / 场景级特效 / 调用与优化与 UXD），五份专题报告均附路径:行号与定量数据：
  - `2026-11-13-osu-repo-architecture.md`（A · 全仓架构）
  - `2026-11-13-osu-ui-layout-colour.md`（B · 排版与配色）
  - `2026-11-13-osu-motion-vfx-pipeline.md`（C · 动效 + VFX 底层，重点）
  - `2026-11-13-osu-vfx-scenes.md`（D · 场景级 VFX，重点）
  - `2026-11-13-osu-callchain-perf-uxd.md`（E · 调用 / 优化 / UXD）
  - `2026-11-14-uireference-progress.md`（UIReference 风格层迁移进度）
- 环境基线：SDK 10.0.100 / C# 14 / ppy.osu.Framework 2026.1111.0；无独立 `.git`（父仓 `P:/Dev`）。
- 核心结论速记：仓库内无 GLSL 源码（在 NuGet framework/resources 包）；shader 预编译在 `Screens/Loader.cs:117-158`（15 次 Load）；VFX 骨架 = 池化 + 绝对时间序列 + 皮肤可替换（`DrawableJudgement.cs:88-150`）；全仓无 reduced-motion / 色盲 / 高对比支持。

## 红线

- `CONTRIBUTING.md:5-7`：AI/LLM 辅助贡献一律关闭锁定，勿代写贡献。
- `CONTRIBUTING.md:73`：禁止 Release 配置跑游戏/测试。
- `osu!.res` 是无代码引用的历史二进制存档，勿误当 shader 源。
