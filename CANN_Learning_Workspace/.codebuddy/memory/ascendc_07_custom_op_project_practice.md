# Ascend C 自定义算子完整工程实战经验（AscTry A+B=C 端到端）

> 本文件是**独立共享经验库**（与 01~06 课程主题文件并列的第七个主题，按用户要求单独成文、不并入其它记忆文件）。
> 内容来自真实项目 `Projects\out\AscTry\`（CANN 9.0.0 + 云上 910B 系列环境）从空壳工程补全 → 云端构建 → aclnn 调用报 161001 → 定位 soc 不匹配的全过程。
> 编写日期：2026-09-05；工程代码快照：host/kernel 已补齐并静态核验，`cp` 改名 workaround 为用户下一步待验证项（官方 issue 作者已验证 `test pass`）。
> 适用范围：任何"新式 OpDef 注册 + autogen aclnn + RUN 包部署"的 Ascend C 自定义算子工程（A2/A3、CANN 9.0.0+）。

---

## 0. 一分钟速查

- **算子名 → 接口名**：`OpDef` 类名 `AscTry` → autogen 生成 `aclnnAscTryGetWorkspaceSize(a, b, out, &workspaceSize, &executor)` 与 `aclnnAscTry(workspace, workspaceSize, executor, stream)`（参数名取 OpDef 的 Input/Output 名，小写）。
- **GetWorkspaceSize 报 161001 的第一嫌疑**：kernel 安装目录的 soc 名 ≠ 设备真实 socVersion。日志会打印 `The binary_info_config.json of socVersion [xxx] does not support opType [YYY]`，照抄日志里的 socVersion 处理即可（详见 §8）。
- **必须在运行前** `source $HOME/vendors/customize/bin/set_env.bash`，否则可执行文件加载不到 `libcust_opapi.so`（现象是"shared object file: No such file"，误以为程序没起，连 161001 都不会有）。
- **真机上 soc 是否匹配无法用 npu-smi 判断**（910B 系都显示 `Ascend910`），必须以运行时日志中 `socVersion [xxx]` 为准。

---

## 1. 工程全景：目录结构与构建/部署链路

工程根：`Projects\out\AscTry\`（注意带 `out\`，此工程是 msopgen 生成的 CANN 9.0.0 新式工程，双目录 + framework + RUN 包打包）。

```
AscTry/
├── CMakeLists.txt          # 顶层：聚合 op_host/op_kernel/framework，出 RUN 包
├── CMakePresets.json       # 关键：ASCEND_COMPUTE_UNIT="ascend910b"（soc 编译目标）
├── build.sh                # cmake preset + build --target binary package
├── op_host/
│   ├── asc_try.cpp         # OpDef 注册 + InferShape/InferDataType + TilingFunc（tiling 三件套之一）
│   └── CMakeLists.txt
├── op_kernel/
│   ├── asc_try_tiling.h    # 共享 Tiling 结构（host 通过 #include ../op_kernel/asc_try_tiling.h 复用同一份）
│   ├── asc_try.cpp         # 算子实现（模板类 KernelAscTry<T> + 核函数 asc_try）
│   └── CMakeLists.txt      # npu_op_kernel_sources/npu_op_kernel_library(ascendc_kernels TILING_LIBRARY cust_optiling)
├── framework/
│   ├── CMakeLists.txt
│   └── tf_plugin/
│       └── tensorflow_asc_try_plugin.cc   # REGISTER_CUSTOM_OP("AscTry") 向 GE 注册
└── test/                   # 自建测试套件（非 msopgen 产物），见 §6
    ├── case_config.h  aclnn_test.cpp  gen_data.py  verify_result.py  run.sh
