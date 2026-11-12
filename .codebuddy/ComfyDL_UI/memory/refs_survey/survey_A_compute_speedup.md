# Refs 调研报告 A：计算提速类（14 仓库）

> 调研日期：2026-10-30。源仓库位于 `P:\Dev\ComfyUI_Refs\Refs\`（浅克隆）。
> 背景：为 ComfyDL_UI（V1 自定义节点 schema、前端 patch、profiling、数据集 IO）提取可借鉴内容。

---

## A1. ComfyUI_TensorRT（comfyanonymous 官方）

**定位**：把 UNet/扩散模型导出 ONNX 再编译为 TensorRT 引擎并回接 ComfyUI 推理，换取显著加速。

**节点**：`DYNAMIC_TRT_MODEL_CONVERSION` / `STATIC_TRT_MODEL_CONVERSION`（tensorrt_convert.py:391/559，输入 MODEL + min/opt/max batch/height/width/context，OUTPUT_NODE 无输出端口）；`TensorRTLoader`（tensorrt_loader.py:113，unet_name + model_type → MODEL）。

**核心实现**：`unload_all_models()` → `load_models_gpu` 强载 → `UNET` wrapper 固定 forward 签名 → `torch.onnx.export`（opset 17, dynamic_axes）→ TRT build_serialized_network（tensorrt_convert.py:132-388）。引擎文件名编码 shape profile（`name#min#opt#max;`，:318-372）；**timing cache** 持久化 `timing_cache.trt`（:101-130）二次编译提速。加载端 `TrTUnet` 按 max batch 分片 `execute_async_v3`（loader:38-104）；`disable_unet_model_creation` 造空壳 model_base + `memory_required=lambda: 0` 促使 batch 合并（:123-172）。

**可借鉴**：`folder_paths.folder_names_and_paths["tensorrt"]` 自定义模型目录注册模式；timing cache 持久化（可平移到 triton/inductor 编译缓存管理）；shape 编码进文件名的缓存键设计；`OUTPUT_NODE=True` 转换节点形态。

**依赖/风险**：`tensorrt>=10.0.1`、`onnx!=1.16.2`；强耦合 comfy 内部 API，SD3/Flux 分支硬编码 context 维度；MIT。

---

## A2. ComfyUI-Torch-Compile（yondonfu）

**定位**：极简 torch.compile 包装节点，加速 VAE 与 ControlNet（源自 KJNodes）。

**节点**：`TorchCompileLoadVAE`（VAE + backend(inductor/cudagraphs) + fullgraph + mode + compile_encoder/decoder → VAE，nodes/torch_compile_vae.py:6）；`TorchCompileLoadControlNet`（→ CONTROL_NET）。

**核心实现**：原地 `setattr` 替换子模块——VAE 对 `first_stage_model.encoder/decoder`（兼容 taesd_ 前缀，:50-79），ControlNet 替换 `control_model` 并用实例级 `_compiled` 布尔幂等防重编译（:37-43）。

**可借鉴**：backend/mode/fullgraph 下拉参数的 V1 schema 写法；"编译一次+幂等标记"模式（ComfyDL 引入时应升级为模块级模型缓存字典，key=id+编译参数 hash，避免原地改写的副作用）；taesd 子模块名兼容写法。

**依赖/风险**：仅 torch；原地改写传入对象有副作用；MIT。

---

## A3. ComfyUI-DN_PatchFlashAttention

**定位**：单节点，把模型 attention 运行时替换为 Flash Attention 2。

**节点**：`PatchFlashAttentionDN`（patch_flash_attention.py:54，MODEL + enabled(BOOLEAN) → MODEL，False 直通）。

**核心实现**：走官方注入点——`model.clone()` 后写 `model_options["transformer_options"]["optimized_attention_override"]`（:90-95），替换函数用 `comfy.ldm.modules.attention.wrap_attn` 装饰匹配签名，`__wrapped__` 防重复 wrap；处理 fp32→fp16 强转与 BHSD/BSHD 两种布局；mask 静默丢弃仅警告。

**可借鉴**：**"clone + transformer_options 注入"是最干净、可逆、作用域限于模型分支的 patch 模式**，优于改全局变量；`enabled=False` 直通便于 A/B profiling。

**依赖/风险**：硬依赖 `flash_attn`（Windows 编译麻烦）；仅 fp16/bf16；对 comfy attention 签名有版本耦合。

---

## A4. ComfyUI-Attention-Optimizer（D-Ogi）

