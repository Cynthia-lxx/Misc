# 项目长期记忆

> 精炼快照；按日详录见同目录 `YYYY-MM-DD*.md`（索引见文末）。

## ⏳ 阶段状态声明（权威）
**Crash site（工作流执行现场快照/恢复）大计划制作阶段：已固化（2026-10-07）**——
正式大计划见 `2026-10-07-crashsite-bigplan.md`（2 个 subagent 前置调研已固化：
**CacheProvider 宿主正道**=`comfy_api/latest/_caching.py` 的 on_store 增量 capture
+ on_lookup lazy 回填，键=输入签名 SHA256，消除抢救窗口问题；cache_type 边界——
RAM_PRESSURE 报错后 eviction 停但执行中 ≥512MB CPU tensor 可被逐、NONE 模式不支持；
GPU tensor 需 .detach().cpu()；safetensors bytes 先例 training_protocol.py:949；
类型映射表 MODEL/CLIP/VAE/CONDITIONING 降级、LATENT/IMAGE/TENSOR/DATASET 完整、
cdlDataloader 不可）。**ADR×7**：独立体系（不受 profiling master switch 影响）、
核心架构=CacheProvider、单 SQLite 库主形态+导出目录视图、类型映射表、tamper=
prompt 改写+常量注入节点、capture 三源、恢复=重 Queue 原 prompt。三 Phase：
M1 核心闭环（provider+库+恢复+最小 UI，2-3 craft）→ M2 汇总表/多文件/跨机/tamper
（3-4）→ M3 训练 checkpoint 并入（2-3，与断点续训合流）；总 7-10 craft。
**待用户审阅大计划后逐 Phase 进 PLAN MODE smallplan + (CRAFT)*n**。
**M1 已交付（2026-10-08，父仓 `c98e77d`，102 commits，tag `v0.4.0` 检查点）**：
CacheProvider 增量快照（app/crashsite/ 四件套）+ 每快照独立 SQLite（复合主键
(cache_key_hash, slot)）+ 独立侧栏 tab + resume 服务端 re-queue；test_crashsite
19/19、全量 301/33/0 双盘、真启动 EXIT=0；**验收堵点修复（`db52d00`，103 commits）**：
cache_middleware 对 index.html 无缓存头 → 浏览器启发式缓存吞掉新 loader tag（整机
重启不清 HTTP 缓存）→ 补 no-cache 强制 revalidate；顺带修 server log 卡
（/internal 是 subapp 不在 /api 孪生内，fetchApi 必 404 → 改原生 fetch）。
**教训：index.html 类单行大文件断言必须 findstr（read_file 截断）；"前端没出现"
类问题先查缓存头；Console 无报错+无请求 = 脚本没被加载**。M2（汇总表/多文件/跨机/
tamper）与 M3（训练 checkpoint）待续。

## 前一阶段（已收官）
**Profiling v2 大计划：✅ 全部交付并验收（2026-10-07 收官）**——四 Phase 依次：

## 前一阶段（已收官）
**Profiling v2 大计划：✅ 全部交付并验收（2026-10-07 收官）**——四 Phase 依次：
P0 止血（`fb270ca`→`3047027`→`9fd5e81`：危险模式 DangerousProbe + X-CDL-Profiling-Dangerous
头、OOM verdict/peak 主门限 + 4MB/16MB 字符串兜底、专用单线程 executor + 60s 超时 +
协作 deadline、safe/refused/timeout 报告形态、FakeTensor 失实修正、M3 文档 §8）；
P1 等效计算图（规则库 + 离线采样 + 零执行组装 + SVG 等效图 + 覆盖率）；P2 实测回流
（RunMeter 单层 dispatch mode + 采样计量 + SQLAlchemy/Alembic 0007 + history 端点 +
estimate 校准，`49a3bc0`→`88ff078`）；P3 三列分区 overlay + 导出矩阵（`cfe446d`，
返修 `c04409d`/`2fb06ad`：sans-first 字体、OOM 门限重构、SVG 内联样式、mermaid.live
按钮削除）。大计划存档 `2026-10-06-profiling-v2-bigplan.md`，逐 Phase 详录
`2026-10-06.md` §6-§12。**当前状态：待用户指派下一项开发**。

## 当前待办
- **▶▶ 双主线能力大计划（2026-10-31 立项草案落盘，待用户审阅）**——正式文档
  `2026-10-31-dual-track-bigplan.md`（能力簇 C1-C7 × 生产/教学双门面矩阵、开销治理
  ADR-D1~D5、共享基建四件、里程碑 M-B/M-C/M-D）+ `2026-10-31-dual-track-smallplans.md`
  （SP-0~SP-7 小计划草案 + 推进顺序 + 调研就位度：七簇中六簇档案已行号级就位）。
  数据底座：`refs_survey/`（50 仓库调研报告）。**里程碑构想备案**：M-A=分类线收官（在案）
  → M-B 性能红利包（P0 基建+attention 选优+compile+显存管家+量化最小版，8-10 craft）
  → M-C 生产力教学包（实验台+数据簇+交互 UI，10-11 craft）→ M-D 观测透视包（5-9 craft，
  crash site 重设计 T6-1 视 SP-0 归因实测结果定夺）；总量 22-29 craft，三级治理。
  待拍板：SP-0 立即执行否 / 合批方案 / 实验台雄心档位 / M-C 是否前移。
  **→ 2026-10-31 用户接受第一步计划 `dual-track-p0-spine`（SP-0 归因实测 + SP-1 P0 基建：
  观测总线/签名缓存/开关注册表/watchdog 迁移，代码直接移植 XPUSYS/Crystools/AMDMonitor/
  Model-Analytics）并开始执行。**
- **✅ 第一步 CRAFT 已交付（2026-10-31，未 commit 待用户确认，详见 `2026-10-31.md` §七）**：
  SP-0 归因报告（`2026-10-31-sp0-attribution.md`）——**真凶=crash site（+20~100%），
  profiling meter≈0~4%；T6-1 立项且设计优先级改为键检查廉价化>惰性建库>ring buffer>重放**；
  SP-1 基建四件全交付（switch_registry/signature_cache/app spine 五件套/watchdog 骑总线，
  proflog 与 crashsite_switch 降薄壳）+ test_obs_spine 39 项；验证：节点冒烟 306/33/0、
  真启动通过、脚本级全绿、lint 0。新踩坑：WMI（热路径禁 platform.system）、采样线程禁
  热导入 server、409 判定需独立 is_hard_disabled、stop 需 join。
- **✅ 分类对称线已收官（M-A 里程碑完成）**——正式大计划
  `memory/2026-10-28-classification-line-plan.md`（对标回归线五件套：DataGen/SoftmaxTrain 教科书版/
  ClassificationTrain 生产版/ModelLoad/ConfusionMatrix + 模板 ×2，ADR×8，Phase P0-P4 共 4.5 craft）。
  5 新节点 / 2 新模板，`d2l/Training` 3→6。三决策点已定稿：SoftmaxTrain 做、标签 v1 只支持类索引、
  指标只出 overall accuracy。**（已合并主干，见 §七/§十二）F 盘已于 2026-10-30 回归，
  GPU 冒烟欠账全清（306/33/0 双端一致，见 `2026-10-30.md`）**。
- **✅ CDL App Panel 批次 4-23 已交付（另一 session，2026-10-31 本日补备案）**：父仓
  `e8a236c`（批次 4-15：原生 dock 仪表盘、run scoping、builder 步跳转、builder 回 App、
  侧栏抽屉共存）→ `dbe3de3`（16-19：增量进度行、motion 套件含全局开关、**矢量实时图表
  （structured preview data，比 bitmap 便宜 10^4x）**、主题安全对比度）→ `8550d20`（20-23：
  曲线缓动、流式文本揭示、可缩放矢量图、previews 滚动条、停止按钮）→ `8f31a0f`（滚动条
  fix）→ `c71dbb8`（README App Panel 章节与性能调优/翻车现场并列）。资产：
  `app/cdlapp_assets/{cdlapp.js,cdlapp.css}` + `app/cdlapp_routes.py`；子模块配套
  `formula_playground_app` 模板 + extra.ds 画布规范化（`900781a`/`e60e7bc`）。
  启动默认工作流已切 `classification_from_scratch`（`fab995e`）。两仓工作区干净。
  **对双主线的影响：SP-5 实验台对比视图前端底座改骑 cdlapp 矢量图表体系（原 profiler.js
  方案作废）；M-D 观测包仪表可进 App Panel 而非另起面板。**
- **🔒 新铁律（2026-10-28 用户原话）**：「做任何算子或者节点都应该有两种形态，一种是最大生产力，
  一种细粒度、容易理解的教科书级。」→ 今后新增算子/节点族默认**双形态交付**（一盒装生产版 +
  细粒度教科书版）；回归线 `CdlLinRegTrain`↔`CdlRegressionTrain` 是对标模板；纯基础设施（类型/
  适配器/可视化/预览）例外。详见 `2026-10-28.md` §六。
