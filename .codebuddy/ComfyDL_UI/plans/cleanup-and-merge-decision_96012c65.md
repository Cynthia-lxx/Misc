---
name: cleanup-and-merge-decision
overview: 清理全部遗留项（删除 .tmp_bal2.py、复核两盘工作区），并将 experimental 分支（crash site M2 可解释性/现场导出/惰性捕获/M3 断点续训，共 4 个提交）合回 master 后同步 F 盘。
todos:
  - id: delete-tmp-file
    content: 删除 P:\Dev\ComfyUI_Refs\ComfyDL_UI\.tmp_bal2.py 并确认 git status 干净
    status: completed
  - id: merge-ff
    content: P 盘 switch master 并 fast-forward 合并 experimental，推送 origin/master
    status: completed
    dependencies:
      - delete-tmp-file
  - id: f-drive-sync
    content: F 盘 fetch + 切 master + reset 对齐 + submodule update，复核 status 干净
    status: completed
    dependencies:
      - merge-ff
  - id: final-verify
    content: 两盘 git log/status 终检，汇报合并结果与 experimental 分支保留状态
    status: completed
    dependencies:
      - f-drive-sync
---

## 产品概述

收尾清理 + 分支合并决策。两部分：

1. **清理项**：删除 P 盘唯一遗留的未跟踪临时调试脚本 `.tmp_bal2.py`（内容为 chr() 混淆路径读取 crashsite.js 的两行残留，已读全文确认无保留价值）；复核两盘工作区无其他未提交/未跟踪项。
2. **分支合并**：将 `experimental`（领先 master 4 个线性提交：M2 可解释性 → 现场导出 → 惰性捕获 → M3 断点续训）合回 `master` 并推送远程，F 盘切回 master 对齐。

## 合并决策分析（结论：建议合并）

- **赞成**：git-fire 故事线（M1/M2/M3）完整闭环，CPU+GPU 双端全绿（301/33/0、49/0、18/0）；惰性捕获已消除 2-10x 性能回退；子模块 comfydl main 已推进到 b46f062，master 拖尾越久指针漂移越大；master 是 experimental 的纯线性祖先，合并为 fast-forward，零冲突，且随时可用 revert 合并提交回退。
- **注意点**：用户尚未在真实 UI 明确确认惰性捕获后的运行速度（其性能反馈早于该修复）——不阻塞合并（experimental 可继续迭代），建议合并后自行抽查一次速度与断点续训。

非代码任务，纯 Git 操作，无新技术引入。

操作要点：

- 删除文件用 Remove-Item（P 盘）。
- 合并形态为 fast-forward：`git switch master && git merge experimental`（预期输出 Fast-forward），随后 `git push origin master`。
- 子模块指针随合并自动推进到 b46f062（comfydl main 已推送，无悬空指针风险）。
- F 盘对齐：`git fetch origin && git switch master && git reset --hard origin/master && git submodule update`（memory 既定同步惯例）。
- 合并后 `experimental` 分支保留不删（后续实验可继续在此迭代，或择机删除远程分支——本次不删，保守处理）。