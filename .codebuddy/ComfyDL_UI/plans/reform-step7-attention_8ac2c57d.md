---
name: reform-step7-attention
overview: 在 `comfy_extras/` 新增 `Network & Layers/Attention` 分类（4 个无状态核心节点：AttentionMultihead / AttentionSelf / AttentionCross / TransformerEncoderBlock），权重全部走 slot、训练态走 TrainingMode 连线、随机性走本地 seed；同步弃用旧 `CdlMultiHeadAttention`、更新 4 份说明文件与语言包计数、扩充冒烟测试，最后按既有链路提交推送交付。
todos:
  - id: create-attention-nodes
    content: 创建 comfy_extras/nodes_attention.py（4 节点 + 共享 _multi_head_attention helper + 详尽 docstring）并加入 nodes.py extras_files 白名单
    status: completed
  - id: deprecate-legacy-mha
    content: 在 comfydl 子模块按弃用三件套处理 CdlMultiHeadAttention（DEPRECATED + 显示名 + 移 d2l/_Legacy，id 不变）
    status: completed
  - id: smoke-and-assertions
    content: 扩展冒烟测试：4 节点成套 dummy 工厂 + 对照 torch 参考实现的数值断言（mask/bias/eval 分支），跑到 0 FAIL
    status: completed
    dependencies:
      - create-attention-nodes
      - deprecate-legacy-mha
  - id: docs-and-locales
    content: 同步 4 份说明文件与新计数（181/33、N&L 49、core 72）、新增 docs/reform-step7-attention.md、重生成 locales 并跑 gen_locales --check
    status: completed
    dependencies:
      - deprecate-legacy-mha
  - id: verify-and-report
    content: 跑全套底层验证（冒烟 287 节点 0 FAIL + 计数一致性核查），产出用户自验收清单；不 commit/push（待用户指令）
    status: completed
    dependencies:
      - smoke-and-assertions
      - docs-and-locales
  - id: b25b5e04
    content: 6. 提交到实验性分支，并推送到GitHub
    status: completed
---

## 用户需求

按项目 reform 主线补齐核心注意力（Attention）节点族，全部落在 bare-`TENSOR` 无状态体系内（与 `BasicLinear` 的“权重走 slot、节点内不存参数”约定一致）：

### 核心节点

- **AttentionMultihead**：吃 `q / k / v` 三个 TENSOR + 可选 `mask`；四组投影权重（q_proj/k_proj/v_proj/out_proj 的 weight 必连、bias 可选）全部走 slot；控件 `num_heads / dropout_p / mode（复用 TrainingMode 连线）/ seed`。

### 便利封装

- **AttentionSelf**：吃 `x` + 一组权重，节点内部做 q/k/v 投影（最常用 self-attention 形态）。
- **AttentionCross**：吃 `x + context`，内部投影（encoder-decoder / 多模态常用）。

### 组装节点

- **TransformerEncoderBlock**（LN → MHA → Add → LN → FFN → Add，post-LN）：把搭一个 Transformer block 的成本从约 7 个节点降到 1 个。

### 分类

新建 `Network & Layers/Attention`，与 Activation、Basic 并列。

### 补充约定（工作区规则）

- 详尽 docstring（What/In/Out 三段式）；widgets 默认值开箱可用；seed 用局部 `torch.Generator` + `control_after_generate=True`；train/eval 走 STRING 连线。
- 改完必须：冒烟测试零 FAIL + 同步 4 份说明文件（含计数）。
- 视觉验收由用户本人做，我只做底层测试；未经明确要求不 commit/push。

## 技术栈

- 纯 PyTorch 函数式实现（`F.linear` + `F.scaled_dot_product_attention`/手工 softmax + `F.layer_norm`），无新依赖，运行于现有 Embedded Python 3.14 venv（`ComfyDL_UI/penv`）。
- 节点定义沿用 V3 `comfy_api.latest`（`io.ComfyNode` + `define_schema` + `ComfyExtension` + `comfy_entrypoint`），样板照抄 `comfy_extras/nodes_layers.py`。

