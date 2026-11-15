# NTLab Agent 工作约定

Windows 系统底层「玩/测试/研究」工具集。本地 server + 浏览器 UI 形态，Python 3.12 + FastAPI 后端，原生 Custom Elements 前端。

## 目录约定

- `main.py` — 唯一启动入口（uvicorn，仅 bind 127.0.0.1）
- `ntlab/api/` — FastAPI 路由薄层：只做参数校验与 JSON 序列化，业务逻辑禁止写在这里
- `ntlab/core/` — 业务核心，纯 Python，不 import fastapi；每个子模块一个目录（`pe/`、`ntobj/`），供将来的 CLI 薄壳复用
- `ntlab/web/` — 前端静态资源（无打包器）；`vendor/` 为从 `P:\Dev\osu\UIReference\dist` 装配的第三方控件（勿手改，许可见 vendor/LICENSE-MIT.txt）
- `ntlab/web/components/` — 自建组件（ui-tree / ui-table），视觉规则必须沿用 vendor 控件的 tokens
- `ntlab/web/pages/` — SPA 页面模块（home / pe-viewer / objdir）
- `legacy/` — 归档的实验脚本（DLLDemo*.py），仅作素材，不参与运行
- `tests/` — pytest；PE 测试用系统自带 exe/dll 作夹具，不提交二进制

## 如何加一个新 API

1. 在 `ntlab/core/<模块>/` 实现纯函数（输入普通参数，输出 dict，异常抛 `NtlabError`）
2. 在 `ntlab/api/` 新建或扩展路由文件，用 `APIRouter`，只做薄封装
3. 在 `ntlab/api/__init__.py` 的 `create_app()` 注册 router
4. `tests/` 补对应测试

## 硬性红线

- server 只 bind `127.0.0.1`；API 一律只读，禁止任何写/创建/删除类 NT API 与注册表写操作
- Playground（Phase 2）的白名单必须在后端强制执行，不得只靠前端
- 前端 vendor/ 目录内容不改名、不内联修改；新增样式一律走自建组件或 `app.css`
- 提交遵循父仓库（P:\Dev）GM 规范：显式路径 `git add -- NTLab/...`、禁 `-A`/`add .`/`-am`、commit 后必须 push

## 运行与调试

```
.venv\Scripts\python.exe main.py          # 默认 127.0.0.1:8765
.venv\Scripts\python.exe -m pytest        # 测试
```

起服务前先 `netstat -ano | findstr :8765` 检查端口占用；重启须杀光旧监听并复查监听数=1。

## 环境备注（本机）

- venv 解释器：`.venv\Scripts\python.exe`（基座 Python 3.12.9 @ C:\Python312）
- MSVC 后备（本期不用）：VS Community 2026 v18.10.3，工具集 14.51.36231，SDK 10.0.26100.0
- PowerShell 5.1 读含中文文件必须 `-Encoding UTF8`
