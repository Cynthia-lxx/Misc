---
name: profiling-v2-p3-layout-exports
overview: Profiling v2 P3：expand 全屏 overlay 改造为三列平面分区布局（左 estimates/verdict + 中 等效计算图主视区 + 右 实测历史），参考 ComfyUI-Manager 的分区范式；同时补齐导出矩阵（新增 SVG/PNG 图像导出——从手绘 SVG 序列化，零新依赖；mermaid live 链接；等效 workflow 与 .vsdx 维持 backlog）。结尾含 memory 写入与 F 盘同步。
design:
  architecture:
    framework: html
  styleKeywords:
    - 分区仪表盘
    - 暗色宿主融合
    - 信息密度均衡
  fontSystem:
    fontFamily: var(--font-family, Segoe UI)
    heading:
      size: 13px
      weight: 600
    subheading:
      size: 12px
      weight: 600
    body:
      size: 11px
      weight: 400
  colorSystem:
    primary:
      - "var(--fg-color, #e0e0e0)"
      - "var(--comfy-menu-bg, #1f1f1f)"
      - "var(--border-color, #2a2a2a)"
    background:
      - rgba(0, 0, 0, 0.7)
      - "var(--comfy-menu-bg, #1f1f1f)"
    text:
      - "var(--fg-color, #e0e0e0)"
      - "var(--input-text-read-only, #9a9a9a)"
    functional:
      - "#f87171"
      - "#fbbf24"
      - "#4ade80"
todos:
  - id: p3-preread
    content: 前置阅读：用 [subagent:code-explorer] 核对 profiler.js 卡片渲染块边界、profiler.css overlay 段、test_profiling_log.py T4 断言清单，并复读 bigplan memory 与 2026-10-06.md P3 决策
    status: completed
  - id: p3-three-col-layout
    content: 改造 profiler.js mountOverlay 静态三列 DOM + renderPanel 按容器分流（overlay 三列/侧栏单列），profiler.css 新增三列布局与窄屏降级，保持 renderOverlay 只切 class 架构
    status: completed
  - id: p3-exports
    content: 扩展 exportAssembled：新增 SVG/PNG 导出与 mermaid.live 快捷方式（剪贴板复制+尝试打开），i18n en/zh 加键，wirePanel 绑定新按钮
    status: completed
  - id: p3-tests
    content: test_profiling_log.py T4 段新增三列布局与导出分支静态断言，跑 profiling 专项与全量冒烟（P 盘）
    status: completed
  - id: p3-docs-sync
    content: docs/profiling-m3-opgraph.md 补 P3 分区布局与导出矩阵说明
    status: completed
  - id: p3-ship-memory
    content: 提交推送 + F 盘同步复验 + memory 写入（2026-10-06.md 续节 + MEMORY.md 索引 + 庆典计数 97/100 更新）
    status: completed
---

## Product Overview

Profiling v2 P3 阶段：将 Profiling 面板的 expand 全屏 overlay 从单列纵向堆叠改造为三列平面分区布局，充分利用横向空间；同时补齐等效计算图的导出矩阵（新增 SVG 文件、PNG 图片、mermaid.live 快捷方式），完成大计划最后一个 Phase。

## Core Features

- **三列分区 overlay**：左列 = estimates 摘要（verdict 三色卡 / assumptions / budget）；中列 = 等效计算图 SVG 主视区（最大空间，横向滚动）；右列 = 实测历史（history 卡 / 爆点记录 / 服务端日志）。侧栏 tab 保持现有单列不变。
- **分区交互**：各分区带可折叠小标题栏（沿用现有 collapse 机制），参考 ComfyUI-Manager 的分区范式（宿主 CSS 变量取色、z-index 分层、窄屏降级为单列堆叠）。
- **导出矩阵扩展**：在已有 JSON / 文本大纲 / mermaid 之上新增——SVG 文件导出（手绘 SVG 字符串直接落 Blob）、PNG 导出（SVG→canvas→toBlob，2x 高分辨率，零新依赖）、mermaid.live 快捷方式（剪贴板复制 + 尝试打开编辑器链接）。
- **验收与收尾**：防回归测试、双盘冒烟、文档同步、memory 写入（含庆典计数 97/100 更新）。

## Tech Stack

- 前端：项目既有原生 JS 前端资产（app/profiling_assets/profiler.js + profiler.css），零新依赖、零构建步骤（宿主补丁层惯例）。
- 样式：宿主 ComfyUI CSS 变量取色（--font-family / --border-color / --comfy-menu-bg / --fg-color 等），全局 sans-first + tabular-nums（memory 约定：monospace 永远最后兜底）。
- 测试：cdl_smoke_tests 既有 check 列表范式（T4 段 JS 静态断言 + 全量冒烟）。

## Implementation Approach

- **布局改造**：mountOverlay 在 overlay body 内静态建立三列容器（.cdlp-cols > 左/中/右三个 .cdlp-col），renderPanel 按目标容器分流——overlay 容器走三列填充函数、侧栏容器维持单列拼接。复用既有卡片渲染块（verdict/assumptions/opgraph 汇总/实测历史/服务端日志），按列归属重新编排，不重写卡片内部。
- **架构红线**：renderOverlay 保持“只切 cdlp-overlay-open class”的 P0 修复架构不动；displayMode 恒从 sidebar 起不持久化（T4j/T4k 防回归必须保持绿）。
- **导出实现**：exportAssembled(kind) 分支扩展：svg = 把 renderEquivalentGraphSvg 输出的 svg 字符串加 XML 头落 Blob；png = new Image(src=blob URL) → canvas(2x) → toBlob 下载；mermaid link = 尝试 navigator.clipboard 复制 buildMermaid 文本并 window.open mermaid.live（无 pako 压缩时提示已复制）。i18n en/zh 同步加键。
- **性能**：三列均为既有 DOM 重建路径，无新增请求；PNG canvas 光栅化仅在点击导出时执行；中列 SVG 复用现有横向滚动容器。

## Directory Structure Summary

```
ComfyDL_UI/
├── app/profiling_assets/profiler.js   # [MODIFY] mountOverlay 三列 DOM、renderPanel 分流、exportAssembled 扩展、i18n 键
├── app/profiling_assets/profiler.css  # [MODIFY] .cdlp-cols 三列 flex 布局、分区标题栏、窄屏降级
├── cdl_smoke_tests/test_profiling_log.py  # [MODIFY] T4 段新增三列布局与导出分支静态断言
├── docs/profiling-m3-opgraph.md       # [MODIFY] §6 前端交互补 P3 分区布局与导出矩阵说明
└── .codebuddy/memory/                 # [MODIFY] 2026-10-06.md 续节 + MEMORY.md 索引与庆典计数
```

## Design Style

延续 Profiling 面板既有视觉体系（宿主暗色 CSS 变量、圆角卡片、分区标题栏 + 折叠箭头）。overlay 改为三列 flex 分区：左 260px estimates 摘要列、中列自适应（等效图 SVG 主视区，overflow-x auto + 点状画布背景延续）、右 300px 实测历史列；各列带小标题栏与折叠按钮；窄屏（<1100px）降级为纵向堆叠。导出按钮组位于中列标题栏右侧，新增 SVG/PNG/Link 三项与既有 JSON/Outline/Mermaid 并列。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose：前置阅读阶段快速核对 profiler.js 的 renderPanel 卡片渲染块边界与 profiler.css overlay 段现状，以及 test_profiling_log.py T4 段断言清单，避免分流改造遗漏调用点。
- Expected outcome：输出卡片渲染块清单（函数名/行号/列归属建议）与 T4 现有断言编号表，供实施直接引用。