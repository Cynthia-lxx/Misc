# Show Heatmaps 节点升级（用户 2026-09-11 构想，**尚未落地**）

> **交接说明**：本文只记录「需求原文 + 已核实的源码事实 + 可行性结论 + 设计建议 + 实施清单 + 待拍板问题」，
> **代码一行未动**。同日已完成的 conversion 节点前端归属修复见 `2026-09-11-conversion-nodes.md` §9。
> 开工前请先读 `MEMORY.md` 的「前端节点库的分区只看 `python_module`」「节点冒烟测试器」两条，
> 以及项目硬规则（widget 默认值开箱可用 / 每次新增或改动节点后必须跑冒烟 / 4 份说明文件同步）。

## 1. 用户需求原文（照录）

> 目前，show heatmaps（d2l分类下的）节点会将整个张量完整的生成heatmap。当输入变成实际生产环境，比如说，
> 1920x1080的图片时，它将消耗非常多的时间，并且192x108与1920x1080的heatmap看起来区别并不大。
> 我想到方案：允许用户设定最大抽样数量。对于超出一个安全大小的张量，进行等间距抽样使得最终抽样密度平均
> 且总量不超过指定量。合理设计抽样算法，使得计算量足够小。
>
> 同时，无论目前输入的shape是啥样，它都会尝试生成二维的heatmap。我觉得可以改善一下功能：
> 1.支持1、2、3三种维度的heatmap，分别生成商品条码一般的、正常平面热力图一般的，一个有透视结构的
> 三维cube（对于第三种，不要将cube做成不透明的）
> 2.允许用户通过控件选择查看张量的哪几个维度，同时也可以选择自动选择；
> 3.如果是自动选择维度，给出几种选择方式：前n个，后n个，信息量最大的n个，信息量最小的n个。
> 4.（这一个可以标记成advanced widget combo，具体请查看文档）同时加上允许用户选择的错误应对策略——
> 要么直接报错，要么fallback成选择前n个维度以保持健壮性。
> 5.一切的原则——无论给这个节点加上什么功能，不要提升太多的计算量。

## 2. 现状：代码事实（全部实读，带行号）

- 文件：`ComfyDL_UI/comfydl/nodes/visualization.py`（**V1 API**，`INPUT_TYPES` + `NODE_CLASS_MAPPINGS`，非 V3）。
- 两个孪生节点，`CATEGORY = "d2l/Visualization"`（→ 前端「Extensions」区，见 MEMORY.md 的 python_module 条）：

| 节点 id | 显示名 | 变体 | 行号 |
|---|---|---|---|
| `CdlShowHeatmapsOutput` | Show Heatmaps (Output) | `OUTPUT_NODE=True`，`RETURN_TYPES=()`，只画在节点上 | 109–170 |
| `CdlShowHeatmaps` | Show Heatmaps | `RETURN_TYPES=("IMAGE",)` + `_fig_to_image_tensor(fig)` | 173–232 |

- 二者 `execute()` 主体**逐字节重复**（仅末尾 `return ()` 与 `return (result,)` 不同）
  → **任何改动都要同步两份**；建议第一步先抽模块级 helper（见 §5.1）。
- 现有 widgets：`matrices`(TENSOR) / `xlabel`(STRING "") / `ylabel`(STRING "") / `figsize_w`(FLOAT 2.5, 0.5–20)
  / `figsize_h`(FLOAT 2.5, 0.5–20) / `cmap`(STRING "Reds") + 可选 `titles`(STRING multiline，逗号分隔)。
- 形状处理（两处相同，108-108 与 200-205）：

```python
m = matrices.cpu().numpy()
if m.ndim == 2:      m = m[np.newaxis, np.newaxis, :, :]   # [H,W]   -> [1,1,H,W]
elif m.ndim == 3:    m = m[:, np.newaxis, :, :]             # [N,H,W] -> [N,1,H,W]
num_rows, num_cols = m.shape[0], m.shape[1]
pcm = ax.imshow(m[i, j], cmap=cmap)                          # 无抽样、无维度选择
```

### 2.1 已确认的 3 个缺陷

1. **无抽样**：`matrices` 1920×1080 会整幅进 `imshow`，耗时随像素数线性增长（`fig.canvas.draw()` 是大头），
   而缩略后的视觉差别很小 —— 正是用户抱怨的点。
