# 主题⑤ 常用 API 参考速查

> API 名称/签名/约束以官方教程与工程源码为准（锚点标注章节/源码文件）。持续扩充中。

## 0. 头文件与命名空间

```cpp
#include "acl/acl.h"          // Host 侧（02 章 Kernel 直调）；aclnn 场景用 aclnn/ 系列
#include "kernel_operator.h"  // Kernel 侧；AscendC:: 命名空间
#include "kernel_operator_dump_tensor_intf_impl.h" // DumpTensor（03 章工程示例已出现）
```
- aclnn 调用程序固定头文件（03.03）：`aclnn/aclnn_base.h`、`aclnn/acl_meta.h`、`acl/acl_rt.h`、`aclnn_<op>.h`（部署后生成）。
- 工程化算子 Host 头文件：`register/op_def_registry.h`、`../op_kernel/xxx_tiling.h`、`tiling/platform/platform_ascendc.h`、模板算子再加 `../op_kernel/tiling_key_xxx.h`。

## 1. Kernel 环境/核信息

| API | 说明 | 锚点 |
|---|---|---|
| `AscendC::GetBlockIdx()` / `GetBlockNum()` | 当前核 ID / 总核数 | 02.02 等 |
| `AscendC::printf(fmt, ...)` | Device 侧打印（`"Core %ld executed %d times..."`）| 02.02、03.02/03.05 |

## 2. GlobalTensor/LocalTensor 与队列内存

| API | 签名要点 |
|---|---|
| `xGm.SetGlobalBuffer((__gm__ T*)addr, length)` | GM 张量绑定；跨核切分地址=基址+blockLength*GetBlockIdx() |
| `queue.AllocTensor<T>()` / `queue.FreeTensor(t)` | 申请/归还队列内存块 |
| `queue.EnQue(t)` / `queue.EnQue<T>(t)` | 数据就绪（CopyIn/Compute 后）|
| `queue.DeQue<T>()` | 取数据（Compute/CopyOut 前）|
| `pipe.InitBuffer(queue, bufNum, lenBytes)` | 队列内存；bufNum=2 双缓冲 |
| `pipe.InitBuffer(tbuf, lenBytes)` / `tbuf.Get<T>()` | 任务内临时内存（不进出队）|

## 3. 数据搬运 DataCopy 系列（02.04/03.05）

- `AscendC::DataCopy(dstLocal, srcGm[offset], len)`：GM→Local（CopyIn）
- `AscendC::DataCopy(dstGm[offset], srcLocal, len)`：Local→GM（CopyOut/写回 workspace）
- **LocalTensor ↔ GlobalTensor(workspace) 也走 DataCopy**（03.05 Compute2：`DataCopy(tmpGm, tmpLocal, len)` 写、`DataCopy(tmpLocal, tmpGm, len)` 读）。
- 有"带参版" DataCopyParams（GM/Local 跨 stride 拷贝）在后续章补充；非连续/非对齐场景用 DataCopyPad（07 章等）。

## 4. Vector 计算指令（03.05 实测签名）

| API | 签名 | 说明 |
|---|---|---|
| `Add(dst, src0, src1, calCount)` | LocalTensor | 逐元素加 |
| `Maxs(dst, src, scalarVal, calCount)` | dst,src Local；scalar 数值 | 逐元素与标量取大（Clamp 用）|
| `Cast(dst, src, RoundMode::CAST_RINT, calCount)` | 数据类型转换 | int32→float 等 |
| Sub/Mul/Mins/Duplicate… | 同族 | 二目/单目/标量运算 |
- 高级参数（mask/repeat/stride）待 API 文档核实后补。

## 5. 跨指令队列显式同步（03.05 Compute2 唯一官方示例）

用于"UB→workspace(GM) 再读回 UB"时强制顺序（写回走 MTE3，读回走 MTE2）：
```cpp
int32_t e = static_cast<int32_t>(GetTPipePtr()->FetchEventID(AscendC::HardEvent::MTE3_MTE2));
AscendC::SetFlag<AscendC::HardEvent::MTE3_MTE2>(e);   // 设标志
AscendC::WaitFlag<AscendC::HardEvent::MTE3_MTE2>(e);   // 等待
// 搬运顺序：先 MTE3 写 GM 完成，再 MTE2 从 GM 读回
int32_t e2 = ...FetchEventID(AscendC::HardEvent::MTE2_V); SetFlag/WaitFlag(...MTE2_V);
```
HardEvent 枚举含 MTE3_MTE2、MTE2_V 等（队列间依赖）；EventID 由 TPipe 分发（每个事件同步点唯一 ID）。

