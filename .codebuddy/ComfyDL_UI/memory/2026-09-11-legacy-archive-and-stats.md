# 2026-09-11 收尾迭代：d2l 软归档 + 工作流类型修复 + SpectralNorm 默认 + 归一化导出批统计量

> 本轮范围由用户两轮多选澄清后确定；Show Heatmaps 升级**留到下一轮**（决策已存档于 `2026-09-11-heatmap-next.md`）。
> 起点：三仓干净（主仓 `9257c2f`、子模块/独立 checkout `3155180`）。

## 1. d2l 重叠节点软归档（7 个）

按 `docs/reform-step1/2/3` 的重叠表，对以下节点做**软归档**（节点保留可用，旧工作流不破）：

| 节点 | 位置 | 新分类 | 核心替代 |
|---|---|---|---|
| `CdlActivation` | `comfydl/nodes/tensor_basic.py` | `d2l/_Legacy/Tensor Basic` | 14 个核心 Activation |
| `CdlReshape` | 同上 | 同上 | `BasicReshape` |
| `CdlBroadcast` | 同上 | 同上 | `BasicBroadcast` |
| `CdlAddNorm` | `comfydl/nodes/model_attention.py` | `d2l/_Legacy/NLP Models` | `NormalizationLayerNorm` + `BasicAdd` |
| `CdlTransformerEncoderBlock` | 同上 | 同上 | 核心层组合 |
| `CdlTransformerEncoder` | 同上 | 同上 | 核心层组合 |
| `CdlModelMode` | `comfydl/nodes/model_utils.py` | `d2l/_Legacy/Model Utils` | `TrainingMode`（TENSOR 图；载荷不同，`cdlModel` 级仍可用本节点） |

三件套机制（沿用上游既有约定，勿发明新标记）：
- 类属性 `DEPRECATED = True`（宿主 `server.py:783` 下发 `info['deprecated']`）；
- 显示名加 `(DEPRECATED)` 后缀（改 `NODE_DISPLAY_NAME_MAPPINGS`，上游 `comfy_extras/nodes_dataset.py:944` 同约定）；
- `CATEGORY` 移入 `d2l/_Legacy/<原分类>`；
- docstring 顶部补"已弃用 + 改用哪个核心节点"。

**关键不变量**：`NODE_CLASS_MAPPINGS` 的 key（节点 id）**不改** → 旧工作流仍可加载执行，节点数 **108 不变**；分类 17 → **20**。
`comfydl/nodes/__init__.py` 包级分类注释已重写（列活跃分类 + Legacy 段）。

复核（code-explorer 子代理 + 全仓搜索）：`CdlDense*` **不存在**；`CdlLinReg` 权重转置约定相反、`CdlConv2d`/`CdlTranspose` 无核心等价 → **均未归档**。

## 2. 示例工作流旧类型名修复

`cdlTensor` 早已是 `TENSOR` 的 legacy alias（`comfydl/nodes/__init__.py`），但 7 个 JSON 里仍以旧字符串存连线类型。

- 就地替换 `"cdlTensor"`→`"TENSOR"`（共 196 处）：All-Nodes Demo 126、Object Detection 23、Linear Regression 22、LeNet 9、Kaggle 6、`_archived/nlp_vocab_pipeline.json` 3、`_archived/bbox_conversion_pipeline.json` 7。
- 只替换类型字符串，**不动节点 id / 连线 id / widget 值**；逐个 `json.loads` 校验通过。
- 同步生成脚本 `example_workflows/_gen_full_test.py` + 迁移记录 `comfydl/_update_nodes.py` 的 `replacements`，避免重新生成写回旧名。
- `ComfyDL_AllNodes_Generated.json` 解析失败经核实为**未跟踪的遗留产物**（非本次引入），未改。
- 历史文档中叙述性的 `cdlTensor` 保留原样。

## 3. SpectralNorm 默认值 1 → 10

`comfy_extras/nodes_normalization.py`：`NormalizationSpectralNorm` 的 `n_power_iterations` 默认 1 → **10**（schema `io.Int.Input` + execute 签名 + docstring），区间不变 `0..20`。
依据：无状态节点无法像 `torch.nn.utils.spectral_norm` 那样热启动 `u`/`v`；实测 est/true 1 次迭代：`(6,5)`→0.996、`(64,64)`→0.706、`(256,128)`→0.783；20 次可达 1.000/0.998。用户拍板改 10。仅新建节点默认档变化。
`docs/reform-step3-normalization.md` §2.1 表 + 实测说明已同步。

## 4. 归一化导出本次批统计量（可直接喂 Training Run Stats）

