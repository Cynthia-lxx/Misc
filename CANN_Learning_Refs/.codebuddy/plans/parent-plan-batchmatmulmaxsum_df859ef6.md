---
name: parent-plan-batchmatmulmaxsum
overview: 设计并落地一份"父计划"长期技术路线文档（比未来下层 PLANS.md 高一级的战略路线），目标是在 2026/10/7 前以"正确性优先+性能最大化"为终点、逐步稳定推进高质量实现 BatchMatmulMaxSum 融合算子赛题；主文档存 .codebuddy/plans/，并在 MEMORY.md/每日日志维护指针与进度快照。
todos:
  - id: survey-resources
    content: Use [subagent:code-explorer] to verify MatMul/Cube tutorial paths, adjacent plans template and knowledge index as resource map
    status: completed
  - id: write-parent-plan
    content: Draft PARENT_PLAN_batchmatmulmaxsum.md under .codebuddy/plans with P0-P5 stages, gates, risks and references
    status: completed
    dependencies:
      - survey-resources
  - id: sync-memory
    content: Add parent-plan pointer and status snapshot to MEMORY.md and daily log 2026-09-05.md
    status: completed
    dependencies:
      - write-parent-plan
---

## 用户需求

设计一条能**逐步稳定推进、最终高质量实现 BatchMatmulMaxSum 赛题目标**的长期技术路线，作为比 PLANS.md 更高一级的"父计划"长期战略文档，统领此后各次下层实施计划。前提：CANN_OpHelper 在**保留现有功能**的基础上，能力扩展以支撑该题；父计划只定战略路线与里程碑门禁，不写逐次实现细节、不写算子/工具代码。

## 需求澄清结果（用户已定夺）

1. **落地载体**：双份——主文档存 `.codebuddy/plans/`（当前为空），同时在 MEMORY.md 与当日日志维护指针与进度快照。
2. **能力预热**：把 Addcmul 一类 Vector 元素算子能力扩展（多 dtype、非对齐尾块等，与 BatchMatmulMaxSum 的 Vector 归约侧共享基础）**纳入为前导里程碑**，先爬坡再攻 Cube。
3. **终点标准**：正确性优先 + 性能最大化——全用例正确（fp32 双 1e-4、fp16/bf16 双 1e-3、与 PyTorch 语义对齐）+ 性能相对"拆分三算子"基线尽可能高。

## 硬约束（路线事实源）