- **homepage（GitHub Pages）暂告一段落（2026-10-28 用户指示）**：homepage 已交付并推送
  （master `0a278d5`，2026-10-14 七连提，详见 `2026-10-28.md` §三），后续维护类工作冻结，
  Pages Source="GitHub Actions" 是否已在远端开启待核实。
- **Profiling 系列（M1-M3 → Profiling v2 大计划）全部交付并验收 ✅（2026-10-07）**：
  M1 内存估算（`2a9b0ea`/`ec33aa6`，金样例 13,276,741,632 字节精确命中）→ M2 compute
  ledger + watchdog（`2026-10-04-profiling-m2.md`）→ M3 opgraph → **Profiling v2**
  四 Phase（详见顶部阶段声明与 `2026-10-06.md` §6-§12）。设计文档
  `docs/profiling-m1-memory-estimation.md` / `profiling-m2-compute-and-watchdog.md` /
  `profiling-m3-opgraph.md` / `profiling-logging.md`。后置 backlog（bigplan 既定）：
  等效 workflow 导出、.vsdx、F4 roofline、W3 建议库。
- **⛔ 铁律（2026-09-12 用户原话）**：「这类视觉测试流程以后都是交给我做，你只需要进行底层测试。」
  → **不要再起浏览器 / 用 `playwright-cli` / 截图 / 读 DOM 做验收**。需要人眼确认的，
  写清「怎么点、看什么、期望值」交给用户。我的职责 = 冒烟器、`--check` 一致性脚本、接口级断言、纯 Python 回归脚本。
- ~~品牌化 v0.3.1 待用户自验收~~ → **2026-10-02 验收完成**：效果一般般、基本目的达到；
  后续可对品牌化 UI 进一步优化（非紧急候选项）。
- ~~**⚠ 已知缺陷（暂不修复，2026-10-02 用户在案）**：Templates → ComfyDL →
  "Language Model: Train and Chat" 模板**入口**坏了~~ → **2026-10-04 已修复并验收提交**
  （主仓 `e0fb9c6`，已推送；用户界面验收通过）：
  根因 = 注入分类 `moduleName: "ComfyDL"` ≠ `"default"`，前端把 moduleName 拷进卡片的
  `sourceModule`，只有 `"default"` 才走 `/templates/<name>.json` 通道，其余一律打到
  `/api/workflow_templates/<module>/...`（custom_nodes 通道，内置 overlay 不在其中）→ 404 →
  前端 `.json()` 抛异常静默 return false → 点击无反应（缩略图同因失效）。修复 =
  `template_catalog.py` 的 `_COMFYDL_CATEGORY["moduleName"]` 改回 `"default"`，
  详见踩坑 22 与 `2026-10-04.md`。
- **新示例工作流：推迟**，等节点更为完备后再做。
- **L2 六占位评估结论（2026-10-02，详见当日 §二）**：非至关重要。全部服务于 SD 图像管线
  （VAE/CLIP/LoRA 三件套），转正 = 回捞 ldm+text_encoders+samplers 整链（战略级决策非节点批次）；
  唯一有独立教学价值的缺口是 **LoRA 思想** → 推荐方案 A：教学级 LoRA 节点作用于自研 NNMODEL
  （不回捞 comfy/lora，衔接 step6/9 训练闭环）。已向用户给出 A/B 两方案待选。
- **reform 残留**：L2 六个占位节点（LoRA×2 / VAE 编解码 / 文本编码截层）在回捞 `ldm`/文本编码器后原地转正（评估见上）。

## 项目 / 架构
- **`ComfyDL_UI v0.3.1`**（对外身份；上游基线是 ComfyUI v0.34.0）= 脱水改装项目，根 `p:\Dev\ComfyUI_Refs`。定位 = ComfyDL 教学节点宿主；下一步转 DL 生产工具。脱水清单 `ComfyDL_UI\docs\dehydrate_manifest.md`（含回捞安全网：删除项可从 `ComfyUI-original/` 按清单回捞），内置化 `docs\comfydl-builtin.md`，战略原文 `2026-09-10.md` §5。品牌/版权/补丁层总说明 `docs\branding-and-frontend-patch.md`。
- **前端改动一律走 `app/frontend_patch.py`**（启动期幂等补丁层，`server.py` 前端根解析后调用）：默认语言 en、`Nodes 2.0` 默认开、About 面板自述分区、六张主题表 `TENSOR:#C6FF00`。按**内容标记**定位（不认哈希文件名）、失败不阻塞启动。**禁止再手工改 site-packages**（旧做法已作废）。
- **语言包**：仓库根 `locales/{en,zh}/{main,nodeDefs}.json`，由 `app/custom_node_manager.py` 的 `build_translations()` 经 `REPO_LOCALES_DIR`/`load_locales_dir()` 并入 `/i18n`。`main.json` 只有 `comfydlAbout.*`（5 键）；`nodeDefs.json` **只有 zh（109 条）**，英文包故意不存在（缺键回退注册表）。生成器 `cdl_smoke_tests/gen_locales.py`（`--check` 校验），中文来源是 `comfydl/FUNCTIONS_zh.md` 的节点小节标题 + `- **功能**：` 行。
- `ComfyDL_UI/` = 改装 + 唯一 `.git`（master）；`ComfyUI-original/` = 只读权威参照，绝不动；`ComfyDL_UI\comfydl/` = submodule（main）；`P:\Dev\ComfyUI_Refs\ComfyDL` = 同源独立 checkout（只 pull）。**git 需带 `-c safe.directory=…`，不改全局 config**。
- 注册链 `nodes.py:init_extra_nodes()` = public_apis → builtin_extra → **builtin_dl** → api → external。`init_builtin_dl_nodes()` 直接 `import comfydl` 合并映射（**不走 `load_custom_node`、不读 `comfy_entrypoint`**）→ comfydl 内 V3 类必须进 `NODE_CLASS_MAPPINGS`；`comfy_extras` 是**硬编码 `extras_files` 白名单**（漏加即不注册），白名单内 `.py` 走 `load_custom_node` 时**支持 `comfy_entrypoint`**（nodes.py:578-608）。
- 已删：ldm / text_encoders / k_diffusion / 采样器 / controlnet / lora / blueprints / tests / comfy_api_nodes。保留 comfy_execution、app、api_server、middleware、utils、comfy_types、model_management。前端 = PyPI `comfyui-frontend-package==1.51.9`，无本地 `web/`。
- **Profiling 引擎（2026-10-04 M1 交付）**：`comfy/profiling/`（纯模块零 torch 执行：engine 拓扑求值 / estimators 16 类注册表 / formulas 上界公式+单张量追踪 / assumptions / postmortem）+ `app/profiling_routes.py`（POST estimate/postmortem，**逐条重新声明**并入 `self.routes` → 自动获得 /api 孪生；勿用 `extend`，RouteTableDef 无此方法，见踩坑 25）+ `app/profiling_assets/profiler.js/css`（仓库资产，`/comfydl/profiling/` 静态路由，frontend_patch 注 loader）+ `cdl_smoke_tests/test_profiling.py`（37 项）。估算输入 = graphToPrompt 输出（prompt 格式名字寻址）。设计文档 `docs/profiling-m1-memory-estimation.md`、注入链路 `docs/branding-and-frontend-patch.md` §8。

## 关键踩坑
1. **前端节点库分区只看 `python_module` 首段**（`nodes`/`comfy_extras`/`comfy_api_nodes` = Comfy Core）→ 并入核心区须 `PYTHON_MODULE="comfy_extras.nodes_comfydl_*"` + `_CoreMergedNode` 基类（`RELATIVE_PYTHON_MODULE` 仅供 `/object_info`，不会被 import）。
2. **V1 弃用三件套**（勿发明新标记）：`DEPRECATED=True`（`server.py:783`）+ 显示名加 `(DEPRECATED)` + 分类移 `d2l/_Legacy/<原分类>`；**节点 id 绝不能改**。`advanced` 在 V1 可用（`server.py:756`）。
3. **节点库只在页面加载时取一次**（`/object_info`）→ 后端改节点后必须刷新页面。
4. **训练/推理态通道必须走连线，勿用 hidden PROMPT 自省**（缓存签名只看已连线输入及祖先 → 静默复用过期缓存）；消费者侧必须是 **STRING**（`validation.py` 拒绝 STRING→COMBO）。
5. **随机数**用局部 `torch.Generator(device=…)`，勿用全局 `torch.manual_seed()`；seed 用 INT 控件 + `control_after_generate=True`。
6. **三套计数口径都对，勿互相"纠正"**：① ComfyDL banner **109/20**；② 宿主注册表 **313/47**（step10 后，2026-10-02）；③ 四份说明文件"内置节点库" **207/35**（= 109 ComfyDL + 98 核心；权威值见 `comfydl/FUNCTIONS.md` 的 Total Node Count 段与 `docs/comfydl-builtin.md` 各步实测段）。软归档不增删节点。
7. ~~**前端 `TENSOR` 槽颜色注入在 site-packages**，前端包升级即丢失，需手工重打~~
   → **2026-09-12 已作废**：前端四项改动（locale / Nodes 2.0 / About / TENSOR 配色）全部由
   `app/frontend_patch.py` 在启动期自动重放，升级前端包后重启即自动重打。`docs/reform-step1-activation-tensor.md`
   里的「手工重放」告警已改写为指向该补丁层。
