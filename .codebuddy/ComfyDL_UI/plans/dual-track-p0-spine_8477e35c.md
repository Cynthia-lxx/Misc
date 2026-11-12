---
name: dual-track-p0-spine
overview: 双主线大计划的第一步 CRAFT：SP-0 开销归因实测 + SP-1 P0 共享基建（观测总线/签名缓存/开关注册表 + watchdog 迁移）。全部代码尽量直接移植自 Refs 参考仓库（XPUSYS/Crystools/AMDMonitor/Model-Analytics），为后续 M-B/M-C 所有特性铺路；顺带补记另一 session 未备案的 App Panel 交付。
todos:
  - id: doc-baseline
    content: 落盘记忆调整（MEMORY.md 补 App Panel 备案、smallplans 调整结论），并跑全量冒烟确认新基线
    status: completed
  - id: sp0-attribution
    content: 执行 SP-0 开销归因实测：四组开关配置对照计时，归因报告落盘，给出 T6-1 立项建议
    status: completed
    dependencies:
      - doc-baseline
  - id: registry-cache
    content: 实现开关注册表与签名缓存服务（comfort/profiling 下新两文件，proflog 降级薄壳，现有开关迁移等价）
    status: completed
    dependencies:
      - doc-baseline
  - id: spine-bus
    content: 实现观测总线 app/spine（XPUSYS provider 协议移植 + CdlSnapshot 契约 + 单 WS 消息 + 控制路由）
    status: completed
    dependencies:
      - registry-cache
  - id: watchdog-migration
    content: 用 [subagent:code-explorer] 核验 watchdog 调用链后迁移 ProfilingWatchdog 骑总线，行为等价
    status: completed
    dependencies:
      - spine-bus
  - id: tests-docs-sync
    content: 新增 test_obs_spine.py 专项、复跑等价测试、真启动验证、更新文档与交付记录、F 盘同步 GPU 冒烟
    status: completed
    dependencies:
      - watchdog-migration
---

## 需求概述

双主线大计划 M-B 里程碑第二批（SP-2+SP-3 合批）：**attention 后端选优双形态节点 + torch.compile 包装族**，交付"生产力 + 教科书级"两套形态，并为生成栈/训练栈提供编译加速能力。

### 核心交付
1. **生产版 `CdlAttentionOptimizer`**（MODEL→MODEL）：枚举本机可用 attention 后端（探测+运行时验证），prompt 建立期一次性 benchmark（3 warmup + sync + perf_counter ×10），结果入签名缓存（miss 才跑、code_version 自动失效），按 min_speedup 门限自动应用最快后端（经 `set_model_optimized_attention` 官方写入点）；输出 MODEL + report（STRING）+ speedup（FLOAT）。
2. **教科书版 `CdlAttentionBackend`**（MODEL→MODEL）：手动 combo 选后端（SDPA/pytorch/flash/xformers/sage 等，动态探测可用性），passthrough 直通开关，info STRING 输出各后端原理讲解（memory hierarchy、为什么 flash 快）；docstring 完整教学。
3. **`CdlCompileModel`**（MODEL→MODEL）：torch.compile 包装（backend/mode/fullgraph/dynamic 参数），经中央编译缓存管理器 + `add_object_patch` 进补丁链（禁 A2 原地 setattr 缺陷），RunMeter dispatch 盲区在 docstring 明示（ADR-D4）。
4. **教学模板 `attention_lab.json`**：CheckpointLoader → AttentionBackend/Optimizer → 采样 → 输出，配 catalog 条目与 zh 描述。
5. 全部节点注册 Tier-1 开关并自我计量（`comfydl.attention_select` / `comfydl.compile`），双盘冒烟全绿。

### 边界
- 不做 T6-1（crash site 键检查廉价化）、不做 opgraph 的 torch.compiler.disable 守卫（归 T6-2）。
- 不引入前端改动（benchmark 触发在节点执行期，教学由 docstring+模板承载）。
- 视觉验收（缩略图/节点 UI）交用户；未经确认不 commit/push。


## 技术方案

### 架构位置与注册方式
- 新节点放 `comfy_extras/`（MODEL 协议层节点惯例区），**V3 风格**（io.ComfyNode + define_schema），由 nodes.py init_extra_nodes 自动加载——与 nodes_model_loaders.py 同层同风格。
- 中央编译管理器放 `comfy/compile_manager.py`（纯 Python，与 signature_cache 同层，可独立单测）。
- attention 选优节点经官方写入点 `comfy/model_patcher.py:692-698 set_model_optimized_attention()` 注入（内部写 `model_options["transformer_options"]["optimized_attention_override"]`，透传 container_function），配合 `attention.py:171-204 wrap_attn` 的官方读取链——不发明新注入通道。

