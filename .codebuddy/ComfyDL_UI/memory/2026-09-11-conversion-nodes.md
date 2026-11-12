# 2026-09-11 开发日志（追加）— Comfy 语义类型 ↔ TENSOR 通用转换节点

> **本文件是任务进行中的上下文留底备份**，用于对话中断后交接。
> 承接 `2026-09-11.md`（reform 第二、三步），但**主题完全独立**：本任务是「转换节点」，
> 不改动 reform 的 Network & Layers 任何内容。
> 计划文件：`.codebuddy/plans/comfy-type-tensor-conversion-nodes.md`（已确认，状态 ready/执行中）。

---

## 1. 需求（用户需求原文照录）

### 1.1 原始意图
新增一组「Comfy 语义类型 ↔ 通用 TENSOR」的双向转换节点：把 IMAGE / MASK / LATENT（核心四类）等
转换为通用 TENSOR 做张量运算，再转回 Comfy 类型。核心诉求是**一个 convert 节点**（而非每类型一个），
偏好「精细控制型 UI」，**健壮性优先**。

### 1.2 第一轮需求确认（用户 4 个回答，原文）
- **q-0 覆盖范围**：「四类 + 预留四类(上述再加 AUDIO / SIGMAS / LORA_MODEL / LOSS_MAP（无产出节点，只能静态检查，冒烟测试需自造输入）)」
- **q-1 TENSOR → Comfy 类型方向架构**：「选择B和C中健壮性较强的那个」
- **q-2 LATENT 往返语义**：「无损往返(额外输出/输入元数据（noise_mask / batch_index / type），保证 ToTensor→FromTensor 完全还原原字典)」
- **q-3 节点位置与分类**：「尽量坐进ComfyDL仓库里，但是通过引用逻辑使其成为Comfy核心节点（utilities/conversion）」

### 1.3 第二轮实现细节确认（用户 3 个回答，原文）
- **q-0 张量集合映射**：「这种转换单独做额外节点，仅把那种能明确单入单出的类型坐进通用转换节点，其余的有特殊结构的都单做」
- **q-1 元数据承载**：「同1，根据我的意思，比较像是C的平行槽位」
- **q-2 发放节点未接锚点时怎么办**：「好像这种有输入和输出的节点，在输出没有任何连线的情况下，ComfyUI的DAG剪枝策略会将其优化掉，所以我觉得这种情况不会发生。你可以再确认一下」

### 1.4 用户授权（本轮）
- 「接受计划并立即开始，同时你可以改一下global git config。」

---

## 2. 架构决策：采用「联合输入 + 具体类型输出槽」，完全不用 MatchType

### 2.1 为什么放弃 B/C（锚点）方案 —— 三条源码级事实

1. **后端对 MatchType 无条件放行**。`comfy_execution/validation.py:31-34` 原文：
   ```python
   # If the received type or input_type is a MatchType, we can return True immediately;
   # validation for this is handled by the frontend
   if received_type == IO.MatchType.io_type or input_type == IO.MatchType.io_type:
       return True
   ```
   → 任何 MatchType 方案在后端拿不到任何错配保护。

2. **后端校验取上游静态 `RETURN_TYPES[槽位]`**（`execution.py:934-937`）。只有**具体类型输出槽**
   才能进入 `validate_node_input` 的精确匹配；联合类型串只能做集合求交，MatchType 直接短路放行。

3. **用户 q-2 的「剪枝」推断不成立**。剪枝判据是「能否反向到达某个 `OUTPUT_NODE=True` 节点」
   （`execution.py:779-780` 的 `execute_outputs` 入队 + `validate_prompt` 的 `execution.py:1163-1165`
   收集输出节点），**与「锚点是否接线」完全无关**。把发放节点输出接到任意预览节点、锚点留空，
   该节点照样执行，而后端又因 MatchType 放行而不报错 → 形成**静默错配**。

### 2.2 采用的范式（官方同类先例）

`comfy_extras/nodes_number_convert.py` 的 `NumberConvertNode`（全文 91 行）：

