# Coopanion：Agent / 提示词管理 / 模板配置 代码级分析

分析日期：2026-11-15。参考目录：`P:\Dev\OI\refs\Coopanion\`。服务于 Persona_Manager 立项调研。
说明：`vendor/cortico` 是 git 子模块，当前工作区为空（`.gitmodules:1-3` 指向 `Pal-AI-Lab/Cortico`），故 Cortico WebUI 后端实现与 Cormini 本体源码无法在本目录内查阅；以下后端路由结论来自前端调用形状、装配代码与契约测试。

## 0. 最重要的概念澄清：Coopanion 没有 "agent" 一等概念

一等概念链：`Cortico（内核/运行时）→ Cormini（Cortico 自带 Persona 类）→ CoopanionPersona（本仓库子类）→ Bot（createBot 的唯一运行时实例）→ Workspace（CONSTITUTION.md 承载身份）`。

"agent" 字面只出现在无关语境：HTTP 代理 `undici.Agent`（`core/system-proxy.ts:15,68-89`）、UI 配色/标记名、Persona 可见性字段 `agentVisible`（指"该页对 Coo 可见"，`console/advanced/console-pages/host.ts:579,612`）、OAuth 协议字段 `multi_agent/dynamic_agent_client`（`packages/cortico-provider-coo/src/chatgpt/auth.ts:5-8`）。**无 agent 创建/列出/切换/删除模块，无多 agent / profile 切换 API，无 agent 命名的类/文件/配置字段。**

- 单一部署 `companion`（`core/seed.ts:12,46-48`）、单一 bot `cormini`（`:51` 写 `deployment.json = { bot: 'cormini' }`）、单一 workspace `<data>/home/companion/workspace`。
- 6 种人设是"预设"而非"多实例"：同一时刻只有一个 `CONSTITUTION.md` 生效，切换=覆盖该文件（`core/seed.ts:149-158`）。
- 多 World / 扩展是能力模块，不是身份或 agent（`core/companion.ts:559-570`）。

## 1. 身份（Persona）的装配与落盘

### 1.1 种子与首次启动（`core/seed.ts`，全文已读）
- `PERSONAS = ['coo','grail','forge','heart','winter','whale']`（`:28`）× 11 语言 = 66 个 `.md`，位于 `core/seed/persona/<id>.<lang>.md`（`personaFile()` `:32`）。
- `seed(home, language)`（`:45-78`）：建 workspace/providers 目录 → 写 `deployment.json` `{bot:'cormini'}`（`:51`）→ 写 `config.json`（displayName/language/activeProvider/web.port/worlds，`:58-65`）→ 复制 `coo.<lang>.md` 为 workspace 的 `CONSTITUTION.md`（`:68`）→ `followLanguage` + 若干升级迁移（`:71-77`）。
- 识别与切换：
  - `personaOf(workspace)`（`:149-153`）：对 `CONSTITUTION.md` 取 SHA-256（换行归一 LF，`hashOf` `:139-140`），与随包 66 个种子哈希 + `seed-history.ts` 历史哈希比对；返回 `PersonaId | null`，`null` 表示被改过（custom）。
  - `applyPersona(workspace,id,lang)`（`:156-158`）：copy 覆盖 `CONSTITUTION.md`。
  - `followLanguage(workspace,lang)`（`:164-167`）：仅当当前文件与随包人设逐字一致时才换成新语言版本；用户/Coo 改过的内容保留。

### 1.2 运行时 Persona（`core/persona.ts`，全文已读）
`coopanionPersona(Base: typeof Cormini)`（`:159`）返回 `class CoopanionPersona extends Base`，在 Cormini 之上做五件事：
1. `PARTNER.md` 整段进系统前缀且不可被 `delete_file` 删；`PARTNER_NOTE`（`:35-37`）+ `partnerDoc()` 覆写（`:177-181`）要求"短画像，细节放 `partner/`"。
2. `MEMORY_NOTE`（`:43-57`）+ `memoryNote()`覆写（`:183-185`）：规定 workspace 维护法（一文件一主题、发现重复即合并、承诺与未完成进 `todo.md`、带日期、旧的进 `archive/`）。
3. 每次写工具调用后提交 Git：`WRITES`（`:68-75`）映射工具名→成功回执，`tools()`（`:223-225`）包裹写工具，`committed()`（`:228-244`）用串行 Promise 队列 `this.commits` 保证一写一提交，作者 `AUTHOR_COO={name:'coo'}`（`:59`）。
4. 交付批次首行 `deliveryLine(events)`（`:149-156`）给出事件数与时间跨度。
5. Persona 自己注入的系统行（session 开场、World 生命周期、交付、context 压力、handoff、轮次上限、卡住）一律英文（`onOpening` `:246`、`onWorldLifecycle` `:250`、`onDelivery` `:269`、`pressureNote` `:280`、`handoffNoteLines` `:291`、`softCapHint` `:304`）；语言由桌面宠物 World 告知。
- `snapshot(version)`（`:194-204`）启动时：忽略并 untrack `blobs/`（`IGNORED` `:63`、`untrack` `:82-86`）→ 首次打 tag `start-<version>`（`SNAPSHOT_TAG` `:65`、`tagStart` `:207-221`）→ `arrangePartner()`（`:95-134`）规整 `PARTNER.md`/`partner/` 大小写与合并。
- `prefixFolds()`（`:308-314`）把 `handoffs/`、`blobs/`、`archive/` 折叠成一行摘要。

### 1.3 运行时装配（`core/companion.ts`，子代理证据）
- `cormini()`（`:416-428`）从 staged Cortico 动态加载 `bots/cormini/index.ts`、`persona/persona.ts`、`persona/config.ts`，`:426` 用 `coopanionPersona(persona.Cormini)` 作为 Persona。
- BotDefinition（`:510-558`）：`build()` `:536-546` 实例化 Persona 并 `snapshot(APP_VERSION)`；`:551-554` 暴露 `webUi.configGroups`；`:572` `loadDeployment(...)`；`:589` `createBot(...)`。
- `botName()`（`:455`）取 `config.displayName`（默认 `Coo`），仅显示名。
- 配置组 `companionGroup()`（`:130-149`，`owner:'persona'`）。

## 2. 人设选择入口（`core/guide.ts:449-470`）
`personaOf(workspace)` → 仅当仍是随包人设才显示选择卡（选项来自 `PERSONAS`）→ `applyPersona(...)` → `POST /api/session/reload-prefix`；并 `pet.dress(PERSONA_LOOKS[picked].skin)`（`:466`）。验证见 `tests/guide-language.test.ts:89-96`。手改后 `personaOf()` 返回 null，统计为 `custom`（`core/companion.ts:375-376`），后续升级不覆盖。

## 3. 提示词管理前端 `console/advanced/features/prompts/`（子代理全量走查）

核心设计：**把渲染后的完整前缀拆成多个 block，放进单个 CodeMirror 文档**；有 `sourceKey` 的 block 可编辑并对应模板文档 `PromptDoc`，无 `sourceKey` 的是只读生成段。

### 3.1 文件职责
| 文件 | 职责（关键行号） |
|---|---|
| `index.ts` | 页面主体 `mountPrompts` `:60`；双路加载 `:148-178`；保存 `:179-213`；立即应用/重启 `:224-242`；编辑回调 `:256-284`；几何布局 `:286-334`；段落/变量 UI `:360-458`；重排与 block 下发 `:464-498`；manifest 入口 `:504-521`；feature 注册 `:538-540` |
| `editor.ts` | CodeMirror 分块核心：`PrefixBlock` `:8-16`；占位符正则 `\{\{\s*([\w.]+)\s*(?:\|([^}]*))?\}\}` `:61-65`；`VariableWidget` `:71-120`；折叠头 `:123-149`；`decorate` `:151-183`；编辑守卫 `sourceGuard` `:208-216`；`createPrefixEditor` `:218-389` |
| `view.ts` | 通用模板卡片库（非 CodeMirror）：`PromptVar`/`PromptDoc` `:14-33`；保存 `:163-190`、恢复默认 `:192-222`、加载 `:242-264` |
| `template-view.ts` | World 归属页模板编辑器，复用 `createPrefixEditor` 但 `headers:false` `:187-214` |
| `variables.ts` | 变量气泡/chip `:7-37` |
| `titles.ts` | 跨页统一模板显示名 `constitution`/`orientation` `:7-13` |
| `strings.ts` + 10 个 `strings.<lang>.ts` | 人读文案 11 语，`pick()` `:182-184` |
| `prompts.css` | 分块配色 `.cm-block-persona/world/derived` `:38-42` 等 |

入口：`console/index.ts:32` 导入，`:73` 挂主导航；`promptsFeature` 的 `route:'prompts'`、`basic:true`（`index.ts:538`）。

### 3.2 关键数据结构（已亲读）
```ts
// editor.ts:8-16
interface PrefixBlock { title; description; sourceKey?; tone: 'persona'|'world'|'derived'; text; vars?; collapsed? }
// index.ts:22
interface PrefixSegment { title; text; sourceKey? }
// view.ts:14-33
interface PromptVar { name; description; multiline?; value? }   // value 缺席=占位符原样保留
interface PromptDoc { key; title; scope?; description?; content; revision?; role?:'envPrompt'|'prefix'; origin?:EnvPromptOrigin; vars? }
```

### 3.3 CodeMirror 分块机制（`editor.ts`）
- 单文档 + 范围表：`setBlocks()` 把 block 文本用 `\n` 拼成一份文档，记录 `{block,from,to,collapsed,preview,minHeight,padding}`（`:333-365`）；身份用 `source:<key>`/`title:<title>` 保住撤销位置与折叠态。
- `blockField` 为 `BlockRange[]`，文档变化时 `tr.changes.mapPos` 平移范围（`:41-56`）；`decorate()` 给折叠块插 `Decoration.replace` 的 `SectionHeader`，变量引用替换为 `VariableWidget`（`:151-183`）。
- 编辑边界 `sourceGuard`（`:208-216`）：只允许改"未折叠且有 sourceKey"的块，防生成段/分隔符被删。
- 变更回流 `updateListener`（`:239-255`）：对每个有 key 的 range 取 `doc.sliceString(from,to)`，只把变化块 `onChange`；主页面维护 `dirty` Map（`:258-268`）。
- 预览不落盘：`setPreview`/`updateVariables` 只换 widget 显示值；fallback 语法 `{{ fixture.name | Friend }}`，优先 `value` 再 fallback（`valueOf` `:67-69`）。
- `setBlocks` 用 `Transaction.addToHistory.of(false)`（`:361-364`），生成段刷新不污染撤销历史。

### 3.4 加载/保存/应用数据流（已亲读 `index.ts:148-242`）
- 加载 `refreshPreview()`：`Promise.all([get('/api/prompts/prefix'), get('/api/prompts')])`；`segments` 拼完整预览；有未保存改动的模板保留自身 `content`+`revision`（`:157-162`）；`layout()` 把"有模板的段 + 前缀未列出但有 role 的模板"映射为 section（`:464-493`）。
- 保存 `persist()`：对每个 `dirty` 串行 `post('/api/prompts', { key, content, baseRevision: doc.revision })`，成功后更新 `doc.content/revision`；保存期间又编辑则继续留 `dirty` 并置 `pendingApply`；最后 `refreshPreview()`（`:179-199`）。
- 立即应用：`POST /api/session/reload-prefix`（保留会话）；重启：`POST /api/storage/clear?owner=core&key=session`（`:224-242`）。
- 快捷键 Ctrl/⌘+S 保存、Ctrl/⌘+Z/Y 撤销（`:245-253`）；折叠态存 `sessionStorage['coopanion.prompt-folds']`（`:45,:276`）。

## 4. 模板配置（Template）

### 4.1 两个模板来源
- **Persona 模板**：`constitution`、`orientation` 两个 key 由 `titles.ts:7-13` 统一标题/说明；真正内容来自 `core/seed/persona/*.md`（不是 TS 常量）。本仓库**没有** `templates.ts` 常量；模板清单由 Cortico `/api/prompts` 返回。
- **World 环境提示词模板**：由 World 用 `promptDocs` 声明。桌面宠物声明在 `packages/cortico-world-desktop-pet/src/world.ts:2205-2212`：
  - key `worlds.desktop-pet.envPrompt`；`title/description` 来自 `c.envPrompt`；默认模板文件 `path: this.envPromptFile()`，即 `ENV_PROMPT_FILES`（`:79-83`，zh→`ENV_PROMPT.md`、en→`ENV_PROMPT.en.md`，缺则回退中文）。
  - 10 个可编辑变量：`pet.user/vocab/moves/voice/body/dress/self/chat/reply/cua`，其中 `vocab/moves/dress/cua` 标 `multiline:true`；说明文案来自 `c.vars`（`i18n/en.ts:231-243`）。
  - CUA 不是独立模板 key：渲染为 `{{pet.cua}}` 注入宠物模板（`world.ts:2141-2147`）。
- `origin`（`EnvPromptOrigin`）：`deployment | package | module`——仅 `deployment` 显示"恢复默认版本"，保存后强制标为 `deployment`（`view.ts:153-155,175-179`）。恢复默认 `POST /api/prompts/reset {key}`。

### 4.2 页面分发（`console/advanced/console-pages/host.ts:496-547`）
World 归属页用 `template-view.ts`（CodeMirror 无段头），其他归属页用 `view.ts`（卡片 textarea）。配置组按 ConfigGroup schema 渲染（`console/advanced/features/config/view.ts:12-17`），`POST /api/config`；契约 `{group:'cormini', values:{'context.maxTokens':…}}`（`tests/console-api.test.ts:224`）。

## 5. API 路由契约（`console/base/api.ts` + `tests/console-api.test.ts:270-286`）
| 路由 | 请求 | 响应 |
|---|---|---|
| `GET /api/prompts` | — | `{ prompts: PromptDoc[] }` |
| `GET /api/prompts/prefix` | — | `{ segments: [{title,text,sourceKey?}] }` |
| `POST /api/prompts` | `{key,content,baseRevision}` | `{result, revision}`；陈旧 revision → **409 + {error}** |
| `POST /api/prompts/reset` | `{key}` | `{result}` |
| `POST /api/session/reload-prefix` | `{}` | `{result}` |
| `POST /api/storage/clear?owner=core&key=session` | `{}` | `{result}` |
| `GET /api/webui/manifest` | — | 变量配置入口，按 `providers[].prompts[].key` 匹配 |

客户端 `console/base/api.ts`：`get` `:89-91`、`post` `:93-99`（`JSON.stringify(body ?? {})`）、`send` `:32-46`（注入语言头/AbortSignal/401 reload）、`readJson`/`throwHttpError` `:56-78`（非 2xx 或非法 JSON 抛 `ConsoleInvokeError`）。

## 6. 人读文案 vs 模型读文本（AGENTS.md:56-62）
- 模型读：`packages/cortico-world-desktop-pet/src/model-text.ts`，导出 `MODEL_TEXT: Record<ModelLanguage, ModelText>`（`:323-325`），World 按 `modelLanguage` 取（`world.ts:61,566-568`）；例 `typed()` `:48-49`、`figureNow` `:69`。
- 人读 11 语：`console/advanced/features/prompts/strings.ts`（`title:'系统提示词'` `:49`、`variablesTitle` `:36`、`loadedFromDeployment` `:75`、`warnUnknownVars` `:67`），`pick()` `:182-184`。
- 环境提示词/事件/release notes 中英双语（`ENV_PROMPT*.md`）；Persona seed 因人也读而 11 语。
- 世界页变量说明回到人读 `i18n/en.ts:231-243`；模型读值由 `envPromptVars()`（`world.ts:2137-2150`）用 `this.t` 填充。

## 7. 测试如何锁定行为
- `tests/console-prompts-page.test.js`：jsdom 挂真实 `mountPrompts`，fetch stub 精确匹配路由（`:37-66`）；锁定 payload/revision 409（`:48-56`）、并发与串行（`:93-140`）、变量预览（`:142-159`）、外部改写（`:161-170`）、部分失败只重试失败源（`:287-302`）、卸载中止、折叠态持久化、离线重试。
- `tests/console-prompt-editor.test.js`：折叠不改文本/CRLF 归一（`:21-39`）、编辑不越界（`:41-59`）、刷新不进撤销栈（`:61-76`）、引用撤销（`:78-91`）、fallback（`:93-110`）、多段 undo 定位（`:112-129`）。
- `tests/console-api.test.ts:270-286`：启真实 Cortico bot 锁路由形状与 409 语义（子模块升级契约测试）。
- `packages/cortico-world-desktop-pet/tests/world.test.ts:89-94`：锁英文环境提示词渲染无汉字、占位符齐全。

## 8. 对 Persona_Manager 的借鉴要点
- 身份用"文件即真相 + 哈希识别是否官方预设"，改过即视为 custom 且升级不覆盖——可借鉴到人格模板的版本管理。
- 前缀 = 多个来源段顺序拼接；"可编辑模板"与"运行时生成段"要分离（sourceKey 守卫），避免用户误改系统段。
- 单 CodeMirror 文档 + 范围表 + 折叠 widget 是编辑"多段长模板"的成熟方案。
- 保存用 `baseRevision` 乐观并发（409 冲突），逐块串行 + 保存中继续接受编辑，体验细节值得复用。
- 明确区分"模型读文本"与"人读 UI 文案"两套 i18n 边界。
- 后端路由不在本仓库时，用集成测试锁定 HTTP 契约是可行的解耦方式。