2. **1 维 / ≥4 维没有定义行为**：
   - 1 维：两个分支都不命中 → `m.shape[1]` 直接 `IndexError`（tuple index out of range）；
   - 4 维（真实生产最常见 `[B,C,H,W]`）：被当成 `[num_rows, num_cols, H, W]`，于是画出 **B×C 张** H×W 子图
     —— 语义完全错（把 batch/channel 当成了子图行列），且**子图数量放大耗时**。这就是用户说的
     "无论输入什么 shape 都尝试生成二维 heatmap"；
   - ≥5 维：`m[i, j]` 仍是 3D → `imshow` 抛 `Invalid shape ... for image data`。
3. 全量 `imshow` 时 matplotlib 仍持有整幅数据（只降采样显示、不降内存）→ 大图内存峰值同样高。

### 2.2 成本口径（优化前后对比用）

- `penv\Scripts\python.exe cdl_smoke_tests\run_smoke_test.py --filter CdlShowHeatmaps`（现 dummy `TENSOR` 是 2D，见 §5.7）；
- 自检脚本里用 `time.perf_counter()` 包住 `execute()` / `_fig_to_image_tensor(fig)`，对 1920×1080 与 192×108 各测一次。

## 3. 可行性关键结论（已核实，别重复踩坑）

1. **`advanced` 对 V1 节点可用，不需要迁移到 V3**（这是需求 4 的前提）：
   - 官方参照：`ComfyUI-original/nodes.py:347` 等大量 **V1** 节点写作
     `("INT", {"default": 512, "min": 64, "max": 4096, "step": 32, "advanced": True})`；
   - 宿主：`ComfyDL_UI/server.py:756` 对 V1 节点直接 `info['input'] = obj_class.INPUT_TYPES()` ——
     **INPUT_TYPES 逐字下发**，不做字段白名单过滤；
   - 前端（`settingStore-KkBYyEnh.js`）：`isWidgetVisible(e,t,n=!1){let r=e.hidden??!1,i=e.advanced??!1;…}`，
     入参来自 `mergedOptions = {...widget.options, ...widgetState.options}`；另有全局设置
     `Comfy.Node.AlwaysShowAdvancedWidgets` 和节点级 `showAdvanced`；
   - 文档依据：`Comfy_CustomNode_Docs_md/custom-nodes/v3_migration.md:433`（`advanced`：隐藏在一个
     "Advanced" 折叠开关后面）。
   → 所以照 V1 语法写 `("COMBO", {"options": [...], "default": ..., "advanced": True})` 即可。
2. 若**将来**想让这两个节点也进前端「Comfy」区（核心分区），按 `python_module` 首段白名单规则加
   `PYTHON_MODULE` 即可；**本次不需要**（用户说的是 d2l 分类下的节点，留在 Extensions 区是对的）。
3. **抽样开销可以做到≈0**：`t[..., ::s, ::s]`、`t[:k]`、`t[..., idx]` 都是 **view**，
   只有当 `torch→numpy` 时才按抽样后的尺寸拷贝；千万不要在大张量上做 `reshape`/`contiguous`/算术。
4. 真正贵的只有 matplotlib：`num_rows × num_cols` 个子图数、每个子图的像素数，以及 `fig.canvas.draw()`。
   → 抽样降的是**每子图像素数**，"维度选择/降维"降的是**子图数量**，二者都要做（需求 1+2）。

## 4. 设计建议（待与用户确认，非最终方案）

### 4.1 抽样（需求 1）

- 新增 widget `max_samples`：INT，`min=0`（**0 = 不抽样**，保留旧行为），建议 `default` 取一个安全值
  （如 65536 或 262144），`max` 放到百万级，`step` 取 2 的幂或 1024。
- 抽样语义：**参与渲染的元素总数 ≤ max_samples**，各轴**等间距**抽样，且**各轴抽样密度尽量平均**；
  确定性（同输入同结果）、O(ndim) 计算、不触碰数据。
  算法骨架（示意，实现时写清每轴分配公式）：
  ```
  若 max_samples == 0 或 numel <= max_samples: 不抽样
  否则：把预算按各轴长度加权分配到 k_i（每轴 k_i >= 1），step_i = ceil(size_i / k_i)
       → 每轴切片 ::step_i（view），实际总量 <= budget
  ```
  必须保证：① 乘积 ≤ 预算；② 每轴至少 1 个元素；③ 各轴密度接近（避免某轴抽 1 个、另一轴全保留）；
  ④ 只对大轴抽样（小轴 step 恒为 1）。
