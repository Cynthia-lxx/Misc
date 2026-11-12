---
name: crashsite-m2-explainability
overview: 为 Crash site 增加恢复可解释性：快照卡片展示逐节点捕获清单，resume 后在新快照上写"跳过/重跑"权威统计（经宿主缓存命中事件精确计数），并让前端 toast 与卡片呈现这些信息。
todos:
  - id: backend-cache-hit-counting
    content: 在 cache_provider.py 加 notify_node_cached 分派、execution.py cached 分支接入，provider 加 on_node_cached/mark_resume，store.py 加 set_note/captured_nodes 聚合
    status: completed
  - id: provider-resume-note
    content: provider 在 on_prompt_end/on_prompt_error 收尾时为 resume 运行写"跳过/重跑"统计 note
    status: completed
    dependencies:
      - backend-cache-hit-counting
  - id: routes-resume-summary
    content: routes.py：resume 调 mark_resume、响应携带源快照捕获摘要，snapshots 列表附 captured 明细
    status: completed
    dependencies:
      - backend-cache-hit-counting
  - id: frontend-explainability
    content: crashsite.js/css：卡片 captured 明细（截断+双语）、toast 文案升级、execution_success/error 自动刷新
    status: completed
    dependencies:
      - routes-resume-summary
  - id: tests-smoke
    content: test_crashsite.py 补 P7/P8/R8 断言，跑 crashsite 专项与全量 CPU 冒烟（基线 301/33/0）
    status: completed
    dependencies:
      - provider-resume-note
      - routes-resume-summary
      - frontend-explainability
  - id: docs-commit
    content: 更新 docs/crashsite.md，P 盘 master 提交（feat(crashsite)），待验收后 F 盘同步跑 GPU 冒烟
    status: completed
    dependencies:
      - tests-smoke
---

## Product Overview

为 ComfyDL Crash site（事故现场）补齐 M2"可解释性"能力：让用户在打断/恢复后能明确知道**哪些节点被快照捕获、恢复时哪些节点被缓存跳过、哪些真正重跑**，消除"resume 像被砍半截"这类误读。

## Core Features

- 快照卡片显示逐节点捕获清单（node_id + class_type + 体积，超 4 个折叠为 "+n more"）
- 恢复运行结束后，在新快照卡片上写入权威统计 note：`resume of <源快照>: 跳过 X/Y（快照 rehydrate a + 本地缓存 b），重跑 c`，成功与打断两种收尾都写
- resume 通知（toast）文案升级：说明"结束后在新快照卡片查看跳过/重跑统计"
- 快照库在每次执行结束（成功/失败）后自动刷新，无需手动点 Refresh
- resume 响应携带源快照的捕获摘要（捕获节点数），供前端即时反馈

## 边界

- 不改变 M1 恢复语义（节点级记忆化），不做训练节点内 checkpoint（M3 范围）
- 统计只基于宿主缓存命中这一权威信号，不做估算

## Tech Stack

- 后端：Python 3.14（兼容 3.12 语法），sqlite3（既有 store.py），aiohttp RouteTableDef（既有路由模式）
- 前端：原生 JS/CSS 扩展（既有 crashsite.js/css 模式，en/zh 双语 I18N，host CSS 变量）
- 测试：cdl_smoke_tests/test_crashsite.py 既有框架（S/P/R 检查 + aiohttp TestServer）

## Implementation Approach

核心决策：**“跳过数”的唯一权威来源是 execution.py `execute()` 的 cached 分支**（约 446-451 行，本地 RAM 命中与快照 rehydrate 都汇聚于此），而不是 provider.on_lookup 计数——同进程 resume 时本地命中不经过 on_lookup，用后者会把缓存跳过错算成重跑。做法沿用项目既有先例（`_notify_prompt_lifecycle` 的 getattr 容错分派）：在 `comfy_execution/cache_provider.py` 新增 `notify_node_cached()` 分派 helper，execution.py cached 分支一行调用（`_has_cache_providers()` 门控，无 provider 时零开销）；`SnapshotProvider.on_node_cached()` 按活动 prompt 累计去重节点数。

resume 标记链路：routes.resume 入队后调用 `provider.mark_resume(prompt_id, source_snapshot_id)`（执行串行，单指针安全，与 on_store 同一并发模型）；`on_prompt_start` 消费该标记；`on_prompt_end/on_prompt_error` 时若为 resume 运行，则向**新**快照写 note：节点总数取 manifest prompt 的 `len(prompt)`，重跑数 = 总数 − 跳过数。

快照明细：store.py 新增按 node_id 聚合的查询（node_outputs 表已有 node_id/class_type/size_bytes，GROUP BY 即可，O(节点数) 无瓶颈），`list_snapshots` 附带 `captured` 数组（库容量小，N 次小查询可接受）；`set_note` 写 note 字段（cs-card 已渲染 s.note，前端零结构改动即可显示）。

## Implementation Notes

- execution.py 改动保持一行调用 + 模块级 helper，避免侵入执行热路径；notify 在 cached 分支 `_send_cached_ui` 之后，与既有 `_send_cached_ui` 的 client_id 门控无关（计数不依赖 websocket）
- on_node_cached 只按 prompt_id 匹配活动快照时计数，杂散事件忽略（与 on_prompt_error 同模式）
- 前端自动刷新用 `api.addEventListener("execution_success"/"execution_error", ...)`，回调内 fetchSnapshots 需幂等（state.loading 已有防抖）
- 快照卡 captured 明细截断展示（最多 4 条 + "+n more"），防长列表撑爆侧栏
- 不碰 FUNCTIONS.md/README 节点计数（crashsite 非节点）

## Architecture Design

```mermaid
graph LR
    A[execution.py execute() cached 分支] -->|notify_node_cached| B[cache_provider.py 分派]
    B -->|on_node_cached| C[SnapshotProvider 计数器]
    D[routes.resume] -->|mark_resume| C
    C -->|on_prompt_end/error 写 note + 统计| E[(snapshot .db)]
    E -->|list_snapshots + captured 明细| F[crashsite.js 卡片/自动刷新]
```

## Directory Structure

```
ComfyDL_UI/
├── execution.py                          # [MODIFY] cached 分支加一行 notify_node_cached 调用
├── comfy_execution/cache_provider.py     # [MODIFY] 新增 notify_node_cached() 分派 helper（getattr 容错）
├── app/crashsite/provider.py             # [MODIFY] on_node_cached 计数、mark_resume、收尾写 note
├── app/crashsite/store.py                # [MODIFY] set_note()、captured_nodes() 聚合查询、list_snapshots 附 captured
├── app/crashsite/routes.py               # [MODIFY] resume 调 mark_resume；响应带源快照捕获摘要
├── app/crashsite_assets/crashsite.js     # [MODIFY] 卡片 captured 明细（折叠）、toast 文案、执行结束自动刷新
├── app/crashsite_assets/crashsite.css    # [MODIFY] captured 明细行样式（小字号、截断）
├── cdl_smoke_tests/test_crashsite.py     # [MODIFY] 新增 P7（on_node_cached 计数）/P8（resume note）/R8+（列表明细、resume 摘要）断言
└── docs/crashsite.md                     # [MODIFY] 补 M2 可解释性说明
```