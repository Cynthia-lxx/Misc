---
name: comfydl-ui-rebrand-frontend-injection
overview: 把 ComfyDL_UI 统一为 v0.3.1 品牌（含 GPL-3.0 版权），并新增一个「启动前动态注入」的前端补丁层：默认开启 Nodes 2.0、默认英文、改写设置→关于面板自述、把 TENSOR 插槽配色的手工静态篡改收编进该机制，同时把我们新增的 UI 文案与节点文案注册进中英语言包。
todos:
  - id: unify-identity
    content: 统一项目名与版本为 ComfyDL_UI v0.3.1（pyproject、comfyui_version、启动日志、docs 口径）并补 GPL-3.0 版权声明
    status: completed
  - id: frontend-patch-layer
    content: 新增 app/frontend_patch.py 幂等补丁层，并在 server.py 前端根解析后、服务启动前接入
    status: completed
  - id: patch-defaults-tensor
    content: 在补丁层实现设置默认值（Nodes 2.0 开启、语言英文）与 TENSOR 配色收编，并清理文档中的手工重放告警
    status: completed
    dependencies:
      - frontend-patch-layer
  - id: locales-register
    content: 新增 locales/en|zh 语言包与生成器，并扩展服务端 /i18n 合并仓库级 locales
    status: completed
  - id: about-and-readme
    content: 补丁 About 面板注入 ComfyDL_UI 自述分区与上游独立分区，并改写根 README 双语文档
    status: completed
    dependencies:
      - frontend-patch-layer
      - locales-register
  - id: comfydl-gpl-and-install
    content: comfydl 转 GPL-3.0，README 安装章节改为 GUI/原版两个选项，并同步四份说明文件
    status: completed
  - id: verify-all
    content: 跑 comfydl 冒烟测试 0 FAIL，并启动实例用 [skill:playwright-cli] 验收设置默认值、About 面板与 TENSOR 配色
    status: completed
    dependencies:
      - patch-defaults-tensor
      - about-and-readme
      - comfydl-gpl-and-install
---

## 产品概述

对 ComfyDL_UI（ComfyUI 的「深度学习特化」脱水 fork）做一轮**小规模、名义性**的品牌与版权统一改造，并把目前散落的手工前端篡改收编为可复现的启动期补丁层，使新克隆的用户「装依赖 + 启动」即可得到与本机完全一致的界面效果。

## 核心功能

1. **统一身份**：项目名统一为 `ComfyDL_UI`、版本统一为 `v0.3.1`，覆盖代码、配置、文档与启动日志；`comfydl` 子模块名称保持 `ComfyDL` 不变。全程按 GPL-3.0 规范补版权保护（保留上游作者声明，独立列出本项目版权）。
2. **ComfyDL 插件文档**：说明 ComfyDL 存在自己的 GUI 版本（即 ComfyDL_UI）；`Installation` 章节由「四步安装」改为两个选择：① 下载 GUI 版本 ② 原有四步手动安装。
3. **前端设置默认值**：设置面板中 `Nodes 2.0` 默认「开启」、语言默认「English」。
4. **应用内自述（设置 → About）**：声明本项目是为深度学习专门改装的性质；保留原版 ComfyUI 的 stars、官网、profile 等链接，但**独立成分区**，与 ComfyDL_UI 开发者信息分开；同时声明这不是完整的原版 ComfyUI，想用原版请从官方仓库下载，并给出官方仓库链接。项目根自述文档同口径改写。
5. **多语言**：把「我们加入的部分」（新增的 UI 文案 + 我们加入的节点显示名/描述）做成多语言，并注册进原有语言包体系。

## 视觉与交互效果