**定位**：自动基准测试型 attention 优化器——枚举本机所有 attention 后端（SDPA/xFormers/SageAttention 变体/FlashAttention）实测耗时，自动应用最快者，结果持久化。

**节点**：`AttentionOptimizer`（nodes.py:675/900，MODEL + 可选 backend 下拉/force_refresh/auto_apply/seq_len/num_heads → MODEL + report 等 STRING×4 + speedup/time_ms FLOAT×2 + head_dim INT）。

**核心实现**：逐后端 try-import 探测（:31-93）+ **运行时验证**（validate_backend 用真实小张量试跑，检测"静默回退"，:219）；统一 wrapper `get_attention_function`（:255）带 @wrap_attn；基准=合成 q/k/v，3 次 warmup + `torch.cuda.synchronize()` + `perf_counter`（:470-505）；持久缓存 cache.py 按"模型指纹_md5_dtype_head_dim"键存 benchmark_db.json，带版本号失效（cache.py:14-85）；应用同样走 clone + optimized_attention_override（:617-632）。

**可借鉴**：与 ComfyDL profiling 高度契合——"warmup + CUDA sync + perf_counter"计时骨架、speedup 报告生成（_build_report，:841）、版本化 JSON 缓存键设计；"import 成功 ≠ 真能跑"的二段式依赖检测。

**依赖/风险**：可选 xformers/sageattention/flash_attn/triton，与 GPU 架构强耦合（sage3 仅 Blackwell）；合成张量基准与真实负载分布可能不符。

---

## A5. ComfyUI-INT8-Fast（BobJohnson24）

**定位**：扩散模型 INT8 逐张量（W8A8）量化与推理加速，支持在线量化、LoRA 烘焙、INT8 模型保存。

**节点**：`OTUNetLoaderW8A8`（unet_name/weight_dtype/on_the_fly_quantization/enable_convrot/lora_mode → MODEL）；`INT8PreLoraLoader`（→ 自定义类型 PRE_LORA，int8_unet_loader.py:290）；`INT8GroupedLora`（MODEL + 10 组 lora/strength → MODEL）；`INT8ModelSave`（OUTPUT_NODE）。

**核心实现**：向 `comfy.quant_ops` 注册 `Int8TensorwiseLayout` 与 `QUANT_ALGOS["int8_tensorwise"]`（__init__.py:18-61）；`Int8TensorwiseOps(manual_cast)` 重写 Linear/Conv/Norm（int8_quant.py:173），前向动态逐 token 激活量化 + `torch._int_mm`（:108-120），有 Triton 走融合 kernel；`convrot.py` 分组 Hadamard 旋转压制离群值。

**可借鉴**：`QUANT_ALGOS`+Layout 可插拔量化注册模式；`excluded_names` 按模型类型豁免敏感层；Triton/`torch._int_mm` 双路径降级。

**依赖/风险**：强耦合新版 comfy `quant_ops`；模型类型白名单硬编码易过时。

---

## A6. ComfyUI-GGUF（city96）

**定位**：让 ComfyUI 直接加载 GGUF 量化 UNet/CLIP，低显存下按需逐层反量化推理。

**节点**：`UnetLoaderGGUF`/`UnetLoaderGGUFAdvanced`（unet_name → MODEL，nodes.py:321-328）；`CLIPLoaderGGUF` 双/三/四联 CLIP 变体（→ CLIP）；注册 `unet_gguf`/`clip_gguf` 目录键（nodes.py:32-33）。

**核心实现**：`GGUFReader` mmap 逐 tensor 读 uint8 包成自定义 tensor 子类 `GGMLTensor`（ops.py:44-91，携带 tensor_type/tensor_shape/patches，重写 to/clone/shape）；纯 torch 位运算逐块反量化（dequant.py:287-301，失败回退 numpy）；`GGMLOps(manual_cast)` 覆写 Linear/Conv/Embedding/LayerNorm，forward 前 cast_bias_weight 即时 dequant（ops.py:227），并 monkeypatch `patch_weight_to_device` 免反量化打 patch（nodes.py:35-78）；ops 经 `model_options={"custom_operations": ...}` 注入。

**可借鉴**：**自定义 Tensor 子类 + lazy dequant 模式**（显存按层解压，ComfyDL 大模型 IO 可复用）；mmap 逐张量读取；"fake state_dict + meta tensor" VRAM 估算技巧（ops.py:145-160）；sd_map_replace 键名重映射表适配多架构。

