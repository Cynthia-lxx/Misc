# 2026-10-04 · Profiling Tools 立项：可行性分析 + Memory Size 分析方案构想

> 状态：**方案阶段，未动工**。用户明确「先不要动工，进行可行性分析与方案构想，写入记忆，
> 待细节明确后再动工」。

## 〇、导火索：1MB Tiny Shakespeare 压测 OOM（已定位根因）

用户配置：window=64 / vocab=256 / d_model=256 / 2×TransformerBlock(4头, ffn 1024) /
AdamW lr=3e-4 / LanguageModelTrain steps=300 **batch_size=0**。第一步即 OOM：
`DefaultCPUAllocator: you tried to allocate 13,276,741,632 bytes`（机器 16GB RAM，
空闲 2.96GB）。

根因链（实读代码核实）：
1. `TextSlidingWindow`（`comfy_extras/nodes_nlp.py`）是 **stride=1** 逐位切窗
   （docstring 原文"d2l seq_data_iter_sequential 的最简 stride-1 形式"）→
   样本数 ≈ token 数（1MB 语料 → 数十万~百万级样本）；
2. `LanguageModelTrain`（`comfy_extras/nodes_lm.py:593-600`）的 `batch_size=0`
   语义 = **每步全库一批**（`per_step<=0 or >=samples → per_step=0 →
   batch_x = inputs 全量`）；
3. 前向第一个 `k_proj` 输出张量 = B×T×E×4B ≈ 13.3GB 单笔申请 > 空闲 RAM → 炸。
   反推 alloc：13,276,741,632 / 4 / 64 / 256 ≈ B≈202,587 个样本一个 batch。
   模型本身只有 ~10M 参数（40MB 级）——**纯批量问题，不是模型问题**。
4. 从 got prompt 到 OOM 耗时 85 秒（Python 循环切窗 + 逐样本 pbar），说明切窗阶段
   也在裸奔（list of 20 万个 64 长度 list 的构建）。

结论：数据规模上来后整条流水线无任何内存预估/护栏——这正是 Profiling Tools
Memory Size 分析的第一个测试用例（golden case）。

## 一、Profiling Tools 总体定位（用户构想，记录在案）

- 远期包含：性能调优、算子融合、计算流分析、性能数值分析等。
- **现在只做 Memory Size 分析**，但结构设计要给上述功能留实装空间；
  **不创建空入口**（用户原话：现在不要创建这些功能的空入口）。
- Memory Size 分析的核心行为：实时根据当前工作流参数 + 数据流 + DL 算法逻辑
  （LLM 训练、SD 等各参数对内存/显存的影响公式），快速算出所需内存估计值，
  与 device/memory/vram 比对，可能 OOM 变黄、一定 OOM 变红，醒目提醒；
  **按下 Run 之后发出二次提醒**。

## 二、UI 构想（用户方案 + 已核实的可行性）

用户方案：
1. **完整 Profiling 界面**：与 Assets/Nodes/Models/Workflows/Apps/Templates 同级的
   侧栏入口（参考 ComfyUI-Manager 的完整页面）；
2. **小型叠加组件**：靠近 Manager 入口/Run 按钮，可 dock 进 ComfyUI 原生右上 dock 区，
   显示估算内存数字（如 13.672GB），可能 OOM 黄 / 一定 OOM 红。

技术核实（2026-10-04 实测前端包 comfyui-frontend-package 1.51.9 bundle）：
- ✅ `app.extensionManager.registerSidebarTab(tab)` 存在（settingStore chunk，
  官方 API，workflows/assets 等侧栏同机制注册）→ 侧栏入口**无障碍**；
- ✅ `registerBottomPanelTab`、`registerCommand`（ComfyCommandImpl：id/icon/label/
  function/category）存在 → 命令面板/快捷键可达；
- ❌ `registerTopbarButton` / `registerTopbarMenu` **不存在**——顶栏按钮只能自挂 DOM
  （ComfyUI-Manager 正是这么做的）或走 registerCommand。叠加徽章的挂载点
  （顶栏 dock 区）依赖前端布局 DOM 结构，**升级可能漂移**，需容错 + 降级
  （挂不上就只留命令面板入口 + 侧栏）。
- ⚠ 工作区里**没有** ComfyUI-Manager 源码目录（用户提到 ./ComfyUI-Manager/）——
  动工前需要用户提供该参考代码，或我从 GitHub 读。