8. **V3 契约**：`io.Schema.category` 默认 `"sd"`，**必须显式传**；`AUDIO` 字段实际拼作 `sampler_rate`；comfydl 用 legacy 风格（模块级空 dict + 类后逐条赋值，docstring 分 `Inputs:`/`Outputs:`）。
9. **冒烟器跳过 `optional=True` 且非 widget 类型（TENSOR/ARRAY/DICT…）的输入** → 这些 `execute` 形参必须给默认值。
10. **前端 3D 画布只认硬编码节点 id** → 自定义节点无法自带 3D 画布，只能"产出 3D 文件 → 交给原生 `Preview3D`"。`loadOBJ` 在 `materialMode==='original'` 时自动加载同名 `.mtl`；⚠ `Preview3D.execute` 会 `save_to(output/preview3d_<uuid>.<fmt>)` **重命名** → 兄弟 `.mtl` 会丢，须覆写 `save_to`。
11. **`_update_readme.py` 会把 FUNCTIONS 附录的总句改写成 ComfyDL 口径（109/20）** → 该句只写"…均由 ComfyDL 本身提供"，**库总计句（177/32）另起一段手写**，否则写进去会被脚本覆盖。README 的 "ships N nodes across M categories" 总句脚本不匹配、需手改。
12. **git 不在 IDE 终端 PATH 里**：真身 `C:\Program Files\Git\cmd\git.exe`。用
   `cmd /c "set "PATH=C:\Program Files\Git\cmd;%PATH%" && cd /d <repo> && git …"`（引号嵌套包 exe 全路径会失败）。
13. **命令里的 `$` 会被终端包装吞掉**（PowerShell `$var`/`$_.Line` 会变形报错）→ 优先 `cmd /c` + `findstr/dir`；
   `findstr /c:"a" /c:"b"` 多模式也会被拆碎，**一次搜一个词**。本工作区巨大，`search_file`/`search_content`
   递归易 10s 超时，定位文件优先 `list_dir` / `cmd dir /b /s <收窄目录>`。
14. **改前端 chunk 前先记字节数 + sha 前缀**（`verify_patches.py` 末尾 FINGERPRINT 行），验收要**同时验磁盘与「被服务副本」**（`*/served` 断言），只看磁盘会漏掉静态路由缓存旧副本。
15. **前端 chunk 压缩变量名不可硬编码**（正则现抓）；`settingStore` 里 `node_slot` 有 7 处但只有 **6 张主题表**，第 7 处是调色板合并表达式 `node_slot:{...t.colors.node_slot` → 判据「`:{` 后紧跟 `...` 则跳过」，且**表数 ≠ 6 必须拒写**。
16. **torch `nn.MultiheadAttention` 布尔掩码 `True`=屏蔽，与 `F.scaled_dot_product_attention`（`True`=可注意）相反**（实测 torch 2.13，含 fast path）；additive 浮点掩码两侧一致。核心注意力节点采 SDPA 语义并写入文档；对照 `nn.MHA` 断言须取反。另外 `need_weights=False` 时 MHA 有 fast path，但行为与慢路径一致。
17. **`comfy_extras` 内跨模块导入必须绝对导入**（`from comfy_extras.nodes_normalization import …`，同 `nodes_latent` 先例）：该分支 `load_custom_node` 忽略 `module_parent`、按文件路径命名模块 → 相对导入必炸「no known parent package」（step7 实测）。
18. **新建文件前先查重名**（step8 两次踩雷）：`comfy_extras/nodes_text.py` 已是上游 Save Text 节点、上游已有 `comfy/model_protocol.py`——动手前先 `git status` + 全库搜文件名/类名；stash 导出的是 diff 不是文件内容（`git stash show -p > f` 得到补丁文本，不能当源码用，需整体重写）。
19. **读/逆向前端 bundle 是允许的，但绝不直接改 site-packages**（2026-09-20 用户立规）：分析 `comfyui_frontend_package` 的压缩 JS 属于正常调研；但任何要落到前端的改动（含 `comfyui_workflow_templates*` 系列 PyPI 包里的模板资产）都必须经 `app/frontend_patch.py` 补丁层或**仓库内服务端代码**（自建路由/请求期改写）实现，直改包内文件会在 `pip install -U` 后丢失。
20. **ComfyUI 节点进度条与实时预览的正确姿势（2026-09-20 进度报告 + 2026-10-02 预览特性实测，见 step11）**：节点侧只需 `import comfy.utils` + `pbar = comfy.utils.ProgressBar(total)` + 循环里 `pbar.update(1)`（`update_absolute(v, total)` 用于比例场景）。`main.py` 用 `hijack_progress(server_instance)` 注册的全局 hook 会通过 `comfy_execution.utils.get_executing_context()`（contextvars）**自动把更新绑定到正在执行的节点**——节点完全不必知道自己的 node_id。附带红利：hook 内调用 `model_management.throw_exception_if_processing_interrupted()`，所以"报了进度的循环"天然变成可被 UI 取消的循环。`ProgressBar` 自带节流（100ms / 0.5%，首末必发），**但带 preview 的调用总是立即发（绕过节流）→ 绘图须节点侧自行限频**。**实时预览 = `update_absolute(value, total, preview)` 的第三参**，格式 `("JPEG", PIL.Image, max_size)` 三元组（与 `server.py:send_image_with_metadata` 一致，KSampler latent 预览同一通道）：hook → progress state → WebSocket 二进制事件（新客户端 `PREVIEW_IMAGE_WITH_METADATA` 带 JPEG 编码+缩放，旧客户端 `UNENCODED_PREVIEW_IMAGE`）→ 前端按 node_id 渲染在执行节点下方。custom node 零侵入复用；备选路径：`PromptServer.instance.send_sync`（需 hidden `unique_id`，重复造轮子）、返回值 `ui` 字段（静态、节点结束才显示）、新 `BinaryEventTypes`（仅新媒介如视频才值得，需动 server+前端）。协议/核心层（如 `comfy/lm_protocol.py`）**不要 import 任何 UI 机制**，改为暴露可选回调（`progress(done, total)` / `on_token(token_id)`）由节点注入。测试范式：monkeypatch `comfy.utils.ProgressBar` 为记录器断言 total/update/preview 次数，再用 `set_progress_bar_global_hook(hook)` 安装真实 hook 验证末次 `value == total`（见 `cdl_smoke_tests/test_progress_reporting.py`，19 PASS）。
21. **aiohttp 路由参数不解码 %20（2026-09-20 模板面板返修实测）**：动态路由 `{path:.*}` 与 `web.static` 的 match info 都只解码 `%2F`/`%25`（`web_urldispatcher._unquote_path_safe`），空格/非 ASCII 到 handler 时仍是 `%xx` 字面量。链路「前端拼 URL → aiohttp 路由 → 文件查找」三条铁律：①文件名只用 `[a-zA-Z0-9._-]`（官方模板 schema 禁空格正是此因）；②handler 必须 `urllib.parse.unquote`；③测试必须经真实路由层（纯函数测试漏检此类 bug——aiohttp TestServer 集成测试范例见 `cdl_smoke_tests/test_templates_catalog.py` T9）。前端 `fetchTemplateJson` 对 404 是 `.json()` 抛异常后**静默 return false**，表现为点击无反应。
22. **模板分类 `moduleName` 必须 `"default"`（2026-10-04 修复两个入口点击无反应）**：前端把分类的 `moduleName` 原样拷进每张卡片的 `sourceModule`，`fetchTemplateJson` / `getTemplateThumbnailUrl` **只认 `moduleName === "default"`** 走核心通道 `/templates/<name>.json`（缩略图 `/templates/<name>-1.jpg`，`-1` 后缀也只在 default 时追加）；任何其他值都路由到 `/api/workflow_templates/<moduleName>/<name>.json`（custom_nodes 通道，内置 overlay 恒 404）→ 点击静默失败、缩略图也失效。`moduleName` 不影响侧栏可见性（可见性由 `isEssential` 决定），故卡片"看得见点不动"。回归测试 `test_templates_catalog.py` T3b2。前端逆向要点：`settingStore-KkBYyEnh.js` 中 `loadWorkflowTemplate` / `fetchTemplateJson` / `getTemplateThumbnailUrl` 三处硬编码 `t==="default"` 分支；`api-D_pCbeK9.js` 的 `fileURL(e)=api_base+e`、`apiURL(e)` 会补 `/api` 前缀。
23. **aiohttp `web.Application` 默认 `client_max_size=1MB`（2026-10-04 profiling 路由测试踩坑）**：大载荷（如 1MB 语料的 prompt，JSON 转义后 ~1.2MB）在裸 `web.Application()` 的 TestServer 上 4xx——生产端 `server.py` 用 `args.max_upload_size` 构造不受影响。**给带大 payload 的路由写集成测试时，TestServer 的 Application 必须带 `client_max_size=100*1024**2`（镜像生产）**，否则误判路由坏。另：TestServer 启动后 router 冻结，`add_static` 必须在 `TestClient(TestServer(app))` 之前完成。
24. **前端 chunk 逆向的字符串围栏（2026-10-04 profiling 前置调查）**：压缩代码用反引号/单引号字符串，`findstr /c:"..."` 双引号模式搜不到 → 用 venv Python 正则扫 chunk 更可靠。官方文档（`ComfyUI_Docs_Work/custom-nodes/js/javascript_*.mdx`）确认的关键契约：①侧栏 tab = `{id, icon, title, tooltip?, type:"custom", render(el)}`，`render` 可返回清理函数；②图变更 = `api.addEventListener("graphChanged", cb)`（官方示例用法）；③`execution_error` 载荷在 `event.detail`（node_id/node_type/exception_message/exception_type/traceback）；④顶栏按钮 = `new ComfyButton({icon, action, tooltip, content}).element` + `new ComfyButtonGroup(el)`，`app.menu.settingsGroup.element.before(group.element)`（Manager 实证）。本机无 node，JS 语法自查用「剥离字符串/注释后括号配平」的 Python 脚本兜底（注意正则字面量含引号会误报，esc() 用 split/join 避开）。
25. **`web.RouteTableDef` 是 `Sequence`，没有 `extend()`；而且"服务构造层"代码冒烟器测不到（2026-10-04 profiling 路由挂载把服务打崩）**：把另一个 RouteTableDef 并进来只能**逐条重新声明**——`for r in other: if isinstance(r, web.RouteDef): routes.route(r.method, r.path)(r.handler, **r.kwargs)`（与 `server.py:1241` 给 `/api` 孪生再声明一次是同款手法）。教训：`routes.extend(...)` 写在 `PromptServer.__init__` 里，而集成测试（aiohttp TestServer 自建 Application）与冒烟器（**不启动服务**）都不会构造 `PromptServer` → 只有真实 `python main.py` 才炸。**凡改动 server 构造/启动路径，必须真启动一次验证**（或至少按同样合并路径最小复现一次，如临时脚本 + TestServer）。
26. **训练节点在宿主 inference_mode 下的完整纪律（2026-10-05 两个回归模板实跑双炸）**：宿主 `execution.py` 把整轮 prompt 包在 `torch.inference_mode()` 里；`inference_mode(False)` **只恢复块内新建代码的 autograd**——①上游传来的张量仍是 inference tensor（不能就地更新、不能存 backward）；②**块外创建的张量/模型参数也是 inference tensor**。CdlLinRegTrain 的 `w -= lr*w.grad`（w 建在块外）与 CdlRegressionTrain 的 `loss_fn(pred, yb)`（yb 是上游 inference tensor）双双炸。修复 = **单一 `inference_mode(False)` 作用域包住"数据物化(`.clone()`)→标准化统计→模型创建→训练→评估"全过程**（step6 规矩的完整版——不止包住循环：输入必须 clone 进作用域、参数必须在作用域内创建）。冒烟器不带宿主 wrapper 测不到 → `test_template_workflows.py` 执行器已改为在 `torch.inference_mode()` 下执行整图（宿主保真）。
27. **前端 seed 控件的隐形 widget（2026-10-05 模板 min_delta 变空串）**：前端 bundle（`core-*.js` 实证 `(t===\`seed\`||t===\`noise_seed\`)`）会给名为 `seed`/`noise_seed` 的 INT widget **自动追加 `control_after_generate` 控件**，其值占 `widgets_values` 一个槽位——手写模板 JSON 时 seed 后必须补一个值（惯例 `"fixed"`），否则 seed 之后所有控件值整体错位一格（B 模板的 `30` 被吃进 control、`early_stop_min_delta` 落空变 `''` → 后端校验报"无法转 FLOAT"）。`test_template_workflows.py` 的 `_widget_order` 已镜像此规则。用户手里已加载的旧图需**重新从模板打开**才能对齐。
28. **全注册表真实执行采样必须防 UI/网络副作用节点（2026-10-07 采样器事故）**：`CdlMessageBox`（ctypes MessageBoxW，默认 block(wait)）被离线采样器以默认参数真实执行 → 用户桌面被原生对话框轰炸、采样卡死无产出。防线：①类名黑名单（messagebox/msgbox/dialog/popup/download/upload，大小写不敏感）②每节点 30s 守护线程超时（超时放弃为 REVIEW，daemon 线程不阻塞进程）③沙箱 chdir 兜底写盘。**凡"遍历注册表逐个真实执行"的工具（采样器/冒烟器/批量工具）上线前必查副作用类**。
29. **V3 节点的 `INPUT_TYPES()` 兼容格式（2026-10-07 采样器）**：V3（io.ComfyNode）经宿主兼容层返回 **list 而非 tuple**（`["STRING", {...}]`），combo 记作 **"COMBO"** 且选项在 `meta["options"]`——解析 INPUT_TYPES 的工具（采样器/模板生成器）必须 tuple/list 两种都吃。另：`census_for` 类查询中**空列表是合法结果**（纯 Python 叶子节点 0 个 ATen 调用），不得与"无规则"混同；JS 里含引号/花括号的 regex 会破坏 T4a 括号配平 linter（stripper 不剥 regex）——一律 split/join（坑 24 延伸）。