### 移植对照（Refs → ComfyDL，行号级档案已就位）
| 组件 | 来源 | 改造点 |
|---|---|---|
| 后端探测/运行时验证 | A4 nodes.py:31-93, 219-241 | try-import + 真实小张量试跑（import 成功≠能跑），异常截断 100 字符 |
| 统一 wrapper | A4 :255 + B1 __init__.py:69 | `__wrapped__` 防二次 wrap + 显式幂等标记（修 A4 缺陷） |
| benchmark 骨架 | A4 :470-505 | 3 warmup + cuda sync + perf_counter ×10 取均值；合成张量参数作为节点 widgets |
| 缓存 | A4 cache.py:14-85 | 改用 SP-1 `JsonNamespaceCache("attention_bench")`（user 目录、原子写、mtime、code_version 失效），弃插件目录 JSON |
| speedup 报告 | A4 :841-896 | 排序渲染 + `_info` 版本指纹（triton/torch/GPU 名） |
| compile 包装 | A10 fbcache_nodes.py L10-82 | `add_object_patch("diffusion_model", compiled)` 进补丁链 + `@torch.compiler.disable()` 守卫判定 |
| compile 参数形态 | A2 torch_compile_vae.py | backend(inductor/cudagraphs...)/mode/fullgraph/dynamic 下拉与布尔 |

### 中央编译管理器（comfy/compile_manager.py）
- `compiled_copy(module, **kwargs) -> module`：key = (id(module), 编译参数 hash)；weakref.ValueDictionary 防对象复活；命中直接复用；miss 调 torch.compile 后登记。
- 显式拒绝原地改写（A2 缺陷）：文档写明，代码审查点。
- 开关注册：`comfydl.compile`（self_meter=True），tick 计入 record_self_overhead。

### Tier-1 开关与元 profiling
- 两个节点族注册 `comfydl.attention_select` / `comfydl.compile`（self_meter=True）；benchmark/apply/编译耗时计入 record_self_overhead；describe() 供未来面板。
- benchmark 仅在 prompt 建立期、缓存 miss 时执行（per-run 位置，成本模型 Tier 0）。

### 模板与文档链
- `comfydl/example_workflows/attention_lab.json`（子模块）+ `app/template_catalog.py` `_COMFYDL_CATEGORY` 条目（en+zh L243-279）+ 缩略图占位（用户后补真图）。
- 文档：FUNCTIONS/README 四件套按 `_update_readme.py` 是否覆盖 comfy_extras V3 节点决定跑或手补；模板坑照单全收（seed 隐形 control_after_generate 槽补 "fixed"、widgets_values 按 INPUT_TYPES 序、required 槽不留空）。

### 冒烟 runner 适配
- run_smoke_test.py SKIP map 新增 MODEL 输入节点条目（仿 GetImageSize 先例 L1036-1040："covered by test_attention_select.py / test_compile.py"）。

### 测试矩阵
- test_attention_select.py（约 20 项）：假 import 环境探测、假计时器 benchmark、缓存命中/miss/code-version 失效、真 ModelPatcher+假模型 apply 后 transformer_options 断言、passthrough、V3 schema、自计量、幂等防重复包装。
- test_compile.py（约 12 项）：mock torch.compile 缓存命中、weakref 失效重编译、add_object_patch 注册断言、参数钳制、自计量、schema。
- 模板：test_template_workflows（清单 L127-129 + 结构断言）与 catalog 断言覆盖 attention_lab。
- 双盘：CPU 节点冒烟基线不回退（节点总数上移）→ F 盘 GPU 复跑 + attention 真实 speedup 合理性抽查。

### 性能与风险控制
- benchmark 在 CPU 盘自动降级：非 CUDA 环境跳过 GPU 后端、仅对比可用后端（避免 P 盘假死）。
- 缓存键含设备指纹（GPU 名/torch 版本），跨机不串。
- 提交顺序：先子模块（模板+文档）后父仓（节点+测试），未经确认不 push。
	

## Agent Extensions
### SubAgent
- **code-explorer**
  - Purpose: 实施期核验移动目标——A4 wrapper/benchmark 段的精确移植边界、V3 define_schema 在 nodes_model_loaders.py 的实际写法范本、test_template_workflows 模板清单断言的精确结构
  - Expected outcome: 每次调用产出当前代码行号级锚点，防止凭档案行号改动过期代码
