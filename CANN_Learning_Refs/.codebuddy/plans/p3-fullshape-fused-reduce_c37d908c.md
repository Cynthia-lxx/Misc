---
name: p3-fullshape-fused-reduce
overview: 把 P2b 单点切片（fp16、B=1、M=N=K=64、C 整幅落 GM）推进到 P3 第一垂直切片：全维度域（B∈1..64、M/N∈1..8192 含非 16 倍尾块、K∈32..8192 且 8 倍）+ 融合归约架构（cube 产出行段 C 即核内行归约收缩，C 不再整幅写 GM），在真机上以数个大 shape/尾块 case 对 golden 全 PASSED，并回填父计划。
---

已完成全部所需探索：P2b 定案工程结构、官方融合范式落点（light 03.05 `matmul_abs.asc`：`Matmul` C 出 `VECIN` + `GetTensorC<true>` + `Iterate<true>` 循环逐 tile 取 C；05.04 `matmul_abs`）、父计划 P3 原文、用例矩阵与 golden 资产、cannbot-skills 高价值参考均已核实。现在生成计划。

<plan_result>
<req>

## 用户需求

2026 CANN 上合赛 BatchMatmulMaxSum 赛题 P3 阶段第一个垂直切片：**把实测 kernel 从 P2b 的「B=1、M=N=K=64、fp16、单点」扩展到题目全域形状，并将归约前移为融合形态，根治 C=B×M×N×4B 中间量在域边缘（如 B=64、M=N=8192、K=32 时约 17GB）爆内存的问题**。

## 产品概述

- 语义固定：`y[b]=Σ_{m=0..M-1} max_{n=0..N-1} Σ_k x1[b,m,k]·x2[b,k,n]`，先 MatMul 再每行 max(N) 后跨行 sum(M)，顺序不可交换；无 batch broadcast。
- 目标形状域：B∈[1,64]、M/N∈[1,8192]、K∈[32,8192] 且为 8 的整数倍、B×M×K 与 B×N×K 均 ≤2^28；M/N 非 16 倍时须正确处理尾块。
- dtype：输入恒 fp16（bf16 留后续子切片）；输出 y 恒 fp32；K 点积与归约 fp32 累加；不修改输入。
- golden 基准：PyTorch `bmm→amax(-1)→sum(-1,dtype=fp32)`，用 FP64 计算转 FP32；判据为相对/绝对误差双 1e-3。

## 核心功能（本切片）

1. **融合归约 kernel**：cube 每 tile 产出的 C 直接经 `GetTensorC` 进入 UB，核内按行块做「行级 max over N → 跨行 sum over M」收缩，C 不再整幅落 GM；每 batch 只写出 B 个标量（或受限小 C）。
2. **全域形状支持**：M/N 任意（含 16 倍尾块）、K 8 倍（含 K%16≠0 的 8 尾段）、B 1..64；保留 P2b 的 vector dot 兜底路径与单点保护。
3. **多 case 数据链**：main/gen/verify/run.sh 支持约 4~6 个域内代表性 case（含大 shape、B>1、K 8 倍非 16 倍、M/N 尾块），真机逐 case 对 golden 全 PASSED。
4. **回归保护**：P2b 锚点（B=1、M=N=K=64）与 M==N==1 点积路径不回归；三官方示例语义由 golden 单测背书。

## 非目标（后续子切片）

transpose 四布局、bf16、多核/流水性能优化（P4）、云端正式提交演练（P5）。
</req>

<tech>

## 技术栈

