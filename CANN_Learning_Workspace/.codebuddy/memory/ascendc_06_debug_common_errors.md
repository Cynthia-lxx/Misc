# 主题⑥ 典型错误模式与调试方法

> 覆盖：日志/产物、精度问题与地址对齐典型场景（07.04 两个动手案例）、CPU 孪生调试、NPU 上板调试、msProf/性能（08 章已读部分）、UT/ST（06 章）快速参考。
> 来源锚点：主课 07_Troubleshooting 与 06 章、08 章；`Sources/07.02/cpu_debug` 工程模板可复用。

## 1. 输出日志与产物位置（07.04 5.1，排错第一步）

- Kernel 内 `AscendC::printf` 输出路径：
  - NPU 域运行（上板）：默认在运行目录下生成 **plog** 目录；查询命令见 07.04 5.1（cce 日志文件名形如 `plog/cce_*.log`），或设备侧 `printf` 重定向到运行目录 stdout。
  - 常见配置：`plog` 里按设备/线程组织，出错时先 tail 该文件。
- 运行目录里还会出现输入输出产物文件（如 kernel 输入/输出的 bin）。**先确认日志与实际输出文件内容再怀疑算法**。
- 工具：`npu-smi info` 看设备状态；`dmesg`（需权限）；CANN 报错码（如 507015）查官方错误码手册。

## 2. 典型错误模式 1：精度不对（数据错位/内存尺寸不足）（07.04 5.2 实战）

- 症状：add 输出全 0 或值错。现象复现路径：`ascendc_operator_development/07_Troubleshooting/Sources/07.04/`（buggy 版本）。
- 根因 1（此案例）：`Init` 中某队列 `pipe.InitBuffer(inQueueX, BUFFER_NUM, this->tileLength);` —— **漏乘 `sizeof(float)`**，而 `tileLength` 是元素个数。队列实际分配字节数不足，DataCopy 写入越界，后续数据错乱（x/y/z 地址重叠污染）。
- 定位手法：
  - `AscendC::printf` 打印 `xLocal.GetSize()`（应等于 tileLength*4=8192）与期望字节对比；打印各块 GM 首地址。
  - 检查所有 `InitBuffer` 是否统一乘 `sizeof(T)`；打印总 buffer 预算（3 路 × tileLength×4）。
  - 用 cpu 域（或 NPU）小 shape 复现（shape 改 (8,8)），缩小范围看首块是否对。
- 通用核对项：块数*每块元素=总元素；每块元素×typeSize=InitBuffer 字节；GM 偏移单位（元素）与 DataCopy 单位一致。

## 3. 典型错误模式 2：地址对齐违规（07.04 5.3 实战，错误码 507015）

- 症状：运行报错（RT_ERROR 507015 之类），定位到 kernel CopyIn/Compute。
- 根因：对 `LocalTensor`/GM 取**非对齐偏移**（如对 zLocal[1] 起做 DataCopy/计算起点 4 字节不对齐）——向量搬运/计算要求地址 32B 对齐（元素个数参数在内部按块换算，起点错位即违规）。
- 修正：不以元素偏移跳着拷；需要偏移时按 32B 对齐的块边界组织（如按 tileLength 整块、GM 偏移用 blockLength 等 32B 倍数）。

## 4. 对齐/越界通用规则（贯穿 02/03/07 章）

- UB 每块 32B 对齐；Vector 指令以 256B/次读取，repeat 数内元素连续。
- DataCopy 的 GM 源/目的地址与长度建议对齐 32B（DataCopyPad 用于非对齐补齐场景，见主题⑤待补）。
- GM 侧多核偏移 = blockLength*GetBlockIdx()（元素）× typeSize，保证 32B 对齐的前提是 blockLength×typeSize 是 32 的倍数 → **设计 tileLength/blockLength 时算上对齐**。
- Tiling 里最终数据不足一块时向上补齐再搬（03.04 的 align32 公式）。

## 5. CPU 域孪生调试（07.02）

- 目标：不依赖 NPU 先在 CPU 上跑通逻辑；用共享内存模拟 GM + CPU 实现等价计算。
- 调试程序结构（`add.cpp`）：包含 kernel（含 `kernel_operator.h`）+ `data_utils.h` + main。
  - 申请内存：`AscendC::GmAlloc(bytes)`（模拟 GM）；`AscendC::GmFree`。
  - `AscendC::SetKernelMode(KernelMode::AIV_MODE);`（矢量算子须设 AIV 模式）。
  - 调用核函数：**`ICPU_RUN_KF(add_custom, numBlocks, x, y, z)`**（专用调测宏，替代 <<<>>>）。
  - 数据准备：python `gen_data.py`（numpy）生成 input_x.bin/input_y.bin/golden.bin；`ReadFile` 载入、`WriteFile` 输出 output.bin。
  - 校验：`verify_result.py output.bin golden.bin`（np.isclose，rtol=1e-3/atol=1e-5，误差比例 ≤1e-3 判 pass）。
