---
name: p2b-real-shape-slice
overview: P2b 计划修订版：P2b 核心代码已落地（真 M/N=64 cube 切片 + bmm_reduce 归约核，commit 8984474），现并入重大新资产——cannbot-skills 矿藏的知识/工具复用（507015 权威排错、Matmul 高阶 API 骨架、平台 tiling 路由、sync-audit、direct-invoke 工程样板），并据其把云验决策树与后续 P2b/P2 收尾修订为「先吸收、再验证、按证据决策」。
todos:
  - id: asset-register
    content: 将 cannbot-skills 探明资产（507015 排查、api-matmul 骨架、平台路由、add_custom 样板、sync-audit、许可裁决）登记入长期记忆与资产索引
    status: completed
  - id: kernel-audit
    content: 用 [subagent:code-explorer] 对照 api-matmul.md 骨架核验 bmm_batch_cube 差异，kernel.asc 补引用注释或必要补丁并另提交
    status: completed
    dependencies:
      - asset-register
  - id: push-retry
    content: 网络恢复后重试推送 fix/p2b-cube-real-shape，失败则交付用户手动 push 命令与云验同步指引
    status: completed
    dependencies:
      - kernel-audit
  - id: cloud-verify-p2b
    content: 给出含 507015 排查 checklist 的云端 run.sh 验证决策树并执行验证，按结果定案或走 vector 兜底决策
    status: completed
    dependencies:
      - push-retry
  - id: record-conclusion
    content: 将 P2b 云验结论与父计划状态更新写入 2026-09-06 记忆与 MEMORY.md
    status: completed
    dependencies:
      - cloud-verify-p2b
---

## 需求概述

P2b 阶段计划修订：在原有「真 M/N Cube 垂直切片」目标（B=1, M=N=K=64，fp16×fp16→fp32，云端 `bash run.sh` 达 `=== PASSED ===`，代码改动已 commit `8984474` 于本地分支 `fix/p2b-cube-real-shape`）基础上，**吸收 cannbot-skills 矿藏探明的知识资产**，用官方权威依据武装 507015 排错与 kernel 骨架核验，并将父计划修订落于长期记忆。

新增要点（源自对 `cannbot-skills-master` 的只读深挖，全部附精确路径）：

- **507015 权威解析入库**：`ops/ascendc-api-best-practices/references/api-matmul.md:339-343`（TilingHeader 误放 L1/UB + GetPhyAddr 喂 Init → MPU address invalid 507015；修复=stack-resident + LoadTilingFromGM）、同文件 :262-266 与 :357-364 检查清单（AIC 守卫/MM_CFG_NO_PRELOAD/IterateAll/SetHF32）；`pypto-op-knowledge/.../experience_classified/machine.md:172-207`（507015=CCU instruction address check，根因含 cube tile kL0>实际 K 越界，K=576 vs kL0=768 实例）。
- **平台路由裁决**：`ops/ascendc-tiling-design/references/matmul/patterns.md:9-12` 确认 910B(DAV_2201)→AscendC::Matmul 高阶 API，仓库内 matmul_blaze/catlass 样例属 Ascend950(DAV_3510) 谱系**不可直接引代码**（仅方法论可参考）。
- **direct-invoke 工程样板对照**：`ops/ascendc-direct-invoke-template/references/add_custom/`（host/main 直调 + 共用 tiling struct + run.sh 护栏 + verify_result 统计）。
- **可执行工具**：`ops/ascendc-sync-audit/scripts/`（静态 Set/Wait 配对扫描，可在后续 vector/cube 融合轮直接运行）；`tools/msaicerr-toolkit/scripts/preflight.sh`（CANN 环境检查 bash 模板）。
- **许可健全性**：LICENSE=CANN OSL v2.0（限华为 AI 处理器系统内使用/修改/分发、保留声明）；本工程属许可用途，复制需保留版权与协议声明。

## 范围边界（本轮明确不做）

- 不把 950/Blaze/Catlass/TileLang 代码引入工程；不做 CANN_OpHelper 工具回流；不做多 case 数据链、transpose/bf16、多核与性能、C 中间量融合（P4）、M/N>64 与 K≡8(mod16)。
- 父计划 artifact 不在本 IDE plans 存储（本机仅 `5a9572`=P2b 与 `709cf33a`=官方文档精读两文件），故**父计划修订以长期记忆为落点**（MEMORY.md「当前阶段目标」段 + 新增资产索引），并在本计划文档注明该事实与更新口径。

## 验收标准

