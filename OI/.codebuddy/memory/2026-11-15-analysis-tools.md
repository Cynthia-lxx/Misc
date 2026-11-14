# 2026-11-15 · 分析工具扩展（设计基线）

## 需求与用户裁决

在首版 Web 控制台上补齐「AI 生成之外」的能力：内存分析、算法复杂度分析、代码风格化/冗余分析、性能调优。尽量本地分析；AI 不得过长、不得过度使用多轮上下文，总用量最多相当于**生成 2~10 次代码**的用量。设计参考 `P:/Dev/ComfyUI_Refs/ComfyDL_UI/`；记忆落盘到本工作区 `.codebuddy/memory/`。

澄清结果（2026-11-15 用户确认）：

1. 分析对象 = 当前输出代码 + 工作区任意 `.cpp`（根目录有 83 个 `P*.cpp` 题解）+ 指定路径文件。
2. 允许真实编译运行测量；用户指出编译器来源 `F:/Software/Dev-Cpp/MinGW64`，并**补充指正：必须把 MinGW64 复制进工作区，成为项目的一部分**，运行时不得依赖 F: 盘路径。
3. AI = **每次分析完成自动 1 次**极短汇总（不是手动按钮），但仍严格单轮/限量。
4. ComfyDL_UI = **只借信息架构与组件词汇表**，视觉沿用首版 osu UIReference 皮肤（`--ui-*` / `.oc-*` / 9 主题 / reduced-motion），不引入 `cdlp-*` CSS、Gmarket 字体、Vue/PrimeVue/LiteGraph。

## 技术栈与不变约束

- 后端继续 `web/server.py` 的 `ConsoleHandler`（标准库 `http.server`），零新增 Python 依赖；只加 `/api/analyze*` 路由分支 + 一个分析包 import。
- 前端新增第 5 个视图「分析」，逻辑/样式拆到 `analyze.js` / `analyze.css`，不把代码塞进现有 `console.js`。
- `OI.py` **不改**；它 `SYSTEM_PROMPT`（`OI.py:91-118` 十一条）是「风格化」规则的唯一来源。AStyle 只做可选 diff 预览，绝不默认改写（会破坏紧凑排版、单字母变量、注释结尾「喵」）。
- 服务仍只监听 `127.0.0.1`；分析与测量纯本地，不外发指标。
- 判定色为固定语义色，不随 9 套主题调色板变化。

## 参考来源（已核实，只读）

`P:/Dev/ComfyUI_Refs/ComfyDL_UI/` 是 ComfyUI v0.34.0 dehydrated fork（GPL-3.0）。分析工具设计指它的 Profiling 子系统：

- 设计文档：`docs/profiling-m1-memory-estimation.md:80-90`（三色表 + 「未注册节点类型显式 unknown，**绝不猜**」）、`:115-121`（四级降级链）、`:58-78`（公式口径）；`docs/profiling-m3-opgraph.md:20-29`（手动 Analyze + 进程内 8 条缓存 + 永不自动进 run 路径）、`:127-157`（四道安全防线）、`:162-180`（coverage 四档）。
- 引擎分层：`comfy/profiling/` — `engine.py`（报告聚合 + 三色判定）、`estimators.py`（估算器注册表）、`formulas.py`（公式/字节口径）、`assumptions.py`（假设层）、`postmortem.py`（事后归因）、`runmeter.py`（实测）、`switch_registry.py`/`proflog.py`（master switch）、`signature_cache.py`。
- 可视化词汇表：`app/profiling_assets/profiler.css` 的 `.cdlp-panel/.cdlp-card/.cdlp-verdict/.cdlp-table/.cdlp-budget/.cdlp-meter/.cdlp-assumption*/.cdlp-burst*/.cdlp-live*`；`profiler.js` 的 `renderPanel()`。
- API：`app/profiling_routes.py` 8 条路由；`GET/POST /enabled`，`/estimate` 不受 master switch 拦截（手动路由常开），`/history` 出错也 200 降级 `available:false`；`:372-378` courtesy rule「nothing here may break a run - every failure degrades to no data」。
- 集成：`app/frontend_patch.py:961-982` 幂等注入一个 module script；`docs/branding-and-frontend-patch.md:232-236` 铁律「前端包唯一写入就是这一个 script 标签」。
- 硬约束 `AGENTS.md:57-73` No Internet Requests（禁止 telemetry/analytics/metrics 外发，纯本地允许）；`:25-28` 拒绝 AI 味代码；`:3-13` 小改动/最少依赖/删死代码；`:30-55` 分层边界。

### 核实后的数据契约（照搬语义，不照搬代码）

