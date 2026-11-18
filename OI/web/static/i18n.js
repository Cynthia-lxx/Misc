/* i18n — 轻量双语（zh 默认原文 / en），与 NTLab 同一套方法。
 *
 * 约定：
 * - key = 中文原文（源码里出现的字符串），en 词典只维护翻译；
 *   zh 自动补恒等映射，中文界面零维护成本。
 * - globalThis.UIReference.i18n = 当前词典（vendor 组件按 data-i18n-key 自解析）
 * - document 派发 "ui-i18n-change"（组件监听并重解析标签）
 * - 应用侧用 t(key, params) 取词、applyDom() 刷新 [data-i18n-key] /
 *   [data-i18n-ph]（placeholder）/ [data-i18n-title]（title）
 * - 语言持久化 localStorage("oi-lang")，**默认英文**（zh 需显式选择），
 *   切换即时生效、刷新保持
 */

const DICTS = {
  en: {
    /* ---- 导航 / 侧栏 ---- */
    '分析报告': 'Analysis',
    '会话历史': 'History',
    '设置': 'Settings',
    '命令速查': 'Cheat Sheet',
    '正在检查服务…': 'Checking service…',
    '已载入上次生成的代码': 'Last generated code loaded',
    '清空历史：等待二次确认': 'Clear history: waiting for confirmation',
    '（暂无输出）': '(no output yet)',
    '（新增）': '(new)',
    '文件名（保存/读取基于工作区解析）': 'Filename (resolved against the workspace for save/open)',
    '变量': 'Variable',
    'API Key 已配置': 'API Key configured',
    'API Key 未设置': 'API Key not set',
    'history: {n} 条': 'history: {n} entries',
    /* ---- 视图描述（标题与 nav 重名复用） ---- */
    '以代码为中心：编辑、运行、调试与关键指标同屏': 'Code-centric: edit, run, debug and key metrics on one screen',
    '内存 / 复杂度 / 风格冗余 / 性能四类体检，纯本地分析 + 一次短 AI 汇总': 'Memory / complexity / style / performance checks, fully local + one short AI summary',
    '服务端持久化的完整对话，与 CLI 的 /show 同源': 'Full conversation persisted server-side, same source as CLI /show',
    'API 与生成参数写入 ~/.oicoder_config.json，CLI 同步生效': 'API & generation params written to ~/.oicoder_config.json, shared with the CLI',
    '原 OI.py 命令在控制台中的落点，以及快捷键': 'Where each OI.py command lives in the console, plus shortcuts',
    /* ---- 顶栏 ---- */
    'Theme': 'Theme',
    '动效: 开': 'Motion: On',
    '动效: 关': 'Motion: Off',
    '日志 {n}': 'Log {n}',
    /* ---- IDE 工具栏 / 侧面板 ---- */
    '打开 ⌂O': 'Open ⌂O',
    '保存 ⌃S': 'Save ⌃S',
    '复制': 'Copy',
    '运行 F5': 'Run F5',
    '调试 F9': 'Debug F9',
    '停止 Esc': 'Stop Esc',
    'AI 生成 ⌘⏎': 'AI Generate ⌘⏎',
    '分析 A': 'Analyze A',
    '命令 ⌘K': 'Commands ⌘K',
    '空闲': 'Idle',
    '指标': 'Metrics',
    '未分析': 'Not analyzed',
    '点击「分析 A」体检当前代码': 'Click "Analyze (A)" to check the current code',
    '完整报告 →': 'Full report →',
    'AI 汇总: 开': 'AI summary: On',
    'AI 汇总: 关': 'AI summary: Off',
    'AI: 开': 'AI: On',
    'AI: 关': 'AI: Off',
    '可关闭 ⓘ': 'Optional ⓘ',
    'AI 汇总随分析自动生成：单轮、不携带历史、只传结构化摘要，0.1 当量/次，默认上限 2 当量。':
      'AI summary is generated per analysis: single-turn, no history, structured summary only. 0.1 credits each, default cap 2.',
    /* ---- dock ---- */
    '调试': 'Debug',
    '（尚未运行）': '(not run yet)',
    '程序标准输入；运行与调试都会使用这里的内容…': 'Program stdin; used by both run and debug…',
    '继续 F5': 'Continue F5',
    '步过 F10': 'Step Over F10',
    '步入 F11': 'Step Into F11',
    '步出': 'Step Out',
    '结束调试': 'Stop Debugging',
    '未调试': 'Not debugging',
    '变量': 'Variables',
    '监视': 'Watch',
    '表达式，如 ans_max 或 (1<<n)': 'Expression, e.g. ans_max or (1<<n)',
    '添加': 'Add',
    '在编辑器行号左侧点击 ● 可切换断点（Ctrl+B 于当前行）。stderr 与 stdout 在上方标签实时分流。':
      'Click ● left of the line numbers to toggle breakpoints (Ctrl+B for the current line). stdout/stderr are split live in the tabs above.',
    /* ---- 运行/状态 ---- */
    '编辑器没有代码，先写点或让 AI 生成': 'Editor is empty — write something or let AI generate',
    '运行中…': 'Running…',
    '（编译中…）': '(compiling…)',
    '（无输出）': '(no output)',
    '（已手动停止）': '(stopped manually)',
    '通过 {ms} ms': 'Passed {ms} ms',
    '超时': 'Timeout',
    '编译失败': 'Compile failed',
    '编译失败，stderr 标签有详情': 'Compile failed — see the stderr tab for details',
    '退出码 {code}': 'Exit code {code}',
    '已停止': 'Stopped',
    '运行失败: {msg}': 'Run failed: {msg}',
    '运行超时（{reason}）': 'Run timed out ({reason})',
    '运行通过：exit 0，{ms} ms，峰值 {mem}': 'Run passed: exit 0, {ms} ms, peak {mem}',
    '缓存编译': 'cached compile',
    '编译 {ms} ms': 'compile {ms} ms',
    '运行结束：exit {code}': 'Run finished: exit {code}',
    '运行已手动停止': 'Run stopped manually',
    '（AI 正在把题解写入编辑器…）': '(AI is writing the solution into the editor…)',
    'AI 生成中…': 'AI generating…',
    'AI 生成完成': 'AI generation done',
    'AI 生成完成：{n} 行': 'AI generation done: {n} lines',
    '生成已停止': 'Generation stopped',
    'AI 生成已停止': 'AI generation stopped',
    '生成失败': 'Generation failed',
    'AI 生成失败: {msg}': 'AI generation failed: {msg}',
    '分析中…': 'Analyzing…',
    '分析（缓存）': 'Analysis (cached)',
    '分析完成': 'Analysis done',
    '分析失败': 'Analysis failed',
    'IDE 分析完成：{verdict}，{n} 项发现': 'IDE analysis done: {verdict}, {n} findings',
    '生成流意外中断': 'Generation stream ended unexpectedly',
    '已保存 {path}': 'Saved {path}',
    '已保存 → {path}': 'Saved → {path}',
    '保存失败: {msg}': 'Save failed: {msg}',
    '代码已复制到剪贴板': 'Code copied to clipboard',
    '代码已复制': 'Code copied',
    '剪贴板不可用': 'Clipboard unavailable',
    '本地体检未发现问题': 'Local checks found no issues',
    '详见「完整报告」视图；unknown 表示静态无法确定，不做猜测':
      'See the "Full report" view; unknown means statically undetermined — no guessing',
    '{verdict} · {n} 项发现': '{verdict} · {n} findings',
    '（无建议）': '(no suggestions)',
    '额度 {used}/{cap} 当量 · 单次 0.1 · {status}': 'Budget {used}/{cap} credits · 0.1 each · {status}',
    '限制': 'Limits',
    ' ✗超时': ' ✗TLE',
    ' ✗超限': ' ✗MLE',
    '时 {a}/{b}ms{mark}': 'Time {a}/{b}ms{mark}',
    '存 {a}/{b}MiB{mark}': 'Mem {a}/{b}MiB{mark}',
    '额度 {used}/{cap} 当量': 'Budget {used}/{cap} credits',
    '可选用 AI：关闭后分析仍返回完整本地报告，仅省去自动汇总': 'AI optional: analysis still returns the full local report when off, only the auto summary is skipped',
    '如 P1007.cpp（相对工作区）': 'e.g. P1007.cpp (relative to workspace)',
    '单次汇总消耗 0.1 次代码生成当量，上限 2 当量。': 'Each summary costs 0.1 code-generation credits, capped at 2 credits total.',
    /* ---- 判定词/维度 ---- */
    '通过': 'Pass',
    '风险': 'Risk',
    '问题': 'Issue',
    '未知': 'Unknown',
    '内存': 'Memory',
    '复杂度': 'Complexity',
    '风格冗余': 'Style',
    '性能': 'Performance',
    /* ---- overlay ---- */
    '打开文件': 'Open file',
    '工作区 .cpp × {n}（Esc 关闭）': 'Workspace .cpp × {n} (Esc to close)',
    '筛选文件名…': 'Filter filenames…',
    '载入 → 编辑器': 'Load → editor',
    '没有匹配的文件': 'No matching files',
    '读取工作区文件列表失败: {msg}': 'Failed to list workspace files: {msg}',
    '已载入 {path}（{n} 行）': 'Loaded {path} ({n} lines)',
    '读取 {path} 失败: {msg}': 'Failed to read {path}: {msg}',
    '保存代码': 'Save code',
    '相对路径基于工作区；自动补 .cpp': 'Relative paths resolve against the workspace; .cpp appended automatically',
    '保存': 'Save',
    'AI 生成题解': 'AI generate solution',
    '必用 AI · 题面 → 流式写入编辑器（1 次生成 ≈ 1 次代码生成用量，不带历史）':
      'AI required · statement → streamed into the editor (1 generation ≈ 1 code-generation credit, no history)',
    '粘贴题目描述（可含样例输入输出）…': 'Paste the problem statement (samples allowed)…',
    '读剪贴板': 'Paste from clipboard',
    '无法读取剪贴板（浏览器权限）': 'Cannot read clipboard (browser permission)',
    '示例题面': 'Sample statement',
    '清空': 'Clear',
    '生成 ⌃⏎': 'Generate ⌃⏎',
    '停止': 'Stop',
    '题面是空的': 'Problem statement is empty',
    '生成中…': 'Generating…',
    '跳转到行': 'Go to line',
    '行号': 'Line number',
    '命令面板': 'Command palette',
    'Ctrl+K · ↑↓ 选择 · Enter 执行': 'Ctrl+K · ↑↓ to select · Enter to run',
    '输入命令…': 'Type a command…',
    '运行代码': 'Run code',
    '启动调试': 'Start debugging',
    '打开文件…': 'Open file…',
    'AI 生成题解…': 'AI generate…',
    '分析当前代码': 'Analyze current code',
    '查看完整报告': 'View full report',
    '复制代码': 'Copy code',
    '跳转到行…': 'Go to line…',
    '打开会话历史': 'Open history',
    '打开设置': 'Open settings',
    '切换主题…': 'Cycle theme…',
    '清空编辑器': 'Clear editor',
    'overlay 层就绪': 'Overlay layer ready',
    'IDE 控制器就绪（CodeMirror 5）': 'IDE controller ready (CodeMirror 5)',
    '调试台就绪（gdbserver + GDB/MI）': 'Debug dock ready (gdbserver + GDB/MI)',
    'OI Code Console 启动（IDE 布局）': 'OI Code Console started (IDE layout)',
    '服务状态获取失败: {msg}': 'Failed to fetch service status: {msg}',
    '打开文件列表…': 'Listing workspace files…',
    /* ---- 分析视图 ---- */
    '来源': 'Source',
    '当前输出': 'Current output',
    '工作区题解': 'Workspace file',
    '指定路径': 'Custom path',
    '分析': 'Analyze',
    '重新分析': 'Re-analyze',
    '清缓存': 'Clear cache',
    '时限 ms': 'Time ms',
    '空间 MB': 'Memory MB',
    '限制判定 ⓘ': 'Limits ⓘ',
    '填写题目时空限制后，实测会判定 TLE/MLE 并计入总判定': 'Fill in the limits and the measured run will verdict TLE/MLE into the overall result',
    '实测': 'Measured run',
    '样例输入 stdin（可选，实测时喂给程序）…': 'Sample stdin (optional, fed to the program when measuring)…',
    '尚未分析': 'Not analyzed yet',
    '选择来源后点击「分析」；无法静态确定的内容会如实标注 unknown。':
      'Pick a source and hit "Analyze"; anything statically undetermined is honestly marked unknown.',
    '峰值内存': 'Peak memory',
    '风格问题': 'Style issues',
    '性能提示': 'Performance hints',
    '编译与运行由工作区内置 MinGW64（GCC 4.9.2）完成，限时并采样峰值内存。':
      'Compile & run use the vendored MinGW64 (GCC 4.9.2) in the workspace, time-limited with peak-memory sampling.',
    '编译 / 运行日志': 'Build / run log',
    '内置 GCC 可用': 'Vendored GCC available',
    '工具链缺失': 'Toolchain missing',
    '题解列表载入失败: {msg}': 'Failed to load solution list: {msg}',
    '预算状态载入失败: {msg}': 'Failed to load budget state: {msg}',
    '每次分析自动一次短汇总：单轮、不携带任何对话历史、只传结构化摘要。':
      'One short summary per analysis: single-turn, no conversation history, structured summary only.',
    '待分析': 'Pending',
    '汇总中…': 'Summarizing…',
    '缓存汇总': 'Cached summary',
    'AI 汇总': 'AI summary',
    'AI 汇总不可用（{reason}）': 'AI summary unavailable ({reason})',
    '已关闭': 'Disabled',
    '已跳过': 'Skipped',
    '额度耗尽': 'Budget exhausted',
    '失败': 'Failed',
    'AI 汇总已关闭（仅本地报告）': 'AI summary disabled (local report only)',
    '没有 API Key，已跳过 AI 汇总': 'No API Key, AI summary skipped',
    'AI 额度已耗尽': 'AI budget exhausted',
    '代码证据': 'Code evidence',
    '命中行有左侧色条，点击指标里的行号可定位。': 'Hit lines have a colored bar; click line numbers in metrics to jump.',
    '（无代码）': '(no code)',
    '{n} 行命中': '{n} lines hit',
    '全局/栈/堆估算与预算阈值': 'Global/stack/heap estimation with budget thresholds',
    '循环嵌套与递归（启发式估算）': 'Loop nesting & recursion (heuristic estimate)',
    '规则源：OI.py SYSTEM_PROMPT 十一条': 'Rules from OI.py SYSTEM_PROMPT (11 rules)',
    '常见本地反模式与热点': 'Common local anti-patterns & hotspots',
    '无发现': 'No findings',
    '指标': 'Metrics',
    '判定': 'Verdict',
    '值': 'Value',
    '依据': 'Evidence',
    '编辑器还没有代码，先写代码或让 AI 生成': 'Editor has no code yet — write some or let AI generate',
    '工作区没有可选的 .cpp 题解': 'No .cpp solutions to pick in the workspace',
    '请填写文件路径': 'Please fill in the file path',
    '缓存已清空 ({n} 份)': 'Cache cleared ({n} reports)',
    '清缓存失败: {msg}': 'Cache clear failed: {msg}',
    '缓存命中': 'cache hit',
    '新分析': 'new analysis',
    '分析失败: {msg}': 'Analysis failed: {msg}',
    '超出题目限制': 'Exceeds limits',
    '实测完成': 'Measured',
    '编译耗时': 'Compile time',
    '运行耗时': 'Run time',
    '未采样到': 'not sampled',
    '退出码': 'Exit code',
    '限制判定': 'Limit verdict',
    '时限判定': 'Time verdict',
    '（超时）': ' (TLE)',
    '（通过）': ' (pass)',
    '空间判定': 'Memory verdict',
    '（超限）': ' (MLE)',
    '编译器': 'Compiler',
    '运行超时': 'Run timeout',
    '说明': 'Note',
    '实测失败': 'Measure failed',
    '阶段': 'Phase',
    '原因': 'Reason',
    '未启用实测': 'Measure off',
    '勾选「实测」后重新分析': 'Check "Measured run" and re-analyze',
    '注意': 'Caution',
    '本次消耗 {a} 当量 · 已用 {b}/{c}': 'This run cost {a} credits · used {b}/{c}',
    '同一份代码与报告已汇总过，未消耗额度': 'Same code & report already summarized — no credits spent',
    'AI 汇总已关闭，本地报告不受影响': 'AI summary disabled; local report unaffected',
    '没有 API Key，AI 汇总已跳过': 'No API Key, AI summary skipped',
    '额度已耗尽；可在设置中调高或清空后重试': 'Budget exhausted; raise or reset it in settings and retry',
    'AI 调用失败，本地报告不受影响': 'AI call failed; local report unaffected',
    '等待汇总…': 'Waiting for summary…',
    '单次汇总消耗 {a} 当量，上限 {b} 当量': 'Each summary costs {a} credits, cap {b}',
    'AI 汇总 {state}': 'AI summary {state}',
    '切换 AI 失败: {msg}': 'Failed to toggle AI: {msg}',
    'AI 自动汇总已开启（本地报告不受影响）': 'Auto AI summary enabled (local report unaffected)',
    'AI 自动汇总已关闭（本地报告不受影响）': 'Auto AI summary disabled (local report unaffected)',
    '已手动关闭': 'Manually disabled',
    '跳到第 {n} 行': 'Go to line {n}',
    /* ---- 历史 ---- */
    '刷新': 'Refresh',
    '清空历史': 'Clear history',
    '再点一次确认': 'Click again to confirm',
    '暂无会话历史。': 'No conversation history yet.',
    '载入到编辑器': 'Load into editor',
    '载入历史消息 #{n} → 编辑器 ({m} 行)': 'Loaded history #{n} → editor ({m} lines)',
    '会话历史已清空': 'History cleared',
    '配置已重新载入': 'Config reloaded',
    '设置已保存': 'Settings saved',
    '保存设置失败: {msg}': 'Save settings failed: {msg}',
    '配置已写入 ~/.oicoder_config.json': 'Config written to ~/.oicoder_config.json',
    '服务端持久化，CLI 的 /show 与此处看到的是同一份记录。': 'Persisted server-side; CLI /show reads the same records.',
    /* ---- 设置卡片 ---- */
    'API 与生成参数': 'API & generation',
    '写入 ~/.oicoder_config.json，与 CLI 的 /setkey、/model、/config 互通。':
      'Written to ~/.oicoder_config.json, shared with CLI /setkey, /model, /config.',
    'API Key': 'API Key',
    '留空表示保持当前值': 'Leave blank to keep the current value',
    'Base URL': 'Base URL',
    '模型': 'Model',
    '保存设置': 'Save settings',
    '重新载入': 'Reload',
    '当前配置': 'Current config',
    '敏感值已脱敏。': 'Sensitive values masked.',
    '外观': 'Appearance',
    '9 套 HSL 主题，切换会写入本地存储。': '9 HSL themes; switches persist to local storage.',
    '恢复默认 (Blue)': 'Reset default (Blue)',
    '提示词模板': 'Prompt templates',
    '模板名称（另存时使用）': 'Template name (used when saving as new)',
    '系统提示词内容…': 'System prompt content…',
    '渲染预览': 'Rendered preview',
    '实时 · 未保存内容也参与': 'live · includes unsaved edits',
    '激活此模板': 'Activate',
    '保存修改': 'Save changes',
    '另存为新模板': 'Save as new',
    '删除': 'Delete',
    '此模板没有变量': 'This template has no variables',
    '留空 = 内联默认值': 'blank = inline default',
    '缺失变量：{list}（生成时将原样保留）': 'Missing variables: {list} (kept verbatim at generation time)',
    '全部变量已解析': 'All variables resolved',
    '内置模板不可编辑；可「另存为新模板」后修改副本': 'Built-in template is read-only; use "Save as new" to edit a copy',
    '自定义模板': 'Custom template',
    '自定义模板 · 当前激活': 'Custom template · active',
    '变量保存失败: {msg}': 'Failed to save variables: {msg}',
    '已激活模板「{name}」': 'Activated template "{name}"',
    '提示词模板切换 → {name}（新生成即生效）': 'Prompt template switched → {name} (applies to new generations)',
    '模板已保存': 'Template saved',
    '保存冲突：模板已被其他窗口修改。已为你重新载入最新内容，请合并后再保存。':
      'Save conflict: the template was modified elsewhere. Reloaded the latest content — merge and save again.',
    '模板内容为空': 'Template content is empty',
    '已另存并激活「{name}」': 'Saved & activated "{name}"',
    '新提示词模板「{name}」已保存并激活': 'New prompt template "{name}" saved & activated',
    '模板「{name}」已删除': 'Template "{name}" deleted',
    '渲染结果已复制到剪贴板': 'Rendered text copied to clipboard',
    '提示词模板载入失败: {msg}': 'Failed to load prompt templates: {msg}',
    /* ---- 服务器卡片 ---- */
    '服务器': 'Server',
    '端口': 'Port',
    '自动打开浏览器': 'Auto-open browser',
    '端口与浏览器设置写入独立配置文件；端口在下次启动服务时生效。':
      'Server settings persist to a separate file; the port takes effect on next launch.',
    '服务器设置已保存': 'Server settings saved',
    '保存服务器设置失败: {msg}': 'Save server settings failed: {msg}',
    '服务器设置已重新载入': 'Server settings reloaded',
    /* ---- 速查页 ---- */
    'CLI → Web 对照': 'CLI → Web mapping',
    '原 OI.py 的每条命令在控制台里的落点。': 'Where every OI.py command lands in the console.',
    'CLI 命令': 'CLI command',
    'Web 操作': 'Web action',
    '说明': 'Notes',
    '本页': 'This page',
    '命令与快捷键说明': 'Commands & shortcuts',
    '设置 → API Key': 'Settings → API Key',
    '留空保持原值': 'Blank keeps the current value',
    '设置 → 当前配置': 'Settings → Current config',
    'API Key 脱敏展示': 'API Key shown masked',
    '设置 → 模型': 'Settings → Model',
    '历史 → 清空历史': 'History → Clear history',
    '同时清空服务端上下文': 'Also clears the server-side context',
    '完整消息，可一键载入编辑器': 'Full messages, one click to load into the editor',
    'IDE → 复制': 'IDE → Copy',
    '浏览器剪贴板 + 系统剪贴板兜底': 'Browser clipboard with system-clipboard fallback',
    'AI 生成弹层 → 读剪贴板': 'AI overlay → Paste from clipboard',
    '读取系统剪贴板填入题面': 'Reads the system clipboard into the statement',
    'IDE → 打开（弹层）': 'IDE → Open (overlay)',
    '相对路径基于工作区': 'Relative paths resolve against the workspace',
    'IDE → 保存 ⌃S': 'IDE → Save ⌃S',
    '自动补 .cpp 后缀与创建目录': 'Appends .cpp and creates directories',
    'IDE → 运行 F5 / 调试 F9': 'IDE → Run F5 / Debug F9',
    'stdout/stderr 分流；gdb 断点、步进、变量与监视（内置 MinGW64）': 'stdout/stderr split; gdb breakpoints, stepping, variables & watch (vendored MinGW64)',
    'IDE → 分析 A': 'IDE → Analyze A',
    '内存 / 复杂度 / 风格冗余 / 性能体检 + 一次限量短 AI 汇总': 'Memory / complexity / style / performance checks + one budgeted short AI summary',
    '快捷键': 'Shortcuts',
    '运行（stdout / stderr 分流）': 'Run (stdout / stderr split)',
    '启动调试（编译 -g -O0，跑向首个断点）': 'Start debugging (compile -g -O0, run to first breakpoint)',
    '调试步过 / 步入': 'Step over / into',
    'AI 生成（弹层，流式写入编辑器）': 'AI generation (overlay, streamed into the editor)',
    '保存当前编辑器内容': 'Save current editor content',
    '在当前行切换断点': 'Toggle breakpoint on current line',
    '跳转到指定行': 'Jump to a line',
    '打开 / 关闭事件日志': 'Open / close event log',
    '切换次要视图（输入框外）': 'Switch secondary views (outside inputs)',
    '关闭弹层 / 日志 / 结束运行': 'Close overlay / log / stop run',
    '关于': 'About',
    '纯本地服务：Python 标准库 http.server + openai SDK。服务仅监听 127.0.0.1，配置与历史存于用户主目录。前端唯一第三方依赖是离线内置于 static/vendor/codemirror/ 的 CodeMirror 5。':
      'Fully local service: Python stdlib http.server + openai SDK. Listens on 127.0.0.1 only; config & history live in the user home. The only third-party frontend dep is CodeMirror 5, vendored offline at static/vendor/codemirror/.',
    '运行与调试由内置 MinGW64（GCC 4.9.2 + gdb/gdbserver）完成：编译产物按内容哈希缓存，stdout/stderr 分流实时显示；调试为 gdbserver 托管 + GDB/MI 驱动，支持断点、步进、变量与监视。所有指标不外发。':
      'Run & debug use the vendored MinGW64 (GCC 4.9.2 + gdb/gdbserver): build artifacts cached by content hash, stdout/stderr split live; debugging is gdbserver-hosted + GDB/MI driven with breakpoints, stepping, variables & watch. No metric ever leaves the machine.',
    '界面样式参考 UI Reference Kit（MIT 派生，见 vendor/LICENSE-MIT.txt）：配色/排版/动效 token、9 套主题与 ui-button 组件。分析视图信息架构参考 ComfyDL_UI profiling（三色判定、unknown 诚实标注、手动触发 + 缓存、master switch）。':
      'UI styled after the UI Reference Kit (MIT-derived, see vendor/LICENSE-MIT.txt): color/type/motion tokens, 9 themes and the ui-button component. The analysis view IA references ComfyDL_UI profiling (three-color verdicts, honest unknowns, manual trigger + cache, master switch).',
    'AI 生成代码使用的风格系统提示词。支持 {{变量 | 默认值}} 插值：留空的变量使用内联默认值，缺失变量在预览中高亮可见。':
      'Style system prompt used by AI generation. Supports {{var | default}} interpolation: blank variables use inline defaults, missing ones are highlighted in the preview.',
    /* ---- 调试状态 ---- */
    '运行中': 'Running',
    '已退出': 'Exited',
    '错误': 'Error',
    '已结束': 'Finished',
    '启动中': 'Starting',
    '已有调试会话在运行，先结束（Esc 或「结束调试」）': 'A debug session is already running — stop it first (Esc or "Stop Debugging")',
    '编辑器没有代码': 'Editor is empty',
    '编译+附加中…': 'Compiling + attaching…',
    '调试会话 {sid} 启动：{reason}': 'Debug session {sid} started: {reason}',
    '启动失败': 'Start failed',
    '调试启动失败: {msg}': 'Debug start failed: {msg}',
    '（无局部变量）': '(no local variables)',
    '运行到断点处后显示': 'shown after hitting a breakpoint',
    '调试结束': 'debug finished',
    '行 {line} · {status}': 'Line {line} · {status}',
    '调试会话已结束': 'Debug session ended',
    '结束调试时出错: {msg}': 'Error stopping debug: {msg}',
  },
};

