---
name: P1 Vector 预热首轮：多 dtype + 非对齐尾块能力扩展（Addcmul 载体真机闭环）
overview: 按父计划 P1（Vector 预热，Addcmul 为载体），把 OpHelper 从「单 dtype float32、可整除、同 shape」推进到「dtype 参数化 + 非对齐尾块 + tiling 多字段 + verify 框架 dtype/shape 化」，以 Addcmul 语义最小工程在云端 CANN Lab 真机 TEST PASSED，且既有 229 测试零回归。
todos:
  - id: explore-tail-paradigm
    content: Use [subagent:code-explorer] to extract official 03 Vector tail-block paradigm and test blast-radius map
    status: completed
  - id: fillgen-dtype-tail
    content: "Modify fillgen: dtype {float,float16} profile, tiling three fields, kernel tail path, byte-stable float legacy"
    status: completed
    dependencies:
      - explore-tail-paradigm
  - id: templates-sync
    content: Sync template snippets/assembly (kernel copy/compute tail, host tiling/infer DT_FLOAT16) keeping concat invariant
    status: completed
    dependencies:
      - fillgen-dtype-tail
  - id: verifygen-dtype
    content: Parameterize verifygen element count, fp16 pack/unpack, dtype interpreter rounding, ACL dtype runner, verify_result
    status: completed
    dependencies:
      - templates-sync
  - id: i18n-docs-tests
    content: Add zh/en i18n keys, update expr-rules/official-patterns docs, write test_p1_dtype_tail.py and run full pytest
    status: completed
    dependencies:
      - verifygen-dtype
  - id: cloud-verify
    content: Create fp16 addcmul tail project in Projects, drive cloud fill-op + run_verify.sh TEST PASSED, backfill plan/memory
    status: completed
    dependencies:
      - i18n-docs-tests
---

## 用户需求

延续 BatchMatmulMaxSum 父计划：P0（语义基线+用例矩阵+FP64 golden）已完成并获云端 torch 对拍背书。用户授权按最推荐选项推进，即启动 **P1（Vector 预热，Addcmul 载体）**。

## P1 首轮垂直切片范围（本次实施）

以 Addcmul 语义退化用例为真机载体，把 CANN_OpHelper 工具链从「单 dtype float32、可整除、固定 8×2048 验证」推向「**多 dtype（float32 + float16）参数化 + 非对齐尾块 + 验证框架 dtype/shape 化 + tiling 多字段**」，为后续 BMM 归约侧打地基。**明确后置**（不在本轮）：int8/int32、NumPy 真广播、value=[1] 标量张量、空张量、评测 asc 壳形态。

## 真机载体裁定（推荐并待用户确认）

