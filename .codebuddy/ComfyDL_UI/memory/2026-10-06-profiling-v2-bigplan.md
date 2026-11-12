# 2026-10-06 Profiling v2 大计划（正式版，bigplan 阶段产物）

> **阶段状态：大计划制作中（2026-10-06 开始）——待用户最后声明收工后结束本阶段。**
> 层级：idea（Profiling Tools）→ milestone（Profiling v2：安全图谱化 + 实测回流 + 可视化）→
> **本大计划** → 每 Phase 一个 smallplan（PLAN MODE 产出，届时才用任务列表工具）→ n 次 CRAFT。
> 执行方案已定：**方案三**，bigplan 派生 (PLAN + CRAFT)*n。
> 本文档由 3 个 code-explorer 前置调研（opgraph 离线化接口面 / run 期 FLOPs 挂点 /
> ComfyUI-Manager 分区范式）的结论汇总而成。

## 0. 关键事实修正（调研发现）

**`comfy/profiling/opgraph.py` 的 `FakeTensorMode` 只在第 53 行被 import，从未使用**——
docstring 宣称的"FakeTensor 探针（零真实内存）"与实现不符：`_probe_one` 一直是
**真实执行 + 真实张量**。这把 1MB corpus 爆炸事故从"设计取舍问题"定性为"文档与实现
不符的缺陷"，也意味着 P1 的离线采样工具复用的机制（FlopCounterMode + Recorder）本身
成立，缺的只是隔离层。

## 1. 战略依据（用户定调，存档）

1. 原始目标 = **等效计算图**（基础算子组成，为算子融合/性能调优准备），不是实测 FLOPs；
2. FLOPs 实测 = run 的附属产物（低开销），入 user database，校准 estimate + 历史参考；
3. 规则库路线（fake tensor 与真实执行都无用后），覆盖 ComfyUI 内置 + ComfyDL + D2L；
4. 成果呈现多样（mermaid / 等效 workflow / .vsdx / 图片 / 常用格式 / JSON / 文本），
   默认项兼顾直观与低计算量；
5. expand 模式利用左右维度做多平面分区（参考 ComfyUI-Manager）。

## 2. 架构决策记录（ADR，基于调研）

- **ADR-1 在线探针退役为显式危险模式**：在线 Analyze 默认不再执行任何节点代码；
  "真实执行"保留为 opt-in（设置项 + 二次确认），供规则库采样期人工使用。
- **ADR-2 规则库 = op census 的静态化**：规则条目由离线采样工具批量生成（复用
  `_bootstrap_registry`/`_probe_one`/`_ProbeRecorder`/`_unwrap_outputs`），schema 含
  `probes[input_signature + ops census]` + `shape_rule` + `flops_from_shape`
  （estimated / params_driven / unknown 三档）+ `source`。注册机制抄 `estimators.py` 的
  `@register` + 模块级 dict（现成可抄约 80%）。
- **ADR-3 在线组装器零执行**：新 `assemble_workflow(prompt)` =
  `engine._topological_order`（纯函数，`engine.py:83-98`）+ 规则查表 +
  `EstimationCtx` 风格 link/shape 解析（`estimators.py:157-184` 已是不执行版）+
  M2 estimator 的 flops_items 作 fallback。shape 覆盖：Cdl* 张量代数链精确、
  LM 链精确、Lazy 模块（Residual/PWFFN 等 param 推迟）= params_driven、
  无 estimator = unknown（诚实标注）。
- **ADR-4 run 期 FLOPs 归属走 contextvar**：挂点 = `execution.py:731 execute_async`
  入口进 / 出口退一个**进程级自定义 TorchDispatchMode（轻量档：只记 count+归属）**，
  **归属在每次 `__torch_dispatch__` 读 `get_executing_context()`
  （`comfy_execution/utils.py:16-46`，CurrentNodeContext contextvar）分桶**——
  不按 mode 生命周期归属（异步 task 交错会算错账）；V1/V3 无差异（两分支在
  `execution.py:292-307` 汇合）。单执行线程（prompt_worker）→ TLS mode 覆盖全部节点。
- **ADR-5 落库走现成 SQLAlchemy 栈**：新表 model + `alembic_db/versions/0007_*.py`
  迁移 + `app/database/db.py:238 create_session()`（worker 线程同步写）。
  口径声明：flops = fwd+bwd 合计；optimizer `_foreach_*`/fused 漏计（FlopCounterMode
  无 handler，记 0 不崩）；复合算子双计用 M3 双口径交叉验证法处理。
- **ADR-6 overlay 三列 flex 分区**（抄 Manager 范式）：外层 flex-wrap 三列
  （窄屏自动退化堆叠，兼容现纵向卡片）、`pf-` 前缀命名纪律、宿主 CSS 变量取色
  （`--fg-color`/`--comfy-input-bg`/`--border-color`）、单例 + display 切换（不销毁
  DOM）、右栏可做 flyover 侧抽屉、z-index 分层（遮罩 1000 / 面板 1099 / 内部 +1）。

## 3. Phase 任务分解

### P0 止血（规模：小；1 smallplan + 1-2 craft）

任务：
1. `profiling_routes.opgraph` 输入规模护栏：STRING/corpus 类输入 > 阈值（初值 64KB）
   的节点直接 fallback（`error=input too large for probe`），不进 executor；
2. 在线真实探针改为显式"危险模式"：请求带确认标志才执行，否则仅规则/缓存命中
   （规则库未上线前 = 直接返回提示性报告）；设置项 `ComfyDL.Profiling.DangerousProbe`；
