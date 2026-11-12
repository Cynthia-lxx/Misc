# Refs 调研报告 C：UI 类（11 仓库）

> 调研日期：2026-10-30。源仓库位于 `P:\Dev\ComfyUI_Refs\Refs\`（浅克隆）。

---

## C1. comfyui-mcp-panel（artokun）

**定位**：嵌入 ComfyUI 侧边栏的 AI Agent 面板，让后台编排器直接在画布上加节点/连线/跑工作流（纯前端插件，零 Python 节点）。

**组成**：`__init__.py` 仅暴露 `WEB_DIRECTORY="./web"` + 注册 `/comfyui_mcp_panel/*` REST 路由，无节点。前端 2.48MB 主 bundle + 模块源码（`cmcp-sidepanel-ui.js` 四合一 Tab 侧板等），`web/js/lib/` 约 200+ 单一职责小模块。

**核心实现**：面板经 WebSocket 连回环桥 `ws://127.0.0.1:9199`（__init__.py:66-96）；Python 侧 `PromptServer.instance.routes` 注册 /status /backends /connect /bridge_url /civitai/* /apps/* /training/*，均带 `_refuse_cross_origin` 防护（:1219-1335）。

**可借鉴**：侧板 Shell 抽象——内容提供者接口 `{key, label(getter 延迟翻译), mount, onSearch, teardown}`（cmcp-sidepanel-ui.js:1-43），适合多面板复用；路由按模块 `register(routes, web)` 注册、失败不阻塞加载（:1228-1253）；JS 字符串一次性 CSS 注入（injectCss）；`/version` 路由检测浏览器缓存旧 JS；i18n 目录式 locale + RTL。

**依赖/风险**：外部 Node≥22 编排器（本项目需规避）；固定端口 9199/9180 占用风险；单 bundle 2.5MB 过重，lib/ 模块化才是方向。

---

## C2. ComfyUI-disty-Flow（diStyApps）

**定位**："App 化"精简前端：把工作流包装成带自定义控件（下拉/步进器/种子/画布）的独立页面，面向不懂节点的终端用户。

**组成**：唯一占位节点 `Flow`（flow/flow_node.py）；后端 flow/（route_manager/flow_manager/api_handlers）；前端 web/core（框架）+ web/flow/js/main.js；组件库 web/core/js/common/components/（Dropdown、Stepper、Seeder、ProgressBar、fabric.js 画布 + MaskBrush/UndoRedo）。

**核心实现**：导入即 `setup_server()` 挂 aiohttp 路由（flow/server_setup.py:7）；每个 flow 目录映射为 `/flow/<url>` 独立页面（flow_manager.py:37-94）；进度/图像走 ComfyUI 原生 `/ws?clientId=` WebSocket，按 JSON/Blob 分流（messageHandler.js:144-153）；对主界面注入仅一个入口：`app.registerExtension` + MutationObserver 等待 `.comfyui-menu-right` 出现后插入按钮（ui_utils.js:80-95）；flowConfig.json 声明式生成控件（main.js:88-141）。

**可借鉴**：**独立子页面方案——完全绕开画布，用原生 /ws + /view + /prompt API 即可做监控/控制台，适合 ComfyDL 的 profiling 面板**；MutationObserver+选择器等待菜单 DOM 的注入模式（比 extension 注册更稳）；flowConfig.json 声明式控件生成。

**依赖/风险**：aiohttp/PIL；硬编码依赖 ComfyUI-Manager 的 extension-node-map.json；项目停更迹象；反向代理路径前缀需自行处理。

---

## C3. ComfyUI_tinyterraNodes（TinyTerra）

**定位**：超大型 SD 节点包（pipe 一键节点、xyPlot 批量扫描、图床查看器）+ 一整套前端 UI 增强。

**组成**：后端 ttNpy/tinyterraNodes.py(177KB)；前端 10 个 JS，`config.ini` 开关实现启动时 rename js→.disable 做功能裁剪（__init__.py L133-144）。`ttN.js` 总入口注册 6 个 extension + 画布右键 ttN 子菜单；`ttNinterface.js` 键盘微移/尺寸对话框/slot 菜单/CSS 注入；`ttNimgViewer.js` 图片查看器；`ttNdynamicWidgets.js` widget 联动显隐等。

**核心实现**：monkey-patch `LGraphCanvas.prototype.getCanvasMenuOptions/getNodeMenuOptions` 注入菜单（ttN.js L28-51, L554-557）；`app.registerExtension` 的 beforeRegisterNodeDef/nodeCreated 按节点名逐个 patch（ttN.js L673-699）；自定义 dialog 挂 `canvas.parentNode`（ttNinterface.js L71-143）；查看器全屏 requestFullscreen / 弹窗 window.open 双形态；图片历史存 localStorage（每节点上限 100），`api.addEventListener("status/progress/executed")` 刷新+防抖；后端 `/ttN/reboot|models|loras` 自定义 REST（ttNserver.py），前端 `api.fetchApi` 消费（utils.js L9）。

**可借鉴**：a) 菜单/交互注入统一收口在一个 class（`TinyTerra extends EventTarget`，统一管理按键与全局 keydown）——适合 frontend_patch 架构；b) `toggleWidget` 显隐方案：改 widget.type + 覆写 `computeSize=()=>[0,-4]` 联动节点高度（ttNdynamicWidgets.js L14-30），V1 节点条件参数 UI 可直接套用；c) 自定义 link 类型配色走 `LGraphCanvas.link_type_colors`（ttNinterface.js L10-20），profiling 着色可复用；d) 自定义 REST + fetchApi（比 WS 轻量）；e) 事件驱动+防抖。

**风险**：大量 monkey-patch LiteGraph 内部 API，前端重构后极易碎；/ttN/reboot 直接 os.execv 粗暴；localStorage 键名与官方 Settings 混用易冲突。

---

## C4. Comfyui-Minimap（OliverCrosby）

**定位**：纯前端画布小地图，在主画布旁渲染整个 workflow 缩略图，点击/拖动导航主画布。

**组成**：仅两个 JS（minimap.js 约 357 行、size_position.js），无节点；`__init__.py` 空 NODE_CLASS_MAPPINGS + `WEB_DIRECTORY="."`——**"零后端、纯前端扩展"的最小骨架范本**。

**核心实现**：自建 `<div id='minimap'>` + `<canvas>` append 到 document.body（z-index 1000），**不依赖 LiteGraph 渲染管线**——直接读 `app.graph._nodes/_groups/links` 的 pos/size 手算 bounds/scale 后 2D context 画矩形连线；视口框经 `app.canvas.ds.scale/offset` 换算（L218-233）；导航反向计算改 `app.canvas.ds.offset` + `setDirty(true,true)`（L266-283）；`setInterval(render, 100)` 轮询；`api.addEventListener("executing")` 给执行中节点画绿框；Ctrl+拖动调位置、Ctrl+滚轮调透明度，设置存 localStorage。

**可借鉴**：`app.canvas.ds.offset/scale` 做"世界↔屏幕坐标"双向换算（profiling 覆盖层/节点高亮可直接复用）；**独立 canvas 叠加层方案完全绕开 LiteGraph 内部 API，比 patch 主 canvas 更抗版本变动**；`waitForAppAndGraph` 轮询等待 app 就绪的注入时序兜底。

**风险**：100ms 轮询费电；访问 `_nodes/_groups` 私有字段，新版前端可能失效。

---

## C5. comfyui-node-organizer（PBandDev）

**定位**：纯前端扩展，一键"Organize Workflow/Group"自动整理画布，含分组感知的分层布局。无后端节点。

**组成**：TS 项目（Vite → `WEB_DIRECTORY=./dist`）：src/index.ts（注册入口）、adapter.ts（LiteGraph 桥接）、layout/framework.ts（核心框架）、layout/algorithms/（sugiyama/grid/horizontal/vertical）、group-membership.ts；并发布 npm 包 `comfyui-node-organizer`。

**核心实现**：入口全走官方 `app.registerExtension` 声明式注册 commands/keybindings(Shift+O)/canvas 右键菜单/actionBarButtons/settings（src/index.ts:204-407），**不走注入 hack**；adapter.ts:77 用结构化类型（GraphLike）把 LGraph 抽成纯数据 nodes/edges/groups；分组归属靠矩形空间包含推断（group-membership.ts）；`layoutWithGroups`（framework.ts:35）分组自底向上拓扑排序（:192），每组独立布局后作为"虚拟节点"参与父层布局；组标题 token（`[HORIZONTAL]/[2ROW]`，tokens.ts:18）切换算法；无连线节点放 DAG 左侧竖排；sugiyama 为完整四阶段实现；布局后 fit-view 延迟一帧 `canvas.animateToBounds`（index.ts:146-168）。

**可借鉴**：**纯函数布局引擎 + 薄 adapter 架构**（布局逻辑可独立测试/复用）；组标题 token 是低成本声明式布局协议；虚拟节点递归分组布局是处理嵌套 group/subgraph 的成熟方案；官方扩展注册点清单比前端 patch 注入更稳。

**依赖/风险**：依赖 `@comfyorg/comfyui-frontend-types`（需新前端 v1.x 扩展 API）；LICENSE 33KB 疑似 GPL，需核对。

---

## C6. ComfyUI-NodeAlign（1038lab）

**定位**：轻量对齐工具栏——对选中节点做对齐/等尺寸/等间距分布，无布局算法。无节点，前端仅两个裸 JS（NodeAlign.js 约 24KB + settings.js）。

**核心实现**：`app.registerExtension` + rAF 轮询等 DOM（`.comfyui-menu`、`#graph-canvas` 多选择器兜底，:45-55）后注入 `<style>` + 浮动工具栏（:77-162），position 存 localStorage；`getSelectedNodes` 兼容 Map/Set/Array/object 三种 selected_nodes 来源（:174-184）；对齐/分布直接改 node.pos/size 后 `setDirty(true,true)` + `graph.setDirtyCanvas`（:249-252）；分布算法：排序后按 `(总跨度-总尺寸)/(n-1)` 均分 gap（:228-247）；工具栏拖拽/吸附菜单栏/双击复位/Shift+WASD。

**可借鉴**：多选择器/多 selected_nodes 兜底（适配新旧前端）；**"选中即显示"（监听 canvas pointerup + rAF 防抖刷新，:254-269）适合 profiling 面板按需显隐**；等间距分布公式直接复用；改 pos 后 setDirty 双刷闭环。

**风险**：DOM/选择器硬编码脆弱（作者为 v3.7.3 前端 fork 维护）——ComfyDL 应优先 node-organizer 式官方 API，仅作兜底参考。零构建零依赖。

---

## C7. comfyui-obvpm（chanon）

**定位**："Bundle"把多根线合并为一根线整理工作流，附 Value Presets、节点内图片裁剪等，约 30 个节点 + 21 个前端 js。

**核心实现**：每功能模块用 `app.registerExtension` + beforeRegisterNodeDef/onNodeCreated 包装原型（web/value_presets.js:864 等），非全局 DOM 注入；后端 `presets.py:922` 在 `PromptServer.instance.routes` 注册 `POST /obvpm/presets/schema`、`GET /obvpm/presets/types`，try/except 兜底 headless 场景；canvas 内交互 widget：隐藏 widget + `computeLayoutSize`/`mouse()` 回调画可拖拽裁剪框，兼容经典 canvas 与 Nodes 2.0 双渲染器（load_image_crop.js:41-710）；`obvpm_ui.js` 提供"空 combo 防冻结"、"隐藏 widget 后残留 socket 清理（dropWidgetSockets）"等坑位修复；compat.py（36KB）专门做前端版本兼容检测；节点 key 带 `(obvpm)` 后缀统一检索 + 旧 ID 迁移注册表（ids.py）。

**可借鉴**：a) PromptServer 自定义 schema/校验路由写法；b) widget-socket 清理与 DOM 面板 `computeLayoutSize+contain:size` 配方；c) per-instance 动态 combo（给 values 传函数，obvpm_dynamic.js:17-30）；d) 双渲染器兼容层结构；e) key 命名空间后缀 + 迁移注册。

**风险**：深度依赖 Litegraph 内部 API，升级维护成本高；单文件过大（value_presets.js 150KB），抽配方不整体照搬。

---

## C8. ComfyUI-MobileClient（vkff5833）

**定位**：移动端友好独立 Web 客户端：响应式图库、队列监控、提示词编辑重生成、工作流 JSON 上传。极简：一个 Dummy 占位节点 + `web/index.html` + index.js（App 类，Bootstrap）。

**核心实现**：纯 REST 轮询无 WS：api.js:24-45 封装 `GET /queue`、`GET /api/history?max_items=N`、`POST /api/prompt`；`startQueueMonitoring` setInterval 轮询更新徽标；图库按 history 反序构建卡片，把整个 prompt JSON 存 `dataset.prompt`（index.js:92），Regenerate 时取出改 seed 后回投；Basic Auth 头（api.js:10-12）；正则 pattern 配置化识别 steps/cfg/seed 字段生成表单。

**可借鉴**：a) **"history → 可再生成 prompt"闭环**：从 /api/history 重建参数表单再回投 /api/prompt——对 ComfyDL 的 profiling 结果页/复跑面板直接可用；b) 字段 pattern 配置化识别；c) localStorage 持久化 + 独立扩展页架构（不侵主画布，风险极低）；d) fetch 封装统一带认证头。

**风险**：Bootstrap CDN；轮询有延迟；近乎停更，借鉴思路为主。

---

## C9. ComfyUI-Enhancement-Utils（phazei）——本批对 ComfyDL 最有价值

**定位**：一站式增强工具集（V3 schema）：资源监控、节点性能剖析、图片加载、JSON 解析、声音/系统通知、节点导航、自动排列等。前端纯 JS 无打包（js/ 约 30 模块），刻意省略 NODE_CLASS_MAPPINGS 走 V3（__init__.py:48-54）。

**核心实现**：
- 资源监控：后台 daemon 线程采集（monitor/collector.py）→ `PromptServer.send_sync` 推 WS 事件 `enhutils.monitor` → 前端 DOM 色条 + ring buffer 历史 + 悬停 sparkline（js/resourceMonitor.js:38-73）；设置走 HTTP：PATCH `/enhutils/monitor`、GET history（monitor/routes.py:26,77,100,154）。
- **Profiler：monkey-patch `PromptServer.send_sync` 拦截 `execution_start/executing/execution_*` 事件 + 包裹 `execution.execute` 精确计时，区分 ran/cached/blocked/skipped**（profiler/hooks.py:1-42）。
- 前端徽章：Legacy 用 `node.badges` + setDirtyCanvas；**Nodes 2.0 Vue 渲染器下改用 `[data-node-id]` 选择器注入绝对定位 div**（js/nodeProfiler.js:1-30）；模块级 Map 存 profiling 数据（不挂 node 对象）+ 徽章 getter 实时查值。
- 还演示 aiohttp middleware 注册（middleware.py:72）重写 `/api/view` 参数。

**可借鉴**：① "事件拦截 + execute 包裹"双层 hook 与 cached/blocked 分类——**直接可用于 ComfyDL profiling 后端**；② "模块级 Map + 徽章 getter"的双渲染器兼容策略是前端注入性能 UI 的最佳参考；③ WS 推送 + HTTP 设置/history 端点分离架构；④ middleware 注册时机（custom node __init__ 早于 app freeze）。

**风险**：依赖 comfy_api V3；monkey-patch 内部 API 随版本易碎；Vue 渲染器路径靠 DOM 约定。

---

## C10. ComfyUI-Elegant-Resource-Monitor（ChrisColeTech）

**定位**：画布上可拖拽悬浮卡片（Chart.js）显示 CPU/RAM/GPU/VRAM/HDD/温度 + 执行进度条。仅一个无输出占位节点，核心在 web/（含 chart.js 371KB 编译产物）。

**核心实现**：**独立 Flask+SocketIO 服务**（api/http_server.py:139-158）daemon 线程 port 5000，每 0.5s 采集 psutil+GPUtil 并 `socketio.emit('data_update')`（:114-136）；flask_restx `/gpu_usage` 带 1s 缓存与手写 CORS；前端 monkey-patch `LGraph.prototype.serialize`、`LGraphCanvas.processMouseDown` 派发自定义事件（elegant.js:73-115）；画布右键菜单按锚点字符串（'Queue Group'/'Arrange ('）定位插入项（:126-201）；监控卡片是 HTMLElement 自定义元素 + Chart.js + 九宫格位置设置；执行进度劫持 `comfy-api-queue-prompt-before` 事件（elegant-resource-monitor-node.js:21-22）。

**可借鉴**：`processMouseDown/adjustMouseEvent` 环绕 patch + CustomEvent 派发（与 frontend_patch 注入思路同源）；右键菜单按锚点定位插入的稳健写法；Chart.js 悬浮卡片 + 九宫格 UX。

**风险**：重依赖（flask/flask-socketio/flask_restx/GPUtil/Chart.js/jQuery）；**独立 5000 端口进程是硬伤**（多开冲突、CORS 全开、绕过 PromptServer）——ComfyDL 的 WS 推送方案应学 C9（复用 PromptServer），仅借鉴其图表卡片 UI。

---

## C11. lf-nodes（lucafoscili）

**定位**：约 100+ 节点的套件，核心卖点"带 UX 的节点"——自研 Stencil.js webcomponents 库（lf-widgets）提供富交互 UI（图片对比、直方图、聊天、图片编辑器等）。TS+Vite 构建，产物 web/deploy/js/lf-nodes.js（426KB）。

**核心实现**：集中扩展注册：`LFManager.initialize()`（web/src/managers/manager.ts:94-126）按 `NODE_WIDGET_MAP`（helpers/manager.ts:24+）**声明式映射"节点→自定义 widget 类型"**，再 `app.registerExtension` + `addCustomWidget` 工厂渲染（widgets.ts:38-102 的 #FACTORIES 工厂表）；后端 `@PromptServer.instance.routes.get/post("/api/lf-nodes/...")` 挂 aiohttp 路由（modules/api/system.py:105 等 33 处），前端统一封装在 web/src/api/*.ts（comfy.ts 封装 fetchApi/事件，含 rAF 节流 scheduleRedraw）；包装节点 onExecuted 从 `lf_output` 取 payload 触发 `CustomEvent('lf-result')`（widgets.ts:62-102）；全局钩子 installLFBeforeFreeHooks/RefreshNodeHook/InterruptHook。

**可借鉴**：1) **节点→widget 声明映射表模式**，替代散落 patch，极适合组织 frontend_patch.py；2) API 路由前缀 + 前端 api/*.ts 分层封装（GPU/CPU stats 路由可直接仿写 system.py）；3) onExecuted 包装器把执行输出转为前端事件——V1 节点可复用推送 profiling 结果；4) rAF 节流重绘。

**风险**：Python 依赖极重（torch/transformers/onnxruntime/rembg）；前端耦合自有 lf-widgets（2MB），只借鉴结构模式。

---

## 批次小结（对 ComfyDL 前端 patch 最有价值的横切主题）

1. **注入方式分档**：官方扩展 API（node-organizer 的 commands/keybindings/settings）> registerExtension + beforeRegisterNodeDef/nodeCreated（obvpm/lf-nodes）> MutationObserver 等 DOM（disty/NodeAlign）> monkey-patch prototype（tinyterra/Elegant，最易碎）。ComfyDL 的 frontend_patch 应按此优先级。
2. **profiling UI 三个现成蓝本**：Enhancement-Utils（事件拦截+徽章，最佳）、lf-nodes（NODE_WIDGET_MAP+onExecuted 事件）、Minimap（独立 canvas 叠加层做世界坐标换算）。
3. **后端数据通道**：优先复用 PromptServer WS send_sync + aiohttp 路由（C1/C2/C9/C11），避免独立端口进程（C10 反例）。
4. **画布整理**：node-organizer 的"纯函数布局引擎 + 薄 adapter + 组 token"是最完整的自动布局参考。
