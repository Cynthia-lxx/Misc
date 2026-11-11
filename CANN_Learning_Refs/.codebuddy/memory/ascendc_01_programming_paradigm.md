# 主题① Ascend C 编程范式与流水线

> 覆盖：三大编程范式、核心组件（TPipe/TQue/TBuf/TPosition）、双缓冲、核函数声明与调用、数据流。
> 来源锚点前缀：`ascendc_operator_development/`；主要：02.03 编程范式与API、02.02 HelloWorld、02.04 Add、01.04 基本概念。

## 0. 硬件抽象背景（01.04）

- AI Core 三大组件：**Scalar**（地址计算/循环/发射指令+同步）、**Vector**（向量运算）、**Cube**（矩阵运算）、**DMA**（GM↔Local 及各层 Local 间搬运）、**Local Memory**（片上所有缓冲区统称）。
- AI Core 内指令队列相互独立异步并行：**Vector 指令队列、Cube 指令队列、MTE 指令队列**（数据搬运 DMA）。
- 三大关键流：异步指令流（Scalar 发射到各单元队列并行执行）、同步信号流（Scalar 下发同步指令保证队列间依赖）、计算数据流（DMA GM→Local→计算→写回 Local→DMA 搬回 GM）。
- **关键原则**：计算单元只能对 Local Memory 中数据计算；数据进/出 Local 必须经 DMA。→ 这正是 CopyIn/CopyOut 的来源。
- 概念：**Tiling** = 因 UB 容量有限，把大数据切块分批搬运计算的切分过程。
- 术语（02.03 速查）：DataBlock=数据搬运最小单位 32B；Repeat：Vector 单元每次读 256B 迭代一次，repeatTime 为总迭代次数；Stride 控制地址步长（DataBlock Stride/Repeat Stride）；Mask 控制每次迭代哪些元素参与（逐bit/连续模式）。

## 1. 三种典型算子编程范式（02.03 1.1）

| 范式 | 任务流水 | 适用 |
|---|---|---|
| 基本矢量范式 | CopyIn → Compute → CopyOut | 最常用/基础，理解流水线起点 |
| 基本矩阵（Cube）范式 | CopyIn → Split → Compute → Aggregate → CopyOut | 需分片或规约的矩阵运算 |
| 复杂矢量/矩阵（融合）范式 | CopyIn→Compute→CopyOut/In→Compute→CopyOut | 多阶段/数据依赖复杂，把多个 CopyOut+CopyIn 首尾相连 |

## 2. 基本矢量范式与数据流（02.03 1.2）

- 数据流方向：`GM → (DMA, MTE2) → Local[VECIN] → Compute(Vector) → Local[VECOUT] → (DMA, MTE3) → GM`。
- CopyIn：GM→Local（VECIN），完成后 EnQue 交给下游。
- Compute：DeQue 取数 → 矢量计算 → EnQue 结果到 VECOUT。
- CopyOut：DeQue 取结果 → Local→GM（VECOUT）。
- 编码设计流程：①Init（TPipe 分配队列/临时内存）②CopyIn ③Compute ④CopyOut（Process 循环驱动）。

## 3. 核心组件（02.03 1.2）

- **GlobalTensor / LocalTensor**：数据载体；GlobalTensor↔Global Memory；LocalTensor↔Local Memory；各种指令 API 直接操作对象。
- **TPipe**：内存管理模块；一个 Kernel **必须且只能有一个** TPipe；通过 `InitBuffer(queue, bufNum, lenBytes)` 为队列分配内存（也支持 TBuf）。
- **TQue**：任务间通信同步队列。位置用 `TQue<TPosition::VECIN/VECOUT, QUEUE_DEPTH>`。API：`AllocTensor<T>()`（取空闲内存）、`FreeTensor(t)`（归还，防止泄漏）、`EnQue(t)`（下游就绪）、`DeQue<T>()`（取数据）。
- **TBuf**：任务内临时变量内存；`pipe.InitBuffer(tbuf, size)` + `tbuf.Get<T>()`；**只能计算，不能出入队**。
- **TPosition**：抽象逻辑位置代替物理存储。矢量编程用：GM / VECIN（搬入存放处）/ VECOUT（搬出存放处）/ VECCALC（临时变量处）。

## 4. Double Buffer 机制（02.03 1.2、02.04 3.2）

- 基础：MTE（搬运）与 Vector（计算）指令队列独立可并行 → CopyIn/CopyOut 与 Compute 可重叠，隐藏搬运时间。
- 实现：为队列申请内存时设 **2 个内存块**：`pipe.InitBuffer(inQueue, 2, bytes)`（代码常用 `BUFFER_NUM=2`/`QUEUE_DEPTH=2` 常量）。
- 循环次数注意（02.04 3.7）：若单核数据切成 tileNum 个 tile 且开 double buffer，**循环次数 = tileNum × BUFFER_NUM**（数据块实际切成 tileNum*2 块），循环内依次 CopyIn(i)/Compute(i)/CopyOut(i)，同一时刻三者可能处理不同块。
- 内存块个数取 2 即双缓冲；更多块=多缓冲。

