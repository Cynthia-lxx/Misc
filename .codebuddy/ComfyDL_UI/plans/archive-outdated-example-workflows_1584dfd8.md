---
name: archive-outdated-example-workflows
overview: 把 comfydl/example_workflows/ 下 9 个已过时的示例工作流 JSON 全部 git mv 到既有 _archived/ 归档位，清理 0 字节自动产物与失效生成脚本 _gen_full_test.py，并移除两份 README 中对应的旧截图小节（assets 图片一律保留不删）。不涉及节点增删改，不做新工作流。
todos:
  - id: archive-and-purge
    content: 用 git mv 归档 9 个过时工作流至 example_workflows/_archived/，并删除失效脚本与 0 字节空产物
    status: completed
  - id: clean-readmes
    content: 改写 README.md 与 README_zh.md：移除 6 个工作流截图小节、补归档说明、同步目录树注释
    status: completed
  - id: smoke-and-check
    content: 运行冒烟测试确认零 FAIL，并核对归档后目录结构与模板路由
    status: completed
    dependencies:
      - archive-and-purge
  - id: verify-references
    content: 用 [subagent:code-explorer] 全仓复查残留失效引用与 gitignore 规则
    status: completed
    dependencies:
      - archive-and-purge
      - clean-readmes
  - id: commit-and-push
    content: 提交 comfydl submodule 并推送 main，更新主仓 gitlink 后推送 master
    status: completed
    dependencies:
      - smoke-and-check
      - verify-references
---

## 用户需求

判定 `comfydl/example_workflows/` 下的示例工作流已全部过时，本次**立即清理**：工作流归档、失效附属物删除、README 中对应的过时截图章节一并收敛；"真能跑"的新工作流等 reform 节点成型后再另立计划，本次完全不碰。

## 处理对象（现状）

`ComfyDL_UI/comfydl/example_workflows/` 顶层现有 9 个工作流 JSON、1 个失效生成脚本、1 个 0 字节自动产物，以及既有的归档位 `_archived/`（内含 3 个更早归档的 JSON）。

## 核心动作

1. **归档**：9 个工作流 JSON 全部 `git mv` 移入既有归档位 `example_workflows/_archived/`，保留可回溯，同时让前端「模板」列表立刻变干净。
2. **删除**：`_gen_full_test.py`（路径写死旧根目录且 `import ComfyDL`，已跑不通）、`ComfyDL_AllNodes_Generated.json`（0 字节空产物）。
3. **文档收敛**：`README.md` / `README_zh.md` 中与这些工作流对应的截图章节移除，仅保留与工作流无关的单节点演示小节，并补一句指向归档位的状态说明；目录树注释同步。
4. **验证**：全仓引用核查 + 冒烟测试零 FAIL + 归档结构核对。

## 范围与边界（明确不做）

- 不新建、不修改任何工作流 JSON；不重命名 `_archived/` 内既有文件。
- 不删任何 `assets/example_*.png`（用户亲截资产），只取消 README 中的引用。
- 不动任何节点代码与节点计数（三套口径 109/20、248/38、141/26 全部不变）。
- `docs/comfydl-builtin.md` 引用 `Image Processing Chain.json` 的 E2E 证据段落**保持原样**（用户明确未勾选），路径失效风险已在下方记录。
- 不改动 `ComfyUI-original/`。

## 视觉/展示效果

归档后前端「模板」面板不再出现这批过时工作流；README 首屏不再是过时的旧工作流截图，而是简短的状态说明，为 reform 后的新工作流预留位置。

## 任务性质与总体策略

纯仓库整理：**0 行代码改动、0 个节点增删改**。因此节点计数与四份说明文件的计数部分全部不变——本次只有两份 README 的展示性内容变化。处置分三类：**归档（9 个 JSON）/ 删除（2 个文件）/ 改写文档（2 份 README）**，另有 1 处刻意不动。

## 关键技术依据（已核实）

