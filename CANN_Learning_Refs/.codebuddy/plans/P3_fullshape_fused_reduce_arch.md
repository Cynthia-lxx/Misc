# P3 首切架构决策记录：大 shape 域 + 融合归约

> 切片：p3-fullshape-fused-reduce（父计划 P3「全约束正确性」的第一垂直切片）
> 日期：2026-09-06
> 载体工程：`p:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\`（分支 `fix/p3-fullshape-fused`，基线 commit `11725a3`）
> 权威依据：官方 light 03.05 `matmul_abs.asc`（融合范式直引）、进阶 05.04 `matmul_abs`（同构）、头库 `cann_ascendc_headers_9.0.0`（2201/dav_c220 语义）。

## 0. 语义与判据（不随实现变）

- `y[b] = Σ_m max_n C[b][m][n]`，`C = x1[b]·x2[b]`；先 MatMul、再每行 max(N)、后跨行 sum(M)，不可交换。
- 输入 fp16、输出 y 恒 fp32、K 点积与归约 fp32 累加、golden FP64→FP32、双 1e-3。
- 形状域：B∈[1,64]，M/N∈[1,8192]，K∈[32,8192] 且 8 倍数，B×M×K/B×N×K≤2^28；M/N 非 16 倍须正确。
- MaxSim 初值=-∞ 或行首（禁 0，全负行返回最大负数）。
- 不修改输入；transpose=false 守卫；dtype 本切片 fp16 only。

## 1. 路线裁决：融合归约取代「C 整幅落 GM」

P2b 是「cube 写 C 到 GM temp → vector 核读回归约」，C=B×M×N×4B，在域边缘（B=64、M=N=8192、K=32）约 17GB 必然爆内存。
**裁决：采用官方 light 03.05 `matmul_abs.asc` 的 `VECIN`+`GetTensorC` 融合范式**，C tile 在核内即行级收缩，不整幅落 GM。

### 1.1 直引范式（已对 03.05/05.04 逐行核对）

```cpp
// kernel 侧：Matmul C 出 VECIN（不写 GM）
Matmul<MatmulType<GM, ND, half>, MatmulType<GM, ND, half>,
       MatmulType<VECIN, ND, float>, MatmulType<GM, ND, float>> mm;