```

### 1.1 顶层/关键配置文件要点

- `build.sh`：解析 `CMakePresets.json` → `cmake --preset=default` → `cmake --build build_out --target binary package`。产物在 `build_out/`：
  - `AscTry_<hash>.o`（kernel 编译产物，hash 由算子名/内容决定）
  - `custom_opp_ubuntu_aarch64.run`（RUN 安装包，路径含 `ubuntu_aarch64` 说明是 aarch64 平台包）
- `CMakePresets.json` 关键变量：
  - `ASCEND_COMPUTE_UNIT = "ascend910b"` ← **决定 kernel 产物安装后的 soc 目录名**（§8 的主角）
  - `ASCEND_CANN_PACKAGE_PATH = /home/developer/Ascend/cann-9.0.0`（云端 CANN 9.0.0 路径）
  - `vendor_name = "customize"` ← 安装后出现在 `$HOME/vendors/customize/`
  - `ENABLE_TEST=True`、`ENABLE_SOURCE_PACKAGE/ENABLE_BINARY_PACKAGE=True`
- `op_kernel/CMakeLists.txt`：
  ```cmake
  if ("${CMAKE_BUILD_TYPE}x" STREQUAL "Debugx")
      npu_op_kernel_options(ascendc_kernels ALL OPTIONS -g -O0)
  endif()
  npu_op_kernel_sources(ascendc_kernels KERNEL_DIR ./)
  npu_op_kernel_library(ascendc_kernels SRC_BASE ${CMAKE_CURRENT_SOURCE_DIR}/ TILING_LIBRARY cust_optiling)
  npu_op_package_add(${package_name} LIBRARY ascendc_kernels)
  ```
- `framework/tf_plugin/tensorflow_asc_try_plugin.cc`：向 GE 注册单算子
  ```cpp
  REGISTER_CUSTOM_OP("AscTry")
      .FrameworkType(TENSORFLOW)
      .OriginOpType("AscTry")
      .ParseParamsByOperatorFn(AutoMappingByOpFn);
  ```

### 1.2 云端构建 + 安装命令（CANN Lab）

```bash
cd /mnt/workspace/out/AscTry
bash build.sh                                   # 编译全绿 → build_out/custom_opp_ubuntu_aarch64.run
./build_out/custom_opp_ubuntu_aarch64.run --install-path=$HOME   # 装到 $HOME/vendors/customize/
```

安装后 `$HOME/vendors/customize/` 下的关键交付物：

```
bin/set_env.bash                                 # 运行前必须 source（见 §7）
op_api/include/aclnn_asc_try.h                   # autogen 生成的 aclnn 接口头（aclnn_test 包含它）
op_api/lib/libcust_opapi.so                      # host 侧算子库（含 autogen aclnn 实现）
op_impl/ai_core/tbe/
├── config/ascend910b/aic-ascend910b-ops-info.json   # 算子属性/原型注册（按 soc 分层）
├── customize_impl/dynamic/asc_try.{cpp,py,tiling.h} # tiling 库源码（host 侧 optiling）
├── kernel/ascend910b/AscTry_<hash>.{json,o}         # 设备侧 kernel 二进制 + 描述
└── kernel/config/ascend910b/{asc_try.json,binary_info_config.json}  # ← NNOP 注册检索入口
```

---

## 2. Host 侧三件套（op_host/asc_try.cpp）

新式（CANN 9.0.0）op_host 写法要点：OpDef 类中链式声明 IO，然后 `OP_ADD(AscTry);` 完成注册；TilingFunc 是 `gert::TilingContext*` 的静态回调。**同代写法以官方 `03_intermediate_vector_operator_development/src/custom_op_04/` 为准**（03.02 的 custom_op 是旧代 TPosition 写法，勿混用）。

### 2.1 OpDef 注册 + 原型推导 + tiling 绑定

```cpp
namespace ge {
static ge::graphStatus InferShape(gert::InferShapeContext* context)   // 输出 shape = 输入 shape
{ const gert::Shape* x1 = context->GetInputShape(0);
  gert::Shape* y = context->GetOutputShape(0); *y = *x1; return GRAPH_SUCCESS; }
static ge::graphStatus InferDataType(gert::InferDataTypeContext* context)  // 输出 dtype = 输入 dtype
{ context->SetOutputDataType(0, context->GetInputDataType(0)); return ge::GRAPH_SUCCESS; }
}

