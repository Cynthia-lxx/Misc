# NTLab

Windows 系统底层「玩 / 测试 / 研究」工具集。本地 server + 浏览器 UI，设计上只读。

## 工具（Phase 1）

| 工具 | 功能 |
|---|---|
| **PE 解剖器** | 拖入 exe/dll/sys — DOS 头、节表、导入表、导出表、资源、数字签名与 PDB 信息 |
| **ObjDir** | 浏览 NT 对象管理器名字空间（`\Device`、`\BaseNamedObjects`、`\KernelObjects`…），树形懒加载 + 符号链接目标解析 |

## 运行

```
python -m venv .venv
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe main.py        # → http://127.0.0.1:8765
```

Windows 10/11 x64。多数名字空间无需管理员；部分目录会返回 ACCESS_DENIED——这是预期行为，也是乐趣的一部分。

## 架构

```
main.py → ntlab/api（FastAPI 薄层）→ ntlab/core（pe / ntobj，纯 Python）
                                   → ntlab/web（SPA，osu! 风格自定义元素）
```

- 后端：FastAPI + pefile + ctypes 直调 NT API，全程只读
- 前端：无打包器；控件来自 osu! 派生参考库（MIT，见 `ntlab/web/vendor/LICENSE-MIT.txt`）；`ui-tree` / `ui-table` 为自建
- 将来的 CLI 薄壳可直接复用 `ntlab/core`

## 路线图

- **Phase 1**（当前）：骨架 + PE 解剖器 + ObjDir
- **Phase 2**：WinAPI Playground — 白名单内交互式调用 API，实时渲染返回值 / GetLastError / NTSTATUS（白名单后端强制）
- **Phase 3**：实时观测 — ETW 仪表盘（进程/文件/注册表）、窗口消息监听、定时器精度实验

详见 `docs/architecture.md` 与 `docs/roadmap.md`。

## 许可

- NTLab 代码：GPL-3.0-or-later
- `ntlab/web/vendor/`：MIT，派生自 ppy/osu（不含 osu! 品牌），审计见 `licence-audit.json`
