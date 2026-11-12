---
name: profiling-v2-p2-measured-flops
overview: Profiling v2 P2 实测回流（共 11 项任务，如下 todolist 必须完整实现）：用户真实运行工作流时以低开销记录 per-node FLOPs（进程级轻量 TorchDispatchMode + CurrentNodeContext contextvar 归属，ADR-4），写入 user database（SQLAlchemy + Alembic 0007 两表，ADR-5），面板新增实测历史卡与 estimate 校准显示（measured vs formula），配套开销基准（<5%）与训练节点兼容性实测、真启动 Queue 落库验证、口径文档、双盘提交同步、memory 收尾。todolist 严格按以下 11 项：(1) p2-preread 前置阅读含 subagent 复核 contextvar 异步传播/alembic 范式；(2) p2-runmeter 实现 comfy/profiling/runmeter.py（RunMeter TorchDispatchMode count/census 双档 + contextvar 分桶 + op 上限降级 + snapshot/reset）+ 单测；(3) p2-hook cli_args 加 --cdl-profiling-record {off,count,census} 默认 count + execution.py:731 execute_async try/finally 挂卸；(4) p2-db 两表 model + alembic 0007 + 落库函数（try/except + proflog，绝不打断执行）+ DB 单测（临时 sqlite）；(5) p2-api /comfydl/profiling/history 端点 + estimate 报告合并 measured_flops/ratio 字段 + 路由集成测试；(6) p2-frontend 实测历史卡 + per-node"实测 vs 公式"行 + en/zh i18n + T4 静态断言；(7) p2-bench 开销基准（LM 模板与大训练负载 count 档开关对比 <5%）+ 训练节点兼容性实测（inference_mode(False)+backward+optimizer.step 组合）；(8) p2-startup 真启动验证（坑25强化：--cpu 启动 + Queue tabular 模板 + 断言两表落库 + /history 返回 + --cdl-profiling-record off 零写入）；(9) p2-docs 口径文档（fwd+bwd 合计/optimizer fused 漏计/复合算子双计交叉验证/自建线程漏计边界/两档语义/开关）；(10) p2-ship 提交推送（rebase 惯例）+ F 盘同步 + 双盘全量冒烟；(11) p2-memory 交付记录 + MEMORY.md 阶段声明更新为 P2 完成。计划结构：开头前置阅读、结尾 memory 写入；执行守则：context 过大先 compact + 写 memory 再继续。
todos:
  - id: p2-preread
    content: 前置阅读：bigplan/P2 分解 + execution.py execute_async/comfy_execution utils/app.database db+models/alembic 0006 现状核对；用 [subagent:code-explorer] 复核 CurrentNodeContext 异步 task 传播语义、alembic 0006 范式、estimate 路由合并点
    status: completed
  - id: p2-runmeter
    content: 实现 comfy/profiling/runmeter.py：RunMeter(TorchDispatchMode) count/census 双档 + contextvar 分桶 + op 上限降级 + snapshot/reset；单测入新文件 test_profiling_runmeter.py
    status: completed
    dependencies:
      - p2-preread
  - id: p2-hook
    content: cli_args 加 --cdl-profiling-record {off,count,census} 默认 count；execution.py execute_async try/finally 挂/卸（courtesy 规则全包）+ 真启动冒烟验证挂载不崩
    status: completed
    dependencies:
      - p2-runmeter
  - id: p2-db
    content: app/database models 加 profiling_runs/profiling_node_stats 两表 + alembic 0007 迁移 + prompt 结束落库函数（try/except + proflog）+ DB 单测（临时 sqlite）
    status: completed
    dependencies:
      - p2-runmeter
  - id: p2-api
    content: GET /comfydl/profiling/history 端点 + estimate 报告合并 measured_flops/ratio 字段；路由集成测试
    status: completed
    dependencies:
      - p2-db
  - id: p2-frontend
    content: 前端：实测历史卡（最近 runs + 展开 per-node 明细）+ per-node "实测 vs 公式"行 + en/zh i18n；test_profiling_log.py T4 静态断言
    status: completed
    dependencies:
      - p2-api
  - id: p2-bench
    content: 开销基准：LM 模板与大训练负载 count 档开关对比（<5% 断言）+ 训练节点兼容性实测（inference_mode(False)+backward+optimizer.step 组合不炸、flops 符合 fwd+bwd 口径）
    status: completed
    dependencies:
      - p2-hook
  - id: p2-startup
    content: 真启动验证（坑25强化）：--cpu 启动 → Queue tabular 模板 → 断言 profiling_runs/profiling_node_stats 落库 + /history 返回该 run → --cdl-profiling-record off 时零写入
    status: completed
    dependencies:
      - p2-db
      - p2-api
  - id: p2-docs
    content: 口径文档：fwd+bwd 合计 / optimizer fused 漏计 / 复合算子双计与双口径交叉验证 / 自建线程漏计边界 / count-census 两档语义 / CLI 开关说明
    status: completed
    dependencies:
      - p2-bench
  - id: p2-ship
    content: 双盘全量冒烟 0 FAIL + 提交推送（rebase 惯例）+ F 盘 fetch/reset/submodule 同步复验
    status: completed
    dependencies:
      - p2-frontend
      - p2-docs
      - p2-startup
  - id: p2-memory
    content: memory 写入：P2 交付记录入 2026-10-06.md（或续篇）+ MEMORY.md 阶段声明更新为 P2 完成（剩余 P3）
    status: completed
    dependencies:
      - p2-ship
