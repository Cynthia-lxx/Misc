# CANN_OpHelper — 长期记忆（MEMORY）

> 跨会话稳定事实；过程细节见按日日志 `2026-09-04.md`~`2026-09-06.md`。精简重写于 2026-09-06。会话记忆一律存于 `.codebuddy\memory\`，不存别处。

## 1. 主任务：2026 CANN 上合赛初赛 BatchMatmulMaxSum（P3 完成 route③）

- 题面权威全文（一字不差存档）：`.codebuddy\memory\stage_goal_batchmatmul_maxsum.md`。内部期限 2026/10/7；官方答题至 10/16、提交截止 10/17 18:00。
- 语义：`y[b]=Σ_m max_n Σ_k x1[b,m,k]×x2[b,k,n]`（顺序不可交换）。x∈fp16/bf16、y∈fp32；K 点积与归约 fp32 累加；golden=FP64 算后转 FP32；B∈[1,64]、M/N∈[1,8192]、K∈[32,8192] 且 8 倍；B×M×K/B×N×K≤2^28；M/N 建议 16 倍，非对齐须尾块；无空/NaN/Inf/mask/broadcast。
- 正确性坑：MaxSim 初值=负无穷或行首（**禁 0**，全负行返回最大负数）；判据 fp16/bf16 双 1e-3、fp32 双 1e-4；不 modify 输入。
- 形态：题目 = **Ascend C Direct Invocation**（main.asc include kernel.asc、`extern "C" run_kernel`、`<<<>>>` 直启核），非 msopgen/fill-op。官方同构范式 = Documentation light 03.03（纯 cube）/03.05（`__mix__(1,2)` 融合）/02.05（vector 多核）。
- 载体：`p:\Dev\CANN_Learning_Workspace\Projects\bmm_p2_slice\`。实测仓库 = `p:\Dev\CANN_Learning_Workspace\Projects`（实测区，与 CANN_OpHelper 仓库分开）。父计划 = `.codebuddy\plans\PARENT_PLAN_batchmatmulmaxsum.md`。

### 阶段结论
- **P0–P1**：golden 双实现交叉背书 `bmm_maxsum_assets\`（23 用例、golden_fp64_stdlib+torch_ref，云端 20/20 PASS）；CANN_OpHelper P1 fp16 244/244 绿。
- **P2a**：case0(M=N=1) vector dot 兜底真机 PASSED；**cube 1×1 判死**（v1 GetTiling OK 但运行 507015）。
- **P2b（commit `11725a3`，分支 fix/p2b-cube-real-shape）**：真 shape B=1 M=N=K=64 全链 PASSED，diff 0.0。定案链：**ASCENDC_CUBE_ONLY=507015 真凶**（必须在 `lib/matmul_intf.h` 前 define，纯 cube 路径必须长期保留）→ 归约方向（先每行 N-max 再 M-sum=`np.max(-1)`+`np.sum(-1)`）→ cube+vector 归约全链。tiling 探针固定 **v1 no SetFixSplit**（v0(128,128) 在 64 shape FAILED 勿再试）；M==N==1 保留 dot 兜底。
- **P3 route③ bounded-C 双核（commit `32b0253`，已 FF 合入实测仓库 master 并 push；`__mix__` 融合三轮 507015 未愈后经用户裁决放弃，延后 P4）**：
  - 复用 P2b 已证 cube+vector；host 按 (batch, 64-row M block) 循环，per block `bmm_batch_cube`（64×N×Kpad→单块 GM C tile）+ `bmm_block_reduce`（行向 chunk ReduceMax≤64+Max 折叠+Add 累加，DataCopyPad 4B 写 partSum）+ per batch `bmm_part_sum` 折 y[b]。**C 中间量=单块 64×N×4B**（域边缘 17GB 内存问题根治）。
  - 数据布局零填充：x1(B,Mpad,Kpad)/x2(B,Kpad,N)，Mpad=ceil(M/64)*64、Kpad=ceil(K/16)*16（gen_data/main/kernel 三方同步）。guard：N%8==0、tiling Ka==Kb==Kpad、usedCoreNum==1 整块。
  - **生命周期修复=真机有效**：launch 完必须显式 `aclrtSynchronizeStream` 再 free scratch，否则 507015/stale C（P2b 靠 D2H dump 隐式排序）。
  - **云验 6 case 全绿**（anchor64/B8/K40→Kpad48/M1000→Mpad1024 16块/N1000 baseN256；case0/4 diff 0.0、case2/3/5=6.1e-05<1e-3）。实证：N tail 由 cube 单核 IterateAll 吸收（N=1000 baseN=256 无需切 vector）；Kpad 零填充进 cube OK（Ka=Kb=48）；M tail 由 realRows 剔 pad 行。
  - **待办（下一切片）**：N%8!=0 非单点是显式边界未覆盖（需行读对齐扩展/vector 兜底）；diff 0.0 vs 6.1e-05 并存待多 seed 复核；域边缘最大 shape（B=64、M=N=8192）未真机；融合性能优化延后 **P4**。

## 2. 角色/边界/环境
- 服务 CANN_OpHelper（Windows 10 本地 Python 工具，**辅助生成** Ascend C 算子工程模板）；本地无编译器/CANN/NPU，**禁本地编译/运行 C++**，验证全在云端 CANN Lab。
- 工作流：算子描述→msopgen 命令→云端执行取回空壳→本地文本填充→上传编译验证。禁臆造 API/JSON 字段；写 CANN 代码前先读本地官方文档/示例。
- Python：嵌入式 3.14.5（`P:\python.bat/.ps1`）；venv=`p:\Dev\CANN_Learning_Refs\penv`（无点号，禁建 .penv）；冒烟优先 `penv\Scripts\python.exe`；**pip 安装一律给用户命令并等用户执行**，AI 只做完整性验证。penv 无 numpy/torch。
- git：CANN_OpHelper 仓库根 `p:\Dev\CANN_Learning_Refs\CANN_OpHelper`（master）；实测区仓库 `p:\Dev\CANN_Learning_Workspace\Projects`（两仓库前缀均 `-c safe.directory="P:/Dev/...`）。提交格式 `<type>(<scope>): <subject>`。AI 跳过实测仓库 status 防卡顿。
- 参考只读副本 4 处同构（Documentation_for_Developers\ / CANN_Learning_Hub_(for_dev)\ / p:\Dev\CANN_Learning_Hub\ / CANN_Learning_Workspace\Reference_And_Documentation\），统一引 Documentation 根。知识沉淀 `.codebuddy\memory\ascendc_01~07_*.md` + `ascendc_knowledge_index.md`。
- 相邻实测区 `p:\Dev\CANN_Learning_Workspace\`（验证/创作工程建 `Projects\`）；其实测区 `.codebuddy\memory\` **只读可引用禁写**，复制入本区须守记忆纪律。

## 3. API 权威（cann_ascendc_headers_9.0.0）
- `p:\Dev\CANN_Learning_Refs\cann_ascendc_headers_9.0.0\` = CANN 9.0.0（aarch64-linux）真实运行时头库本地副本（2697 .h；`asc\` 与 `ascendc\include\` 同构）。**= bisheng/云端编译真正搜索的头文件**：签名/参数序/count 与 mask 语义/枚举取值一律以它为准（优先级高于任何教程/博客）；冲突改代码并注释出处。分层：`basic_api/` 接口层决定可调用形态，`basic_api/impl` 按 `__NPU_ARCH__` #include `dav_c220/…`（910B 系=2201）决定真实行为。**只读资产：禁改禁删禁入 git**；结论沉淀注明「已与头库核对」。
- 已核对结论：无 `Maximum`/`VerifyReduceMax`/`VerifyReduceSum`；元素/标量 max=`Max(dst,src0,src1,count)`/`Maxs`；whole-reduce=count 版 `ReduceMax(dst,src,tmp,count,calIndex=0)`（无 isSetMask 模板形参；ReduceSum count 版才有 `T,isSetMask=true` 默认 true）；2201 ReduceMax 走树形 whole-reduce（上限≈整 UB），MaxImpl 支持 half/float、count=1 合法。Cast Level2=`Cast(dst,src,roundMode,count)`；DataCopyPad{1,4,0,0,0} 写 4B。

## 4. 复用资产：cannbot-skills-master（2026-09-06 探矿完成）
- 位置：`p:\Dev\CANN_Learning_Refs\cannbot-skills-master\cannbot-skills-master\`（双层；7366 文件）。CANNBot = 昇腾官方 Agent Skills 仓库；许可 **CANN OSL v2.0**（学习合规；整目录复制须带 LICENSE）。只读审视。
- S 级（行级出处见 2026-09-06.md）：
  1. **507015 权威排错**：api-matmul.md——TilingHeader 放 L1/UB+GetPhyAddr 喂 Init 即 507015，修=LoadTilingFromGM 栈上；machine.md——**cube tile kL0>实际 K 越界**是高频根因、K=1 禁 cube tile。
  2. **平台路由**：910B=DAV_2201→AscendC::Matmul 高阶 API；仓库 matmul_blaze/catlass 属 950=DAV_3510 谱系，**910B 只参照文档不引其代码**。
  3. **Matmul 高阶骨架**：Init 前 `if ASCEND_IS_AIV return`/LoadTilingFromGM/SetSubBlockIdx(0)/SetHF32/Process `IterateAll`+`PipeBarrier<PIPE_AIC>` 收尾。
  4. **direct-invoke 样板**：`ops/ascendc-direct-invoke-template/references/add_custom/`（与 bmm_p2_slice 同构）。
  5. **工具**：`ops/ascendc-sync-audit/scripts/`（cube+vector 跨核 Set/Wait 扫描）；`tools/msaicerr-toolkit/scripts/preflight.sh`。
- 局限：ascendc-runtime-debug 错误码表只收 507035(AIV) 不收 507015。

## 5. CANN_OpHelper 工程现状（浓缩）
- 模块：model/yamlio/msopgen/proto/wizard/cli + `template\`（assembly 唯一事实源+engine+snippets）+ `expr\`（parse/lower/eval/presets）+ fillgen/apply/verifygen。fill-op 两阶段；gen-msopgen `-i` 决策；soc=_clean_soc；host 方法映射用字典（勿 capitalize）。
- **fillgen kernel 骨架必须含 `AscendC::TPipe pipe;` 成员**；apply=整文件重写仅三文件（LF）；verifygen 产出确定性 bin+aclnn runner+run_verify.sh（内置 soc 别名）+verify_result.py。
- P1 dtype：fillgen dtype 域 + 32B 块大/小核尾块 tiling（TilingData totalLength/bigDataNum/smallDataNum/tailBlockNum；host totalBlocks=totalLength/16…）；shape hint 驱动 verify；`<e` RNE 打包；边界：<32B 残尾 DataCopyPad 不生成、fp16 call 族真机未验证。契约→docs/expr-rules §6、official-patterns §10。
- 云端 910B 系 socVersion **161001**（ascend910_93/DAV_2201）：装包后 `cp -r` 建 `kernel/config/ascend910_93/` 别名目录（build 重装会清，run_verify.sh 已内置）。`.ps1` 存 UTF-8 with BOM。