- ⚠ 待定：纯 stride 抽样（成本≈0，可能引入 aliasing）vs 平均池化下采样（更"平均"但要复制数据、成本更高）。
  用户原话只要求"等间距抽样 + 计算量足够小"，**default 建议纯 stride**，池化可作为可选策略。

### 4.2 维度与维数（需求 1、2、3）

- **1D** → "商品条码"：整条 `(1, N)` 色带，`imshow` 用 `aspect="auto"` 或 `pcolormesh`，关掉 y 轴刻度。
- **2D** → 现状的平面热力图（保持不变）。
- **3D** → 有透视结构的 cube，**不要不透明**（两个候选，需用户拍板）：
  - **A（推荐，成本低）**：`projection="3d"` 轴 + 3 个正交切面（mid-slice 或用户指定索引），
    `alpha≈0.5~0.7`，`plot_surface` / 3D 上的 `imshow`；
  - **B**：`ax.voxels` 半透明体素 —— 成本高（体素数 × 三角面），必须严格受 `max_samples` 约束，
    或仅在体积很小时启用。用户只说"不要不透明"，**A 更符合"不提升太多计算量"的总原则**。
- **维度选择 widget**：
  - `dims` COMBO：`"auto"` + 手动轴组合（建议允许负数下标、逗号分隔、接受 1~3 个轴，如 `"0,1"` / `"-2,-1"` / `"1"`）；
  - **未被选中的轴如何降维 —— 用户没提，必须补**：建议 COMBO `axis_reduce`，取值
    `mean`(默认，成本 O(numel)) / `max` / `first`(索引 0，纯 view、最省) / `mid`(中间索引)；
    若选中的轴数已 ≥ 输入秩则不降维（1D 由 1 个轴直接得到）。
- **自动策略** COMBO `auto_select`（需求 3）：`first_n` / `last_n` / `most_informative_n` / `least_informative_n`
  （用户措辞："前n个，后n个，信息量最大的n个，信息量最小的n个"）。
  与维数配套：需要 n（= 生成 1D/2D/3D 所需的轴数，默认 2），建议 `auto_n` INT 或与 `dims` 联动；
  - **信息量度量建议**：逐轴方差（把其余轴 reduce 掉，`t.var(dim=...)`），成本 O(numel)；
    为了守住"不提升太多计算量"，可在**抽样后的张量**上计算（近似估计），并在 docstring 里注明是抽样估计。
  - 需要定义边界语义：秩 < n 时怎么办（报错 / 退回全部轴）；`most/least_informative` 在含非浮点 dtype 时的行为；
    空张量、0 维标量、单元素轴等退化情形（现有节点对 0 维同样会炸，可一并给明确报错信息）。
- **错误策略** COMBO `on_error`（需求 4，**`advanced=True`**）：取值建议
  `"error"`（直接抛错，信息里说清"为什么这个 shape 不行"）/ `"fallback_first_n"`（退化为选前 n 个轴，保持健壮）。
  ⚠ 默认取哪个要问用户（从"保持健壮性"的措辞看，`fallback_first_n` 可能更合用户意，但会掩盖错误）。

### 4.3 与既有 widget 的关系（向后兼容）

- `figsize_w/h` 是"每子图"尺寸：抽样后子图数不变、单图更小 → 渲染更快；**不要**把 4D 再展开成 B×C 子图。
- **旧行为必须可复现**：2D/3D 输入 + `max_samples=0` + `dims=auto` 时，输出外观应与现在**完全一致**，
  否则用户既有 workflow 的观感会被改动（这类"视觉回归"很难被发现，务必显式验证）。
- 新增 widget 的默认值要"开箱可用"（项目硬规则）：默认档位下应当**更慢的旧行为被新的安全行为取代**，
  即默认就抽样（这也是用户痛点），但 `0` 仍保留全量逃生口。

## 5. 实施清单（建议顺序）

1. **零行为变化重构**：抽模块级 helper（`_sample_tensor_axes()` / `_select_axes()` / `_render_heatmaps()`），
   两个孪生节点共用；先跑冒烟确认 224/22/0 不变，再往下做。