## 实现方案

### 新文件：`ComfyDL_UI/comfy_extras/nodes_attention.py`

- 模块级 `ATTENTION_CATEGORY = "Network & Layers/Attention"`；仿 `_basic_schema` 写 `_attention_schema()` helper（统一单 TENSOR 输出 `output`）。
- **共享核心** `_multi_head_attention(...)`：`F.linear` 投影 → 按 `num_heads` reshape/transpose 分头 → scaled dot-product（`mask` 转 additive 后参与 softmax；bool mask True=可注意，float additive mask 原样相加；形状需可广播到 `(N, heads, L, S)`）→ 仅 `mode=="train"` 时用本地 `torch.Generator(device=...)` + `seed` 对注意力权重做 dropout → out 投影。`E`（embed dim）由权重形状推出，不做 channel 控件（沿用 step4 卷积决策）。
- **四个节点**：`AttentionMultihead`（q/k/v 必连 + 四组权重）、`AttentionSelf`（x + 同一组 q/k/v/out 权重，q=k=v=x）、`AttentionCross`（q 从 x，k=v 从 context）、`TransformerEncoderBlock`。前三个只是“q/k/v 从哪来”的糖，共用 helper。
- **TransformerEncoderBlock**（post-LN）：必连 slot = attn 四组 weight + `ffn1_weight` + `ffn2_weight`；可选 slot = attn 四个 bias、ffn 两个 bias、`ln1_weight/ln1_bias/ln2_weight/ln2_bias`（**不连 = 无 affine 的 `F.layer_norm`**，最大化开箱可用）+ `mask`；控件 `num_heads / dropout_p / ffn_activation(combo: relu|gelu，默认 relu) / mode / seed`。FFN = `BasicLinear→激活→BasicLinear` 的内联实现。
- `num_heads % E != 0` 时报清晰错误（沿用旧 `CdlMultiHeadAttention` 行为）；输入输出 dtype 提升策略与 `BasicLinear` 一致。
- `mode` 输入复用 `nodes_normalization.py` 的 `_mode_input()` 同款形态（`io.String.Input(default="train", optional=True, force_input=True)`），必要时直接 import 该模块的常量避免漂移。

### 注册：`ComfyDL_UI/nodes.py`

`extras_files` 白名单（:693-705）按字母序插入 `"nodes_attention.py"`（`nodes_activation.py` 之后）。**漏加即完全不注册**。

### 旧节点弃用：`comfydl/nodes/model_attention.py`（子模块）

`CdlMultiHeadAttention` 按既有“弃用三件套”处理：`DEPRECATED=True` + 显示名加 `(DEPRECATED)` + `CATEGORY` 移到 `d2l/_Legacy/NLP Models`（**id 不改**），docstring 补 Replacement 说明（指向新 Attention 族）。`CdlDotProductAttention`/`CdlAdditiveAttention` 数学独立、在 cdlModel 管线内无替代，保留不动。

### 冒烟测试：`cdl_smoke_tests/run_smoke_test.py`

- `_DUMMY_OVERRIDES` 为 4 个新节点提供**成套形状匹配**工厂（如 `q/k/v = (2, 5, 16)`、投影权重 `(16, 16)`、block 的 ffn 权重 `(64, 16)`/`(16, 64)`；通用 `(2,3)` dummy 会因维度不匹配 FAIL）。
- `_OUTPUT_CHECKS` 加数值断言，用 `_rerun_v3` 对照 torch 参考实现：
- `AttentionMultihead` ≡ `nn.MultiheadAttention`（batch_first、手写权重灌入、`need_weights=False`）；
- Self/Cross ≡ Multihead 的退化情形；
- mask（bool 与 additive）与 bias 分支各验一次；
- `mode="eval"` 时 dropout 恒等（同 seed 两次结果 bit 级一致）；
- `TransformerEncoderBlock` ≡ 手工组合 LN→MHA→Add→LN→FFN→Add 的输出。
- 新基线预期：**283 → 287 节点（260→264 PASS），0 FAIL**。

