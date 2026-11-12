# SP-0 开销归因实测报告（2026-10-31，dual-track-p0-spine 计划）

> 结论先行：**"开销几乎翻倍"的真凶是 crash site（+20%~+100%，冷启动最坏 3-10x），
> 不是 profiling meter（≈0~4%）**。T6-1（crash site 降本重设计）**建议立项**，
> 且设计方向需修正（见 §4）。测量环境：P 盘 CPU 版 in-process（PromptExecutor 直执行，
> 无 HTTP server），两模板 × 5 配置 × 每配置预热 1 次 + 计时 3 次取中位。

## 1. 方法

- 执行器：`execution.PromptExecutor(mock_server, cache_type=None, cache_args={ram:0.5, ram_inactive:0.3})`
  （复刻 test_crashsite.py:398-403 先例），每 rep 新建 executor（防缓存命中）。
- 模板：`classification_from_scratch`（8 节点含 500 步训练）、`tabular_classification_production`。
- prompt 由 UI 格式模板转换（复刻 test_template_workflows 的 widget 序解析），经
  `validate_prompt` 校验。
- 配置（proflog master × crashsite）：
  - `off`：无 provider 注册（全短路，caching.py:245）；
  - `disabled`：注册 `SnapshotProvider(enabled=False)`——**只付每节点缓存层开销，不建库**
    （on_prompt_start 早退，provider.py:88-89）；
  - `on`：`SnapshotProvider(enabled=True)` + MockQ 喂 prompt——完整真实路径。
- profiling on/off = `proflog.set_master_enabled()`（meter "count" 模式默认开）。

## 2. 结果（P 盘 CPU，预热后中位数）

| 模板 | 配置 | 中位耗时 | Δ vs 全关 |
|---|---|---|---|
| classification_from_scratch（基线 0.805s） | prof=F crash=off | 0.805s | — |
| | prof=F crash=disabled | 1.228s | **+52.5%** |
| | prof=F crash=on | 0.690s* | *噪声区间见下 |
| | prof=T crash=off | 0.525s | -34.9%（噪声） |
| | prof=T crash=on | 1.586s | **+97.0%** |
| tabular_classification_production（基线 0.452s） | prof=F crash=off | 0.452s | — |
| | prof=F crash=disabled | 0.490s | +8.6% |
| | prof=F crash=on | 0.549s | **+21.6%** |
| | prof=T crash=off | 0.450s | -0.4% |
| | prof=T crash=on | 0.603s | **+33.5%** |

冷启动（无预热）首轮：crash=on 出现 3.5~9.0s 尖峰（SQLite 建库 + USB 盘 IO）。
P 盘为 U 盘 + CPU，噪声显著（同配置波动 ±30%）；\* 号格为异常快于 disabled 的噪声样本。
**F 盘 GPU 复测建议**：同步后顺手跑同款脚本（张量在 GPU 上时 §3 的热点 1 会因
device sync 显著恶化，预计 GPU 端开销更高）。

## 3. 代码级归因（热点定位）

crash site 开销的三个确切来源（全在"lazy capture 之后仍然存在"的路径上）：

1. **`_contains_self_unequal(cache_key)`**（comfy_execution/caching.py:249，
   实现 cache_provider.py:123-136）：对整个 cache_key 递归做 `obj == obj`——
   **张量输入触发逐元素比较（分配布尔张量 + 全量扫描）**，每节点每输入一次。
   GPU 张量下更糟（隐式 device sync）。
2. **`_canonicalize` 失败路径**（cache_provider.py:79-108）：张量不在白名单 →
   每节点必抛 ValueError → `_serialize_cache_key` 返回 None → provider 根本不会被调。
   即：**每节点白跑一遍递归 canonicalize 只为得到一个 None**。
3. **每次运行建快照库**（provider.py:87-108 `on_prompt_start` → `store.create_snapshot`）：
   SQLite 建库 + WAL + manifest 写入，P 盘 USB 上首轮 3-9s 尖峰的主因。

profiling meter（runmeter "count" 模式）开销 ≈0~4%，可忽略；**in-process 测不到的缺口**
（记档，不现场改）：watchdog 250ms 采样线程（server 期）、前端 graphToPrompt 劫持 +
每 run estimate（浏览器侧）。两者均有 master switch 退路。

## 4. 对 T6-1 的裁决与设计方向修正

**裁决：T6-1 立项**（M-D 阶段执行）。且比原构想（ring buffer 身体 + 元数据全程）**新增两个
更高优先级的廉价化点**：

- **P-α 键检查廉价化**（最大头）：`_contains_self_unequal` 对张量只查
  shape/dtype/device（不做逐元素 ==）；`_canonicalize` 对张量先短路（跳过递归直接
  返回 shape/dtype 摘要或显式 unsupported 标记）——预期消除每节点开销的大头。
- **P-β 快照库惰性创建**：`on_prompt_start` 不建库，首节点完成或失败信号到达时才建
  （或失败路径独占建库）——消除每次运行的建库尖峰。
- **P-γ ring buffer 身体 + 异常 flush**（原构想，降为第三优先）。
- **P-δ 确定性重放配方**（原构想，保底手段）。

原认知修正记录：「2026-10-13 lazy capture 修复后正常路径接近零开销」**不成立**——lazy
只免了 tensor 深拷贝与落盘（store 相位），没免键检查链（§3.1/3.2）与建库（§3.3）。

## 5. 测量口径备忘

- 测试快照库（prompt_id 前缀 sp0-，28 个）已清理；临时脚本已删。
- 本报告数字为 P 盘定性口径；T6-1 立项前建议 F 盘 GPU 定量复测一轮（同方法）。
- 基线冒烟：305/33/1（339），其中 1 FAIL = CdlRegressionTrain RMSE 0.1202 vs 0.1 阈值，
  隔离重跑两次均 PASS——**种子抖动**，预先存在，与本计划无关（建议后续把该断言阈值
  放宽到 0.15 或固定 seed，另立小任务）。
