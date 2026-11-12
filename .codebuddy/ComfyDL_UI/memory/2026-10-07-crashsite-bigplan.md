# Crash site 大计划（bigplan）——工作流执行现场的快照与恢复

> 状态：**制作中（2026-10-07）**——待用户最后声明收工后本阶段结束；每 Phase 走
> PLAN MODE smallplan + (CRAFT)*n。spike 可行性已全部实证（`2026-10-07.md` §2）。

## 1. 用户需求原话存档

- "类似 git-fire 的功能——当用户想要叫停工作流时，分析当前正在执行的节点，得出有
  哪些数据正在流动，后方的过程和节点需要哪些数据，汇总成表后让用户选择保存的内容
  和保存的形式（比如序列化成数据库，或者按照数据类型 supposed 的文件格式（.sft，
  .pt，.txt，.png，.json，.dll，.db）进行保存），确保当前 site 被保留后杀死工作流
  进程，以及反过来的恢复操作。"
- "所有数据装进单个数据库文件……可以是分文件保存的 alternative。"
- 命名 **crash site**：三场景统一——执行报错时的数据抢救、用户主动打断并保存、
  主动快照。"一键保存出一个数据库，也能详细到列出所有在野数据并允许用户从中选择
  全部或一部分，保存为单个数据库或多个文件"；多文件时另存 manifest 指出"到哪了，
  要哪些数据/文件"，恢复时服务器自动选择或让用户提供（跨机场景）。
- 衍生功能：**执行到一半篡改数据流看效果**（what-if）。
- 拍板：单库与多文件都做；`.dll` 未回复（按笔误处理，类型表不含）。

## 2. 事实基础（两轮调研 + spike，引用 `2026-10-07.md` §2）

- **spike 实证**：报错后 caches.outputs 存活（窗口到下次 set_prompt）；同 prompt
  重 Queue 宿主自动跳过已完成节点；缓存判定 **hash 驱动**（注入 CacheEntry 不传播
  → tamper 正解 = prompt 改写 + 常量注入节点）；不同 prompt 入队即清旧条目。
- **宿主正道（本轮调研核心发现）**：`comfy_api/latest/_caching.py` 的
  **CacheProvider**（register_cache_provider）：`on_store`（每节点 outputs.set 后
  异步回调=增量 capture）、`on_lookup`（本地 miss 时回填=恢复注入）、`should_cache`
  （类型过滤）、`on_prompt_start/end`；键=输入签名 SHA256（跨会话确定，与 prompt_id
  无关）；仅 outputs cache 启用；provider 异常被吞不破坏执行。
- **cache_type 边界**：CLASSIC/LRU 旧条目清于下次 set_prompt；RAM_PRESSURE 报错后
  eviction 停（finally 拆回调）但执行中 ≥512MB CPU tensor 可被 active release 逐
  （增量 capture 消除此窗口）；**NONE（--cache-none）模式 NullCache 不走 provider**
  ——本计划声明不支持 NONE。
- **GPU tensor**：cache 内不自动 .cpu()（VAE intermediate_device）——序列化前
  `.detach().cpu()`；inference_mode 包裹全程（clone 防视图失效）。
- **execution_error 仅发 client_id**（broadcast=False）→ 报错抢救必须服务端捕获，
  不能靠前端事件。
- **subgraph**：子图内部节点在 ephemeral subcache，仅可恢复 expand 节点自身输出
  （诚实边界）。
- **序列化**：safetensors.torch.save/load 返回 bytes 的先例在
  `comfy/training_protocol.py:949/1001`；torch.load 统一 `weights_only=True,
  map_location="cpu"`；safetensors 只接受扁平 dict[str, Tensor]，非 tensor 项走
  JSON sidecar / __metadata__（str only）。
- **快照库落点**：`user/comfydl/snapshots/<snapshot_id>.db`（folder_paths.
  get_user_directory 拼接，无冲突）；std sqlite3 直用（不走 SQLAlchemy）。

## 3. 类型 → 格式映射（ADR-4，调研建议表为准）

| 类型 | 默认格式 | 完整度 | 备注 |
|---|---|---|---|
| LATENT / TENSOR / SIGMAS / IMAGE / MASK | safetensors BLOB | 完整 | IMAGE 恢复保证 cpu/float32；png 留给导出视图（float32 有损） |
| AUDIO | safetensors + JSON | 完整 | sampler_rate/sample_rate 双键名兼容 |
| STRING/INT/FLOAT/BOOL | JSON | 完整 | |
| cdlVocab | JSON | 完整 | 纯 dict |
| DATASET (CdlDataset) | safetensors + JSON sidecar | 完整 | features/labels + names/target/meta |
| CONDITIONING | safetensors 展平 + JSON | 降级 | dict 含非 tensor 项时部分保存 |
| MODEL / CLIP / VAE / CLIP_VISION / CONTROL_NET | safetensors（state_dict 路线，CheckpointSave 先例） | **降级** | 丢 patches/hooks/tokenizer 等；should_cache 可配置拒收或降级保存（默认：拒收 MODEL，其余降级保存+诚实标注） |
| nn_model | torch.save BLOB（weights_only=True） | 降级 | 需节点 spec 重建（regression_train 先例） |
| cdlDataloader | — | **不可** | UI 标注"恢复时由 DATASET 重建" |

