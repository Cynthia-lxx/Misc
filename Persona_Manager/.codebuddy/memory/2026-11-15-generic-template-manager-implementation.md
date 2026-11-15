# Persona_Manager：通用 AI 内容模板管理（Coopanion 架构泛化实现）

记录日期：2026-11-15。目标：把 Coopanion 的提示词/模板管理架构脱离 Agent 与桌宠概念，泛化为任何基于 LLM API 的工具均可使用的"AI 内容设置/管理"模板，后端 Python、前端原生 HTML/CSS/JS，形成可运行可调用的代码。

## 1. 概念映射（Coopanion → 泛化实现）

| Coopanion 原概念 | 泛化实现 |
|---|---|
| Persona / `CONSTITUTION.md` / 6 人设种子 | 可编辑 `role` 模板 + manifest 内 3 套角色预设（general/concise/teacher），预设应用=写入覆盖文件，SHA-256 识别 active preset |
| `PARTNER.md`/`MEMORY_NOTE`/World/CUA/桌宠 | 全部删除，改为通用 `behavior`（行为准则）与 `output`（输出约定）段 |
| `/api/prompts/prefix` segments（sourceKey 有无区分可编辑/生成段） | 保留：`editable`/`generated` 字段区分，`runtime` 段为只读生成段 |
| `PromptDoc {key,title,role,origin,content,revision,vars}` | 保留同名契约，`origin` 简化为 `builtin|user`（外加 `generated`） |
| revision + `baseRevision` 乐观并发 409 | 保留：revision = SHA-256(content LF 归一)，无需额外簿记 |
| `POST /api/prompts/reset` | `POST /api/templates/reset` |
| `POST /api/session/reload-prefix` | `POST /api/reload`（无缓存，仅为契约兼容保留） |
| 人读 11 语 / 模型读双 i18n | 模板内容=模型读（英文为主含中文标题双写）；UI 文案 zh/en 在前端 `I18N` 字典 |
| CodeMirror 单文档分块编辑 | 原生 `<textarea>` 分块卡片 + 折叠 + dirty 追踪，零依赖 |

## 2. 目录结构与运行

```
Persona_Manager/
├── backend/
│   ├── config.py      # 路径与常量；HOST/PORT 可用 PERSONA_MANAGER_HOST/PORT 环境变量覆盖（默认 127.0.0.1:18765）
│   ├── render.py      # {{ name | fallback }} 变量解析/渲染/references()/builtin_values()（date 自动注入）
│   ├── store.py       # 存储层：manifest 基线 + data/overrides/<key>.md 覆盖；模板 CRUD、变量值、预设、prefix 组装
│   ├── server.py      # stdlib ThreadingHTTPServer；/api/* JSON + 静态托管 frontend/；CORS 全开
│   └── defaults/
│       └── manifest.json  # 内置模板(role/behavior/output/runtime) + variables + presets 的唯一真源
├── frontend/
│   ├── index.html     # 布局：顶栏(预设/刷新/保存) + 左模板段列表 + 右变量面板与预览
│   ├── styles.css     # 深色主题 CSS 变量、role 色标、badge、toast、响应式
│   └── app.js         # 零依赖客户端：加载/渲染/dirty/串行保存(409 处理)/变量即时预览/Ctrl+S
└── data/              # 运行时生成（首次运行自动创建）：overrides/ + config.json
```

运行：`python backend/server.py` → 打开 `http://127.0.0.1:18765/`。零第三方依赖（Python 3.10+，实测 3.12.9）。

## 3. API 契约（已全部实测通过）

| 路由 | 方法 | 说明 |
|---|---|---|
| `/api/health` | GET | `{ok:true, version}` |
| `/api/templates` | GET | `{templates:[PromptDoc]}`；PromptDoc 含 `key/title/role/origin/generated/editable/content/revision/vars` |
| `/api/templates/prefix` | GET | `{prompt, segments, variables}`：渲染后各段 + 拼接完整提示词 |
| `/api/templates` | POST | `{key, content, baseRevision}` → `{result, template}`；陈旧 revision → **409** |
| `/api/templates/reset` | POST | `{key}` → 删覆盖恢复内置 |
| `/api/vars` | GET/POST | GET 列出；POST `{values:{name:value}}` 持久化到 `data/config.json` |
| `/api/presets` | GET | `{presets:[{id,title}], active}`；active 由 SHA-256 比对 role 内容得出 |
| `/api/role/apply` | POST | `{id}` → 写入 role 覆盖 |
| `/api/prompt` | GET | **LLM 工具的单一调用点**：直接拿渲染好的完整系统提示词 |
| `/api/reload` | POST | 兼容保留，无实际缓存 |

实测结果（2026-11-15）：GET 六端点 + 保存(origin→user)/陈旧 409/重置(origin→builtin)/变量持久化/预设应用(active teacher)/重置回 general 全部 PASS；静态页 `/`、`/styles.css`、`/app.js` 均 200。

## 4. 关键设计决策与易错点
- **文件即真相 + 哈希识别**：revision=SHA-256(LF 归一内容)，`active_preset()` 同法比对——借鉴 Coopanion `personaOf()`，升级不覆盖用户内容。
- **覆盖层**：内置 `defaults/manifest.json` 只读基线；用户编辑写 `data/overrides/<key>.md`；reset=删覆盖文件。
- **生成段**：`runtime` 模板 `generated:true`，前端 textarea 只读、`save_template` 直接 `ValueError` 拒绝。
- **变量渲染顺序**：调用方传入值 > manifest/config 值 > 内联 fallback > 原样保留 token（空值可见，不静默丢弃）。
- **date 变量永远自动**：`render.builtin_values()` 注入当天日期，config 值为空则回退 builtin。
- **踩坑**：本机 8765 端口被用户另一服务占用 → 默认端口改 18765 并支持 `PERSONA_MANAGER_PORT` 环境变量；静态服务有 frontend 目录白名单校验（防路径穿越）。
- **前端预览**：未保存编辑用前端本地正则替换即时预览，保存后以服务器 `/api/templates/prefix` 为准。

## 5. 后续扩展点
- manifest 增加模板只需加 JSON 条目（key/title/role/content/vars），前后端零改动。
- 若需多语言模板变体（`<key>.<lang>.md`）可在 `_tpl_def` 解析处扩展。
- 工具集成方式：HTTP `GET /api/prompt`（跨域已放开），或 Python `import store; store.build_prefix()`。
