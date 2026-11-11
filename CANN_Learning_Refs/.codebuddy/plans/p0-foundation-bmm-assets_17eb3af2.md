---
name: p0-foundation-bmm-assets
overview: 执行父计划 P0「地基与语义基线」：研读官方 MatMul/Cube/融合教程并记录 Cube 数据流理解，在实测区新建 bmm_maxsum_assets\ 资产目录，产出题目语义基线文档、PyTorch 对标用例矩阵（FP64 golden，纯标准库实现 + torch 对拍脚本双形态），为后续 P1–P5 钉死语义与验证地基。
todos:
  - id: scaffold-assets
    content: 创建 bmm_maxsum_assets 目录骨架，并用 [subagent:code-explorer] 核实教程/记忆/skills 资源路径清单
    status: completed
  - id: study-cube-datapath
    content: 研读官方 04_matmul_basic 与 05_fused 教程及 ascendc 索引，撰写 docs/cube_datapath_notes.md 数据流笔记
    status: completed
    dependencies:
      - scaffold-assets
  - id: write-semantic-baseline
    content: 撰写 docs/semantic_baseline.md：公式/torch 映射/四存储布局/精度判据/官方三示例推导
    status: completed
    dependencies:
      - scaffold-assets
  - id: build-case-matrix
    content: 建立 cases/case_matrix.md 与 case_matrix.json 用例矩阵（含三示例锚点与维度域/边界标记）
    status: completed
    dependencies:
      - write-semantic-baseline
  - id: implement-golden
    content: 实现 golden/golden_fp64_stdlib.py（fp16/bf16 量化+FP64 计算+fp32 输出+三示例断言）与 golden_torch_ref.py 对拍脚本
    status: completed
    dependencies:
      - build-case-matrix
  - id: smoke-sync-memory
    content: penv 实跑 stdlib 冒烟三示例留档，补全 resources_index.md，双写记忆（日志+P0 完成状态）
    status: completed
    dependencies:
      - study-cube-datapath
      - write-semantic-baseline
      - implement-golden
---

## 需求概述

执行 BatchMatmulMaxSum 父计划 **P0 阶段（地基与语义基线）**：研读官方 MatMul/Cube/融合教程并沉淀理解；固化题目对标语义与 FP64 golden；建立覆盖题目约束的用例矩阵；产出资源索引；本地冒烟验证官方三示例。全程不写算子/工具代码、不改 CANN_OpHelper 仓库、不装 pip 包。

## 用户定夺（本计划硬约束）

