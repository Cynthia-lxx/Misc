---
name: comfyfront-sep-homepage-mvp2
overview: 将 ComfyFront_sep 静态前端升级为"项目主页"形态：官方扩展机制注入 home.js（iframe 容器节点、Catalog 菜单按钮、全屏叠加层），扩充 object_info 为 Docs/Web/Demo 三族内容节点（英文优先、i18n 键位化），烘焙原版 slot 配色补丁，重做带预连线演示管线的默认主页工作流。
design:
  architecture:
    framework: html
  styleKeywords:
    - 原生画布风
    - 深色主题
    - 节点即内容
    - 分区布局
  fontSystem:
    fontFamily: system-ui
    heading:
      size: 22px
      weight: 600
    subheading:
      size: 14px
      weight: 500
    body:
      size: 12.5px
      weight: 400
  colorSystem:
    primary:
      - "#3f789e"
      - "#1ABC9C"
    background:
      - "#0f1013"
      - "#16171c"
    text:
      - "#e6e6ec"
      - "#9a9aa5"
    functional:
      - "#262832"
      - "#FF8C42"
todos:
  - id: bake-slot-colours
    content: 编写 bake_patches.py 烘焙 TENSOR/nn_model/DATASET 槽色补丁，并生成 extensions-manifest.json（内置 36 个 JS + home.js 清单）
    status: completed
  - id: expand-mock-client
    content: 扩写 mock_client.js：extensions 返回全集、object_info 换为 Docs/Web/Demo 三族（英文正文、真实槽类型）、i18n 回源静态文件、新默认主页工作流（预连线演示管线）
    status: completed
    dependencies:
      - bake-slot-colours
  - id: home-extension
    content: 用 [subagent:code-explorer] 核查 DOM widget/markdown 旗标/i18n 路径后编写 extensions/comfydl_home/home.js（iframe 容器节点、大默认尺寸、懒加载、Catalog 全屏叠加层、sidebar tab 入口）
    status: completed
    dependencies:
      - expand-mock-client
  - id: catalog-and-i18n
    content: 复制 catalog HTML 到 catalog/ 目录，创建 i18n/en 的 main.json 与 nodeDefs.json 英文骨架
    status: completed
  - id: smoke-verify
    content: 本地起服务冒烟（扩展可加载、object_info/i18n/manifest 均 200 且 JSON 可解析），交用户视觉验收
    status: completed
    dependencies:
      - home-extension
      - catalog-and-i18n
---

## Product Overview

