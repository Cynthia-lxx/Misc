---
name: fix-p2a-gm-cast
overview: 修复 P2a 工程 kernel.asc 中 GM_ADDR(__gm__ uint8_t*) 被 reinterpret_cast 为普通指针导致的 3 处编译错误，保持 __gm__ 限定做指针算术与 launch 传参，然后重新云端运行 run.sh 验证 case0。
todos:
  - id: fix-gm-pointer-casts
    content: 修改 kernel.asc 行 205-207 与 209-214：base 及 batch 指针保留 __gm__ 直接拷贝 GM_ADDR，删除 reinterpret_cast 与 const_cast，更新行注释
    status: completed
  - id: text-sanity-check
    content: 全文自检：确认无残留 const/reinterpret_cast/const_cast，kernel 体与 ACL 分配未误改，整理云测说明与兜底预案
    status: completed
    dependencies:
      - fix-gm-pointer-casts
---

## 需求概述

修复 `bmm_p2_slice` 工程在 CANN Lab 云端编译失败的问题。用户将工程上传后执行 `bash run.sh`，bisheng 编译器在 `kernel.asc` 的 host 侧 `run_kernel` 内报 3 个错误：

- 行 205-207 使用 `reinterpret_cast` 将 `GM_ADDR`（本质为 `__gm__ uint8_t*`）强转为不带 `__gm__` 限定的普通指针（`const uint8_t*` / `uint8_t*`），bisheng 禁止此类去地址空间限定符的转换。

## 核心功能点

- 最小改动消除 3 处编译错误：base 指针与 batch 派生指针全程保留 `__gm__` 限定，直接拷贝 `GM_ADDR`，删除 `reinterpret_cast` 与 `const_cast`。
- 保持 P2a 既有架构与语义不变：单 batch Cube MatMul 计算 C 到临时 GM 缓冲，M==N==1 时经 D2D 拷贝把 C[0,0] 写入 y；bias/workspace/C 缓冲来自 `aclrtMalloc` 的普通指针保持原样（03.03 官方同款用法）。
- 本地仅做文本级自检（无编译器），交付后由用户重新上传云测 `bash run.sh`，预期编译 0 error、case0 输出 `PASSED`。

## 边界与验证判据

- 只改 `kernel.asc` 一个文件，不触碰 `main.asc`、CMake、脚本及其他工程。
- 云测通过标准：`[2/4]` 编译成功（3 个 cast 错误消失）；`[4/4]` 运行后 `verify_result.py` 对 case0 输出 PASS（rtol=atol=1e-4）。
- 若云测再报新错误（如 `aclrtMemcpyAsync` 收 `__gm__` 指针、极小 shape 或 matmul 头文件问题），按报错逐条修复并记录为 P2 核心发现。

## 技术栈

- Ascend C（.asc 异构混合编程，bisheng/ccec 编译）
- Cube 高阶 Matmul API + Host 侧 ACL（aclrtMalloc / aclrtMemcpyAsync / <<<>>> 直调）
- 编译约束：`GM_ADDR` 即 `__gm__ uint8_t* __restrict__`，bisheng 在 host 段同样校验地址空间限定符，任何去掉 `__gm__` 的显式强转均被禁止；但 kernel launch 边界允许普通指针向 `__gm__` 形参的隐式传递（03.03 官方样例同款）。

## 实现方案

### 修改策略

单文件、单点修复 `p:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\kernel.asc`：不再把 GM 指针"降级"为普通指针再传回 launch，而是让 `__gm__ uint8_t*` 指针全程在 GM 地址空间内拷贝、算术与传参，从而完全消除 reinterpret_cast / const_cast。

### 精确改动（行 205-214）

```
// 行 205-207：GM_ADDR 直接拷贝，保留 __gm__
__gm__ uint8_t* x1Base = x1;
__gm__ uint8_t* x2Base = x2;
__gm__ uint8_t* yBase = y;

// 行 209-214：派生指针同为 __gm__，launch 去掉 const_cast
for (int64_t b = 0; b < B; ++b) {
    __gm__ uint8_t* aBatch = x1Base + b * M * K1 * static_cast<int64_t>(sizeof(uint16_t));
    __gm__ uint8_t* bBatch = x2Base + b * K2 * N * static_cast<int64_t>(sizeof(uint16_t));
    uint8_t* cBatch = cDevice + b * M * N * static_cast<int64_t>(sizeof(float));   // 普通指针，保持不动
    bmm_batch_cube<<<1, nullptr, stream>>>(aBatch, bBatch, biasDevice, cBatch, wsDevice, tiling);
```

- kernel 形参本就是 `__gm__ uint8_t*`，`aBatch/bBatch` 类型精确匹配，无需任何 cast。
- `aclrtMalloc` 返回的普通指针（`biasDevice/cDevice/wsDevice`）按 03.03 同款直接传 launch，不动。
- 行 217 的 `aclrtMemcpyAsync(yBase + ...)` 沿用 yBase（现为 `__gm__ uint8_t*`）作 D2D 目的地址，D2D 设备地址语义不变。

### 兜底预案（仅当云测对行 217 再报"**gm** 指针不能转 void*"时启用）

将临时 C 缓冲与 host D2D 拷贝整体去掉，让 P2a 极小 shape（M==N==1）下 kernel 的 `c` 形参直接指向 y 的 GM 偏移（`c = yBase + b*sizeof(float)`，同为 `__gm__` 指针，语义等价于把 C[0,0] 写入 y[b]），删除 cDevice 的 malloc/free 与 memcpy 逻辑。默认不启用，保持改动最小。

## 实现注意

- 严格限定改动范围在 run_kernel 的指针处理与 launch 调用，避免无关重构。
- kernel 体内 `reinterpret_cast<__gm__ half*>` 等转换本就合规，不属本次范围，勿动。
- 修复后本地逐行文本自检：确认无残留 `reinterpret_cast`/`const_cast`/`const uint8_t*`，确认行内注释更新（解释为何必须保留 `__gm__`），随后交付用户云测。
- 若云测暴露新错误，保留本次 log 与修复记录，逐条跟进，作为 P2 里程碑的真实环境发现沉淀。

## 目录结构

```
p:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\
└── kernel.asc  # [MODIFY] run_kernel 行 205-214：base/batch 指针保留 __gm__ 限定并直接拷贝 GM_ADDR；
                #   删除行 205-207 的 reinterpret_cast 与行 213 的 const_cast；更新相关行注释；
                #   其余（kernel 体、tiling、ACL 分配、M==N==1 stand-in 语义）保持不变。
```