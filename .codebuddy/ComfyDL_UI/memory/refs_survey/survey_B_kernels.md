# Refs 调研报告 B：算子融合 / kernel 类（7 仓库）

> 调研日期：2026-10-30。源仓库位于 `P:\Dev\ComfyUI_Refs\Refs\`（浅克隆）。

---

## B1. comfyui-flex-attention（sorryhyun）

**定位**：单文件节点插件，把模型注意力替换为 PyTorch 原生 Flex Attention（torch.compile 编译），让不支持 FA4 的普通显卡也获得融合注意力加速，无额外依赖。

**节点**：仅 `FlexAttention`（__init__.py:75），MODEL 进 MODEL 出。

**核心实现**：`model_clone.model_options["transformer_options"]["optimized_attention_override"] = attention_override_flex`（:69），配合 `@comfy.ldm.modules.attention.wrap_attn`（:16）；有 mask 自动回退 `attention_pytorch`（:19-22）；`torch.compile(flex_attention)` 全局只编译一次（:14）。

**可借鉴**：MODEL-patch 模式可直接复用；`_flex_logged` 一次性日志模式（:24-27）适合 profiling 开销控制；保留 `skip_reshape/skip_output_reshape` 协议兼容内部调用约定；单文件 + 局部 NODE_CLASS_MAPPINGS 注册与 ComfyDL schema 一致。

**依赖/风险**：仅 PyTorch 2.5+；首次调用有编译 warmup；MIT；风险极低。

---

## B2. flash-attention-sm120-fix（sorryhyun）

**定位**：Dao-AILab flash-attention fork，修复 FA4（CuTeDSL 版）在 SM120（RTX 50 系）上的正确性问题并做 SM120 专属调优。库型仓库，非节点插件。

**结构**：修改集中在 `flash_attn/cute/`：`interface.py`（架构分发/调参）、`flash_fwd_sm120*.py`、`flash_bwd_sm120.py`、`pack_gqa.py`（bug 修复）；另有 `issue.md`（bug 分析）与 `test_sm120_nan.py`（数值诊断）。

**核心实现**：issue.md 记录 pack_gqa 在 SM80/SM120 非 TMA 路径 crd2idx 坐标/layout 不匹配崩溃，架构级 guard（interface.py:103-110）；SM120 受 99/101KB SMEM 约束按 D 维分支调 tile/stages，暴露 `FA4_SM120_*` 环境变量（:468-477, 1020-1039）；SDPA 作参照做 fwd/bwd NaN 与数值 diff 回归（test_sm120_nan.py）；docs/green_context_cfg.md 用实测**否定** green context 分 SM 跑 CFG（慢 2 倍），推荐 batched CFG。

**可借鉴**："issue.md + 最小复现 + 数值回归测试"调试闭环；环境变量覆盖调参模式（`os.environ.get(默认值)`）；SDPA 作 golden reference 的验证脚本；先 benchmark 否定复杂方案并记录负面结论的文档习惯；按 `arch // 10 in [8, 12]` 分发的兼容层写法。

**依赖/风险**：CUDA 12.0+、`nvidia-cutlass-dsl`（4.4.x 受 bug 影响）、需本地编译；目标 SM120/SM80；BSD 3-Clause 可安全借鉴。

---

## B3. flash-attention-rdna3（chelokot）

**定位**：AMD RDNA3（gfx1100，RX 7900 系）的 Triton FlashAttention-2 实现，长序列 causal 比 SDPA 快 5–15 倍。

**节点**：单一 `ApplyRDNA3FlashAttention`（fa_rdna3/comfyui.py:257-302，MODEL 进出）；库层含 `interface.py`、`sdpa.py`（全局 SDPA monkeypatch）、`kernels/`（forward/backward/varlen/decode/paged/tiny 六个 Triton kernel）、`_validation.py`、tests/（20+ 文件）。