把 `P:\Dev\ComfyUI_Refs\ComfyFront_sep\`（comfyui-frontend-package 的静态拷贝，已含 MVP mock）升级为 ComfyDL_UI 的“画布即主页”静态站点：README、交互式 catalog、使用说明、技术文档、联系方式全部以节点形态呈现，牺牲真实执行功能（mock 后端供数）。未来部署 GitHub Pages；目录现位于主仓库外层，所有改动仅在此目录。

## Core Features

- **内容节点化**：README（定位/特性/效果）、简单使用说明、联系方式 → 画布 markdown 容器节点（`Docs/*` 族），正文存于 object_info widget 默认值，用户从节点库侧边栏按分类拖入画布即得全文
- **iframe 容器节点**：`Web/Catalog` 等节点通过 DOM widget 内嵌 HTML 页面，默认尺寸大（≥1200×750，满足 catalog 半屏/全屏设计要求），iframe 懒加载
- **Catalog 全屏叠加层**：顶部入口打开全屏 iframe 直接加载现有 919 KB 自包含 HTML，预留多 tab 结构供未来其他分区接入
- **可玩数据流**：`Demo/*` 假节点族（DataLoader→Network→TrainingLoop→Evaluate），槽使用真实类型（TENSOR/nn_model/cdlDataloader/DATASET）并预连线，用户可拆可重连
- **i18n 骨架**：英文优先，内容键位化落入 i18n/en JSON 文件，用户后期补 zh 表
- **配色烘焙**：将主仓 frontend_patch 的三个槽色补丁（TENSOR/nn_model/DATASET×6 主题）烘焙进静态拷贝
- 原有真实后端节点全部拿掉，仅保留 Note/MarkdownNote 原语作画布注释工具

## Tech Stack

- 前端：comfyui-frontend-package 静态拷贝（Vue 3 预编译包，零构建）+ 原生 JS 扩展
- Mock 层：mock_client.js（fetch 拦截 + WebSocket stub）
- 扩展层：官方 extension API（`app.registerExtension` / `beforeRegisterNodeDef` / `addDOMWidget` / `app.extensionManager.registerSidebarTab`，均已对照 ComfyUI_Docs_Work 权威文档确认）
- 服务：本地 `P:\Software\python-3.14.5-embed-amd64\python.exe -m http.server 8188`（启动主页.bat）；部署目标 GitHub Pages（免费、相对路径已就绪）

## Implementation Approach

1. **扩展走官方机制**：mock 的 `/api/extensions` 返回扩展清单；home.js 经前端动态 import 加载，负责 iframe DOM widget 挂载、节点大默认尺寸、Catalog 叠加层。远期可直接迁入真实后端 `EXTENSION_WEB_DIRS` 复用
2. **内容走 object_info**：Docs/Web/Demo 三族节点全部定义在 mock 的 object_info 中（markdown 正文 = STRING widget 默认值 + `markdown:true` 旗标，前端原生渲染，无需自写渲染器）
3. **配色走烘焙**：frontend_patch.py 的 `_patch_tensor_slot_colour/_patch_nn_model_slot_colour/_patch_dataset_slot_colour` 是内容标记式替换、签名仅依赖 assets 路径，可直接对拷贝调用

## Implementation Notes

- **关键发现**：真实 server.py 的 `/api/extensions` 会 glob `web_root/extensions/**/*.js`（server.py:416-420），即包内 36 个内置扩展 JS 也会被加载——mock 必须返回“内置清单 + home.js”全集，否则会破坏 maskEditor 等内置功能。方案：生成一份扩展 manifest JSON，mock 经 realFetch 读取后返回
- **懒加载**：iframe 的 src 在节点首次绘制/交互时才赋值，避免主页首屏即拉 919 KB catalog
- **Catalog 入口双保险**：`registerSidebarTab`（文档确认的 API）为主入口；菜单栏注册 API 若存在则 feature-detect 追加，不存在则不阻塞
- 默认工作流仍走 `Comfy.Workflow.Workflow` 设置键（MVP 已验证链路）
- 视觉验收交用户，AI 只做 HTTP 层冒烟（端点 200、路径可达、JSON 可解析）

## Architecture Design

- 数据流：index.html → mock_client.js（拦截 /api/*，喂 object_info/settings/i18n/extensions）→ 前端 Vue 应用加载 home.js（官方扩展）→ home.js 给 Web/* 挂 iframe widget、注册 Catalog 叠加层 → 用户从节点库拖 Docs/Web/Demo 节点到画布
- 分层清晰：内容（object_info + i18n JSON）与行为（home.js）分离；catalog HTML 为纯静态资产原样复制，不改主仓任何文件

## Directory Structure

```
ComfyFront_sep/
├── index.html                          # [MODIFY]（如需）微调；mock_client 注入已在 MVP 完成
├── mock_client.js                      # [MODIFY] 扩写：/api/extensions 返回全集清单；object_info 换成 Docs/Web/Demo 三族+Note/MarkdownNote；i18n 端点改为回源静态文件；新默认主页工作流（About/Contact/Demo pipeline 预连线）
├── extensions/
│   └── comfydl_home/
│       └── home.js                     # [NEW] 官方扩展：beforeRegisterNodeDef 挂 iframe DOM widget+大默认尺寸+懒加载；Catalog 全屏叠加层（iframe，多 tab 预留）；registerSidebarTab 文档入口
├── extensions-manifest.json            # [NEW] 内置 36 个扩展 JS + home.js 的路径清单（供 mock 返回）
├── i18n/
│   └── en/
│       ├── main.json                   # [NEW] UI 字符串键位骨架（英文）
│       └── nodeDefs.json               # [NEW] Docs/Web/Demo 节点显示名与描述（英文，键位化待补 zh）
├── catalog/
│   └── network-and-layers.html         # [NEW] 从 ComfyDL_UI/docs 原样复制（只读复制，不动主仓）
├── bake_patches.py                     # [NEW] 一次性脚本：import 主仓 app.frontend_patch，仅烘焙三个槽色补丁到 assets/
└── 启动主页.bat                          # [已有] 不变
```

## Design Style

宿主界面完全沿用 ComfyUI 前端原生观感（深色画布、节点图元、面板体系），不做皮肤改造。自研部分仅两处视觉面：

1. **Catalog 全屏叠加层**：全屏 iframe + 半透明遮罩，右上角关闭按钮；顶部预留 tab 条（当前仅 Network & Layers 一页），风格向 catalog 自带深色主题（#0f1013 底、#262832 边框）靠拢，保证 iframe 内容与外壳无缝。
2. **主页画布排版**：Group 分区（About / Contact / Try it / Docs）+ 预连线演示管线；节点标题与内容英文，深浅主题均可用（跟随前端 6 套主题）。iframe 容器节点默认 1200×750 起，绝不小于画布上任意现有节点。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose：CRAFT 期间对已编译前端包做定点核查（markdown widget 的 `markdown:true` 旗标在编译产物中的实际消费方式、DOM widget API 的实际签名、/api/i18n 的前端请求路径格式），防止按文档想象写错
- Expected outcome：home.js 与 mock_client.js 使用的每个前端内部 API 都有编译产物或权威文档双重佐证