## 运行 / 验证
- **双环境规则（2026-10-04 新增，权威源 `.codebuddy/rules/Project_Location.mdc`）**：
  ① **P 盘 CPU 主开发**：Py3.14 embedded、`penv`、torch 2.13.0+cpu + torchvision 0.28；
  **开发代码不得用 Python 3.12 以上语法**（保 GPU 版兼容）；
  ② **F 盘 GPU（`F:\Dev\ComfyUI_Refs\`，2TB 移动 SSD）**：系统 Py3.12（`py -3.12`）、
  `ComfyDL_UI\penv_new\`、torch/torchvision/torchaudio = 2.11.0/0.26.0/2.11.0+cu130、
  CUDA 13.1、RTX 5060；F 盘整体为 robocopy 完整拷贝（含 ComfyDL_releases）。
  ③ **冒烟测试须双环境各跑一遍**；④ **P 盘提交后 → F 盘 `git fetch` + `git switch
  <分支>` 同步**；⑤ F 盘离线/异常时在 P 盘照常开发 + 记忆在案，等环境回归再补
  （详见规则原文「环境异常处理」）。
- venv `ComfyDL_UI\penv\Scripts\python.exe`（Py3.14）优先；兜底 `P:\python.ps1`。P 盘无 GPU（自动 `--cpu`）；多实例抢 `user/comfyui.db` 用 `--database-url` 隔离。
- **requirements.txt 口径（2026-10-04 `a2aed42` 重写）**：目标环境 = 双环境；恢复的
  可脱水依赖（transformers/tokenizers/sentencepiece/torchsde/PyOpenGL/comfy-angle/
  spandrel/yarl）均带 `[dehydratable]` 标记与用途注释；注释全英文；**cu130 的三份
  whl 留在仓库根目录但不 track（`*.whl` 进 .gitignore，`/penv_new/` 同）**。
- **冒烟器**（不启动服务）：`penv\Scripts\python.exe cdl_smoke_tests\run_smoke_test.py [--filter X] [--categories] [--verbose] [--timeout S]`，基线 **290 PASS / 23 SKIP / 0 FAIL（共 313，2026-10-02 step10 后）**。dummy `_DUMMY_OVERRIDES`（记忆旧名；现文件内为 `_INPUT_OVERRIDES`）；值断言 `_OUTPUT_CHECKS`（回调 `(result, args)`）；`_rerun_v3(id, **overrides)` 重跑时**未显式传入的张量输入会重新随机**，断言依赖一致性时须全量传入；`_REGISTRY` 供按 id 重跑；类型工厂 `_VALUE_FACTORIES`；沙箱假 checkpoint + `folder_paths` 临时重定向供 model loader 真执行；训练节点另有 `PARAMS`/`OPTIMIZER` 工厂与成对 `x,y`。
- **端到端自检**：`penv\Scripts\python.exe main.py --cpu --quick-test-for-ci`（本构建**没有** `--quick-test-for-home`），grep `IMPORT FAILED` 应为 0。
- **`comfydl\_update_readme.py`** 重建 README 类别表 + 修 FUNCTIONS 计数（`--check` 只校验）；**不管 README 的 "provides N nodes across M categories" 总句**，需手改。
- 专项 `cdl_smoke_tests/test_conversion_roundtrip.py`；workspace 工具 `.codebuddy/scripts/`（compare_dups / e2e_verify / import_scan / dep_audit / req_audit）。

## 工作方式约定
- **🎉 第 100 次 commit 庆典提交（2026-10-07，✅ 已完成）**：父仓 `31e0c21`（**100 commits**），message/ASCII art 全文存 `2026-10-07-celebration-100.md`；第 99 = `4656a32`（.gitignore 根目录 *.csv）。执行方式：ASCII art 存 UTF-8 临时文件 + `git commit -F`（防 shell 截断/emoji 乱码）。**庆典后暂停 CRAFT，待用户相关工作完成后继续**。
- 先查 `ComfyUI-original/` 与 `Comfy_CustomNode_Docs_md/`，改动只落 `ComfyDL_UI/`。ComfyDL 改动：`comfydl/` 提交 → push main → 主仓 `git add comfydl` 更新 gitlink → push master → 独立 checkout `git pull`。
- `comfydl/assets/example_*.png` 是**用户亲截截图**，勿当临时文件清理。cmd 下 `&&` 会被转成 `;`；推送前 `git pull origin master --rebase`。
- **`comfydl/example_workflows/` 现由模板目录补丁层服务**（2026-09-20 起，`app/template_catalog.py`，见 `docs/branding-and-frontend-patch.md` §7）：旧认知「`app/custom_node_manager.py` 的 `/workflow_templates` 通道 = 模板数据源」**已被证伪**——该通道只扫 `custom_nodes/` 目录与 `load_custom_node` 模块，内置模块（comfydl 经 `init_builtin_dl_nodes` 直 import）两处都不覆盖，恒返回空。新机制：`server.py` legacy 分支在元包无 `templates/` 目录时回退到 `FrontendManager.template_asset_handler()`，请求期改写 `/templates/index*.json`（剔死模板→剔空分类→首位注入 ComfyDL Examples 分类）并从仓库直发示例工作流/封面。顶层已于 2026-09-11 归档清空，现存 2 张 LM 工作流 + `_archived/`。
- 硬规则：临时文件及时清理；改动节点后**跑冒烟（零 FAIL）+ 同步 4 份说明文件**（`FUNCTIONS.md`/`FUNCTIONS_zh.md`/`README.md`/`README_zh.md`，含所有计数）；widgets 默认值"开箱可用"；新节点写详细 docstring。会话信息长期维护在 `.codebuddy/`。
- **未经用户明确要求不得 commit / push**；交付时的完整链路 = 子模块 commit+push → 主仓 `git add comfydl` + commit + `git pull origin master --rebase` + push → 独立 checkout `git pull --ff-only`。
- **视觉/浏览器验收由用户本人做，我只做底层测试**（2026-09-12 立，见「当前待办」）；临时文件及时清理，清理前先 `Stop-Process` 自己遗留的实例进程（**只杀带自己沙箱路径/`.tmp_*` 的**，同期用户的 `msedge` 等绝不能碰）。
- 版本里程碑打 annotated tag 并推送（现有 `v0.2.0-dehydrated`、`v0.3.1`）。
- 临时目录：`.codebuddy/`（记忆/计划/技能，**长期，不要删**）；`p:\Dev\ComfyUI_Refs\.tmp_*\`（一次性验收工作区，用完即删）。

## 示例工作流归档（2026-09-11，细节见 `2026-09-11.md` §17）
- 顶层 9 个 JSON 全部 `git mv` → `_archived/`（共 12，保持原名）；删 `_gen_full_test.py` + 0 字节 `ComfyDL_AllNodes_Generated.json`（`.gitignore` 规则同步移除）。两份 README 移除 6 个截图小节、保留 `The Mysterious "?"`；**`assets/example_*.png` 7 张一律保留**。提交：子模块 `59142ed` → 主仓 gitlink `beedd9d`。
- ⚠ 遗留：`docs/comfydl-builtin.md:146,151` 仍引用 `example_workflows/Image Processing Chain.json`（用户明确不动），路径已失效。

## reform 进度
- 分类树 `Comfy节点 → Network & Layers → {Activation(14), Basic(8), Attention(7), Normalization(7), Regularization(1), Training(17), Pooling(2), Convolution(2), Text(4)}` = 核心 **62**；再加 `model/{loaders(7), merging(11), latent(2), conditioning(2)}` 22 与 `3d/Preview 3D` 1 → 核心共 **85**。
- step4 `nodes_pooling.py`(4 节点→2)/`nodes_convolution.py`(2)：`mode`×`dims` 分派，`output_size=1` 即全局池化；权重走 slot、无 channel 控件；`padding_mode` 用 `F.pad` 手工实现（`F.conv*d` 无该参数）。
- step5 `comfy/model_protocol.py` + `nodes_model_loaders/merging/inference.py`：**state_dict 级** `MODEL`/`CLIP`/`VAE`；L1 真执行 16（5 loader + 7 合并 + 4 保存），L2 占位 6（LoRA×2、VAE 编解码、文本编码/截层）。合并直接做张量运算，**不构造 ModelPatcher 补丁**（否则走 `comfy.lora.calculate_weight` 必炸）。`comfy/` 框架层零改动（仅新增文件）。
- step1 `nodes_activation.py` 14 + 核心类型 **`TENSOR`**（`#C6FF00`，归并 `cdlTensor→TENSOR`、`cdlBbox→BBOX`）；step2 `nodes_layers.py` 8 Basic（无状态、权重走 slot；Dense≡Linear、Residual/Skip≡Add）；step3 `nodes_normalization.py` 7 归一化 + `TrainingMode`/`TrainingRunStats`；step4 `Dropout` 并入该文件；转换节点 `comfydl/nodes/conversion.py` 6 个。文档 `ComfyDL_UI/docs/reform-step*.md`。
- 软归档 8 个重叠 d2l 节点 → `d2l/_Legacy/*`（id 未改）；`SpectralNorm` 默认 1→10；归一化导出批统计量。
- **step6（2026-09-12，已提交 `ad4bc10`）** `comfy/training_protocol.py` + `comfy_extras/nodes_training.py` 9 节点，
  全在 `Network & Layers/Training`：`TrainingParameters` / `TrainingParametersMerge` / `TrainingParametersExtract` /
  `TrainingOptimizer` / `TrainingLoop` / `TrainingSaveParameters` / `TrainingLoadParameters` /
  `TrainingParametersToText` / `TrainingTextToParameters`；新增 `PARAMS` / `OPTIMIZER` 两个 `@comfytype`。
  **⚠ 硬约束**：`execution.py:751` 用 `inference_mode()` 包住整轮 prompt → 前向+反向+`optimizer.step()`
  **必须同在一个节点体内**（`with torch.inference_mode(False):` + 输入 `detach().clone()`），循环只能是节点内的 `steps` 控件。
  文档 `docs/reform-step6-learnable-params-and-optimizer.md`。