1. 探明的 S/A 级资产以「精确路径 + 一句话价值 + 许可/平台限制」登记入长期记忆与资产索引，可回查。
2. kernel.asc 完成对照 api-matmul.md §7 骨架的静态核验（AIC 守卫语义/按值传 TCubeTiling 是否需 LoadTilingFromGM 形态/MM_CFG_NO_PRELOAD 等），需改则补丁并新增 commit。
3. `fix/p2b-cube-real-shape` 推送成功（网络恢复后重试；仍失败则交付用户手动命令）。
4. 云端 `bash run.sh` 验证走升级决策树：PASSED → 记录探针命中号并定案 cube 真 shape 可用；若崩 507015 → 先按资产 A 清单排查再决定二轮 vector 全 shape 兜底保 PASSED。
5. 结果如实写入 `2026-09-06.md` 与 MEMORY.md（含父计划状态更新）。

## 技术栈（延续，无新依赖）

- 实测区 `P:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\`（Ascend C，bisheng/CANN 9.0.0 云端；本地不编译），git safe.directory 前缀照旧。
- API 权威：`cann_ascendc_headers_9.0.0`（不改、只读）。
- 资产只读根：`p:\Dev\CANN_Learning_Refs\cannbot-skills-master\cannbot-skills-master\`。
- 记忆根：`p:\Dev\CANN_Learning_Refs\.codebuddy\memory\`。

## 修订后总体策略

原「CUBE→REDUCE 链 + 云端结果仲裁」主线不变；本次修订把「矿藏资产吸收」作为独立前置阶段（不阻塞已完成的代码改动），并把云验决策树从「崩了直接兜底」升级为「崩了先按权威清单排查、再决定兜底」。执行次序：资产登记 → kernel 骨架核验（可选补丁）→ push → 云验 → 结论归档。

### 资产登记落点

- 新增记忆资产索引文件（或并入 MEMORY.md「重大复用资产」节）：逐条记录 S/A/B 级资产路径与用途、507015 排查 checklist、平台路由裁决、CANN OSL v2.0 许可约束、910B vs 950 边界、sync-audit/msaicerr preflight 工具入口。
- kernel.asc 文件头注释增补「507015 已核实根因与对策」引用块（标注出处路径），供后续轮与云端排错直接调用。

### Kernel 骨架核验（对照 api-matmul.md §7）

- 静态核对我们 `bmm_batch_cube` 的形态：TCubeTiling 按值传参进 kernel（非 GM TilingHeader + GetPhyAddr 反模式），确认是否需补 `if ASCEND_IS_AIV return` 守卫语义、`SetHF32(false,0)`、`PipeBarrier<PIPE_AIC>` 收尾等与 9.0.0 头库一致的写法；发现实质差异才落补丁（另 commit），无差异则仅补引用注释。
- bmm_reduce 已按头库核实的 `Max(dst,src0,src1,count=1)` 恒等 running-max 路线实现（无 Maximum/VerifyReduceMax），本轮不重写，仅登记资产引用。

### 云验决策树（升级版）

1. 日志含 `run path: CUBE->REDUCE (B=1 M=64 N=64 K=64)` 且 `=== PASSED ===` → P2b 门禁达成，记录 tiling 探针命中号，cube 真 shape 可用性定案。
2. 若崩 `ret=507015` 或 tiling 全 FAILED → 先按已登记清单排查：api-matmul.md:339（header 装载形态）→ machine.md:172-207（kL0/K 越界、K=1 禁 cube、算法缺步骤）→ patterns.md:9-12（平台路由复核 910B）→ 按结论二轮决定（修正 tiling 参数重测 或 补 vector 全 shape 伪 MatMul+归约兜底保 PASSED，cube 议题按证据归档）。

## 健全性约束

- 资产只读纪律：只读审视、确认许可前不整目录复制入 git；复制片段须保留版权与协议声明。
- 平台路由纪律：910B 只参照 AscendC::Matmul 高阶 API 文档，不搬 950/Blaze 代码。
- 版本纪律：任何从资产引入的 API 用法均须与 `cann_ascendc_headers_9.0.0` 复核一致后方可入注释/代码。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose: 执行阶段 2（kernel 骨架核验）时，定向复核 `api-matmul.md` §7 骨架、`ascendc-api-matmul-tiling.md`、`ascendc-sync-audit/scripts/` 与头库的对应实现，给出「我们的 bmm_batch_cube 与官方最小骨架的差异清单与是否需要补丁」的结论；如需要，辅助确认 507015 排查 checklist 的引用行号准确性。
- Expected outcome: 差异清单（含是否补 AIC 守卫/SetHF32/PipeBarrier 等）与逐条证据路径；kernel.asc 补丁范围被精确限定，杜绝臆造与过度改动。