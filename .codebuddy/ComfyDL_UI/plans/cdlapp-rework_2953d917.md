---
name: cdlapp-rework
overview: 把 ComfyUI Beta App mode 改造成 DL 工作台：官方中央与输入面板保留，新增 CDL App Panel 浮层（输出=文件下载/可复制数据/低算力 preview、详细进度条、错误卡"死的明白"、跳转节点图与 builder 选择器入口），全部官方契约经 adapter 层隔离，为官方 beta→正式的更新与合并留出空间。Phase P0-P3 共约 4 craft。
design:
  architecture:
    framework: html
  styleKeywords:
    - 暗色工具面板
    - 高信息密度
    - 官方变量跟随
    - 等宽数据
    - 低饱和 teal 点缀
  fontSystem:
    fontFamily: inherit ComfyUI frontend stack
    heading:
      size: 13px
      weight: 600
    subheading:
      size: 12px
      weight: 500
    body:
      size: 12px
      weight: 400
  colorSystem:
    primary:
      - "#1ABC9C"
      - "#16A085"
    background:
      - "var(--comfy-menu-bg, #1E1E1E)"
      - "#252525"
    text:
      - "var(--fg-color, #FFFFFF)"
      - "#9E9E9E"
    functional:
      - "#E5484D"
      - "#F5A623"
      - "#4CAF50"
todos:
  - id: cdlapp-skeleton
    content: 新建 cdlapp.js/css：OFFICIAL_SURFACE compat 层、App 模式多信号检测、浮层挂载卸载、三导航按钮、settings 开关
    status: completed
  - id: cdlapp-loader-pin
    content: frontend_patch loader 注入；新建 test_cdlapp.py：pip 前端锚点契约 pin + JS 内容断言
    status: completed
    dependencies:
      - cdlapp-skeleton
  - id: cdlapp-progress-error
    content: 进度段（progress_state per-node 列表）与错误段（execution_error 错误卡，traceback 折叠+复制）
    status: completed
    dependencies:
      - cdlapp-skeleton
  - id: cdlapp-artifacts-routes
    content: 新建 cdlapp_routes.py（artifacts 列表/下载/path jail），路由集成测试 + 真启动验证
    status: completed
    dependencies:
      - cdlapp-skeleton
  - id: cdlapp-outputs
    content: 输出段：executed 产物收集、下载/复制条目、输出节点 summary 路径端口补齐
    status: completed
    dependencies:
      - cdlapp-progress-error
      - cdlapp-artifacts-routes
  - id: cdlapp-i18n-docs
    content: 面板双语字典与 settings zh 键、docs/cdlapp-mode.md 设计文档、README/FUNCTIONS 计数同步
    status: completed
    dependencies:
      - cdlapp-loader-pin
      - cdlapp-outputs
  - id: cdlapp-finalize
    content: 全量冒烟双盘 + 真启动 EXIT=0 + formula_playground_app 试验床验收，经用户确认后提交推送与 F 盘同步
    status: completed
    dependencies:
      - cdlapp-i18n-docs
---

## Product Overview

把 ComfyUI 的 Beta App mode（官方内部代号 linear mode）改造成深度学习工作台界面：官方 App 视图的中央画布与右侧输入面板保留，新增 ComfyDL 自有的「CDL App Panel」浮层，聚焦三要素——输入（官方面板承载）、输出（文件下载/可复制数据/低算力预览，与输入平权，不再让观赏性图片式的输出占据中央）、进度（详细 per-node 进度条）。核心约束：官方 App 未来可能由 beta 转为正式功能，所有对官方内部契约为依赖必须在代码层面隔离，为参考与合并留出空间。

## Core Features

- **输出区（重设计）**：收集运行产物为条目列表——可下载文件（CSV/txt/pt 等真实文件）、可复制文本/数据、低算力高质量预览（复用 CdlPlot/CdlHeatmaps 的 matplotlib PNG 与既有实时预览通道）；tensor 类输出只出 shape/dtype/统计摘要，不传原始张量
- **进度区**：per-node 详细进度列表（节点名 + 百分比 + running/finished/error 等状态）+ 总进度条；只消费官方 progress_state 事件，绝不反向发送
- **错误区（死的明白）**：execution_error 驱动的错误卡——节点标题/类型/异常消息 + 可折叠 traceback + 一键复制
- **导航入口**：面板顶栏「节点图」按钮（跳回原 workflow）、「编辑 Inputs / 编辑 Outputs」按钮（直达 builder 对应步骤，不再只能在建 App 流程里选择）
- **官方演进隔离**：compat 适配层集中管理全部官方契约（DOM 锚点/事件名/命令 id/访问路径/前端版本），能力探测 + 按特性降级；契约 pin 测试在前端包升级时响亮失败并给出迁移指引
- **独立开关**：`ComfyDL.AppPanel.Enabled` 设置项，关闭时零 DOM 注入