`comfy_extras/nodes_normalization.py`：
- `_normalization_schema(...)` 增 `outputs: list | None = None`（默认 `[output]`，仅 BatchNorm/InstanceNorm 传 3 个）——**5 个节点共用**，默认参数保证其余 3 个零行为变化。
- `NormalizationBatchNorm.execute`：`dims=(0,)+tuple(range(2,tensor.dim()))`；train 分支**单次归约**算 `mean=mean(dims)`、`var=var(dims,unbiased=False)`（有偏，与 `F.batch_norm` 同口径）后以 `training=False` 调 `F.batch_norm`，**不写 running stats**；eval 分支回显所用（已广播的）running 统计量；rank<2 早退也返回标量统计量。输出 `(output, mean, var)`。
- `NormalizationInstanceNorm.execute`：输出仍走 `F.instance_norm`；额外导出逐通道口径 `mean=gm.mean(0)`、`var=gv.mean(0)+gm.var(0,unbiased=False)`（`gm`/`gv` 为 `(N,C)` 的空间归约）——**总方差分解**，与 BatchNorm/eval 槽对齐。
- `TrainingRunStats`：新增两个可选 `io.Tensor.Input("mean"/"var", optional=True)`；`_linked_stats(linked, text, name, fallback)` 实现"连线优先、否则解析字符串"；输出名 `running_mean`/`running_var` 与数量不变。
- 同步 3 处过时说明（模块头 37-40、325-328、385-388）。
- 性能：BatchNorm 由"框架一次 + 导出再算一次"压成一次归约；InstanceNorm 多一次同阶归约；均为追加式输出（索引不变）→ 旧工作流可加载。符合 AGENTS.md「backward compatible by default」。

## 5. 冒烟测试扩展

`cdl_smoke_tests/run_smoke_test.py`：
- 新增 dummy：`_f_tensor_4d`（`(2,3,4,4)`，让逐通道归约真正发生；否则 InstanceNorm 会早退）、`_f_stats_linked`（与控件默认 `0.0`/`1.0` 刻意不同，以便区分连线/控件路径）。
- `_DUMMY_OVERRIDES`：BatchNorm/InstanceNorm 的 `tensor` 用 4D；`TrainingRunStats` 只连 `mean`（一次覆盖"连线优先"与"文本解析"两条路径）。
- `_OUTPUT_CHECKS` + `_check_batchnorm_stats`/`_check_instancenorm_stats`/`_check_training_run_stats`：断言导出统计量与 `tensor.mean/var` 一致、输出确由导出统计量构建。

## 6. 文档同步

- 四份说明文件（`comfydl/FUNCTIONS.md`、`FUNCTIONS_zh.md`、`README.md`、`README_zh.md`）：7 个归档节点加"已弃用/替代"说明；BatchNorm/InstanceNorm 输出列、SpectralNorm 默认值、TrainingRunStats 输入列更新；附录分类表加 3 条 Legacy 行。
- `comfydl/_update_readme.py`：`_CAT_ZH`/`_CAT_EN`/`_CAT_DESC_EN`/`_CAT_DESC_ZH` 四表各加 3 条 `d2l/_Legacy/*`。
- 运行 `_update_readme.py`（**16 处**变更）后 `--check` 全绿；手改 README 总句 "140 nodes across **22→25** categories"（脚本正则不覆盖该句）。
- `docs/reform-step1-activation-tensor.md`（§5 归档 + 旧 JSON 修复后续）、`reform-step2-network-layers.md`（§5 归档）、`reform-step3-normalization.md`（§6 数字更新块 + §7 归档 + 新增"批统计量导出"小节 + §2.1 默认值表 + `n_power_iterations` 实测说明）、`docs/comfydl-builtin.md`（分类布局加 Legacy 更新块）。

## 7. 验证结果

- 冒烟：`penv\Scripts\python.exe cdl_smoke_tests\run_smoke_test.py` → **224 PASS / 22 SKIP / 0 FAIL（246 节点）**。
- 文档：`_update_readme.py --check` → 四份文件与注册表一致。
- 注册表：banner **108 节点 / 20 分类**；宿主 **246 节点 / 38 分类**（软归档 +3 分类）。
- `read_lints`：改动的 5 个 py 文件无新增诊断。

## 8. 计数口径（更新）

- ① ComfyDL banner = **108 节点 / 20 分类**
- ② 宿主注册表 = **246 节点 / 38 分类**
- ③ 四份说明文件"内置节点库" = **140 节点 / 25 分类**（= 108 ComfyDL + 32 核心；分类 = d2l 20 + `Network & Layers/*` 5）

## 9. 收尾

- 待提交：子模块（nodes/*、example_workflows/*、四份 md、`_update_readme.py`）→ 主仓 gitlink + `comfy_extras/nodes_normalization.py` + `cdl_smoke_tests/run_smoke_test.py` + `docs/*`。
- 独立 checkout `P:\Dev\ComfyUI_Refs\ComfyDL` 需 `git pull` 同步。