- **step7（2026-09-19，两仓分支 `reform-step7-attention`：子模块 `e945f26`、主仓 `600a5c6`，均已推送）**
  `comfy_extras/nodes_attention.py` 4 节点（分类 `Network & Layers/Attention`）：`AttentionMultihead` /
  `AttentionSelf` / `AttentionCross` / `TransformerEncoderBlock`（post-LN 块，LN 仿射可选=不连无仿射，
  FFN 激活 relu/gelu 控件）。共享 `_multi_head_attention()` helper；权重四组走 slot、`mode` 连线、
  本地种子 dropout；布尔 mask 采 SDPA 语义（True=可注意，见踩坑 16）；屏蔽位用 `finfo.min` 防全屏蔽行 NaN；
  显式 softmax 不走 fused SDPA（保证逐位确定性）。`CdlMultiHeadAttention` 软归档。冒烟 `_INPUT_OVERRIDES`
  成套工厂 + 4 个 `_OUTPUT_CHECKS`（对照 `nn.MultiheadAttention`/手工组合）。文档 4 份 + `_update_readme.py`
  `_CAT_DESC` 更新 + `docs/reform-step7-attention.md` + `comfydl-builtin.md` step7 段。**注意：本次交付走
  实验分支而非 master/main，master 尚未合入**。
