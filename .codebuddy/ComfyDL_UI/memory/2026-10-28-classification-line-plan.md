# Classification Line 大计划（正式版 v1，2026-10-28 定稿）

> 状态：**已定稿，未动工**（用户 ASK 模式拍板「先试试主线 2」，本文为 PLAN 产物）。
> 前置铁律：**算子/节点双形态原则**（2026-10-28 新立）——任何算子/节点默认同时交付
> 「最大生产力一盒装」与「细粒度教科书级」两种形态。本线是第一条按该原则立项的线。
> 环境约束：F 盘离线 → 本计划全部在 P 盘 CPU 版执行与验证；GPU 冒烟待环境回归后补。

## 0. 对标关系（设计总图）

| 回归线（已有，对标模板） | 分类线（本计划新增） | 形态 |
|---|---|---|
| `CdlFormulaDataGen` | `CdlClassificationDataGen` | 数据生成 |
| `CdlLinRegTrain`（四端口） | `CdlSoftmaxTrain`（五端口） | 教科书级 |
| `CdlRegressionTrain` | `CdlClassificationTrain` | 最大生产力 |
| `CdlRegressionModelLoad` | `CdlClassificationModelLoad` | 持久化 |
| —（回归不需要） | `CdlConfusionMatrix` | 评估件 |
| 模板 ×2 | 模板 ×2 | 内容 |

合计 **5 新节点 / 2 新模板**；`d2l/Training` 3→6，数据生成归 `d2l/Datasets`。

## 1. ADR（已定稿）

- **ADR-1 标签语义 v1 = 类索引**：y 约定为 `[n]`（或 `[n,1]` 压平）的 float/int tensor，
  训练前 `round → long`；DATASET.labels 恒为 float，同样 round。**字符串标签→idx 映射推迟**
  （需动 `CdlReadCSV` 的 dtype 路径，另立项）。`CdlClassificationDataGen` 直接产出 long。
- **ADR-2 生产分类器复用 `_Regressor`**：分类器 = 同构 MLP（out=k）+ `F.cross_entropy` 头，
  **不新建模型类**；标准 z-score buffer 机制原样继承（nn_model 自包含）。同包内 import，
  `regression_train.py` 与 `classification_train.py` 分文件但共享 `_Regressor/_parse_hidden/_EarlyStop`。
- **ADR-3 stratified split**：与回归版的随机 perm 不同，分类版按类比例分层切分
  （每类各自 randperm 后按 test_size 抽取），保证验证集各类非空。test_size=0 时不切分（同回归）。
- **ADR-4 早停监控量可切**：widget `early_stop_metric`（loss/accuracy，默认 **loss**——
  accuracy 阶梯状会误触 patience；`_EarlyStop` 现为「越小越好」语义，accuracy 路径传 1-acc 或镜像取负）。
- **ADR-5 混淆矩阵可视化零新节点**：`CdlConfusionMatrix` 只产出矩阵 TENSOR（+行归一化+accuracy），
  渲染交给现有 `CdlHeatmaps`（labels 逗号串→titles）。端口最少化。
- **ADR-6 数据生成手写 torch，不引 sklearn**：blobs（各簇高斯）/moons（两半圆+noise）/
  circles（同心环+noise）三模式数学都很短；输出 X float / y long / DATASET 三端口。
- **ADR-7 教科书版做成**：`CdlSoftmaxTrain` 是本线教学灵魂（d2l 第 3 章 softmax regression），
  手写 `exp → normalize → NLL` 链条，`F.cross_entropy` 做对照断言。
- **ADR-8 指标只出 overall accuracy**：precision/recall/F1 不设端口，通过混淆矩阵输出自算
  （教学上更好——用户看得见矩阵才算学会）。

## 2. 节点规格

### P0 `CdlClassificationDataGen`（d2l/Datasets）
- Widgets：`mode`（blobs/moons/circles，默认 blobs）/ `n_per_class`（INT，默认 200）/ `noise`
  （FLOAT，默认 0.1）/ `seed`（INT，默认 0）/ `n_classes`（INT，默认 3，仅 blobs/circles 生效，moons 恒 2）。
- 输出：`X`（TENSOR [n,f]）、`y`（TENSOR [n] long）、`dataset`（DATASET，labels 存 float 便于通用）。
- 默认值必须开箱可出有效图。

### P1 `CdlSoftmaxTrain`（d2l/Training，教科书级）
- 输入：X、y（类索引）、lr、steps、seed（全批量 GD，无 batch/早停——教科书要的是透明）。
- 输出：`W [k,f]` / `b [k]` / `logits [n,k]` / `probabilities [n,k]` / `loss_history [steps]`。
- 手写 softmax 交叉熵；docstring 讲清「CE 内置 log_softmax、为何不等价于先 softmax 再 NLL」。

### P2 `CdlClassificationTrain`（d2l/Training，生产力）+ `CdlClassificationModelLoad`
- 输入：X/y（required 回退）+ dataset（optional forceInput，DATASET 胜出）；widgets 对齐回归版：
  test_size / standardize / hidden（默认 `"32"`，分类需要非线性）/ activation / steps / batch_size
  （0=full）/ lr / seed / early_stop_patience / early_stop_min_delta / **early_stop_metric** / save_path。
