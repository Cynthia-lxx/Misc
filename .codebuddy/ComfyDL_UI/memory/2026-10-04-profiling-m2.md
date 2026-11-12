# 2026-10-04 · Profiling Tools M2 交付（计算量账本 + 实测耗时 + 瞬爆监控）

> 立项讨论见同日会话（TFLOPs 设想 → 生产定位重估 → W 瞬爆需求追加 → 范围拍板）。
> 设计文档 `docs/profiling-m2-compute-and-watchdog.md`；本文记录过程、决策与验证。
> 状态：**代码全部交付，专项 26 项 + M1 37 项全绿、全量冒烟 290/23/0 不回退，
> 真启动验证通过；未提交**（铁律：等用户界面验收通过后再 commit/push + F 盘同步）。

## 一、范围（用户拍板）

- M2 = **F1（逐节点实测耗时）+ F2（静态 FLOPs）+ W2（watchdog 瞬爆监控）**最小闭环；
- W1（静态地雷扫描）/ W3（优化建议库）/ F3-F5（MFU、roofline、时间预警）→ M3；
- watchdog 前端**执行中实时显示**（顶栏胶囊：当前节点 + CPU%/MEM%）。

## 二、交付物（10 文件，零节点行为改动；新增 2 文件）

| 文件 | 改动 |
|---|---|
| `comfy/profiling/formulas.py` | 新增计算账本：`FlopsItem`（kind: attn/ffn/logits/other，approx 标记）、`lm_forward_flops`（每块 attn=8BTE²+4BT²E、ffn=4BTEF，logits=2BTEV）、`lm_training_flops`（3×前向×steps）、`lm_generate_flops`（无 KV cache：逐步长求和，>10⁶ 步退化上界）、`mlp_forward/training_flops`、`six_nd_estimate`（6ND 交叉验证） |
| `comfy/profiling/estimators.py` | `NodeEstimate` 加 `flops_items/flops_status/flops_reason`（estimated/zero/unknown 三态；`as_unknown` 连带 flops unknown）；四个算力 estimator 填账，纯 Python 循环节点（VocabBuild/Encode/SlidingWindow）记 unknown，直通记 zero |
| `comfy/profiling/engine.py` | `REPORT_VERSION=2`；节点记录加 flops 三字段；报告加 `flops` 节（total/by_kind/largest/any_estimated）——**内存是峰值、计算量是可加**，文档写明口径 |
| `app/profiling_watchdog.py`（新） | `ProfilingWatchdog`：psutil 250ms 采样线程（进程 per-core 口径 + 系统级 + RSS）、`last_node_id` 归因、瞬爆状态机（≥90% 持续 2s 开启；×0.8 两拍或节点切换关闭；system_wide 标志）、jsonl 案底（写失败退化 64 条环形缓冲）、`snapshot()/recent()`、stop join；env 覆盖 `COMFYDL_WATCHDOG_{CPU,SUSTAIN,INTERVAL,LOG}` |
| `app/profiling_routes.py` | 新增 GET `watchdog/status`、`watchdog/log`；`_LAST_REPORT` 缓存 + `scale_hint_for()`（引擎↔监控握手：案底带 data_scale） |
| `server.py` | import watch模块；`__init__` 中 `last_prompt_id` 初始化 + 构造 watchdog 并 `set_watchdog`（在路由合并后） |
| `execution.py` | `executing` 事件出口补 `server.last_prompt_id = prompt_id` 一行（watchdog 归因用） |
| `main.py` | finally 块 join watchdog（`PromptServer.instance.profiling_watchdog.stop()`，AttributeError 防御） |
| `app/profiling_assets/profiler.js` | F1：executing/executed 时间线（<5ms 标缓存命中不进主榜、对数条形、prompt_id 隔离）；F2：汇总卡（自动单位 + 三色构成条 + 诚实口径文案）+ 节点行"内存 · FLOPs" + 明细表；W2：执行期 1s 轮询 → 顶栏实时胶囊（CPU 绿/黄/红分档）+ 案底红卡；i18n zh/en 补 18 键 |
| `app/profiling_assets/profiler.css` | 计算量卡/构成条、对数刻度耗时条形（300ms 展开动画）、案底红卡（左红描边）、顶栏胶囊（CPU 色档）；全程复用主题变量与既有视觉语言 |
| `cdl_smoke_tests/test_profiling_m2.py`（新） | 26 项：F1a-f 金样例第一性重算/by-kind 自洽/unknown 口径/版本；F2a-b 6ND 区间验证；F3a-c forward/generate（含 vocab 链接覆盖 widget 的坑）；F4a MLP；F5a-b 空图；W1a-W4d 状态机合成样本/真线程/路由 503/200 |
| `docs/profiling-m2-compute-and-watchdog.md`（新） | 设计文档（公式、判据、字段、降级链、§9 人工验收清单、§10 M3 候选） |
| `docs/branding-and-frontend-patch.md` | §8 补 M2 说明（无新注入项）+ 链路图补两条 GET |

## 三、验证记录

- `test_profiling_m2.py` **26 PASS / 0 FAIL**；`test_profiling.py` **37 PASS / 0 FAIL**（M1 零回退）；
- 全量冒烟：**290 PASS / 23 SKIP / 0 FAIL（313）**；
- **真启动验证（踩坑 25 规程）**：`python main.py --port 8199` 后台起服，
  `/api/comfydl/profiling/watchdog/status` 200 返回实时采样（进程 CPU 5.9%/
  系统 11.4%/RSS 623MB/18 核/running），`log` 路由 200；杀进程后端口释放；
- JS 配平检查通过（剥离字符串/注释后括号平衡，正则字面量处理过的脚本）；
- py_compile 全部改动文件 OK；read_lints 全部干净；
- 临时文件（.tmp_profiling）已清理。

## 四、过程中的坑

1. **测试期望三次才算对**：F3a/F3b 的 vocab——链接的 TextVocabBuild 输出
   **覆盖** widget 的 vocab_size（估算器的正确行为），语料 "abcde-abcde-abcde"
   有 6 个不同字符 → 词表 7，不是 widget 写的 16，也不是我先猜的 6。
   教训：期望值要从数据流推导，不要从 widget 字面值抄。
2. **一次 replace 误删 basis 字典**：把 `estimate.basis["params"] = {` 当
   锚点时把字典体一起删了（py_compile 前读回上下文补救成功）。教训：
   同文件多次 replace 后，编辑前必须重读目标区域。
3. psutil 进程 `cpu_percent()` 是 **per-core 口径**（单核≈100，N 核≈100N），
   系统级是 0-100 归一——两个口径并存是故意的：单核瞬爆（vocab build）
   用进程口径抓，整机拖卡用系统口径抓。

## 五、待办

1. **用户界面验收**（清单 = 设计文档 §9 六项）；
2. 验收通过后：README 中英文章节更新（含截图）→ commit + push → F 盘
   `fetch` + `ff-only` 同步 → F 盘专项 + 全量冒烟（双环境规则）；
3. M3 候选已入设计文档 §10（W1 地雷扫描、W3 建议库、F3 MFU、F4 roofline、
   F5 时间预警——W2 案底正是它们的实测数据源）。
