# ComfyUI "App mode"（Beta）侦察档案（2026-10-30，下一阶段储备）

> 组成：官方文档（`ComfyUI_Docs_Work/interface/app-mode.mdx`，en/zh/ja/ko 四语）+
> 前端 chunk 逆向（subagent，comfyui_frontend_package 1.51.9）+ 后端/脱水现状核验。
> 前端 1.41.13 起官方支持，1.51.9 属早期测试态；**内部代号 = "linear mode"**。

## 0. 功能面（文档层）

把工作流包装成"App"：4 步 Builder（选 Inputs → 选 Outputs → Preview → 设默认视图
App/Node Graph），App 视图 = 右侧输入参数面板 + 输出展示区 + Run/Cancel + 移动端
三 tab（输入/输出/Assets）。入口：左上菜单 "Enter app mode" / 面包屑下拉 / 侧栏
Apps tab（`Workspace_ToggleSidebarTab_apps`）。保存后 `Ctrl+S`，Apps 面板列出已存
App 工作流。分享链接（Create a link）**仅 Comfy Cloud**。

## 1. 入口与命令（chunk 锚点）

- 命令表 `assets/commands-BKdhxncd.js`（全文可读）：
  `Comfy.ToggleLinear`（"Toggle App Mode"）、`Workspace_ToggleSidebarTab_apps`
  （tooltip "Apps"，侧栏 tab id = **`apps`**）；相邻 `Experimental_ToggleVueNodes`。
- 菜单注册 `GraphView-Cc2GkGnZ.js` 的 **`useWorkflowActionsMenu`**：
  `{id:'enter-app-mode', command: Comfy.ToggleLinear}` / `exit-app-mode` /
  `enter-builder-mode`（`ensureWorkflowActive(); enterBuilder()`）。
  visible 条件：`isRoot && (hasSeenLinear || flags.linearToggleEnabled) && currentMode!=='app'`。
- `WorkflowActionsDropdown`（`data-testid="view-mode-toggle"`）= graph/app 分段切换器。

## 2. 状态与持久化（核心契约）

- **全在 workflow JSON 的 extra**（zod schema 于 `assets/api-D_pCbeK9.js`）：
  ```
  extra.linearMode: boolean?          // 默认视图 = App
  extra.linearData: {
    inputs?:  [[nodeId, widgetName, {height?}]]   // id 可 string|number
    outputs?: [nodeId]                            // 输出节点 id 列表
  }
  ```
- 已保存 App 工作流 = userdata 文件名后缀 **`.app.json`**（菜单判定 + Apps 面板列表依据）。
- workflow 对象有 `initialMode`（打开时由 `getWorkflowMode()` 从 extra.linearMode /
  `.app.json` 后缀 / 云端 `default_view` 推出）与 `activeMode`（运行时 `setMode()` 可切）。
- 云端专属：workflow metadata `default_view: 'workflow'|'app'`（GET/PATCH /workflows）。
- localStorage：`builder-splitter` / `builder-splitter-right`（构建器 splitter）；
  settings：`Comfy.WorkflowActions.SeenItems`、`Comfy.AppBuilder.VueNodeSwitchDismissed`。
- 执行 store 每 job 记 `viewMode`/`isAppMode`（`storeJob()` ← `getWorkflowMode`）。

## 3. Builder（构建器）

- 状态机 **`useAppMode()`**（`settingStore-KkBYyEnh.js` line 209）：
  `mode: 'graph'|'app'|'builder:inputs'|'builder:outputs'|'builder:arrange'`
  （第三步 arrange 对外叫 **Preview**；第四步 default view 在保存对话框里：
  `"By default, this workflow will open as:"` app/nodeGraph + View app 按钮）。
- 输入候选：右侧参数面板中 `mode===ALWAYS` 的已提升 widget + `nodeTypeValidForApp(node.type)`
  且无错误的节点（locale 示例：Load image / Text prompt / Steps）。
- 输出：builder:outputs 步骤点输出节点（SaveImage/SaveVideo 类 `output_node`）。
- `pruneLinearData()` 清失效引用；校验 `connectOutput`（至少一个输出）。
- 构建器**强制切 Nodes 2.0（Vue nodes）**，有一次性提示（VueNodeSwitchDismissed）。

## 4. Run 流

- 就是普通队列：POST `/prompt`（api chunk schema `In`），取消 `Comfy.Interrupt`。
  widget 值经 Nodes 2.0 widget 系统直接写图；app 输入面板 = Nodes 2.0 widget 渲染。
- 进度：`QueueProgressOverlay`（total/current node %）；job 结果走 **`/jobs*` 端点**
  （`/jobs?status=`、`/jobs/{id}`、`/jobs/{id}/assets`）。
- ⚠ **后端核验（2026-10-30）**：`/jobs*` 是 **Comfy Cloud 专属**——原版 v0.34.0 与我们的
  脱水后端 `api_server/routes/`（仅 internal）均无 jobs 路由 → 本地 Run 可用，
  但 App 视图的 results/assets 面板会 404 降级；queue 视图走 /queue 正常。

## 5. 视图切换

- **非路由切换**：仍停 GraphView 路由，`useAppMode().mode` 驱动；canvas store 有
  `linearMode` computed；app 模式固定缩放 `appScalePercentage`/`setAppZoomFromPercentage`。
- 布局：`LiteGraphCanvasSplitterOverlay`（GraphView chunk）——builder 模式 splitter
  state-key 换 `builder-splitter(-right)`、class 加 `-builder`、右侧参数面板插槽
  `min-w-78` 显示输入面板。
- 默认视图兑现：打开工作流时 `initialMode` → 决定 graph/app。

## 6. Blueprints 关系

- Blueprints = **独立特性**（Subgraph Blueprints：`Comfy_PublishSubgraph`、nodeSource
  type `blueprint`、原版后端顶层 `blueprints/` 目录 89 个蓝图——**我们的脱水构建已删**）。
- App mode **不基于 blueprints**（locale 分区 linearMode/builder*/appBuilder 与相关代码
  零 blueprint 引用；仅 extra schema 同时容纳两者）。置信度高。

## 7. 本地可用性结论（对 ComfyDL 的意义）

- 纯本地可用面：Enter app mode、Builder、extra.linearData 契约、Run/Cancel、
  `.app.json` 保存、Apps 侧栏面板。不可用：分享链接、default_view 云字段、/jobs 结果面板。
- **可复制点**：`extra.linearData` 契约极简单——我们的模板目录（template_catalog）可以
  在模板 JSON 里预置 `extra.linearMode/linearData`，让 ComfyDL 教学模板"开箱即 App"，
  或以此为参照给 ComfyDL 做自己的 App 面板（CDL 输入面板 = 我们节点 widgets 的子集）。
- ⚠ 未完全确认（中置信度）：`nodeTypeValidForApp()` 的精确类型白名单与
  `getWorkflowMode()` 回退顺序在 settingStore line 208（155KB 单行，read_file 截断），
  如需精确契约需再探针。

## 8. 文档树索引（ComfyUI_Docs_Work，下层储备）

- `interface/`：app-mode、nodes-2（Nodes 2.0）、features/、settings/、shortcuts、user。
- `development/`：api-development、cloud、comfy-api、comfy-router、comfyui-server
  （comms_messages/comms_routes/execution_model_inversion_guide/startup-flags/api-examples）、
  deploy、run-workflows、serverless。
- `agent-tools/`（in-app-agent/mcp/cli）、`api-reference/v2/` 亦在库中。
