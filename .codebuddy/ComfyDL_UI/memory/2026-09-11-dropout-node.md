# 2026-09-11 开发日志（追加）— Network & Layers / Regularization 核心节点 Dropout（reform 第四步）

> 承接 `2026-09-11.md`（reform 第一~三步）与同日 `2026-09-11-conversion-nodes.md`（转换节点）。
> 本文件记录 **reform 第四步**：新增 `Network & Layers/Regularization` 分类及其首个节点 `Dropout`。
> 计划文件：`.codebuddy/plans/dropout-node.md`（已确认，状态 ready/执行中）。

---

## 1. 需求（用户原文照录）

- **原始请求**：「请继续在normalization下新增Dropout节点。」
- **澄清答复**（4 问 4 答，原文）：
  1. **分类归属**：「新建 Network & Layers/Regularization(语义更准确（PyTorch 里 Dropout 独立于 normalization 成组），但会让分类数从 21 变 22，需要在分类树上新增一个家族。)」
  2. **随机数策略**：「Comfy的随机数自己有一套成熟的控制策略，按照最大众的方法走。」
  3. **覆盖范围**：「只做元素级 F.dropout(单节点自适应任意秩，符合本文件「一函数一节点」原则（BatchNorm 也没有 1d/2d/3d 三份）。)」
  4. **文档范围**：「4 份说明文件 + MEMORY.md(更新 comfydl/FUNCTIONS.md、FUNCTIONS_zh.md、README.md、README_zh.md（含计数），镜像到独立 checkout ComfyDL/，并更新 .codebuddy/memory/MEMORY.md。这是既有惯例。)」
     → 即：**不**新增 `docs/reform-step4-*.md`，**不**改动 `docs/comfydl-builtin.md`。
- **授权**：「接受计划并立即开始」

---

## 2. 设计决策

| 决策点 | 结论 | 依据 |
|---|---|---|
| 分类 | 新增 `REGULARIZATION_CATEGORY = "Network & Layers/Regularization"`，`NORMALIZATION_CATEGORY` 不动 | 用户答复 1；PyTorch 中 Dropout 与 Normalization 分属不同组 |
| 落点 | **直接加在既有文件 `comfy_extras/nodes_normalization.py`**（不新建文件、不改 `nodes.py` 白名单） | 该文件已有"一个文件承载两个分类"的先例（Normalization + Training）；Dropout 必须复用其私有 `_mode_input()` / `_normalize_mode()` / `_warn()`，同文件复用零耦合。备选"新建 `nodes_regularization.py`"被否决：需跨模块 import 私有 helper 或复制一份，且多改一处白名单 |
| schema | **不复用** `_normalization_schema()`（其 `category` 写死 Normalization），按 `NormalizationSpectralNorm` 的做法直接构造 `io.Schema` | 避免为单个节点给公共 helper 加参数 |
| node_id | `RegularizationDropout`（`display_name="Dropout"`） | 沿用 `Normalization*` / `Basic*` / `Activation*` 的家族前缀惯例；全仓无命名冲突 |
| 随机数 | `io.Int.Input("seed", default=0, min=0, max=0xFFFFFFFFFFFFFFFF, control_after_generate=True)`；执行体用**局部** `torch.Generator(device=tensor.device)` + `manual_seed(int(seed))` | 用户答复 2"最大众"：与 `comfy_extras/nodes_images.py::ImageAddNoise` 逐字一致；官方 `Comfy_CustomNode_Docs_md/custom-nodes/v3_migration.md` 明确 `control_after_generate=True` **等价于** `io.ControlAfterGenerate.randomize`。**不用** `torch.manual_seed()` + `F.dropout()`：后者没有 generator 参数、只能读进程全局 RNG，在长期运行的服务里会污染其它节点的随机态 |
| 缓存 | **不实现** `fingerprint_inputs` | seed 是普通 widget，已进入节点输入签名（`CacheKeySetInputSignature`）；官方 `server_overview.md` 对随机节点给出的建议正是"提供 seed 输入让用户控制可复现性" |
| p 边界 | `p <= 0` 恒等；`p >= 1` 返回同 shape/dtype/device 全零 | 显式短路规避 `1/(1-p)` 的除零；PyTorch 原生路径在该边界依赖 kernel 实现细节，本节点把语义定死为"全零" |
| mode 插槽 | 复用 `_mode_input()`，把它扩展为 `_mode_input(tooltip: str = _DEFAULT_MODE_TOOLTIP)` | 原 tooltip 写的是"用本次调用的统计量归一化"，直接复用到 Dropout 语义不符；加可选参数后 BatchNorm / InstanceNorm 的 tooltip 文本逐字不变 |