/* zh 恒等映射：key 即中文原文，中文界面零维护 */
for (const k of Object.keys(DICTS.en)) {
  if (!(k in (DICTS.zh ||= {}))) DICTS.zh[k] = k;
}

/* zh 富文本覆盖：HTML 段落的 key 是剥掉标记的纯文本（textContent 写入会丢 <code>），
   这里把 zh 指回带标记的原文，英文用纯文本即可 */
DICTS.zh['纯本地服务：Python 标准库 http.server + openai SDK。服务仅监听 127.0.0.1，配置与历史存于用户主目录。前端唯一第三方依赖是离线内置于 static/vendor/codemirror/ 的 CodeMirror 5。'] =
  '纯本地服务：Python 标准库 <code>http.server</code> + <code>openai</code> SDK。服务仅监听 <code>127.0.0.1</code>，配置与历史存于用户主目录。前端唯一第三方依赖是离线内置于 <code>static/vendor/codemirror/</code> 的 CodeMirror 5。';
DICTS.zh['运行与调试由内置 MinGW64（GCC 4.9.2 + gdb/gdbserver）完成：编译产物按内容哈希缓存，stdout/stderr 分流实时显示；调试为 gdbserver 托管 + GDB/MI 驱动，支持断点、步进、变量与监视。所有指标不外发。'] =
  '运行与调试由内置 MinGW64（GCC 4.9.2 + gdb/gdbserver）完成：编译产物按内容哈希缓存，stdout/stderr 分流实时显示；调试为 gdbserver 托管 + GDB/MI 驱动，支持断点、步进、变量与监视。所有指标不外发。';
