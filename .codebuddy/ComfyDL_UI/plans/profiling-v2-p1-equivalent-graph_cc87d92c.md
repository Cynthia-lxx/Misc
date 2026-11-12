---
name: profiling-v2-p1-equivalent-graph
overview: Profiling v2 P1 等效计算图（核心 Phase）：建立 op census 规则库（oprules.py + 规则 JSON），离线采样工具批量生成规则（复用现有探针件，护栏内），在线 Analyze 重写为零执行的纯查表组装器 assemble_workflow（危险头才走旧探针），前端默认 mermaid 渲染等效计算图 + 覆盖率四档报表 + 第一批导出器（JSON/文本/mermaid 文件）。验收：12 节点 LM 模板 Analyze 零节点执行出图且快速、覆盖率报表诚实标注、双盘冒烟 0 FAIL。计划结构：开头前置阅读（docs/memory/代码）、结尾记忆文件写入；执行守则：context 过大先 compact + 写 memory 再继续；规模大，todolist 依赖序排列、支持跨 CRAFT 会话续跑。
todos:
  - id: p1-preread
    content: 前置阅读：bigplan/M1-M3 docs/estimators+shapes+engine 现状/profiler.js census 区；用 [subagent:code-explorer] 评估 mermaid 渲染途径与 EstValue 接口
    status: completed
  - id: p1-schema
    content: 实现 comfy/profiling/oprules.py：规则 schema+OP_RULES 注册+规则 JSON 加载+覆盖率查询；单测入 test_profiling_opgraph
    status: completed
    dependencies:
      - p1-preread
  - id: p1-sampler
    content: 实现 comfy/profiling/sampler.py：presets 工厂+sample_op_census+护栏内采样生成 data/op_rules.json 与人工复核清单
    status: completed
    dependencies:
      - p1-schema
  - id: p1-review
    content: 审阅采样结果：手工补 manual 条目、固化首版规则 JSON 与覆盖率基线（文档记录）
    status: completed
    dependencies:
      - p1-sampler
  - id: p1-assembler
    content: 实现 assemble_workflow 零执行组装器（拓扑+查表+shape 解析+M2 fallback+coverage 字段）；路由无头路径切换，危险头保留旧探针
    status: completed
    dependencies:
      - p1-review
  - id: p1-frontend
    content: 前端 assembled 适配：rule 状态 census 展示+覆盖率报表卡+模式徽标+en/zh i18n
    status: completed
    dependencies:
      - p1-assembler
  - id: p1-mermaid
    content: mermaid 语法生成+渲染途径落地或降级+JSON/文本/mermaid 导出器
    status: completed
    dependencies:
      - p1-assembler
  - id: p1-tests
    content: 测试：双模板零执行出图断言+覆盖率断言+路由集成+T4 静态；双盘全量冒烟 0 FAIL
    status: completed
    dependencies:
      - p1-frontend
      - p1-mermaid
  - id: p1-ship
    content: 提交推送（rebase 惯例）+ F 盘 fetch/reset/submodule 同步复验
    status: completed
    dependencies:
      - p1-tests
  - id: p1-memory
    content: memory 写入：P1 交付记录入 2026-10-06.md（或续篇）+ MEMORY.md 阶段声明更新为 P1 完成
    status: completed
    dependencies:
      - p1-ship
---

## P2 计划：实测回流（任务清单补全版）

前 7 项不变（p2-preread / p2-runmeter / p2-hook / p2-db / p2-api / p2-frontend / p2-bench），补全与追加：

- **p2-bench（完整版）**：开销基准 = LM 模板与大训练负载在 count 档下的开关对比（<5% 断言）+ 训练节点兼容性实测（inference_mode(False) + backward + optimizer.step 与 dispatch mode 组合不炸、flops 记账符合 fwd+bwd 合计口径）。
- **p2-startup（新增）**：真启动验证（坑 25 强化版）——`--cpu` 启动后 Queue 一个小工作流（如 tabular 模板），断言 profiling_runs/profiling_node_stats 落库 + /history 端点返回该 run；验证 `--cdl-profiling-record off` 时零写入。
- **p2-docs（新增）**：口径文档写入 docs/profiling-m2-compute-and-watchdog.md（或新 §）——fwd+bwd 合计、optimizer fused 漏计边界、复合算子双计与双口径交叉验证、节点内自建线程漏计边界、count/census 两档语义、开关说明。
- **p2-ship（新增）**：提交推送（rebase 惯例，父仓库）+ F 盘 fetch/reset/submodule 同步 + 双盘全量冒烟 0 FAIL。
- **p2-memory（新增，收尾）**：P2 交付记录写入 2026-10-06.md（或续篇 §）+ MEMORY.md 阶段声明更新为 P2 完成；随后剩余 Phase 为 P3（布局与导出矩阵第二批）。

依赖：p2-bench 依赖 p2-hook；p2-startup 依赖 p2-db+p2-api；p2-docs 依赖 p2-bench；p2-ship 依赖 p2-frontend/p2-docs/p2-startup；p2-memory 最后。