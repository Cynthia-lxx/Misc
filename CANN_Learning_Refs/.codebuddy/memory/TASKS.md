# CANN_OpHelper — 任务清单（Task List）

> 本文件持久化当前任务列表与状态，作为跨会话的单一事实来源（single source of truth）。
> 更新规则：完成任务后立即修改状态；与会话内 todo 工具保持一致。
> 最后更新：2026-09-05

## 状态图例

- `pending`：未开始
- `in_progress`：进行中
- `completed`：已完成
- `cancelled`：已取消/不再需要

## 当前任务（Phase 1 地基）

| ID | 任务 | 状态 | 说明 / 进展 |
|---|---|---|---|
| init-project | 初始化包结构与文档，复用既有 venv `penv` 安装依赖 | completed | 包结构/pyproject/README/gitignore/引导脚本已建；**依赖已由用户装入工作区根 `penv`（2026-09-04），冒烟 40/40 通过** |
| official-patterns | 调研官方 msopgen 命令与 Add 样例，沉淀"官方模式基准" | completed | 已产出 `docs/official-patterns.md`，逐条标注出处；基准：03.02 章 AddCustomTemplate 工程（op_kernel/op_host 实测源码） |
| core-models | 算子元信息模型 + YAML 读写 + msopgen 命令生成与元信息记录 | completed | 已建 `model.py`(OpSpec/TensorSpec/AttrSpec/OpSpecError) + `yamlio.py`(load/dump) + `msopgen.py`(命令拼装/云端说明)；附 `examples/add.yaml` 与 `tests/` 三文件 |
| template-engine | Jinja2 模板引擎 + 核间/核内均分 Tiling 策略 + Element-Wise Kernel/Host 模板 | completed | **首轮（整文件模板，仅 AddCustomTemplate，94/94）+ 第二轮 apply-pipeline（片段拼装 snippet assembly，111/111）均已完成（2026-09-04/05）**；模板现为 `templates/snippets/` 15 个片段 + `assembly.py` 装配驱动 |
| apply-pipeline | 工程读取→定位→渲染→写回流水线 + 文本级 sanity 检查 | pending | **本轮已做完「片段拼装地基」（不写盘）**；写盘/文本定位替换轮待启动（以片段 id/首行特征为锚） |
| cli-tests | typer/rich CLI 串联子命令 + 示例 YAML + pytest 端到端 | pending | 至少覆盖 Add 算子端到端演示；含 `generate`/`show` + `--lang` 双语 |

## 任务详情

### init-project（completed，2026-09-04）
- `pyproject.toml`（PEP 621、依赖、CLI 入口、src 布局）、`.gitignore`、`README.md`。
- `scripts/bootstrap_venv.ps1`（复用既有 `penv` 安装依赖；纯 ASCII）+ `scripts/bootstrap_venv.cmd`。
- 用户已运行引导：依赖装入工作区根 `penv`（typer 0.27.2/rich 15.0.0/jinja2 3.1.6/PyYAML 6.0.3/pytest 9.1.1/cann-ophelper editable），pip 26.2.1。
- 冒烟（venv 执行）：`pytest tests\` **40/40 通过**；`examples/add.yaml` load/往返/dump、`build_msopgen_command` 输出与官方格式一致。
- 环境坑：pytest 9.1.1 收尾 tmpdir 死链清理报 WinError 448（Windows junction/装入点安全策略），导致退出码非 0，但用例全过（以 dots 100% 为准）。

### official-patterns（completed）
- 产出 `docs/official-patterns.md`：msopgen 命令格式与参数（出处 03.02 章）、生成工程目录结构、Kernel 四段式模式、Host 侧 TilingFunc/Infer/OpDef 注册、构建与运行、官方流程与本工具插入点。
- 关键事实：msopgen 参数仅 `-i/-c/-lan/-out`；`-c` 用 `ai_core-ascend910b1` 写法；原型 JSON 的 type/format 为并行数组、param_type 取 required/optional。

### core-models（completed）
- `src/cann_ophelper/model.py`：TensorSpec/AttrSpec/OpSpec dataclass；默认值、identifier/dtype/format/soc 校验、OpSpecError（带 field_path+hint）；camel→snake（AddCustomTemplate→add_custom_template）；单侧长度 1 自动广播。
- `src/cann_ophelper/yamlio.py`：YAML↔OpSpec；缺失文件/语法错/字段非法均报带上下文错误；块式输出、字段序稳定；OpSpec.source 注入来源路径。
- `src/cann_ophelper/msopgen.py`：`build_msopgen_command`（顺序 `-i→-c→-lan→-out`，soc 自动加 `ai_core-` 前缀不重复，shell 引号保护）；`show_cloud_instructions`。
- `__init__.py` 导出 OpSpec/TensorSpec/AttrSpec/ParamType/OpSpecError。
- 自检：嵌入式 Python 已跑通 model+msopgen 冒烟，命令输出与官方完全一致；6 文件 `py_compile` 全过。

### 备注
- 用户要求：影响环境/联网的命令须先展示并经批准；pip 安装由用户亲自执行，AI 不代装。
- 具体历史与决策详见 `2026-09-04.md`。
