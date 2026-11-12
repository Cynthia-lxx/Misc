# 2026-10-04（下午）：Profiling Tools M1 交付

> 立项方案见同日 `2026-10-04-profiling-plan.md`。本文记录 M1 的实现过程、决策与验证。
> 状态：**已验收并交付**——用户界面验收通过（真实图预估 2.94 TB → CERTAIN OOM + 确认框 +
> 分解表），提交 `2a9b0ea`（feat）+ `ec33aa6`（docs readme）已 push，F 盘已 ff-only 同步，
> P/F 双环境冒烟均 290/23/0。收尾记录见本文 §六。

## 一、交付物

| 文件 | 角色 |
|---|---|
| `comfy/profiling/__init__.py` | 包入口：estimate_workflow / analyse_oom / DeviceBudget / AssumptionSet |
| `comfy/profiling/shapes.py` | 值对象：TensorVal（dim 可 None）/ SpecVal（EmbeddingSpec+BlockInfo 链）/ VocabVal / OptimizerVal / Unknown（带原因）等 |
| `comfy/profiling/assumptions.py` | 假设层：默认档位（samples=10000 / sequence_length=64）+ 覆盖 + used 记录（default/override 来源） |
| `comfy/profiling/formulas.py` | 公式库：lm_parameter_count（与物化模型逐一相等）、lm_training/lm_inference/mlp_training 上界、优化器状态系数（AdamW/Adam 2、RMSprop 1、SGD 0）、MemoryItem 同时记总量与 single_bytes |
| `comfy/profiling/estimators.py` | ESTIMATORS 注册表 16 类：Text{VocabBuild,Encode,Decode,SlidingWindow}、LanguageModel{Embedding,TransformerBlock,Build,Train,Forward,Generate,Save,Load}、TrainingOptimizer、TrainingLoop；未注册 → engine 报 unknown |
| `comfy/profiling/engine.py` | Kahn 拓扑序求值（环 → 节点级 unknown）、三色判定（红：单张量>free 或峰值≥90%总量；绿：<70%free；黄：之间；全未知 → unknown）、Report v1 |
| `comfy/profiling/postmortem.py` | CPU/CUDA/GiB/GB 三种拼写解析、张量归属（±1% 精确）、建议 batch_size 二分搜索（可行判据=峰值与最大单张量都 ≤70%free） |
| `app/profiling_routes.py` | POST /comfydl/profiling/{estimate,postmortem}；坏 JSON→400；budget 缺省时 model_management 取主设备 |
| `app/profiling_assets/profiler.js` | 前端扩展（~600 行）：zh-en 字典（跟随 Comfy.Locale）、侧栏页、顶栏徽章、graphToPrompt monkey-patch 红色确认、graphChanged 防抖 500ms、execution_error→postmortem 面板+一键应用 batch_size、全链 try/catch 降级 |
| `app/profiling_assets/profiler.css` | 面板样式：ComfyUI 主题变量+fallback、三色徽章体系、等宽数字 |
| `cdl_smoke_tests/test_profiling.py` | 37 项：T1 金样例、T2 公式 vs 真模型、T3/T4/T5/T6/T7/T8 引擎与 postmortem、T9 路由层（真实 TestServer + 资产静态路由）、T10 loader 幂等 |
| `docs/profiling-m1-memory-estimation.md` | 设计文档 + §9 用户人工验收清单（10 项） |
| `docs/branding-and-frontend-patch.md` §8 | 注入链路文档 |
| `server.py` | 3 处：import profiling_routes；逐条重新声明并入 `self.routes`（→ 自动获得 /api 孪生前缀；**不能用 `extend`**，踩坑见 §六）；extensions 静态路由后挂 `/comfydl/profiling` 资产目录（isdir 守卫） |
| `app/frontend_patch.py` | `_patch_profiling_loader`：index.html `</body>` 前幂等注入 `<script type="module" src="/comfydl/profiling/profiler.js">`，标记 `<!--ComfyDL_UI:profiling-loader-->` |

## 二、金样例验证（测试 T1）

202,651 字符语料（255 个循环字符 → 词表恰 256）+ window 64 + d_model 256 + 2×block(4头,
FFN 1024) + AdamW + batch_size=0：
- B = 202,651−64 = **202,587**（与事故反推一致）；
- attention q/k/v 输出单张量 = 202587×64×256×4 = **13,276,741,632 字节**（逐字节等于
  报错数字）；
- verdict = **red / single_exceeds_free**；FFN 隐层（B×T×F）是更大单张量（上界正确性）；
- postmortem：报错归属 train 节点 exact 匹配；建议 batch=705（约 535KB 单张量级别，
  回代自检绿）。
- T2：公式参数量与 `lm_protocol.build_model()` 物化模型逐一相等（1,712,896）。

## 三、过程要点与踩坑