DICTS.zh['界面样式参考 UI Reference Kit（MIT 派生，见 vendor/LICENSE-MIT.txt）：配色/排版/动效 token、9 套主题与 ui-button 组件。分析视图信息架构参考 ComfyDL_UI profiling（三色判定、unknown 诚实标注、手动触发 + 缓存、master switch）。'] =
  '界面样式参考 UI Reference Kit（MIT 派生，见 <code>vendor/LICENSE-MIT.txt</code>）：配色/排版/动效 token、9 套主题与 <code>ui-button</code> 组件。分析视图信息架构参考 ComfyDL_UI profiling（三色判定、unknown 诚实标注、手动触发 + 缓存、master switch）。';
DICTS.zh['AI 生成代码使用的风格系统提示词。支持 {{变量 | 默认值}} 插值：留空的变量使用内联默认值，缺失变量在预览中高亮可见。'] =
  'AI 生成代码使用的风格系统提示词。支持 <code>{{变量 | 默认值}}</code> 插值：留空的变量使用内联默认值，缺失变量在预览中高亮可见。';

let lang = localStorage.getItem('oi-lang') === 'zh' ? 'zh' : 'en';

globalThis.UIReference = { ...(globalThis.UIReference ?? {}), i18n: DICTS[lang] };

export function t(key, params) {
  let s = DICTS[lang][key] ?? DICTS.zh[key] ?? key;
  if (params) {
    for (const [k, v] of Object.entries(params)) s = s.replaceAll(`{${k}}`, String(v));
  }
  return s;
}