3. 探针 executor 隔离与超时：独立单线程 executor + per-node / 总体 deadline，
   超时放弃 future 并标记（接受线程跑完才释放，文档说明）；
4. UI/文档同步 + 防回归测试（虚拟 1MB corpus 断言不再进 executor）。

验收：1MB corpus 场景 Analyze 秒回 fallback、estimate/refresh 不受影响、服务可正常
Ctrl+C。依赖：无（先行）。

### P1 等效计算图（规模：大；1 smallplan + 4-6 craft）

任务：
1. `comfy/profiling/oprules.py`：规则库 schema + `@register`/`OP_RULES` 机制
   （规则 JSON 落 `comfy/profiling/data/op_rules.json`，随仓库走）；
2. 离线采样 CLI（`comfy/profiling/sampler.py`，可挂 CI）：presets 生成
   （INPUT_TYPES 默认值 + 标准小张量工厂）→ 复用 `_probe_one` 采样 → 规则 JSON；
   人工复核清单输出（fallback/unknown 的 class_type 列表）；
3. 在线组装器 `assemble_workflow`（ADR-3），路由 `/comfydl/profiling/opgraph` 切换
   到它（保留旧探针端点仅供危险模式）；
4. 前端：分析结果默认 **mermaid 渲染**（数据形态 = 组装图 JSON）、覆盖率报表
   （covered / params_driven / unknown / missing 四档计数 + 明细）；
5. 导出器第一批：JSON、文本大纲、mermaid 文件（前端 blob 下载，零服务端计算）；
6. 测试：LM/tabular 两模板零执行出图断言、覆盖率断言、危险模式护栏断言。

验收：12 节点 LM 模板 Analyze <100ms 零执行出图；覆盖率报表可用；Analyze 不再可能
卡死服务（结构性保证）。依赖：P0（护栏兜底先行）。

### P2 实测回流（规模：中-大；1 smallplan + 3-5 craft）

任务：
1. `comfy/profiling/runmeter.py`：轻量计数 TorchDispatchMode（count-only 档 + 
   per-op census 档）+ contextvar 分桶 + OPS_LIMIT 保护；
2. `execute_async` 挂/卸（ADR-4），节点边界标记（可选）；
3. DB：`profiling_runs` + `profiling_node_stats` 两表 + alembic 0007 迁移 + 写入；
4. 面板历史卡（按 run 列表 → per-node flops/wall time）+ estimate 校准显示
   （measured vs formula 比例，滚动平均）；
5. 开销基准：1MB 与大训练负载下 <5% 断言 + 训练节点（inference_mode(False) +
   backward + optimizer.step）兼容性实测；
6. 口径文档（fwd+bwd 合计、optimizer 漏计边界、双计校验）。

验收：真实运行后历史卡出数、校准比例显示、开销达标。依赖：无硬依赖，可与 P1 并行
（建议 P1 收尾后做，避免同时动 Analyze 前端）。

### P3 可视化与布局（规模：中；1 smallplan + 3-4 craft）

任务：
1. overlay 三列分区改版（ADR-6：左 estimates / 中图可视化 / 右实测历史），
   窄屏退化；
2. mermaid 图交互增强（缩放/分区折叠）；
3. 导出器第二批：PNG/SVG（graphviz 依赖评估，否决则前端 canvas 截图降级）；
4. backlog 登记：等效 workflow（宏节点替换）、.vsdx。

验收：三分区布局可用、第二批导出全绿。依赖：P1（图数据）+ P2（历史数据）。

## 4. 风险汇总

| 风险 | 对策 |
|---|---|
| 采样 presets 难以覆盖怪节点（多输入/动态） | 复用冒烟器 `_VALUE_FACTORIES`/`_INPUT_OVERRIDES` 资产；采不到的入人工复核清单，规则库允许 missing |
| dispatch mode 每-op Python 开销在长训练下可观 | count-only 轻量档默认；census 档 opt-in；OPS_LIMIT |
| 异步节点 task 交错误归属 | contextvar 分桶（ADR-4），不按 mode 生命周期 |
| 节点内部自建线程的 torch 漏计 | 与 progress 归属同边界，文档标注 |
| mermaid 大图渲染性能 | 图节点数百级 + 分区折叠；必要时虚拟滚动 |
| DB 写入阻塞执行 | worker 线程同步写 + 小事务 |

## 5. 推进顺序与闸门

P0（立即）→ P1（核心）→ P2（可与 P1 尾部并行）→ P3。
每 Phase 一个 PLAN MODE smallplan（届时启用任务列表工具）+ n 次 CRAFT；
Phase 闸门 = 上述各自验收 + 双盘冒烟 0 FAIL + memory 入档。

## 6. 调研附录（subagent 结论索引）

- opgraph 离线化：`_probe_one`/`_ProbeRecorder`/`_unwrap_outputs`/`_bootstrap_registry`
  直接复用；`engine._topological_order`（engine.py:83-98）与 `EstimationCtx`
  （estimators.py:93-195）是与执行解耦的现成件；estimators 144+ 注册先例。
- run 期挂点：`execution.py:731 execute_async` 进出 mode；归属 =
  `comfy_execution/utils.py:16-46` contextvar；V1/V3 汇合于 execution.py:292-307；
  DB 走 SQLAlchemy+Alembic（app/database/db.py）。
- Manager 范式：三列 flex（cm-menu-container）、`pf-` 前缀、宿主 CSS 变量、
  单例 display 切换、flyover 侧抽屉、z-index 分层
  （ComfyUI-Manager/js/{comfyui-manager,custom-nodes-manager,common}.js）。
