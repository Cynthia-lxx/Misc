# 主题③ Tiling 策略设计原则与计算

> 覆盖：Tiling 概念与函数职责、设计三原则、四种切分场景、32B 对齐计算、核间均分/核内切分公式、UB 容量预算、泛化 Tiling 模板公式。
> 主要来源：`ascendc_operator_development/03_intermediate_vector_operator_development/`（03.02 6.4、03.04 全节、03.05 6.x）。

## 0. 概念回顾（03.02 6.4 / 03.04 2）

- **Tiling**：AI Core 片上 Local Memory（UB 等）容量有限，数据一次装不下 → 按计算粒度切块、按"搬运→计算→回写"循环流水完成全量计算。
- **Tiling 块**：每次搬运的那一块数据。
- **Tiling 算法/策略**：依据输入形状确定搬入基本块大小的算法（Host 侧）。
- **Tiling 函数**：实现该算法的函数，一般定义在 Host 侧（工程里放 op_host/*.cpp 的 optiling 命名空间）。
- Tiling 函数产物：**TilingData 结构体**（传给 Kernel 的切分参数集合，Host 填、Kernel 读）。

## 1. 设计三原则（03.04 3.1-3.3）

1. **内存对齐原则**：UB 要求数据空间 32B 对齐。
   - 不满足时把总长**向上对齐到 32B** 后再参与计算（`align32 = ((len + 31) / 32) * 32`，按字节算）。
   - 所有 Tiling 切分计算以 **32B 为最小粒度**。
2. **访存优化原则**：GM↔AI Core 交互开销大 → 尽量占满 UB、增大单次搬运块、减少搬运次数。
3. **多核均衡原则**：把任务均匀分到各 AI Core，避免闲置/不均。

## 2. 切分的四种场景（03.04 3.3）

| 核间 | 核内 | 处理方式 |
|---|---|---|
| 均分 | 均分 | 各核数据量与核内每块均相同 |
| 均分 | 不均分 | 核内出现**尾块**：常规块 + 尾块处理 |
| 不均分 | 均分 | **尾核**处理：前几核多分 1 个块（大核/小核）|
| 不均分 | 不均分 | 尾核 + 尾块 同时处理 |

## 3. 泛化 Tiling 数值示例（03.04 3.4，half, shape=(1,660), 4 核）

1. **32B 对齐**：660×2B=1320B → `((1320+31)//32)*32 = 1344B`，即 **42 个 32B 块**。
2. **核间拆分**：42//4=10 块/核；42%4=2 → 前 2 核为大核各 11 块(352B)，后 2 核小核各 10 块(320B)。元素数=字节/2。
3. **核内拆分**（按 UB 容量）：如 UB=768B，Add 需 3 路（x/y/z）同驻 → 单路 ≤ 768/3=256B = 8 个 32B 块 = 128 元素/批 → 11 块分 2 批(8+3)、10 块分 2 批(8+2)。常规批处理 `tileDataNum` 个元素，最后一批处理 `tailDataNum`。
4. TilingData 关键字段（大核/小核版模板）：
```cpp
struct TilingDataTemplate {
    uint32_t smallCoreDataNum;  // 小核总元素数
    uint32_t bigCoreDataNum;    // 大核总元素数
    uint32_t finalBigTileNum;   // 大核批次数（循环次数）
    uint32_t finalSmallTileNum; // 小核批次数
    uint32_t tileDataNum;       // 单批标准元素数（UB预算决定）
    uint32_t smallTailDataNum;  // 小核尾批元素数
    uint32_t bigTailDataNum;    // 大核尾批元素数
    uint32_t tailBlockNum;      // 大核个数（=余数）
};
```

## 4. Host 侧泛化 Tiling 函数公式模板（03.04 4.1）

```cpp
auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
uint32_t coreNum = ascendcPlatform.GetCoreNum();              // 可用核数
uint32_t inputNum = context->GetInputShape(0)->GetStorageShape().GetShapeSize(); // 元素数
uint32_t typeLength = 0;
ge::TypeUtils::GetDataTypeLength(context->GetInputDesc(0)->GetDataType(), typeLength);
uint32_t inputLength = inputNum * typeLength;                 // 总字节数

const uint32_t BLOCK_SIZE = 32;
uint32_t inputLengthAlign32 = ((inputLength + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
coreNum = std::min(coreNum, inputLengthAlign32 / BLOCK_SIZE); // 数据量不足则少启核
coreNum = std::max(coreNum, 1u);                              // 至少1核
uint32_t everyCoreInputBlockNum = inputLengthAlign32 / BLOCK_SIZE / coreNum;
uint32_t tailBlockNum = (inputLengthAlign32 / BLOCK_SIZE) % coreNum; // 余数→大核个数
context->SetBlockDim(coreNum);

uint64_t ubSize;  ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
uint32_t ubDataNumber = 3;   // Add: x/y/z 三块同驻；依算子输入输出数而定
uint32_t tileBlockNum = (ubSize / BLOCK_SIZE) / ubDataNumber;
uint32_t tileDataNum = (tileBlockNum * BLOCK_SIZE) / typeLength; // 单批元素数

// 小核（everyCore 块）
uint32_t smallCoreDataNum = everyCoreInputBlockNum * BLOCK_SIZE / typeLength;
uint32_t smallTileNum = everyCoreInputBlockNum / tileBlockNum;
uint32_t finalSmallTileNum = (everyCoreInputBlockNum % tileBlockNum) == 0 ? smallTileNum : smallTileNum + 1;
uint32_t smallTailDataNum = smallCoreDataNum - tileDataNum * smallTileNum;
smallTailDataNum = smallTailDataNum == 0 ? tileDataNum : smallTailDataNum;
// 大核（+1 块）：复制上面逻辑但 everyCoreInputBlockNum+1
everyCoreInputBlockNum += 1;   // big 系列公式与 small 相同
// ... 填 tiling 字段、return GRAPH_SUCCESS
```

## 5. Kernel 侧消费泛化 Tiling（03.04 4.2 关键）

- Init 判断大/小核：`blockIdx < tailBlockNum` → 大核参数；否则小核参数。
- **GM 偏移以"大核元素数"为单位累计**：初始 `globalBufferIndex = bigCoreDataNum * blockIdx`；小核修正 `globalBufferIndex -= (bigCoreDataNum - smallCoreDataNum) * (blockIdx - tailBlockNum)`。
- `SetGlobalBuffer((__gm__ T*)ptr + globalBufferIndex, coreDataNum)`。
- UB 按 **tileDataNum（单批最大）** 分配：`pipe.InitBuffer(q, BUFFER_NUM, tileDataNum*sizeof(T))`（3.4 场景 BUFFER_NUM=1）。
- Process 循环 `tileNum` 次；`if (i == tileNum-1) processDataNum = tailDataNum;` 再 CopyIn/Compute/CopyOut 用 processDataNum。
- 宏取数入口：
```cpp
REGISTER_TILING_DEFAULT(TilingDataTemplate);            // 注册标准C++ TilingData
GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, tiling_data, tiling); // 解析到结构体变量
```

## 6. 动态 shape 工程化 Kernel（03.02 6.4-7）

- 算子工程核函数带 `workspace`、`tiling` 两个额外 GM 参数：`add_custom_template(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling)`。
- 用 `GET_TILING_DATA...` 后以 `tiling_data.totalLength/tileNum` 动态 Init（对比：Kernel 直调模式用固定常量与值传参）。
- Process 循环 `tileNum * BUFFER_NUM`（开双缓冲时）或 `tileNum`（不开时）。CopyIn/Compute/CopyOut 与独立核函数一致。
- 例子：BUFFER_NUM=1/QUEUE_DEPTH=1（03.02、03.04 未开双缓冲）；BUFFER_NUM=2 见 02 章。

## 7. Workspace 与系统 workspace（03.05 2）

- workspace = GM 上一块内存，分**系统 workspace**（Ascend C API 内部需要的缓存，大小经 `platform_ascendc::PlatformAscendC(...).GetLibApiWorkSpaceSize()` 获取，如 Matmul 高阶 API）与**用户 workspace**（算子自行换缓存/跨核暂存时才申请）。
- Host 申请写法：
```cpp
size_t *currentWorkspace = context->GetWorkspaceSizes(1);  // 入参=块数，当前限制 1 块
currentWorkspace[0] = usrSize + sysWorkspaceSize;          // 总大小，框架自动申请 GM 内存
```
- Kernel 使用：把 workspace 当 GM 指针 `AscendC::GlobalTensor<float> tmpGm; tmpGm.SetGlobalBuffer((__gm__ float*)workspace);` 即可 DataCopy 搬入搬出。
- **代价提示**：workspace 走 GM，存取都要搬运 → 性能差；算子内临时数据优先用 TBuf（Local Memory）。

## 8. 其余工程要点速记

- Host 头文件（泛化/平台能力）：`register/op_def_registry.h`、`../op_kernel/xxx_tiling.h`、`graph/utils/type_utils.h`、`tiling/platform/platform_ascendc.h`。
- 取输入元素数两种写法：`GetInputShape(0)->GetOriginShape().GetShapeSize()` 与 `GetInputShape(0)->GetStorageShape().GetShapeSize()`（动态 shape 建议 StorageShape）。
- `context->SetBlockDim(n)` 设核数；`context->GetTilingData<T>()` 拿结构体指针填字段；`GetAttrs()->GetFloat(0)` 取属性。

## 9. TilingKey（多分支 kernel 选择，03.05 5.1）

- Host 侧 `context->SetTilingKey(1/2)`；Kernel 侧用宏判断分支：
```cpp
__global__ __aicore__ void xx(GM_ADDR ..., GM_ADDR workspace, GM_ADDR tiling) {
    if (TILING_KEY_IS(1)) { ProcessA(); } else if (TILING_KEY_IS(2)) { ProcessB(); }
}
```
- 缺点：数字难以记忆与维护 → 官方推荐"模板化算子"替代（见下）。

## 10. 模板化算子 / 模板化 Tiling（03.05 5.2、6.3-6.6）

- **思想**：把数据类型/切分策略等编译期确定的选择，写成模板参数表，由框架按 Host 选择的参数组合实例化多个 kernel 二进制并自动调度，取代手写 TilingKey 数字分支。
- 三个文件协同：
  1. **模板定义文件** `op_kernel/tiling_key_<op>.h`：`#include "ascendc/host_api/tiling/template_argument.h"`，用 `ASCENDC_TPL_ARGS_DECL(<OpType>, ...)` 声明各模板参数（DATATYPE_DECL/UINT_DECL/BOOL_DECL），用 `ASCENDC_TPL_SEL( ASCENDC_TPL_ARGS_SEL(...) )` 限定合法组合。文件名推荐 `tiling_key_+算子名.h`，可自定义。
  2. **Host**：`ASCENDC_TPL_SEL_PARAM(context, D_T_X, D_T_Y, TILE_NUM, IS_SPLIT)` 传当前实际参数（数据类型从 GetInputDesc(0)->GetDataType() 强转 int）。
  3. **Kernel**：核函数写成函数模板 `template <typename D_T_X, typename D_T_Y, int TILE_NUM, int IS_SPLIT> __global__ __aicore__ void xx(...)`，**不加 extern "C"**；内部 `if constexpr` 或普通 if 按模板参数选 Process1/Process2 等逻辑。
- 类型映射注意：模板参 D_T_X 声明 `C_DT_FLOAT/C_DT_FLOAT16`；在 Host 侧 ge::DataType 与模板参直接按 int 对应。
- msopgen 的 `-lan cpp` 输出工程默认 kernel 用 DTYPE_X/Y/Z 宏（继承模板能力）：工程化 Add 例 kernel 入口 `KernelAdd<DTYPE_X, DTYPE_Y, DTYPE_Z> op;`（03.02 src 实测）。

## 11. TBuf（核内临时缓冲，03.05 提及，官方模板样例）

- TBuf 用于任务内**临时计算结果**（不跨任务/不出队）。初始化 `pipe.InitBuffer(tbuf, byteSize)`，取用 `AscendC::LocalTensor<T> t = tbuf.Get<T>();`。
- 特性：只做计算，不能 EnQue/DeQue（与 TQue 区别）。典型用途：多步中间结果、向量指令序列的中间 buffer。
- 原则：临时小数据优先 TBuf（Local）而非 workspace（GM，需搬运且慢）。

## 12. 属性（Attr）怎么参与 Tiling（03.05 6.1/6.5）

- 算子原型 JSON `attr` 段声明（name/type/param_type/default_value），例：`{"name":"min","type":"float","param_type":"optional","default_value":0}`。
- Host Tiling 取属性并放进 TilingData → Kernel 读：`float min = *context->GetAttrs()->GetFloat(0); tiling->min = min;`，Tiling 结构体加同名字段；aclnn 第一段接口会带上属性形参（`aclnnClampGetWorkspaceSize(x, min, y,...)`）。
- 属性类型 API：`this->Attr("min").Float();`（原型注册 OpDef 内）。
