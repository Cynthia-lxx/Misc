# Ascend C 知识索引

> 本索引是算子知识库的入口：主题文件地图 + 主题↔来源章节对照 + 检索指引。
> 所有要点以来源锚点（章节目录 / notebook / 源码相对路径）标注，写作与引用时回到官方原文。
> 文档基准路径：`Reference_And_Documentation\tutorials\ascendc_operator_development\`（主课）、`ascendc_operator_development_light\`（light）。

## 主题文件地图

| 主题 | 文件 | 覆盖内容 |
|---|---|---|
| ①编程范式 | `ascendc_01_programming_paradigm.md` | CopyIn→Compute→CopyOut、TPipe/TQue 生命周期、双/多缓冲、事件同步、核函数结构、头文件顺序 |
| ②算子模板 | `ascendc_02_operator_templates.md` | 逐元素/规约/Matmul（高阶API）/VV-CV 融合算子骨架与典型源码摘录 |
| ③Tiling | `ascendc_03_tiling_strategy.md` | Host Tiling 流程、多核/核内均分、32B 对齐、UB 约束、模板化 Tiling、Tbuf/Workspace |
| ④Host↔Device 协作 | `ascendc_04_host_device_collaboration.md` | 工程结构、算子原型/InferShape、Tiling 传递、aclnn/pybind 调用链、msopgen/CMakeLists |
| ⑤API 参考 | `ascendc_05_api_reference.md` | DataCopy/DataCopyPad、Vector 指令、Matmul 高阶 API、DumpTensor/printf、workspace API 签名与要点 |
| ⑥调试与错误 | `ascendc_06_debug_common_errors.md` | 对齐/越界/空洞等典型错误、CPU 孪生/NPU 上板调试、msProf 流程 |

## 章节 → 主题 对照表（主课 9 章）

| 章 | 目录 | 核心可沉淀主题 |
|---|---|---|
| 01 | `01_basic_overview` | 概念基础（AI/算子、CANN 架构、NPU 原理、Ascend C 概念）：索引性背景 |
| 02 | `02_AscendC_basic` | ①范式 ②Add 模板 ⑤API ④Host 直调（ACL）|
| 03 | `03_intermediate_vector_operator_development` | ③Tiling ④工程结构/原型/InferShape/workspace ⑤API（DataCopy 等）|
| 04 | `04_matmul_basic` | ②Matmul 模板 ⑤高阶 API |
| 05 | `05_fused_operator_development` | ②VV/CV 融合模板 ④多输入/属性 |
| 06 | `06_opensource_repo_operator_intro_and_contribution` | ④开源算子仓结构 ②开源仓算子实现 |
| 07 | `07_Troubleshooting` | ⑥调试（CPU 孪生/上板）|
| 08 | `08_performance_optimization` | ⑥msProf/仿真、性能优化要点 |
| 09 | `09_course_practice` | ②综合 vector 实践综合演练 |

## 章节 → 主题 对照表（light 4 章，差异优先）

| 章 | 目录 | 独有/补充内容 |
|---|---|---|
| 01 | `01_basic_overview` | 与主课 01 同源，不重复沉淀 |
| 02 | `02_AscendC_basic` | 02.05 泛化 Tiling、02.06 **PyTorch 下 Kernel 直调**（独有）、02.07/02.08 实践 |
| 03 | `03_simple_operator_practice` | 03.03 高阶 Matmul、03.05~03.07 CV 融合开发与实践（与主课 05 部分同源）|
| 04 | `04_debug` | 上板调试/性能工具/仿真，与主课 07-08 同源，核对差异 |

## 检索指引（写新算子时的查证路径）

1. 确定算子类型 → 查 ②模板文件 找同型骨架。
2. 定 Kernel 主流程 → 查 ①范式 确认 TPipe/TQue/同步模式。
3. 定切分策略 → 查 ③Tiling（含示例计算公式）→ 与 ④Host Tiling 代码配合。
4. 落实 API 细节 → 查 ⑤API 速查（签名/约束/头文件）。
5. 出错 → 查 ⑥典型错误清单与调试手段。

## 阅读进度速览

- [x] init-memory-base（两课程结构盘点完成，见日志；索引/骨架就绪）
- [x] 主课 01-02（概念+HelloWorld+范式+Add 已沉淀到主题①/②/④/⑤）
- [x] 主课 03（工程化+Tiling+workspace+模板化算子已沉淀到主题③/④/⑤）
- [x] 主课 04-05（Matmul 高阶 API + VV/CV 融合模板已沉淀到主题②/⑤）
- [x] 主课 06-07（开源仓/UT-ST 与调试方法已沉淀到主题⑥/④）
- [x] 主课 08-09（msProf/性能优化闭环已沉淀到主题⑥）
- [x] light 独有内容（PyTorch 直调/泛化模板/仿真已沉淀到主题④/⑥）
- [x] 终检（本索引与 MEMORY.md 已同步）

## 已读未沉淀（过程留痕，可回读）

- 章节介绍/练习类 notebook（如 01.01/01.02/01.05、02.01/02.05、04.01/04.04、05.01/05.05、06.01/06.03/06.05、07.01、08.01/08.05 等）及重复代码单元本轮按策略跳过或仅核对差异，不影响六大主题覆盖；详细"跳过说明"见 `2026-09-05.md` 各章小节。