## Tech Stack

- 前端注入：vanilla ES module（`cdlapp.js`）+ `cdlapp.css`，静态 serve 于 `/comfydl/cdlapp/`，boot 模式照抄 profiler.js/motion.js（waitFor comfyAPI → **settingsStoreReady gate**（`extensionManager.setting.isReady`，2026-10-30 白屏铁律）→ registerExtension）
- 服务端：aiohttp 新路由模块 `app/cdlapp_routes.py`，**逐条重新声明**并入 `/api` 孪生（坑 25），真启动验证
- 输出文件通道：复用文件节点既有 `output/` 相对路径约定（CdlWriteCSV/CdlModelSave/SaveText）
- 测试：新建 `cdl_smoaste_tests/test_cdlapp.py`（aiohttp TestServer 集成 + JS 内容断言 + **pip 前端锚点契约 pin**），主冒烟 306/33/0 回归
- i18n：面板内联双语字典（SPLASH_I18N 模式）+ settings zh 键进 frontend_patch 常量

## Implementation Approach

**方法**：不碰官方 Vue chunk 内部（beta 期随时会变），以「官方锚点探测 + 自有浮层」实现全部功能；把「官方会变」作为第一设计约束。

**关键决策与权衡**：

1. **ADR-1 布局**：中央画布与官方右侧输入面板（真 widget 语义、零维护）原样保留；CDL App Panel 作左侧 dock 浮层（z-index 安全带 2001–9999，官方 splash=9999/tooltip=2000 实测值）。不重排中心区——Vue 组件不可注入，patch chunk 成本高且与官方演进正面冲突
2. **ADR-8 官方演进隔离（本计划新增，响应用户核心要求）**：所有官方依赖收敛进 cdlapp.js 顶部单一 `OFFICIAL_SURFACE` 常量（锚点/事件/命令/版本号），运行时能力探测 + **按特性降级**（某锚点消失只降级对应段落，console.warn 指明，绝不白屏）；`test_cdlapp.py` 逐条 pin 依赖的官方 chunk 字符串——前端 pip 包升级时测试响亮 FAIL 并打印迁移指引（沿用 frontend_patch「签名漂移 fail loudly」哲学）；服务端 artifacts 载荷刻意镜像官方 `/jobs/{id}/assets` 的资产 schema（filename/subfolder/type/mediaType），未来官方本地化实现可直接替换
3. **ADR-3 输出通道**：新路由 `GET /comfydl/artifacts`（列 output/ 产物）+ `GET /comfydl/artifacts/download`（Content-Disposition attachment；path jail = resolve 后强制限定 output 根 + 拒绝符号链接 + 仅 GET）
4. **ADR-5 进度/错误**：只消费不发（每步恰一次 pbar 契约）；错误卡展示 `resolveNodeInfo(node_id).title` 回退 node_type，traceback 数组折叠渲染
5. **ADR-6 builder 入口**：`enterBuilder()` 位于 builder store（settingStore 208:95489），无命令 id——CRAFT 时先用 Python 探针确认从扩展上下文可达的调用路径（app.extensionManager 或 store 暴露面），探不通则降级为仅「节点图」按钮 + 打开面包屑菜单

**性能与可靠性**：面板为事件驱动零轮询（WS 事件 + MutationObserver 锚点监听）；artifacts 列表惰性请求（每次 Run 完成后刷新，限 200 条按 mtime 排序）；下载流式传输不进内存；面板 DOM 挂载/卸载幂等，observer 断连干净（motion.js 纪律）。

## Architecture Design

```mermaid
graph TB
    subgraph 官方前端（只读依赖，compat 层隔离）
        A[data-coach-id 锚点] --> D
        B[WS: progress_state / executed / execution_error] --> D
        C[命令: Comfy.ToggleLinear / Interrupt] --> D
    end
    subgraph ComfyDL 注入层
        D[cdlapp.js<br/>OFFICIAL_SURFACE compat + 能力探测] --> E[CDL App Panel 浮层]
        E --> E1[输出段: 文件/复制/预览]
        E --> E2[进度段: per-node 列表]
        E --> E3[错误段: 错误卡]
        E --> E4[导航: 节点图 / 编辑Inputs / 编辑Outputs]
        D --> F[ComfyDL.AppPanel.Enabled 开关]
    end
    subgraph 服务端（自有命名空间）
        G[cdlapp_routes.py<br/>/comfydl/artifacts + /download] --> H[output/ 目录 path jail]
        E1 --> G
    end
```

