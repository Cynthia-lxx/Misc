# SP-4 计划：量化加载最小版（GGUF + 格式描述符 + estimators 字节率硬捆绑）

> 2026-10-31 立项草案，待用户审阅。上游依据：bigplan ADR-D3（硬捆绑裁决）、
> smallplans SP-4 节、refs_survey/survey_A A6（ComfyUI-GGUF city96 行号级档案）。
> M-B 剩余批次：SP-4（本计划）+ P1c 显存管家（0.5-1 craft，未排）。

## 0. 目标与边界

**一句话**：让 ComfyDL 直接加载 GGUF 量化扩散模型跑推理，且 profiling 内存估算器
对量化模型给出正确 verdict（ADR-D3：两者同批交付，不许拆）。

**双形态铁律落点**：
- 生产门面：`CdlLoadGGUF`（低显存加载 GGUF UNet → MODEL）
- 教学门面：`CdlQuantizationInspector`（逐层字节率表 + 等效 bits 统计 STRING，
  Quantization Lab 的第一块砖；完整 Lab 模板留给 P7，本批不做模板）

**首期边界（明确不做）**：CLIP GGUF 变体、kitchen 量化域（INT8-Fast/Layout 路线）、
在线量化、GGUF 保存、多 LoRA on-quantized 兼容矩阵。

## 1. 前置事实（2026-10-31 已核对）

| 事实 | 状态 |
|---|---|
| `gguf` pip 包 | penv/penv_new **均未安装** → 进 requirements.txt，用户手动装（红线禁运行时自装）；节点侧惰性 import + 缺失抛可读提示（pyodbc 先例） |
| 参考仓库 | `Refs/ComfyUI-GGUF`（city96）在位，survey_A A6 有行号级档案 |
| estimators 扩展点 | `profiling/shapes.py` `TensorVal(shape, itemsize, dtype)`，`nbytes=numel*itemsize`；`QUANT_ALGOS` 不存在需新建 |
| 加载通道 | UNETLoader 走 rehydrated 真栈 `comfy.sd.load_diffusion_model`；A6 走 `load_diffusion_model_state_dict` + `model_options["custom_operations"]`——该通道在本仓 comfy.sd 的可用性动工首日核对（dehydrated 时可能被裁） |
| triton | GGUF dequant 纯 torch 位运算，与 triton 缺失无关 |
| 兼容注记 | torch<2.8 需禁 torch.compile（A6 ops.py:21-42）；本仓 torch 2.11/2.13 无虞，docstring 记录即可 |

## 2. 任务分解（CRAFT 序，预算 2-3 craft）

### T1 格式描述符接口 + GGUF 移植（核心，最大件）
新文件 `comfy/quant_formats.py`：
- `QuantFormat` 协议：`bytes_per_element(qtype) -> float`（bits/8，K-型非整数字节按
  block 折算）/ `dequantize(tensor) -> torch.Tensor` / `wrap_state_dict(sd) -> dict`
- GGUF 首个实现（A6 全套移植）：
  - `GGMLTensor`（torch.Tensor 子类：携带 `tensor_type`/`tensor_shape`/`patches`，
    重写 to/clone/shape；mmap 逐张量读 uint8 不展开）
  - `GGMLOps(manual_cast)`：覆写 Linear/Conv/Embedding/LayerNorm，
    `cast_bias_weight` 前即时 dequant
  - `patch_weight_to_device` 免反量化打 patch（A6 nodes.py:35-78 模式；评估能否收窄
    patch 面而不是 monkeypatch——红线只禁 recursive_execute，此项允许，但求最小面）
  - dequant 纯 torch 位运算（A6 dequant.py 移植，numpy 回退保留）
- `gguf` 包惰性 import：缺失时 `CdlLoadGGUF`/`Inspector` 抛
  "pip install gguf>=0.13.0" 可读错误；requirements.txt 增补

### T2 CdlLoadGGUF 节点（comfy_extras V3，CATEGORY model/perf）
- `folder_paths.add_folder_name_and_extensions("unet_gguf", ["unet_gguf"], {".gguf"})`
- inputs: `unet_name`(COMBO, unet_gguf 目录) + `dequant_dtype`(default "target")
  + `patch_dtype`(default "target")；输出 MODEL
- 走 `load_diffusion_model_state_dict` + `custom_operations`（动工首日核对；若通道被裁
  则按 A6 方式自组 ModelPatcher + model_detection）
- 与 SP-3 组合语义：GGMLTensor 经 compile 的行为未验证 → docstring 注记
  （建议 GGUF 分支不叠 CdlCompileModel，或 aot_eager）