### 数值 / 性能要点
- 掩码：`keep = torch.rand(tensor.shape, generator=gen, device=tensor.device, dtype=torch.float32) >= p`，输出 `torch.where(keep, tensor / (1 - p), tensor.new_zeros(()))`。用**布尔**掩码（1 字节/元素，约为浮点掩码的 1/4）。
- 随机数**在张量自身设备上生成**，不走 CPU（`ImageAddNoise` 里的 `device="cpu"` 是历史兼容路径，照抄会引入主机同步 + 跨设备拷贝）。
- 快路径零开销：`eval`、`p<=0`、`p>=1` 三种情况**完全不进随机数分支**（推理图里放一个 Dropout 不产生额外计算）。
- 期望值正确性：保留元素除以 `1-p`，故 `E[output] == E[input]`。

---

## 3. 实现要点（`comfy_extras/nodes_normalization.py`）

新增/改动共 5 处，其余 9 个既有节点零改动：

1. 模块 docstring：标题改为 "Normalization and regularization nodes plus the train/eval state helpers"，分组清单增列 `regularizers - RegularizationDropout`，设计要点增列"**Randomness is seeded, never implicit**"。
2. 常量：`REGULARIZATION_CATEGORY = "Network & Layers/Regularization"`。
3. `_mode_input()` → `_mode_input(tooltip: str = _DEFAULT_MODE_TOOLTIP)`（新增模块常量 `_DEFAULT_MODE_TOOLTIP` 承载原文案，向后兼容）。
4. 新增模块级 helper `_dropout(tensor, p, seed)` + 节点类 `RegularizationDropout`（`define_schema` + `execute`）。
5. `NORMALIZATION_NODES` 末尾追加 `RegularizationDropout`（列表名未改，上方补注释说明它跨三个 Network & Layers 家族），`NormalizationExtension` docstring 改为 "Normalization / Regularization families"。

输入：`tensor`(TENSOR, 必填) → `p`(FLOAT, default=0.5, min=0.0, max=1.0, step=0.01) → `seed`(INT, default=0, min=0, max=0xFFFFFFFFFFFFFFFF, `control_after_generate=True`) → `mode`(`_mode_input()`，optional STRING，`force_input`)。输出：单个 `io.Tensor.Output(display_name="output")`。

`execute` 分支顺序：`eval` → 原样返回（连 `p=1` 也不置零，与 PyTorch `training=False` 一致）；`p<=0` → 原样返回；`p>=1` → `torch.zeros_like`；其余 → `_dropout(...)`。全程不原地写入、保持 dtype/device、任意秩（含 0 维标量与空张量）。

**冒烟测试器无需登记**：`run_smoke_test.py` 对 optional 且 io_type 属于 `_WIDGET_TYPES` 的输入按**声明 default** 造值，故 `mode` 拿到 `"train"`、`seed` 拿到 `0`，节点走 train 分支正常执行 —— 未加入 `_SKIPPED_NODES` / `_INPUT_OVERRIDES`。

---

## 4. 验证记录

### 4.1 三套计数（实测，非推算）

| 口径 | 改前 | 改后 | 测量方式 |
|---|---|---|---|
| ① ComfyDL 启动 banner | 102 / 16 分类（conversion 前）→ 108 / 17 | **108 节点 / 17 分类** | 冒烟输出 `[ComfyDL] Loaded 108 deep learning nodes` + `已注册 108 个节点，共 17 个分类` |
| ② 宿主注册表 | 245 / 34 | **246 节点 / 35 分类** | `run_smoke_test.py` 输出 `246 selected of 246 registered`；`--categories` 表尾 `registered nodes: 246 / categories: 35` |
| ③ 四份说明文件"内置节点库" | 139 / 21（核心 31） | **140 节点 / 22 分类（核心 32）** | 4 文件逐处核对一致 |

`Network & Layers` 核心家族：Activation 14 + Basic 8 + Normalization 7 + **Regularization 1** + Training 2 = **32**。
（说明：`comfy_extras` 的 6 个转换节点 **不属于** 核心，它们是 ComfyDL 侧 `comfydl/nodes/conversion.py`，只是并入核心分类 `utilities/conversion`。）

### 4.2 冒烟（`cdl_smoke_tests/run_smoke_test.py`，venv 运行，不启动服务）

- 改前基线：`223 PASS, 22 SKIP, 0 FAIL (of 245)`
- 改后：**`224 PASS, 22 SKIP, 0 FAIL (of 246)`** —— PASS 与总数各 +1，无 FAIL。
- `--categories` 确认新增 `Network & Layers/Regularization 1`，且该分类只可能来自 `comfy_extras.nodes_normalization`。

### 4.3 数值自检（临时脚本，已删除，未留仓库）