- Ascend C（Direct Invocation 形态：`main.asc` include `kernel.asc`、`extern "C" run_kernel`、`<<<>>>` 直启 `__cube__`/`__vector__`/`__mix__`），910B 系（`__NPU_ARCH__==2201`，云端 socVersion 161001 / ascend910_93）。
- Python 3（numpy）作数据/golden 脚本，云端 python3 执行；本地零编译，云端 CANN Lab 真机为事实仲裁。
- 工程载体：`p:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\`（实测仓库 `Projects`，起新分支 `fix/p3-fullshape-fused`，保留 P2b 分支）。

## 实现方案（核心设计决策）

### 1. 融合归约范式（对齐官方已验证样板）

以官方 light 03.05 `matmul_abs.asc` 为直引骨架（`p:\Dev\CANN_Learning_Workspace\Reference_And_Documentation\tutorials\ascendc_operator_development_light\03_simple_operator_practice\answer\03.05\matmul_abs.asc`）：

- `Matmul` 的 C 模板类型从 `GM` 改为 **`VECIN`**（L0C 直通 vector 侧），`SetTraverse(FIRSTM)`；host tiling `SetCType(TPosition::VECIN, ND, FLOAT)`。
- 用 `while (matmulObj.Iterate<true>()) { GetTensorC<true>(ubLocal, false, true); ... }` 逐 tile 取 C 到 UB，取代 P2b 的 `IterateAll(cGlobal)` 整幅写 GM。
- 核类型改用 `__mix__(1,2)`（1 AIC + 2 AIV），以在本体同时执行 cube 与 vector 归约。
- UB 中逐 tile 收缩：对每个 (baseM×baseN) tile 的行向 ReduceMax（fp32 每 repeat 64 lanes，baseN>64 拆 chunk 后 `Max` 合并）→ 与 UB 中 baseM 个 running-max 做元素级 `Max(dst,acc,part,count)` 合并；该行块全部 N 块到齐后 `ReduceSum(runningMax,baseM)` 得行块部分和 → 写入 y 或受限 workspace（按 m 顺序/核隔离，避免写冲突）。
- 每 batch 循环独立处理；每核每 batch 只产出 1 个 fp32 部分和，最终由一次轻量 vector 归约（或按 b 直接落 y）汇总为 B 个标量。

### 2. 尾块与 K 尾段策略（真机裁决项，计划内置回退）

- M 尾块：M 的 16 倍主区走融合 cube 路径；不足 16 的尾行（<16 行）回退 **vector dot 逐行兜底**（沿用 P2a 点积哲学，逐行逐 K 乘加，行内 max 后并入该 batch 的 sum）。真机先裁决「M 单行/尾行（baseM=1..15、baseN≥16）cube tile 是否可跑」——**不得假设可行**（cube 1×1 已判死 507015）。
- K%16==8 尾段（K=8×odd 场景）：cube 只算 16 对齐主段，尾部 8 half 元素用 vector 乘加修正 C 后并入（在行 max 之前完成，保证 Σ_k 完整）；若 tiling/GetTiling 能直接接受 K%16≠0 则优先整 K 走 cube，由真机裁决。
- N 尾块：行向 reduce 的 chunk 循环天然支持任意 N，无需 cube 整列约束。

### 3. 铁律继承（勿重复试错）

- `#define ASCENDC_CUBE_ONLY` 必须在 `#include "lib/matmul_intf.h"` 之前（507015 真凶修复，官方 direct-invoke cube 样例同款），只要走 Cube 路径就保留。
- tiling 探针阶梯单次使用、逐探针重建（v0 SetFixSplit(128,128) 官方默认 / v1 不 SetFixSplit / v2..v5 自适应），每探针打 tiling 转储日志（M/N/Ka/Kb/singleCore*/base*/usedCores/isBias）；融合路径新增探针（baseM/baseN 针对新遍历/尾块形态）须先在头库核对 `dav_c220` 分支语义。
- 归约方向铁律：先每行 max over N、再跨行 sum over M（np.max(axis=-1) 后 np.sum(axis=-1)）；MaxSim 初值禁 0（全负行返回最大负数）。
- workspace 尺寸必须来自 `platform->GetLibApiWorkSpaceSize()`；tiling 一律栈上 by-value 传递（UB/L1 GetPhyAddr 喂 Init = 507015）。

### 4. 数据链多 case 化（最小 blast-radius）

- `main.asc`：把写死的 `CASE_*` 常量改为「case 表 + argv case_id 选择」；shape/字节数仍从 case 常量派生，防 drift。
- `scripts/gen_data.py`：按 case 表生成域内 shape（含大 shape M/N=8192 或 ≥2048、K=40/1000、M/N 尾块、B>1）的 fp16 输入与 golden（调用 `BatchMatmulMaxSum.impl`）；本地用 `bmm_maxsum_assets\golden\golden_fp64_stdlib.py` 交叉背书期望值。
- `scripts/verify_result.py`：`case_output_specs` 扩到多 case（y: fp32, rtol/atol=1e-3, tol≈0）。
- `run.sh`：按 case_id 拷贝 input/golden、运行、verify；失败护栏保留。
- 注：penv 无 numpy/torch，数据/golden 生成由云端 python3 执行；本地只跑零依赖 stdlib golden 做期望背书。

### 5. 复杂度与性能预期

- 每 tile：cube 计算 B·baseM·baseN·K，归约线性于 baseM·baseN（行向 chunk-reduce + running-max 合并），无 N+1 类重复读 GM；C 中间量从 B×M×N×4B 收缩到「B×每核部分和」量级，域边缘内存问题根治。
- 本切片正确性优先，允许单 cube 块 + 逐 batch 串行（`SetDim(1)` 起步，多核留 P4）；真机 timeout 120s 内 M=N=8192、K 大值若超时，先以域内代表性中大规模验证，明确记录未覆盖边界并回填父计划。

## 架构设计

```mermaid
flowchart TD
    subgraph Host (main.asc + run_kernel)
        A[读 TensorGroupInfo / case shape] --> B[GenBatchMatmulTiling 探针阶梯]
        B --> C[aclrtMalloc x1/x2/workspace/y]
        C --> D[启动融合 kernel __mix__ per batch]
    end
    subgraph Device 融合 kernel __mix__ (1 AIC + 2 AIV)
        E[Matmul C 出 VECIN] --> F[Iterate loop]
        F --> G[GetTensorC tile -> UB]
        G --> H[行向 chunk ReduceMax + Max 合并 running-max]
        H --> I{N 块完?}
        I -- no --> F
        I -- yes --> J[ReduceSum running-max -> 部分和]
        J --> K[写 y 或受限部分和]
    end
    D --> E
    K --> L[轻量 vector 汇总 -> y[b] fp32]
    L --> M[verify_result.py 对 golden PASSED]
```

