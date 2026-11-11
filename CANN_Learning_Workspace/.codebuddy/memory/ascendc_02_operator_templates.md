# 主题② 常见算子类型实现模板

> 覆盖：逐元素（Add 完整范例）、规约、Matmul、VV/CV 融合。目前完成逐元素/Add；Matmul/融合待 04-05 章精读后补。
> 来源锚点前缀：`ascendc_operator_development/`。

## 1. 逐元素算子（Element-Wise，以 Add 为例）—— 已完成（02.04 + src/add_custom.asc）

### 1.1 算子分析与两级并行（02.04 第2节）

- 数学式 z = x + y；float、ND 布局；示例 shape (8,2048)。
- **两级并行设计**：
  - 第一级多核并行：`blockLength = totalLength / GetBlockNum()`；每核处理一段，偏移 = `blockLength * GetBlockIdx()`。
  - 第二级单核内流水并行：单核 blockLength 再切 tileNum 块（如 8），每块再按 BUFFER_NUM=2 双缓冲细分 → `tileLength = blockLength / tileNum / BUFFER_NUM`。
- Tiling 结构体由 Host 填充传给核函数，实现切分策略参数化（改切分只动 Host 参数，不改核代码）。

```cpp
struct AddCustomTilingData {
    uint32_t totalLength; // 总元素数，如 8*2048
    uint32_t tileNum;     // 单核内切块数，如 8
};
```

### 1.2 Init：地址设置 + 队列内存分配（公式最核心）

```cpp
__aicore__ inline void KernelAdd::Init(GM_ADDR x, GM_ADDR y, GM_ADDR z,
                                       uint32_t totalLength, uint32_t tileNum) {
    this->blockLength = totalLength / AscendC::GetBlockNum();              // 多核切分
    this->tileNum = tileNum;
    this->tileLength = this->blockLength / tileNum / BUFFER_NUM;           // 双缓冲再细分
    xGm.SetGlobalBuffer((__gm__ float *)x + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    yGm.SetGlobalBuffer((__gm__ float *)y + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    zGm.SetGlobalBuffer((__gm__ float *)z + this->blockLength * AscendC::GetBlockIdx(), this->blockLength);
    pipe.InitBuffer(inQueueX, BUFFER_NUM, this->tileLength * sizeof(float));
    pipe.InitBuffer(inQueueY, BUFFER_NUM, this->tileLength * sizeof(float));
    pipe.InitBuffer(outQueueZ, BUFFER_NUM, this->tileLength * sizeof(float));
}
```

### 1.3 Process：三级流水循环

```cpp
__aicore__ inline void KernelAdd::Process() {
    int32_t loopCount = this->tileNum * BUFFER_NUM;  // 双缓冲：循环块数翻倍
    for (int32_t i = 0; i < loopCount; i++) { CopyIn(i); Compute(i); CopyOut(i); }
}
```

### 1.4 三个流水任务

```cpp
// CopyIn：GM→UB 入队
AscendC::LocalTensor<float> xLocal = inQueueX.AllocTensor<float>();
AscendC::DataCopy(xLocal, xGm[progress * this->tileLength], this->tileLength);
inQueueX.EnQue(xLocal);
// Compute：出队计算，结果入队，输入 FreeTensor
AscendC::LocalTensor<float> xLocal = inQueueX.DeQue<float>();
AscendC::LocalTensor<float> zLocal = outQueueZ.AllocTensor<float>();
AscendC::Add(zLocal, xLocal, yLocal, this->tileLength);
outQueueZ.EnQue<float>(zLocal);
inQueueX.FreeTensor(xLocal);   // 记得释放输入，避免泄漏
// CopyOut：出队搬回 GM 再 FreeTensor
AscendC::LocalTensor<float> zLocal = outQueueZ.DeQue<float>();
AscendC::DataCopy(zGm[progress * this->tileLength], zLocal, this->tileLength);
outQueueZ.FreeTensor(zLocal);
```

### 1.5 完整代码源

`02_AscendC_basic/src/add_custom.asc`（含 main 验证：golden=const 值，std::equal 比对输出 `[Success]/[Failed]`）。
换 Sub 只需把 Add 换 Sub 等向量指令，模板其余部分不变（02.04 课后 sub_custom 印证）。

## 2. 规约算子（Reduction）

<!-- 待补充：Reduction 模式（多核部分和 + 跨核规约）后续在开源仓/实践中找官方实现 -->

## 3. 矩阵乘算子（Matmul）—— 已完成（04 章 + answer 源码）

### 3.1 数学式与规格（04.02/04.03）
- C[M,N] = A[M,K] × B[K,N] + Bias[1,N]（例：M=1024,K=256,N=640；A/B half、Bias/C float、ND）。