export function getLang() {
  return lang;
}

/** 切换语言：更新 UIReference 全局词典 → 刷 i18n 标记 DOM → 派发组件级刷新事件。 */
export function setLang(next) {
  if (!DICTS[next] || next === lang) return lang;
  lang = next;
  localStorage.setItem('oi-lang', lang);
  document.documentElement.lang = lang === 'zh' ? 'zh-CN' : 'en';
  globalThis.UIReference = { ...(globalThis.UIReference ?? {}), i18n: DICTS[lang] };
  applyDom();
  document.dispatchEvent(new CustomEvent('ui-i18n-change'));
  return lang;
}

/** 刷新 i18n 标记的 DOM：文本 / placeholder（data-i18n-ph）/ title（data-i18n-title）。 */
export function applyDom(root = document) {
  root.querySelectorAll('[data-i18n-key]').forEach((el) => {
    if (el.shadowRoot) return;   // ui-button 等组件按 ui-i18n-change 自刷新
    el.innerHTML = t(el.dataset.i18nKey);   // 词典受控，允许 <code> 等富文本
  });
  root.querySelectorAll('[data-i18n-ph]').forEach((el) => {
    el.placeholder = t(el.dataset.i18nPh);
  });
  root.querySelectorAll('[data-i18n-title]').forEach((el) => {
    el.title = t(el.dataset.i18nTitle);
  });
}

/* 初始同步：静态 HTML 里的 ui-button 等组件会先于本模块（经 console.js 的模块图
 * 延迟加载）完成首帧渲染，此刻 globalThis.UIReference.i18n 尚未就位，按
 * data-i18n-key 解析会落空成空白。词典就位后补发一次变更事件 + 刷新标记 DOM。 */
const bootstrapI18n = () => {
  applyDom();
  document.dispatchEvent(new CustomEvent('ui-i18n-change'));
};
if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', bootstrapI18n, { once: true });
} else {
  bootstrapI18n();
}