- 产物落点：新建实测区独立资产目录 `p:\Dev\CANN_Learning_Workspace\Projects\bmm_maxsum_assets\`（与题目模板并排；题目工程保持原样）。
- golden 形态：纯 Python 标准库 FP64 参考实现（本地 penv 零依赖、可跑中小 shape）+ torch 对拍脚本（供有 torch 环境生成大 shape golden 与语义对拍）。

## P0 Exit Criteria（门禁）

- 语义基线文档 + 用例矩阵就绪；Cube 数据流（GM→L1→L0A/L0B→L0C→UB）理解有记录；stdlib golden 在本地 penv 实跑官方三示例通过；资源索引路径逐一核实。

## 技术栈

- Python 3.14.5（penv 嵌入式，`p:\Dev\CANN_Learning_Refs\penv\Scripts\python.exe`），**纯标准库**（无 numpy/torch，不代装）；`struct` 实现 fp16 半精度量化与 fp32 舍入；bf16 用手工高位舍入实现。
- 产物为 Markdown 文档 + JSON + Python 脚本，无 UI、无构建系统。

## 实施方案

1. 先建 `bmm_maxsum_assets\` 目录骨架，并用 [subagent:code-explorer] 核实官方教程三镜像中 04_matmul_basic / 05_fused_operator_development 的 ipynb 与 answer 源码路径（`matmul_custom.cpp`、`matmul_abs.cpp` 等），产出资源索引的可信清单。
2. 研读教程与 `ascendc_01~07` 索引，撰写 Cube 数据流笔记（GM→L1→L0A/L0B→L0C→UB、高阶 Matmul API、Vector 归约范式，并映射到本题 Cube+Vector 两级归约语义），作为语义基线文档的硬件侧依据。
3. 撰写语义基线文档：以 `stage_goal_batchmatmul_maxsum.md` 为唯一事实源复述公式 `y[b]=Σ_m max_n Σ_k x1·x2`；PyTorch 对标映射（`bmm(fp32)→amax(dim=-1)→sum(dim=-1,dtype=fp32)`）；transposeX1/X2 四存储布局与逻辑 shape 的关系；精度判据按题面原文表述（fp16/bf16 双 1e-3；不臆造输出按 fp32 判 1e-4）；复现官方三示例推导（y=[2.0]、y=[2.0]、y=[-1.0]）。
4. 建立用例矩阵（.md 表格 + .json 双形态）：覆盖 dtype×四 transpose 布局×维度域（B∈{1,2,16,64}、M/N∈{1,16,17,8192}、K∈{8,32,1000,8192} 且 8 倍规则）×非对齐尾块×全负行/边界值，含三官方示例锚点与规模约束（2²⁸ 上限）。
5. 实现 golden 参考：`golden_fp64_stdlib.py`（输入生成：fp16/bf16 量化模拟实际存储；FP64 bmm→amax→sum；输出 fp32 golden + fp64 精确值；内置三示例断言与 CLI 冒烟）；`golden_torch_ref.py`（torch fp64 参照、与 stdlib 结果 isclose 对拍，标注需 torch 环境，本地 import 缺失时友好跳过）。
6. 本地冒烟：penv 实跑 stdlib 三示例通过并留输出记录；随后双写记忆（2026-09-05.md 追加 P0 完成段 + MEMORY.md 父计划状态更新为 P0 完成/P1 未启动）。

## 目录结构（bmm_maxsum_assets\ 新建）

```
bmm_maxsum_assets/
├── README.md                       # [NEW] 资产目录说明：用途、文件索引、与父计划 P0 对应关系、如何扩展
├── docs/
│   ├── semantic_baseline.md        # [NEW] 语义基线文档（公式/torch 映射/四布局/精度判据/三示例推导）
│   ├── cube_datapath_notes.md      # [NEW] Cube 数据流与范式理解记录（GM→L1→L0A/L0B→L0C→UB、高阶 Matmul、Vector 归约；映射本题）
│   └── resources_index.md          # [NEW] 资源引用索引（三镜像教程/answer 源码/ascendc 记忆/skills/stage_goal/父计划；逐一核实路径）
├── cases/
│   ├── case_matrix.md              # [NEW] 用例矩阵（人读表格：shape/dtype/transpose/对齐/全负行/边界值/规模标记）
│   └── case_matrix.json            # [NEW] 用例矩阵（机器可读：字段含 id/logical_shape/storage_flags/dtype/tags/expect_ref）
└── golden/
    ├── golden_fp64_stdlib.py       # [NEW] 纯标准库 FP64 参考：fp16/bf16 量化、FP64 bmm→max→sum、fp32 golden；内置三示例断言；CLI 冒烟
    ├── golden_torch_ref.py         # [NEW] torch 对拍脚本：fp64 参照输出 + isclose 对比；无 torch 时友好提示跳过
    └── smoke_outputs/              # [NEW] 冒烟产物：stdlib 三示例运行输出（json/txt）留档
```

不触碰：题目模板工程、CANN_OpHelper 仓库、`.codebuddy` 既有文件。

## 实现要点

- 半精度量化：fp16 用 `struct.pack('e')/unpack('e')` 往返；bf16 手工实现 round-to-nearest-even 截取 fp32 高 16 位（注意次正规与舍入边界，三示例用简单值不触发即可，实现仍按规范写）。
- FP64 计算后转 FP32：`struct.pack('d')→unpack('f')` 舍入；输出同时保留 fp64 精确值与 fp32 golden（供 P3 判据使用）。
- 确定性：所有生成用例使用固定随机种子（沿用惯例 20260905）保证可复现；矩阵规模上限内避免过大内存（中小用例本地，大 shape 由 torch 脚本在云端跑）。
- 语言：正文中文，路径/API/术语保留英文；不使用 emoji。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose: P0 早期核实资源地图——定位官方教程三镜像中 04_matmul_basic / 05_fused_operator_development 的 ipynb 与 answer 源码（matmul_custom.cpp、matmul_abs.cpp 等）确切路径、ascendc 索引与 skills 结构，避免资源索引写入臆造路径。
- Expected outcome: 一份逐条核实存在的资源清单（绝对路径 + 一句话用途），直接支撑 docs/resources_index.md 与研读素材选择。