```python
class NumberConvertNode(io.ComfyNode):
    """Converts various types to numeric FLOAT and INT outputs."""
    @classmethod
    def define_schema(cls) -> io.Schema:
        return io.Schema(
            node_id="ComfyNumberConvert",
            display_name="Convert Number",
            category="utilities",
            search_aliases=[...],
            inputs=[
                io.MultiType.Input("value", [io.Int, io.Float, io.String, io.Boolean],
                                   display_name="value"),
            ],
            outputs=[
                io.Float.Output(display_name="FLOAT"),
                io.Int.Output(display_name="INT"),
            ],
        )
    @classmethod
    def execute(cls, value) -> io.NodeOutput:
        ...
        return io.NodeOutput(float_val, int_val)
```

**要点**：联合输入用 `io.MultiType.Input`，输出全用**具体类型槽**，**零 MatchType**，`category="utilities"`。
健壮性严格优于 B/C：输出槽静态确定 → 前端物理隔离 **且** 命中后端精确校验；不存在锚点状态，
用户 q-2 的整个问题域自动消失。

---

## 3. 已核实的源码结论（全部实读，带行号）

### 3.1 注册机制（q-3「物理在 ComfyDL、逻辑是核心节点」天然成立，零改动）

- `ComfyDL_UI/nodes.py:745-758` `init_builtin_dl_nodes()`：
  ```python
  dl_class_mappings = getattr(comfydl, "NODE_CLASS_MAPPINGS", {}) or {}
  for name, node_cls in dl_class_mappings.items():
      if name in NODE_CLASS_MAPPINGS:
          logging.warning("ComfyDL node '{}' collides ...".format(name)); continue
      NODE_CLASS_MAPPINGS[name] = node_cls
      node_cls.RELATIVE_PYTHON_MODULE = getattr(node_cls, "__module__", None) or "comfydl"
  ```
  → 直接 `import comfydl` 合并，**不经过 `load_custom_node`**，不调用 `comfy_entrypoint`。
- `comfydl/nodes/__init__.py:51-64`：目录自动扫描所有 .py 并合并模块级 `NODE_CLASS_MAPPINGS`。
- `nodes.py:569-579`：V1（`NODE_CLASS_MAPPINGS`）分支**优先**，V3（`comfy_entrypoint`）是 elif，互斥。
- `comfy_api/latest/_io.py:2178-2183`：`_ComfyNodeBaseInternal.INPUT_TYPES()` 是 `@final` classmethod，
  由 schema 派生 → **V3 类自带 legacy shim，双路兼容**。
- `server.py:754-765`：`/object_info` 优先走 V3 分支 `obj_class.GET_NODE_INFO_V1()` → MultiType 能正确暴露给前端。
- `execution.py:160,274,503,880` 用 `issubclass(class_def, _ComfyNodeInternal)` 判定 V3，
  而 `io.ComfyNode → _ComfyNodeBaseInternal → _ComfyNodeInternal` → **放进 NODE_CLASS_MAPPINGS 的 V3 类会被正确执行**。

**⚠ 关键约束：comfydl 里不要写 `comfy_entrypoint`（host 不会调用它）。V3 类必须放进 `NODE_CLASS_MAPPINGS`。**

### 3.2 IO 类型（`comfy_api/latest/_io.py`）

- `ComfyTypeIO`（`:236-244`）**兜底提供 `Input` 和 `Output` 两个内部类** → 只要是 ComfyTypeIO 子类，
  既能作输入也能作输出，即使作者没显式写。只有 `Autogrow` 是 `ComfyTypeI`（仅输入）。
- 逐个类型：

| 类型 | 行号 | io_type | Python Type | 可作输入 |
|---|---|---|---|---|
| `io.Tensor` | 449-458 | `"TENSOR"` | `torch.Tensor` | ✅ |
| `io.Image` | 420-422 | `"IMAGE"` | `torch.Tensor` | ✅ |
| `io.Mask` | 444-446 | `"MASK"` | `torch.Tensor` | ✅ |
| `io.Latent` | 460-470 | `"LATENT"` | `LatentDict`（samples / noise_mask / batch_index / type） | ✅ |
| `io.Audio` | 665-670 | `"AUDIO"` | `AudioDict`（waveform / **sampler_rate**） | ✅ |
| `io.Sigmas` | 597-599 | `"SIGMAS"` | `torch.Tensor` | ✅ |
| `io.String` | 322-342 | `"STRING"` | `str` | ✅（WidgetInput） |
| `io.Int` | 267-291 | `"INT"` | `int` | ✅（WidgetInput） |
| `io.Array` | 965-967 | `"ARRAY"` | `list` | ✅（继承的 **plain Input**，非 widget） |
| `io.Dict` | 961-963 | `"DICT"` | `dict` | ✅ |
| `io.MultiType` | 969-1016 | `"COMFY_MULTITYPED_V3"` | `Any` | ✅（Input only） |

