---
name: dropout-node
overview: 在 Network &amp; Layers 下新增 Regularization 家族与核心 Dropout 节点（comfy_extras/nodes_normalization.py），并同步 4 份说明文件、独立 checkout 与项目记忆
todos:
  - id: baseline-smoke
    content: 用 venv 跑全量冒烟测试，记录节点总数与 PASS/SKIP/FAIL 真实基线
    status: completed
  - id: impl-dropout
    content: 在 nodes_normalization.py 新增 RegularizationDropout 节点与 Regularization 分类并注册
    status: completed
    dependencies:
      - baseline-smoke
  - id: verify-numerics
    content: 用临时脚本核对 Dropout 的 train/eval、p 边界、seed 可复现与 dtype 保持，随后删除脚本
    status: completed
    dependencies:
      - impl-dropout
  - id: smoke-gate
    content: 重跑全量冒烟确认 0 FAIL、PASS 与总数各 +1，并核对分类树出现 Regularization
    status: completed
    dependencies:
      - impl-dropout
  - id: docs-counts
    content: Use [subagent:code-explorer] 定位 4 份说明文件全部计数点，补 17.5 节并改为 140 / 22 / 32
    status: completed
    dependencies:
      - smoke-gate
  - id: docs-mirror-git
    content: 先 git pull，再同步 ComfyDL 镜像的 4 份文档，提交推送子模块与主仓 gitlink
    status: completed
    dependencies:
      - docs-counts
  - id: memory-log
    content: 订正 MEMORY.md 计数（含旧值）并新建 2026-09-11-dropout-node.md 详录
    status: completed
    dependencies:
      - docs-mirror-git
---

## 产品概述

在 ComfyUI 节点库的 `Network &amp; Layers` 分支下新增 `Regularization` 子分类，并为其加入首个节点 `Dropout`，使正则化家族与既有的 Activation / Basic / Normalization / Training 并列。

## 核心功能

- **节点位置与命名**：分类为 `Network &amp; Layers/Regularization`，显示名 `Dropout`。
- **单输入单输出**：一个张量输入、一个张量输出，形状不变；任意维度（含 0 维标量）都可用，无需选择 1d/2d/3d。
- **训练态行为**：按丢弃概率随机把元素置零，并对保留下来的元素按 1/(1-p) 放大补偿，使输出的期望与输入一致。
- **推理态行为**：原样输出、不做任何丢弃，与 PyTorch 的推理行为一致。
- **丢弃概率控件**：数值控件，默认 0.5（PyTorch 默认），范围 0 到 1。取 0 时恒等；取 1 时输出全零。
- **随机种子控件**：整数种子 + ComfyUI 标准的「生成后控制」下拉（固定 / 递增 / 递减 / 随机化）。同一种子下结果逐元素完全可复现；把下拉设为随机化，即可让每次生成得到不同的丢弃掩码。
- **训练/推理开关复用**：可把已有 `Training Mode` 节点的输出连到本节点的模式插槽来切换；不连线时按训练态执行，节点可独立使用。
- **文档同步**：4 份说明文件（中英 FUNCTIONS、中英 README）补充本节点说明与分类表，并把总数更新为 140 节点 / 22 分类（其中核心 Network &amp; Layers 节点 32 个）。

## 技术栈选择

- **语言/运行时**：Python 3.14，使用项目 venv `ComfyDL_UI\penv\Scripts\python.exe`（冒烟与自检脚本优先用它）。
- **张量库**：PyTorch（`torch`、`torch.nn.functional`）。
- **节点 API**：沿用本仓库已有的 ComfyUI **V3** 节点 API（`comfy_api.latest` 的 `io.ComfyNode` + `io.Schema` + `ComfyExtension` / `comfy_entrypoint`），不用旧式 `INPUT_TYPES` / `RETURN_TYPES`。
- **插槽类型**：核心 `TENSOR`（`comfy/comfy_types/node_typing.py` 的 `IO.TENSOR` + `comfy_api/latest/_io.py` 的 `@comfytype("TENSOR")`），与 Network &amp; Layers 其它节点共用。
- **注册链**：`comfy_extras/` 核心节点链。`nodes.py` 的 `extras_files` 白名单已含 `"nodes_normalization.py"`，**无需修改 `nodes.py`**。
- **验证**：`cdl_smoke_tests/run_smoke_test.py`（进程内、不启动服务）。
- **规范依据**：`AGENTS.md` 的工程风格，`Comfy_CustomNode_Docs_md/custom-nodes/*` 的 V3 API 与缓存约定。

## 实现方案

### 总体策略