### 3.2 Cube 硬件与分形格式背景（04.02 2）
- Cube 每次按小块（half 为 16×16×16）并行计算 → 需要**分形格式**（NZ/ZZ/ZN），数据连续且复用。
- Mmad 基础 API 要求：L0A 中 A 用 FRACTAL_ZZ，L0B 中 B 用 FRACTAL_ZN，L0C 中 C 用 FRACTAL_NZ；**Matmul 高阶 API 自动完成 ND→分形转换**。
- 分形推导套路：`(…,X,Y)->pad->拆分(X1*X0,Y1*Y0)->reshape->transpose`。ZZ：分形 (16, 32B/位宽)；ZN：分形 (32B/位宽,16)；NZ 分形 16×16 布局按列主序排大块、行主序排内块。
- 逻辑存储位置（矩阵范式）：A1/B1/C1（整块，L2 类比）→ A2/B2/C2（小块，L1 类比）→ CO1/CO2（Cube Out，L0C）→ GM/VECIN/VECOUT。

### 3.3 切分（04.02 3.2）
- 多核：A 沿 M 轴切 singleCoreM、B 沿 N 轴切 singleCoreN（K 大时再切 singleCoreK）；核数 usedCoreNum=(M/singleCoreM)×(N/singleCoreN)。
- 核内：A 沿 M/K 再切 baseM/baseK、B 沿 N/K 切 baseN/baseK（Iterate 循环累加 K）。
- TCubeTiling 关键字段：usedCoreNum、M/N/Ka/Kb、singleCoreM/N/K、baseM/N/K、depthA1/depthB1、stepM/N/Ka/Kb、isBias、iterateOrder。

### 3.4 Kernel 五步标准流程（04.02 3 / 04.03 5.3）
1. **创建 Matmul 对象**：`typedef MatmulType<TPosition::GM, CubeFormat::ND, half> aType; ... Matmul<aType,bType,cType,biasType> matmulObj;`（类型信息=内存位置+格式+数据类型）。
2. **初始化**：`REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tilingData.cubeTilingData);`
3. **设置 A/B/Bias**：SetTensorA(aGlobal)/SetTensorB(bGlobal)/SetBias(biasGlobal)（先按 blockIdx 算 GM 偏移并 `aGlobal=aGlobal[offset]` 切片）。
4. **执行**：`while(matmulObj.Iterate()){ ...; matmulObj.GetTensorC(gmC); }` 或 `matmulObj.IterateAll(cGlobal)`（一次算 singleCoreM×singleCoreN）。
5. **结束**：`matmulObj.End()`。
- 完整 Kernel 类（04.03）：`MatmulKernel<aType,bType,cType,biasType>`，内含 matmulObj、a/b/c/bias GlobalTensor、tiling(TCubeTiling)、localMemSize；`CalcOffset(blockIdx, tiling,...)` 计算 M/N 方向核索引与 GM 偏移（公式见 04.03 5.3.4：offsetA=mCoreIndx*Ka*singleCoreM 等）。
- 入口：`extern "C" __global__ __aicore__ void matmul_custom(GM_ADDR a,b,bias,c, workspace, tiling)` + REGISTER_TILING_DEFAULT + GET_TILING_DATA。
- Host 侧用 **MultiCoreMatmulTiling** API 生成 TCubeTiling（见主题⑤/③的 04 章行；workspace = GetLibApiWorkSpaceSize()，Matmul 需要系统 workspace）。

### 3.5 Host Tiling（MultiCoreMatmulTiling 链式调用）
```cpp
MultiCoreMatmulTiling cubeTiling(ascendcPlatform);
cubeTiling.SetDim(2);
cubeTiling.SetAType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT16);
cubeTiling.SetBType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT16);
cubeTiling.SetCType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT); // CV 融合时用 VECIN
cubeTiling.SetBiasType(TPosition::GM, CubeFormat::ND, DataType::DT_FLOAT);
cubeTiling.SetShape(M,N,K); cubeTiling.SetOrgShape(M,N,K);
cubeTiling.SetFixSplit(baseM, baseN, -1);  // baseK=-1 自动
cubeTiling.SetBias(true);
cubeTiling.SetBufferSpace(-1,-1,-1);       // L1/L0/UB 分配，-1 自动
cubeTiling.GetTiling(tiling->cubeTilingData);  // 填 TCubeTiling
```
- Tiling 结构体用 `TILING_DATA_FIELD_DEF` 宏注册（tiling.h）或用普通 struct + `#include "kernel_tiling/kernel_tiling.h"` 内含 TCubeTiling（answer 05.04 用后者）。
- 固定 shape 时 SetBlockDim 可与 GetTiling 前 SetDim 一致（也可整核单核 SetBlockDim(1)+SetDim(2) 由框架自动分核，见 04.03 原例）。

## 4. 融合算子（VV/CV）—— 已完成（05 章 + answer 源码）

