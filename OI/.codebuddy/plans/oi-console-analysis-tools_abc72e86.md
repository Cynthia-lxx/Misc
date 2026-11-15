---
name: oi-console-analysis-tools
overview: 在现有 OI Web 控制台中新增「内存 / 复杂度 / 风格冗余 / 性能」四类以本地分析为主的工具，把 MinGW64 工具链内置进工作区，借 ComfyDL_UI profiling 的信息架构并沿用现有 osu 风格皮肤；AI 每次分析自动做 1 次严格限量的短汇总；设计决策落盘到工作区 .codebuddy/memory/。
design:
  architecture:
    framework: html
  styleKeywords:
    - 暗色分层控制台
    - 沿用现有主题 token
    - 三色判定与 unknown 诚实标注
    - 密集指标表与预算条
    - 证据行号定位
    - 轻微错峰动效
  fontSystem:
    fontFamily: Noto Sans
    heading:
      size: 22px
      weight: 700
    subheading:
      size: 14px
      weight: 600
    body:
      size: 12px
      weight: 400
  colorSystem:
    primary:
      - "#66CCFF"
      - "#99DDFF"
      - "#3399CC"
    background:
      - "#171A1C"
      - "#2E3338"
      - "#3D444A"
    text:
      - "#FFFFFF"
      - "#AAB8C0"
    functional:
      - "#4ADE80"
      - "#FBBF24"
      - "#F87171"
      - "#9A9A9A"
      - "#88B300"
      - "#ED1121"
todos:
  - id: verify-reference
    content: 用 [subagent:code-explorer] 核实参考仓库报告契约与前端扩展点，写设计基线到记忆
    status: completed
  - id: vendor-toolchain
    content: 内置 MinGW64 与 AStyle.exe 到 toolchain/，写来源许可说明并验证工作区内编译可用
    status: completed
  - id: build-analyzer
    content: 实现 analyzer 本地引擎：词法、四类估算器、假设层、三色判定与签名缓存
    status: completed
    dependencies:
      - verify-reference
  - id: add-runtime-ai
    content: 实现 toolchain.py 限时实测与 ai_summary.py 单轮限量汇总、预算及降级
    status: completed
    dependencies:
      - vendor-toolchain
      - build-analyzer
  - id: add-api-routes
    content: 在 server.py 增加 /api/analyze* 路由，复用现有响应与 is_within 安全边界
    status: completed
    dependencies:
      - build-analyzer
      - add-runtime-ai
  - id: build-analysis-view
    content: 新增「分析」视图：判定卡、指标表、预算条、证据定位、实测区与 AI 汇总区
    status: completed
    dependencies:
      - add-api-routes
  - id: verify-and-record
    content: 用 [skill:playwright-cli] 端到端验证，清理临时文件并回读校验 .codebuddy/memory 落盘
    status: completed
    dependencies:
      - add-api-routes
      - build-analysis-view
---