- 报告级：`version`（bump 规则 `engine.py:42-44`）、`budget{name,total_bytes,free_bytes}`、`verdict`、`verdict_reason`、`peak_bytes`（取**峰值**非累加）、`largest`、`nodes[]`、`assumptions_used[{key,value,source}]`、`error`、`disclaimer_key`、`flops{total,any_estimated,by_kind,largest}`（`engine.py:121-141, 242-257`）。`engine.py:25-26`「engine never raises on malformed graph: the report carries the error」。
- 项级：`status=estimated|unknown`、`confidence=exact|approx`、`reason`（unknown 必填）、`items[{kind,label,bytes,single_bytes,approx}]`、`basis{key,params}`（`estimators.py:63-90`、`formulas.py:47-73`）。
- 判定：`green < free*0.7`；`red ≥ total*0.9 或 single > free`；其余 yellow；无预算/全未知 → `unknown`（`engine.py:38-40, 101-111, 253-254`）。前端文案 SAFE / MAYBE OOM / CERTAIN OOM / NO ESTIMATE 只是展示层（`profiler.js:65-76`）。
- unknown 表达：`Unknown(reason)` 值对象（`shapes.py:9-11, 31-38`）、`NodeEstimate.as_unknown(reason)`（`estimators.py:82-90`）；**未知项绝不贡献 peak_bytes**。
- 假设层：`AssumptionSet.get(key) -> (value, source)`，`source ∈ override|default`，`used_entries()` 逐条进报告（`assumptions.py:26-33, 47-65`）；用了假设即降级 `confidence=approx`。
- 口径声明：内存取 peak、计算量取 sum、bytes 为整数、时间是 measured 而非由公式推导（`engine.py:17-23`、`formulas.py:18-27`）。
- 缓存键：`(namespace, code_version, canonical_json(payload))` → SHA-256，`sort_keys=True, separators=(",", ":")`（`signature_cache.py:81-97`）；轻量替代 = 规范化 JSON hash + 进程内 8 条 FIFO，**带 error 的报告不缓存**（`opgraph.py:520-558`）。
- master switch：默认开、热切换、**不持久化**、进程内状态；hard-disable 需重启（`proflog.py:133-153`、`switch_registry.py:117-152`）。

## 本项目落地的数据契约

`Finding{dimension, key, verdict, value, unit, evidence, reason, confidence}`（confidence ∈ measured/estimated/heuristic/unknown）；`AnalysisReport{version, digest, source, verdict, verdict_reason, findings, assumptions, measurements, ai_summary, unknown_count}`。三色 + unknown 显式标注、假设行写 `source`、估算值统一带 `estimated`。

## AI 预算口径（关键）

- 每次分析完成**自动** 1 次短汇总：单轮 `system + user`、**不携带 `oi.history` 任何内容**、输入只传本地报告的结构化摘要（紧凑 JSON，不反复传整份代码）、`max_tokens` 远小于生成的 8192、短超时、要求 3~5 条建议。
- 一次汇总 ≈ **0.1 次代码生成当量**；默认会话级硬上限 **2 次当量**，可配置至 **10**；按 `sha256(code)+规则版本` 去重；master switch 可一键关闭。
- AI 无 Key / 超时 / 超预算 / 被关闭 / 失败时，一律返回**完整本地报告**（`ai_summary.status` 为对应状态），不弹错误、不阻塞诊断。
- 与历史彻底解耦：AI 汇总不写入 `~/.oicoder_history.json`，不占用 CLI 上下文。

## 工具链 vendoring 事实

- `F:/Software/Dev-Cpp/MinGW64`：**336.4 MB / 5755 文件**；bin 91.2 MB、lib 35.9 MB、libexec 41.9 MB、share 13.8 MB、x86_64-w64-mingw32 153.3 MB、include 0.3 MB。
- `bin/g++.exe` = `g++.exe (tdm64-1) 4.9.2`（TDM-GCC，支持 `-std=c++14` 与 `bits/stdc++.h`，**不要**用 C++17 语法做探针）。另有 `size.exe`（text/data/bss）、`gprof.exe`、`gdb.exe`、`objdump.exe`、`nm.exe`；运行期 DLL `libstdc++-6.dll`、`libgcc_s_seh_64-1.dll`、`libwinpthread-1.dll` 等。
- `F:/Software/Dev-Cpp/AStyle/AStyle.exe` 存在，仅用于可选风格 diff 预览。
- 全量复制并**保持内部相对目录名**（g++ 靠 `bin / libexec / lib / include / x86_64-w64-mingw32` 的相对结构找 `cc1plus` 与标准库）；可裁剪清单：`share/`、doc、`python27.dll`、gdb 符号，默认不裁。
- `g++`/`clang++` 均不在系统 PATH；运行时须把 `toolchain/MinGW64/bin` 注入子进程 PATH 或加 `-static-libgcc -static-libstdc++`。

## 安全与实现要点

- 编译在独立临时目录、`-std=c++14 -O2`、stdin 样例、超时（默认 5s）、进程内存/CPU 上限用标准库 `ctypes` + Win32 Job Object / `GetProcessMemoryInfo`（不引 psutil）、输出长度截断、产物清理；失败自动降级为纯静态报告。
- 静态分析单次线性扫描 O(n)：自写轻量 C++ tokenizer（去注释/字符串、括号匹配、函数与循环边界、行号映射、嵌套深度上限）；复杂度结论一律标 `estimated`，不谎称精确 Big-O；无法识别一律进 `unknown` 并给原因。
- 报告按 `sha256(code)+规则版本+实测标志` 缓存（进程内上限 8，带 error 不缓存）；编译产物与临时文件用完清理。
- 日志复用现有 `log()` 与事件日志面板；不打印 API Key、不整份转储代码与编译 stderr。

## 验证与踩坑（待实施后补充）

- [ ] 内置工具链编译运行 `#include<bits/stdc++.h>` 通过
- [ ] 工作区 `P*.cpp` 样本四类报告 + unknown 分支 + 缓存命中
- [ ] 浏览器端到端（playwright-cli + `--browser=msedge`）分析视图、证据定位、AI 降级、9 主题、0 控制台错误
- [ ] 隔离 HOME 测试时 JSON 必须**无 UTF-8 BOM**
