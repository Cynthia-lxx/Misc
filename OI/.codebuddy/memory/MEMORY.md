# 工作区记忆索引 · p:/Dev/OI

本项目（OI Code Console / OI.py）专属记忆一律落在本目录 `.codebuddy/memory/`，**不要写入全局记忆 GM**。
换一个完全无关的项目仍然适用的通用习惯才写 GM；仓库结构、技术栈、版本约定、调研结论、进度与踩坑都写这里。

## 索引

- [2026-11-15-cloud-ide.md](2026-11-15-cloud-ide.md) — 云 IDE 改版（**当前主页形态**）：CodeMirror 5 vendor、IDE 三区布局、Overlay 层、`/api/run` stdout/stderr 分流、gdbserver + GDB/MI 断点调试架构、快捷键表与 10 条关键踩坑。
- [2026-11-15-analysis-tools.md](2026-11-15-analysis-tools.md) — 分析工具扩展（内存 / 复杂度 / 风格冗余 / 性能）：架构决策、ComfyDL_UI profiling 参考契约与行号、AI 预算口径、内置 MinGW64 工具链、验证结论与踩坑。
- [2026-11-14-web-console.md](2026-11-14-web-console.md) — 首版 Web 控制台：零依赖 `http.server` + 原生前端、osu UIReference MIT 风格层、SSE 生成链路与验证结论。

## 约定

- `OI.py` 是既有 CLI，保持行为不变；它同时是配置/历史/系统提示词的唯一规则源。
- 项目零新增 Python 依赖；后端只用标准库 + 已装的 `openai`/`click`/`rich`/`pyperclip`。
- 服务只监听 `127.0.0.1`；分析与测量纯本地，不做任何外发（参考仓库 AGENTS.md 的 No Internet Requests 精神）。
- 详录文件命名用该记忆自身标注的日期；先完整落盘并回读校验，再删除/修改对应条目。

## Git 提交规范（P:\Dev 父仓库，用户 2026-11-15 指定）

- `P:\Dev\` 是装有许多项目的 monorepo；OI 无独立 `.git`，属于父仓库的一部分。
- **提交节奏**：每累计约 500~10000+ 行更改就提交一次。
- **提交格式**：`[OI] [标签] 简述`，标签如 `feat` / `web` / `fix` / `env` / `config` / `UI`；多个主题用 `&` 连接（例：`[OI] [feat] DBG & IDE & MemProfiling & Analysis`）。
- **安全红线**：`git add` 必须显式指定只属于本项目的路径（如 `-- OI/`）；禁用 `git add -A` / `git add .`；用 `-m` 提交、**绝不使用 `-am`**（`-a` 会把别的目录的改动一并暂存）。暂存后先 `git status --short` 核对全部条目都限定在 `OI/` 内再 commit。
- 已跟踪文件的删除也用路径限定的 `git add -u -- OI/` 暂存；新增文件逐个显式 `git add -- 路径`。