- 关键架构约束（踩坑 19 铁律）：本仓无 web/，前端 = PyPI 包，**一切前端改动必须经
  `app/frontend_patch.py` 启动期补丁或仓库内服务端路由**。Profiling UI 的 JS/CSS
  放仓库内（如 `app/profiling_assets/`），server 加静态路由（`/comfydl/profiling/...`），
  `frontend_patch.py` 给 index.html 幂等注入 `<script src=...>` loader——前端代码本体
  不打补丁进 chunk，只是"合法扩展 + 官方 API 的使用者"。

## 三、架构分层设计（提议）

```
┌ 前端扩展（profiler.js，仓库资产，frontend_patch 注入 loader）
│   ├ 侧栏页 registerSidebarTab（完整 Profiling 界面，先只渲染 Memory 一个区块）
│   ├ 顶栏徽章（自挂 DOM 容错） + registerCommand 兜底入口
│   ├ 图变更监听（graphChanged 等 hook）→ 防抖 → POST /comfydl/profiling/estimate
│   └ Run 二次确认：verdict=红 → confirm 弹窗（wrap queuePrompt，Manager 同款手法；
│       官方 beforeQueued hook 待查证，有则优先）
├ HTTP API（server.py 或 app/profiling_routes.py，PromptServer 路由）
│   ├ POST /comfydl/profiling/estimate   body=前端序列化的 workflow graph
│   └ GET  /system_stats（复用现有；RAM/VRAM 预算来源）
└ 估算引擎（纯后端包 comfy/profiling/，零 UI 依赖，可独立冒烟）
    ├ 抽象求值器：拓扑序遍历图，逐节点跑 estimator，传播符号形状
    ├ ESTIMATORS: dict[node_id, callable(EstimationCtx) -> NodeEstimate]
    │   输出 = 各输出槽形状 + 内存分项(weights/activations/optimizer_states)
    │   + 置信度 + 依据文本；未注册节点 → 显式 unknown，不瞎猜
    ├ Shape 系统：dim 为 int 或 Symbolic（文本长度等未知量 → 假设层假设值）
    ├ 假设层（assumption set）：输入数据规模不可静态知（文本长度/数据集大小）
    │   STRING widget 按 len() 计；文件类将来按 stat；未知量用户可在面板覆盖，
    │   默认档位 + 明示"基于假设 N tokens"
    ├ DL 内存公式库（即"算法逻辑"）：
    │   LMTrain 峰值 ≈ P + G + AdamW(m,v=2P) + Σ激活(B×T×E×每层系数) + logits(B×T×V)
    │   推理 ≈ P + 激活；逐层 attention(Q/K/V/O/softmax)、FFN 展开
    └ Report 结构：sections={memory:…} 预留 flops/timing/fusion 并列位
      verdict: ok / maybe / certain（对 device 预算比较，阈值可配）
```

MVP 覆盖 estimator 的节点族：LM 文本流水线（VocabBuild/Encode/SlidingWindow）+
spec 链（Embedding/TransformerBlock）+ Build/Train/Forward/Generate + TrainingLoop
——正好覆盖 OOM golden case。

## 四、风险与难点（可行性结论：可行，难点已识别）

1. **输入规模未知**是最大方法论难点 → 假设层 + 用户覆盖 + 明示假设（上面已设计）。
2. **峰值 ≠ 稳态**：PyTorch 分配器缓存/碎片、autograd 释放时机 → 输出"上界近似 +
   置信度"，并用真实运行校准系数。
3. **CPU 构建 VRAM=RAM**（system_stats 同口径）→ 预算模型统一处理。
4. **顶栏 DOM 挂载点漂移**（前端升级）→ 容错降级。
5. **估算不准时的信任问题** → 三色规则建议：绿 est<free×0.7；黄 est∈[free×0.7,
   total×0.9]；红 est>total×0.9 或**任一单张量>free**（OOM golden case 即后者）。
   黄只提醒，红二次确认但可强制继续（免责：启发式）。
6. 估算性能：纯静态图分析（无真实执行）应 <100ms 量级，防抖 500ms 无压力。

## 五、我的补充建议（待用户裁决，均未实施）

1. **运行时护栏（与预估互补）**：预估可能错，`LanguageModelTrain` 可加可选
   `max_batch_tokens`（超限自动切片或显式报错带建议值）。动节点行为，需用户点头。
2. **Post-mortem 模式（低成本高价值）**：OOM 已发生时，解析 RuntimeError 的字节数，
   在 Profiling 面板给出"哪个张量多大 + 建议参数"（如"batch_size=1024 可解"）。
   本次的 13.3GB 报错就是第一个样例。
3. 侧栏页 MVP 内容建议：设备预算卡（total/free/已用）+ 总估算 + verdict +
   按节点分解表（形状链 + 内存分项 + 依据）+ 假设编辑区。

## 六、实施分期（待用户确认后动工）