- 启动后进入设置面板：`Nodes 2.0` 处于开启态、语言项为 English；连线面板中 `TENSOR` 插槽仍为柠檬绿 `#C6FF00`。
- 设置面板底部的 About 面板出现 ComfyDL_UI 自述区块：上方是本项目定位与开发者链接，下方是**明确标注为「上游原版 ComfyUI」**的独立分区（stars/官网/profile 等等原链接照旧可用），并含「非完整原版、请走官方仓库」的说明。
- 中文语言下，我们新增的界面文案与节点显示名显示中文；英文下显示英文。以上效果不依赖任何手工改包操作，克隆后首次启动即成立。

## 技术栈选择

- 运行时与依赖：沿用现有 Embedded Python 3.14 venv（`ComfyDL_UI/penv`），**不引入任何新依赖**。
- 宿主：现有 ComfyUI 脱水 fork（`main.py` / `server.py` / `nodes.py` / `app/*`）与既有 SQLite+JSON 用户配置链路（`app/app_settings.py` 的 `/settings`）。
- 前端：第三方 pip 包 `comfyui-frontend-package==1.51.9`（`penv/Lib/site-packages/comfyui_frontend_package/static/`，**不在版本控制内**），通过内容标记定位的幂等补丁层在启动期改写，不 fork 前端。
- 语言包：仓库级 `locales/<lang>/{main,nodeDefs,commands,settings}.json` + 服务端 `/i18n` 合并（复用 `app/custom_node_manager.py` 既有 `build_translations()` / `merge_json_recursive` 机制）。
- 方言与安全：所有新增日志纯 ASCII（`main.py` 已对 stdout/stderr 做 UTF-8 reconfigure，避免 GBK 控制台崩溃）。

## 实现方案

### 总体策略

把「品牌/版权统一」做成纯文本与配置的定点改动；把「前端效果」做成一个**仓库内受版本控制的启动期补丁层**（`app/frontend_patch.py`），在前端根路径解析之后、静态资源开始被服务之前执行，按**内容标记**（不是 hash 文件名）定位目标 chunk 并就地改写，天然幂等、可重复执行，前端升级后自动重新应用。

### 关键决策与权衡

1. **为什么不改前端包源码**：`comfyui-frontend-package` 是 pip 依赖，升级即丢失；补丁层随仓库走，升级后自动重放（这是把 `docs/reform-step1-activation-tensor.md` 里那次手工篡改收编的直接动机）。
2. **为什么用「直接改写 about 相关文件」而不是 DOM 注入**：用户明确优先直接改文件；`AboutPanel-*.js` 与 `useExternalLink-*.js` 无扩展点，改写 chunk 是唯一能保留原面板结构（版本号、系统统计）的方案；DOM 注入会被前端升级与渲染时序打破。
3. **About 面板文案的语言来源**：面板自身标签是硬编码非 i18n 的，因此新增区块的文案统一从 `locales/<lang>/main.json` 的专用命名空间读取，语言取当前 `Comfy.Locale`（走既有 `/settings` 接口）。这样同一份 JSON 既服务 vue-i18n 合并（原有语言包注册），又服务 About 区块渲染，**单一数据源**，避免重复维护。
4. **设置默认值**：改 `GraphView-*.js` 中设置定义列表的 `defaultValue`（`Comfy.Locale`、`Comfy.VueNodes.Enabled`）。已存在的 `user/default/comfy.settings.json` 与 `/settings` 持久化优先于默认值，属正确行为（用户显式改过就尊重用户），故「开箱」判据以「无该键的用户/全新 user 目录」为准。
5. **语言包注册位置**：`comfydl/` 位于仓库根、不在 `custom_nodes/`，现有 `/i18n` 扫不到我们；在 `app/custom_node_manager.py` 中以**追加一个来源目录**的方式（不改既有 custom_nodes 扫描语义）纳入仓库级 `locales/`，让 `build_translations()` 一并返回，前端既有的 `mergeCustomNodesI18n` 会自动合并。
6. **节点多语言规模控制**：177 个节点（109 ComfyDL + 68 core）的 `en` 侧从注册表生成，`zh` 侧以 `comfydl/FUNCTIONS_zh.md` 的中文口径为创作来源、缺项回退英文名；用一次性生成脚本产出并入库，避免人工维护漂移。