**⚠ AUDIO 字段名实际拼作 `sampler_rate`**（不是 `sample_rate`），实现与文档必须沿用。

- `io.MultiType.Input` 签名（`:976`）：`(id, types, display_name=None, optional=False, tooltip=None, lazy=None, extra_dict=None, raw_link=None, advanced=None)`
  `get_io_type()`（`:1005-1010`）返回**逗号分隔大写串**（去重保序），如 `"IMAGE,MASK,LATENT,AUDIO,SIGMAS"`。

### 3.3 `io.Schema`（`_io.py:1654-1720`，dataclass）

字段：`node_id`（必填）、`display_name=None`、**`category="sd"`（默认！必须显式传）**、`inputs`、`outputs`、
`hidden`、`description=""`、`search_aliases`、`is_input_list`、`is_output_node`、`is_deprecated`、
`is_experimental`、`is_dev_only`、`is_api_node`、`price_badge`、`not_idempotent`、`enable_expand`、
`accept_all_inputs`、`essentials_category`、`has_intermediate_output`。

`inputs`/`outputs` 元素必须是 `io.Xxx.Input(...)` / `io.Xxx.Output(...)` 实例。

### 3.4 Output 的 display_name（`_io.py:216-233`）

`display_name = display_name if display_name else id`；省略时净效果 = 显示 io_type 字符串。
写 `display_name="IMAGE"` 只是把次序显式化，**语义与省略一致**。

### 3.5 CATEGORY / GET_SCHEMA 赋值链（决定 `comfydl/__init__.py` 是否需要改）

```python
# _io.py:2066-2072
_CATEGORY = None
@final
@classproperty
def CATEGORY(cls):
    if cls._CATEGORY is None:
        cls.GET_SCHEMA()
    return cls._CATEGORY

# _io.py:2193-2240
@final
@classmethod
def GET_SCHEMA(cls) -> Schema:
    cls.VALIDATE_CLASS()
    schema = cls.FINALIZE_SCHEMA()
    schema.validate()
    if cls._CATEGORY is None:
        cls._CATEGORY = schema.category      # ← :2202-2203 赋值点
    ...
```

`classproperty` 定义在 `comfy_api/internal/__init__.py:92-96`，类访问 `cls.CATEGORY` 即触发 getter。

**✅ 结论：`comfydl/__init__.py` 的 `_print_startup_info()` 无需改动即可正确统计 V3 节点** ——
`getattr(cls, "CATEGORY", ...)` 会走 classproperty → 自动触发 `GET_SCHEMA()` → 返回 `schema.category`。
**唯一要求**是 `define_schema` 里显式给 `category`（否则返回默认 `"sd"`）。

**⚠ 但存在一个真实健壮性风险（本次要修）**：`comfydl/__init__.py:25,35-36` 的整个 `Counter` 被
`try/except` 包裹，**一个 V3 类的 schema 异常会拖垮整张统计表**（而不是只跳过该节点）。
计划要求「proactively add error handling」→ 本次把统计改成逐类容错。

### 3.6 冒烟测试器（`cdl_smoke_tests/run_smoke_test.py`）

- `_VALUE_FACTORIES`（`:385-403`）已有的 key：`INT, FLOAT, STRING, BOOLEAN, COMBO, TENSOR, IMAGE,
  MASK, LATENT, CDLMODEL, CDLDATALOADER, CDLVOCAB, ARRAY, DICT, COLOR, BOUNDING_BOX, "*"`。
  → `ARRAY`（`[1.0,2.0,3.0]`，`:342-344`）、`DICT`（`{"a":1}`，`:347-349`）**都有 dummy**。
  → **`AUDIO` / `SIGMAS` 无 dummy**。
