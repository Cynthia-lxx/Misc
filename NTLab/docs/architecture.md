# NTLab 架构

## 分层

```
浏览器 (SPA, hash 路由)
   │  REST JSON (/api/*)
   ▼
ntlab/api  — FastAPI 薄层：参数校验 + JSON 序列化。无业务逻辑。
   │
   ├─ ntlab/core/pe     — pefile 封装：头/节/导入/导出/资源/签名 → dict
   ├─ ntlab/core/ntobj  — ctypes 直调 ntdll：目录枚举/符号链接目标，只读
   └─ ntlab/core/errors — NtlabError + NTSTATUS/Win32 错误 → 可读消息
   │
ntlab/web  — 静态托管：SPA + vendor 控件（osu! 派生, MIT）+ 自建 ui-tree/ui-table
```

`main.py` 创建 uvicorn（`127.0.0.1:8765`）并服务 `create_app()`。

## 关键决策

1. **core 不依赖 fastapi** — 将来的 CLI 薄壳（`ntlab pe foo.dll`）直接复用。
2. **PE 大文件分页** — `/api/pe/imports`、`/api/pe/exports` 支持 `offset/limit`；解析结果按 `path+mtime` 在进程内缓存，避免重复解析 ntoskrnl 这类大文件。
3. **ObjDir 懒加载** — 前端展开目录节点时才请求 `/api/ntobj/list`；句柄即用即关（`finally: NtClose`）。
4. **只读红线** — 后端不 import 任何写类 NT API；路由仅 GET/POST(parse by path)。
5. **主题** — `vendor/themes.css` 以 `[data-theme]` 属性切换（默认 blue），`tokens.css` 为基础变量层。

## 前端装配（vendor）

来源 `P:\Dev\osu\UIReference\dist`（assemble.py 产物）：

- `vendor/tokens.css`、`vendor/themes.css` — 设计令牌与主题层
- `vendor/components/<name>/<name>.{js,css}` — 22 个原生 Custom Element（ES module 导入即注册，Shadow DOM 自取 CSS）
- `vendor/i18n/{en,zh-CN}.json`、`settings.json`、`manifest.json` — 数据
- `vendor/LICENSE-MIT.txt`、`licence-audit.json` — 许可与审计（勿删）

自建组件（`ntlab/web/components/`）沿用 tokens 变量（`--ui-surface`、`--ui-accent` 等）保持一致视觉。

## 错误模型

`NtlabError(code, message)`：core 层唯一异常类型。NTSTATUS（如 `0xC0000022`）在 `core/ntobj/ntapi.py` 映射为 `{status, name, message}`；api 层转 `{"error": {...}}` + 对应 HTTP 状态码。