### 性能与可靠性

- 补丁层只做「读 → 字符串定位 → 写」，文件量级为个位数 chunk（单文件约 1.6 MB），**在启动期一次性完成**，不进入任何热路径；用「已含标记即跳过」的短路判断保证重复启动零成本。
- 每个补丁独立 `try/except`：任一处失败只打印告警并继续启动（与 `nodes.py` 对 submodule 缺失的处理风格一致），绝不因前端格式变动导致服务起不来。
- 语言包为静态 JSON + `@lru_cache(maxsize=1)` 的既有聚合，无额外运行时开销。

### 避免技术债

复用既有模式：补丁层挂在 `server.py` 已有的前端根解析分支上；i18n 复用 `merge_json_recursive`；文档沿用 `docs/*.md` 记录惯例与 `_update_*.py` 生成器惯例（同 `comfydl/_update_nodes.py`、`_update_readme.py`）。不为本任务引入抽象层、配置框架或新目录体系。

## 执行注意事项

- **注入时机**：必须在前端根确定后、`prompt_server.start()` 之前；`--front-end-root` 覆盖默认路径时同样生效，因此补丁目标取**解析后的 web root**，不写死 `penv/...`。
- **定位标记要跨越 hash 变化**：以 `id:`Comfy.Locale``、`node_slot:{`、`AboutPanel `内既有链接数组等**内容特征**为锚点；`node_slot `必须跳过调色板合并表达式（`node_slot:{...t.colors.node_slot,…}`），沿用已验证的 6 表判定逻辑。
- **爆炸半径**：不改 `comfy/` 框架层与节点注册链；不动 `ComfyUI-original/`；不改 `nodes.py` 的 ComfyDL 注册逻辑；前端补丁只碰列出的目标 chunk。
- **日志**：新增日志无 emoji、无中文字面量混入错误路径；失败信息给出「chunk 名 + 期望标记未找到」的可操作提示。
- **文档同步**：`comfydl` 改动后必须同步 `README.md`/`README_zh.md`/`FUNCTIONS.md`/`FUNCTIONS_zh.md` 四份（本次无节点增删，节点数量口径 177/32 保持不变），并在根 `docs/` 记录本次品牌化与补丁层。

## 架构设计

```mermaid
graph TD
  A[main.py 启动] --> B[server.py 解析前端根<br/>--front-end-root 或 FrontendManager.init_frontend]
  B --> C[app/frontend_patch.py<br/>apply_frontend_patches web_root]
  C --> C1[默认设置补丁<br/>GraphView-*.js defaultValue]
  C --> C2[About 面板补丁<br/>AboutPanel-*.js 自述分区]
  C --> C3[TENSOR 配色补丁<br/>settingStore-*.js 6 主题表]
  C --> D[静态路由注册 / 服务启动]
  E[仓库级 locales/en,zh] --> F[CustomNodeManager.build_translations]
  F --> G[/i18n 合并 → mergeCustomNodesI18n]
  G --> C2
```

## 目录结构

