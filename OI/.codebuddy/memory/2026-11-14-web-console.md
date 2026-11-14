# 2026-11-14 · 首版 Web 控制台

## 做了什么

把 `OI.py`（基于 DeepSeek API 的 OI 代码生成 CLI）做成 Web 控制台，放在 `web/`，原 CLI 不改。

- `web/server.py`：Python 标准库 `ThreadingHTTPServer` + `BaseHTTPRequestHandler`，零新增依赖（环境：Python 3.12 / `OIenv` 虚拟环境，已装 openai、click、rich、pyperclip；**无** Flask/FastAPI/psutil）。
- 直接 `import OI as oi`，复用 `Config` / `History` / `SYSTEM_PROMPT` / `extract_code`，因此 Web 与 CLI 共用 `~/.oicoder_config.json`、`~/.oicoder_history.json`。
- 路由：`GET /api/status`、`GET/DELETE /api/history`、`POST /api/config`、`POST /api/generate`（SSE 流式，支持 body 覆盖 model/max_tokens/temperature）、`POST /api/save`、`POST /api/read-file`、`POST /api/clipboard`；静态服务 `web/static/` 用 `is_within()` 防穿越。
- 修正了 CLI 的一个缺陷：原 `call_deepseek` 只在 `is_new_topic=True` 时注入 system prompt，Web 版每轮都注入。
- 前端：`index.html` + `console.css` + `console.js`，`.oc-shell` 骨架（侧栏 / 顶栏 / 4 视图 / 底栏 / 事件日志），hash 路由；工作台含题面输入、剪贴板/路径/文件载入、Temperature、SSE 实时生成、手写 C++ 高亮 + 行号、复制/保存/下载。

## UI 来源

`P:/Dev/osu/UIReference/`（从 ppy/osu MIT 源码抽离的 UI Reference Kit，自带许可审计）。复制进 `web/static/vendor/`：`tokens.css`、`themes.css`、`motion.css`、`glow.css`、`status-pulse.css`、`components/ui-button/{css,js}`、`LICENSE-MIT.txt`。

- 约束（其 `00-overview/licence-audit.json`）：保留 MIT 声明与 SPDX 头、组件统一 `ui-` 前缀、不使用 osu!/ppy/lazer 品牌、不分发 Torus 字体二进制、不复制 GLSL/Crowdin 译文。
- 9 套 `[data-theme]`（blue 默认，hue 200；red 0 / orange 45 / lime 90 / green 125 / aquamarine 160 / purple 255 / plum 320 / pink 333）。
- `tokens.css` 的 `--ui-accent/--ui-surface*/--ui-text*/--ui-border` 是组件唯一入口；`--motion-scale` 支持 reduced-motion。
- 视觉语言来自 `10-templates/preview/preview.css` 的 `.pv-*`（暗色分层、5/10px 圆角、状态点、日志面板），本项目用 `.oc-*` 重写。

## 验证结论

- 静态资源与全部 API 200；`POST /api/generate` 用隔离 `USERPROFILE` + 本地 mock OpenAI SSE 服务跑通 start/delta/done、代码提取、历史写入，未触碰真实配置与历史。
- 浏览器（playwright-cli + `--browser=msedge`，本机无 Chrome）实测视图/主题/tab/日志/生成交互，控制台 0 错误。
- **踩坑**：隔离 HOME 的配置 JSON 若用 PowerShell `Set-Content -Encoding UTF8` 会带 BOM，`OI.py` 的 `json.load` 报 `Expecting value: line 1 column 1`；要用无 BOM UTF-8 写入。
- **踩坑**：PowerShell `Invoke-RestMethod` 首次连接偶发「无法连接到远程服务器」，`curl.exe --noproxy "*"` 稳定；服务启动约需 1~2 秒。
- **安全遗留**：`OI.py:31` 仍硬编码明文 DeepSeek API Key，Web 版只是读取同一份配置，未改动。
