---
name: 核心课程通读与AscendC知识沉淀
overview: 系统精读 Reference_And_Documentation 下 ascendc_operator_development（9章）与 ascendc_operator_development_light（4章）的全部核心 notebooks 与源码，按用户要求的六大知识主题归纳沉淀为 .codebuddy/memory/ 下多主题长期记忆文件，为后续 CANN 算子工程开发提供"文档优先"的知识底座。
todos:
  - id: init-memory-base
    content: Use [subagent:code-explorer] to inventory notebooks and source files of both courses, then create MEMORY.md, daily log, topic file skeletons, and knowledge index
    status: completed
  - id: read-full-ch1-2
    content: Read full-course chapters 01-02 (concepts, HelloWorld, paradigm, Add kernel) and distill paradigm, host-device, API topics with source anchors
    status: completed
    dependencies:
      - init-memory-base
  - id: read-full-ch3
    content: Read full-course chapter 03 notebooks and SRC sources; write Tiling/engineering topic and supplement API topic
    status: completed
    dependencies:
      - read-full-ch1-2
  - id: read-full-ch4-5
    content: Read full-course chapters 04-05 (Matmul high-level API, VV/CV fusion) and complete operator template and API topics
    status: completed
    dependencies:
      - read-full-ch1-2
  - id: read-full-ch6-7
    content: Read full-course chapters 06-07 (open-source repo, UT/ST, debugging typical issues) and finish debugging topic plus daily log
    status: completed
    dependencies:
      - init-memory-base
  - id: read-full-ch8-9
    content: Read full-course chapters 08-09 (msProf, simulation, perf demos, vector practice) and append performance/template topics
    status: completed
    dependencies:
      - init-memory-base
  - id: read-light-diff
    content: Read light-course unique parts (PyTorch kernel call, vector/Matmul/CV practices) diff-first, reuse full-course notes
    status: completed
    dependencies:
      - read-full-ch1-2
      - read-full-ch4-5
  - id: finalize-verify
    content: Cross-check six topic files for completeness and source anchors, update index and MEMORY.md, output static self-check summary
    status: completed
    dependencies:
      - read-full-ch1-2
      - read-full-ch3
      - read-full-ch4-5
      - read-full-ch6-7
      - read-full-ch8-9
      - read-light-diff
---

## 需求概述

作为 CANN 编程开发专属助手，首要任务是**系统性精读官方参考文档**，并把关键知识沉淀为可长期复用的记忆，供后续所有算子工程开发"文档优先"地调用。

## 核心特性

