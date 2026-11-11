# 主题④ Host 侧与 Device 侧协作关系

> 覆盖：两种交付/调用路线（Kernel 直调 .asc、msopgen 工程化算子）、工程结构、算子原型注册/InferShape、Tiling 传递、aclnn/pybind 调用链、编译部署。
> 主要来源：主课 02（直调）、03 章（工程化：03.02/03.03/03.04/03.05）+ 各章 src 工程源码。

## 0. 两条开发/交付路线的对照（务必分清）

| 维度 | 路线 A：Kernel 直调（02 章） | 路线 B：msopgen 工程化算子（03 章起主课主线）|
|---|---|---|
| 交付件 | 单文件 `.asc`（kernel+host main 混编） | 完整算子工程 custom_op（op_host/op_kernel/...）+ 部署包 custom_opp*.run |
| 调用方式 | ACL + `kernel<<<blockDim,nullptr,stream>>>` 直启 | aclnn 单算子 API / pybind / 框架入图 |
| 部署 | 无（本机可跑） | `bash build.sh` → `./build_out/custom_opp*.run --install-path` |
| 适用 | 学习/快速验证 kernel | 算子交付/业务集成 |
| 编译 | `bisheng .asc` / find_package(ASC) | CMake + npu_op_* 宏（msopgen 生成）|
| Tiling 传递 | 结构体按值作 kernel 参数 | TilingData 经 `GM_ADDR tiling` 指针 + workspace 传参 |

## 1. 工程化算子工程结构（msopgen gen 生成，03.05 6.2）

`msopgen gen -i op.json -c ai_core-ascend910b1 -lan cpp -out custom_op`

```
custom_op/
├── CMakeLists.txt            # 根 CMake（一般不修改）
├── CMakePresets.json         # 编译配置：改 ASCEND_CANN_PACKAGE_PATH / ASCEND_COMPUTE_UNIT 等
├── build.sh                  # 一键编译脚本
├── framework/                # 可选：tf_plugin/onnx_plugin 等框架插件
├── op_host/
│   ├── <op>.cpp              # 算子原型注册 + tiling 实现 + shape/dtype 推导（合并单文件）
│   └── CMakeLists.txt        # host 侧编译（npu_op_code_gen 等宏，一般不改）
└── op_kernel/
    ├── <op>_tiling.h         # TilingData 结构体定义（host/kernel 共用头）
    ├── <op>.cpp              # Kernel 实现
    └── CMakeLists.txt        # kernel 侧编译（npu_op_kernel_* 宏）
```
- 文件名按 op.json 的算子名生成（小写+下划线）。
- 带模板化算子时 op_kernel 下再加 `tiling_key_<op>.h`（模板参数声明）。
- **原型 JSON**：描述 op 名/input_desc/output_desc/attr（字段 name、param_type、format[]、type[] 等，类型列表按序与 format 一一对应）。proto 与 aclnn 头文件在编译期由框架自动生成。

## 2. op_host/<op>.cpp：三段式单文件（03.02 src 实测）

### 2.1 命名空间 optiling：Tiling 实现
```cpp
namespace optiling {
static ge::graphStatus TilingFunc(gert::TilingContext *context) {
    uint32_t totalLength = context->GetInputShape(0)->GetOriginShape().GetShapeSize();
    context->SetBlockDim(8);
    TilingDataTemplate *tiling = context->GetTilingData<TilingDataTemplate>();
    tiling->totalLength = totalLength;  tiling->tileNum = 1;
    return ge::GRAPH_SUCCESS;
}
}
```
（泛化/平台版用 `platform_ascendc::PlatformAscendC(...)` 动态取核数/UB 大小，见主题③。）

### 2.2 命名空间 ge：InferShape / InferDataType
```cpp
namespace ge {
static graphStatus InferShape(gert::InferShapeContext *context) {
    const gert::Shape *inShape = context->GetInputShape(0);
    gert::Shape *outShape = context->GetOutputShape(0);
    *outShape = *inShape;  return GRAPH_SUCCESS;
}
static graphStatus InferDataType(gert::InferDataTypeContext *context) {
    context->SetOutputDataType(0, context->GetInputDataType(0));  return ge::GRAPH_SUCCESS;
}
}
```