- **step8（2026-09-19，续在分支 `reform-step7-attention`：子模块 `ecb0421`、主仓 `b43ff40`，均已推送）**
  语言模型端到端流水线 13 节点：`nodes_attention.py` +3（CausalMask/PaddingMask/PositionalEncoding）；
  新文件 `nodes_nlp.py` 4 节点（TextVocabBuild/TextEncode/TextDecode/TextSlidingWindow，新分类
  `Network & Layers/Text`）；新文件 `nodes_lm.py` 6 节点（LanguageModelEmbedding/TransformerBlock/
  Build/Train/Forward/Generate，分类 Training）。新类型 `VOCAB`/`MODELSPEC`/`NNMODEL`（`MODEL` 已被
  ModelPatcher 占用故模型类型叫 NNMODEL）；协议层 `comfy/lm_protocol.py`（**不是 model_protocol**，
  上游撞名，见踩坑 18）只 import torch+training_protocol。**核心架构**：spec 冻结链（EmbeddingSpec 首
  + TransformerBlockSpec×N，纯值可缓存）→ Build 物化 → Train 在 deepcopy 上跑
  `inference_mode(False)` 闭环、复用 `OPTIMIZER` 槽、返回新 eval 模型（缓存安全）；逐位置交叉熵使
  300 步收敛。生成贪心/温度采样均走本地 Generator 确定性。冒烟 +13 `_OUTPUT_CHECKS`（掩码三角性/
  词表确定性/loss 下降/训练不污染输入/同 seed 复现）。文档：4 份说明文件 196/34、
  `docs/reform-step8-language-model.md`、`comfydl-builtin.md` step8 段；locales 无需改（只覆盖
  ComfyDL 109）。~~master 尚未合入~~ → **2026-10-02 核实：step7/8 已合入 master/main**（实验分支
  `reform-step7-attention` 完成历史使命）。
- **step9（2026-10-02 核实已在 master：主仓 `3439bc2`+`1fb05da`+`c049167`+`d731120`，子模块
  `3eecbe1`，均已推送）** 训练闭环补全：新类型 `SCHEDULER`（frozen `SchedulerConfig` 走连线、
  训练节点用自己的 optimizer 物化真正 scheduler，`t_max=0`=跟随训练器步数）+ 4 新节点
  （LR Scheduler / Loss / Metrics / Evaluate，分类 Training）；两个训练节点加 `scheduler` 可选输入、
  梯度裁剪（进 `OPTIMIZER` 载荷，默认 0=关）与早停回滚（滑动窗 8，回滚到最佳点）。纯函数层
  `comfy/training_metrics.py`（loss 6 种 / metric 6 种，只依赖 torch）。未连线/未开开关时行为与
  step8 逐字一致。`c049167` 顺带修了 `gen_locales` 标题正则（限定单行，消除跨小节吞并）。
  基线更新：**冒烟 283 PASS / 23 SKIP / 0 FAIL（306 节点/46 分类）；FUNCTIONS 口径 200/34
  = 109 ComfyDL + 91 core**。文档 `docs/reform-step9-training-loop-completion.md`。
- **step10（2026-10-02 交付，主仓 `e81cd89`，子模块 `38e124a`，均已推送）** 循环/解码器/上采样
  三族 7 节点：① 新文件 `comfy_extras/nodes_recurrent.py` + 新分类 `Network & Layers/Recurrent`
  ——RNN/LSTM/GRU 显式 cell 数学（GRU 重置门只缩放隐侧，故输入/隐侧分开 F.linear）、权重/偏置/
  初始状态全走可选 slot（`nn.RNNBase` 布局、两 bias 相加、门行序 [i,f,g,o]/[r,z,n]）、不连即零
  （只连 x 即可运行）、batch_first、单层（堆叠= y→x，续写= hn/cn→h0/c0）、唯一控件 hidden_size=16；
  ② `TransformerDecoderBlock`（Attention 分类）：Self-Attn→Add→LN→Cross-Attn(k/v 来自 context)→
  Add→LN→FFN→Add→LN，post-LN，两组注意力权重+FFN 走 slot，self_mask/cross_mask（SDPA 布尔语义，
  接 CausalMask 即自回归）；③ 上采样族并入 Convolution：Upsample（F.interpolate，mode 必须匹配
  dims，秩精确校验防静默重释）+ PixelShuffle/PixelUnshuffle（纯重排，同 r 无损往返）。软归档
  `CdlRNNScratch/CdlRNN/CdlGRU`→`d2l/_Legacy/NLP Models`（id 不变，RNNLM* 包装族保留）。
  冒烟 290 PASS/23 SKIP/0 FAIL（313 节点/47 分类）；FUNCTIONS 口径 207/35 = 109 + 98 core；
  d2l/_Legacy/NLP Models 现 7 个（软归档累计 11）。文档 `docs/reform-step10-recurrent-decoder-
  upsampling.md` + `comfydl-builtin.md` step10 段（step9 无 builtin 段，其计数仅在其设计文档）。
- **step11（2026-10-02 交付，主仓 `8f2c829`，子模块 `6bbdffd`，均已推送）实时预览**：零新节点，
  为三个长耗时节点在官方 preview 通道上加实时图。新纯模块 `comfy/loss_preview.py`：
  `LossCurvePreviewer`（每步 `record(loss, force)`，内部限频 0.5s，末步 force 必渲染完整曲线；
  matplotlib Agg 惰性导入 + PIL ImageDraw 兜底，**所有失败路径降级为 None，预览绝不打断训练**）+
  `render_text_snapshot`（已生成文本卡片）。接入：TrainingLoop / LanguageModelTrain 推实时 loss
  曲线；LanguageModelGenerate 推文本卡片（未接 vocab 显示原始下标）——`lm_protocol.generate_tokens`
  新增可选 `on_token(token_id)` 姊妹回调（`progress` 原样保留），节点改由 on_token 单通道驱动进度条。
  **契约严格保持：每步/每 token 恰好一次 pbar 调用**（帧附在该步本就发的那次调用上，绝不额外多发；
  初版独立 final-frame 更新曾使计数超契约，实测 FAIL 后改为末步 force 折入）。专项
  `test_progress_reporting.py` 11→**19 PASS**（T1/T2/T3 加 previews≥1 断言、T3 改带 Vocab、新增 T7
  模块单测）；全量冒烟不变 290/23/0（313/47）。README 两份无需改（分类级概览、计数不变）。
  文档 `docs/reform-step11-live-preview.md` + `comfydl-builtin.md` step11 段 + Live Progress 段更新。

## heatmap 升级（2026-09-11，细节见 `2026-09-11-heatmap-build.md`）
- 两孪生节点（`comfydl/nodes/visualization.py`）共用模块级 helper；`HeatmapSpecError` 承载报错策略。6 新控件**只能追加在 `titles` 之后**（旧工作流按索引映射）：`max_samples`=262144(0=不抽样) / `dims`=auto / `axis_reduce`=mean / `auto_select`=last_n / `auto_n`=0 / `on_error`=fallback_first_n；`titles` 语义改为"单张图标题"。
- 抽样 = 纯 stride；渲染 1D 条码 / 2D 平面 / 3D 正交半透明立方体。`CdlHeatmapsTo3D`（`comfydl/nodes/mesh3d.py`）+ `comfy_extras/nodes_preview_3d.py::Preview3D`。手写 OBJ：颜色 64 级量化、**绕序必须朝外**、`save_to` 覆写同写 `.mtl` + 首行 `mtllib`。

## 已知限制 / 候选
- ~~可学习参数对象 + 优化器~~（step6 已落地，见 reform 进度）；"成品工作流粒度" EX 节点；性能优化 / profiling；UI 改造。
- ~~品牌化/前端补丁层已交付（tag `v0.3.1`），仅剩用户本人的界面自验收~~ → **2026-10-02 验收完成**（效果一般般，UI 可再优化，非紧急）；`OPTIMIZER` 只承载超参，**不跨 prompt 保存 Adam 动量**（无状态可表达，文档已列为非目标）。
- `TENSOR` 槽颜色不再随前端包升级丢失（改由 `app/frontend_patch.py` 自动重放，见踩坑 7）。
- 用户已多次表示「我自己一眼就能看」，**不要再把 token 花在浏览器验收上**。

## 数据 / 回归工具链批次（2026-10-05 起，生产化第一阶段）
- **批次 1-4 已交付、已提交、已双端推送（2026-10-05）**：补齐"真实数据进出 + 回归训练"两大能力，efficiency-first 生产定位。
  - **批次1 DATASET 类型**：`comfydl/nodes/__init__.py` 加 `DATASET` 常量；`data_types.py` 的 `CdlDataset` dataclass（features/labels/feature_names/target_name/meta，零拷贝引用，from_dataframe/to_dataframe）；`frontend_patch.py` 新增 `_patch_dataset_slot_colour`（teal `#1ABC9C`，6 套主题）；`dataset_adapters.py` 三适配器 `CdlTensorsToDataset`/`CdlDatasetToTensors`/`CdlDatasetToLoader`。专项 `test_data_types.py` 18/18。
  - **批次2 读取节点**：`data_io.py` 7 读（`CdlReadText/String/CSV/JSON/XLSX/DB/AccDB`），`requirements.txt` 加 pandas+openpyxl，可选依赖惰性 import、缺失抛可读 pip 提示；pandas usecols/dtype + `torch.from_numpy` 零拷贝；冒烟跳过 `CdlReadXLSX`/`CdlReadAccDB`。
  - **批次3 导出节点**：`data_io_writers.py` 5 写（`CdlWriteCSV/JSON/XLSX/DB`）+ `CdlDatasetPreview`，向量化、大文件分块（DB `chunksize=1000`）、预览 STRING 输出。
  - **批次4 细粒度回归 `CdlLinRegTrain`**：`linreg_train.py`，w/b/loss_history/y_hat 四端口，mini-batch SGD 均方损失，吃 X/y TENSOR 或 DATASET；与 `CdlLinReg`+`CdlSquaredLoss` 组成验证闭环。主冒烟新增 loss 递减断言；专项 `test_regression.py` 18/18。
  - **文档**：`comfydl/FUNCTIONS.md`/`FUNCTIONS_zh.md`/`README.md`/`README_zh.md` 经 `_update_readme.py` 同步，新增 `d2l/Training` 分类（125 ComfyDL 节点 / 21 分类）。
  - **提交**：子模块 `comfydl` main `ea2bf1c`、父仓 `ComfyDL_UI` master `0253cd2`，均已 push origin；F 盘 fetch+reset --hard+submodule update 同步，**双端全量冒烟 298 PASS / 33 SKIP / 0 FAIL 无回退**。
  - **实验分支**：`exp/data-regress-1-4`（子模块 + 父仓）已推送，作为本阶段成果检查点（master/main 已含同等内容）；详见 `2026-10-05.md`。