### T3 estimators 字节率适配（ADR-D3 硬捆绑，同批交付）
- 新表 `QUANT_ALGOS`：qtype → 元素字节数。来源 `gguf.GGML_QUANT_SIZES`，但
  **estimators 不得 import gguf**（保持 stdlib+torch 层）→ 内置精简常量表
  （Q8_0/Q6_K/Q5_K*/Q4_K*/Q4_0/Q3_K*/IQ 系列按需，block size+type size 精确抄
  GGML 规范），加单元测试与 gguf 包对照（测试环境有 gguf 即校验全表）
- `TensorVal` 侧：量化张量 `itemsize = type_size/block_size`（浮点字节数，
  `nbytes` 相应取整语义记录清楚）；模型权重字节估算路径（formulas/params 侧）同步
- 验收精度：NVFP4 档 0.5B+1/16B 级（smallplans 测试要求）——即估算字节与真实
  文件字节差 <10%

### T4 CdlQuantizationInspector 节点（教学门面）
- inputs: `model_path`(COMBO, 扩到 checkpoints/unet/diffusion_models/unet_gguf 四目录
  或简化为 unet_gguf+unet 双源，动工定) ；输出 STRING
- 读 gguf 元数据（无 gguf 时 safetensors 头亦可）→ 逐层表：层名 / qtype / 原始 fp 字节 /
  量化字节 / 压缩比 / 等效 bits；尾部汇总（总字节、平均 bits、省显存 %）
- 教学文案：per-layer 表旁边附一行"为什么 Q4_K_M 等效 4.5 bits 而不是 4"（block scale 开销）

### T5 测试（随做随写，闸门在 T6）
- `test_quant_gguf.py`：
  1. GGMLTensor 语义（to/clone/shape 保型）
  2. dequant 正确性：Q8_0/Q4_K 小张量对已知值（fixture 用 gguf 包构建脚本生成）
  3. QUANT_ALGOS 表与 gguf.GGML_QUANT_SIZES 全等（测试环境有 gguf 时）
  4. estimators：合成 GGUF 模型 workflow 的 verdict 字节与手算一致（NVFP4 精度档）
  5. 节点注册 + 冒烟策略（见 T6）
- fixture：**构建脚本生成**（`cdl_smoke_tests/data/make_gguf_fixture.py`，
  跑测试时自动生成微型 .gguf <100KB 进临时目录）而非入库二进制——规避大文件红线，
  且不受 git LFS 困扰

### T6 冒烟/文档/双盘（收尾惯例）
- 冒烟 runner：`CdlLoadGGUF` SKIP（"needs a real GGUF file in unet_gguf - covered by
  test_quant_gguf.py"）；`CdlQuantizationInspector` 若能对 fixture 目录跑则内联，否则同 SKIP
- 五份文档惯例检查（extras V3 不入 FUNCTIONS 为既定边界，`--check` 须一致）
- CPU 全绿 → 提交（先子模块后父仓，**push 后 `git submodule status` 复核 gitlink**）
- F 盘同步 → GPU 冒烟 → 用户提供真实 GGUF 文件实测（加载 + 显存占用 vs fp16 对比 +
  estimators verdict 正确性）→ 用户界面验收

## 3. 风险与预案

| 风险 | 预案 |
|---|---|
| `custom_operations` 通道被 dehydrate 裁掉 | 按 A6 自组 patcher：GGUFReader → sd → `model_detection` + ops 注入（A6 nodes.py 整段就是蓝图） |
| estimators 权重字节路径找不到统一挂点 | 最小面：在模型权重聚合处按 `getattr(t, "tensor_type", None)` 分流，不动的 estimator 不碰 |
| Windows mmap 页文件尖峰（A6 ops.py:234-240） | 移植其 workaround（读段尺寸上限） |
| `gguf` 包 API 版本漂移 | requirements 钉 `gguf>=0.13.0,<0.14`（A6 同款），import 处版本探测 |
| 用户暂不装 gguf | 节点报可读错误；Inspector 的 safetensors 路径照常可用（教学门面半独立） |

## 4. 验收清单（预填，验收时逐项打钩）

- [ ] P 盘：requirements 更新说明给出；无 gguf 时两节点错误信息可读
- [ ] CPU 冒烟全绿（308+N/34+M/0）
- [ ] test_quant_gguf 全绿（dequant/字节率/estimators 三层）
- [ ] F 盘：真实 GGUF 模型（用户提供）加载推理成功
- [ ] GPU 上同模型 fp16 vs GGUF 显存占用对比数字合理（≈压缩比）
- [ ] profiling verdict 对 GGUF workflow 正确（红/黄/绿判定与真实占用一致）
- [ ] Inspector 逐层表出现且数字与文件大小一致
- [ ] Settings 面板/模板回归无破坏
- [ ] 文档同步、双盘 git 状态一致（含 gitlink）