## Directory Structure

```
ComfyDL_UI/
├── app/
│   ├── cdlapp_assets/
│   │   ├── cdlapp.js    # [NEW] 面板本体：OFFICIAL_SURFACE compat 层（锚点/事件/命令/版本集中）+
│   │    │               #   App 模式多信号检测（coach-id ∥ view-mode-toggle 卸载）+ 浮层挂载/卸载 +
│   │    │               #   输出/进度/错误三段 + 三导航按钮 + settings 开关；按特性降级，绝不白屏
│   │   └── cdlapp.css   # [NEW] 样式：官方主题 CSS 变量 + 品牌点缀，z-index 2001-9999，紧凑行高
│   ├── cdlapp_routes.py # [NEW] GET /comfydl/artifacts（列表）+ /comfydl/artifacts/download（流式下载，
│   │                    #   Content-Disposition）；path jail（resolve+commonpath+拒符号链接+仅 GET）；
│   │                    #   载荷镜像官方资产 schema；逐条并入 /api 孪生
│   ├── frontend_patch.py # [MODIFY] _PATCHERS 表加 cdlapp loader 注入（幂等，fail loudly）
│   └── server.py         # [MODIFY] static 挂载 /comfydl/cdlapp/ + cdlapp_routes 并入
├── comfydl/nodes/
│   ├── data_io_writers.py # [MODIFY] P2：输出节点核对/补齐产物路径 STRING 输出（写盘后返回绝对相对路径）
│   └── model_utils.py 等   # [MODIFY] P2：CdlModelSave 等补 summary/路径端口（docstring + 计数同步）
├── cdl_smoke_tests/
│   └── test_cdlapp.py     # [NEW] ①artifacts 路由集成（TestServer，client_max_size 惯例，越权 403）
│                          #   ②cdlapp.js 契约内容断言（gate/幂等/事件名/锚点）③pip 前端锚点 pin
│                          #   （GraphView/settingStore chunk 内官方字符串逐条在场，漂移即 FAIL+迁移指引）
└── docs/
    └── cdlapp-mode.md     # [NEW] P3：设计文档（含官方演进合并指引：哪些层需在前端升级时复核）
```

## Key Code Structures

compat 层是本计划的枢纽（官方契约单点收敛，升级时只改此处 + pin 测试自动报警）：

```js
// cdlapp.js 顶部——官方表面契约（唯一允许触碰官方内部的区域）
const OFFICIAL_SURFACE = {
  frontendVersion: "1.51.9",            // pip 包 Sentry release，升级时与 pin 测试对账
  anchors: {                             // 全部为多信号或门成员，任一漂移只降级对应特性
    appMode: ['[data-coach-id="app-run-button"]',
              '[data-coach-id="inputs-list"]',
              '[data-testid="view-mode-toggle"]' /* 反向：卸载即 app 模式 */],
  },
  events: ["progress_state", "executed", "execution_error", "b_preview_with_metadata"],
  commands: { toGraph: "Comfy.ToggleLinear", interrupt: "Comfy.Interrupt" },
  zIndex: { band: [2001, 9999] },        // 实测：官方 splash 9999 / tooltip 2000
  builder: { enterBuilderProbe: "builder store 访问路径，CRAFT 探针确认后落位" },
};
// 每个特性注册 { deps: [...锚点], fallback: '隐藏段落' }，drift 时 console.warn + 局部降级
```

## Design Style

CDL App Panel 为注入式工具面板（vanilla JS/CSS），完全融入 ComfyUI 暗色设计系统：背景/圆角/描边优先引用官方 CSS 变量（--comfy-menu-bg 系），保证官方主题切换时自动跟随。布局为左侧固定宽度 dock（约 300px，可折叠成图标条），纵向三段：输出（默认展开，条目=图标+文件名+大小+下载/复制按钮）、进度（总进度条 + per-node 紧凑行）、错误（红调错误卡，traceback 等宽字体折叠块）。交互遵守既有铁律：节点 DOM 动画 opacity-only、cdlCancelAfter、门类 class；面板开合 200ms ease。数据文本用等宽字体（官方 mono 栈），品牌点缀沿用 teal。移动端/窄屏不特殊处理（官方 app 自带移动布局，浮层在窄屏自动折叠）。