namespace ops {
class AscTry : public OpDef {
public:
    explicit AscTry(const char* name) : OpDef(name) {
        this->Input("A").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->Input("B").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->Output("C").ParamType(REQUIRED).DataType({ge::DT_FLOAT}).Format({ge::FORMAT_ND})
            .UnknownShapeFormat({ge::FORMAT_ND});
        this->SetInferShape(ge::InferShape).SetInferDataType(ge::InferDataType);
        this->AICore().SetTiling(optiling::TilingFunc);
        this->AICore().AddConfig("ascend910b");   // soc 名，见 §8 注意事项
    }
};
OP_ADD(AscTry);   // 类名 AscTry → autogen aclnn 接口名 aclnnAscTry...
}
```

经验：
- 类名与 `OP_ADD` 决定 autogen 接口名与 GE 的 op type；改名要同步改 framework/tf_plugin 与 test include。
- `AddConfig("ascend910b")` 与 CMakePresets 的 `ASCEND_COMPUTE_UNIT` 通常是同一个 soc 名。
- 文件 include 顺序参考：`../op_kernel/asc_try_tiling.h`（tiling struct 在 kernel 侧目录，host include 过来保证两处同一份）、`register/op_def_registry.h`、`graph/utils/type_utils.h`、`tiling/platform/platform_ascendc.h`、`<algorithm>`。

### 2.2 TilingFunc 完整逻辑（逐段注释级拆解）

```cpp
namespace optiling {
static ge::graphStatus TilingFunc(gert::TilingContext* context)
{
    auto ascendcPlatform = platform_ascendc::PlatformAscendC(context->GetPlatformInfo());
    uint32_t coreNum = ascendcPlatform.GetCoreNum();          // 平台可用核数上限

    uint32_t inputNum = context->GetInputShape(0)->GetStorageShape().GetShapeSize();
    uint32_t typeLength = 0;
    ge::TypeUtils::GetDataTypeLength(context->GetInputDesc(0)->GetDataType(), typeLength);
    uint32_t inputLength = inputNum * typeLength;             // 输入总字节数（元素×类型长）
    const uint32_t BLOCK_SIZE = 32;                           // 搬运/对齐基本单位 32B

    // ① 总字节向上 32B 对齐 → 折成 32B 块数
    uint32_t inputLengthAlign32 = ((inputLength + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
    coreNum = std::min(coreNum, inputLengthAlign32 / BLOCK_SIZE);   // 块太少则少开核
    coreNum = std::max(coreNum, static_cast<uint32_t>(1));          // 至少 1 核，防除零
    uint32_t everyCoreInputBlockNum = inputLengthAlign32 / BLOCK_SIZE / coreNum; // 每核平均 32B 块
    uint32_t tailBlockNum = (inputLengthAlign32 / BLOCK_SIZE) % coreNum;  // 余数块 → 前 N 个核各 +1 块（大核）
    context->SetBlockDim(coreNum);                             // ← 必须：写回启动核数

    // ② 核内切批：UB 里同时驻 A/B/C 三路（ubDataNumber=3），双缓冲按 BUFFER_NUM=2 预留
    uint64_t ubSize;
    ascendcPlatform.GetCoreMemSize(platform_ascendc::CoreMemType::UB, ubSize);
    const uint32_t BUFFER_NUM = 2, ubDataNumber = 3;
    uint32_t tileBlockNum = (ubSize / BLOCK_SIZE / BUFFER_NUM) / ubDataNumber; // 单批 32B 块数
    uint32_t tileDataNum = (tileBlockNum * BLOCK_SIZE) / typeLength;           // 单批元素数

    // ③ 小核参数：total = everyCoreInputBlockNum × 32B；切批数 = ceil(块数 / 每批块数)
    uint32_t smallCoreDataNum = everyCoreInputBlockNum * BLOCK_SIZE / typeLength;
    uint32_t smallTileNum = everyCoreInputBlockNum / tileBlockNum;
    uint32_t finalSmallTileNum = (everyCoreInputBlockNum % tileBlockNum) == 0 ? smallTileNum : smallTileNum + 1;
    uint32_t smallTailDataNum = smallCoreDataNum - tileDataNum * smallTileNum;
    smallTailDataNum = smallTailDataNum == 0 ? tileDataNum : smallTailDataNum;  // 整除时尾批=整批，避免 0

    // ④ 大核 = 小核基础上块数 +1（即 total 多 32B），参数同法
    everyCoreInputBlockNum += 1;
    uint32_t bigCoreDataNum = everyCoreInputBlockNum * BLOCK_SIZE / typeLength;
    uint32_t bigTileNum = everyCoreInputBlockNum / tileBlockNum;
    uint32_t finalBigTileNum = (everyCoreInputBlockNum % tileBlockNum) == 0 ? bigTileNum : bigTileNum + 1;
    uint32_t bigTailDataNum = bigCoreDataNum - tileDataNum * bigTileNum;
    bigTailDataNum = bigTailDataNum == 0 ? tileDataNum : bigTailDataNum;

    AscTryTilingData *tiling = context->GetTilingData<AscTryTilingData>();
    tiling->smallCoreDataNum = smallCoreDataNum;   tiling->bigCoreDataNum = bigCoreDataNum;
    tiling->tileDataNum = tileDataNum;
    tiling->smallTailDataNum = smallTailDataNum;   tiling->bigTailDataNum = bigTailDataNum;
    tiling->finalSmallTileNum = finalSmallTileNum; tiling->finalBigTileNum = finalBigTileNum;
    tiling->tailBlockNum = tailBlockNum;
    return ge::GRAPH_SUCCESS;
}
}
```

数值检查经验（便于人肉验算，如 {7,97} float，679 元素 / 2716B / 对齐 2720B / 85 块 / 20 核）：
- `everyCoreInputBlockNum=85/20=4`、`tailBlockNum=85%20=5` → 5 个大核每核 5 块(40 元素)，15 个小核每核 4 块(32 元素)，合计 5×40+15×32=680 = 85×8 ✓。
- UB 预算验算：910B UB≈192KB=196608B → `tileBlockNum=(196608/32/2)/3=1024` → `tileDataNum=8192`，远大于单核数据量 → 每核只有 1 批（final*TileNum=1，尾批=全量）。

### 2.3 Tiling 结构体（op_kernel/asc_try_tiling.h，host/kernel 共用）

```cpp
struct AscTryTilingData {
    uint32_t smallCoreDataNum;   // 小核(常规核)总元素数
    uint32_t bigCoreDataNum;     // 大核(多32B)总元素数
    uint32_t finalBigTileNum;    // 大核循环批次数
    uint32_t finalSmallTileNum;  // 小核循环批次数
    uint32_t tileDataNum;        // 单批标准元素数(UB预算决定)
    uint32_t smallTailDataNum;   // 小核尾批元素数
    uint32_t bigTailDataNum;     // 大核尾批元素数
    uint32_t tailBlockNum;       // 大核个数(=32B块总数对核数取余)
};
```

注意：新式工程 tiling struct 放在 op_kernel 目录，host 通过相对 include `../op_kernel/asc_try_tiling.h` 复用，避免两份拷贝不同步。字段必须全部 uint32（gert tiling 传递按 4B 对齐打包）。

---

## 3. Kernel 侧（op_kernel/asc_try.cpp）

### 3.1 模板类 + 双缓冲流水（框架要点）

- 常量：`BUFFER_NUM=2`（每队列缓冲数）、`QUEUE_DEPTH=2`。
- 队列位置：输入用 `TQue<QuePosition::VECIN, QUEUE_DEPTH>`，输出用 `TQue<QuePosition::VECOUT, QUEUE_DEPTH>`（新式 QuePosition，旧代用 TPosition）。
- 单缓冲区大小按 **整批 tileDataNum** 分配（`InitBuffer(inQueueA, BUFFER_NUM, tileDataNum*sizeof(T))`），尾批在循环末批把实际拷贝数降为 `tailDataNum`（缓冲容量仍按整批分配即足够）。
- 循环内范式固定：CopyIn（Alloc→DataCopy→EnQue）→ Compute（DeQue→Add→EnQue）→ CopyOut（DeQue→DataCopy→FreeTensor），各队列事件由 TPipe 自动管理。

```cpp
__aicore__ inline void Init(GM_ADDR a, GM_ADDR b, GM_ADDR c, /*8 个 tiling 参数，全部按值传入*/)
{
    uint32_t blockIdx = AscendC::GetBlockIdx();
    uint32_t globalBufferIndex = bigCoreDataNum * blockIdx;   // 先按"全大核"排布计算 GM 起点
    if (blockIdx < tailBlockNum) {        // 大核：前 tailBlockNum 个核
        coreDataNum = bigCoreDataNum;  tileNum = finalBigTileNum;  tailDataNum = bigTailDataNum;
    } else {                             // 小核
        coreDataNum = smallCoreDataNum;  tileNum = finalSmallTileNum;  tailDataNum = smallTailDataNum;
        // 小核排在所有大核之后，起点要扣掉中间"本该属大核"的差值
        globalBufferIndex -= (bigCoreDataNum - smallCoreDataNum) * (blockIdx - tailBlockNum);
    }
    aGm.SetGlobalBuffer((__gm__ T*)a + globalBufferIndex, coreDataNum);
    bGm.SetGlobalBuffer((__gm__ T*)b + globalBufferIndex, coreDataNum);
    cGm.SetGlobalBuffer((__gm__ T*)c + globalBufferIndex, coreDataNum);
    pipe.InitBuffer(... × 3, BUFFER_NUM, tileDataNum*sizeof(T));
}
```

**大小核 GM 偏移修正（高频易错，背下来）**：host 把数据按"前 tailBlockNum 个核每核多 1 个 32B 块"均分。kernel 的 GM 起始索引公式：
- 大核（blockIdx < tailBlockNum）：`off = bigCoreDataNum * blockIdx`
- 小核（blockIdx ≥ tailBlockNum）：`off = bigCoreDataNum * blockIdx − (bigCoreDataNum − smallCoreDataNum) × (blockIdx − tailBlockNum)`
- 每经过一个小核，相对全大核排布累计少 `big−small` 个元素（即 1 块=8×T 元素），与 host 端 tiling 分配严格一致。

### 3.2 Process 尾批处理

```cpp
this->processDataNum = this->tileDataNum;
for (int32_t i = 0; i < (int32_t)tileNum; ++i) {
    if (i == (int32_t)tileNum - 1) processDataNum = tailDataNum;  // 末批按尾批实际元素数
    CopyIn(i); Compute(i); CopyOut(i);
}
```

### 3.3 核函数签名与 tiling 解包

```cpp
extern "C" __global__ __aicore__ void asc_try(GM_ADDR A, GM_ADDR B, GM_ADDR C,
                                              GM_ADDR workspace, GM_ADDR tiling)
{
    REGISTER_TILING_DEFAULT(AscTryTilingData);
    GET_TILING_DATA_WITH_STRUCT(AscTryTilingData, tiling_data, tiling);
    KernelAscTry<float> op;
    op.Init(A, B, C, tiling_data.smallCoreDataNum, tiling_data.bigCoreDataNum, ...);  // 显式展开 8 字段
    op.Process();
}
```

经验：
- 形参顺序必须是 **输入 → 输出 → workspace → tiling**；`workspace` 本算子未用但保留占位。
- `REGISTER_TILING_DEFAULT` + `GET_TILING_DATA_WITH_STRUCT` 是一对宏，tiling 结构自动解包成 `tiling_data.<字段>`。
- kernel 编译产物 `.o` 文件名含内容 hash（如 `AscTry_86b4d5e856421b58e8ebf8c472ee770d.o`），每次改代码 hash 变化，属正常。

---

## 4. autogen aclnn 接口（新式工程自动生成）

- 构建时会自动从 OpDef 生成 `op_api/include/aclnn_asc_try.h`，**用户代码不能手写**该接口，只能按签名调用。
- 名称规则：类名 `AscTry` → `aclnnAscTryGetWorkspaceSize` / `aclnnAscTry`；参数为 OpDef 里 IO 名小写 `a/b/out`。
- 已确认签名（CANN 9.0.0，无 handle 新式）：
  ```cpp
  aclnnAscTryGetWorkspaceSize(const aclTensor* a, const aclTensor* b, const aclTensor* out,
                              uint64_t* workspaceSize, aclOpExecutor** executor);
  aclnnAscTry(void* workspaceAddr, uint64_t workspaceSize, aclOpExecutor* executor, aclrtStream stream);
  ```

---

## 5. aclnn 测试程序骨架（可直接复用/改成单输入输出）

参考文件 `Projects\out\AscTry\test\aclnn_test.cpp`。固定流程：

1. **Init**：`aclInit(nullptr)` → `aclrtSetDevice(0)` → `aclrtCreateStream(&stream)`（任一步失败即退出，输出 ERROR: <码>）。
2. **构造 aclTensor**（`CreateAclTensor`，官方姿势，勿改）：
   ```cpp
   auto size = GetShapeSize(shape) * sizeof(T);
   aclrtMalloc(&addr, size, ACL_MEM_MALLOC_HUGE_FIRST);
   aclrtMemcpy(addr, size, hostData.data(), size, ACL_MEMCPY_HOST_TO_DEVICE);
   *tensor = aclCreateTensor(shape.data(), shape.size(), dataType,
                             nullptr, 0,                      // strides 传 nullptr/0 = 连续 ND 布局
                             aclFormat::ACL_FORMAT_ND,
                             shape.data(), shape.size(),      // storageShape = 逻辑 shape
                             *addr);
   ```
   **不要把 shape 当 strides 传**；连续布局就 `nullptr, 0`。
3. **两段式调用**：
   ```cpp
   uint64_t workspaceSize = 0; aclOpExecutor* executor = nullptr;
   ret = aclnnAscTryGetWorkspaceSize(a, b, out, &workspaceSize, &executor);  // 报错 161001 就查 §8
   if (workspaceSize > 0) aclrtMalloc(&workspaceAddr, workspaceSize, ACL_MEM_MALLOC_HUGE_FIRST);
   ret = aclnnAscTry(workspaceAddr, workspaceSize, executor, stream);
   aclrtSynchronizeStream(stream);
   aclrtMemcpy(result.data(), ..., outAddr, ..., ACL_MEMCPY_DEVICE_TO_HOST);
   ```
4. **释放清单（漏一个就泄）**：每个 `aclDestroyTensor` → 每个 `aclrtFree`（含 workspace）→ `aclrtDestroyStream` → `aclrtResetDevice` → `aclFinalize`。
5. 头文件：`acl/acl.h` + 生成的 `aclnn_asc_try.h` + 自定义 `case_config.h`。

---

## 6. 测试数据工具链与 run.sh

```
test/
├── case_config.h    # 单一 shape 来源：#define CASE_SHAPE {7,97}（int64 数组）、CASE_ACL_DTYPE ACL_FLOAT、HostDataType float
├── gen_data.py      # 正则从 case_config.h 解析 CASE_SHAPE → 生成 inputA.bin/inputB.bin/golden.bin（golden=A+B，float32 struct.pack）
├── verify_result.py # np.isclose(atol=1e-4) 比对 output.bin 与 golden.bin，全过打印 TEST PASSED!
├── run.sh
└── aclnn_test.cpp   # §5 骨架
```

- 选 `{7,97}`=679 元素：非 32B 对齐（679×4=2716B，对齐后 2720B），能同时覆盖"大核/小核并存 + 尾块不整除 + 单核单批"路径，是比全对齐 shape 更好的回归用例。
- 换 shape 验证多批/多核均分：只改 `case_config.h` 的 `CASE_SHAPE` 后重跑 `bash run.sh`。
- `run.sh` 逻辑（注意顺序，全是踩过的坑）：
  ```bash
  python3 gen_data.py
  g++ -I"." -I"$ASCEND_TOOLKIT_HOME/include" -I"$HOME/vendors/customize/op_api/include" \
      -L"$ASCEND_TOOLKIT_HOME/lib64" -L"$HOME/vendors/customize/op_api/lib" \
      aclnn_test.cpp -lcust_opapi -lnnopbase -lacl_rt -o execute_op
  set +u
  source "$HOME/vendors/customize/bin/set_env.bash"    # 必须在运行前 source（§7）
  set -u
  ./execute_op .
  python3 verify_result.py
  ```

---

## 7. 云端运行环境经验

- `ASCEND_TOOLKIT_HOME`（或 Lab 预置的等价环境变量）指向 CANN 安装根（本案例 `/home/developer/Ascend/cann-9.0.0`），编译期需要；`build.sh` 还依赖 `ASCEND_HOME_PATH`（由 build.sh 自身从预设推导，无需手设）。
- **LD_LIBRARY_PATH 陷阱**：手编 `execute_op` 后直接 `./execute_op` 会报
  `error while loading shared libraries: libcust_opapi.so: cannot open shared object file`
  这不是程序问题，是没 source `$HOME/vendors/customize/bin/set_env.bash`（该脚本负责把 `op_api/lib` 加进 LD_LIBRARY_PATH 等）。用 run.sh 即可自动处理。
- source 脚本前注意 shell 的 `set -u` 兼容：脚本可能引用未定义变量，先 `set +u` 再 source、完事 `set -u`（run.sh 已示范）。

---

## 8. 大坑专节：GetWorkspaceSize 报 161001 —— socVersion 不匹配

### 8.1 现象

```
generated inputA.bin/inputB.bin/golden.bin, shape=(7, 97), dtype=float32
aclnnAscTryGetWorkspaceSize failed. ERROR: 161001
```

161001 出现在 **GetWorkspaceSize（纯 host 构图/注册阶段）**，kernel 根本还没执行。它不等同于某个具体语义——同样的码可能是参数校验、也可能是 GE/NNOP 单算子注册失败，**必须开日志看上下文**（§9），不要瞎改代码。

### 8.2 关键日志（定位决定性两行）

```
[ERROR] OP ... errno[161001] OpName:[AscTry_0] Get regInfo failed,
The binary_info_config.json of socVersion [ascend910_93] does not support opType [AscTry].
[ERROR] OP ... errno[161001] OpName:[AscTry_0] Check nnopExecutor != nullptr failed
```

另有两条辅助信息（WARNING 级，说明系统包也不带该文件，不是根因）：
```
Open jsonfile path for /home/developer/Ascend/cann-9.0.0/opp/static_kernel/ai_core/config/ascend910_93/binary_info_config.json failed.
Open jsonfile path for /home/developer/vendors/customize/op_impl/ai_core/tbe/kernel/config/ascend910_93/binary_info_config.json failed.
```

### 8.3 根因（课程环境已知问题）

- 设备真实 socVersion = **`ascend910_93`**（910B 系变体，runtime 经 `rtGetSocVersion` 上报；NNOP 只按这个名找算子配置目录）。
- 工程 CMakePresets 的 `ASCEND_COMPUTE_UNIT` 硬编码为 **`ascend910b`** → 编译安装后 kernel 落在 `.../kernel/{ascend910b, config/ascend910b}`。
- NNOP 检索的是 `.../kernel/config/ascend910_93/` → 找不到 → `AscTry` 算子 "not support" → regInfo 失败 → GetWorkspaceSize 返回 161001。

**无法用 npu-smi 预判**：910B 系列 npu-smi 一律显示 `Ascend910`（如 `npu-smi 25.5.5`，Name 列 `Ascend910`），不区分 B1/B2/B3/soc 细分，只能靠日志里 `socVersion [...]` 或 runtime API 确认。

### 8.4 官方验证过的修复（复制目录改名，无需重编）

```bash
cd $HOME/vendors/customize/op_impl/ai_core/tbe/kernel
cp -r ascend910b ascend910_93
cp -r config/ascend910b config/ascend910_93
source $HOME/vendors/customize/bin/set_env.bash
```

- `ascend910b` 编译的 `.o` 可直接在 `ascend910_93` 设备上运行（910B 系兼容），cann-learning-hub issue #390（同款 `AddCustomTemplate` 工程）作者按此法跑通 `test pass`。
- **复发条件**：重新 `bash build.sh` + 重新安装 RUN 包会清掉手工复制出的 `ascend910_93` 目录，需要重跑两条 `cp -r`。
- **根治方案**：把 `CMakePresets.json` 的 `ASCEND_COMPUTE_UNIT` 改为设备真实 soc 后重编重装（`ascend910_93` 可用；更细的如 `ascend910_9362` 不要试——当前 CANN 无对应 ops 模板会编译失败）。缺点：工程绑定该设备，多人/多云环境不通用，故 Lab 学习场景常用复制法。

### 8.5 其它 161001 排查顺序（本案例已排除）

1. 测试调用姿势与官方模板逐字比对（§5）——先排除自己代码问题；
2. `aclCreateTensor` strides 姿势（连续布局必须 `nullptr,0`）；
3. tiling 字段 host/kernel 同名一致、数值非除零；
4. 若以上都干净 → 开日志，90% 概率落到本节的 soc/目录/注册类问题（§8.4）。

---

## 9. 云端调试方法论（让框架把话说清楚）

- 开全量日志并打屏（必须在 source 环境之后执行程序）：
  ```bash
  export ASCEND_GLOBAL_LOG_LEVEL=1 ASCEND_SLOG_PRINT_TO_STDOUT=1
  ./execute_op . > full.log 2>&1
  ```
- 关键行快速定位：
  ```bash
  grep -n -iE "error|fail|invalid|161001" full.log | head -60
  ```
- 日志三段前缀 `[LEVEL] 模块(pid,进程名):时间 [文件:行号] 线程号 消息`：模块有 `RUNTIME / GE / OP / ASCENDCL / TDT`。`OP` 模块（NNOP）里的 `errno[161001]` + 详细描述是本坑的主战场；`RUNTIME` 的 WARNING 噪音（`Get env[xxx] failed`、`CQE warning` 等）可忽略。
- 判断"程序是否真的启动到算子阶段"：若日志只有 `error while loading shared libraries` → 环境问题（§7），与算子无关。
- 目标信息：`OpName:[AscTry_0]`、`socVersion [xxx]`、`binary_info_config.json` 路径、`opType`。拿到这三个字符串基本能定性。

---

## 10. 写算子工程检查清单（供其它项目复用）

- [ ] host OpDef：Input/Output 名、dtype、format 与 kernel/autogen 一致；`SetInferShape`+`SetInferDataType`；`SetTiling` 绑定 `TilingFunc`；`AddConfig` soc 与 `CMakePresets.json` 的 `ASCEND_COMPUTE_UNIT` 相同（并确认 = 设备真实 socVersion，见 §8）。
- [ ] tiling struct 头文件：host/kernel 共享同一份；字段全 `uint32_t`；字段名两处一致；`SetBlockDim(coreNum)` 必写。
- [ ] TilingFunc：`coreNum=min(核数, 总块数)` 后仍 `max(...,1)` 防除零；UB 预算按 A/B/C 三路 × 双缓冲核算；`GetTilingData<T>()` 只写不 new。
- [ ] kernel：形参顺序 输入→输出→workspace→tiling；`REGISTER_TILING_DEFAULT`+`GET_TILING_DATA_WITH_STRUCT` 一对；大小核 GM 偏移修正公式（§3.1）逐字节验算一遍；尾批元素数非 0（整除时置为整批）。
- [ ] 非 32B 对齐 shape 的 padding 越界：`DataCopy` 只搬实际元素数（`processDataNum`），SetGlobalBuffer 只声明实际可达元素，杜绝读 GM padding 越界。
- [ ] 运行前 `source set_env.bash`；错误码一律先开日志再判断，不盲改代码。
- [ ] 静态自检：字段一致性、头文件依赖、read_lints 0 错 → 才交付云端。

---

## 11. 状态记录（本文件对应的工程进行时）

- 已完成：host/kernel 三件套补齐（逻辑与官方 03.04 同构，静态核验 0 错）、test 套件落盘、云端 build 全绿、RUN 包安装成功、161001 根因定位（socVersion 不匹配）、复制法后数值全错问题解决（§12），`{7,97}` shape 实测 **TEST PASSED**（2026-09-05）。
- 遗留（可选）：用 `{8,16,64}` 等更大 shape 回归"多核多批"路径；多 soc 多云复用需求再做 ASCEND_COMPUTE_UNIT 根治。

---

## 12. 坑中坑：soc 复制法 workaround 后算子数值全乱（679/679 全错）——2026-09-05 实证

### 12.1 现象

按 §8.4 复制法越过 161001 后首次上板 `{7,97}`：

```
TEST FAILED!
mismatch count: 679/679
max abs diff: inf
index: 0, output: 4.364, golden: 79.75, diff: 75.39
index: 3, output: 100001.08, golden: 78.91, ...
index: 5, output: 2092737.4, ...
index: 6, output: 3.28e19, ...
```

特征：**每个元素都错**，output 混有"幅值像样的浮点(±100 量级)"与"巨数(1e5~1e19)"，且执行过程无任何报错（注册、kernel 加载、执行、`aclrtSynchronizeStream` 全绿）。

### 12.2 逐项排除（判别思路，可复用）

| 假设 | 判别方法 | 结论 |
|---|---|---|
| 非对齐 shape 尾元素越界 | {7,97}=679 元素 → 32B 对齐后 85 块=680 slot，只多余 1 slot 越界 4B | 解释不了 index 0~8 全错 → 排除 |
| 算法逻辑 bug | host/kernel 与官方 03.04 `AddCustomTemplate` 逐行同构（官方在同类环境复制法下 test pass） | 不该全乱 → 低嫌疑 |
| tiling 全 0 / kernel 没跑 | host 侧把输出 tensor 初始化为 0 再拷贝 device，实测 output 非 0 | kernel 确实执行且写了 C → 排除 |
| soc 目录缺失 | `ls` 两目录均含 `asc_try/` 的 `.json + .o`，npu-smi 显示 Ascend910（910B 系） | 目录层正常 → 排除 |
| **二进制版本混搭（根因）** | 161001 修复期间反复 build/install/cp，`.o`(hash) 与 host 库/配置不再配套 | **确诊**：kernel 拿到错位 tiling/元数据 → 越界读写 → 全量乱数 |

### 12.3 根因与解决（最小闭环）

根因：**改过 op_host 或 op_kernel 后，只重编译 execute_op 或只重做目录复制，而 `.o` hash、`libcust_opapi.so`(autogen+host tiling 实现)、`libcust_opmaster_rt2.0.so`(tiling 回调)、`binary_info_config.json` 之间版本不配套**。这种错配无任何 error 日志，只表现为数值错，是整条链路里最难查的一类。

解决（一次全链干净重建即 PASS）：

```bash
cd <工程根，含 build.sh>
rm -rf build_out                       # 关键 1：清掉旧 hash 产物
bash build.sh
./build_out/custom_opp_ubuntu_aarch64.run --install-path=$HOME
cd $HOME/vendors/customize/op_impl/ai_core/tbe/kernel
rm -rf ascend910_93 config/ascend910_93   # 关键 2：旧 soc 复制目录也删干净
cp -r ascend910b ascend910_93
cp -r config/ascend910b config/ascend910_93
source $HOME/vendors/customize/bin/set_env.bash   # 关键 3：so 被覆盖，必须重新 source
cd <工程根>/test && bash run.sh
```

### 12.4 经验沉淀（跨项目复用）

1. **改任何 host/kernel 代码后，验证闭环 = 全链重建 + 重装 RUN + 重做 soc 复制 + 重新 source**，四步缺一都可能踩"数值全错但无报错"。
2. **RUN 包每次重装都会清掉手工 cp 出的 `ascend910_93` 目录**，必须重跑两条 `cp -r`；`set_env.bash` 指向的 so 已被覆盖，也要重新 source（旧 shell 环境变量指向旧库）。
3. **判别"数值全错"四步走**：① 输出 tensor 预填 0，判断 kernel 是否真跑（非 0=跑了）；② 排除 tiling 全 0（loop 不执行则输出保持初始值）；③ 排除非对齐尾元素（只影响对齐后多出的 1 个 slot）；④ 以上都干净 → 锁定二进制版本混搭，走 §12.3 全链重建。
4. 日志"全绿"不代表没问题：soc 复制/二进制混搭类错误的症状在数值层，**先重建再怀疑算法**，避免在正确代码上反复空转。
5. 验证基线 shape 建议：用 `{7,97}` 这种**非 32B 对齐** shape 当回归用例（覆盖大/小核 + 尾块不整除），跑通后再上 `{8,16,64}` 测多核多批。

---

## 附：与本文件配套的工程/文档锚点

- 本工程源码：`Projects\out\AscTry\`（op_host/op_kernel/test/framework 全量；git 提交历史见 45a81bf / f3641ca 之后）。
- 同代官方参照工程：`Reference_And_Documentation\tutorials\ascendc_operator_development\03_intermediate_vector_operator_development\src\custom_op_04\`。
- aclnn 测试骨架参照：`Reference_And_Documentation\tutorials\ascendc_operator_development\09_course_practice\src\09.01_testcase\testcase_1\aclnn_test.cpp` 与 `03_intermediate_vector_operator_development\src\custom_op_04_test\test\main.cpp`（官方 `CreateAclTensor`/资源释放/调用序列的权威样本）。
- 课程知识主题文件（01~06）索引：`ascendc_knowledge_index.md`；本文件为其第七份独立经验文件，未并入索引地图（按用户"不追加已有文件"要求单独存放，需要时再手动登记）。
- 云端问题参考：gitcode `cann/cann-learning-hub` issue #390（课程环境 910B soc 不匹配的官方反馈与 workaround）。