- 输出五端口：`nn_model` / `accuracy`（标量）/ `predictions`（类索引 [n]）/ `probabilities` [n,k] /
  `loss_history`。
- 内部机制照抄回归版全套：inference_mode(False) 全程作用域、ProgressBar 每步恰一次 +
  LossCurvePreviewer、早停滑动窗+回滚、**断点续训（M3 checkpoint，run_key widgets 含新参）**。
- Load 节点与回归版同码薄壳（`core.0.weight` 识别一致），输出 info 里注明 classifier。

### P3 `CdlConfusionMatrix`（d2l/TorchOps）
- 输入：`y_true`（TENSOR）、`y_pred`（TENSOR，接 trainer predictions）；widgets：`num_classes`
  （INT，0=自动 max+1）、`labels`（STRING 逗号分隔显示名，默认空）。
- 输出：`matrix [k,k]`（行=真实，列=预测）/ `normalized`（行归一化）/ `accuracy`（标量）。
- 溢出/负索引类标签抛可读 ValueError。

### P3 模板 ×2
- `classification_from_scratch`：DataGen → SoftmaxTrain → ConfusionMatrix → Heatmap。
- `tabular_classification_production`：ReadCSV → DATASET → ClassificationTrain → ConfusionMatrix →
  Heatmap + WriteCSV 导出（predictions/probabilities）。
- 已知模板坑照单全收：seed 隐形 `control_after_generate` 槽补 `"fixed"`、required 槽不留空、
  widgets_values 按 INPUT_TYPES 序、封面 PNG 留待用户亲截（先占位）。

## 3. Phase 划分（PLAN MODE smallplan → CRAFT 序列）

| Phase | 内容 | 估量 | 验收 |
|---|---|---|---|
| **P0** | DataGen 节点 + 专项测试开头 | 0.5 craft | 三模式 shape/簇中心断言、同 seed 复现 |
| **P1** | `CdlSoftmaxTrain` | 1 craft | loss 单调下降（凸问题）、与 `F.cross_entropy` 逐位对照、W/b 手验 |
| **P2** | `CdlClassificationTrain` + ModelLoad | 1.5 craft | acc 上升、stratified 比例守恒、早停回滚、断点续训（M3 hook 范式）、save/load 往返 |
| **P3** | `CdlConfusionMatrix` + 模板 ×2 | 1 craft | 矩阵行和=样本数、对角=各类正确数、模板执行器真跑 PASS |
| **P4** | 四份说明文件 + locales + 计数 + 全量冒烟 + 提交推送 | 0.5 craft | 冒烟 0 FAIL、`gen_locales --check` 绿 |

总计 **4.5 craft**（P0–P2 可先行，P3 依赖 P1/P2 产出，P4 收尾）。

## 4. 测试矩阵（`cdl_smoke_tests/test_classification.py` 专项 + 主冒烟 `_OUTPUT_CHECKS`）

1. DataGen：三模式 shape/dtype/类均衡、同 seed 确定性、moons 恒 2 类。
2. SoftmaxTrain：loss 递减、`F.cross_entropy` 对照、probabilities 行和=1、W/b 梯度方向手验。
3. ClassificationTrain：acc 上升、同 seed 复现、`_rerun` 一致性（全量传参）、inference-mode 纪律
   （模板执行器宿主保真路径）、断点续训 resume/fresh-run 双路径、stratified 断言。
4. ConfusionMatrix：行和守恒、对角守恒、num_classes 自动推断、溢出标签报错、labels→titles 传递。
5. Load 往返：save→load→forward 逐位一致、info 字符串、坏文件可读报错。
6. 模板执行器：两模板真跑（继承 `test_template_workflows.py` 迷你执行器）。

## 5. 文档与计数

- 四件套同步：`FUNCTIONS.md` / `FUNCTIONS_zh.md` / `README.md` / `README_zh.md`（`_update_readme.py`
  + **README 总句手改**——脚本不管 "provides N nodes" 句）。
- 计数：ComfyDL 125→130 / 分类 21→22（或按当时权威口径重算）；总库 223→228。
- `gen_locales.py --check`：新节点中文条目进 `locales/zh/nodeDefs.json`。
- docstring 全英文、分 `What/In/Out/Errors` 段（comfydl 惯例）。

## 6. 风险与已知坑（引用既有踩坑，动工前重读）

- 坑 26：宿主 inference_mode 纪律——物化 clone→统计→建模→训练全程一个作用域（回归版已验证照抄）。
- 坑 27：模板 seed 隐形 widget 槽位错位。
- CE 对未标准化特征敏感度低于 MSE，但 standardize 默认 yes 保持一致；lr 默认建议 `3e-2`
  （CE+Adam 在 1e-1 下小数据集偶发震荡，CRAFT 时以实测定）。
- `_EarlyStop` 语义是「越小越好」，accuracy 监控需镜像（传 `1-acc`）。
- F 盘离线：GPU 冒烟欠账在案，等环境回归补跑；提交推送不受影响（origin 在 GitHub）。

## 7. 明确不做（本线边界）

- 字符串标签→idx（动 CSV 读取，另立项）。
- precision/recall/F1 端口（ADR-8，经混淆矩阵自算）。
- class weights / 类不平衡处理（留待真实需求）。
- 图像分类（CNN trainer 属教学线 1c，另立项）。