## 6. Kernel 入口的 Tiling 解析宏（工程化算子）

```cpp
__global__ __aicore__ void add_custom_template(GM_ADDR x, GM_ADDR y, GM_ADDR z, GM_ADDR workspace, GM_ADDR tiling) {
    REGISTER_TILING_DEFAULT(TilingDataTemplate);               // 注册 Tiling 结构体
    GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, tiling_data, tiling); // 解析
    KernelAdd<DTYPE_X, DTYPE_Y, DTYPE_Z> op;                  // DTYPE_* 为框架按配置注入
    op.Init(...);
    op.Process();
}
```
- 核函数签名统一 5 参：输入…+`GM_ADDR workspace`+`GM_ADDR tiling`。
- DTYPE_X/Y/Z：msopgen 模板化算子中框架根据类型配置注入的宏；Kernel 类模板 `<class dtypeX,...>`。

## 7. 模板化算子（TilingKey 替代方案）API（03.05 5.2/6.3）

```cpp
#include "ascendc/host_api/tiling/template_argument.h"  // 模板定位文件必引头文件
// op_kernel/tiling_key_<op>.h 内定义：
ASCENDC_TPL_ARGS_DECL(Clamp,                            // 算子名
    ASCENDC_TPL_DATATYPE_DECL(D_T_X, C_DT_INT32, C_DT_FLOAT, ASCENDC_TPL_INPUT(0)),
    ASCENDC_TPL_DATATYPE_DECL(D_T_Y, C_DT_INT32, C_DT_FLOAT, ASCENDC_TPL_OUTPUT(0)),
    ASCENDC_TPL_UINT_DECL(TILE_NUM, ASCENDC_TPL_8_BW, ASCENDC_TPL_UI_MIX, 2, 0, 2, 3, 5, 10, 12, 13, 9, 8),
    ASCENDC_TPL_BOOL_DECL(IS_SPLIT, 0, 1),
);
// 合法组合限定（数据类型一致才合法）：
ASCENDC_TPL_SEL(
    ASCENDC_TPL_ARGS_SEL(ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_INT32), ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_INT32)),
    ASCENDC_TPL_ARGS_SEL(ASCENDC_TPL_DATATYPE_SEL(D_T_X, C_DT_FLOAT), ASCENDC_TPL_DATATYPE_SEL(D_T_Y, C_DT_FLOAT)),
);
```
- Host Tiling 里用 `ASCENDC_TPL_SEL_PARAM(context, D_T_X, D_T_Y, ...)` 自动算好并设置 TilingKey。
- Kernel 把核函数写成模板：`template <typename D_T_X, typename D_T_Y> __global__ __aicore__ void clamp(...)`（**去掉 extern "C"**），内部用 `if constexpr(std::is_same_v<D_T_X,float>)` 选分支。
- 传统方式对照：`context->SetTilingKey(1)` + kernel `if (TILING_KEY_IS(1))`；模板化方式免记忆数字。

## 8. Host 侧 Tiling API（TilingContext）

| API | 说明 |
|---|---|
| `context->GetInputShape(0)->GetOriginShape().GetShapeSize()` | 输入元素数 |
| `context->GetInputDesc(i)->GetDataType()` / `GetOutputDesc(i)->...` | 数据类型（强转 int 供模板参）|
| `context->GetAttrs()->GetFloat(0)` | 取 float 属性（带 `*` 解引用）|
| `context->SetBlockDim(n)` | 启动核数 |
| `context->GetTilingData<T>()` | 取 Tiling 结构体指针填字段（TilingData 模板）|
| `platform_ascendc::PlatformAscendC(info).GetCoreNum()` / `.GetLibApiWorkSpaceSize()` / `.GetCoreMemSize(CoreMemType::UB, ubSize)` | 平台能力查询 |
| `context->GetWorkspaceSizes(1)` | 返回数组，`arr[0]=usr+sys` 申请 workspace |
| `context->SetTilingKey(k)` | 设置传统 TilingKey |

## 9. aclnn 两段式调用（03.03/03.05 实测）

