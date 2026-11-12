---
name: reform-step10-rnn-decoder-upsample
overview: reform step10：新增 7 个核心层节点——RNN/LSTM/GRU 三件套（新建循环层文件）、TransformerDecoderBlock（注意力族）、Upsample/PixelShuffle/PixelUnshuffle（上采样族）；d2l 旧 RNN/GRU 节点软归档到 _Legacy；不动 LM 流水线。全程对齐 step7 注意力家族先例（TENSOR 流、权重走 slot、无状态），交付含冒烟测试零 FAIL 与四份说明文件同步。
todos:
  - id: recurrent-nodes
    content: 新建 comfy_extras/nodes_recurrent.py（RNN/LSTM/GRU，显式 cell 数学）并加入 nodes.py 白名单
    status: completed
  - id: decoder-block
    content: nodes_attention.py 新增 TransformerDecoderBlock（自注意力+交叉注意力+FFN，权重走 slot）
    status: completed
  - id: upsampling-family
    content: nodes_convolution.py 新增 Upsample / PixelShuffle / PixelUnshuffle 三节点
    status: completed
  - id: archive-d2l-rnn
    content: 软归档 model_nlp.py 三个 RNN/GRU builder 至 d2l/_Legacy 并重新生成 locales
    status: completed
    dependencies:
      - recurrent-nodes
  - id: smoke-assertions
    content: 冒烟器新增 7 节点覆盖与 torch 数值对齐断言，全量回归零 FAIL
    status: completed
    dependencies:
      - recurrent-nodes
      - decoder-block
      - upsampling-family
      - archive-d2l-rnn
  - id: docs-sync
    content: 同步四份说明文件、_update_readme.py、docs/reform-step10 设计文档与 comfydl-builtin.md step10 段
    status: completed
    dependencies:
      - smoke-assertions
  - id: commit-push
    content: 子模块与主仓按序提交（feat(extras)+docs(step10)），rebase 后推送 master/main
    status: completed
    dependencies:
      - docs-sync
---

## Product Overview

为 ComfyDL_UI 核心节点库（reform step10）新增三个批次共 7 个节点：循环网络族、Transformer 解码器块、上采样族，全部遵循项目既有"无状态、权重/数据走 slot、TENSOR 流"的核心层风格。

## Core Features

- **RNN 三件套**（新分类 `Network & Layers/Recurrent`）：RNN / LSTM / GRU 三个节点——显式手写 cell/门数学（tanh、LSTM 三门、GRU 两门），权重（weight_ih / weight_hh / bias）与隐状态（h0，LSTM 另有 c0）全部走 optional slot，输出 y + hn（LSTM 另有 cn）；固定 batch_first、单层（多层可由节点堆叠组合），输出 detach 保证缓存安全
- **Transformer Decoder Layer**（并入 `Network & Layers/Attention`）：TransformerDecoderBlock = 自注意力（可选因果 mask）→ Add → LN → 交叉注意力（k/v 来自 context）→ Add → LN → FFN → Add → LN，post-LN 与 TransformerEncoderBlock 一致，权重全走 slot（自注意力 4 组 + 交叉注意力 4 组 + FFN 2 组 + LN 仿射可选），复用 `_multi_head_attention` helper 与 SDPA 布尔掩码语义（True=可注意）
- **上采样族**（并入 `Network & Layers/Convolution`）：Upsample（F.interpolate，mode=nearest/linear/bilinear/bicubic/trilinear，scale_factor 默认 2，align_corners 控件，dims 1/2/3 分派）+ PixelShuffle（r 默认 2）+ PixelUnshuffle（逆操作，r 默认 2），纯重排无参数
- **软归档**：comfydl d2l 层旧 CdlRNNScratch / CdlRNN / CdlGRU 三个 builder 移入 `d2l/_Legacy/NLP Models`（DEPRECATED=True + 显示名后缀 + id 不变）；RNNLM 包装族保留不动
- 数值正确性以冒烟断言对照 torch 原生 nn.RNN/nn.LSTM/nn.GRU、nn.TransformerDecoderLayer 结构语义与 F.interpolate/F.pixel_shuffle 逐步对齐