### 文档与语言包同步（4 份说明文件 + 1 份新文档）

- `comfydl/FUNCTIONS.md` / `FUNCTIONS_zh.md`：§17 顶层树与节点表插入 `Network & Layers/Attention` 小节（4 节点行，格式照 Basic 表）；`CdlMultiHeadAttention` 小节补 Replacement 行。
- `comfydl/README.md` / `README_zh.md`：类别表 + 手改总句（177→**181** nodes / 32→**33** categories = 109 ComfyDL + 68→**72** core；N&L 45→**49**）。`_update_readme.py` 只管 ComfyDL 口径（109/20 不变），总句手改。
- 新增 `ComfyDL_UI/docs/reform-step7-attention.md`（双语记录：设计、mask 语义、弃用、验证结果）。
- `penv\Scripts\python.exe cdl_smoke_tests\gen_locales.py --check`：`CdlMultiHeadAttention` 显示名变更需重生成 `locales/zh/nodeDefs.json`（仍 109 条，核心节点不进 nodeDefs）。

### 性能与可靠性

- 单节点一次前向，纯函数无状态；分头后可用 `F.scaled_dot_product_attention` 时优先用（含 fused kernel 路径），不满足其 mask 形态时回退手工 `softmax(QK^T/√d + mask)`，复杂度同标准 MHA。
- mask 转 additive 时用 `torch.finfo(dtype).min` 而非 `-inf`，避免 NaN 传播。
- 所有错误信息含 shape 期望值；不吞形状错误（wiring 错误必须显式报）。

### 关键代码结构

```python
# comfy_extras/nodes_attention.py —— 共享核心（三个注意力节点复用）
def _multi_head_attention(
    q: torch.Tensor, k: torch.Tensor, v: torch.Tensor,          # (N, L, E) / (N, S, E)
    q_weight, k_weight, v_weight, out_weight: torch.Tensor,       # (E, E) each, wired in
    q_bias=None, k_bias=None, v_bias=None, out_bias=None,          # optional (E,)
    mask: torch.Tensor | None = None,                              # bool or additive, broadcastable
    num_heads: int = 4, dropout_p: float = 0.0,
    training: bool = False, seed: int = 0,
) -> torch.Tensor: ...                                             # (N, L, E)
```

### 目录结构

```
ComfyDL_UI/
├── comfy_extras/
│   └── nodes_attention.py                 # [NEW] Attention 4 节点 + _multi_head_attention helper
├── nodes.py                               # [MODIFY] extras_files 白名单加 "nodes_attention.py"
├── cdl_smoke_tests/
│   └── run_smoke_test.py                  # [MODIFY] _DUMMY_OVERRIDES 成套工厂 + _OUTPUT_CHECKS 数值断言
├── docs/
│   └── reform-step7-attention.md          # [NEW] 本次设计/弃用/验证的双语记录
└── comfydl/                               # 子模块（名称保持 ComfyDL）
    ├── nodes/model_attention.py           # [MODIFY] CdlMultiHeadAttention 弃用三件套 + docstring
    ├── FUNCTIONS.md / FUNCTIONS_zh.md     # [MODIFY] Attention 小节 + 计数（181/33、N&L 49、core 72）
    └── README.md / README_zh.md           # [MODIFY] 类别表 + 手改总句 + 弃用说明
（生成物：locales/zh/nodeDefs.json 由 gen_locales.py 重生成后随主仓提交）
```

### 执行注意事项

- 冒烟器对 `optional=True` 且非 widget 类型（TENSOR）的输入跳过合成 → 所有可选 TENSOR 形参必须给默认值 `None`。
- 改后必须刷新浏览器页面才见新节点（节点库只取一次 `/object_info`）——此点写入交付给用户的自验收清单。
- 未经用户明确要求不 commit/push；本次交付终点 = 底层测试全绿 + 自验收清单。