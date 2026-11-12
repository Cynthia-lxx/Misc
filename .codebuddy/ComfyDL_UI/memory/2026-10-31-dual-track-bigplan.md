# 双主线能力大计划（草案 v0.95，2026-10-31 落盘；同日 v0.95 增补：ADR-D7 面板就绪契约 + M-D/P6b Optimizer 面板，用户拍板）

> 状态：**草案，待用户审阅**。审阅通过后按 program → milestone → smallplan 三级治理推进，
> 每个 smallplan 进 PLAN MODE 细化后 (CRAFT)*n 执行。
> 数据来源：`refs_survey/` 5 份分类报告（50 仓库广度调研）+ 7 簇专项实现档案
> （2026-10-31 会话，行号级）+ ComfyUI-original 机制锚点 + 官方文档契约核对。
> 前置阅读：`MEMORY.md`（项目现状）、`2026-10-28-classification-line-plan.md`（在案分类线）。

## 0. 定位与命名

**正式名：双主线能力大计划（Dual-Track Capability Program）**。
视角：不是"给 app/crash site/profiling 打补丁"，而是**生产级深度学习 × 教学级深度学习**
双门面驱动的产品能力空间建设。核心组织单位 = **能力簇**：每簇一个底层引擎，
长出生产/教学两个门面，各配差异化验收标准（与 2026-10-28 双形态铁律同构）。

## 1. 能力簇 × 门面矩阵（C1-C7）

| 簇 | 底层能力 | 生产门面 | 教学门面 | 主要来源 | Tier |
|---|---|---|---|---|---|
| C1 性能自动化 | benchmark+缓存+注入引擎 | attention 自动选优、torch.compile 包装、量化加载、显存管家 | Attention Lab（手动切后端看 speedup 表+原理） | A2-A7/A10-A14 | 0/1 |
| C2 实验台 | param_grid + run 记录三层 schema | GridSearch、auto-queue 批量、runs 历史对比 | 超参探索器（lr 曲线族、seed 方差、XY 网格图） | D2/D5/E6/E3 | 0（追踪）/按需（扫描） |
| C3 交互教学 UI | 节点内富 widget 引擎 | 数据/结果内联检查（直方图、对比滑块、表格） | 交互画布、Notebook 单元格、变量巡检 | C7/C11/D4 | 0（Notebook 为 2） |
| C4 数据工程 | DATASET 操作语义层 | 双列表语义、Pack/Unpack 字段→端口、分片流式加载 | EDA 教学节点族 + 表格预览 | D1/D3/D7/D8 | 0 |
| C5 可靠性与资产 | ring buffer + 重放配方 + 加载埋点 | crash site 降本重设计（视 SP-0 结果）、模型资产盘点 | "崩溃的解剖课"（postmortem 即教材） | E6/E7 | 0-5% |
| C6 观测与仪表 | 观测总线（spine） | 资源监控条、执行徽章、runs 面板 | 显存剧场（训练时实时 VRAM 曲线） | E4/E5/C9 | 1 |
| C7 原理透视 | 计量+可视化引擎（复用 C1/C5/C6 数据） | 性能报告导出矩阵 | Quantization Lab、净耗时标注、PhaseTimer 管线解剖 | B6/B7/E1/A6 | 0/2 |

C7 的特殊性：**不新建底层能力**，把 C1/C5/C6 产出的数据翻译成教学语言——
这是 ComfyDL 对 ComfyUI 生态的独占差异化（50 仓库全是纯生产工具，无人做原理透视）。

## 2. 开销治理制度（全计划统一，ADR-D1）

**成本位置模型**（决定设计，比大小分类更根本）：

| 位置 | 例子 | 治理规则 |
|---|---|---|
| import 期 | 注册表构建 | 不治理 |
| prompt 建立期 | clone+注入、compile、benchmark 查缓存 | 摊销后趋零，天然 Tier 0 |
| 每步期 | ring buffer 快照、preview | 常数小成本或可重叠（异步 D2H 独立 stream） |
| 每算子期（per-dispatch） | RunMeter 的 TorchDispatchMode | **最危险**，只能 Tier 2 或采样化 |
| run 收尾期 | 落盘、报告 | 不敏感 |

**Tier 判定与自证**：
- Tier 0（运行期零开销，默认启用）/ Tier 1（<5% 且可一键关，默认启用带开关）/ Tier 2（仅手动触发，永不默认启用）。
- **元 profiling 铁律**：每个 Tier 1 特性必须计量自身开销（hook 包 perf_counter，汇入设置面板），
  不自我计量的特性不予合入。
- Tier 声明必须有测量背书（方法学：warmup 丢弃、取中位、设备指纹入记录、try/finally 异常也计时——E3 模板）。

## 3. 架构级冲突裁决（ADR-D2 ~ D5）