- 编译：CMake `find_package(tikicpulib REQUIRED)` + 链接 `ascendcl` 与 `tikicpulib::${soc_version}`，`-Dsoc_version=Ascend910B4`；`-D_GLIBCXX_USE_CXX11_ABI=0 -Wall -Werror`。
- 注意事项（07.02 4）：CPU 孪生不支持部分算子（如涉及真实硬件专用指令/异步特征）；仅适合逻辑验证。
- data_utils.h 提供 `INFO_LOG/WARN_LOG/ERROR_LOG/CHECK_ACL(x)/ReadFile/WriteFile/DoPrintData/DoPrintHalfData`，可复用到 NPU 工程。

## 6. NPU 域上板调试（07.03）

- 手段 1：`AscendC::printf`（支持 int8/uint8/int16/…/float/half 等，见格式符表）；在 NPU 上板上正常输出到 plog。
- 手段 2：**`AscendC::DumpTensor`**（07.03；03 章已见头文件）：
  - 对 LocalTensor：`AscendC::DumpTensor("label", localTensor, count)`；
  - 对 GlobalTensor：指定起始地址与 shape 的多参版本；dump 出各核数据便于离线比对。
  - 需要包含 `kernel_operator_dump_tensor_intf_impl.h`。
- 上板数据核对套路：先 `DumpTensor` 看输入是否搬运正确（与 python 生成 bin 比对），再看输出，缩小到 Compute 或 CopyOut。

## 7. 性能定位（08 章）

- **采集命令**：`msprof op --output=prof_dir ./execute_op`；关键参数 `--kernel-name`（算子名/前缀）、`--launch-count`（默认1，采集最大算子数 1~5000）、`--launch-skip-before-match`（跳过前面若干算子）、`--warm-up`（预热提频，避免降频干扰小算子数据）。可执行程序参数放命令最后；先 `source ${HOME}/vendors/customize/bin/set_env.bash` 加载自定义算子 so 路径。
- **输出文件**（prof 目录 OPPROF_xxx/）：`OpBasicInfo.csv`（算子名/block dim/总耗时，先看这个做前后对比）、`PipeUtilization.csv`（aiv_time、aiv_vec_time+ratio、aiv_scalar_time+ratio、aiv_mte2/3_time+ratio、active_bw、icache miss 等）、`ResourceConflictRatio.csv`（UB bank conflict）、`Memory*.csv`/`L2Cache.csv`/`MemoryUB.csv`（带宽、L0/L1/L2/UB 读写）、`ArithmeticUtilization.csv`（Cube/Vector 指令耗时占比）。pandas 直接读 csv 分析。
- **理论耗时估算**：搬运 = 字节数/理论带宽（如 GM≈1.8TB/s）；计算 = 元素数/理论算力。耗时长的流程 + 与理论差距大的环节 = 瓶颈。
- **常见瓶颈与对策（08.04 三个方向）**：
  1. **Tiling 策略**：核间不均分 → 尾核空闲（如 45 块×20480 上 40 核=2x 耗时）。对策：算好 SetBlockDim 与均分使各核负载基本相等（每核 23040 等量）；不能均分时把尾块打散到前几核而非另起一档。
  2. **流水编排**：搬入/搬出与计算不重叠 → 开双缓冲（BUFFER_NUM=2）隐藏搬运时间；开销记录循环内 scalar 控制（越少越好）。
  3. **内存访问/指令级**：scalar 占比过高（例>90%）→ 把能在 Host 算的挪到 Host（tileLength 已挪到 TilingData）；去掉循环内 printf 等无用 scalar 指令；复用 UB、避免小粒度多次搬运。
- 优化闭环：改 Host Tiling/核代码 → `bash build.sh; ./build_out/custom_opp*.run --install-path=${HOME}` → 重抓 msprof → 与优化前 OpBasicInfo 对比耗时。

## 7b. 综合实践（09 章 practice，多类型 Vector 算子）