- M1：估算引擎（LM/训练族 estimator + 假设层）+ API 路由 + 侧栏页（只读报告）
  + 顶栏徽章 + Run 二次确认 + 冒烟专项测试（OOM golden case 反推断言）。
- M2：假设层完善（文件大小来源、用户覆盖持久化）、更多节点族 estimator、校准。
- M3+：flops/timing 等其他 Profiling 板块（结构已预留，届时再开口）。

## 七、ComfyUI-Manager v3.41 参考调查（2026-10-04 用户放入工作区后专项探索）

版本/架构：Manager 3.41，**纯原生 JS ES Module、无构建步骤**（js/ 即产物），
`WEB_DIRECTORY = "js"` 经 `/extensions/ComfyUI-Manager/` 静态分发；文件以
`import { app } from "../../scripts/app.js"` 开头（相对路径走前端 import map——
我们注入的 module script 同样可用该 map）。对 Profiling 的关键发现：

1. **顶栏徽章的官方正解**：动态 `import(".../scripts/ui/components/buttonGroup.js")`
   取 `ComfyButtonGroup`/`ComfyButton`，`app.menu.settingsGroup.element.before(group.element)`
   插入（comfyui-manager.js:1579-1644）——比自挂 DOM 稳，主题自动跟随，
   `ComfyButton.content` 可放 HTML（内存数字徽章可用）；旧 UI 用
   `document.querySelector(".comfy-menu")` + try/catch 降级。
2. **Run 前拦截的正解 = monkey-patch `app.graphToPrompt`**（components-manager.js:781-812
   先例）：保存原函数 → await 原函数 → 校验/改写 → 返回。graphToPrompt 是 Run 提交
   必经点，不受顶栏 Vue 重构影响，比 wrap queuePrompt 稳。二次确认在此弹
   `extensionManager.dialog.confirm`。
3. **"等 Vue 就绪"轮询模式**（:1549-1569）：setup() 常早于 Vue onMounted，
   Manager 用 100ms 轮询 `app?.extensionManager?.toast`（上限 30s）再初始化——
   我们的徽章/侧栏初始化同样需要。
4. **通知/对话框三级容错**（js/common.js:120-176）：优先
   `extensionManager.dialog.confirm / toast.add / toast.addAlert`，catch 后回落
   自研全屏 modal / alert()。照抄此模式。
5. **registerSidebarTab Manager 自己没用**（全仓零匹配，其"完整页面"是
   ComfyDialog 全屏模态 + 手拼 PrimeVue class `p-dialog-*` 伪装原生对话框，
   `buildGuiFrame` comfyui-gui-builder.js:30-134；单例 + display 切换的页面生命周期
   custom-nodes-manager.js:50,2212）。侧栏 tab 对象结构仍需从前端包类型定义确认
   （存在于 1.51.9，前次已核实 API 名）。
6. **CSS/资产**：Manager 用 `import.meta.resolve(cssFile)`（仅 /extensions/ 下有效）；
   我们走自建路由，直接硬编码 `/comfydl/profiling/profiler.css` 即可（我们控制路由）。
   图标可学 Manager 内联 SVG 字符串。
7. **后端通信三件套**（照抄即可）：`routes = PromptServer.instance.routes` + 装饰器；
   前端 `api.fetchApi`；`PromptServer.instance.send_sync("自定义事件")` ↔
   `api.addEventListener`。
8. `registerExtension` 还支持 `commands`（进命令面板）与 `aboutPageBadges` 字段。

→ 对方案的影响：§二 UI 构想中"顶栏徽章需自挂 DOM 容错"升级为
"ComfyButtonGroup 官方手法 + 旧 UI 降级"；§三 Run 二次确认从"wrap queuePrompt"
改为"monkey-patch graphToPrompt"；两条风险（顶栏漂移/Run 拦截手法）等级下降。


## 八、本次核查的技术事实存档

- `registerSidebarTab` / `registerBottomPanelTab` / `registerCommand` 均在
  `settingStore-KkBYyEnh.js` 的 extensionManager 上（1.51.9 实测）；
  `registerTopbarButton`/`registerTopbarMenu` 不存在。
- `comfy/model_management.py:318 get_total_memory / :1755 get_free_memory` +
  `server.py:689 /system_stats` = 设备预算现成来源。
- `nodes_lm.py:593-600` batch_size=0=全库；`nodes_nlp.py` SlidingWindow stride=1
  （`mp.iter_windows`）；OOM 反推 B≈202,587×T=64×E=256。
- 本机为 CPU-only 构建（16GB RAM），free 只有 2.96GB（被进程自身占掉大半）——
  黄/红判定建议同时看 total 口径与 free 口径。
