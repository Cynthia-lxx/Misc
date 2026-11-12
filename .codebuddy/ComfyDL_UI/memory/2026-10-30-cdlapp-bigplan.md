# CDL App Mode 改造大计划（正式版 v1，2026-10-30 立项）

> 状态：**调研已固化，待用户审阅后逐 Phase 动工**。
> 愿景（用户原话归纳）：官方 Beta App mode 是为"观赏性图片输出"设计的；DL 的输出是
> 张量/模型/文字/文件。改造后 App 界面三要素 = **输入、输出、进度**（前两者平权）：
> 输出以"可直接拖出/下载的文件 + 可复制/导出的数据"为主，preview 只做高质量低算力；
> 进度要有详细进度条，失败要"死的明白"；要有 App→节点图入口；Inputs/Outputs 选择器
> 要在 App 界面有入口（不能只在 builder 里）。
> 前置调研：`2026-10-30-appmode-recon.md` + 本文件 §9（App 视图逆向 v2，subagent）。

## 0. 官方 App 视图现状（逆向 v2 要点，全部逐字锚点）

- App 视图 UI **内联**在 GraphView-Cc2GkGnZ.js（L3/L21 不可整读区）+ settingStore-KkBYyEnh.js，
  无独立 chunk；官方中心区 = litegraph canvas（app 模式下 view-mode-toggle 被卸载），
  输出展示走 **assets 侧栏 tab**（`openSidebarTab:'assets'`）+ `/jobs*`（**云端专属，本地 404**）。
- **官方扩展面**（可用）：`registerSidebarTab` / `registerBottomPanelTab` / dialog API /
  commands（`Comfy.ToggleLinear`、`Comfy.Interrupt`、`Comfy.QueuePrompt`…）；Actionbar
  **硬编码无扩展槽**；builder 入口 = builder store 的 `enterBuilder()`（settingStore 208:95489，
  步骤枚举 inputs/outputs/arrange，无直达命令）。
- **稳定锚点（注入用）**：coachmark 指令写 `data-coach-id`——app 模式下
  `[data-coach-id="app-run-button"|"inputs-list"|"outputs"|"assets-panel"]`；
  反向信号 = `[data-testid="view-mode-toggle"]` 被卸载。z-index 安全带 **2001–9999**。
- **事件（全部本地可用）**：`progress_state`（zod：value/max/state/node_id/prompt_id/
  display_node_id…，官方进度 store 已有 formatted percent）、`executed`（节点输出 payload）、
  `execution_error`（node_id/node_type/exception_message/exception_type/traceback[]/
  current_inputs/outputs）、`b_preview_with_metadata`（type4，带 node_id 的 JPEG 预览）。
- 输入面板 = 官方 `properties-panel`（Vue widgets 真渲染，`computeProcessedWidgets`）；
  输入候选条件 = widget mode===ALWAYS（提升）+ 非 Note + 无错误。
- 服务端：`/view` 只管图像类；我们的文件节点写 `output/` 相对路径（CdlWriteCSV/
  CdlModelSave/SaveText）→ 下载需自有路由。

## 1. ADR（待用户确认）

- **ADR-1 布局 = "中央不动 + 自有侧浮层"**：官方 app 视图中心与右侧输入面板（真 widgets）
  原样保留；我们新增 **CDL App Panel** 浮层（左侧 dock：输出 / 进度 / 错误三段），以
  coach-id/testid 锚点检测 app 模式，z-index 2001–9999。不重写官方中心区（Vue 组件不可注入，
  patch chunk 成本/风险过高，且 beta 期官方随时改）。
- **ADR-2 输入 = 官方右面板原样**（零维护、真 widget 语义）；我们的面板不重复渲染输入，
  只在需要时提供"编辑 Inputs"按钮（跳 builder:inputs 步骤）。
- **ADR-3 输出主形态 = 文件与数据**：①新增服务端路由 `GET /comfydl/artifacts`
  （列 output/ 目录产物：文件名/大小/mtime，path jail）+ `GET /comfydl/artifacts/download`
  （流式下载，Content-Disposition attachment；仅 output/ 白名单根）；②输出条目来源 =
  `executed` 事件中输出节点的文件路径 STRING 输出（SaveText/CdlWriteCSV/CdlModelSave/
  未来节点约定）+ 文本输出（可复制）。tensor 类输出在节点侧补 **摘要 STRING 端口**
  （shape/dtype/mean/std）——不传原始张量。
- **ADR-4 preview = 复用既有低算力渲染件**：CdlPlot / CdlHeatmaps（matplotlib PNG）+
  `b_preview_with_metadata` JPEG 通道（训练节点已有实时 loss 预览）——不发明新渲染器。
