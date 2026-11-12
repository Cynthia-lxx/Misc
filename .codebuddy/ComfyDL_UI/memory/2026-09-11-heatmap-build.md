# 2026-09-11 heatmap 施工（Show Heatmaps 重构 + 真 3D 导出 + Preview3D 薄节点）

> 上游设计见 `2026-09-11-heatmap-next.md`（拍板）与本日 MEMORY.md「heatmap 拍板 + 落地」。本文只记施工与验证事实。

## 交付物

| 文件 | 状态 | 说明 |
|------|------|------|
| `comfydl/nodes/visualization.py` | 改 | 抽出模块级 helper；两孪生节点 +6 控件 +1/2/3D 渲染分支 |
| `comfydl/nodes/mesh3d.py` | 新 | `CdlHeatmapsTo3D`：OBJ+MTL 真 3D 导出（≈465 行） |
| `comfy_extras/nodes_preview_3d.py` | 新 | 仅注册 `Preview3D`（`node_id="Preview3D"`，V3 `ComfyExtension`） |
| `nodes.py` | 改 | `extras_files` 白名单加入 `nodes_preview_3d.py` |
| `cdl_smoke_tests/run_smoke_test.py` | 改 | 两个新节点覆盖 + 抽样性能自检 + OBJ/MTL 落盘断言 |
| `comfydl/FUNCTIONS(.zh).md`、`README(.zh).md` | 改 | Show Heatmaps 两节重写 + `Heatmaps to 3D` 新节 + 计数同步 |

## 关键设计落地

- **抽样 = 纯 view**：`step = ceil(numel/max_samples)`，各轴统一 `::step`；`Π ceil(size_i/step)` 仍超预算时用收紧 `step` 的确定式循环补刀。`max_samples=0` 或未超预算 → 不抽样。全流程无 `reshape`/`contiguous`/算术。
- **1/2/3D 渲染**：1D → `aspect="auto"` 色带（隐藏 y 刻度）；2D → `imshow`；3D → `projection="3d"` + 正交中截面 + 框架线（**不用 `ax.voxels`**，太慢）。
- **`titles` 语义变更**：旧的"多子图阵列"取消 → 单张图标题。旧的 2D 行为在 `max_samples=0 + dims=auto` 下逐像素一致（已 `torch.equal` 验证）。
- **3D 网格**：颜色量化 64 级 → 64 个 `newmtl` + 1 个 `m_label`；`d` 承载透明度（OBJ 顶点色无 alpha）。1D 256 格 / 2D 每轴 64 / 3D 每轴 32；3D = 6 外表面 + 3 中截面（双面）。
- **绕序**：three.js 默认 `FrontSide` → 反面不可见；`_MeshBuilder` 用 `dot(normal, outward) < 0` 自动反转。用"有向体积 == 理论体积"验证绕序（见下）。
- **`save_to` 覆写**：`Preview3D` 会把 `Types.File3D` 重命名为 `preview3d_<uuid>.obj` → 子类 `save_to` 同目录写同名 `.mtl`，并把 `mtllib <最终名>` 写入 OBJ 首行。

## 验证证据

**临时校验脚本（已删除）**：
- 绕序/有向体积：1D 色带 `= n·1·thickness`、2D 平板 `= cols·rows·thickness`、3D 立方体 `= d0·d1·d2`，全部**精确相等**（相对误差 < 1e-6）→ 外法线绕序正确。
- 每个秩（1D/2D/3D×3/4D/5D）导出的 OBJ：面全为四边形、索引在界内、顶点有限、`usemtl` ⊆ MTL 的 `newmtl`、`usemtl` 与 `g` 集合一致。
- 退化输入：常数张量、全 NaN、未知 cmap、`(0,)`/`(0,4)`/`(0,0,4)` 空张量 → 均正常产出。
- 成本：1080p 输入 35–100 ms / 130–590 KiB OBJ。

**冒烟器**：`226 PASS / 22 SKIP / 0 FAIL`（原基线 224 → 新增 2 节点）。新增断言：
- `_check_heatmap_ranks`（对 `CdlShowHeatmaps` / `...Output`）：1D–5D 全部渲染为单张 RGB 有限图像、`dims` 越界时 `error` 必须抛 / `fallback_first_n` 必须不抛、**抽样必须比不抽样更快**（实测 0.110 s vs 0.360 s @1080p）。
- `_check_heatmaps_to_3d`：每个秩导出后 `save_to` → `.obj` 与同名 `.mtl` 都在、OBJ 首行 `mtllib` 正确、`opacity=0.25` → MTL 含 `d 0.2500`。
- `_check_preview3d`：`Preview3D` 收到 `Types.File3D` 后 output 目录真的出现 `preview3d_*.obj`；收到 `CdlHeatmapsTo3D` 的负载时 **兄弟 `.mtl` 同步落盘**（即本次最关键的回归点）。
- 冒烟器新增能力：`_REGISTRY`（main 填充）+ `functools.partial` 绑定 node_id 的 `_OUTPUT_CHECKS` 回调、`_VALUE_FACTORIES` 增 `FILE_3D` / `FILE_3D_OBJ`。

**计数同步**（`_update_readme.py --check` 全绿）：ComfyDL banner 108→**109**/20；宿主注册表 246→**248**/38；四份说明"内置节点库" 140→**141**/25→**26**（核心 32→33，新增 `3d/Preview 3D`）。README 总句为手改项（脚本不管）。

## Git

- 子模块 `ComfyDL`：`123530e..0a49873`（main）— `feat(visualization): sample, reduce and render 1/2/3-D heatmaps; add OBJ 3D export`
- 主仓 `ComfyDL_UI`：`451f66c..bbc2a64`（master）— `feat(extras): add a bare Preview3D node as the 3D export target`
- 独立 checkout `P:\Dev\ComfyUI_Refs\ComfyDL` 已 `git pull` 到 `0a49873`，`nodes/mesh3d.py` 在位。

## 遗留

- `comfydl/example_workflows/ComfyDL All-Nodes Demo.json` 未纳入 `Heatmaps to 3D`（非硬规则要求，计划目录结构也未列）。