**依赖/风险**：仅 `gguf>=0.13.0`；torch 2.8 以下需禁 torch.compile（ops.py:21-42）；Windows 页文件尖峰有专门 workaround（ops.py:234-240）。

---

## A7. ComfyUI_Kitchen_nvfp4_Converter（tritant）

**定位**：离线权重转换节点，把 safetensors 权重转 NVFP4/MXFP8/INT8/INT4/W4A8 等低比特格式。

**节点**：仅 `ConvertToNVFP4`（convert_to_nvfp4_node.py:298，model_name + output_filename + model_type(19 种预设) + quant_format + device → STRING status，OUTPUT_NODE）。

**核心实现**：逐层遍历 state_dict（:180-277）：黑名单层（嵌入/norm/调制层，per-model_type 预设表 L103-160）保留 bf16；二维 weight 调 comfy-kitchen Layout 的 quantize；float8/e8m0 先 `.view(torch.uint8)` 再回转保证 safetensors 可序列化（L263-267）；量化格式 JSON 写进 metadata `_quantization_metadata`（L279-291）；失败自动降级 bf16（L271-273）。

**可借鉴**："黑名单白名单 + per-layer 量化映射写进 safetensors metadata"管线；dtype view(uint8) 序列化兼容技巧；`comfy.utils.ProgressBar` 绑定节点进度。

**依赖/风险**：强依赖第三方 `comfy-kitchen`；输出格式只能被 comfy-kitchen 识别。

---

## A8. ComfyUI-FSampler（obisin）

**定位**：自定义采样器套件，核心"步间跳过"：用外推预测器（线性/Richardson/h4）代替部分模型前向调用加速采样，兼容官方 KSampler 语义。

**节点**：`FSamplerAdvanced`（MODEL/CONDITIONING×2/LATENT + 30 余参数 → LATENT, STRING(metadata)，nodes.py:618-622）；`FSampler`（简化版）；`FSamplerSelect`（→ SAMPLER, SIGMAS，可喂 KSamplerCustom）。

**核心实现**：**工厂函数返回 `comfy.samplers.KSAMPLER(闭包)`**（sampling/engine.py:192-1039）——用自己的闭包函数注册为官方 KSAMPLER，再经 `comfy.sample.sample_custom()` 跑官方管线，guider/cfg/预览回调全复用官方；跳过逻辑在闭包内维护 error/epsilon 历史；per-run 统计元数据经 thread-local 回传（L1083）；comfy_copy/ fork 官方 k_diffusion 与 res4lyf 调度器做兼容。

**可借鉴**：**"闭包→KSAMPLER→sample_custom"是对自研 sampler 最干净的 hook 方式**（不 patch 源码、与 SamplerCustomAdvanced 天然兼容）；thread-local 回传统计适合 profiling；安全预览回调（失败降级纯进度条，nodes.py:10-26）。

**依赖/风险**：仅 comfy/torch 本体，但 comfy_copy/ 是官方快照，内核 API 升级需同步维护；engine.py 复杂度极高，建议只借鉴结构。

---

## A9. ComfyUI-TeaCache（welltop-cn）

**定位**：TeaCache 加速：采样时跳过"变化不大"的整个 forward（复用上一步残差），支持 FLUX/HunYuan/Wan/LTXV/HiDream 等原生模型及 diffusers 版 CogVideoX。

**节点**：`TeaCache`（MODEL + model_type 系数下拉 + rel_l1_thresh(FLOAT 0.4) + start/end_percent + cache_device → MODEL，nodes.py L1063）；`CompileModel`；`TeaCacheForCogVideoX`。

**核心实现**：clone 后把参数写进 `model_options["transformer_options"]`，再用 `unittest.mock.patch.multiple` 把 `diffusion_model.forward_orig` 绑定为自定义函数（nodes.py L862-881）；判定取第一个 double block 的 img_mod 调制输出累加 `poly1d(coeffs, 相对L1差)`（L86-106），超阈值才真正计算，否则 `img += previous_residual`；每模型多项式系数表 L17-34。

**可借鉴**：transformer_options 传参 + mock.patch 替换 forward 的非侵入范式；"rel_l1_thresh 单旋钮控跳过力度 + 每模型预拟合系数表"的参数设计。

**依赖/风险**：仅 torch/einops；每种模型需手写 forward 复制体，与 comfy 模型结构强耦合、版本易碎。

---

## A10. Comfy-WaveSpeed（chengzeyi）