### 2.3 命名空间 ops：算子原型注册（OpDef 子类 + OP_ADD）
```cpp
namespace ops {
class AddCustomTemplate : public OpDef {
public:
    explicit AddCustomTemplate(const char *name) : OpDef(name) {
        this->Input("x").ParamType(REQUIRED).DataType({ge::DT_FLOAT16, ge::DT_FLOAT})
            .Format({ge::FORMAT_ND, ge::FORMAT_ND});
        this->Output("z").ParamType(REQUIRED).DataType({...}).Format({...});
        this->Attr("min").Float();                                   // 带属性时
        this->SetInferShape(ge::InferShape).SetInferDataType(ge::InferDataType);
        this->AICore().SetTiling(optiling::TilingFunc).AddConfig("ascend910b"); // AddConfig=所支持算力
    }
};
OP_ADD(AddCustomTemplate);
}
```
- 头：`register/op_def_registry.h` + 本地 tiling 头。Data/Format 列表按输入序给出"可支持组合"（长度一一对应）。

## 3. op_kernel 侧

- `<op>_tiling.h`：`struct TilingDataTemplate { uint32_t totalLength; uint32_t tileNum; };`（host 同 include，字段即契约）。
- `<op>.cpp`：
  - Kernel 类 + `__global__ __aicore__` 入口，签名 `(GM_ADDR 输入..., GM_ADDR workspace, GM_ADDR tiling)`。
  - 入口内 `REGISTER_TILING_DEFAULT(TilingDataTemplate); GET_TILING_DATA_WITH_STRUCT(TilingDataTemplate, d, tiling);` 后 Init/Process。
  - 数据流/API 见主题①/②/⑤。

## 4. TilingData 流向总览（03 章核心心智模型）

1. 框架调 Host TilingFunc(context)；
2. TilingFunc 读 shape/dtype/attr → 填 TilingData 结构体 → 写回 context（GetTilingData<T> 指针直接改字段；或用 tiling.set_x + SaveToBuffer + SetDataSize 的形式）；
3. 同时 SetBlockDim 决定核数、GetWorkspaceSizes(1) 声明 workspace、SetTilingKey/ASCENDC_TPL_SEL_PARAM 选 kernel 版本；
4. 框架按原型为每个核实例启动 kernel：kernel 参数固定带 GM_ADDR workspace 与 GM_ADDR tiling；
5. kernel 入口用 GET_TILING_DATA_WITH_STRUCT 恢复结构体 → 各核按 blockIdx 处理自己份额。
- 关键：**Tiling 结构体字段必须在 host 与 kernel 间保持字节级一致**（同一头文件），否则错位读数据。

## 5. 单算子 aclnn 调用（03.03，交付算子必用）

1. `aclnnInit(nullptr)` + `aclrtSetDevice(0)` + `aclrtCreateStream(&stream)`；
2. 建输入输出 aclTensor：`aclrtMalloc` + `aclCreateTensor(shape, dims, aclDataType, nullptr,0, ACL_FORMAT_ND, shape,dims, addr)`；host 数据 `aclrtMemcpy` H2D；
3. `aclnnXxxGetWorkspaceSize(in..., [attr,] out..., &workspaceSize, &executor)` → `aclrtMalloc(workspace)`；
4. `aclnnXxx(workspace, workspaceSize, executor, stream)` → `aclrtSynchronizeStream` → D2H 取结果；
5. `aclDestroyTensor`/`aclrtFree`…/`aclrtDestroyStream`/`aclrtResetDevice`/`aclnnFinalize`。
- 部署产物位置：`$ASCEND_HOME/opp/vendors/customize/{op_api/{include/aclnn_xx.h, lib/libcust_opapi.so}}`；编译调用程序加 `-lcust_opapi -lnnopbase -lacl_rt`。

## 6. pybind 调用（03.03，Python 便捷验证）

- 本质：C++ 里 `EXEC_NPU_CMD(aclnnXxx, x, y, z)` 调 aclnn，用 `PYBIND11_MODULE` 导出 python 函数；setuptools+torch_npu 的 `NpuExtension` 编译成 wheel 安装。
- python：`x.npu()` 转 NPU 张量 → 调用 → `torch.allclose(golden, out.cpu())` 校验。
- 依赖：torch / torch-npu / pybind11（版本对齐）。