2. 加抽样（§4.1）+ 维度选择与降维（§4.2 的 `dims` / `axis_reduce`）。
3. 加 1D / 3D 渲染分支（方案 A/B 待用户拍板）。
4. 加自动策略（`auto_select` + `auto_n` + 信息量度量）与错误策略 `on_error`（`advanced=True`）。
5. 补 docstring（项目硬规则：干什么 / 入什么 / 出什么，含 shape 语义与降维规则）。
6. **4 份说明文件同步**：`comfydl/FUNCTIONS.md`（Show Heatmaps 两节在 1074–1098 行附近，含 widget 表）、
   `comfydl/FUNCTIONS_zh.md`（1073–1093 附近）、`comfydl/README.md`、`comfydl/README_zh.md`
   （节点数与分类数不变则只改节点说明；**若真的增减了节点/分类，四处计数必须同步**）。
7. 冒烟与测试：
   - `penv\Scripts\python.exe cdl_smoke_tests\run_smoke_test.py --filter CdlShowHeatmaps`；
   - 现有 `TENSOR` dummy 是 `torch.randn(2, 3)`（`run_smoke_test.py:178-181`）→ **只覆盖 2D 路径**。
     新增分支需要 `_INPUT_OVERRIDES` 补形态 dummy（至少把 **4D `[B,C,H,W]`** 写进去，那是用户的生产场景；
     1D/3D 可用临时自检脚本覆盖，**自检脚本用完即删**，项目规则要求及时清理临时文件）；
   - 性能自检：1920×1080 与 192×108 的耗时对比（改前/改后各一次），断言抽样后耗时显著下降。
8. 提交与同步：子模块 `feat(visualization): …` → `git push origin main` → 主仓 `git add comfydl` 更新 gitlink
   （`chore(submodule): bump …`）→ push master → 独立 checkout `P:\Dev\ComfyUI_Refs\ComfyDL` 执行 `git pull`。
   所有 git 命令带 `-c safe.directory=…`（见 MEMORY.md）。

## 6. 需要用户拍板的问题（**动手前先问**）→ 2026-09-11 已拍板，答案如下

> 用户明确要求：**本轮（2026-09-11 下半场）不做热力图升级**，先做"清理类待办 + SpectralNorm 默认值 + 归一化导出批统计量"。
> 下列答案在本轮开工前已拍板并落盘，**下一轮直接按其施工，不要再重复提问**。

1. **3D 渲染 → A 和 C 都做，做成两个不同的节点**
   - **节点 A**：matplotlib 渲染的**半透明玻璃/亚克力立方体**（正交半透明切面 + 浅色棱边），输出 `IMAGE`；零新依赖、算力最低，不触碰脱水边界。
   - **节点 C**：**真 3D 对象**节点，把 1 / 2 / 3 维都搓成 3D：
     - 1 维 → **可拖动的、平放着的条形码状渐变色条带**（不要完全 0 厚度，要留一点厚度，并**明确标注它是 1 维**）
     - 2 维 → 同理（1、2 维都可以做成**全不透明**的）
     - 3 维 → 按前述要求做**半透明立方体**
   - 总原则："**以最大化压缩计算量为主**"；可创建真 3D 对象的前提是"不太吃算力"，可参考 Comfy 文档与 Comfy 原生 3D API 源码。
   - 已核实：`comfy_extras/nodes_load_3d.py`（`Load3D` / `Preview3D`）**不在脱水白名单**（`nodes.py:679-700`），
     仅 `comfy_api/latest/_io.py` + `comfy_api/latest/_util/geometry_types.py` 保留了类型骨架
     → 走真 3D 需要把 3D 节点重新请回，成本最高，**需在下一轮单独评估**（与"压缩算力"存在张力）。
2. `max_samples` 抽样上限默认值 = **262144（约 512²）**；`0` 一律保留为"不抽样"逃生口。
3. 未被选中的轴如何降维：**仍未拍板**（候选 `mean` / `max` / `first` / `mid`）。
4. `auto` 的"信息量"度量与 n 的来源：**仍未拍板**。
5. 抽样是否需要 anti-alias（池化）档位：**仍未拍板**（倾向纯 stride）。
6. `on_error`（`advanced=True`）默认值 = **`fallback_first_n`**（选不出合法维度时退化为取前 n 个轴，工作流不中断，符合"健壮性"诉求）。