```cpp
// 第一段：算 workspace + 创建执行器
aclnnStatus aclnnClampGetWorkspaceSize(aclTensor* x, double min, aclTensor* y, uint64_t* workspaceSize, aclOpExecutor** executor);
// 第二段：执行
aclnnStatus aclnnClamp(void* workspace, uint64_t workspaceSize, aclOpExecutor* executor, aclrtStream stream);
// 流程：aclrtMalloc(workspaceSize) → aclnnXxx → aclrtSynchronizeStream → D2H → aclDestroyTensor/aclrtFree...
```
- aclTensor 创建：`aclCreateTensor(shape.data(), dims, aclDataType::ACL_FLOAT16/INT32, nullptr, 0, ACL_FORMAT_ND, shape.data(), dims, deviceAddr)`。
- pybind 封装内用 `EXEC_NPU_CMD(aclnnAddCustomTemplate, x, y, z)`（pytorch_npu_helper.hpp）一步完成两段调用；模块 `PYBIND11_MODULE(TORCH_EXTENSION_NAME, m){ m.def(...); }`。

## 10. 编译/构建辅助要点

- g++ 直编调用程序（aclnn 场景）：
  `g++ -I$ASCEND_TOOLKIT_HOME/include -I$HOME/vendors/customize/op_api/include -L.../lib64 -L.../op_api/lib xx.cpp -lcust_opapi -lnnopbase -lacl_rt -o xx`
- Python 侧 pybind 编译：NpuExtension + BuildExtension（见 03.03 setup.py 模式）。
- Kernel 直调 .asc 编译：`bisheng xx.asc --npu-arch=dav-2201 -o demo` 或 CMake find_package(ASC)。

## 11. Matmul 高阶 API（Kernel 侧，04 章 / answer 源码实测）

### 11.1 类型包装 MatmulType
`typedef MatmulType<TPosition::GM, CubeFormat::ND, half> aType;` —— 位置+格式+数据类型三元组。Bias 类型同理；**C 的位置声明为 VECIN 即实现 CV 融合**。

### 11.2 对象操作序列
```cpp
Matmul<aType, bType, cType, biasType> matmulObj;   // 类成员
REGIST_MATMUL_OBJ(&pipe, GetSysWorkSpacePtr(), matmulObj, &tilingData.cubeTilingData); // Init 内注册
matmulObj.SetTensorA(aGlobal);                      // 已按核偏移切片的 GlobalTensor
matmulObj.SetTensorB(bGlobal);
if (isBias) matmulObj.SetBias(biasGlobal);
matmulObj.Iterate();          // 核内单步推进（K 累加）；配 GetTensorC(gmC) 取结果
matmulObj.IterateAll(cGlobal); // 或一次性算完 singleCore 块直写 GM
matmulObj.End();
```
- `GetTensorC<false>(gmC)`：C 结果搬出到 GM；`GetTensorC<true>(ubLocal, false, true)`：取到 UB LocalTensor 供 Vector 指令继续（CV 融合，`GetTensorC<true>` 模板参数=同步等待 Cube 完成）。
- Matmul 需要**系统 workspace**（Matmul API 内部用）；Host 侧大小取 `ascendcPlatform.GetLibApiWorkSpaceSize()`。
- Tiling 数据的类型是 `AscendC::tiling::TCubeTiling`（含 usedCoreNum/singleCore* /base* /depthA1B1/stepM/N…），Host 由 `matmul_tiling` 命名空间的 `MultiCoreMatmulTiling` 对象生成（matmul_abs.cpp 已摘录）。头：`#include "tiling/tiling_api.h"; using namespace matmul_tiling;`。
- 核内 localMemSize 预算：`sizeof(half)*depthA1B1*(baseM+baseN)*...`（立方缓冲，具体见 04.03 5.3.3 公式）。

### 11.3 DataCopyParams（子矩阵带 stride 搬运，05.04 实测）
```cpp
AscendC::DataCopyParams p = {blockCount, blockLen, srcStride, dstStride};
// 单位：DataBlock=32B；blockLen/stride 都以 32B 计
AscendC::DataCopy(dstGlobalTensor[startOffset], srcLocalTensor, p);
// 例：把 UB 内 baseM×baseN 写回 GM 大矩阵 [singleCoreM×N]：
// blockCount=baseM; blockLen=baseN*sizeof(cType)/32; srcStride=0; dstStride=(N-baseN)*sizeof(cType)/32;
```

### 11.4 融合场景同步细节
- `Iterate<true>`：Cube 与 Vector 链式计算需同步；VECIN 位置 Matmul 对象内部处理 C 数据就绪。
- 05.04 的 while 循环中每轮迭代：AllocTensor(C out queue)→GetTensorC<true>→Abs→EnQue→CopyOut（下一轮 Cube 与上一轮 Vector 天然流水）。