**定位**：First-Block-Cache（FBCache）+ 量化/编译加速套件：只跑第一个 block，若其输出与上一步残差相似则跳过剩余所有 block。

**节点**：`ApplyFBCacheOnModel`（model + object_to_patch + residual_diff_threshold + start/end + max_consecutive_cache_hits → MODEL，fbcache_nodes.py L10-82）；`EnhancedLoadDiffusionModel`/`EnhancedCompileModel`；Velocator 系列量化节点（需 xelerate 包）。

**核心实现**：`model.set_model_unet_function_wrapper()` 挂 wrapper（fbcache_nodes.py L293）管理每 tick 缓存状态（ensure_cache_state 处理 CFG 多块调用/序列号，L130-162）；对通用 DiT 用 `mock.patch.object` 临时换 `CachedTransformerBlocks`（L245-291）；判定=第一 block 残差相对 L1 差 < 阈值（first_block_cache.py L113-121）；patch `execution.get_output_data` 清缓存上下文。

**可借鉴**：**`set_model_unet_function_wrapper` 官方 hook 点比 mock forward 更稳**；`max_consecutive_cache_hits` + sigma 区间校验的细粒度跳过参数设计；`@torch.compiler.disable()` 保护判定函数兼容 torch.compile；运行时探测 blocks 属性名（transformer_blocks/double_blocks/joint_blocks）做通用适配。整体耦合比 TeaCache 低，缓存状态机设计更适合 ComfyDL 参考。

**依赖/风险**：类名硬编码（"UNetModel"/"Flux"等）；全局 `_current_cache_context` 非线程安全。

---

## A11. ComfyUI-nunchaku（nunchaku-tech 官方）

**定位**：SVDQuant/nunchaku 4-bit 推理引擎官方插件，接入 FLUX/Qwen-Image/Z-Image 量化 DiT，解决低比特模型加载、LoRA、缓存与低显存运行。

**节点**（__init__.py L79-162，逐个 try-import 防御式注册，仍是 V1 schema）：加载器 `NunchakuFluxDiTLoader`/`NunchakuQwenImageDiTLoader`/`NunchakuZImageDiTLoader`（模型路径 + attention/cpu_offload/data_type → MODEL）；`NunchakuTextEncoderLoader(V2)`（→ CLIP/T5）；LoRA `NunchakuFluxLoraLoader/Stack`；PuLID/IPAdapter；工具 `NunchakuModelMerger`/`NunchakuWheelInstaller`；预处理 `NunchakuDepthPreprocessor`。

**核心实现**：`NunchakuFluxTransformer2dModel.from_pretrained(offload=…)` 读 svdq safetensors（nodes/models/flux.py L254）；`ComfyFluxWrapper(nn.Module)` 持有引擎模型，forward 内做 patch 化输入/LoRA 组合/FB 缓存，塞进 `model.diffusion_model` + `ModelPatcher`（wrappers/flux.py，flux.py L304-314）；offload auto=显存<14/15GB 阈值判定，Qwen 支持 `num_blocks_on_gpu`/pin_memory 细粒度换入换出（qwenimage.py L146-166）；`to_safely` 拦截 comfy 的 `.to()` 保护量化权重（mixins/model.py）；新路线（Z-Image）直接继承 `ModelPatcher`（model_patcher/zimage.py L95）深度接管。

**可借鉴**：**"引擎模型 + nn.Module wrapper + 借用 ModelPatcher 壳"的 adapter 模式**——外部推理引擎无需实现全部权重接口；offload auto 阈值 + block 级 GPU 驻留数参数；防御式逐节点注册；`to_safely` 拦截机制。

**依赖/风险**：nunchaku 引擎按 torch×CUDA SM 预编译 wheel，在线装 wheel 节点，耦合极重；插件要求 nunchaku≥1.0 并初始化即校验版本、probe GPU sm；license 需向上游确认（MIT）。

---

## A12. ComfyUI-wanBlockswap（orssorbit）

**定位**：Wan2.1 轻量 block swap 节点（借鉴 kijai），部分 transformer block 常驻 CPU 降 VRAM。

**节点**：`WanVideoBlockSwap`（nodes.py，MODEL + blocks_to_swap(INT 0-40) + offload_img_emb/offload_txt_emb/use_non_blocking → MODEL）。

**核心实现**：非执行期换入换出，而是 `model.clone()` + `add_callback(CallbacksMP.ON_LOAD, swap_blocks)`（nodes.py:48-49）在模型加载时一次性分配：前 N 个 block `.to(offload_device)`，其余 `.to('cuda')`（:33-43）；换出后 `soft_empty_cache()` + `gc.collect()`（:45-46）。