- `_synthesise()`（`:471-481`）按 `str(type_name).split(",")` 取**第一个可命中的备选** →
  `"IMAGE,MASK,LATENT,AUDIO,SIGMAS"` 会命中 `IMAGE` → 联合输入**自动 PASS**。
- `_v3_inputs()`（`:575-618`）关键行为：
  - `:597-598`：**`optional=True` 且 io_type 不在 `_WIDGET_TYPES={"INT","FLOAT","STRING","BOOLEAN","COMBO"}`
    的输入 → 直接 `continue`，不造值、不传给 execute**。
    → optional 的 TENSOR/ARRAY/DICT 都不传参，**`execute` 必须给这些参数默认值**！
  - optional 的 widget 类型（INT/FLOAT/STRING/BOOLEAN/COMBO）**会**造值。
  - `:610-611`：`required` 只记录非 optional 的槽。
  - `:617-618`：输出 arity = `len(schema.outputs)`；`is_output_node=True` 允许少返回。
- `_INPUT_OVERRIDES`（`:440-468`）：`dict[str, dict[str, Callable[[dict, str], Any]]]`
  = **节点 id → {输入名 → 造值函数}**，对 V3 同样生效（用 `slot.id` 匹配）。
- 主循环（`:781-814`）：`_SKIPPED_NODES` 黑名单 / 造值 `Unsupported` / `TimeoutError` → **SKIP**；
  execute 抛异常或输出数不符 → **FAIL**；正常 → **PASS**。**SKIP 永远不会让冒烟失败。**

### 3.7 既有约定（`comfydl/nodes/misc.py`、`tensor_basic.py` 实读）

- 模块 docstring：三引号，首行概述 + `Nodes:` 清单。
- 节点类 docstring：一句话概述 + 详细段，用 `Inputs:` / `Outputs:` 分节（**不是 Args/Returns**）。
- 模块级建空 dict，**每个节点类定义后立即逐条赋值**：
  ```python
  NODE_CLASS_MAPPINGS = {}
  NODE_DISPLAY_NAME_MAPPINGS = {}
  ...
  NODE_CLASS_MAPPINGS["CdlTimer"] = CdlTimer
  NODE_DISPLAY_NAME_MAPPINGS["CdlTimer"] = "Timer (Benchmark)"
  ```
- 显示名用 unicode 箭头 `"Tensor \u2192 String"`。
- **非 `d2l/*` 分类已有先例**：`"utilities"`（`misc.py:87,144,191,237`）、`"image/color"`、
  `"image/transform"`、`"image"`（`image_tools.py`）。
- comfydl 现有 100+ 节点**全部是 legacy 风格**，`GET_SCHEMA`/`define_schema` 在 `comfydl` 下 **0 命中**
  → **`conversion.py` 将是 comfydl 首个 V3 节点文件**。
- `misc.py:1-6` 模块 docstring 写明「merged into the ComfyUI core "utilities" category ...
  the ComfyDL/Misc category no longer exists」→ 与本次「归入核心 `utilities/conversion`」的思路一脉相承。

### 3.8 命名冲突检查

- `comfydl/nodes/conversion.py` **不存在**。
- 节点 id `CdlToTensor` / `CdlFromTensor` / `CdlValueToTensor` / `CdlTensorToValue` **全部不存在** → 无重名风险。
- `comfydl/nodes/` 现有 17 个 .py：`datasets / device_utils / gan / image_tools / misc /
  model_attention / model_cv / model_nlp / model_seq2seq / model_utils / nlp_utils /
  object_detection / semantic_segmentation / tensor_basic / tensor_ops / visualization / __init__`。

### 3.9 4 份说明文件结构

- `comfydl/FUNCTIONS.md`(EN, 1757 行) / `FUNCTIONS_zh.md` / `README.md` / `README_zh.md`。
- FUNCTIONS：顶部 `## Data Types` 表 → **编号分类节** `## N. <组名> (M nodes)` → `### <显示名>` 条目。
- 条目模板（`FUNCTIONS.md:191-206` MessageBox）：
  ```markdown
  ### MessageBox
  - **Class**: `CdlMessageBox`
  - **Purpose**: ...
  - **Inputs**:
    | Name | Type | Default | Description |
    |------|------|---------|-------------|
  - **Outputs**:
    | Name | Type | Description |
    |------|------|-------------|
  ```