1. **归档即"下架"的机制**：`app/custom_node_manager.py:104` 发现模板用 `glob(os.path.join(folder, f"*/example_workflows/*.json"))`，**非递归**。移入 `_archived/` 子目录后自动从 `/workflow_templates` 结果消失——这正是"清掉过时噪音"的实现路径，无需改路由代码。
2. **空目录不报错**：`:126` 仅以 `os.path.exists(workflows_dir)` 判断是否注册静态路由；`example_workflows/` 顶层即便只剩 `_archived/` 子目录，也只是该节点对应的模板列表为空（`setdefault` 后无项），**不会抛异常**，无需额外兜底。
3. **脚本链已完全断裂**：`_gen_full_test.py:25` 写死旧根路径注释 `f:/Dev/ComfyDL_Refs`，`:29` `import ComfyDL` 依赖顶层包（内置化后已不存在），文档串 `:11` 提到的配套 `_validate_wf.py` 全仓不存在 → 删除无副作用。
4. **资产保护**：`assets/example_*.png`（7 张）为既有约定中的"用户亲截用例截图，不得当临时文件清理"，只取消引用、全部保留，供 reform 后复用/替换。
5. **git 上下文**：仓库根 `P:\Dev\ComfyUI_Refs\ComfyDL_UI\`，`comfydl/ `为 submodule（remote `Cynthia-lxx/ComfyDL.git`，main）。命令须带 `-c safe.directory=P:/Dev/ComfyUI_Refs/ComfyDL_UI`，**不改全局 git config**；移动用 `git mv`（保留历史），删除用 `git rm`。

## 具体做法

### A. 工作流归档（9 个，`git mv`）

`ComfyDL All-Nodes Demo` / `Fashion-MNIST Data Explorer` / `Fashion-MNIST Explore` / `Image Processing Chain` / `Kaggle-Style Data Pipeline` / `LeNet Inference Demo` / `Linear Regression Demo` / `Object Detection Utilities` / `RNN Language Model Demo`（均 `.json`）→ `example_workflows/_archived/`。**保持原文件名不重命名**（`_archived/` 内既有 3 个为小写 kebab-case，但统一改名会破坏用户识别与回溯习惯，不值得）。归档后 `_archived/` 共 12 个 JSON。

### B. 删除（2 个）

`git rm example_workflows/ComfyDL_AllNodes_Generated.json`（0 字节）与 `git rm example_workflows/_gen_full_test.py`；同时检查 `.gitignore` 是否有针对该产物的条目，若有则一并清除，避免留下空规则。

### C. README 改写（`README.md:15-52`、`README_zh.md:14-51`）

- **移除 6 个"工作流截图"小节**：RNN Language Model / Linear Regression / LeNet Inference / Fashion-MNIST Exploration / Iteration Timer Benchmark / Lots of Nodes。
- **保留 `The Mysterious "?"`**：该节描述的是单个节点的开关（与工作流过时无关）。
- **节首补一行状态说明**（英中对应），交代旧工作流已归档至 `example_workflows/_archived/`、新版将随节点重构重做，避免"图没了却没交代"。
- **目录树注释同步**：`README.md:118` 与 `README_zh.md:116` 的 `example_workflows/  # Sample workflow JSONs` 改为标注"已归档"。
- 判断留白：`Lots of Nodes` 对应被归档的 All-Nodes Demo 故一并移除；若希望保留，回退仅需删一行对应小节。

### D. 刻意不动（1 处）

`docs/comfydl-builtin.md:146-151` 两处以 `example_workflows/Image Processing Chain.json` 为 E2E 证据的段落保持原样（它是"核心图像节点替换 `CdlImage*` 后 `/prompt` 跑通"的有效技术记录）。代价：归档后该路径失效；如需修正，只需把 `example_workflows/` 改成 `example_workflows/_archived/`（1 行补丁），留待用户决定。

## 验证方案