- 09.01 任务形态 = 完整闭环样例：给出公式（LogSigmoid：log(1/(1+exp(-x)))）→ json 原型（`type`:["float16","float","bf16"] 一一对应 format）→ msopgen 生成 → 改 op_host Tiling（StorageShape 循环求 total size）→ 改 tiling 结构体 → 填 kernel → build.sh+install → 多 case 跑 `src/09.01_testcase/testcase_N/run.sh` 验证。
- 通用点：`for dim in StorageShape: data_sz *= dim` 等价 GetShapeSize；`GET_TILING_DATA(tilingData, tiling)` 在 REGISTER_TILING_DEFAULT 后可直接得到结构体引用（注意 vs GET_TILING_DATA_WITH_STRUCT 两种写法都见官方代码）。

## 7c. 仿真调优（无实体硬件的离线性能验证，light 04.04 / 主课 08.03）

- 前置：算子能正常调用；性能**仿真**模拟实际硬件指令执行（如 A2 产品用 Ascend910B1 simulator），非编译器静态分析。
- 编译：CMake 配置加 `-DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201`；同时给 ASC 编译加 `-g`（源码行号可见，便于 trace 定位代码行）。
- 采集：`msprof op simulator --soc-version=Ascend910B1 --output=./prof ./demo`（指定核：`--core-id=0`；不设 LD_LIBRARY_PATH 时须给 --soc-version）。
- 产物：`OPPROF_xxx/simulator/{trace.json, visualize_data.bin}`（core*.veccore*/cubecore* 分目录）；`dump/` 为中间件。
- 分析：trace.json 用 chrome://tracing（W/S/A/D 缩放平移）或 MindStudio Insight（可看每行源码对应时钟周期）。
- 调优例：trace 显示耗时集中在 SCALAR（多为 `printf`）→ 编译加 `-DASCENDC_DUMP=0`（`target_compile_definitions`）屏蔽打印 → 重新采集对比，scalar 耗时显著下降。
- 核心优势：不依赖实体硬件即可完成性能验证与瓶颈定位（计算模块/搬运模块/同步等待耗时占比对比），缩短优化周期。

## 8. 静态自检清单（本地 Windows 无编译器可用——每写完一版先过此单）

1. 头文件与顺序：`kernel_operator.h`、`acl/acl.h`；DumpTensor 场景加 dump 头；Host 侧 register/op_def_registry.h、platform_ascendc.h 等。
2. 核函数声明：`extern "C" __global__ __aicore__`（模板化算子不加 extern "C"）；返回 void；GM 指针 `__gm__`。
3. 签名五段：输入… + `GM_ADDR workspace` + `GM_ADDR tiling`（工程化）；直调式带 tiling 结构体值参。
4. Tiling 结构体 host/kernel 字段逐字节核对（同一头文件或 struct 一致性）；macro REGISTER_TILING_DEFAULT/GET_TILING_DATA_WITH_STRUCT 与结构体名一致。
5. InitBuffer 字节 = 元素×sizeof(T)；BUFFER_NUM 与循环次数配合（tileNum×BUFFER_NUM）。
6. SetGlobalBuffer 偏移单位元素对齐 32B；多核偏移与 blockLength 定义一致。
7. DataCopy 元素数/偏移同单位；子矩阵回写用 DataCopyParams 的 blockLen/stride 按 32B 折算。
8. Matmul：MatmulType 位置/格式/类型与 Host MultiCoreMatmulTiling Set*Type 一致；C 为 VECIN 时（CV 融合）TQue 深度 1、GetTensorC<true> 同步。
9. EnQue/DeQue/FreeTensor 成对（AllocTensor 的都要 Free；DeQue 的用后 Free）。
10. 编译宏（DTYPE_X 等）在模板化/msopgen 场景才存在；不要手写多余宏。
11. aclnn：GetWorkspaceSize 与执行两段接口签名顺序（含 attr 形参）与部署后头文件一致。

## 9. UT/ST（开源仓验证，06.04）

- UT 目录（放算子目录 tests/ut 下）：`op_host/test_${op}_tiling.cpp`、`op_host/test_${op}_infershape.cpp`、`op_kernel/test_${op}.cpp`；无需部署即可验证 InferShape/Tiling/Kernel。
- ST：系统级端到端验证（部署后跑 aclnn/pybind 调用 + golden 比对）。
- 开源仓 per-op 文件组织（与 msopgen 单文件不同）：`op_graph/add_proto.h + add_graph_infer.cpp`、`op_host/{def,tiling,infershape}.cpp`、`op_kernel/{.h,.cpp,tiling_data.h,tiling_key.h}`、`examples/test_aclnn_*.cpp`。
- 开源仓 URL：ops-nn / ops-math / ops-transformer / ops-cv（gitcode.com/cann/…）。用户自研算子放 `experimental/{math,conversion,random}/`。