- **批次5 已交付（粗粒度生产回归 `CdlRegressionTrain`，2026-10-05）**：吃 DATASET(可选)/X·y TENSOR(回退)，一体化 切分+标准化(z-score 内嵌模型)+mini-batch Adam+早停(验证损失滑动均值+回滚最优)+评估，输出 nn_model/mae/rmse/predictions/loss_history，可存盘。设计上**等价复用** TrainingLoop 的已验证机制（早停+回滚、隐藏层 MLP）但用纯 torch 自包含实现，保持 comfydl 与 rehydration `comfy` 核心解耦（那些 Training* 是 V3 io.ComfyNode，耦合会破坏独立单测）。标准化内嵌 `_Regressor` buffer，故 nn_model 自包含、可直接进 `CdlNNForward` 或存盘重载。双端全量冒烟 299 PASS/33 SKIP/0 FAIL，专项 test_regression.py 32/32；节点计数 126/21。提交：comfydl main `2b919c7`、父仓 master `25c9b3e`，均已推送；F 同步+GPU 冒烟通过。详见 `2026-10-05.md`。
- **批次6 已交付（`CdlFormulaDataGen` + 两个回归模板工作流，2026-10-05，待用户界面验收）**：新节点 = 公式(AST 白名单向量化求值)/模型(数字孪生 y=model(X)+noise) 双模式数据生成器，输出 X/y/DATASET，节点计数 **127**。新模板 `linear_regression_from_scratch`（教科书闭环+验证）与 `tabular_regression_production`（生产管线+CSV 导出，实测调参 lr=0.05/min_delta=0 贴噪声地板 mae 0.079），已注册 `_COMFYDL_CATEGORY` + zh 覆盖，封面取 Wikimedia 占位待用户亲截。**手写模板三坑**：required 槽留空会被 prompt 校验拒、widgets_values 须按 INPUT_TYPES 序（combo 两种写法/widget 转槽位后剔除）、SaveText prefix 不带扩展名。新验证工具 `test_template_workflows.py`（迷你执行器真跑模板图，10 PASS）；T3d 因 rehydration 复活官方 Image 分类改白名单 `{Node Basics, Image}`。双端全量 **300/33/0**，data-gen 专项 28/28，模板专项 57/57。提交：comfydl `3983222`、父仓 `d3e30b1`。详见 `2026-10-05.md`。

