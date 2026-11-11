---
name: template-engine 整文件模板引擎(Element-Wise Add)
overview: 为 CANN_OpHelper 实现 Jinja2 模板引擎首个可交付轮:以官方 AddCustomTemplate 工程(S1–S5)为基准,构建整文件模板与渲染上下文,输入 OpSpec(YAML)即可渲染出逐字对齐官方范式、可独立 diff 验证的完整算子工程文件(op_kernel/*.cpp、*_tiling.h、op_host/*.cpp),并配套测试与文档。
todos:
  - id: naming-maps-helpers
    content: 新建 template 包并实现 naming.py/maps.py,补 i18n tmpl.* zh/en 键,写命名与映射单测并跑 pytest 后提交 feat(template)
    status: completed
  - id: context-builder
    content: 实现 context.py 从 OpSpec 构建渲染上下文,补 test_template_context 通过后提交,并更新当日记忆日志
    status: completed
    dependencies:
      - naming-maps-helpers
  - id: engine-templates
    content: 实现 engine.py 与三个整文件 .j2 模板(逐字对照官方 S1-S3),改 pyproject 打包数据,建 golden 快照与官方对齐测试,通过后提交并更新记忆
    status: completed
    dependencies:
      - context-builder
  - id: docs-patterns
    content: 更新 docs/official-patterns.md 增补 template-engine 基准与命名/AddConfig 暂定规则及逐字差异清单,提交 docs
    status: completed
    dependencies:
      - engine-templates
  - id: final-verify
    content: 全量 pytest 回归并核对 lint,更新 MEMORY.md 与当日记忆,整理 git 提交后推送 master
    status: completed
    dependencies:
      - docs-patterns
---

## 产品概述

为 CANN_OpHelper 增加 **template-engine(Jinja2 模板渲染层)**,作为「阶段二:在 msopgen 初始工程上填充算子实现」的地基。本轮采用**整文件模板起步**(以官方 AddCustomTemplate 工程为逐字基准),待 apply-pipeline 轮再拆解为片段拼装。

## 核心功能

- 提供包内模板资源 + Jinja2 渲染入口,输入 `OpSpec`(如 examples/add.yaml),输出三份完整文件文本:
- `op_kernel/<op>_snake.cpp`(Kernel 四段式,逐字对照官方 S1)
- `op_kernel/<op>_snake_tiling.h`(Tiling 结构体,对照官方 S2)
- `op_host/<op>_snake.cpp`(TilingFunc / InferShape / InferDataType / OpDef 注册链,对照官方 S3)
- 单点支持 **AddCustomTemplate**(Element-Wise 二元,x/y→z,float16/float + ND),渲染结果与官方样例逐字可 diff(仅保留两处有记录的有意修正)。
- 与 OpSpec 解耦的**命名/映射助手**:snake 名、kernel 类名、tiling 结构体名、dtype/format→ge::DT_*/ge::FORMAT_*、soc→`AddConfig`。
- 不引入 apply-pipeline、不碰 CLI、不新增任何 pip 依赖。

## 验收要点

- 全部新模块带单元测试;pytest 全量通过(注意本机 pytest 9.1.1 收尾 WinError 448 退出码非 0 属已知坑)。
- 渲染产物有**自包含 golden 快照**测试(不依赖大目录),并有**官方 S1–S3 实样对齐**测试(只读精确路径 3 个文件)。
- 按里程碑逐步 git commit(master,`<type>(<scope>): <subject>`),每个可独立验证功能点即提交。
- 每个里程碑同步更新 `.codebuddy\memory` 当日日志;收尾更新 MEMORY.md。

## 技术栈选择

- 复用现有技术栈:Python 3.11+、**Jinja2 3.1.6**(已在 penv)、PyYAML、pytest 9.1.1;pathlib 处理路径。
- **不新增任何 pip 依赖**(无需用户执行 pip install)。模板作为包内数据文件,随 editable 安装(`pip install -e`)在源码树中可直接被 PackageLoader 定位。
- 测试命令:`p:\Dev\CANN_Learning_Refs\penv\Scripts\python.exe -m pytest CANN_OpHelper\tests -q`。

## 实现思路

采用**分层小步推进**:`naming/maps`(纯函数、无状态)→ `context`(OpSpec→渲染上下文)→ `engine + .j2 模板`(渲染产出 dict[相对路径,文本])→ golden/官方对齐测试。上下文是「模板变量契约」的唯一来源,后续 apply-pipeline 轮把整文件模板拆成片段时只改模板与 engine,不动 context/maps。

关键设计决策:

1. **命名不虚构推导**:已检索全部官方样例(13 处 Kernel 类、62 处 TilingData 结构体、20 处 AddConfig),证据表明 msopgen 命名跨章节不一致(03 章模板工程 `KernelAdd`/`TilingDataTemplate`;开源仓路线 `<Op>TilingData`;AddConfig 全部为 `ascend910b`)。因此本轮只实现**与 03 章 AddCustomTemplate 一致的暂定规则**:`kernel_class = "Kernel" + op_type 去掉尾部 CustomTemplate/Custom`;`tiling_struct = "TilingDataTemplate"`(常量,注释说明仅对 msopgen 模板工程路线成立);soc→AddConfig 用**显式映射表** `{"ascend910b1": "ascend910b"}`,未收录的 soc 抛 i18n 错误,避免错误的通用规则。
2. **逐字对齐策略**:模板保留官方 license 头逐字内容;输出对官方仅有两处**有记录修正**——官方 S3 拼写 `intputShape`→`inputShape`、清理 S1 的  `__global__` 前导空格与 printf 双空格(官方文件本身风格不齐)。对齐测试用归一化函数(仅修官方侧这两处)断言等价。
3. **确定性**:输入/输出张量顺序按 spec 保持;渲染上下文各列表有序;`keep_trailing_newline=True`,不用 trim/lstrip,避免破坏 C++ 空行导致 diff 噪音。
4. **模板变量最小化**:变量只覆盖 op_snake/类名/结构体名/include guard/张量名 token/ge 枚举列表;四段式、注册链、TilingFunc 结构均为逐字官方代码,最大化可读与可 diff。
5. **Kernel dtype 宏不展开**:入口函数保留 `DTYPE_X/DTYPE_Y/DTYPE_Z`(官方构建系统注入宏),模板只生成宏名(`DTYPE_` + 张量首字母大写),与 S1 实样一致。

## 实施注意(Execution Notes)

- 模板文件以**逻辑名**存包内(`kernel.cpp.j2` 等),engine 负责映射到输出相对路径 `op_kernel/<op_snake>.cpp` 等,避免包内出现动态文件名。
- `pyproject.toml` 需补 `[tool.setuptools.package-data] cann_ophelper = ["template/templates/**/*.j2"]`,保证 wheel 安装也带模板。
- i18n 新增 `tmpl.*` 键(zh+en 双语),供 maps/naming 错误与引擎提示使用;不要改既有键。
- 官方参考目录跨卷且大(检索曾超时):**对齐测试只读 3 个精确文件路径**,运行期不得扫目录;golden 快照是主回归手段,官方对齐是次。
- pytest 9.1.1 收尾死链 WinError 448 导致退出码非 0 但用例全过——属已知坑,勿误判。
- 错误处理:maps/命名/引擎不合法输入一律抛 `OpSpecError`(带 field_path/hint,i18n 文案),保持与 model/yamlio 一致的错误契约。

## 架构设计

分层数据流:`OpSpec(examples/add.yaml)` → `naming.py + maps.py`(命名 token / ge 枚举 / soc)→ `context.py: build_render_context(spec) -> dict` → `engine.py: render(spec) -> {relpath: text}`(Jinja2 PackageLoader)→ 测试层(golden 快照 / 官方 S1–S3 对齐)。各层无状态、可单测;engine 只依赖 context,context 只依赖 naming/maps/model,杜绝循环依赖。性能无热点(模板规模小),无需缓存;此轮不做日志(错误即抛,遵循现有 i18n 契约)。

## 目录结构

```
CANN_OpHelper/
├── src/cann_ophelper/
│   ├── i18n.py                                  # [MODIFY] 新增 tmpl.* 键(zh/en)
│   └── template/                                # [NEW] template-engine 包
│       ├── __init__.py                          # [NEW] 导出 TemplateEngine/build_render_context 等
│       ├── naming.py                            # [NEW] 纯函数:op_snake/kernel_class/tiling_struct/dtype_alias/macro_alias/tiling_guard
│       ├── maps.py                              # [NEW] ge_dtype/ge_format/opdef_soc 映射表+校验(抛 OpSpecError)
│       ├── context.py                           # [NEW] build_render_context(spec)->dict(命名+张量 token+ge 枚举+host 注册字段)
│       ├── engine.py                            # [NEW] TemplateEngine:PackageLoader(Jinja2)、render(spec)->dict[relpath,text]
│       └── templates/                           # [NEW] 包内模板资源(逻辑名)
│           ├── op_kernel/kernel.cpp.j2          # [NEW] 整文件:S1 逐字模板(变量:类名/入口名/tiling include/张量 token)
│           ├── op_kernel/tiling.h.j2            # [NEW] 整文件:S2 逐字模板(变量:guard/结构体名)
│           └── op_host/host.cpp.j2              # [NEW] 整文件:S3 逐字模板(变量:注册链/ge 枚举/AddConfig)
├── pyproject.toml                               # [MODIFY] package-data 声明 *.j2
├── tests/
│   ├── test_template_naming.py                  # [NEW] 命名规则单测(AddCustomTemplate→KernelAdd/TilingDataTemplate/guard)
│   ├── test_template_maps.py                    # [NEW] ge 枚举映射+未知 dtype/format/soc 抛错(i18n zh 断言)
│   ├── test_template_context.py                 # [NEW] 上下文键完备性/顺序确定性/ge 列表渲染串
│   ├── test_template_engine.py                  # [NEW] 3 文件渲染、golden 快照、官方 S1–S3 实样对齐(归一化)
│   └── fixtures/golden/                         # [NEW] 自包含快照:add_custom_template.cpp / _tiling.h / op_host cpp
├── docs/official-patterns.md                    # [MODIFY] 新增 §7 template-engine 基准(命名/AddConfig 暂定规则/逐字差异清单)
└── .codebuddy/memory/2026-09-04.md, MEMORY.md   # [MODIFY] 里程碑记忆
```

## 关键接口契约(简)

- `naming.op_snake(op_type)` 委托 `model.camel_to_snake`;`kernel_class(op_type)` 返回 `Kernel`+去尾缀核心名;`tiling_struct()` 返回常量 `TilingDataTemplate`;`dtype_alias(name)`→`dtypeX`;`macro_alias(name)`→`DTYPE_X`;`tiling_guard(op_snake)`→`ADD_CUSTOM_TEMPLATE_TILING_H`。
- `maps.ge_dtype("float")`→`ge::DT_FLOAT`;`ge_format("ND")`→`ge::FORMAT_ND`;`opdef_soc("ascend910b1")`→`ascend910b`;未知输入抛 `OpSpecError`(i18n `tmpl.*` 键)。
- `context.build_render_context(spec: OpSpec) -> dict`:含 `op_type/op_snake/kernel_class/tiling_struct/tiling_guard/inputs/outputs`(每张量含 `name/cap/dtype_alias/macro_alias/ge_types(逗号串)/ge_formats(逗号串)`)、host 注册段所需 `op_type/AddConfig`。
- `engine.TemplateEngine.render(spec) -> dict[str, str]`:键为 `op_kernel/<snake>.cpp`、`op_kernel/<snake>_tiling.h`、`op_host/<snake>.cpp`,值为整文件文本(尾随换行保留)。