- **冒烟**：`penv\Scripts\python.exe cdl_smoke_tests\run_smoke_test.py`，基线 226 PASS / 22 SKIP / 0 FAIL；本次不动节点 → 预期逐项一致，用于确认零回归。
- **结构核对**：顶层仅剩 `_archived/`；`_archived/` 内 12 个 JSON；无残留失效脚本与空产物。
- **引用核查**：全仓（排除 `penv/` 与只读的 `ComfyUI-original/`）不存在指向已归档 JSON 的失效路径（此前一轮工作区级 grep 曾超时截断，需用 [subagent:code-explorer] 做一次定向、可收敛的复查）。
- **模板路由（推荐）**：`penv\Scripts\python.exe main.py --cpu` 启动后请求 `/workflow_templates`，确认正常返回且日志无 `example_workflows` 相关异常。
- **文档一致性**：`comfydl/_update_readme.py --check` 应仍全绿（不涉及节点计数）。

## 提交与同步

- submodule `comfydl/`：`chore(workflows): archive outdated example workflows` → `git pull origin main --rebase` → push main。
- 主仓 `ComfyDL_UI/`：`git add comfydl` 更新 gitlink → push master。
- 推送前均先 rebase；禁止 `--force`；不提交任何二进制/敏感文件。

## 目录结构改动清单

```
ComfyDL_UI/
├── comfydl/
│   ├── example_workflows/
│   │   ├── _archived/                            # [MODIFY] 既有归档位
│   │   │   ├── bbox_conversion_pipeline.json     # [KEEP]
│   │   │   ├── bleu_score_eval.json              # [KEEP]
│   │   │   ├── nlp_vocab_pipeline.json           # [KEEP]
│   │   │   └── （新移入 9 个 JSON，保持原名）      # [MOVE]
│   │   ├── ComfyDL All-Nodes Demo.json           # [MOVE] → _archived/
│   │   ├── Fashion-MNIST Data Explorer.json      # [MOVE] → _archived/
│   │   ├── Fashion-MNIST Explore.json            # [MOVE] → _archived/
│   │   ├── Image Processing Chain.json           # [MOVE] → _archived/
│   │   ├── Kaggle-Style Data Pipeline.json       # [MOVE] → _archived/
│   │   ├── LeNet Inference Demo.json             # [MOVE] → _archived/
│   │   ├── Linear Regression Demo.json           # [MOVE] → _archived/
│   │   ├── Object Detection Utilities.json       # [MOVE] → _archived/
│   │   ├── RNN Language Model Demo.json          # [MOVE] → _archived/
│   │   ├── ComfyDL_AllNodes_Generated.json       # [DELETE] 0 字节自动产物
│   │   └── _gen_full_test.py                     # [DELETE] 路径失效、依赖已不存在的顶层包
│   ├── README.md                                 # [MODIFY] 移除 6 个工作流截图小节 + 目录树注释
│   ├── README_zh.md                              # [MODIFY] 同上（中文对应）
│   ├── FUNCTIONS.md                              # [KEEP] 无工作流引用，节点计数不变
│   ├── FUNCTIONS_zh.md                           # [KEEP] 同上
│   ├── _update_readme.py                         # [KEEP] 同步脚本，本次仅用于 --check 验证
│   └── assets/example_*.png                      # [KEEP] 7 张亲截资产，仅取消 README 引用
└── docs/comfydl-builtin.md                       # [KEEP] 用户明确不动（路径将失效，可后续 1 行修正）
```

## Agent Extensions

### SubAgent

- **code-explorer**
- Purpose: 在归档/删除/文档改写完成后，做一次定向且可收敛的全仓引用复查——确认没有任何文件仍指向 9 个被移动的 JSON、被删除的 `_gen_full_test.py` 及其提到的 `_validate_wf.py`，并核对 `.gitignore` 是否有残留规则。（此前一轮工作区级 grep 因目录过大曾超时截断，需要一次有计划、限定目录与文件类型的复查来收敛结论。）
- Expected outcome: 一份明确的引用清单：要么确认零残留失效引用，要么精确列出仍需修正的文件路径与行号，作为提交前的最后一道关卡。