**可借鉴**：`CallbacksMP.ON_LOAD` 挂回调做 swap，无需侵入核心；non_blocking 传输选项（快但费 RAM）。

**依赖/风险**：硬编码 `isinstance(base_model, WAN21)` 与 cuda 单卡；静态布局无执行时双向流水。

---

## A13. ComfyUI-DistorchMemoryManager（ussoewwin）

**定位**：综合显存管家（v2.5.0）：节点式清理 + NVML 感知非 PyTorch 占用 + sage attention 运行时补丁。

**节点**：`MemoryManager`（any→any，clean_gpu/clean_cpu/force_gc/reset_virtual_memory 开关）、`DisTorchPurgeVRAMV2`（可选清理 SeedVR2/Qwen3-VL/Nunchaku/Ollama 等，nodes/purge_vram.py:30-43）、`PatchSageAttentionDM`（MODEL→MODEL，__init__.py:255-278）。

**核心实现**：启动补丁测 NVML 系统占用 − torch 占用 = 非 PyTorch VRAM，写入 `mm.EXTRA_RESERVED_VRAM`（__init__.py:166-194）让调度感知浏览器等外部占用；强制卸载用 `mm.free_memory(1e30, device)`（free_memory(0) 是 no-op，memory_manager.py:104-118）；多设备循环 `empty_cache()+ipc_collect()`；直改 `current_loaded_models` 卸载 patch 模型 + `cleanup_models_gc()`（purge_vram.py:71-124）。

**可借鉴**：EXTRA_RESERVED_VRAM 修正法可移植进 ComfyDL profiling；统一 purge 次序 `gc.collect → empty_cache → ipc_collect → synchronize`；monkey-patch 函数上打 `_dm_sage160_guard` 防重复补丁的 guard 模式。

**依赖/风险**：依赖 pynvml 且启动期自动 pip 升级（有风险）；purge_vram.py 270KB 深度依赖 comfy 私有结构，版本耦合大。

---

## A14. Comfyui-Memory_Cleanup（LAOGOU-666）

**定位**：VRAM/RAM 双清理节点，特色是能清**系统级 RAM**（文件缓存、进程工作集、DLL）。

**节点**：`VRAMCleanup`（offload_model/offload_cache，OUTPUT_NODE）、`RAMCleanup`（clean_file_cache/clean_processes/clean_dlls/retry_times）（__init__.py:155-158）。

**核心实现**：VRAM 侧 `mm.unload_all_models()` + `soft_empty_cache()` + `PromptServer.instance.prompt_queue.set_flag("free_memory", True)`（:51-56）借队列标志让调度器在节点间卸载；RAM 侧 ctypes 调 `SetSystemFileCacheSize`/`EmptyWorkingSet`/`SetProcessWorkingSetSize`，Linux 用 `malloc_trim`，带重试与前后 MB 统计（:106-147）。

**可借鉴**：`IS_CHANGED` 返回 `time.time()` 强制每次执行的技巧（:44-46）；`set_flag("free_memory", True)` 请求调度器卸载比手动遍历更安全；psutil 打印释放量（可结合 profiling）；Windows RAM 清理 ctypes 三件套。

**依赖/风险**：psutil + ctypes；`EmptyWorkingSet` 遍历全系统进程属激进操作，需管理员权限——ComfyDL 中建议默认关闭仅作可选档位。

---

## 批次小结（对 ComfyDL 最有价值的横切主题）

1. **非侵入 patch 三个层级**（由稳到激进）：`set_model_unet_function_wrapper` 官方 hook（WaveSpeed）> clone + `transformer_options` 注入（DN_PatchFlashAttention/Attention-Optimizer）> mock.patch forward（TeaCache）。
2. **attention 后端自动选优**：探测+运行时验证+基准+版本化缓存（Attention-Optimizer）可直接作为 ComfyDL "profiling 驱动加速"模块蓝本。
3. **量化接入两种模式**：custom tensor 子类 + lazy dequant（GGUF）vs ops/Layout 注册（INT8-Fast、comfy-kitchen）。
4. **外部引擎 adapter**：nn.Module wrapper + ModelPatcher 壳（nunchaku）。
5. **显存清理统一例程**：`gc.collect → empty_cache → ipc_collect → synchronize` + `set_flag("free_memory")` + NVML 外部占用修正。