- README：`## Function Overview`（`:77`/`:75`）放分类汇总表（Category | Count | Description）。
- **硬编码计数的四处**：`README.md:79-104`、`README_zh.md:77-102`、
  `FUNCTIONS.md:1727-1756` 的 `## Appendix → ### Total Node Count`、`FUNCTIONS_zh.md:1708` 的 `## 附录`；
  **各分类节标题本身也硬编码**（如 `FUNCTIONS.md:34` `## 1. d2l / Device Utils (3 nodes)`）。
- 现有非 d2l 分类节：`## 4. ComfyUI / utilities (4 nodes)`（`:187`）、`## 14. ComfyUI / image/color (3 nodes)`、
  `## 15. ComfyUI / image/transform (1 node)`、`## 16. ComfyUI / image (1 node)`、
  `## 17. ComfyUI / Network & Layers (31 nodes)`。
- **三套计数口径都不要互相"纠正"**（`MEMORY.md:38`）：
  ① ComfyDL 启动 banner = 102 节点 / 16 分类；
  ② 宿主注册表（冒烟 `--categories` 或 `/object_info`）= 239 节点 / 33 分类；
  ③ 四份说明文件的"内置节点库"口径 = 133 节点 / 20 分类（= ComfyDL 102 + 核心 31）。
  **本次新增节点会影响 ①②③ 三套口径中的 ①（+N）与 ③（+N），② 视是否纳入冒烟范围而定。**

---

## 4. 计划 Todo 与进度

| # | todo id | 内容 | 状态 |
|---|---|---|---|
| 1 | `verify-v3-pattern` | 用 code-explorer + lsp 核实 V3 槽位签名/命名/文档模板 | ✅ **已完成**（子代理报告已产出，见 §3） |
| 2 | `impl-collect-node` | 通用收集节点（MultiType 入 + 平行元数据出） | ✅ 已完成（`CdlValueToTensor`） |
| 3 | `impl-dispatch-node` | 通用发放节点（5 个具体类型出 + optional 元数据入） | ✅ 已完成（`CdlTensorToValue`） |
| 4 | `impl-collection-nodes` | LORA_MODEL / LOSS_MAP 打包解包节点对（预留） | ✅ 已完成（4 个节点） |
| 5 | `fix-v3-category` | 修 `comfydl/__init__.py` 的 V3 CATEGORY 统计（逐类容错） | ✅ 已完成 |
| 6 | `test-roundtrip-smoke` | 往返专项测试 + 全量冒烟零 FAIL + 清理临时文件 | ✅ 已完成（75 passed / 0 failed；全量 0 FAIL） |
| 7 | `update-docs-sync` | 更新 comfydl 4 份说明文件，再镜像到 `ComfyDL/` | ✅ 已完成（哈希校验 MATCH） |
| 8 | `commit-submodule` | 子模块提交 + 父仓库 gitlink 同步 | ✅ 已完成（子模块 3 提交 + 父仓 2 提交） |

---

## 5. 交付物清单

```
p:/Dev/ComfyUI_Refs/ComfyDL_UI/            # 宿主仓库（Git 根，master）
├── comfydl/                                # ComfyDL 子模块工作区
│   ├── __init__.py                         # [MODIFY] 统计函数逐类容错
│   ├── nodes/conversion.py                 # [NEW] 6 个 V3 节点类 + 模块级映射
│   ├── FUNCTIONS.md / FUNCTIONS_zh.md      # [MODIFY]
│   └── README.md / README_zh.md            # [MODIFY]
├── cdl_smoke_tests/
│   ├── run_smoke_test.py                   # [MODIFY] 仅在 dummy 缺失导致 SKIP 时补
│   └── test_conversion_roundtrip.py        # [NEW] 往返专项测试
p:/Dev/ComfyUI_Refs/ComfyDL/                # 独立克隆（自带 .git，需同步）
└── nodes/conversion.py + 4 份说明文件      # [SYNC] 从 comfydl 镜像
```

---