1. **用户三决策**：M1 不做运行时护栏（不动节点 → 无四件套更新）；红色=确认框+可
   强制继续；事后分析纳入 M1；i18n 双语（js 内嵌 zh/en 字典，随 Comfy.Locale 切换，
   不走 gen_locales 管线——那是节点 docstring 口径）。
2. **估算输入选型**：graphToPrompt 输出（prompt 格式，名字寻址），避开了 widgets_values
   顺序映射的坑；估算内部调用保存的 originalGTP 避免与 Run 拦截递归。
3. **踩坑 23**：TestServer 裸 `web.Application()` 的 `client_max_size=1MB` 默认把 1MB
   语料 payload 打成 400（生产端是 `args.max_upload_size`）→ 测试需 `client_max_size=100MB`
   镜像生产。另：TestServer 启动即冻结 router，`add_static` 要在创建 client 之前。
4. **踩坑 24**：chunk 逆向用 venv Python 正则（findstr 双引号模式搜不到反引号字符串）；
   官方文档实证四个契约（tab 结构/graphChanged 事件/execution_error 载荷/ComfyButton
   用法——最后者从 ComfyUI-Manager 源码实证 `new ComfyButton({icon, action, tooltip,
   content}).element`）。
5. **JS 语法自查**（本机无 node）：Python 剥离字符串/注释后括号配平；正则字面量含引号
   会误报 → esc() 改 split/join 写法后通过。
6. `_normalize_optimizer`/`_optimiser_multiplier` 两处 title() 把 "SGD" 变 "Sgd" 导致
   回退 AdamW——canonical 小写表查找修复（T7a 抓获）。
7. **/api 孪生**：`fetchApi` 会补 `/api` 前缀 → 路由挂 `self.routes`（而非直接
   `app.add_routes`）自动获得孪生，这是 template_catalog 之外的新先例。

## 四、验证记录

- `test_profiling.py`：**37 PASS / 0 FAIL**；
- 全量冒烟：**290 PASS / 23 SKIP / 0 FAIL**（313/47，不回退）；
- `py_compile` 全部改动文件 OK；`gen_locales.py --check` OK（本批无节点/语言包改动）；
- JS 括号配平检查通过；临时文件（.tmp_profiling/*）已全部清理。
- F 盘（GPU，Py3.12.9 / torch 2.11.0+cu130 / RTX 5060，`penv_new`）复核：`test_profiling.py`
  **37 PASS / 0 FAIL**、全量冒烟 **290 PASS / 23 SKIP / 0 FAIL（313）**，与 P 盘一致。

## 五、待办

- ~~用户界面验收、commit/push、F 盘同步与双环境冒烟~~ → §六 已全部完成；
- M2 候选：运行时护栏（max_batch_tokens）、激活系数校准、符号形状完整化、更多节点族
  （RNN scratch / 卷积 / Embedding 训练）、徽章悬停明细卡（设计文档 §10）。

## 六、验收与收尾（2026-10-04 晚）

1. **踩坑 25（真实启动才暴露）**：首次 `python main.py` 崩在 `server.py` 的
   `routes.extend(profiling_routes.routes)` —— aiohttp 的 `RouteTableDef` 是 `Sequence`，
   **没有 `extend()`**。集成测试（自建 `web.Application` + TestServer）与冒烟器（不启动服务）
   都不会构造 `PromptServer`，所以全绿也没拦住。修复 = 逐条重新声明（与 `server.py:1241`
   给 `/api` 孪生再声明一次同款），并用临时脚本最小复现验证 `/comfydl/...` 与
   `/api/comfydl/...` 同时 200 后删脚本。**教训：改动 server 构造/启动路径必须真启动一次。**
2. **用户界面验收通过**（用户本人执行，符合铁律）：真实图上预估峰值 **2.94 TB**、最大单张量
   `logits (B×T×V)` **1.21 TB**、verdict = **CERTAIN OOM**（单张量 > 空闲）、Run 弹出确认框
   （可 Confirm 继续）、侧栏分解表按节点列出参数/梯度/优化器状态/激活分项——与会话开头的
   13.3GB 事故形成闭环（同一份图，只是 batch 规模不同）。
3. **提交**：`2a9b0ea` feat(profiling) 15 文件 3307 行 + `ec33aa6` docs(readme) 3 张验收截图
   入 `assets/profiling_memory_{verdict,warning,breakdown}.png`，README/README_zh 新增
   「Memory profiling / 内存分析」章节（含公式/阈值/降级链指向设计文档）。已 push 到
   `origin/master`；F 盘 `fetch` + `merge --ff-only` 同步到 `ec33aa6`（F 盘 git 需用
   `-c safe.directory=F:/Dev/ComfyUI_Refs/ComfyDL_UI` 单次覆盖，跨用户名 dubious ownership，
   **不改全局 config**）。
4. 本批**零节点改动** → FUNCTIONS×2 / comfydl README×2 的节点说明与计数无需改动
   （节点数仍 313/47、口径 207/35 = 109 ComfyDL + 98 core）。