## 4. ADR 决策

- **ADR-1 独立体系**：crash site 是数据安全功能，独立于 profiling（独立模块
  `app/crashsite/` + 独立侧栏 tab "Crash Site"）；profiling master switch 停用
  不影响它。
- **ADR-2 核心架构 = CacheProvider**：增量 capture（on_store 即时序列化落盘，
  消除抢救窗口与 RAM eviction 风险）+ 恢复 lazy lookup（on_lookup 从库回填）。
  手动 set caches.outputs 仅作 spike 遗产不采用；NONE cache 模式不支持（文档声明）。
- **ADR-3 单 SQLite 库为主形态**：`snapshots(id, created_at, prompt_id,
  prompt_json, note)` + `node_outputs(snapshot_id, node_id, slot, output_type,
  format, size_bytes, sha256, data BLOB)` + `meta`；多文件 = 导出视图（manifest 与
  单库 manifest 同构）；std sqlite3 直开。
- **ADR-4 类型映射**：§3 表为准；不可救/降级类型在汇总表诚实标注。
- **ADR-5 tamper = prompt 改写 + 常量注入节点**：恢复预览里对任意已完成节点提供
  "替换"——实现为自动改写 prompt 链接 + typed 常量节点，下游 hash 变化从注入点
  重算（spike 实证路径）。
- **ADR-6 capture 三源**：①provider.on_store 增量（主力）；②报错/interrupt 收割
  （execute_async 两处 handle_execution_error 后 break 前——provider 已覆盖大部分，
  此处兜底记录 error 元数据）；③主动快照端点（空闲时全量收割当前 cache）。
- **ADR-7 恢复 = 重 Queue 原 prompt**（POST /prompt 原工作流 JSON），键=输入签名
  自动命中；建议复用原 prompt_id 便于前端对账；画布改动导致签名 miss 时明确提示。

## 5. Phase 分解

### M1 核心闭环（估 2-3 craft）
任务：SnapshotProvider（on_store 增量写库 + should_cache 过滤 + on_prompt_end 收
尾标记）；snapshot 库 schema + sqlite3 封装（`app/crashsite/store.py`）；capture
端点（GET /comfydl/crashsite/snapshots 列表、POST capture）；恢复（重 Queue 由前
端发起 + provider.on_lookup 自动命中）；最小 UI（独立 tab：快照列表 + 恢复按钮 +
运行中"保存现场"按钮）。
验收：真启动 → 跑模板 → 中途打断 → 快照库有全部已完成节点 → 重 Queue 秒回且仅重
算剩余节点；报错场景 error 元数据入 manifest；P/F 全量冒烟不回退。
风险：provider 接口版本耦合（comfy_api.latest）→ 封装隔离 + 版本探测降级。

### M2 汇总表与多形态（估 3-4 craft）
任务：汇总表 UI（在野数据清单：node/type/size/format/下游依赖标记/勾选）；选择性
保存（单库或多文件 + manifest）；跨机导入（.db 导入 / manifest + 用户提供文件，
缺失项标红）；导出目录视图；tamper UI（恢复预览 + 常量注入节点族 + 替换预览）。
验收：选择子集保存→恢复→重算范围恰为勾选节点下游；跨机 manifest 流程走通；tamper
后下游拿到新值且未触及节点零重算（冒烟断言）。

### M3 训练 checkpoint 并入（估 2-3 craft，与断点续训方向合流）
任务：训练类节点（regression/LM train）周期 checkpoint 接入 crash site 库（epoch
边界保存 optimizer/step/RNG）；恢复时 train 节点从 checkpoint 续跑而非重头。
验收：LM 训练打断 → 恢复 → 从断点 epoch 继续，曲线连续。

### Backlog
ModelPatcher 完整往返（patches/hooks 序列化研究）、subgraph 内部节点恢复、
NONE cache 模式支持、.vsdx 类外部工具消费。

## 6. 待派 subagent 清单（smallplan 时使用）
1. comfy_api.latest Caching provider 注册的运行时细节（注册时机/多 provider 共存/
   测试钩子）——M1 smallplan 前置；
2. 前端独立 tab 与恢复交互的宿主范式（registerSidebarTab 已熟；新点 = 文件选择/
   导入对话框范式）——M2 前置。

## 7. 工作量与层级定位
idea（本文件）→ 每 Phase smallplan → craft；总计约 7-10 个 craft 会话。