// Process() 循环（替代 IterateAll）
while (mm.template Iterate<true>()) {
    GetTensorC<true>(tileLocal, false, true);  // 每轮取一个 baseM×baseN C tile 到 UB
    /* 核内行级归约：Max 合并进 running-max → N 块到齐后 ReduceSum */
}
```

- host tiling：`SetCType(TPosition::VECIN, CubeFormat::ND, DT_FLOAT)`；其余探针选项与 P2b 一致；
  `SetBias(true)` 全零 bias 保留（与 P2b 输入结构兼容，bias 全 0 不引入数值）。
- 核形态：官方样例用 `__mix__(1, 2)`（AIC+AIV 双算力融合）。direct-invocation 下 `run_kernel` 只启动本核。
- `ASCENDC_CUBE_ONLY` 铁律保留：必须在 `lib/matmul_intf.h` 之前 define。
- REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), mm, &tiling) 用法与 P2b 相同（栈上 by-value tiling，禁 GM/L1 GetPhyAddr 喂 Init）。
- 每 batch 循环独立处理：tiling 是 per-batch（同 shape 可复算）；主循环 `for b in 0..B` 内 `Iterate` 遍历该 batch 全部 M×N tile。

### 1.2 行块归约（C 收缩内核）—— 关键设计

每轮 `GetTensorC` 得一个 (baseM × baseN) fp32 C tile（UB 内**行主序连续**，从 03.05 `CopyOut` 的
`DataCopy` blockCount=baseM / blockLen=baseN 参数可证：UB tile 即 baseM 行、每行 baseN 个连续 fp32）。
要在不落 GM 前提下得到 `y[b]=Σ_m max_n`，采用「running-max + 分块 ReduceMax/Max 合并」：

1. **tile 内行向 max over N**：对 tile 中每一行（该行含 baseN 个 fp32）做整行 ReduceMax，得到 baseM 个「行局部 max（该 N 段内）」。
   - 2201 ReduceMax count 版上限 = TOTAL_UB_SIZE/T（fp32 elementNumPerRep=64）——**count≤64 单 repeat、count 更大内部自动拆**，
     因此 baseN≤128 时一次 `ReduceMax(dst, src, tmp, baseN)` 即可处理一整行（count 语义整段归约，非逐元素）。baseN 若 >128（如 256）内部树形完成，count 上限仍是 UB 容量，理论可用；但为稳妥，行向归约分 chunk 进行（每 chunk≤64 lanes 一段后再 `Max` 合并）是**正确且最稳**的兜底写法，与 P2b 的 count 版用法一致。
   - **N 块不完整（尾 N）**：行向 chunk 循环天然支持任意 N（行内 N 尾段单独一小 chunk ReduceMax + Max 合并），无需 cube 整列约束 → **N 任意（含非 16 倍）由行向归约吸收**，这是相对 P2b（要求 N≤64）的重大放开。
2. **running-max 跨 N 块合并**：题目对同一行 m 的 max 需覆盖**全部 N**。设每核负责 (singleCoreM × singleCoreN) 的 C 域；
   为让每行最终 max 完整，**每核应覆盖整 N 行（singleCoreN == N）或对该行各 N 段分别算局部 max 再合并**。
   - 裁决：**行块级 running-max 缓冲 UB = singleCoreM 个 fp32**。遍历 roundN 个 N 块时，对每个 tile 的行向 reduce 结果
     与 running-max 中对应行做元素级 `Max(running, running, tileRowMax, count)`（count≤64 分块）合并。
     当该核的 N 方向全部块处理完，running-max 即为每行最终 max。
   - 每行最终 max 就绪后：`ReduceSum(runningMax, singleCoreM)`（64 元素分块累加，P2a 已验证 Add(acc,part,1) 形态）得行段部分和。
3. **跨 M 段/跨核汇总**：本切片的归约目标是 **B 个标量**。多核边界见 §2。核内把其行段的部分和累加写 y[b] 对应槽位
   （或受限 workspace），由一个轻量 vector 汇总核把 per-batch 各段和相加得 y[b]；若每核负责整 M 段（singleCoreM==M）则核内直接写 y[b]。

### 1.3 tile 的 M 起点推算（不依赖 GM 布局）

03.05 的 `CopyOut` 用 `count`（Iterate 轮次）推导 tile 的全局位置：
`startOffset = count%roundM * baseM*N + count/roundM * baseN`，其中 `roundM = singleCoreM/baseM`。
即 tile 遍历顺序是**先 M 后 N**（FIRSTM：每完成一个 N 段内所有 M 块）。归约侧需要知道当前 tile 的行块全局行号
`mBlockIdx = count % roundM`（FIRSTM）。若实测遍历序相反（FIRSTN），则 mBlock 推算式改为 `count / roundN`；
**实现时先在日志转储一轮 tile 序号与 M/N 前进方向，确证 FIRSTM 语义后再固定公式**（真机裁决项 R2）。
- 注：03.05 直接按上述公式回写 GM，证明「tile 序号 → (m,n)」是库的确定遍历语义；融合侧只需要 mBlock 序号，
  不必知 N 块序号（running-max 只需知道第几行）。唯一前提：`roundM` 整除语义成立，即每轮 tile 高度恒 = baseM（M 尾块例外，见 §3）。

### 1.4 UB 容量预算

- GetTensorC tile 缓冲：baseM×baseN×4B。官方 baseM/baseN≤128 时 =64KB。加 running-max（singleCoreM≤8192 → 32KB）、
  行向归约临时与 chunk tmp、bias 等。910B 单核 UB≈192KB（mix 核 AIV 侧）。
  **风险：baseM/baseN 由 tiling 库给 128 而 singleCoreM 也大时，总 UB 可能超**。缓解：tile 缓冲复用（一个 tile 算完再取下一个）、
  running-max 不动、行向 reduce 用「每 chunk 一个临时行」循环而非一次性铺满。真机裁决项 R3（base 128×128 是否放得下 192KB 预算）。
  - 03.05 官方即 baseM=baseN=128、VECIN buffer=baseM*baseN*4=64KB + matmul 库内部（库自管）在一个 kernel 跑通，
    说明该预算在 2201 可行；我们额外增加 running-max≤32KB + 临时块，仍应可放，但必须实测确认。

## 2. 多核边界（本切片取单核正确性优先，多核留 P4）

- **裁决：本切片 `SetDim(1)` + 每 batch 单核遍历其全部 M×N tile**（对齐 P2b 已验证形态，把变量最小化）。
  B 个 batch 由 host 循环串行或单核内 batch 循环；多核行段分配、cube+vector 流水（P4）不进入本切片实现。
  - 依据：P2b 已证明 SetDim(1)、per-batch tiling、直启 __mix__ 单核可跑通 64 shape；本切片先保正确性，
    性能主题（多核/尾块均衡/double buffer）在父计划 P4。
- 若真机暴露「单核无法在 120s 内完成大 shape」，用「沿 B 拆核」（batch 并行）优先——各 batch 完全独立、天然无跨核归约，
  代价仅是 host 侧 per-batch 启动；但 tiling 库是 per-batch SetDim(1) 的，多核需各自 REGIST 一份 tiling（同 shape 可共用）。
  **此属真机性能裁决（R5），不阻塞正确性骨架。**

## 3. 尾块与 K 尾段策略（真机裁决 + 内置回退）

| 情形 | 首选手法 | 真机裁决点 | 内置回退 |
|---|---|---|---|
| M 尾块（M 非 baseM 倍数，余 1..15 行） | M 的 16 对齐主区走融合 cube；不足 16 的尾行走 **vector dot 逐行兜底**（沿用 P2a 点积哲学，行内 max 后并入该 batch sum） | R1：baseM=1..15（baseN≥16）的 cube tile 是否可跑——**不得假设可行**（cube 1×1 已判死 507015）；若可跑则尾行也可直接喂 cube | vector dot 兜底（确定的正确路径，慢但正确） |
| K%16==8 尾段（K=8×odd） | cube 只算 16 对齐主段，尾部 8 half 用 vector 修正 C 后并入（行 max 之前完成 Σ_k） | R4：tiling/GetTiling 能否直接接受 K%16≠0（=8）整 K 走 cube | 修正法：`GetTensorC` tile 行向归约前，用 DataCopyPad 取 A 尾段与 B 尾段行、vector 乘加累加到 C 的对应 tile（fp32 累加，修正量在行 max 前并入） |
| N 尾块 | 行向 chunk reduce 天然支持任意 N（§1.2.1） | —（不依赖 cube 整列） | 同上 |

- 三个真机裁决点 R1/R4/R5 均**不影响骨架可写性**：骨架先按「M/N 16 对齐 + K%16==0 + SetDim(1)」编写并真机 PASS，
  再逐步打开尾块/尾段开关逐项裁决，符合切片「正确性骨架主力 + 内置回退」定位。

## 4. 探针与铁律继承（勿重复试错）

- `#define ASCENDC_CUBE_ONLY`（matmul_intf.h 之前）——507015 真凶修复，cube 路径长期保留。
- tiling 探针阶梯单次使用逐探针重建；融合路径新增探针需求：**CType=VECIN 是否影响 baseM/baseN/singleCore 选择、
  GetTensorC tile 是否仍为 baseM×baseN 行主序**——首个探针命中后立即在日志打印 tiling dump（M/N/Ka/Kb/singleCore*/base*/usedCores/isBias）
  与 `roundM=singleCoreM/baseM`，供 R2/R3 裁决。