```
ComfyDL_UI/
├── pyproject.toml                 # [MODIFY] name→ComfyDL_UI，version→0.3.1，license 与 URL 口径统一
├── comfyui_version.py             # [MODIFY] __version__ → "0.3.1"（与 pyproject 保持一致）
├── main.py                        # [MODIFY] 启动日志 "ComfyUI version" → ComfyDL_UI 口径，接入补丁层调用
├── server.py                      # [MODIFY] 在前端根解析处、静态路由/服务启动前调用 apply_frontend_patches(web_root)
├── LICENSE                        # [MODIFY] 保留 GPL-3.0 全文，追加本项目版权声明（与上游声明分开列）
├── README.md                      # [MODIFY] 改写为 ComfyDL_UI 自述；原版 ComfyUI 链接/徽章独立分区；非完整原版声明 + 官方仓库链接
├── README_zh.md                   # [NEW] 中文自述，与 README.md 同口径（项目文档双语惯例）
├── app/
│   ├── frontend_patch.py          # [NEW] 幂等前端补丁层：apply_frontend_patches(web_root)，内含默认设置/About/TENSOR 三个子补丁与统一告警
│   └── custom_node_manager.py     # [MODIFY] build_translations() 追加仓库级 locales/ 来源，使 /i18n 合并我们的语言包
├── locales/
│   ├── en/main.json               # [NEW] 我们新增 UI 文案（About 自述等）的英文表，命名空间隔离
│   ├── en/nodeDefs.json           # [NEW] 我们加入的节点显示名/描述英文表（由注册表生成）
│   ├── zh/main.json               # [NEW] 同上中文表
│   └── zh/nodeDefs.json           # [NEW] 节点中文显示名/描述（源自 FUNCTIONS_zh.md 口径，缺项回退英文）
├── cdl_smoke_tests/
│   └── gen_locales.py             # [NEW] 语言包生成器：en 取注册表、zh 取中文口径，可重复执行、结果入库
├── docs/
│   ├── branding-and-frontend-patch.md   # [NEW] 本次品牌化/版权/补丁层/语言包注册的双语记录（含回退与验证）
│   └── reform-step1-activation-tensor.md # [MODIFY] §3.1/§6 去掉「需手工重放」告警，改为指向补丁层
└── comfydl/                       # 子模块（名称保持 ComfyDL）
    ├── LICENSE                    # [MODIFY] MIT → GPL-3.0
    ├── pyproject.toml             # [MODIFY] license MIT → GPL-3.0（name 保持 ComfyDL）
    ├── README.md                  # [MODIFY] 增加「有 GUI 版本 ComfyDL_UI」说明；Installation 改为 GUI / 原版四步两个选项
    ├── README_zh.md               # [MODIFY] 同上中文口径
    ├── FUNCTIONS.md               # [MODIFY] 同步许可证/GUI 口径（节点数量 177/32 不变）
    └── FUNCTIONS_zh.md            # [MODIFY] 同上
```

## 关键代码结构

```python
# app/frontend_patch.py —— 补丁层对外只暴露一个入口，失败不阻塞启动
def apply_frontend_patches(web_root: str) -> None:
    """Apply all ComfyDL_UI frontend patches to the resolved web root.

    Idempotent: each patch locates its target by a content marker (never by the
    hashed asset filename) and short-circuits when the marker is already present.
    A failure in one patch logs an ASCII warning and never aborts startup.
    """

def patch_setting_defaults(assets: Path) -> None: ...   # Comfy.Locale=en, Comfy.VueNodes.Enabled=true
def patch_about_panel(assets: Path) -> None: ...        # ComfyDL_UI 自述分区 + 上游独立分区
def patch_tensor_slot_color(assets: Path) -> None: ...  # 收编原手工 node_slot 补丁（6 张主题表）
```

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose: 在实施前做一次全仓定位核查：所有残留的名称/版本字面量（`0.34.0`、`ComfyUI` 品牌位、`v0.2.0-dehydrated` 口径）、`server.py` 前端根解析与 `start_all()` 的精确注入行号、以及 `comfydl/FUNCTIONS_zh.md` 中可作为节点中文名来源的段落结构。
- Expected outcome: 产出一份「文件 + 行号 + 现状值」的改动清单，确保计划中的落点无遗漏、无虚构路径，并让补丁锚点与文档改写点可直接开工。

### Skill

- **playwright-cli**
- Purpose: 在启动后的实例上做前端验收：打开设置面板确认 `Nodes 2.0` 默认开启、语言为 English；翻到 About 面板截图核对 ComfyDL_UI 自述分区与上游独立分区；在画布上确认 `TENSOR` 插槽仍为 `#C6FF00`。
- Expected outcome: 得到可存档的界面截图与断言结果，证明补丁在真实浏览器中生效且未破坏原面板结构。