一次性脚本 `_tmp_dropout_check.py`（写在 workspace 根，跑完删除）覆盖：schema 契约（分类 / node_id / 输入顺序与默认值 / `control_after_generate=True` / mode 为 optional STRING / 单 TENSOR 输出）；`eval` 返回**同一对象**且 `p=1` 也不置零；`p=0` 恒等；`p=1` 全零且 shape/dtype/device 不变；train 输出为新张量且输入未被原地修改；保留元素恰为 `1/(1-p)`；同 seed **逐比特**复现、异 seed 不同；丢率 ≈ p（200×500 上 0.2996）且均值 ≈ 1.0；掩码确为逐元素（每行都同时有 0 与非 0）；rank 0/1/3/4 形状与 float64 保持；float16/float32/float64 与 device 保持；空张量 `(0,3)` 不报错；非连续输入正常；非法 `mode` 回落 train 并打印 `[Network & Layers]` 告警；`mode="EVAL"` 大小写不敏感；与 `torch.nn.functional.dropout` 的均值/置零比例统计一致。**全部通过。**

---

## 5. 文档与提交

### 5.1 4 份说明文件（`comfydl/`，镜像同步）

| 文件 | 改动 |
|---|---|
| `FUNCTIONS.md` | `## 17. … (31 nodes)` → `(32 nodes)`；导语五组（增 Regularization）；新增 `### 17.5 Regularization (1 node)`（节点表 + 一段说明：秩自适应 / 局部 generator 的 seed 语义 / 为何需要 mode 插槽 / p 边界短路）；概览 `139→140`、`21→22 categories`、明细增 `+ 1 core Network & Layers/Regularization`；分类表增 `Network & Layers/Regularization | 1`；脚注"纯核心分类"清单增该分类 |
| `FUNCTIONS_zh.md` | 与上同构的中文改动（`### 17.5 Regularization（1 个节点）`） |
| `README.md` | `139→140 nodes / 21→22 categories`；`plus 31 → 32 ComfyUI core nodes` 并在括号内增 `Regularization 1`；分类表增一行 |
| `README_zh.md` | 同上（中文） |

已用 grep 复核：4 份文件中**无**残留 `139` / `21 categories` / `21 个类别` / `31 nodes` / `31 个节点` / `plus 31` 等旧值（剩余 `21 个类别` 命中均为 PASCAL VOC 的类别数，非本口径）。

### 5.2 提交

- 子模块 `comfydl`（main）：`f92a61d` — `docs(dropout): document the core Dropout node and refresh node/category counts`
- 主仓 `ComfyDL_UI`（master）：`7d52ca9` — `feat(regularization): add the core Dropout node under Network & Layers/Regularization`；`7b12bd3` — `chore(submodule): bump comfydl to f92a61d (dropout docs)`
- 均已 `pull --rebase` 后推送成功。
- 独立 checkout `P:\Dev\ComfyUI_Refs\ComfyDL`：拉取前其工作区**残留着与远端逐字节相同的本地改动**（4 份文档 + 未跟踪 `nodes/conversion.py`，SHA256 与子模块完全一致，说明是上一轮手工同步留下的），处理方式为 `git stash push -u` → `git pull origin main`（快进 `64282e1..f92a61d`）→ 再次哈希比对**全 SAME**；现工作区干净。
- ⚠ **遗留**：该镜像现存 1 条冗余 stash（内容与远端逐字节相同），**是否删除待用户确认**（未擅自 `stash drop`）。

---

## 6. 交接要点 / 已知限制

- **Dropout 不参与任何全局状态**：没有 hidden 输入、没有 `fingerprint_inputs`、不做缓存自省；训练/推理开关与其它 Network & Layers 节点共用同一条 `TrainingMode.mode` 连线通道（即"全局状态必须走真实连线"这条既有结论的延续）。
- `seed` **只在 train 模式被读取**；`eval` 模式下完全不抽随机数，因此前端把下拉设为 randomize 时输出也不会变（节点仍会因输入签名变化被重算一次，但结果相同）。
- **通道级 dropout**（如 RNN 的 `dropout=` 语义）**未实现**，按用户答复 3 只做元素级；若将来需要，另建节点（可放同一 `Regularization` 分类，本次引入的常量与分类无需改动）。
- **无 `inplace` 选项**：项目规则要求无状态、不得原地写入（同一张量可能被图中多个节点共享）。
- 前端"生成后控制"下拉的默认档位由前端包（`comfyui-frontend-package`）实现；后端只声明 `control_after_generate=True`，官方文档定义其等价于 `randomize`。若前端表现与此不符，属前端包行为，非本节点缺陷。
- 计划文件 `.codebuddy/plans/dropout-node.md` 中的 todo 全部完成。