## 6. 关键约束与已踩实的坑

1. **`io.Schema.category` 默认是 `"sd"`**，不显式写就会跑偏 → 必须显式 `category="utilities/conversion"`。
2. **AUDIO 字段拼作 `sampler_rate`**（源码 `_io.py:665-670`），不得写成 `sample_rate`。
3. **烟测对 `optional=True` 的非 widget 输入（TENSOR/ARRAY/DICT）会跳过不传值**
   → `execute` 必须给这些参数默认值，否则 `TypeError: missing required positional argument` → FAIL。
4. **comfydl 的 V3 节点必须走 `NODE_CLASS_MAPPINGS`**，写 `comfy_entrypoint` 不会被调用。
5. **`Cdl` 前缀必须保留**：`nodes.py:752-754` 有节点名碰撞检查。
6. **冒烟盲区**：联合输入只命中第一个分支（IMAGE），MASK/LATENT/AUDIO/SIGMAS 路径与元数据往返
   都需靠新增的往返专项测试覆盖。
7. **git safe.directory 已在本次配置完成**（用户授权）：
   ```
   P:/Dev/ComfyUI_Refs/ComfyDL_UI
   P:/Dev/ComfyUI_Refs/ComfyDL_UI/comfydl
   P:/Dev/ComfyUI_Refs/ComfyDL
   ```
   → `git -c safe.directory=...` 前缀不再是必需（但仍可用）。
8. **临时文件必须清理**；往返测试脚本本身作为长期资产保留于 `cdl_smoke_tests/`。
9. **回归红线**：不得触碰现有 100+ legacy 节点行为；全量冒烟必须零 FAIL。
10. **读取本目录记忆文件时**：PowerShell `Get-Content` 需 `-Encoding utf8`，否则中文乱码（文件本身 UTF-8 无害）。

---

## 7. 下一步（下一位 agent 从这里继续）

1. 读 `comfydl/nodes/misc.py` / `tensor_basic.py` 确认风格（已完成，见 §3.7）。
2. 新建 `comfydl/nodes/conversion.py`，按 §2.2 范式实现：
   - 通用收集 `CdlValueToTensor`：`io.MultiType.Input("value", [io.Image, io.Mask, io.Latent, io.Audio, io.Sigmas])`
     → 输出 `TENSOR` + `origin`(STRING) / `noise_mask`(TENSOR) / `batch_index`(ARRAY) /
     `latent_type`(STRING) / `sampler_rate`(INT, default 44100)。
   - 通用发放 `CdlTensorToValue`：输入 `TENSOR` + 上述元数据（**全部 optional，且 execute 必须有默认值**）
     → 输出 `IMAGE` / `MASK` / `LATENT` / `AUDIO` / `SIGMAS` 五个具体类型槽，未使用槽返回 `None`。
   - LORA_MODEL / LOSS_MAP 打包解包节点对（预留，标注 experimental）。
   - 全部 `category="utilities/conversion"`，`search_aliases` 覆盖 `to tensor` / `convert` / `cast` 等。
3. 修 `comfydl/__init__.py` 统计函数（逐类 try/except）。
4. 写 `cdl_smoke_tests/test_conversion_roundtrip.py`，跑全量冒烟。
5. 更新 4 份说明文件 → 同步 `ComfyDL/`。
6. 子模块提交 + 父仓 gitlink。

---

## 8. 完成快照（2026-09-11 收尾）

### 8.1 实际交付

| 文件 | 状态 |
|---|---|
| `comfydl/nodes/conversion.py` | 新建，784 行，6 个 V3 节点 + 模块级映射 |
| `comfydl/__init__.py` | 修改，统计逐类 try/except（失败归 `d2l/Unknown` 并单独列出） |
| `cdl_smoke_tests/run_smoke_test.py` | 新增 `_f_lora_model` / `_f_loss_map` + `_INPUT_OVERRIDES` 两条 |
| `cdl_smoke_tests/test_conversion_roundtrip.py` | 新建，12 个用例 / 75 条断言 |
| 4 份说明文件（comfydl） | FUNCTIONS.md / FUNCTIONS_zh.md / README.md / README_zh.md 全部更新 |
| 4 份说明文件 + `nodes/conversion.py`（`ComfyDL/` 独立克隆） | 已镜像，哈希 MATCH |

