---
name: dehydrate-task6-verify
overview: 完成 ComfyDL_UI 脱水任务 6：清理 ~illow 残骸与记录 IPython 依赖，按脱水清单 §7 六项标准执行最终验证并更新 docs/dehydrate_manifest.md，最后提交 verify 检查点。
todos:
  - id: cleanup-and-record-deps
    content: 完成两样清理：删除 ~illow-12.3.0.dist-info 残骸，宿主与插件 requirements.txt 记录 IPython/matplotlib-inline/matplotlib，验证 Pillow 与 pip check 后提交 chore
    status: completed
  - id: verify-startup
    content: 正式复跑 quick-test 留档确认 §7-1/2/4（106 节点、无 IMPORT FAILED），并用 Measure-Command 计时取最短、尝试 original 对照
    status: completed
  - id: verify-e2e-workflow
    content: §7-3 工作流验证：起 server、PIL 生成 input 测试图、经 /object_info 选 ComfyDL IMAGE 节点、POST /prompt 轮询 /history 断言成功与产物落盘
    status: completed
  - id: verify-revertible
    content: §7-6 回捞校验：运行 compare_dups.py 统计落档，抽样删除项做 temp 目录 sha256 dry-run 证明可原样回捞
    status: completed
  - id: finalize-docs-and-commit
    content: 更新 docs/dehydrate_manifest.md（§7 结果、修复记录、检查点[6]），核对 git 状态后提交 docs
    status: completed
    dependencies:
      - cleanup-and-record-deps
      - verify-startup
      - verify-e2e-workflow
      - verify-revertible
---

## 用户需求

完成脱水项目收尾：先做两样环境清理，再执行任务 6（脱水清单 §8 检查点 `[6] verify 最终验证 + 文档更新`）。

## 两样清理

1. **`~illow` 残骸清理**：删除 `penv/Lib/site-packages/~illow-12.3.0.dist-info/`（Pillow 安装中断残留，pip 每次报 `Ignoring invalid distribution`），并验证 Pillow 仍可用、`pip check` 干净。
2. **IPython 依赖记录**：将本次为修复 ComfyDL 插件 d2lcore 而降级的 `IPython`（及 `matplotlib-inline`、`matplotlib`）记录到宿主 `requirements.txt` 与插件 `requirements.txt`，避免未来重建环境后 ComfyDL 插件退回 80/106 节点。

## 任务 6：最终验证 + 文档更新（manifest §7 六项标准）

1. 启动成功，日志无 ModuleNotFoundError；
2. ComfyDL 约 106 节点完整注册（14 分类）；
3. 基础工作流 LoadImage → ComfyDL 节点 → PreviewImage 真实可执行（HTTP prompt + 产物落盘）；
4. 启动日志无 IMPORT FAILED；
5. 记录脱水后启动耗时（无同机前置基线，需如实说明）；
6. 回捞可逆：`compare_dups.py` 校验 original 完好 + 抽样删除项 dry-run 可原样回捞。

## 文档与提交

- 更新 `docs/dehydrate_manifest.md`：§7 六项结果落表、新增"脱水后实测修复记录"（GBK reconfigure、插件 sys.path + IPython，附 commit）、§8 检查点 `[6] verify` 状态。
- 按仓库提交规范分两次提交（清理+依赖记录 / 文档收口）。`custom_nodes/ComfyDL` 为 gitignore 外独立仓库，其提交由用户另行决定，不越权。

## 技术方案

### 环境清理与依赖记录

- **删除** `ComfyDL_UI\penv\Lib\site-packages\~illow-12.3.0.dist-info\`（仅此一目录；正常 `pillow-12.3.0.dist-info` 保留）。删除后验证：`python -c "import PIL; print(PIL.__version__)"` 与 `pip check` 无错误。
- **宿主 `ComfyDL_UI/requirements.txt`**：新增小节"ComfyDL 教学插件（custom_nodes/ComfyDL，独立仓库）运行依赖"，加入 `matplotlib` / `IPython` / `matplotlib-inline` 三个可安装条目并注明用途（d2lcore + 可视化），确保重建环境后 106 节点不降级。
- **插件 `custom_nodes/ComfyDL/requirements.txt`**：在现有 `matplotlib` 行下补 `IPython`、`matplotlib-inline`，保持插件自洽。
- 提交：`chore(dehydrate): 清理 ~illow 残骸并记录 ComfyDL 运行依赖`。

### §7 验证执行方法

- **§7-1/2/4**：正式复跑 `penv\Scripts\python.exe main.py --cpu --quick-test-for-ci`（默认无 `-X utf8` 模式，验证 GBK 修复），日志留档，确认 `Loaded 106 deep learning nodes`、无 IMPORT FAILED/Warning。
- **§7-5 启动耗时**：PowerShell `Measure-Command` 对同一命令冷启动计时多轮取最短，记录"脱水后绝对启动耗时"；尝试用同一 penv 跑 `ComfyUI-original\main.py --quick-test-for-ci` 作对照，若因依赖剔除（torchvision/transformers 等）无法跑通则如实记录"无同机前置基线"的结论，不伪造对照数据。
- **§7-3 工作流 e2e**：后台启动 `main.py --cpu`（端口 8188）→ 用 PIL 生成测试图入 `input/` → `GET /object_info` 动态挑选一个**无外部权重、纯离线可跑**的 ComfyDL IMAGE 链节点（Image Tools / Visualization 类优先，其确切 class name 以 object_info 实测为准）→ 构造 API 格式 prompt `LoadImage → ComfyDL 节点 → SaveImage/PreviewImage` → `POST /prompt` 轮询 `/history/<id>` 断言 `status.success == true` 且 output 目录有产物 → 终止 server。一次性脚本放工作区 `.codebuddy/scripts/`（不入 git）。
- **§7-6 回捞校验**：运行 `p:/Dev/ComfyUI_Refs/.codebuddy/scripts/compare_dups.py`（只读 original），stdout 重定向落档，统计 `checked/diffs/missing`（脱水产生的 MISSING/DIFF 属预期回捞清单，非失败）；再抽查代表删除项（如 `comfy/ldm/__init__.py`）复制至临时目录做 sha256 与原文件比对后清理，证明可原样回捞。

### 文档更新

- `docs/dehydrate_manifest.md`：§7 每项验证结果落记录；新增"脱水后实测修复记录"小节（3 项修复 + commit 引用）；§8 检查点表补 `[6] verify` 提交哈希与日期；头部状态行同步。
- 提交：`docs(dehydrate): 记录任务6最终验证结果`。