## 7. 构建部署（03.02/03.05 实测命令）

```bash
bash build.sh                       # 生成 build_out/custom_opp*.run
./build_out/custom_opp*.run --install-path=${HOME}/   # 安装到 HOME（含自定义算子 so）
# 编译并运行 aclnn 调用程序：
g++ -I$ASCEND_TOOLKIT_HOME/include -I${HOME}/vendors/customize/op_api/include \
    -L$ASCEND_TOOLKIT_HOME/lib64 -L${HOME}/vendors/customize/op_api/lib \
    aclnn_test.cpp -lcust_opapi -lnnopbase -lacl_rt -o exec_op
source ${HOME}/vendors/customize/bin/set_env.bash; ./exec_op
```
- CMakePresets.json 需修改项：`ASCEND_CANN_PACKAGE_PATH`（CANN 实际路径）、`ASCEND_COMPUTE_UNIT`（ascend910b 等）、vendor_name（默认 customize）。
- msopgen 的 -c 取值如 `ai_core-ascend910b1`；教程工程示例配置 ascend910b + ENABLE_TEST=True。

## 8. PyTorch 框架下 Kernel 直调（light 02.06 独有）

- 复用 Kernel：在 `.asc` 里写 **C 接口外壳**，内部用 `<<<>>>` 启动（供 C++/Python 层统一调用）：
```cpp
extern "C" void add_custom_impl(uint32_t numBlocks, void* aclStream, uint8_t* x, uint8_t* y, uint8_t* z, uint32_t totalLength) {
    add_custom<<<numBlocks, 0, aclStream>>>(x, y, z, totalLength);
}
```
- 两层封装都基于 at::Tensor：
  - **pybind11**：`at::empty_like(x)` 建输出 → `c10_npu::getCurrentNPUStream().stream(false)` 取 NPU stream → `mutable_data_ptr()` 拿指针 → 调 `add_custom_impl`；`PYBIND11_MODULE(ascendc_ops, m){ m.def("ascendc_add", ...); }`。Python：`import ascendc_ops; ascendc_ops.ascendc_add(x.npu(), y.npu())`。
  - **torch.library**：`TORCH_LIBRARY(ascendc_ops, m){ m.def("ascendc_add(Tensor x, Tensor y) -> Tensor"); }` + `TORCH_LIBRARY_IMPL(ascendc_ops, PrivateUse1, m){ m.impl("ascendc_add", TORCH_FN(ascendc_ops::ascendc_add)); }`。Python：`torch.ops.load_library(...)` 后 `torch.ops.ascendc_ops.ascendc_add(...)`。
- 构建（两路通用）：CMake 里 python 查 torch/torch_npu/pybind11 路径（`import torch; torch.utils.cmake_prefix_path` 等）→ `find_package(ASC)` 先编共享 `libascendc_kernels.so`（.asc），再 `pybind11_add_module` 或普通 SHARED 编绑定库并链接 `ascendc_kernels + torch_npu`；RPATH 设 `$ORIGIN`。注意 `-DPython3_EXECUTABLE` 指向装了 torch 的解释器。
- Python 测试：`TestCase.assertRtolEqual(output, expected)`（torch_npu.testing）比对 torch 参考结果。

## 9. 直调式泛化 Tiling 模板（light 02.08 sigmoid，自包含 .asc）

- Host 侧用 `platform_ascendc::PlatformAscendCManager::GetInstance()`（manager 单例指针，区别于 op 工程里 context->GetPlatformInfo() 构造实例）取 `GetCoreNum()`/`GetCoreMemSize(UB)`；Kernel 类模板 `<typename T>` 由数据类型（float/half）实例化（template + `sigmoid_custom<T><<<numBlocks,nullptr,stream>>>`），核函数需显式 `__vector__`。
- Kernel Init 里按 `blockIdx < tailBlockNum ? big : small` 选择 big/small 参数，并对**小核 GM 偏移修正**（`-= (bigCoreDataNum-smallCoreDataNum)*(blockIdx-tailBlockNum)`）。
- 该形态的 Host main 与 ACL 环境代码全部在 .asc 内（gm bin 读写 + golden 校验同进程），适合做免 op_host 的 kernel 级回归。
