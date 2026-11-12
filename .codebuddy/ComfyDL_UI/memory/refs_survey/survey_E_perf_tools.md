# Refs 调研报告 E：perf tools 类（7 仓库）

> 调研日期：2026-10-30。源仓库位于 `P:\Dev\ComfyUI_Refs\Refs\`（浅克隆）。

---

## E1. comfyui-profiler（tzwm）

**定位**：极简逐节点执行计时器，计算每个节点扣除子节点耗时后的"净耗时"，并在画布节点上实时标注。

**组成**：无自定义节点（__init__.py:102 空映射），纯 hook 型扩展；前端 index.js 挂 `onDrawForeground` 把耗时画在节点标题栏，注册精度设置项。

**核心实现**：monkey-patch `execution.recursive_execute`（__init__.py:54-99），`time.perf_counter()` 包裹节点执行；递归函数 `get_total_inputs_time`（:39-51）沿 prompt 图遍历输入节点，把已计入的子节点时间从父节点减去得净耗时；WS `send_json('profiler', ...)` 推前端，执行结束打汇总日志（:92-96）。

**可借鉴**：**净耗时递归去重算法**（DAG 计时很实用）；WS 事件 + 前端 canvas 绘制模式可直接嵌 ComfyDL 节点标注。

**风险**：零第三方依赖；但 patch `recursive_execute` 签名，ComfyUI 主线已改 async/ExecutionList 架构，此 hook 对新版已失效——借鉴算法不借鉴 hook 点。

---

## E2. ComfyUI_ProfilerX（ryanontheinside，v2.0）——本批与 ComfyDL 最直接对标

**定位**：完整工作流性能剖析套件：时间 + VRAM/RAM + 缓存命中统计 + 历史趋势（均值/标准差），带前端仪表盘。

**组成**：无节点（走 ProgressHandler + REST + web/ 前端）；v1 遗产 prestartup.py（patch execution.execute，含 profiler_core.py 方法级装饰器追踪）已被 v2 取代。

**核心实现**：官方 `ProgressHandler` API（handler.py:11-131）：start_handler/finish_handler 记录每节点计时，**"有 finish 无 start"即判缓存命中**（L103-119）；内存 metrics.py 抽象三后端（CUDA：`max_memory_allocated` + `reset_peak_memory_stats`；MPS；CPU psutil）；单 monkey-patch 重注入 handler（__init__.py:19-27，因 ProgressRegistry 每次执行重建）；REST `/profilerx/stats` 等（routes.py）。

**可借鉴**：① **ProgressHandler 是官方稳定钩子（比 patch execute 前向兼容）**；② VRAM 峰值用 `reset_peak` + `peak - base` 算净增量（handler.py:87,129）；③ **Welford 在线标准差**（storage.py:51-58）；④ 历史异步批量落盘（每 5 次、单线程 executor）；⑤ 缓存命中率统计；⑥ MPS 降级 no-op 的后端抽象层。

**风险**：仅 psutil；v1 死代码勿误借鉴；未集成 torch.profiler（无 kernel/operator 级数据）——ComfyDL 的 operators 分析需自行接 torch.profiler，此仓库只提供节点级 wrapper 思路。

---

## E3. comfyui-benchmark（Comfy-Org 官方）

**定位**：官方基准测试节点（v1.0.5）：对任意工作流无侵入式剖析——自动计时模型加载/采样/VAE/CLIP 各阶段并采集 GPU/内存负载，输出 JSON 供离线可视化对比。

**组成**：__init__.py（约 640 行核心：monkey-patch + ExecutionContext 数据收集 + nvidia-smi 采样线程）；nodes.py 唯一 `BenchmarkWorkflow` 节点（V3 io.Schema，capture_benchmark/file_prefix/file_postfix，无输出纯控制开关）；visualize_benchmark.py（约 50KB Plotly 可视化，单结果+多结果对比，--save-html/--save-image/--no-show）；config.yaml 每次执行热加载（load_config @L120）。

**核心方法学**：不自带测试集，直接测用户当前执行的任意 workflow；`require_node` 配置决定是否强制图中含节点才保存（L517）；`time.perf_counter()` + **try/finally 保证异常时仍记 valid_timing=False**（L254-270）；每条记录带绝对 start_time 重建时间线；采样阶段经 `comfy.patcher_extension.add_wrapper_with_key` 挂 `PREDICT_NOISE`/`SAMPLER_SAMPLE` wrapper 逐迭代记 iteration_times（L429-491）；NVIDIA 用守护线程 0.25s 间隔 `nvidia-smi --query-gpu`（显存/利用率/功耗/PCIe），RAM psutil（L53-79）；结果 JSON 落 `outputs/benchmark/`（含设备信息、只记非默认启动参数 @L95、各阶段耗时、硬件时间序列）。

**可借鉴**：1) monkey-patch + try/finally 计时模板（异常也记时长并标 valid_timing）；2) **`WrappersMP.PREDICT_NOISE/SAMPLER_SAMPLE` 官方 wrapper 做采样粒度计时（不改核心代码）**；3) `_inside_benchmark_hook` 嵌套去重标记（L246-262）解决多层 hook 重复计时；4) **nvidia-smi 轮询线程 + perf_counter 时间戳对齐，硬件曲线与操作时间线共用时间轴**；5) JSON schema：load_data/sampling_data/vae_data/clip_data/caches_data 分类 + 绝对 start_time，便于堆叠多 run 对比；6) 每条记录附设备指纹（VRAM state、PyTorch 版本、非默认 CLI 参数）保证可比性。

**风险**：依赖 comfy.* 内部模块；monkey-patch 私有属性（executor.caches）易失效；GLOBAL_CONTEXT 非线程安全；文件名靠时间戳同秒并发会覆盖。

---

## E4. ComfyUI-Crystools（crystian）

**定位**：调试/工具套件（v1.27.4）：菜单栏实时硬件监控条（CPU/RAM/HDD/GPU/VRAM/温度）+ 执行进度条 + 约 25 个调试类小节点。监控 UI 是最直接参考样板。

**节点/模块**：__init__.py:14-26，约 25 节点分 8 类（primitive/switch/debugger/list/pipe/image/metadata/parameters/utils）。监控前端：web/monitor.ts（扩展入口+设置项）、monitorUI.ts（仪表 DOM）、progressBar.ts/UI、monitor.css。

**核心实现**：后端采样线程 general/monitor.py：`CMonitor` 单例（默认 1s），threading.Thread + threading.Event 控启停，daemon 线程内 `asyncio.run(MonitorLoop())` 采集并 `server.PromptServer.instance.send_sync('crystools.monitor', data)` WS 推送（L24-58）；指标获取 hardware.py:getStatus（psutil CPU/RAM/HDD，L90-132）+ gpu.py（pynvml 利用率/显存/温度，L259-293），Jetson 走 jtop 回退，CUDA 不可用逐项降级 -1；控制 API server/monitor.py：PATCH `/crystools/monitor` 改采样率/开关；前端 monitor.ts 用 registerExtension + `api.addEventListener('crystools.monitor', ...)`（L575），`ComfyButtonGroup` 插到 `app.menu.settingsGroup` 前（L446-457），Top/Bottom/Disabled 三种位置，设置经 `app.extensionManager.setting` 注册；progressBar.ts 监听原生 status/progress/executed/execution_start/execution_error 事件渲染进度。

**可借鉴**：1) **WS 单频道 JSON 快照 + 前端整体 updateDisplay 的极简推送协议**，直接映射 ComfyDL profiling 数据流；2) "设置项驱动后端采样率、rate=0 完全停线程"的启停模型（省电）；3) monitorUI.ts 用 CSS order 排仪表、温度 `color-mix` 渐变着色、按 GPU 跟踪 maxVRAM 峰值；4) `ComfyButtonGroup` + insertAdjacentElement 注入菜单栏，兼容新旧 UI 双路径。

**风险**：依赖 pynvml/py-cpuinfo/psutil/deepdiff/piexif；采样循环线程内新建 asyncio event loop 较脆弱（作者自注释）；`whichHDD='/'` Windows 失败（hardware.py:20 已知坑）；线程级布尔开关无锁。ComfyDL 建议改用 asyncio 定时任务而非 daemon 线程 + asyncio.run。

---

## E5. ComfyUI-XPUSYS-Monitor（allanmeng）

**定位**：跨厂商（Intel/NVIDIA/AMD）GPU 实时监控 + 工作流显存成功率预测面板。零节点，纯"后端 provider + 前端 patch"架构。

**模块**：providers/base.py（抽象 provider + GPUSnapshot 数据契约）、intel/nvidia/amd 三实现、xpu_server.py（HTTP/WS 路由）；前端 web/xpu_monitor.js（105KB，Spark 实时图表、PRED 预测器）+ gpu_specs.json 硬件规格库。

**核心实现**：**provider 抽象层**——BaseGPUProvider daemon 线程按 interval（默认 1s）轮询 `_poll()`，锁保护原子替换 GPUSnapshot；前端 `GET /xpusys/stats` 拉取 + `send_json("xpu_stats", ...)` WS 广播双通道；Intel 用 **ctypes 直调 Level Zero Sysman**（ze_loader.dll，zesMemoryGetState/zesEngineGetActivity/zesPowerGetEnergyCounter，intel.py:1-160），功率需管理员权限；PRED 预测算法（docs/predictor_algorithm.md）：硬约束 P_peak（最大单模型 vs 有效显存，`V_eff = (V_free+V_alloc+V_rsv)×0.9` 指数衰减）× 软约束 P_load（总量 vs RAM/虚拟内存中转分段函数），平台系数 γ（CUDA UVM=4.0）。

**可借鉴**：a) **GPUSnapshot 统一数据契约 + provider 多态——ComfyDL profiling 的 memory 层可直接套用**；b) "只算可控资源"的显存回收模型（V_alloc+V_reserved 可回收、驱动/桌面占用为固定成本）；c) PromptServer 上挂后台协程广播（run_coroutine_threadsafe，__init__.py:36-41）；d) PRED 成功率分级配色可移植到 profiling 报告。

**风险**：psutil 必需、pynvml 可选、ADLXPybind/amdsmi 故意不自动装（原生 DLL 崩溃会杀整个进程，requirements.txt:8-23 有详细记录）；Level Zero ctypes 平台耦合强（Windows-only 权限检查）；α/γ 均为经验常量。

---

## E6. ComfyUI-AMDMonitor（the-Macro-Man）

**定位**：AMD/ROCm 卡 VRAM 监控 + run 历史记录 + 崩溃预警 + 可选 LLM 遥测分析。核心洞察：Crystools 的 pynvml 是 NVIDIA-only，而 ComfyUI 自带 `/system_stats` 走 PyTorch HIP 后端本就能正确报 AMD VRAM——GPU 数据直接复用官方端点。

**模块**：零节点，纯前端 patch。后端单文件 __init__.py（693 行，挂 /amdmonitor/stats|run/start|run/end|alert|runs|ai/*|version 路由）；前端 js/amd_monitor.js（75KB：GPU 条、sparkline、峰值追踪、toast/系统通知、History 表格、CSV 导出、AI 分析面板）。

**核心实现**：GPU 指标前端轮询 `/system_stats`（amd_monitor.js:1477），按 device 算 `used = vram_total - vram_free` 维护客户端峰值；系统级 swap/CPU/磁盘/网络吞吐用 psutil 差分计算（__init__.py:248-284）；**崩溃安全日志**：`_RunLogHandler`（:289-327）作全局 logging handler 逐条 flush 到当前 run 的 log 文件——因 ROCm OOM 进程直接 abort 无 traceback，边跑边写才能留到崩溃瞬间；run/start、run/end 路由由前端在 execution 事件触发，后端落 JSON + 追加 runs.csv（含 peak_vram_gb、sec_per_step、slowest_node 等列，:386-389），保留最近 50 条；loopback 校验（_is_local_request）防远程写文件、API key 与 endpoint 绑定。

**可借鉴**：a) **"逐条 flush 的 run 日志 handler"——ComfyDL profiling 崩溃取证可直接抄**；b) **runs.csv 扁平 schema（peak_vram/sec_per_step/slowest_node）是现成 profiling 输出表结构**；c) 前端轮询 /system_stats 维护 per-device peak 的轻量做法；d) 写入目录用 `folder_paths.get_user_directory()` 而非插件目录（防 Manager 更新清空历史）。

**风险**：psutil 软依赖（缺失优雅降级）；AI 分析依赖用户自配 OpenAI 兼容端点默认关闭；GPU 温度/功耗/频率拿不到（/system_stats 没有）；峰值依赖前端轮询频率，短峰值可能漏采。

---

## E7. ComfyUI-Model-Analytics（Bennowan）

**定位**：只读模型资产管理面板：盘点磁盘模型、关联已存工作流、追踪运行时实际加载，找出"占用磁盘但从未使用"的模型。零节点，纯 server extension + 前端。

**模块**：scanner.py（物理+逻辑盘点）、tracker.py（运行时加载追踪+持久化）、api_server.py（单一只读路由 GET /model_analytics/inventory）、web/（vanilla JS 仪表盘）。

**核心实现**：物理盘点遍历 `folder_paths.folder_names_and_paths` 但用 `_get_model_root_dirs()`（scanner.py:121-156）过滤 custom node 注册路径（必须位于官方 models/ 根下 + 路径段黑名单双重校验）；逻辑盘点递归解析用户 workflow JSON，`_iter_model_strings` 提取模型文件名字符串建 模型→工作流 映射（:354-364）；**运行时追踪 `install_tracker_hooks()`（tracker.py:277-314）monkey-patch `comfy.utils.load_torch_file`，加载成功才算"使用"**，写 model_history.json（use_count/last_used，tmp + os.replace 原子写）；used/unused 非存储标志，每次请求用 `is_recently_used()`（:89-103）从 last_used 现算，阈值天数环境变量配置；全程防御式扫描（每文件/workflow/条目独立 try，单点失败不毁请求）。

**可借鉴**：a) **patch `comfy.utils.load_torch_file` 记录模型加载事件——可扩展为 profiling 记录"哪个模型何时加载、耗时多少"的最小埋点**；b) `_relativize()`（tracker.py:240-274）把绝对路径对齐回 folder_paths 根的规范化方法，可统一 profiling 的模型键名；c) mtime 比对缓存失效策略（:120-172）适合多进程；d) 防御式逐项 try 扫描模式。

**风险**：零第三方依赖；patch 点耦合内部 API（tracker.py 已做防御：API 变了只禁用追踪不崩启动）；Hot reload 双 patch 用 `_model_analytics_patched` 标志防重入——ComfyDL 硬改自带节点栈时注意同类标志冲突。

---

## 批次小结（对 ComfyDL profiling 模块最有价值的横切主题）

1. **hook 点优先级**：官方 ProgressHandler（ProfilerX）> add_wrapper_with_key 官方 wrapper（benchmark 的 PREDICT_NOISE/SAMPLER_SAMPLE）> patch load_torch_file 等单一函数（Model-Analytics）> patch recursive_execute/execute（tzwm 已失效、ProfilerX v1 死代码）。ComfyDL 的 profiling 应选前两级。
2. **采集层骨架**：XPUSYS 的 provider 抽象 + GPUSnapshot 契约 + daemon 轮询线程 + WS/REST 双通道，是 memory/compute 采集层的现成蓝本。
3. **数据落盘**：benchmark 的分类 JSON schema（带绝对 start_time + 设备指纹）+ AMDMonitor 的 runs.csv 扁平表（peak_vram/sec_per_step/slowest_node）+ 逐条 flush 崩溃安全日志。
4. **统计方法**：Welford 在线标准差（ProfilerX）、净耗时 DAG 递归去重（tzwm）、缓存命中判定"有 finish 无 start"（ProfilerX）、嵌套 hook 去重标记（benchmark）。
5. **算子级分析空白**：所有仓库均未集成 torch.profiler——ComfyDL 的 operators 分析是差异化能力，需自行接 torch.profiler。