## 日记索引
`2026-10-31-dual-track-bigplan.md`（**双主线能力大计划草案（待审）**：50 仓库调研 + 7 簇专项档案
汇聚而成；能力簇×门面矩阵、开销成本位置模型与元 profiling 铁律、架构级冲突裁决 ADR-D2~D5
——含重大事实修正：crash site 2026-10-13 后正常路径已 lazy capture（should_cache 恒 False），
"开销翻倍"感受需 SP-0 归因实测分离 profiling 嫌疑；七簇全收草案 + 里程碑 M-A~M-D 备案 +
审阅清单 5 问）、`2026-10-31-dual-track-smallplans.md`（**小计划集与推进计划**：SP-0 归因实测
→ SP-1 P0 基建（观测总线/签名缓存/开关注册表）→ SP-2/3 C1 合批 → SP-4 量化最小版（格式描述符
+ estimators 字节率硬捆绑）→ SP-5 实验台（runs 三层 schema）→ SP-6/7 数据簇+交互 UI 合批；
调研就位度六簇齐、C5 待 SP-0；合批原则与依赖图）、`2026-10-31.md`（**Refs 调研收官日**：
50 仓库克隆+深度调研落盘 refs_survey/ 5 报告+INDEX；项目代码/ComfyUI-original/离线文档三路
深读；双主线理论讨论（开销画像→能力簇重构→大计划骨架）+ 7 簇专项调研全回收）、
`2026-10-30-cdlapp-bigplan.md`（**CDL App Mode 改造大计划（立项待审）**：三要素=输入/输出/进度
平权、输出=文件下载+可复制数据+低算力 preview、详细进度+错误卡"死的明白"、App→节点图与
builder 选择器入口；ADR×7（中央不动+左侧浮层、输入用官方面板、artifacts 下载路由 path jail、
进度/错误走 progress_state/execution_error、builder store 入口、独立开关）；Phase P0-P3 共
4 craft；逆向 v2 固化：coach-id 锚点 data-coach-id、z-index 安全带 2001-9999、Actionbar 硬编码、
enterBuilder@settingStore 208:95489）、`2026-10-30-appmode-recon.md`（**ComfyUI App mode（Beta）侦察档案，下一阶段储备**：内部代号
linear mode；契约 = workflow extra.linearData/linearMode + `.app.json` 后缀 + userdata；命令
Comfy.ToggleLinear、侧栏 tab id `apps`；builder 三步 useAppMode 状态机 + 强制 Nodes 2.0；
Run 走普通 /prompt，`/jobs*` 结果面板 Comfy Cloud 专属（本地后端无此路由，降级）；blueprints
为独立特性且已在我们构建中删除；可复制点 = 模板目录预置 linearData 让教学模板开箱即 App）、
`2026-10-30.md`（**环境回归日**：F 盘在线并同步 `fab995e`/`e60e7bc`，GPU 冒烟 306/33/0 欠账全清；
启动工作流 tabular→classification_from_scratch + 烘焙 URL 代际升级 helper；settings 注册
竞速白屏根治——官方 load() 断言 `Setting values must be loaded...`，三模块 gate
`extensionManager.setting.isReady`；模板 extra.ds 规范化 0.72 自适应 + offset[100,140]；
test_frontend_patch 24→33/0）、`2026-10-28-classification-line-plan.md`（**分类对称线正式大计划（定稿未动工）**：对标回归线五件套双形态交付，ADR×8、Phase P0-P4、测试矩阵、风险表）、`2026-10-28.md`（**环境变更 + homepage 补记 + 双形态铁律 + 分类线草稿**：F 盘离线、嵌入式 Python 移至 P:\Software\python-3.14.5-embed-amd64、homepage 七连提回读（mock 后端/base-path 敏感/bake_patches 与 frontend_patch 两套补丁）、仓库基线 0a278d5/b46f062 双干净））、`2026-10-07.md`（**Master switch：profiling 完全停用/一键恢复（父仓 `7360f63`，101 commits）**——动机=profiling 存在必然拖慢 Run（前端 graphToPrompt 劫持+每 run estimate、watchdog 250ms 恒采样、meter 记账）；Settings `ComfyDL.Profiling.Enabled`（默认 on）热切换：前端保守恢复 graphToPrompt（`app.graphToPrompt === _patchedGTP` 才恢复）+ 四监听命名化可卸 + 徽章/胶囊隐藏 + 两面板空态（图标+"Profiling currently disabled"+Enable 按钮，侧拉/叠加切换保留）；后端 `GET/POST /comfydl/profiling/enabled` + watchdog stop/start + comfy 层 master flag（proflog.py，app 写 comfy 读）短路 meter_from_args/persist_run（executor 零改动）；硬关 `--cdl-profiling-disable`（409 拒改回、空态无按钮）；log 108/108、全量 301/33/0 双盘、真启动验证；过程坑：here-string 传长中文被拒→GUI 工具追加、settings 无 category 是既有约定）、`2026-10-07-celebration-100.md`（**🎉 第 100 次 commit 庆典存档**：父仓 `31e0c21`，main.py 顶部纪念注释 + message/ASCII art 全文原文存档（防覆盖）；第 99 = `4656a32` .gitignore 忽略根目录 *.csv 运行产物；执行教训：多行/emoji 文本用 UTF-8 临时文件 + `git commit -F` 引用，绝不经 shell 传参；庆典后按约定暂停 CRAFT 待用户指令）、`2026-10-06-profiling-v2-bigplan.md`（**Profiling v2 大计划——正式版（制作中，见顶部阶段声明）**：3 个 subagent 前置调研已回收并固化为 ADR×6——①opgraph 离线化：`_probe_one/_ProbeRecorder/_unwrap_outputs/_bootstrap_registry` 直接复用，`engine._topological_order`+`EstimationCtx` 是与执行解耦的现成件，规则库抄 `@register` 机制约 80%；②run 期 FLOPs：挂点 `execute_async` 进出进程级轻量 TorchDispatchMode，归属走 `get_executing_context()` contextvar 每次 dispatch 分桶（不按 mode 生命周期），DB 走 SQLAlchemy+Alembic 0007，口径=fwd+bwd 合计且 optimizer foreach 漏计需声明；③Manager 分区范式：三列 flex+`pf-` 前缀+宿主 CSS 变量+单例 display 切换+flyover 抽屉+z-index 分层。**关键事实修正：opgraph 的 FakeTensorMode 只 import 从未使用，探针始终真实执行真实张量**。P0 止血（护栏+危险模式+超时）→P1 规则库/离线采样/零执行组装器/mermaid+覆盖率报表 →P2 实测回流/校准 →P3 三列分区/导出矩阵；各 Phase 验收与风险表已固化）、`2026-10-06.md`（**§12 P3 交付（v2 收官 Phase）**：三列分区 overlay——卡片 data-col 标记 + overlay 侧 appendChild DOM 移动（280px/1fr/320px grid，窄屏降级；renderOverlay 只切 class 架构未动）；导出矩阵补 SVG/PNG（canvas 2x 光栅化）/mermaid.live（复制+打开，无 pako）；T4aa-T4am ×13 防回归，log 78/78、全量 301/33/0 双盘；**P0-P3 四 Phase 全部交付，待用户验收后可宣告 Profiling v2 大计划收工**；父仓 `cfe446d`，97 commits——距庆典还剩 3 次）、**启动默认工作流替换**：前端 `settingStore` chunk 的 `loadDefaultWorkflow` 程序化构建 SD 文生图图（非模板系统），`frontend_patch._patch_default_workflow` 重写为 fetch `/templates/tabular_regression_production.json`（overlay 通道，失败回退上游），marker 幂等；专项 test_frontend_patch 15/15，父仓 `d0ef6e3`；**Profiling M3 opgraph 交付**：FakeTensor+FlopCounterMode+自定义 recorder（最外层拦截 `_local_scalar_dense` 返回常量）逐节点 ATen 探针，手动 Analyze 按钮 + 侧拉/叠加双显示模式 + per-node op census；`POST /comfydl/profiling/opgraph`（version 1，与 engine REPORT_VERSION=2 独立）；踩坑：`init_extra_nodes` 是 async、宿主 nodes 模块级 utilities 使判空失效、受限 eval `__builtins__:{}` 下 `标量*fake` 炸 `__import__`（data_gen 已修）、多节点须共用同一 FakeTensorMode 会话、`get_flop_counts()` 双层 dict；test_profiling_opgraph 20/20、全量 301/33/0 双端、真启动验证；子模块 `cec0590`、父仓 `8c4b464`；**同日追加：Profiling 四档日志 + 面板双 bug 修复**——`--cdl-profiling-log {off,low,medium,high}`（默认 off）+ `comfy/profiling/proflog.py`（stdlib-only，请求头 `X-CDL-Profiling-Log` 只升不降）+ 前端 Settings combo + 面板「服务端日志」卡（`/internal/logs/raw`）；**根因坐实：`renderOverlay()` 被调 4 处却从未定义**——Analyze 卡死（首渲染在 try 外、loading 置位后 fetch 前抛 ReferenceError）与 Expand 无反应（overlay class 加不上）同根因；修复 = 补定义 + requestOpgraph 全包 try/finally；test_profiling_log.py 36/36（T4 静态防回归）、全量 301/33/0 双盘、真启动验证 EXIT=0；父仓 `49334b3`。**同日二轮（用户实测回收）**：high 档日志照出 LM 模板 Analyze 仅 2/12 probed——根因 = V3 节点返回 `io.NodeOutput` 包装、探针未像宿主一样拆 `.args`/`.expand` → 下游类型检查炸并沿链级联 fallback；`_unwrap_outputs()` 修复后 LM 模板 11/12（探针会真实执行节点，LanguageModelSave 真写盘——副作用别当真实结果）；用户反馈启动自动展开 overlay"跳脸"→ displayMode 恒 sidebar 起且不再持久化；opgraph 22/22、log 38/38；父仓 `7b3ed45`）、`2026-10-04-profiling-m1.md`（**Profiling Tools M1 交付**：估算引擎+路由+前端扩展+loader 注入 37 项测试全绿、全量冒烟 290/23/0 不回退、踩坑 23/24 入档、待用户验收后提交）、`2026-10-04-profiling-plan.md`（**Profiling Tools 立项：Memory Size 分析可行性方案**——OOM golden case 根因定位、四层架构、假设层、三色阈值、风险、分期，未动工）、`2026-10-04.md`（**修复模板入口点击无反应并交付**：根因=注入分类 `moduleName:"ComfyDL"`≠`"default"`，前端卡片 fetch 被路由到 custom_nodes 通道恒 404 静默失败；修复=改回 `"default"`，JSON+缩略图双通道复活；T3b2 回归测试、专项 43/43、全量冒烟 290/23/0 不变；用户验收通过，主仓 `e0fb9c6` 已推送；踩坑 22 入档；另记 PowerShell 下 cmd /c 嵌套引号不稳定、改用 `& 'git.exe 全路径' -C <repo>` 的经验）、`2026-10-02.md`（**验收成果四项入档**：v0.3.1 验收完成关闭待办、LM 工作流实测可跑真小型语言模型、Templates→ComfyDL→"Train and Chat"入口坏（暂不修复在案）、新工作流推迟；**L2 六占位重要性评估**：非至关重要，推荐教学级 LoRA 方案 A / 回捞 SD 整链方案 B 待用户选；另核实 step7/8/9 已合 master、基线 283/23/0 与 200/34；**step10 当日交付**：RNN/LSTM/GRU 显式 cell + TransformerDecoderBlock + Upsample/PixelShuffle/PixelUnshuffle 共 7 节点 + d2l RNN/GRU builder 软归档，主仓 `e81cd89` / 子模块 `38e124a` 已推送，基线 290/23/0（313/47）与 207/35，详见正文 step10 段；**step11 同日交付：实时预览**——KSampler preview 通道零侵入复用（`update_absolute` 第三参三元组），`comfy/loss_preview.py` 曲线/文本卡片渲染（限频+末步 force+全失败降级），三节点接入，每步恰好一次 pbar 调用契约保持，专项 19 PASS，主仓 `8f2c829` / 子模块 `6bbdffd` 已推送，详见正文 §三）、`2026-09-20.md`（**模板面板返修 + 节点实时进度报告**：前端拼 URL 不做编码 → aiohttp 只解 `%2F`/`%25` → `%20` 404 → 点击无反应，故重命名文件为 URL-safe + handler `unquote`；`comfy.utils.ProgressBar` 经 `get_executing_context` 自动绑定节点且顺带中断检查，4 个长循环节点接入 + 专项测试 11 PASS；**该两项工作后续已在 master 提交：`e67424f`（重命名）与 `c9b575f`（进度报告，2026-10-02 核实）**）、`2026-09-19.md`（reform step7 注意力族 4 节点 + `TransformerEncoderBlock`、布尔掩码语义踩坑、extras 绝对导入、实验分支交付；**同日 reform step8 语言模型流水线 13 节点**：spec 链协议、文件撞名两次踩雷、277/23/0 基线）、`2026-09-12.md`（**Part A** reform step6 训练闭环；**Part B** 品牌化 `ComfyDL_UI v0.3.1` + `frontend_patch.py` 补丁层 + 语言包；**Part C** 工具环境经验：git 全路径、`$` 被吞、大仓搜索超时、playwright 验收手法；**Part D** 交付记录：两仓提交+推送+tag `v0.3.1`+独立 checkout 同步+临时文件清理，**§D4 视觉验收改由用户做的铁律**、§D5 用户自验收清单）、`2026-09-11.md`（step1–3、§17 工作流归档、**§18 reform step4/step5 + 计数口径更新**、技能安装）、`2026-09-11-heatmap-build.md`、`2026-09-11-heatmap-next.md`、`2026-09-11-legacy-archive-and-stats.md`、`2026-09-11-conversion-nodes.md`、`2026-09-11-dropout-node.md`、`2026-09-10.md`（reform 战略原文）、`2026-09-09.md`（脱水收尾 + 内置化）、`2026-08-30.md`（盘点）、`2026-10-05.md`（**阶段性成果保存**：DATASET 通用类型 + 数据 I/O 读取/导出节点 + CdlLinRegTrain 细粒度回归四端口节点（批次 1-4 交付），双端全量冒烟 298/33/0 无回退，实验分支 exp/data-regress-1-4 已推送；批次 5 粗粒度 CdlRegressionTrain 已交付（双端 299/33/0，提交 2b919c7/25c9b3e））。