| # | 冲突 | 裁决（草案，可推翻） |
|---|---|---|
| ADR-D2 | **crash site 开销归因**：专项档案证实 2026-10-13 后正常路径 `should_cache` 恒 False（lazy capture，rescue 才捕获），"几乎多一倍"的感受需归因验证 | **SP-0 前置实测**（关 profiling 开 crash site / 开 profiling 关 crash site / 全关三组对照），结果决定 T6-1（crash site M2 重设计）立项与否与规模 |
| ADR-D3 | 量化加载 × 内存估算器：estimators 按 dtype/shape 算字节，量化 layout 改变 bytes-per-element | **硬捆绑交付**：量化节点批次必须同批交付 estimators 字节率适配（扩展点：`TensorVal.itemsize` + `QUANT_ALGOS[fmt]["storage_t"].itemsize` + group scale 开销；GGUF 字节率 `gguf.GGML_QUANT_SIZES[qtype]`）。接口统一为"格式描述符"（bytes_per_element/dequantize/state_dict），执行域双路径（kitchen 量化域直算 vs GGUF 前向 dequant）。首期最小版只接 GGUF，不动 kitchen 侧 |
| ADR-D4 | torch.compile × RunMeter：inductor 绕过 dispatch，实测回流有盲区 | 承认盲区 + **编译区域按节点粒度归账**（progress 事件计时兜底）；文档明示。compile 一律走 WaveSpeed 模式（`add_object_patch` 进补丁链 + 中央 compile 管理器缓存字典，禁原地 setattr） |
| ADR-D5 | 跳步/FBCache × crash site 重放：隐藏 cache 状态破坏确定性 | 重放配方显式记录"生成栈 patch 清单"，**恢复路径默认关跳步**（保真优先），文档注明 |
| ADR-D7 | 性能后端各自发明控制机制 → 将来面板要逆向 N 套接口（Optimizer 面板立项时确立，2026-10-31 用户拍板） | **面板就绪契约（性能后端准入四条，违反不得合入）**：① 注册 `switch_registry`（id / hot_toggle / self_meter）；② 实现 `describe()`/`status()` 供面板渲染当前状态（生效后端、几何、缓存命中）；③ 影响显存的必须绑 estimators（ADR-D3）；④ 硬禁用语义（禁降级 error-continue，SP-4 先例）。P6b 面板因此是薄渲染层而非架构重构 |

## 4. 共享基建（P0，四件，ADR-D6/D12）

1. **观测总线（observation spine）**：单 daemon 采样线程 + 无锁队列 + 单 WS 频道 + 版本化
   GPUSnapshot 契约（XPUSYS 模式）。服务 C5/C6/crash site/profiling 四家，rate=0 全停
   （Crystools 启停模型）。**吸收**：GPUSnapshot 不可变契约+原子替换、Enhancement-Utils
   双层 hook（send_sync 拦截 + execute 包裹）与 ran/cached/blocked 判定、我们 watchdog
   已有的 burst 状态机；**摒弃**：Crystools 线程内 asyncio.run、每插件私有路由与消息名。
2. **签名缓存服务**：统一缓存键 = prompt 签名 + 代码版本 + 配置指纹（attention benchmark
   库 / opgraph 探针结果 / crash site 重放配方 / compile 缓存共用）。
3. **开关注册表**：proflog master switch 模式推广为通用机制，每 Tier 1 特性注册
   `{id, 默认值, 热切换, 自我计量}`，设置面板统一呈现（含实时开销）——元 profiling 载体。
4. **compile 管理器**：中央编译缓存 + 副本包装策略（解决原地改写副作用），C1 的生成栈与
   训练栈 compile 节点共建其上。

## 5. Phase 划分与里程碑（备案）

### 前置（P-1）
- **SP-0 开销归因实测**（0.5 craft，半天）：crash site × profiling 开关三组对照计时，
  产出归因报告 → 决定 T6-1 去留。

### 里程碑 M-B：性能红利包（≈8-10 craft）
| Phase | 内容 | craft |
|---|---|---|
| P0 基建 | 观测总线 + 签名缓存 + 开关注册表（compile 管理器可延至 P1） | 2-3 |
| P1a | attention 选优双形态（C1a 档案：计时骨架/验证/缓存层直接移植；补幂等标记、缓存移出插件目录） | 1.5-2 |
| P1b | torch.compile 包装族（compile 管理器 + add_object_patch 模式） | 1.5-2 |
| P1c | 显存管家（统一 purge 例程 + NVML 外部占用修正） | 0.5-1 |
| P2 | 量化加载最小版（GGUF loader + 格式描述符 + estimators 字节率适配，硬捆绑） | 2-3 |

### 里程碑 M-C：生产力与教学包（≈10-11 craft）
| Phase | 内容 | craft |
|---|---|---|
| P3 | 实验台（GridSearch 修正 direction 缺陷 + auto-queue + runs 三层 schema + 对比视图） | 3-4 |
| P4 | 数据簇（双列表语义 + Pack/Unpack 字段→端口 + ShowText 预览 + 分片加载） | 3 |
| P5 | 交互教学 UI（obvpm 原生模式富 widget + Notebook 单元格 Tier2 默认关） | 3-4 |