纯增量实现：在既有文件里新增一个分类常量 + 一个节点类 + 一处注册，其余既有节点零改动。这样既不触碰 `comfy/` 注册链、不新增插槽类型，也不改变任何现有 node_id / 分类，因此不会破坏已有工作流。

### 落点决策

Dropout 节点直接加在 **`comfy_extras/nodes_normalization.py`**，理由：

1. 该文件已有「一个文件承载两个分类」的先例（`Normalization` + `Training`），再加 `Regularization` 与既有结构一致；
2. Dropout 必须复用该模块的私有训练/推理态通道（`_mode_input()`、`_normalize_mode()`、`_warn()`），同文件复用零耦合；
3. 免去 `nodes.py` 白名单改动。

**备选方案（已否决）**：新建 `comfy_extras/nodes_regularization.py`。否决原因是它必须跨模块 import 私有 helper（把"开关走连线而非隐藏 prompt"这一核心结论的承载点对外暴露），或复制一份 `_mode_input` / `_normalize_mode`，且还要多改一处 `nodes.py` 白名单——为单个节点引入更大的爆炸半径，不符合 `AGENTS.md`「改动尽量少文件」的要求。

### 关键决策

| 决策点 | 选择 | 理由与取舍 |
| --- | --- | --- |
| 分类 | 新增 `REGULARIZATION_CATEGORY = "Network &amp; Layers/Regularization"`，不改 `NORMALIZATION_CATEGORY` | 语义准确；代价是文档分类数 21 → 22 |
| schema 构造 | 不复用 `_normalization_schema()`（它把分类写死为 Normalization），按 `NormalizationSpectralNorm` 的做法直接构造 `io.Schema` | 避免为一个节点给公共 helper 加 category 参数 |
| 随机数 | 局部 `torch.Generator(device=tensor.device)` + `manual_seed(int(seed))` | **不用** `torch.manual_seed()` + `F.dropout()`：`F.dropout` 没有 generator 参数、只能读进程全局 RNG，在长期运行的服务里会污染全局随机态影响其它节点；局部 generator 既确定又可复现 |
| 种子控件 | `io.Int.Input("seed", default=0, min=0, max=0xFFFFFFFFFFFFFFFF, control_after_generate=True)` | 与本仓库 `nodes_images.py` 的 `ImageAddNoise` 完全一致；官方文档 `v3_migration.md` 明确 `True` 等价于 `io.ControlAfterGenerate.randomize`，属 ComfyUI 最主流做法 |
| 缓存 | **不实现** `fingerprint_inputs` | seed 是普通 widget，已进入节点输入签名；官方文档 `server_overview.md` 对随机节点给出的建议就是「提供 seed 输入让用户控制可复现性」 |
| 边界 p | `p &lt;= 0` 恒等；`p &gt;= 1` 返回同 shape/dtype/device 的全零张量 | 规避 PyTorch 式 `1/(1-p)` 在 p=1 时的 inf/nan |
| mode 插槽 | 复用 `_mode_input()`，但把它参数化为可传 tooltip（默认文本保持原样） | 现有 tooltip 写的是"用本次调用的统计量归一化"，直接复用到 Dropout 上语义不符；给 helper 加一个可选参数即可做到零行为变更 |


### 数值与性能

- 掩码：`keep = torch.rand(tensor.shape, generator=gen, device=tensor.device, dtype=torch.float32) >= p`，输出 `torch.where(keep, tensor / (1 - p), 0)`。用 **布尔**掩码比较而非先造浮点掩码，掩码显存为 1 字节/元素（约为浮点掩码的 1/4）。
- 复杂度 O(numel)，与 PyTorch 自带 dropout 同量级：一次全尺寸随机数生成 + 一次 `where` + 一次输出分配。随机数的生成设备取 `tensor.device`，**不走 CPU**（本仓库 `ImageAddNoise` 用 `device="cpu"` 是为了兼容旧路径，Dropout 若照抄会引入一次主机同步与一次跨设备拷贝）。
- 快路径零开销：`eval` 模式、`p &lt;= 0`、`p &gt;= 1` 三种情况**完全不进入随机数分支**，直接返回输入或全零张量——推理图上放一个 Dropout 不产生任何额外计算。
- 期望值正确性：保留元素除以 `(1-p)`，故 `E[output] == E[input]`，与 `F.dropout` 的训练语义一致。

### 可扩展性

新增 `REGULARIZATION_CATEGORY` 常量 + 独立节点类，后续正则化节点（如按通道丢弃、权重衰减类）可在同一分类下追加，无需再动本次引入的任何代码。

## 实现注意事项