**核心实现**：节点往 `optimized_attention_override` 挂 override（comfyui.py:271-293），reshape/GQA/mask 归一化后按 `_is_supported` 白名单分发自研 kernel，不满足即回退（:114-182）；支持与已有 override 组合（:211-248）与防递归 bypass 上下文管理器；`_validation.py` 集中能力校验 + `unsupported_reason()` 报错而非静默失败；split-K 触发条件写常量并注明测量依据（sdpa.py:28-49）。

**可借鉴**：override 链挂 + 组合/防递归；集中式能力校验报错模式；白名单分发 + 硬件实测阈值决策；完整 bench/pytest 对照 fp32 参考的测试体系（基准方法学可参考）；防重复加载同名包检查（__init__.py:7-23）。

**依赖/风险**：triton + 较新 torch（`torch.library.triton_op`）；仅 ROCm/gfx1100 实测；强绑定 ComfyUI attention override 接口签名。

---

## B4. ComfyUI-FixFlashAttnSchema（love530love）

**定位**：单文件加载期补丁，修复 ComfyUI 内置 `flash_attn_wrapper` 因 `torch.library.custom_op` schema 注册不完整导致断言失败、FlashAttention 静默回退 SDPA 的 bug（PyTorch issue #172944，torch 2.6–2.11.dev 复现）。

**组成**：仅 `__init__.py`（62 行）+ node.zip；不注册节点（`NODE_CLASS_MAPPINGS = {}`，:60）。

**核心实现**：import `comfy.ldm.modules.attention` 后 `hasattr` 防御性探测 `flash_attn_wrapper`/`flash_attn_func`（:37），前者替换为直连后者的普通函数（:38-43）绕开 custom_op 包装；失败打警告不崩溃（:46-57）。

**可借鉴**：与 ComfyDL `frontend_patch.py` 同范式（不改核心文件、加载期 monkeypatch、升级存活）；**hasattr 探测 + PATCHED 状态标志 + 缺失时列举具体符号名的降级报告**，值得补丁模块统一采用；README 中文双语"现象/根因/确认命令"可作 patch 类组件文档模板。

**依赖/风险**：零依赖；符号名/签名与 ComfyUI 版本强耦合；副作用是失去该路径 FakeTensor/torch.compile 追踪；属社区 workaround。

---

## B5. ComfyUI-QuantOps（silveroxides）——已弃用，仅参考

**定位**：为 ComfyUI 量化系统扩展 layout（INT8/FP8/MXFP4 加载）；已被 ComfyUI 官方吸收，仅作参考。

**节点**：QuantizedModelLoader / QuantizedUNETLoader / QuantizedCLIPLoader / QuantizedDualCLIPLoader / BNB4bitUNETLoader / EfficientVAELoader（loader_nodes.py L1287-1298），核心端口 `quant_format`（auto/int8/int8_tensorwise/float8_blockwise/rowwise/mxfp8/nvfp4）与 `kernel_backend`（pytorch/triton）。

**核心实现**：启动时调官方 `comfy.quant_ops.register_layout_class()` + 向 `QUANT_ALGOS` setdefault 注册，全部 try/except 懒加载降级（__init__.py L75-200）；`UnifiedQuantOps` mixin 单类混跑任意量化格式，经 `make_quant_ops(base_ops)` 注入 `custom_operations`（unified_ops.py L42+）；加载三路径：mmap 动态 VRAM / 并行线程 async / comfy 默认（loader_nodes.py L31-113）；auto 模式扫描 state_dict 判断量化格式（L245-259）。

**可借鉴**："注册官方注册表 + QUANT_ALGOS dict + 懒加载降级"三件套；`make_quant_ops(base_ops)` **叠加式注入**而非覆盖；state_dict 启发式探测格式。

**依赖/风险**：深度耦合 torch 2.10+/cu130 与 comfy 内部 API；README 明言废弃——只借鉴思路勿复用代码。MIT。

---

## B6. Veda-on-ComfyUI（veda-sparse）——本批对 ComfyDL 最有价值

**定位**：视频扩散（MiniMax-H3）学习型块稀疏注意力节点：蒸馏的小打分器预测哪些 128-token attention tile 重要，只算约前 10%，端到端提速 2.9 倍。