- 题目全文唯一事实源：`p:\Dev\CANN_Learning_Refs\.codebuddy\memory\stage_goal_batchmatmul_maxsum.md`（用户要求一字不差存档），路线设计不得臆造题目语义。
- 内部期限 2026/10/7 前完成；官方在线答题 09/05–10/16、提交截止 10/17 18:00（17:30 封榜）。
- 实测工作区：`p:\Dev\CANN_Learning_Workspace\Projects\batchmatmulmaxsum_problem_1746_template\`（姊妹题 Addcmul 模板同区，可作 P1 载体）。
- 写任何 CANN 相关代码前必须先查官方本地文档（`Documentation_for_Developers\`、`CANN_Learning_Hub_(for_dev)\`），不臆造 API/范式。

## 总体策略

父计划文档是一份**战略路线 + 里程碑门禁**的长期资产，两条腿并行：① AscendC 官方范式/知识沉淀（本地文档 + CANN_Learning_Hub + 既有 ascendc_0X 记忆 + 衔接相邻工作区学习计划）；② CANN_OpHelper 工具能力扩展（保留既有 expr/verify 链路），全程以云端真机闭环验证为"事实仲裁"。路线按 **P0→P5 六阶段**推进，每阶段设可验证 Exit Criteria，未过不下放。

## 父计划文档结构蓝图（写入 .codebuddy/plans/PARENT_PLAN_batchmatmulmaxsum.md）

1. **文档头与定位声明**：文件名、创建日期、上级/下级关系（本文件 > PLANS.md/迭代计划）、维护规则（下层计划实现后回填状态；本文件只在阶段门禁/重大决策时修订）。
2. **北极星与验收定义**：内部期限 2026/10/7；终点=全用例正确（PyTorch 对标、FP64 golden→FP32、精度判据）+ 性能相对拆分基线最大化；官方提交截止 10/17 18:00。
3. **路线总图（mermaid 时间轴/阶段流）**：周粒度对齐 09/05–10/07，标注 P0–P5 起止与依赖、各阶段产物类型。
4. **阶段详案（每阶段含：目标 / 关键动作 / 产出 / Exit Criteria 门禁 / 决策点 / 回退路径）**：

- **P0 地基与语义基线**（约第 1 周前段）：研读官方 MatMul/Cube 教程与示例（CANN_Learning_Hub 及本地文档的进阶算子开发）；用 PyTorch 固化对标语义与 FP64 golden；建立覆盖 dtype×shape 域×transpose 四组合×非对齐尾块×全负行/边界值的用例矩阵；盘点可复用资源（衔接 ascendc 记忆与相邻学习计划）。门禁：语义基线文档 + 用例矩阵就绪，Cube 数据流（GM/L1/L0/UB）理解有记录。
- **P1 Vector 预热（Addcmul 为载体）**：fillgen/verifygen 的 dtype 参数化、非对齐尾块循环、tiling 多字段能力、verify 资产 dtype 化；以 Addcmul（或先共享子集：dtype+尾块+验证升级，广播按门禁决定）为试金石真机回归 TEST PASSED。门禁：既有 229 测试全绿 + 新能力真机闭环 + 回归无破坏（blast-radius 受控）。
- **P2 垂直切片（最小正确版）**：BatchMatmulMaxSum 固定最小场景（fp16、形状对齐、transpose 默认 false）打通 host/tiling/kernel/verify 真机闭环，一次跑对；建立 Cube 工程骨架与评测 runner 语义基线。**决策门**：记录"算子策略/骨架层 vs 表达式驱动扩展"的架构取舍依据。门禁：最小场景真机 TEST PASSED 且与 PyTorch 数值一致。
- **P3 全约束正确性**：四 transpose 存储布局、全维度域与非对齐尾块、bf16、K 维 FP32 累加、MaxSim 禁 0 初值（全负行正确）、输出有限性、多次执行一致性；对标用例矩阵全绿。门禁：用例矩阵全绿 + 云端提交演练零 161001 类环境坑。
- **P4 性能最大化**：Cube/Vector 协同流水（MatMul 搬运与 UB 内 MaxSim→Sum 两级归约重叠）、沿 B/M 多核分配与尾块负载均衡、Double Buffer/Ping-Pong、msprof 采集 Cube/Vector/MTE 利用率；建立拆分基线性能库并持续记录加速比。门禁：性能报告入档，加速比单调改进有据。
- **P5 收尾封板**：在线评测提交演练与封板（50 次/日限额内分批）、版本 tag、文档收口、整体复盘并把状态回填父计划。门禁：可复现提交版本 + 复盘结论落档。

5. **横切实践**：每里程碑 git tag（type(scope) 提交规范）；验证矩阵与性能基线入库；每日日志 + MEMORY 快照同步；每步可独立验证、避免超大重构；所有 CANN 代码先查官方范式。
6. **风险与缓解**：工具抽象扩展过度（以决策门与"先共享子集"约束范围）；云端队列与提交限额（本地充分验证后批量提交）；评测环境差异（soc 161001/ascend910_93 别名坑已有 run_verify.sh 内置 workaround，P3 演练前置）；与课程学习计划时间争抢（P0 将学习与地基合并排程）；Addcmul 广播范围蔓延（门禁限定，非共享部分后置）。
7. **引用文件索引**：stage_goal 全文、MEMORY.md、按日日志、Addcmul 差距评估、相邻工作区学习计划/记忆（只读引用）、官方文档与示例路径。

## 记忆双写（与文档同时完成）

- MEMORY.md「当前阶段目标」小节：追加"父计划已建档"与文档指针、一句话状态（P0 未启动）。
- 当日日志 `2026-09-05.md`：记录建档动作与文件位置。

## 性能与可靠性说明

- 本任务产物为战略文档，无运行时性能问题；可靠性体现在：以 stage_goal 文件为唯一事实源、资源路径全部核实后写入、门禁可验证、状态可回填。
- 文档采用中英双语语言包惯例不适用（非 CLI 文案），正文以中文为主、文件内路径/术语保留英文原名，符合项目双语偏好。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose：在撰写父计划前盘点资源地图——定位 CANN_Learning_Hub 中 MatMul/Cube 官方教程与示例、Documentation_for_Developers 相关子目录、相邻工作区学习计划模板与 ascendc 知识索引，核实每个将被写入"引用文件索引"的路径真实存在。
- Expected outcome：一份核实的资源清单（路径+一句话用途），直接支撑父计划文档中 P0 资源与"引用文件索引"章节，避免文档写入臆造/失效路径。