- **复用而非重写**：`_warn()`（统一 `[Network &amp; Layers]` 前缀）、`_normalize_mode()`（无法解析时回落 `"train"` 并告警）、`_mode_input()`。对 `_mode_input()` 只做「加可选 tooltip 参数」这一处向后兼容的扩展，BatchNorm / InstanceNorm 的现有 tooltip 文本逐字不变。
- **无状态**：绝不原地写入输入张量（同一张量可能被图中多个节点共享）；保持输入 dtype / device。
- **不改既有行为**：现有 9 个节点的 schema、id、分类、执行体全部不动；只追加常量、新类、`NORMALIZATION_NODES` 列表一项、模块 docstring 说明。
- **docstring 必须完整**：按本文件既有 What / In / Out 三段式，英文书写，写明单输入单输出、任意秩、train/eval 语义、`seed` 的可复现性与前端「生成后控制」下拉的作用、p 的边界行为。
- **控件默认值开箱可用**：`p=0.5` 在 `[0,1]` 内且为 PyTorch 典型值；`seed=0` 在 `[0, 0xFFFFFFFFFFFFFFFF]` 内。
- **冒烟测试无需登记**：`run_smoke_test.py` 对 optional 且类型属于 `_WIDGET_TYPES` 的输入会按声明默认值造值，因此 `mode` 会拿到 `"train"`、`seed` 会拿到 `0`，节点会走训练分支正常执行；无需加入 `_SKIPPED_NODES` 或 `_INPUT_OVERRIDES`。
- **三套计数口径不要互相纠正**：① ComfyDL 启动 banner（108 节点 / 17 分类，本次不变）；② 宿主注册表（当前 245 节点）；③ 4 份说明文件「内置节点库」口径（当前 139 节点 / 21 分类）。本次只影响 ②（+1）与 ③（140 / 22）。
- **临时文件必须清理**：数值自检脚本跑完即删，不留在仓库里。
- **git 细节**：仓库根 `P:\Dev\ComfyUI_Refs\ComfyDL_UI\`；命令需带 `-c safe.directory=...`；cmd 下 `&amp;&amp;` 会被转成 `;`，git 命令分条写；推送前先 `git pull origin master --rebase`；禁止强推 master。

## 架构设计

不引入任何新的架构模式，完全沿用 reform 第一至三步确立的做法：单个 `comfy_extras` 模块承载一个（或多个）`Network &amp; Layers` 子分类，通过 `ComfyExtension.get_node_list()` 注册，节点之间只靠 `TENSOR` 插槽与 STRING 连线交互。

- **数据流**：`TENSOR` → Dropout → `TENSOR`（形状/dtype/device 不变）。
- **控制流**：`TrainingMode.mode`（STRING 输出）→ Dropout 的 `mode` 插槽（可选，`force_input`）。连线存在时该开关进入消费者的缓存签名，翻转下拉会真正使下游重算；不连线时按 `train` 处理。
- **种子流**：`seed` 是节点自身 widget，其值属于节点输入签名的一部分——用户改种子或前端「随机化」刷新该值时会触发重新执行，从而得到新掩码；种子不变则输出逐元素可复现。
- **分类树变化**：`Comfy节点 → Network &amp; Layers` 下新增 `Regularization`(1)，与 `Activation`(14) / `Basic`(8) / `Normalization`(7) / `Training`(2) 并列，核心节点合计 32 个。

## 目录结构

本次为在既有项目上做增量改动，仅列出被修改/新增的文件。

```
ComfyDL_UI/
├── comfy_extras/
│   └── nodes_normalization.py                # [MODIFY] 本次唯一代码改动点。新增 REGULARIZATION_CATEGORY 常量；新增 RegularizationDropout 节点类（node_id="RegularizationDropout"，display_name="Dropout"，分类 Network & Layers/Regularization，输入 tensor/p/seed/mode，输出单个 output）；把 _mode_input() 扩展为可传 tooltip（默认文本不变）；把 RegularizationDropout 追加进 NORMALIZATION_NODES 并在 NormalizationExtension 处补注释；模块 docstring 增补 Regularization 家族说明。要求：无状态、不原地写、保持 dtype/device、快路径（eval / p<=0 / p>=1）不产生随机数开销、英文完整 docstring。
├── comfydl/
│   ├── FUNCTIONS.md                           # [MODIFY] 17 节标题 31→32 nodes；在 17.4 Training 之后新增 "### 17.5 Regularization (1 node)" 小节（含节点表：RegularizationDropout / Dropout / 输入 p、seed、mode / 输出 output）；概览段 "139 nodes across 21 categories" 改为 "140 nodes across 22 categories"，核心明细补 "+ 1 core Network & Layers/Regularization"；分类表新增 Regularization 行；脚注的 "pure ComfyUI core categories" 清单加入 Regularization。
│   ├── FUNCTIONS_zh.md                        # [MODIFY] 与 FUNCTIONS.md 同构的中文改动（"### 17.5 Regularization（1 个节点）"、140 节点 / 22 分类、分类表与脚注）。
│   ├── README.md                              # [MODIFY] "plus 31 ComfyUI core nodes (… + Normalization 7 …)" 改为 32 并补 Regularization；分类表新增一行。
│   └── README_zh.md                           # [MODIFY] "另加在其之上新增的 31 个 ComfyUI 核心节点（…）" 改为 32 并补 Regularization；分类表新增一行。
└──（仓库根：P:\Dev\ComfyUI_Refs\）
    ├── ComfyDL/                               # [MODIFY] 独立 checkout 镜像：同一套 4 份说明文件（FUNCTIONS.md / FUNCTIONS_zh.md / README.md / README_zh.md）。先 git pull 同步，再施加与 comfydl/ 完全一致的改动，提交并推送 main。
    └── .codebuddy/memory/
        ├── MEMORY.md                          # [MODIFY] 订正三套计数口径与冒烟基线（② 245→246，③ 139/21→140/22，核心 31→32；并一并订正 conversion 那一轮遗留的旧值：25/30/37/38/39 行仍写着 102 节点 / 16 分类、239 节点、133/20、217 PASS）。
        └── 2026-09-11-dropout-node.md         # [NEW] 本轮详录：需求与澄清结论、设计决策（分类归属、落点、随机数策略、p 边界）、实测计数、冒烟结果、数值自检记录、提交号。