- **阅读范围（已确认）**：仅精读 `Reference_And_Documentation\tutorials\` 下两大核心课程——完整教程 `ascendc_operator_development`（9 章：基础概念、AscendC 基础、Vector 算子工程化与 Tiling、Matmul、融合算子、开源算子仓、调试、性能优化、综合实践）与精简教程 `ascendc_operator_development_light`（4 章，Kernel 直调路线）；其余目录（quick_start/blogs/skills 等）本轮不精读，后续按需查阅。
- **沉淀六大知识主题**：① CopyIn→Compute→CopyOut 编程范式核心模式（TPipe/TQue、双缓冲、事件同步）；② 常见算子类型实现模板（逐元素、规约、Matmul、VV/CV 融合）；③ Tiling 设计原则与常用计算（多核均分、32B 对齐、UB 约束、Tbuf/Workspace）；④ Host 侧（算子原型/InferShape/Tiling）与 Device 侧（Kernel）协作关系与工程结构；⑤ 常用 API 使用方式与签名要点（DataCopy/Add/Matmul/DumpTensor/printf 等）及头文件顺序；⑥ 典型错误模式与调试方法（CPU 孪生/NPU 上板/msProf）。
- **记忆组织（已确认）**：在 `.codebuddy\memory\` 下按主题拆分多文件（6 个主题文件 + 1 个索引），另建 MEMORY.md 与当日日志；每条要点标注来源（章节/notebook/源码文件路径）。
- **质量约束**：与官方原文逐字核对 API 与范式，禁止臆造；文档矛盾或模糊处停下询问用户；精简课程与完整课程重叠内容采用"差异优先"策略复用沉淀，避免重复劳动。
- **环境边界**：本地无 CANN 套件/NPU/编译器，仅做静态文本阅读与知识文件编写，不编译、不运行、不生成可执行内容。

## 技术选型

- 纯本地静态阅读与 Markdown 知识文件编写，无代码编译/运行；文件操作仅限 `.codebuddy\memory\` 目录。
- 读取对象：`.ipynb`（JSON 文本，含 markdown 与代码单元，直接 read_file 读取）、章节内 SRC 源码（.cpp/.h/.sh/.py/.asc）、章节 README/.md；必要时读取架构示意图 PNG。
- 输出格式：中文 Markdown，主题文件 + 每日日志 + MEMORY.md。

## 实施方案

### 执行策略

- **阅读顺序**：完整课程按 01→09 章推进；每章先读全部 notebook 文本与代码单元，再精读该章被 notebook 引用的 SRC 关键源码，最后核对章节 README 澄清知识地图。
- **写入节奏**：每完成一章（或同主题几章）立即增量更新对应主题文件与 `2026-09-05.md` 日志，防止信息滞留上下文。
- **去重策略**：`_light` 课程的 01 章、调试与性能相关章节与完整课程同源，只做差异核对并引用完整课程笔记；重点精读 light 独有内容（02.06 PyTorch 下 Kernel 直调、02.07/02.08 核函数与 Vector 实践、03 章 Matmul/简单算子/CV 融合实践）。
- **静态自检**（写入前执行）：主题是否覆盖六大主题；API 名称/签名、头文件顺序、Tiling 字段是否与官方示例一致；每条要点是否带来源路径。

### 记忆文件目录结构（全部 [NEW]，位于 .codebuddy\memory\）

```
.codebuddy/memory/
├── MEMORY.md                            # [NEW] 本工作区稳定长期记忆：环境硬约束（无编译器/无CANN/禁运行）、工程规约、记忆文件地图与使用约定。
├── 2026-09-05.md                        # [NEW] 当日阅读进度日志：逐章记录阅读范围、沉淀要点与待核对疑问，作为过程追溯。
├── ascendc_knowledge_index.md           # [NEW] 知识索引：六主题文件映射、主题→来源章节对照表、速览与检索指引。
├── ascendc_01_programming_paradigm.md   # [NEW] 主题①编程范式：CopyIn→Compute→CopyOut、TPipe/TQue 生命周期、双/多缓冲、事件同步、核函数结构模板、头文件包含顺序。
├── ascendc_02_operator_templates.md     # [NEW] 主题②算子模板：逐元素（Add 等）、规约、Matmul（高阶 API/MIX/Cube）、VV/CV 融合的 Kernel 实现骨架与典型源码摘录。
├── ascendc_03_tiling_strategy.md        # [NEW] 主题③Tiling：Host 侧 Tiling 流程、多核/核内均分、32B 对齐与 UB 容量约束、泛化/模板化 Tiling、Tbuf/Workspace 分配策略与计算公式。
├── ascendc_04_host_device_collaboration.md # [NEW] 主题④协作关系：op_host/op_kernel 工程结构、算子原型注册、InferShape、Tiling 经 workspace 传 Kernel、aclnn/pybind 调用链、msopgen/CMakeLists 骨架。
├── ascendc_05_api_reference.md          # [NEW] 主题⑤API 参考：DataCopy/DataCopyPad、Add/Mul 等 Vector API、Matmul 高阶 API、DumpTensor/printf、SetUserWorkspace 等签名要点、约束与代码示例来源。
└── ascendc_06_debug_common_errors.md    # [NEW] 主题⑥调试与错误：对齐/越界/空洞等典型错误清单、CPU 孪生与 NPU 上板调试流程、printf/DumpTensor 用法、msProf 性能定位路径。
```

### 质量与一致性保障

- 每条要点带来源锚点（如 `ascendc_operator_development/02_AscendC_basic/02.03_...ipynb` 或具体 SRC 文件），保证后续可回查官方原文。
- 主题文件中只收录经原文核实的 API/范式；不确定内容以"待核实"标记并汇总询问用户，不写入定论。
- 精简课程与完整课程共享的知识只写一份，跨文件用索引交叉引用，避免漂移。

## Agent 扩展

### SubAgent

- **code-explorer**
- 用途：在阅读开始前对两大课程各章的 notebook 与 SRC 源码文件做目录盘点，产出一份"章节→待读文件清单"映射，供后续逐章精读直接定位，避免主流程反复探查目录。
- 预期结果：`ascendc_knowledge_index.md` 与 `2026-09-05.md` 中落地可执行的两课程逐章文件清单（含关键 SRC 文件与 notebook 对应关系）。