### 4.1 概念（05.02）
- **VV 融合**：多个连续 Vector 算子合成一个算子，一次搬入/一次搬出；中间结果留在 Local 链式计算（**不搬回 GM、不写 workspace**）。
- **CV（Mix）融合**：Cube+Vector 算子融合；单核内 Cube 与 Vector 流水并行（一块数据的 Vector 与下一块的 Cube 并行）。
- 收益：减少调度开销、减少 GM 往返搬运、中间结果驻留 Local、减内存占用。
- Tiling 注意：UB 预算按**输入数最多的算子**设计（如 Add 2 输入 vs Sinh 1 输入 → 按 Add 算 buffer 数）。

### 4.2 VV 融合模板（z=(x+y)(x-y)，05.03 SquareDiff）
- Host/Kernel 骨架与 Add 完全一致（Tiling totalLength/tileNum、blockLength/tileLength、SetGlobalBuffer、CopyIn/CopyOut 不变）。
- Compute 内链式计算同一/跨 LocalTensor，可原地复用输入（如先 Add 存 zLocal，再 Sub 覆盖 xLocal，最后 Mul 到 zLocal）；BUFFER_NUM/QUEUE_DEPTH=1（例）。
- 核入口同样 5 参 `(x,y,z, workspace, tiling)` + REGISTER_TILING_DEFAULT + GET_TILING_DATA；数据类型用 DTYPE_X/Y/Z 宏。
- 测试验证：msopgen 生成 → 改 tiling.h/host/kernel → `bash build.sh` → `./build_out/custom_opp_*.run --install-path=${HOME}` → g++ 编译 aclnn 测试（见主题⑤§9）。

### 4.3 CV 融合模板（Matmul→Vector 激活，05.04 matmul_abs）
- **核心：C 矩阵的 MatmulType 位置声明为 `TPosition::VECIN`**（而非 GM）→ Matmul 结果直接落到 UB 侧 LocalTensor，Vector 指令免 GM 往返直接消费。
```cpp
Matmul<MatmulType<GM,ND,aType>, MatmulType<GM,ND,bType>,
       MatmulType<TPosition::VECIN,ND,cType>, MatmulType<GM,ND,biasType>> matmulObj;
TQue<QuePosition::VECOUT, 1> reluOutQueue_;   // 向量结果队列（深度1）
```
- Process 驱动：SetTensorA/B/Bias → `while (matmulObj.Iterate<true>())`（模板参 true=同步）→
  ①MatmulCompute：`reluOutLocal = reluOutQueue_.AllocTensor<cType>(); matmulObj.GetTensorC<true>(reluOutLocal, false, true);`
  ②激活：`AscendC::Abs(reluOutLocal, reluOutLocal, baseM*baseN); reluOutQueue_.EnQue(reluOutLocal);`
  ③CopyOut：`reluOutQueue_.DeQue<cType>();` 用带 stride 的 DataCopy 把 [baseM×baseN] 子块回写到 cGlobal 大矩阵 → `reluOutQueue_.FreeTensor(reluOutLocal);`
  → `matmulObj.End();`
- **子矩阵回写要点**（整矩阵切块后拼接回 GM）：用 `DataCopyParams` 版 DataCopy：
```cpp
DataCopyParams p = {blockCount=baseM, blockLen=baseN*sizeof(cType)/32, srcStride=0,
                    dstStride=(N-baseN)*sizeof(cType)/32};
DataCopy(cGlobal[startOffset], local, p);   // 行间按整行长步进
startOffset = count%roundM * baseM*N + count/roundM * baseN; // 每 Iterate 块在 GM 的位置
```
- Init 需 `pipe->InitBuffer(reluOutQueue_, 1, baseM*baseN*sizeof(cType))`；入口 `REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tilingData.cubeTilingData)` 后再 Init/Process。
- Host 侧与纯 Matmul 相同，仅 SetCType 位置用 VECIN。
- **单文件自包含变体**（light 03.05 matmul_leakyrelu_custom.asc，非 msopgen 工程）：全部写在一个 .asc（头含 `lib/matmul_intf.h`、`kernel_tiling/kernel_tiling.h`、`tiling/tiling_api.h`、`using namespace matmul`）；Host main 里用 `GenerateTiling()` 函数返回 `AscendC::tiling::TCubeTiling` 直接按值传核；Kernel 用 `__mix__(1, 2)`（1 AIC + 2 AIV）启动，C 类型仍 VECIN；适合 kernel 级回归/演示。

## 5. 各模板间差异速查

| 模板 | CopyIn | Compute | CopyOut | 要点 |
|---|---|---|---|---|
| 逐元素(Add) | DataCopy GM→双队列 | 单条向量指令 | DataCopy 出队→GM | 双缓冲可开 |
| VV 融合 | 同 Add | 多条向量指令链式 | 同 Add | 中间结果 Local 链式，buffer 常=1 |
| Matmul | Matmul 自动（SetTensorA/B/Bias） | Iterate/IterateAll | GetTensorC/IterateAll 直写 GM | 需 REGIST_MATMUL_OBJ + 系统 workspace |
| CV 融合 | 同 Matmul（C 落 VECIN） | Iterate<true> + Vector 指令 | DataCopyParams 子矩阵回写 | C 位置=VECIN；队列深度1 |