## 5. 核函数（Kernel）声明规则（02.02 2.x）

- 修饰符：`__global__`（标识核函数，可用 `<<<...>>>` 调用）+ 执行空间限定符之一：
  - `__aicore__`：AI 核执行，不区分核型，编译器按代码中 Vector/Cube 指令自动识别；若只含标量指令必须显式指定 vector/cube/mix。
  - `__vector__`：仅 Vector 核；`__cube__`：仅 Cube 核；`__mix__`：Cube+Vector 融合。
- 返回值必须 void；入参仅支持**指针类型或 C/C++ 内置基础类型**（float*/int32_t 等）；指针入参带 `__gm__` 限定符指向 Global Memory。
- `GM_ADDR` 宏简化写法：`#define GM_ADDR __gm__ uint8_t* __restrict__`；使用时如 `__gm__ float*`。
- 典型入口函数体（02.04 3.4）：
```cpp
__global__ __aicore__ void add_custom(GM_ADDR x, GM_ADDR y, GM_ADDR z, AddCustomTilingData tiling) {
    KERNEL_TASK_TYPE_DEFAULT(KERNEL_TYPE_AIV_ONLY); // 显式声明矢量计算内核（AIV）
    KernelAdd op;
    op.Init(x, y, z, tiling.totalLength, tiling.tileNum);
    op.Process();
}
```

## 6. Host 侧调用与 SPMD（02.02 3-4）

- 异构模型：Host（ACL 运行时：内存/设备/Stream 管理）+ Device（AI Core 执行 Kernel）。Kernel 以 Task 形式进入 Stream 队列，调度到空闲 AI Core。
- **SPMD**：一份代码多核执行、各核数据不同；每核通过 `block_idx` 区分身份；代码中 `GetBlockIdx()` 取当前核逻辑 ID（0 起）、`GetBlockNum()` 取总核数。
- 调用语法：`kernel_name<<<blockDim, l2ctrl, stream>>>(arg list);`
  - blockDim：执行核数；l2ctrl：保留参，固定 `nullptr`；stream：`aclrtStream` 任务队列。
- 调用完整步骤（02.02/02.04）：aclInit → aclrtSetDevice → aclrtCreateStream → 申请 HOST/Device 内存并拷贝输入 → `<<<>>>` 启动 → `aclrtSynchronizeStream` → 结果拷回 Host → 释放资源 → aclrtResetDevice → aclFinalize。详见主题④/⑤。

## 7. 头文件与工程交付件

- 头文件（固定，自定义算子直接复用）：
```cpp
#include "acl/acl.h"          // Host 侧应用
#include "kernel_operator.h"  // Kernel 侧（AscendC:: 命名空间）
```
- 单文件 `.asc` 交付件模式（02.04 第5节）：**add_custom.asc**（Device 核函数+Host 调用+main 混编于一个 .asc）+ **CMakeLists.txt**。文件后缀 `.asc` 默认 C++17。
- 编译（bisheng，云端）：`bisheng xxx.asc --npu-arch=dav-2201 -o demo`；或 CMake：`cmake_minimum_required(VERSION 3.16)` + `find_package(ASC REQUIRED)` + `project(kernel_samples LANGUAGES ASC CXX)` + `add_executable(demo add_custom.asc)` + `target_compile_options(... --npu-arch=dav-2201)`。
- `--npu-arch` 取值：Atlas A2/A3 系列 = 2201（本教程仅支持 dav-2201）；A200I/500 A2=3002；Atlas 推理系列=2002；Atlas 训练系列=1001。`ASC_DIR=$ASCEND_HOME_PATH/aarch64-linux/tikcpp/ascendc_kernel_cmake/`。

## 8. 简单矢量算子（Add）类骨架模板（02.04）

```cpp
class KernelAdd {
public:
    __aicore__ inline KernelAdd() {}
    __aicore__ inline void Init(GM_ADDR x, GM_ADDR y, GM_ADDR z, uint32_t totalLength, uint32_t tileNum);
    __aicore__ inline void Process();
private:
    __aicore__ inline void CopyIn(int32_t progress);
    __aicore__ inline void Compute(int32_t progress);
    __aicore__ inline void CopyOut(int32_t progress);
private:
    AscendC::TPipe pipe;
    AscendC::TQue<AscendC::TPosition::VECIN, QUEUE_DEPTH> inQueueX, inQueueY;
    AscendC::TQue<AscendC::TPosition::VECOUT, QUEUE_DEPTH> outQueueZ;
    AscendC::GlobalTensor<float> xGm, yGm, zGm;
    uint32_t blockLength, tileNum, tileLength;  // 每核长度 / 每核tile数 / 每tile长度
};
```
Init 的关键公式与调用见主题②（算子模板）完整代码。
