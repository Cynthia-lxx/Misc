# NTLab 路线图

## Phase 1 — 骨架 + 只读探索器（当前）

- [x] 项目骨架 / venv / AGENTS / docs
- [ ] PE 分析器（DOS 头、节表、导入、导出、资源、签名、PDB）
- [ ] ObjDir（NT 命名空间树、懒加载、符号链接目标）

## Phase 2 — WinAPI Playground

交互式调用白名单内的 Win32/NT API，实时渲染返回值、GetLastError、NTSTATUS 与结构化输出。

- 白名单**在后端强制执行**（每张「卡牌」= 一个后端声明的 API 描述：参数 schema、副作用级别、清理动作）
- 侧向分级：`safe`（MessageBox/Beep/临时文件）→ `observable`（定时器精度、NtQuery* 家族）→ 默认不含任何注入/远程线程/写注册表 HKLM 类
- 素材：`legacy/DLLDemo*.py` 已有大量 ctypes 原型可移植
- 卡牌化 UI：每 API 一卡，可组合执行序列

## Phase 3 — 实时观测

- ETW 消费者（python-etw/pywintrace 或届时 C 助手 DLL）：进程启动、文件 I/O、注册表事件实时流（WebSocket）
- 窗口消息监听器（Spy++ 开源重制，公开 API）
- 内存页实验台（VirtualAlloc/VirtualProtect/Guard Page 可视化）
- 定时器精度实验（NtSetTimerResolution + 漂移曲线）

## 远期想法（无承诺）

- 句柄/内核对象浏览器（SystemHandleInformation）
- syscall 号查看器（解析 ntdll stub）
- token/特权可视化对比
- MSVC C 助手 DLL：性能热点外包车间（工具集 14.51 已就位）