- 归约方向铁律：先每行 max over N、再跨行 sum over M（impl.py `np.max(axis=-1)` 后 `np.sum(axis=-1)`）；MaxSim 初值禁 0。
- workspace 尺寸必须 `platform->GetLibApiWorkSpaceSize()`；tiling 栈上 by-value。
- GetTensorC 从 L0C 拷贝到 UB 与 vector 算子需同步：`Iterate<true>`（sync=true 模板）+ `GetTensorC<true>` 保证每轮 tile 就绪；
  参考 05.04 的 `while (matmulObj.Iterate<true>())` 与 GetTensorC<true>(…,false,true) 语义。

## 5. 关键待核 API（写码前以头库为准）

- `GetTensorC<true>(co2Local, enAtomic, enSequentialWrite)`（已在 `matmul.h` 确认签名与 VECIN 限制：只支持 NZ 数据格式达 VECIN？——
  03.05 用 ND + VECIN 实际可跑，说明 VECIN 承载经库内部转换；以 03.05 直引为准，头库注释为保守声明）。
- `ReduceMax(dst,src,tmp,count,calIndex)` count 版（已核，2201 count 上限 TOTAL_UB_SIZE/T）；
  `Max(dst,src0,src1,count)` 元素级 count 版（已核存在）。
- 行向 reduce 的 chunk 合并需要把 tile 的 baseM 行看成 row-major：用 `tileLocal[row*baseN ...]` 子视图不可行（LocalTensor 无切片），
  **用块搬移/指针 offset（`tileLocal[row*baseN]` 的 Tensor 下标偏移）逐行 ReduceMax 处理**——03.05 的 Abs 是对整体 baseM*baseN 一次 count 版调用，
  说明 count 版可一次覆盖整块；行级分别 reduce 时按行 offset 步进（每行独立一次 ReduceMax count=baseN），成本 = baseM 次/repeat/tile。
  实测若太慢，改「整 tile 一次 count 版 ReduceMax 拿整块 max 再…」不可行（那是全块归约非行归约）；保持逐行/逐 chunk。