### 8.2 三套计数口径的最终值（实测，非推算）

- ① ComfyDL 启动 banner：**108 节点 / 17 分类**，含 `utilities/conversion: 6`
- ③ 四份说明文件「内置节点库」口径：**139 节点 / 21 分类（ComfyDL 108 + 核心 31）**
- ② 宿主注册表（冒烟全量）：**245 节点**，本轮 6 个新节点已全部纳入

### 8.3 测试结果

- 往返专项：`=== conversion round trip: 75 passed, 0 failed (12 test cases) ===`
- 全量冒烟：`=== smoke result: 223 PASS, 22 SKIP, 0 FAIL (of 245) ===`

### 8.4 实现期发现并修掉的真实 bug

`_split_tensors` 假定输入是 1-D；`CdlTensorToLoraModel` 收到多维扁平张量时抛
`RuntimeError: shape '[1]' is invalid for input of size 3`。修法：入口先
`if not isinstance(flat, torch.Tensor): raise TypeError(...)` 再 `flat = flat.reshape(-1)`，
并补一条回归用例「multi-dimensional input is flattened first」。
另：`_decode_shape` 对畸形形状串显式返回 `None`，由调用方带提示降级为 0-D 标量（而非静默跳过）。

### 8.5 提交记录

子模块 `comfydl`（分支 `main`，远程 `Cynthia-lxx/ComfyDL`）：

```
d56c755 docs(conversion): document the 6 conversion nodes and refresh node/category counts
ad058f8 fix(startup): keep one bad node out of the whole category stats table
b94ae0a feat(conversion): add 6 Comfy value <-> TENSOR conversion nodes on the V3 API
```

父仓库 `ComfyDL_UI`（分支 `master`）：

```
66741fa chore(submodule): bump comfydl to d56c755 (conversion nodes)
12cf8af test(conversion): add a round-trip suite and smoke dummies for the conversion nodes
```

### 8.6 ⚠ 遗留事项（下一位 agent 或用户处理）

**均未 push**。子模块 3 个提交只在本地；父仓库 gitlink 指向的 `d56c755` 尚未出现在远程，
因此父仓提交在 push 前必须先 push 子模块，否则他人 clone 会缺对象。

`p:/Dev/ComfyUI_Refs/ComfyDL/`（独立克隆，与子模块**同一个远程** `Cynthia-lxx/ComfyDL`，
当前 HEAD 仍为 `64282e1`）中镜像文件处于**未提交**状态：

```
 M FUNCTIONS.md / FUNCTIONS_zh.md / README.md / README_zh.md
?? nodes/conversion.py
```

**不要在该克隆里另做一个 hash 不同的提交**（会与子模块提交在同一远程上分叉）。推荐做法：
先在子模块 `git push origin main`，再到该克隆 `git fetch origin && git reset --hard origin/main`
（`--hard` 属破坏性操作，须用户明确许可）。

---

## 9. 补记（同日）：这 6 个节点「在节点库里找不到」的根因与修复

### 9.1 用户报告

「conversion 节点没有出现在节点列表里，也没有找到。可能是没有注册或者没有导入。」（同时授权删掉镜像里那条冗余 stash）

### 9.2 先排除的两件事（都实测过，均非根因）

| 假设 | 实测 | 结论 |
|---|---|---|
| 没有导入 | 冒烟启动 banner `[ComfyDL] Loaded 108 deep learning nodes` + `共 17 个分类`，其中 `utilities/conversion: 6`；`comfydl/nodes/__init__.py` 的 importlib 动态聚合确实收到了该模块 | 导入正常 |
| 没有注册 / 序列化抛错被丢弃 | 进程内复现 `server.py` 的 `node_info()`：`issubclass(cls, _ComfyNodeInternal)` 为 True、`GET_NODE_INFO_V1()` 正常返回（category = `utilities/conversion`）；真实服务 `/object_info` 共 247 条且含全部 6 个 `Cdl*` 转换节点（用户当时在跑的 8188 实例同样如此） | 注册与下发都正常 |

### 9.3 真正的根因：前端节点库的分区只看 `python_module`（与 `CATEGORY` 无关）