沿用既有 **msopgen 空壳 → fill-op 写盘 → verify 资产 → 云端 run_verify.sh** 三文件流程（与 AddCustomTemplate 云端 TEST PASSED 同型），在实测区 `Projects\` 下新建工程。用例语义 = `out = a + x1 * x2 * 2.0`（3 输入 1 输出、同 shape、expr 常量表达 value=2.0，与 torch.addcmul(a,x1,x2,value=2.0) 数学等价，规避 v1 无法表达的 [1] 标量广播）；dtype=fp16、总元素数非 32 整倍数（触发尾块）。golden 由同一 ExprProgram 解释器生成（verify 自洽），无须 torch 背书。

## 验收门禁（对齐父计划 P1）

1. 既有 229 测试全绿、旧 float/整除路径三文件产物**逐字节不变**（blast-radius 受控）；
2. 新能力单测覆盖：fp16 生成、非对齐 totalLength 的 tiling 字段、verify dtype 化解释器；
3. 云端真机：fp16 + 尾块用例在实测区工程 `bash verify/run_verify.sh` 输出 TEST PASSED；
4. 文档/i18n（zh+en）同步，父计划与记忆回填。

## 技术栈

沿用既有 Python 工具链（typer + rich + Jinja2 + PyYAML + pathlib，penv 运行）；不新增第三方依赖（fp16 打包用标准库 struct `<e`，与 BMM golden 同级 RNE 量化方法一致）。真机验证在云端 CANN Lab，本地不编译任何 C++。

## 实现方案与关键技术决策

1. **dtype 参数化（最小侵入）**：fillgen 的 `profile_from_spec` 现有 v1 校验强制 `dtype_set == {"float"}`（fillgen.py L197），而 `_DTYPE_GE` 已含 float16 映射（L50）。放宽为：全部张量同 dtype 且 ∈ {float, float16}；`TensorRef.dtype_ge`/`FileProfile.cpp_dtype` 随 dtype 派生（float→float/DT_FLOAT，float16→half/DT_FLOAT16）。旧 spec 无 dtype/默认 float 时走原路径，产物逐字节不变。
2. **非对齐尾块（tiling 多字段）**：现有 TilingData 仅 `totalLength + tileNum`（fillgen L391/430，host 假定整除）。扩展为三字段：`totalLength / blockLength / tailLength`（tailLength = totalLength - blockLength×blockDim，仅末核处理；blockLength = totalLength÷blockDim 向下取整）。kernel Init/Process 增加末核尾段 DataCopy+循环路径。尾块循环形态先按官方 03_intermediate_vector_operator_development 非 32 整倍数范式核对后再定（写代码前必查官方文档）。
3. **verify dtype/shape 化**：verifygen 现有 `DATA_LENGTH = 8*2048` 固定、`_FLOAT32`/`_f32` 逐语句舍入、runner 硬编码 ACL_FLOAT 与固定 tensorShape（L45/51/278/306/311）。改造：元素数由用例 shape 派生（默认仍 8×2048 保回归）；输入按 dtype 打包（float32 LE / float16 用 `<e` RNE）；golden 解释器新增按 dtype 的舍入函数（fp16 语句级舍入，与 kernel 同精度）；runner 的 tensorShape/ACL dtype/读回解析按 profile dtype 参数化；`verify_result.py` 按 dtype 解包与判据（沿用 rtol/atol 体系）。
4. **真机载体**：实测区 `Projects\` 下新建 addcmul_custom 工程（云端 msopgen 生成 fp16 空壳 + 本地 fill-op 写盘 + verify 资产；或本地用既有 gen-msopgen 出命令交用户云端执行）。验证沿用 run_verify.sh（含 soc 别名 workaround）。
5. **回归防线**：所有新增能力由 spec 显式 dtype=float16 或非整除 shape 触发；float32+整除默认路径与旧三文件字节级一致；i18n 新键 zh+en 成对。

## 架构与数据流（改动链）

spec(YAML: dtype/expr/shape) → fillgen `profile_from_spec`（放宽校验+派生 dtype/cpp 类型）→ 模板 snippets（kernel tail 循环 + host tiling 三字段 + host InferShape/DataType）→ 三文件写盘 → verifygen（dtype/shape 化资产 + 解释器）→ 云端 run_verify.sh 判 TEST PASSED。不变量保持：`concat(render_snippet(s)) == render(spec)[relpath] == golden`。

## 目录结构与改动清单

```
CANN_OpHelper/
├── src/cann_ophelper/
│   ├── fillgen.py      # [MODIFY] dtype 校验放宽（{float,float16} 同 dtype 域）、cpp/dtype_ge 派生、
│   │                   #          tiling 三字段（totalLength/blockLength/tailLength）生成、kernel 末核尾段循环
│   ├── model.py        # [MODIFY] 仅按需微调（shape 元数据已存在；如无必要不动）
│   ├── template/
│   │   ├── assembly.py # [MODIFY] FILE_ASSEMBLIES/上下文随新 tiling 字段与 dtype 扩展
│   │   └── templates/snippets/
│   │       ├── kernel_init.j2 / kernel_compute.j2 / kernel_copyin.j2 / kernel_copyout.j2
│   │       │            # [MODIFY] half 类型声明、tail 分支（末核余段 copy+compute）
│   │       ├── host_tiling.j2 / host_infer.j2 / host_opdef.j2 / tiling_body.j2
│   │       │            # [MODIFY] 三字段计算（blockLength=totalLength/blockDim、tailLength=余数）、
│   │       │            #          DataType({ge::DT_FLOAT16})、tensorShape 随用例
│   ├── verifygen.py    # [MODIFY] 元素数参数化、fp16 打包(‘<e’)/解包、解释器按 dtype 舍入、
│   │                   #          runner tensorShape+ACL dtype+读回、verify_result dtype 化
│   └── i18n.py         # [MODIFY] 新增键（fillgen.*/verifygen.*）zh+en 成对
├── docs/
│   ├── expr-rules.md         # [MODIFY] 增补 dtype 域与尾块/tiling 三字段契约
│   └── official-patterns.md  # [MODIFY] 增补 fp16 DataType/tail 官方依据
└── tests/
    └── test_p1_dtype_tail.py # [NEW] fp16 三文件生成、非对齐 tiling 字段、verify 资产 dtype/shape、字节级回归
实测区（工作区外，云端联动）：
p:\Dev\CANN_Learning_Workspace\Projects\out\AddCustomFp16\  # [NEW] msopgen 空壳(云端)+fill-op 写盘+verify 资产；名称待执行时按实测区命名规范定
```

## 关键代码结构（仅接口级）

- TilingData 扩展（三处同步：kernel 类内结构、`{entry}_tiling.h`、host 填充）：

```cpp
struct TilingData {  // 向后兼容：totalLength/tileNum 语义不变；tail 字段新增
    uint32_t totalLength;   // 张量总元素数（含尾）
    uint32_t blockLength;   // totalLength / blockDim（向下取整，每核主段）
    uint32_t tailLength;    // totalLength % blockDim 对应元素数，仅末核处理（可为 0）
    uint32_t tileNum;       // 核内二次切分数（v1 语义保留）
};
```

- dtype 派生约定：`dtype label ∈ {float, float16}` → `cpp type ∈ {float, half}`、`ge::DT_*`、ACL 枚举三表映射集中定义（fillgen 一处 + verifygen 一处），禁散落硬编码。

## 性能与可靠性

- 尾块仅影响末核最后一小段，主路径零额外开销；不引入 N+1 式重复搬运。
- golden 解释器与 kernel 共用同一 ExprProgram 与同精度舍入 → 真机比对自洽，判据沿用 1e-3 体系。
- 安全：不臆造 CANN API；fp16 打包/舍入方法复用 BMM golden 已验证的 RNE 路径。

## Agent Extensions

### SubAgent

- **code-explorer**
- 用途：第一步定向探索官方 03_intermediate_vector_operator_development 的非 32 整倍数尾块处理范式与现有 tests 覆盖结构，确定 kernel tail 循环形态与新增测试的挂载点。
- 预期产出：官方尾块代码范式摘录（含路径出处）与「哪些既有测试会触碰 dtype/整除假定」的影响面清单，供实现阶段直接引用。