- **ADR-5 进度 = progress_state 驱动的 per-node 列表**：每节点一行（标题 + pct + state
  四态），复用现有 pbar 管线（训练节点已接入）；总进度条置顶。**错误 = execution_error
  驱动的错误卡**：节点标题/类型/消息 + traceback 折叠 + 复制按钮（"死的明白"）。
- **ADR-6 导航与选择器入口**：Panel 顶栏三按钮——「节点图」（execute Comfy.ToggleLinear）、
  「编辑 Inputs」/「编辑 Outputs」（进 builder 对应步骤；经 builder store，CRAFT 时落
  `app.extensionManager`/store 访问路径，探针确认调用形态）。
- **ADR-7 独立开关**：`ComfyDL.AppPanel.Enabled`（默认 on，category ["ComfyDL","App"]），
  仿 motion/settings 模式；关闭时零 DOM 注入。

## 2. 组件与文件

- `app/cdlapp_assets/cdlapp.js`（新，静态 serve `/comfydl/cdlapp/`，仿 motion/profiler boot：
  waitFor comfyAPI + **settingsStoreReady gate（2026-10-30 铁律）** + registerExtension；
  MutationObserver 监听 app 模式锚点挂/卸浮层；reduced-motion 不涉及）。
- `app/cdlapp_assets/cdlapp.css`（同上 serve）。
- `app/cdlapp_routes.py`（artifacts 两路由，**逐条重新声明**并入 server /api 孪生——踩坑 25）。
- `app/frontend_patch.py`：注 loader（幂等）。
- 输出节点摘要端口（P2）：SaveText/CdlWriteCSV/CdlModelLoad 等补 `summary`/路径 STRING 输出
  （docstring 同步，FUNCTIONS 四件套 +1 计数改动）。

## 3. Phase 划分（每 Phase 独立可验收）

| Phase | 内容 | 估量 | 验收 |
|---|---|---|---|
| **P0** | cdlapp.js 骨架：app 模式检测 + 浮层挂载/卸载 + settings 开关 + 「节点图/编辑 Inputs/编辑 Outputs」三按钮 | 1 craft | 真启动 EXIT=0；用户看：进 App 模式出现浮层、三按钮可用（跳图/跳 builder 对应步） |
| **P1** | 进度段 + 错误段：progress_state 列表（节点名/pct/state）+ execution_error 错误卡（traceback 折叠+复制） | 1 craft | formula_playground_app 实跑：进度逐节点更新；人为制造失败（坏公式）→ 错误卡全信息 |
| **P2** | 输出段 + artifacts 路由：executed 事件收集文件/文本输出 → 下载/复制条目；artifacts 路由 + path jail + 集成测试；输出节点摘要端口 | 1.5 craft | 跑 playground → 面板出现可下载条目；CSV/txt/pt 三类真实下载成功；越权路径 403 |
| **P3** | i18n（zh bake 常规流程）+ 文档四件套 + 计数 + 全量冒烟双盘 + 提交推送 | 0.5 craft | 0 FAIL；README App 段更新 |

总计 **4 craft**。P0/P1 不改节点（纯前端+无新路由），P2 动服务端与节点端口。

## 4. 测试矩阵（`test_cdlapp.py` 专项 + 主冒烟）

1. 路由集成（aiohttp TestServer，client_max_size 惯例）：artifacts 列表/下载/越权 403/空目录。
2. cdlapp.js 内容断言（仿 test_motion 契约式）：settingsStoreReady gate 在场、coach-id 锚点、
   z-index 带内、事件名逐一在场、幂等挂载。
3. frontend_patch loader 注入断言。
4. settings 开关断言（ComfyDL.AppPanel.Enabled + zh bake 常量）。
5. 输出节点摘要端口的单测（shape/fmt 字符串）。
6. 主冒烟 0 FAIL 回归。

## 5. 风险与已知坑（引用既有踩坑）

- **坑 26/进度**：每步恰一次 pbar 契约——进度面板只消费不发。
- **坑 25**：新路由必须真启动验证 + 逐条并入 /api 孪生。
- **坑 22/template**：`isApp: True` 模板条目契约（已入档）。
- L3/L21 不可读区残留未知（官方 app 中心渲染细节）→ P0 先用真启动 + 锚点探测小步验证，
  视觉观感全部交用户。
- 官方 beta 迭代可能移 coach-id/testid → 锚点检测做成多信号或门（app-run-button ∥
  inputs-list ∥ view-mode-toggle 卸载），并留 settings 逃生开关。
- 下载安全：path jail + 符号链接拒绝 + 仅 GET。

## 6. 明确不做（本线边界）

- 不重写官方输入面板/中心区；不碰 Vue chunk 内部。
- 不做 tensor 原始传输（只做摘要/文件化）；不做云 share/links。
- 不改 builder 四步流程本身（只加入口）。
- /jobs 结果面板本地 404 属官方云边界，不修（我们的输出段取代之）。