## Tech Stack

- 沿用项目现有栈：Python 3.14 (Embedded) + PyTorch + ComfyUI V3 节点契约（`comfy_api.latest` io.Schema，显式传 category）
- 验证：venv `ComfyDL_UI\penv\Scripts\python.exe` + `cdl_smoke_tests\run_smoke_test.py`（现基线 283 PASS / 23 SKIP / 0 FAIL，306 节点 / 46 分类）

## Implementation Approach

**三处新增 + 一处归档，全部复用既有先例，不引入新模式：**

1. **RNN 族显式实现而非包装 nn 模块**（对齐 step7"显式 softmax 不走 fused SDPA"的决策）：逐门手写 tanh/sigmoid 数学，教学价值高、逐位确定、无隐藏状态；冒烟对照 `nn.RNN/LSTM/GRU`（bias 拆 ih/hh 两项求和，与 nn 一致）数值对齐。单层设计：多层 = 上一节点 y 接下一节点 x，天然可组合，控件从简（无 dropout / 无双向 / batch_first 固定 True）。权重 slot 形状按 (4H·I / 3H·I / H·I) 由 x 与 hidden_size 推导校验。
2. **DecoderBlock 复用 nodes_attention.py 既有积木**：`_multi_head_attention()` helper、`AttentionCross` 的 context 语义、布尔掩码转 additive（finfo.min 防全屏蔽 NaN）、TrainingMode 连线、本地 Generator 种子——与 TransformerEncoderBlock 同构，只是多一级交叉注意力与 LN。
3. **上采样族并入 nodes_convolution.py**：与 Conv/ConvTranspose 同域（decoder/generator 网络积木），dims 分派照既有 `_conv_schema` helper 模式；Upsample 运行时校验 mode 与 dims 匹配（1D: nearest/linear，2D: nearest/bilinear/bicubic，3D: nearest/trilinear），PixelShuffle/Unshuffle 走 F 原生纯重排。
4. **软归档照 model_attention.py 铁律**：DEPRECATED=True + 显示名 "(DEPRECATED)" + 分类 `d2l/_Legacy/NLP Models` + **id 绝不改**；仅归档 3 个 builder，RNNLM 包装族（CdlRNNLMScratch/CdlRNNLM/CdlRNNLMScratchPredict）保留——它们不与核心层重复且仍可用（也可接归档后的旧 builder，连线索不断）。

## Implementation Notes

- **白名单注册**：新建 `nodes_recurrent.py` 必须加入 `nodes.py:679` 的 `extras_files`（漏加即不注册）；建文件前先全库查重名（踩坑 18）
- **冒烟器约束**：optional 且非 widget 的 TENSOR 输入（h0/c0/weights/mask）execute 形参必须给默认值 None（踩坑 9）；断言对照 torch 时 `_rerun_v3` 未显式传入的张量会重新随机，一致性断言须全量传入
- **随机性**：DecoderBlock dropout 用本地 `torch.Generator` + seed 控件（踩坑 5）；RNN 族单层无 dropout，无需 seed
- **dtype**：浮点输入/权重提升到公共 dtype，对齐 BasicLinear/注意力先例；隐状态输出 `detach()`
- **计数口径**：注册表 306→313 / 46→47（新分类 Recurrent）；说明文件 200/34→207/35（core +7、新分类 +1，以实测为准）；README "ships N nodes across M categories" 总句手改（踩坑 11）
- **locales**：软归档改 display_name 后需重跑 `gen_locales.py`（来源 FUNCTIONS_zh.md 标题，正则已在 c049167 修复）并 `--check` 校验
- **提交策略**：本批次直接在 master 开发；每完成可独立验证的功能点提交一次（feat(extras) / docs(step10) 等），推送前 `git pull origin master --rebase`；子模块 comfydl 的改动（FUNCTIONS/README/软归档）单独提交 main 后回主仓更新 gitlink