## 目录结构

```
p:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\
├── kernel.asc                 # [MODIFY] P3 融合归约主体：VECIN+GetTensorC+行级归约；
│                              #   新 __mix__ 融合核、M/N 尾块与 K 尾段兜底、case 表守卫放开；
│                              #   保留 bmm_batch_dot 兜底、ASCENDC_CUBE_ONLY、探针阶梯与铁律注释
├── main.asc                   # [MODIFY] case 表 + argv case_id 选择；shape 仍从常量派生
├── CMakeLists.txt             # [MODIFY 视需要] 无实质改动预期（同工程编译）
├── run.sh                     # [MODIFY] 支持 argv case_id：拷 input/golden → 运行 → verify
└── scripts\
    ├── gen_data.py            # [MODIFY] 多 case（域内 4~6 例）fp16 输入 + golden
    ├── verify_result.py       # [MODIFY] case_output_specs 多 case（y fp32, 双 1e-3）
    └── BatchMatmulMaxSum.py   # [不变] impl 事实源（对拍背书）
p:\Dev\CANN_Learning_Refs\.codebuddy\
├── memory\2026-09-06.md       # [MODIFY] P3 首切经验/裁决/真机结果追加
├── memory\MEMORY.md           # [MODIFY] P3 状态快照更新
└── plans\PARENT_PLAN_batchmatmulmaxsum.md  # [MODIFY] P3 已切/待切子主题回填 + 本切片决策记录指针
（只读参考）p:\Dev\CANN_Learning_Workspace\Reference_And_Documentation\tutorials\...\light\03.05\matmul_abs.asc
（只读参考）...\ascendc_operator_development\05_fused_operator_development\answer\05.04_answer\op_kernel\matmul_abs.cpp
（只读参考）p:\Dev\CANN_Learning_Refs\cannbot-skills-master\cannbot-skills-master\ops\ascendc-api-best-practices\references\api-matmul.md
（只读权威）p:\Dev\CANN_Learning_Refs\cann_ascendc_headers_9.0.0\
```

## 关键代码结构（接口锚点，均须与头库核对后落码）

- Matmul C 出 VECIN 的模板形态（对齐 03.05）：

```cpp
Matmul<MatmulType<GM, ND, half>, MatmulType<GM, ND, half>,
       MatmulType<VECIN, ND, float>, MatmulType<GM, ND, float>> mm; // C 走 L0C→UB
while (mm.Iterate<true>()) { mm.GetTensorC<true>(ubTile, false, true); /* 行归约 */ }
```

- host tiling：`SetCType(TPosition::VECIN, ND, FLOAT)` + `SetTraverse(MatrixTraverse::FIRSTM)`；其余探针选项与 P2b 逐字节一致。
- 以上签名若与头库/官方样例冲突，以头库 `basic_api/impl` 的 `dav_c220` 分支为准并注释出处。

## 实施注意事项

- 每写一处 cube/归约 API 前先检索头库确认签名（GetTensorC 模板参数、ReduceMax/ReduceSum count 版、Max 元素级 count 版、DataCopyPad 4B 精确写）；结论写入注释与记忆并注明「已与头库核对」。
- 真机前本地完成静态自检（探针日志格式、case 表一致性、K%8/尾块分支可编译性）；git 提交粒度 = 每个可独立验证点（kernel 改造 / 数据链 / 每轮真机修复），push 前 pull --rebase。
- 云端每轮 run.sh 输出回传后先查「run path」与探针命中，再查 Max diff，避免误判方向。
</tech>

<extensions>

## Agent Extensions

### SubAgent

- **code-explorer**
- 用途：实现阶段深挖官方融合范式（light 03.05 matmul_abs.asc、进阶 05.04 matmul_abs.cpp）、cannbot api-matmul.md §7 骨架、头库 `basic_api/impl/dav_c220` 中 GetTensorC/ReduceMax/Max/Iterate 的真实签名与分支行为，以及跨工程比对 P2b kernel 与官方样板差异。
- 预期产出：返回带行级出处的关键接口结论与可直引代码片段清单，支撑 kernel 融合改造与架构决策记录落档。
</extensions>

<todolist>
<item id="fused-arch-decision" deps="">裁决融合路线并落架构决策记录（读 03.05/05.04/头库，定行块归约/尾块/K 尾段/多核边界）</item>
<item id="multi-case-data-chain" deps="">改造 main.asc 与 gen/verify/run.sh 支持 4~6 个域内 case 切换</item>
<item id="fused-kernel-impl" deps="fused-arch-decision">实现融合归约 kernel（VECIN+GetTensorC 行级归约+尾块/K8 兜底，保留铁律）</item>
<item id="cloud-verify-fix" deps="multi-case-data-chain,fused-kernel-impl">云端真机逐 case 对 golden 验证并迭代修复，回归 64³ 锚点与单点路径</item>
<item id="closeout-backfill" deps="cloud-verify-fix">落档经验与真机结果，回填父计划 P3 状态，实测仓库提交与 tag</item>
</todolist>
</plan_result>