---

## Profiling v2 P2：实测回流（FLOPs 成为 run 的附属产物）

用户真实运行工作流时，以低开销记录每次运行 per-node 的实测 FLOPs，写入 user database 供两类用途：①纠正 estimate（measured vs formula 滚动校准比例）；②为用户提供历史记录参考。同时明确报告口径边界（fwd+bwd 合计、optimizer 漏计、复合算子双计）。

## 核心功能

- **run 期计数器**：进程级轻量 TorchDispatchMode（默认 count-only 档，census 档 opt-in），每次算子调用按 CurrentNodeContext contextvar 归属到当前执行节点；单节点 op 上限保护；DB/计数失败绝不打断执行。
- **执行路径挂载**：`execute_async` 入口挂载、出口卸载并汇总；CLI 开关 `--cdl-profiling-record {off,count,census}` 默认 count。
- **持久化**：`profiling_runs` + `profiling_node_stats` 两表（SQLAlchemy model + Alembic 0007 迁移），run 结束同步落库。
- **查询与校准**：`GET /comfydl/profiling/history`（最近 runs + per-node 明细）；estimate 报告为有历史实测的节点附 `measured_flops` 与 `ratio` 字段。
- **前端**：实测历史卡（最近 runs 列表 + 展开 per-node 明细）+ per-node "实测 vs 公式"行（en/zh）。
- **质量关**：开销基准 <5%（LM 模板与大训练负载）、训练节点（inference_mode(False)+backward+optimizer.step）兼容性实测、真启动 + 实际 Queue 落库验证、口径文档。

## Tech Stack

- 后端：Python 3.12 兼容语法（双盘规则）、torch TorchDispatchMode（TLS 语义）、SQLAlchemy 2.0 + Alembic（项目既有栈）、aiohttp 路由。
- 前端：profiler.js 原生 JS（cdlp- 前缀 + 宿主 CSS 变量，P0/P1 既有范式），无新增依赖。
- 测试：cdl_smoke_tests 既有 check() 范式 + aiohttp TestServer + 临时 sqlite；真启动 + Queue 落库验证。

## Architecture

### 挂载与归属（ADR-4）

```mermaid
flowchart LR
    A["execute_async 入口"] --> B["RunMeter.__enter__ (count/census 档)"]
    B --> C["节点执行 (V1/V3 汇合于 CurrentNodeContext)"]
    C -->|"每次 __torch_dispatch__"| D["读 get_executing_context() 按 node_id 分桶"]
    D --> C
    C --> E["prompt 结束 __exit__ 汇总"]
    E --> F["SQLite 两表落库 (worker 线程, try/except)"]
    F --> G["GET /history + estimate 校准字段"]
```

### Module Division

- `comfy/profiling/runmeter.py`：RunMeter（计数模式：per-node {flops_total, op_count}；census 模式：另存 per-op 计数，超 OPS_LIMIT 降级为 count-only）；`snapshot()` 返回 {node_id: {...}}；contextvar 缺失的算子计入 "unattributed" 桶。
- `execution.py`：execute_async try/finally 挂/卸（courtesy 规则：任何异常吞掉 + proflog）。
- `app/database/models.py` + `alembic 0007`：两表。
- `app/profiling_routes.py`：/history 端点 + estimate 报告合并 measured 字段。
- `profiler.js`：历史卡 + 校准行 + i18n。

### Data Flow

Queue 运行 → dispatch 记账按 contextvar 分桶 → prompt 结束汇总 → create_session 同步写两表 → 前端历史卡/校准显示。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose: p2-preread 中复核 CurrentNodeContext contextvar 在异步 task 交错下的传播语义、alembic 0006 迁移文件范式、app/database/models.py 表定义范式与 estimate 路由合并点
- Expected outcome: 精确的挂载点行号、迁移文件模板、表定义模板与 estimate 合并接口清单