## Architecture Design

数据流（全部 TENSOR 流、无状态、可缓存）：

```
权重 slot ──┐
h0/c0 slot ─┼→ [RecurrentRNN/LSTM/GRU] → y + hn (+cn)
x ──────────┘

x ──────────┐
context ────┤→ [TransformerDecoderBlock] → out
自 attn 权重×4 + mask(可选) ─┤   (自注意力 → Add → LN →
交叉 attn 权重×4 ────────────┤    交叉注意力 → Add → LN →
FFN 权重×2 + LN 仿射(可选) ──┘    FFN → Add → LN)

x ──→ [Upsample / PixelShuffle / PixelUnshuffle] → x'
```

## Directory Structure

```
ComfyDL_UI/
├── nodes.py                                  # [MODIFY] extras_files 白名单 +nodes_recurrent.py
├── comfy_extras/
│   ├── nodes_recurrent.py                    # [NEW] RNN/LSTM/GRU 三节点（新分类 Network & Layers/Recurrent）；显式 cell 数学、权重与隐状态走 optional slot、batch_first 固定、单层可堆叠；详细 What/In/Out docstring
│   ├── nodes_attention.py                    # [MODIFY] +TransformerDecoderBlock（自注意力+交叉注意力+FFN，post-LN，权重走 slot，复用 _multi_head_attention、SDPA 布尔掩码、TrainingMode、本地种子 dropout）
│   └── nodes_convolution.py                  # [MODIFY] +Upsample（F.interpolate，mode/scale_factor/align_corners/dims）/+PixelShuffle/+PixelUnshuffle；照 _conv_schema 共享 helper 模式
├── comfydl/  (submodule)
│   ├── nodes/model_nlp.py                    # [MODIFY] CdlRNNScratch/CdlRNN/CdlGRU 软归档：DEPRECATED=True + 显示名后缀 + 分类移 d2l/_Legacy/NLP Models，id 不变；RNNLM 族保留
│   ├── FUNCTIONS.md / FUNCTIONS_zh.md        # [MODIFY] 新增 7 节点条目 + 软归档标注 + 计数 207/35
│   ├── README.md / README_zh.md              # [MODIFY] 类别表与总句同步
│   └── _update_readme.py                     # [MODIFY] _CAT_DESC 增 Recurrent 分类描述（--check 校验）
├── cdl_smoke_tests/
│   └── run_smoke_test.py                     # [MODIFY] 7 节点 _INPUT_OVERRIDES 成套工厂；_OUTPUT_CHECKS：RNN 族逐步对照 nn.RNN/LSTM/GRU（ih/hh bias 求和）、DecoderBlock 对照手工组装/nn.TransformerDecoderLayer 语义、上采样族对照 F 原生
├── locales/zh/nodeDefs.json                  # [MODIFY] 经 gen_locales.py 重新生成（软归档 display_name 变化）
└── docs/
    ├── reform-step10-recurrent-decoder-upsampling.md  # [NEW] 设计文档（动机/清单/决策/默认值表/验证结论）
    └── comfydl-builtin.md                    # [MODIFY] 新增 step10 实测段
```

## Key Code Structures

RNN 族共享 schema 要点（以 LSTM 为例，三节点同构）：输入 `x`（TENSOR，(B, T, I)）+ optional slots `weight_ih (4H, I)`、`weight_hh (4H, H)`、`bias_ih (4H,)`、`bias_hh (4H,)`、`h0 (1, B, H)`、`c0 (1, B, H)`；控件仅 `hidden_size` INT（默认 16）；输出 `y (B, T, H)`、`hn (1, B, H)`、`cn (1, B, H)`，均 detach。RNN/GRU 无 c0/cn，GRU 权重行数 3H、RNN 为 H。h0/c0 未连时置零——"开箱可用"：只连 x 即可运行。