- （实现时依据 tiling dump 确认 singleCoreM/N 与 tile 序号映射，再落 running-max 缓冲长度与 M 起点公式。）

## 6. 验证矩阵（真机对 golden）

域内 case 组建议（数据链 todo 展开；每个 case 须 `verify_result.py` PASSED）：
C0 锚点 M=N=K=64（回归，沿用 P2b 数据）；C1 大 shape 如 M=N=8192,K=32（域内 B×…≤2^28 上限附近或降为 M=N=2048,K=64 先真机可跑）；C2 B>1（如 B=8 或 16）；C3 K=40/1000（8 倍非 16 倍）；C4 M/N 尾块（非 16 倍如 1000×1000 或 1000×64）；C5 混合/随机形状。
golden 与数据生成在云端 python3（penv 无 numpy），本地零依赖 `bmm_maxsum_assets\golden\golden_fp64_stdlib.py` 背书期望。

## 7. 本切片 Exit Criteria（承接计划）

域内代表性 case 真机对 golden 全 PASSED；64³ 锚点与 M==N==1 点积路径不回归；
诊断证实 C 未整幅落 GM（或明确记录收缩边界）；本决策 + 真机裁决结果落档；
经验入 `.codebuddy\memory\2026-09-06.md`，回填父计划 P3 已切/待切子主题。

## 8. 待办真机裁决清单（R1–R5，按出现顺序触发）

- R1 M 尾行(<16) cube tile 可跑性 → 决定尾行走 cube or vector dot。
- R2 Iterate tile 遍历序（FIRSTM?）实测定案 mBlock 起点公式。
- R3 UB 预算（base128 + running-max ≤8192 + 临时）在 192KB 内可放。
- R4 K%16≠0（%8==0）能否整 K 走 cube / 需 vector 尾修正。
- R5 大 shape 单核 120s 时间预算；不足则沿 B 拆核。
