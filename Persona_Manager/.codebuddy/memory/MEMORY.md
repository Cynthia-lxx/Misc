# Persona_Manager 项目记忆指引（MEMORY.md）

本目录存放属于 Persona_Manager 项目自身的记忆（立项信息、架构、进度、踩坑、约定等）。全局记忆（GM）只放跨项目通用内容，项目级信息一律写此处，不得写入 GM。

## 立项信息（2026-11-15）
- **项目名称**：Persona_Manager
- **项目根目录**：`P:\Dev\Persona_Manager\`
- **参考目录**：`P:\Dev\OI\refs\Coopanion\`（Coopanion 的提示词/Persona 管理与设置页架构，详见下方索引）
- **记忆目录**：`P:\Dev\Persona_Manager\.codebuddy\memory\`（本文件所在处）
- **提交格式**：`[Persona] ……`
  - 本项目无独立 `.git`，git 操作在父仓 `P:\Dev`（main 分支）进行。
  - git add 必须显式限定本项目路径 `Persona_Manager/`，禁用 `git add -A` / `git add .` / `-am`。

## 详录索引

| 文件 | 内容 | 记录日期 |
|---|---|---|
| `2026-11-15-coopanion-agent-prompt-template-analysis.md` | Coopanion 的 Agent 概念澄清、Persona 身份装配与落盘、提示词管理前端、模板配置（Persona/World）、API 契约、人读/模型读文本边界、测试锁定与借鉴要点 | 2026-11-15 |
| `2026-11-15-generic-template-manager-implementation.md` | 通用 AI 内容模板管理的泛化实现（去 Agent/桌宠）：概念映射、backend(Python stdlib)+frontend(原生三件套)结构、API 契约、实测结果、设计决策与踩坑 | 2026-11-15 |

## 参考目录要点（来自 Coopanion 记忆，供后续开发查用）
Coopanion 的完整架构详录见 `P:\Dev\OI\refs\Coopanion\.codebuddy\memory\`。与本项目最相关的结论：
- 提示词由 Cortico 的 Persona（Cormini）机制生成；种子在 `core/seed/persona/`（66 个 `.md`，6 人设 × 11 语言），落盘到 workspace 的 `CONSTITUTION.md`。
- 真正发给模型的前缀在 `core/persona.ts`（`coopanionPersona()`：PARTNER.md / MEMORY_NOTE / workspace 清单）。
- 提示词编辑 UI 在 `console/advanced/features/prompts/`；后端 `/api/prompts*` 路由在 `vendor/cortico` 的 WebUI API，Coopanion 只消费不自建。
- Coo 自读的英文文本进 `model-text.ts`，人读文案进 `strings*.ts`（十一语言）。

## 阅读顺序提示
- 新会话接手本项目时，先读本指引，再按索引读对应详录。
- 涉及提示词/Persona 逻辑时，先查参考目录 Coopanion 的记忆与源码。
