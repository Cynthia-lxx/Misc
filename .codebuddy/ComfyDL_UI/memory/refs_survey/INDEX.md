# Refs 参考库调研索引（2026-10-30）

对 `P:\Dev\ComfyUI_Refs\Refs\` 下 50 个 ComfyUI 生态参考仓库的深度调研。

- 调研方式：每仓库由探索子代理深入源码后汇总；结论均附绝对路径证据。
- 输出：本目录下 5 份分类报告（A–E）+ 本索引。
- 项目背景：ComfyDL_UI（ComfyUI 硬改的模块化深度学习工具箱，V1 自定义节点 schema + 前端 patch + profiling + 数据集 IO）。

| 报告 | 分类 | 仓库数 |
|---|---|---|
| survey_A_compute_speedup.md | 一、计算提速 | 14 |
| survey_B_kernels.md | 二、算子融合 / kernel | 7 |
| survey_C_ui.md | 3、UI | 11 |
| survey_D_utils.md | 4、utils | 11 |
| survey_E_perf_tools.md | 5、perf tools | 7 |

每份报告内每个仓库含：① 定位（是什么/解决什么）② 节点清单 ③ 核心实现与可借鉴代码 ④ UI/前端参考 ⑤ 对 ComfyDL 的可操作借鉴点 ⑥ 依赖与风险。每份报告结尾附"批次小结"提炼横切主题。

## 全局最有价值仓库（按 ComfyDL 相关性）

| 仓库 | 类别 | 核心价值 |
|---|---|---|
| ComfyUI-GGUF | A | 自定义 Tensor 子类 + lazy dequant，大模型 IO 范本 |
| ComfyUI-Attention-Optimizer | A | 探测/验证/基准/版本化缓存框架，profiling 驱动加速蓝本 |
| Comfy-WaveSpeed | A | set_model_unet_function_wrapper 官方 hook + FBCache 状态机 |
| Veda-on-ComfyUI | B | PhaseTimer CUDA event 计时、自检报错设计 |
| H3-Optimizations | B | Plan/patch 标记、冲突降级、benchmark 方法论 |
| ComfyUI-Enhancement-Utils | C | 事件拦截 profiler + 双渲染器徽章兼容，前端性能 UI 最佳参考 |
| comfyui-node-organizer | C | 纯函数布局引擎 + 官方扩展 API 全家桶 |
| lf-nodes | C | NODE_WIDGET_MAP 声明式节点→widget 映射 |
| ComfyUI_NNT + basic_data_handling + List-Utils + Data-Analysis | D | DATASET 类型/双列表语义/Pack-Unpack/ShowText 预览，四合一蓝本 |
| EternalKernelPytorchNodes | D | validate_inputs 包装放行自定义类型；GridSearchTraining |
| ComfyUI_ProfilerX + comfyui-benchmark + XPUSYS-Monitor + AMDMonitor | E | profiling 钩子选型、采集层骨架、落盘 schema、崩溃取证 |

## 已知风险红线（勿引入）

- 运行时 pip 自装（DistorchMemoryManager、LogicUtils）
- 无沙箱 exec（Notebook、List-Utils Exec、NNT 裸 eval）
- 独立端口进程做监控（Elegant-Resource-Monitor）
- patch 已废弃的 recursive_execute（tzwm profiler）
- license 待核：H3-Optimizations（无 LICENSE）、node-organizer（疑似 GPL）、Veda 打分器权重（MiniMax H3 Community License）

（状态：调研完成，2026-10-30，50/50 仓库覆盖）