**节点**：单一 `VedaSparseAttention`（nodes.py L128-273，**V3 schema** io.ComfyNode + define_schema，MODEL 进出，高级输入默认隐藏）；predictor 注册到 `models/veda` 目录。

**核心实现**：经 `transformer_options["optimized_attention_override"]` 做 override，每步在 ON_PREPARE_STATE 重装，能识别并接管/拒绝与其他注意力节点组合（comfy_patch.py L1-30）；流水线 gather→pool→打分→top-k→block mask→kernel→scatter，按自由显存自适应 head 分块（core/engine.py L245-326，L35-45 实测调参注释）；fork SageAttention v1 Triton INT8 kernel 改 key 循环按保留 tile 列表行走；后端注册表 sm80+/gfx11-12→triton-int8、Apple→MLX，无回退链、不可跑直接报错。

**可借鉴**：a) "节点先自检、报错给修复指引而非静默降级"设计原则；b) **`PhaseTimer`（CUDA event 按阶段计时、结束一次性读取，engine.py L110-129）与设备端累计 Stats 可直接搬进 ComfyDL profiling**；c) override 每步重装 + patch 组合/冲突检测；d) predictor bundle 按 (mtime,size) 缓存；e) 双语错误信息 + 节点显示诊断文本。

**依赖/风险**：triton/triton-windows/mlx 按平台条件安装；要求 ComfyUI ≥0.38；kernel 硬绑定架构；代码 MIT，打分器权重受 MiniMax H3 Community License 约束（NOTICE.md）。

---

## B7. H3-Optimizations（Zironic）

**定位**：MiniMax H3 视频生成模型优化节点包（v0.2.46）：显存优化（分块/streaming）+ 稀疏注意力降峰值 VRAM 加速长视频（RTX 4070 实测 VRAM ~6GB→~2.9GB，稀疏 KV30 提速 ~1.6x）。

**节点/注册**：**不用 NODE_CLASS_MAPPINGS**，用 `comfy_entrypoint()` + `ComfyExtension.get_node_list`。5 节点：H3MemoryOptimization、H3AIMDOResidencyLimiter、H3SparseAttention(Advanced)、H3VSAAttention，均 MODEL→MODEL（V3 io.Schema），`ui.PreviewText` 回显状态。

**核心实现**：不可变 Plan dataclass（plan.py，PLAN_KEY 存 model_options，节点可叠加且顺序无关）+ apply.py 统一应用；patch.py 用 owner/signature/original 三标记管理 object_patches，**冲突时保留他人 patch 并禁用自身对应子优化**；多后端路由（Kitchen INT8/Sparse Sage/Triton/FP8 Flex/FROST）带 preflight 自检与失败回退；内存模块按 linear/embedding/final_layer/fused_residual 拆分分块。

**可借鉴**：a) Plan+标记的幂等可撤销 patch 模式；b) 冲突共存降级策略；c) 不兼容模型直接 passthrough+警告；d) 独立 `validate_inputs`；e) 防双加载 spec aliasing；f) README 严谨 benchmark 方法论（warmup 丢弃、中位数、隔离 text encoder）适合作 profiling 参考。

**依赖/风险**：无 pip 依赖但深度耦合 comfy.quant_ops/comfy-kitchen（要求 ComfyUI ≥0.33.0）；自带原生二进制限 Windows x64/Linux x86-64、NVIDIA SM75+；**未见 LICENSE**。

---

## 批次小结

1. **attention 替换的统一范式已完全收敛**：`model.clone()` + `optimized_attention_override`（+ `wrap_attn`），B1/B3/B6 与批次A的 A3/A4 全部一致——ComfyDL 的 attention patch 层照此实现即可。
2. **工程纪律亮点**：Veda 的 PhaseTimer/自检报错、H3 的 Plan/patch 标记/冲突降级、FixFlashAttnSchema 的 hasattr 探测降级报告，都是 ComfyDL patch 模块可直接吸收的模式。
3. **kernel 仓库价值主要在方法学**（数值回归、基准方法论、负面结论记录），代码本身硬件耦合深、不建议直接复用。