### 里程碑 M-D：观测与透视包（≈5-9 craft，含 P6b 后 7-12）
| Phase | 内容 | craft |
|---|---|---|
| P6 | 观测包（监控条 + 执行徽章 + runs 面板，骑在 P0 spine 上） | 3 |
| P6b | **Optimizer 面板**：侧栏统一性能控制面（attention/compile/quant/显存管家 + 收编 profiling/crash site/perftools 现有开关），呈现各后端 describe()/status()、benchmark 表、缓存管理。**闸门制，不设硬排期**：性能后端家族 ≥4 个（收编项计入）且 ADR-D7 契约全数齐备即动工，闸门不满足自动顺延至 P7 之后。复用 P6 侧栏基建；Inspector 教学节点届时可择机从画布迁入面板（次优节点形态的归宿） | 2-3 |
| P7 | 原理透视（Attention Lab / Quantization Lab 模板 + 净耗时标注 + PhaseTimer） | 3 |
| T6-1 | crash site M2 重设计（**视 SP-0 结果决定**：ring buffer 身体 + 元数据全程 + 确定性重放配方 + 异常 flush） | 0-3 |

### Backlog（P8 深水区，按需启动）
跳步采样（FBCache/TeaCache）、INT8 在线量化、block swap、EDA 全家桶、自动布局、
模型资产盘点、App 融合 tab。启动条件：生成栈实际使用频率上来 / 用户点名。
深水区性能后端成熟时**凭 ADR-D7 契约即插即入 P6b 面板**（加条目，不返工）。
perftools 收编注记：P6b 落地时，Settings > ComfyDL > Perf 组的开关统一收编为面板
条目（Settings 项退役或降级为面板入口开关）——收编路径立此存照，届时再细化。

### 与在案工作的关系
- **里程碑 M-A = 分类线收官**（`2026-10-28-classification-line-plan.md`，已定稿未动工，
  **不受本计划影响，仍第一个动工**）。
- App Mode 大计划与 C2/C6 有界面融合点（实验台面板可作 App Mode tab），建议合审。
- Profiling backlog（等效 workflow 导出、.vsdx、roofline、建议库）并入 P7 视情况排。

**总量：22-29 craft（含 P6b 后 24-32）**（约为 Profiling v2 的 3 倍），确认 program→milestone→smallplan 三级治理。

## 6. 验收标准（双门面差异化，ADR-D8）

| 维度 | 生产门面 | 教学门面 |
|---|---|---|
| 核心指标 | 吞吐/延迟/显存/可靠性数字（附微基准） | "看得见、摸得着、改得动" |
| 交付单元 | 节点 + API + 面板 | **必须配教学模板工作流**（如 attention_lab.json）——写入大计划验收 |
| docstring | 参数语义、性能特征、边界 | 原理讲解（先例：CE 内置 log_softmax 教学注释） |
| 测试 | 数值正确性 + 微基准 + Tier 自证 | 模板可跑通 + UI 存在性断言（底层测试，不做视觉验收——铁律） |

通用硬规则照旧：双盘冒烟零 FAIL、四份说明文件同步、widgets 开箱可用、新节点详细 docstring、
合批交付摊薄文档/测试固定开销。

## 7. 风险登记册与红线

- **红线（全计划禁入）**：运行时 pip 自装、无沙箱 exec（Notebook 除外且默认关+警告）、
  独立端口监控进程、patch 已废弃 recursive_execute。
- **License 待核**：H3-Optimizations（无 LICENSE，只借鉴思路）、node-organizer（疑似 GPL，
  **只借鉴架构思想，禁抄代码**）、Veda 打分器权重（H3 Community License，不引入）。
- **交互矩阵残余风险**：block swap 搬运污染"最慢算子"榜（meter 加搬运/计算分类标记）；
  probe × compile（探针会话统一 `torch.compiler.disable`，签名缓存键含编译配置）。
- **P 盘无 GPU 适配**：attention/compile 在 CPU 盘只测正确性与逻辑（假计时器），
  GPU 性能断言只在 F 盘冒烟层；compile 类测试独立分类不进默认冒烟（编译慢，防拖垮日常循环）。

## 8. Smallplan 索引

见 `2026-10-31-dual-track-smallplans.md`（小计划集 + 推进顺序 + 前置调研清单）。

## 9. 审阅清单（用户拍板点）

1. 七簇是否全收（C3 工程量最大，唯一可砍项——砍后教学交互退化为 badge/图表级）？
2. 实验台雄心：轻（run 记录+对比表，2 craft）还是中（扫描→曲线族→最优高亮闭环，4-5）？草案按"中"。
3. Notebook 单元格收不收？草案：收，Tier 2 默认关。
4. ~~SP-0 前置实测是否同意~~（已完成：crash site 正常路径无罪，T6-1 倾向不立项）。
5. 里程碑顺序 M-B → M-C → M-D 是否认可，还是 M-C（生产力）提前？
6. ~~Optimizer 面板插入位置~~（已拍板：M-D 内 P6 之后作 P6b，闸门制；ADR-D7 契约同步入册）。
