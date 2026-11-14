# 工作区记忆索引 · p:/Dev/OI

本项目（OI Code Console / OI.py）专属记忆一律落在本目录 `.codebuddy/memory/`，**不要写入全局记忆 GM**。
换一个完全无关的项目仍然适用的通用习惯才写 GM；仓库结构、技术栈、版本约定、调研结论、进度与踩坑都写这里。

## 索引

- [2026-11-15-analysis-tools.md](2026-11-15-analysis-tools.md) — 分析工具扩展（内存 / 复杂度 / 风格冗余 / 性能）：架构决策、ComfyDL_UI profiling 参考契约与行号、AI 预算口径、内置 MinGW64 工具链、验证结论与踩坑。
- [2026-11-14-web-console.md](2026-11-14-web-console.md) — 首版 Web 控制台：零依赖 `http.server` + 原生前端、osu UIReference MIT 风格层、SSE 生成链路与验证结论。

## 约定

- `OI.py` 是既有 CLI，保持行为不变；它同时是配置/历史/系统提示词的唯一规则源。
- 项目零新增 Python 依赖；后端只用标准库 + 已装的 `openai`/`click`/`rich`/`pyperclip`。
- 服务只监听 `127.0.0.1`；分析与测量纯本地，不做任何外发（参考仓库 AGENTS.md 的 No Internet Requests 精神）。
- 详录文件命名用该记忆自身标注的日期；先完整落盘并回读校验，再删除/修改对应条目。