```

## 关键代码结构

仅给出接口级契约（节点 schema 与执行签名），实现体按既有文件风格补全。

```python
REGULARIZATION_CATEGORY = "Network & Layers/Regularization"

class RegularizationDropout(io.ComfyNode):
    """…英文 What / In / Out 三段式 docstring…"""

    @classmethod
    def define_schema(cls) -> io.Schema:
        return io.Schema(
            node_id="RegularizationDropout",
            display_name="Dropout",
            category=REGULARIZATION_CATEGORY,
            description="…",
            search_aliases=["dropout", "regularization", "drop", "probability", "dropout rate"],
            inputs=[
                io.Tensor.Input("tensor", tooltip="…"),
                io.Float.Input("p", default=0.5, min=0.0, max=1.0, step=0.01, tooltip="…"),
                io.Int.Input("seed", default=0, min=0, max=0xFFFFFFFFFFFFFFFF,
                             control_after_generate=True, tooltip="…"),
                _mode_input(tooltip="…Dropout 专用措辞…"),
            ],
            outputs=[io.Tensor.Output(display_name="output")],
        )

    @classmethod
    def execute(cls, tensor: torch.Tensor, p: float = 0.5, seed: int = 0,
                mode: str = MODE_TRAIN) -> io.NodeOutput: ...
```

`NORMALIZATION_NODES` 列表末尾追加 `RegularizationDropout`，并同步该列表与 `NormalizationExtension` 的注释措辞（现注释只提"Normalization 家族与训练态 helper"）。

## 验证方案

1. **改代码前先跑一次全量冒烟**，取得真实基线（不要照抄文档里的历史数字）。
2. **改后重跑全量冒烟**：要求 `0 FAIL`，且 PASS 与节点总数各比基线 +1。
3. **数值自检（临时一次性脚本，跑完删除）**：eval 模式恒等；`p=0` 恒等；`p=1` 全零；同 seed 两次逐元素完全一致；不同 seed 结果不同；train 模式实际丢弃比例接近 `p` 且已按 `1/(1-p)` 缩放（统计期望约等于输入期望）；float16 / float64 与 device 保持；非法 `mode` 回落 train 并打印 `[Network &amp; Layers]` 告警。
4. **分类树核对**：注册树中出现 `Network &amp; Layers/Regularization`(1)，且 `python_module == comfy_extras.nodes_normalization`。
5. **文档一致性**：4 份说明文件（含镜像）四处计数一致为 32 / 140 / 22。

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose: 在 `ComfyDL_UI/comfydl/` 与仓库根 `ComfyDL/` 两处，全量定位 4 份说明文件中所有与节点数、分类数相关的行（含 `## 17.` 标题、17.x 小节、概览段、分类表、脚注、README 概览行），避免只改到部分计数点。
- Expected outcome: 给出精确的 `文件:行号` 清单，作为文档改动的执行依据；改完后可据同一清单复核再无残留旧值（139 / 21 / 31 等）。