前端 `comfyui_frontend_package==1.51.9` 的 `settingStore-*.js`：

```js
// getNodeSource()：把 python_module 按 "." 切开，只看首段
Pt = ["nodes", "comfy_extras", "comfy_api_nodes"]        // 只有这三个前缀算 Comfy Core
parts[0] in Pt ? Core
  : parts[0] == "custom_nodes" ? CustomNodes
  : parts[0] == "blueprint"    ? Blueprint
  : Unknown
// classifyNodes()：只有 Core / Essentials 进节点库的「Comfy」区
(nodeSource.type === Core || nodeSource.type === Essentials) ? comfyNodes : extensions
```

6 个转换节点被 `init_builtin_dl_nodes()` 合入核心注册表时，`RELATIVE_PYTHON_MODULE` 取的是
`getattr(node_cls, "__module__")` = **`comfydl.nodes.conversion`**，首段 `comfydl` 不在白名单 →
**Unknown → 整体被丢进节点库的「Extensions」区**。而文档、分类树与当初的设计意图都把它们视为
**Comfy 核心节点**（分类 `utilities/conversion`，与核心 `utilities` 并列），所以在「Comfy」区里
当然找不到。

### 9.4 修复（两处）

- `comfydl/nodes/conversion.py`：新增模块常量 + 基类，6 个节点类改为继承 `_CoreMergedNode`
  （仍是 `io.ComfyNode`，V3 语义不变）：
  ```python
  PYTHON_MODULE = "comfy_extras.nodes_comfydl_conversion"

  class _CoreMergedNode(io.ComfyNode):
      PYTHON_MODULE = PYTHON_MODULE
  ```
- `nodes.py:init_builtin_dl_nodes()`：优先采纳节点自己声明的模块名，并把日志里的 `expected ~102`
  更正为 `~108`：
  ```python
  node_cls.RELATIVE_PYTHON_MODULE = (
      getattr(node_cls, "PYTHON_MODULE", None)
      or getattr(node_cls, "__module__", None)
      or "comfydl"
  )
  ```
- 安全性依据：`RELATIVE_PYTHON_MODULE` 在后端**只**用于填 `/object_info` 的 `python_module`
  （`server.py` 的 V1 分支与 `comfy_api/latest/_io.py` 的 `GET_NODE_INFO_V1()`），不会被 import，
  因此用一个"核心形态"的模块名是安全的。

### 9.5 验证

- 进程内按前端 `getNodeSource()` / `classifyNodes()` 逻辑复算：6 个转换节点 = **Comfy Core**；
  `d2l/*` 等未声明 `PYTHON_MODULE` 的 ComfyDL 节点仍为 Extensions（符合"教学节点"定位）。
- 真实服务 `/object_info`：6 个节点的 `python_module` 均为 `comfy_extras.nodes_comfydl_conversion`，
  总条目 247、`Cdl*` 108 条不变。
- 全量冒烟：**224 PASS / 22 SKIP / 0 FAIL (of 246)**，与修复前一致（节点数与执行行为未变）。
- 节点计数未变 → 4 份说明文件无需改动。

### 9.6 提交与镜像

- 子模块：`3155180 fix(conversion): report a Comfy core python_module so the nodes land in the Comfy section`
- 父仓库：`2a92722 fix(nodes): let merged ComfyDL nodes declare their frontend python_module` +
  `9257c2f chore(submodule): bump comfydl to 3155180`
- 均已 push；独立克隆 `P:\Dev\ComfyUI_Refs\ComfyDL` 已 `git pull` 到 `3155180`，工作区干净；
  历史遗留的冗余 stash 已按用户许可删除 —— §8.6 的遗留事项至此全部清账。

### 9.7 仍待注意

- 用户当时在跑的 8188 实例仍是旧代码：**需重启服务 + 刷新页面**才会看到分区变化（前端只在页面加载
  时取一次 `/object_info`，`cache: 'no-store'`）。
- 若希望 `d2l/*` / 原 Misc / Image Tools 那批 ComfyDL 节点也进「Comfy」区，只需给对应类补
  `PYTHON_MODULE`；当前按"教学节点"定位让它们留在 Extensions 区。
