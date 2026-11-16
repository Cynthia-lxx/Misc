<div align="center">

# NTLab

**A local-server toolkit for Windows system internals inspection, experimentation and research.**

</div>

## What's This?

`NTLab` serves a browser-based environment for examining Windows binaries, the NT object-manager namespace and Win32/NT API behavior. A Python server runs on the loopback interface and delivers a single-page application built from native Custom Elements.

This build includes three tools: a PE analyzer, an NT object directory browser (ObjDir) and a WinAPI Playground driven by a server-side whitelist. Analysis runs against local file paths or in-memory uploads, and each tool returns structured JSON to the frontend. The API surface operates in read mode; system information, PE parsing, object enumeration and whitelisted calls are exposed as discrete endpoints.

> The server binds `127.0.0.1` by default. A custom host value produces a startup warning. Most namespaces are accessible under a standard user account; protected directories return their NTSTATUS value through the normal error channel.

Architecture decisions live in [`docs/architecture.md`](docs/architecture.md), and the staged feature plan in [`docs/roadmap.md`](docs/roadmap.md).

## What it looks like

PE analysis opens in a tabbed inspector. Summary, header, section, import, export, resource and signature views share a single file context:

![PE Viewer summary view](assets/pe-viewer-summary.png)

Large tables use pagination and typed columns:

<p align="center">
  <img src="assets/pe-viewer-imports.png" alt="PE Viewer imports table with paginated rows and typed columns" width="760" />
</p>

IDA-style views render binary structure as text:

<p align="center">
  <img src="assets/pe-viewer-disasm.png" alt="PE Viewer disassembly view with IAT comments and cross references" width="760" />
</p>

## Features

`NTLab` organizes its capabilities into binary inspection, namespace browsing, API experimentation and a component-based frontend. Each area below lists the behavior together with the modules that provide it.

- Local execution through one entry point, `main.py`, with uvicorn bound to the loopback interface.
- Read-mode endpoints for PE parsing, object enumeration and system information.
- Static frontend delivery from `ntlab/web/`, with ES modules and hash routing loaded directly by the browser.
- Child-process isolation with a fixed timeout for exported functions discovered at runtime.
- A pytest suite under `tests/` covering PE parsing, IDA-style views, namespace access, call queues and program control flow.

### PE Viewer — structured binary inspection

The PE analyzer accepts a local path, a drag-and-drop upload (up to 64 MB) or one of five preset system files. Recent paths persist in `localStorage`, and uploaded files stay in memory. Parsing goes through a `pefile` wrapper that caches results by `(path, mtime, size)` with an eight-entry bound, so repeated reads of large images such as `ntoskrnl.exe` reuse the parsed object.

![PE Viewer signature and PDB information](assets/pe-viewer-signature.png)

The inspector exposes a tab per data domain:

| Tab | Content | Source module |
|---|---|---|
| Summary | Machine, subsystem, entry point, section count, directory layout | `ntlab/core/pe/parser.py` |
| Headers | DOS, NT and optional header fields | `ntlab/core/pe/parser.py` |
| Sections | Name, virtual address, raw size and characteristics | `ntlab/core/pe/parser.py` |
| Imports / Exports | Paginated symbol tables with `offset` / `limit` | `ntlab/core/pe/parser.py` |
| Resources | Resource tree and entry metadata | `ntlab/core/pe/parser.py` |
| Signature | Authenticode and PDB descriptor fields | `ntlab/core/pe/parser.py` |
| Strings | ASCII and UTF-16LE printable runs | `ntlab/core/pe/hexdump.py` |
| Hex view | 16-byte rows addressed by RVA or file offset | `ntlab/core/pe/hexdump.py` |
| Search | Text or hexadecimal patterns with a case toggle | `ntlab/core/pe/hexdump.py` |
| Disassembly | Linear x86-64 instructions with rip-relative IAT comments | `ntlab/core/pe/disasm.py` |

Cross references resolve call and jump targets into a target-to-callers map, and MSVC decorated names are expanded through `dbghelp.UnDecorateSymbolName` in `ntlab/core/pe/demangle.py`. Import and export rows link into the WinAPI Playground, carrying the module and symbol names into a call card.

<p align="center">
  <img src="assets/pe-viewer-strings-hex.png" alt="PE Viewer strings and hexadecimal view with the binary search panel" width="760" />
</p>

Fourteen REST endpoints in `ntlab/api/pe.py` serve these views: `/parse`, `/headers`, `/imports`, `/exports`, `/resources`, `/signature`, `/strings`, `/hex`, `/search`, `/disasm`, `/xrefs`, `/demangle`, `/resolve` and `/upload`.

### ObjDir — NT object namespace browser

ObjDir presents the object-manager namespace as a tree. Twelve well-known roots are probed at startup, and directory children are requested from `/api/ntobj/list` only when a node expands. A breadcrumb tracks the active path, and the detail panel resolves symbolic-link targets.

<p align="center">
  <img src="assets/objdir-tree.png" alt="ObjDir lazy tree, breadcrumb navigation and symbolic link detail panel" width="760" />
</p>

The backend contract is explicit: `ntlab/core/ntobj/ntapi.py` declares `NtOpenDirectoryObject` with `DIRECTORY_QUERY | TRAVERSE`, `NtQueryDirectoryObject`, `NtOpenSymbolicLinkObject`, `NtQuerySymbolicLinkObject` and `NtClose`. Handles are released in `finally` blocks. `browser.py` validates paths (leading `\`, length ≤ 1024, NUL-byte rejection), enumerates entries and maps NTSTATUS values to `{status, name, message}` through `STATUS_NAMES` and `STATUS_MESSAGES`.

Three endpoints in `ntlab/api/ntobj.py` serve the page: `/roots`, `/list` and `/object`.

### WinAPI Playground — whitelisted calls and program flow

The Playground turns API descriptions into executable call cards. `ntlab/core/winapi.py` defines a catalog of roughly twenty APIs across `kernel32`, `user32` and `ntdll`, each carrying a parameter schema, a side-effect grade (`safe` or `observable`) and a cleanup action. The whitelist is enforced in the backend.

<p align="center">
  <img src="assets/playground-blocks.png" alt="WinAPI Playground block program with call cards and ordered steps" width="760" />
</p>

Execution modes:

- **Call queue** — up to 64 steps, with references such as `$N`, `$N.ret` and `$N.out.x` feeding later arguments.
- **Program orchestrator** — `call`, `repeat`, `if`, `break` and `continue`, with expressions evaluated through an AST whitelist and a 2000-step budget.
- **Dynamic export calls** — exports discovered by the library scanner run in a child process through `ntlab/core/winapi_worker.py`, using a Windows x64 ABI shim and a two-second timeout.
- **Assembler** — keystone assembles x86-64 text and capstone reads the bytes back for verification; the bytes remain rendered as text.

The export library scanner in `ntlab/core/winapi_library.py` walks `System32` and `SysWOW64` in a background thread, caches results in process memory and classifies exports as `generic`, `restricted` or `blocked` through a pattern list covering write, deletion, process, registry-write and memory-operation families.

Return values, `GetLastError` and NTSTATUS render beside each step, and completed runs are recorded as sessions.

<p align="center">
  <img src="assets/playground-asm.png" alt="WinAPI Playground assembly text synchronized with the block program" width="760" />
</p>

`programToAsm()` and `asmToProgram()` in `ntlab/web/pages/playground.js` keep the block program and the x86-64 text view synchronized in both directions, including register call conventions (`rcx`, `rdx`, `r8`, `r9`) and `$N` references.

<p align="center">
  <img src="assets/playground-result.png" alt="WinAPI Playground result panel showing return value, GetLastError and NTSTATUS" width="760" />
</p>

### Frontend — native components and shared design tokens

The browser application loads ES modules directly, with a hash router in `ntlab/web/router.js` coordinating `exitPage`, content swap and `enterPage` transitions. A small REST helper in `api.js` unwraps the shared `{error: {code, message, detail}}` envelope.

Twenty-two Custom Elements come from an MIT-licensed, osu!-derived reference kit under `ntlab/web/vendor/`. Design tokens in `tokens.css` and theme layers in `themes.css` provide nine color schemes, with `blue` as the default, plus `en` and `zh-CN` message catalogs.

Three first-party elements extend that kit:

| Element | Behavior |
|---|---|
| `ui-tree` | Callback-driven lazy children, directory and link badges, `ui-tree-select` events |
| `ui-table` | Typed columns (`text`, `hex`, `num`, `mono`, `link`), sorting, row and cell activation events |
| `ui-codeview` | Fixed-row virtual scrolling for 100k+ lines, incremental `loadMore()`, jump events |

The animation layer (`motion.js` / `motion.css`) uses the Web Animations API for page swaps and staggered entrances, with a `prefers-reduced-motion` path and an optional `localStorage` probe for frame-level inspection.

### Architecture — layered core with a thin API

```
Browser (SPA · hash routing)
   │  REST JSON  /api/*
   ▼
ntlab/api        FastAPI routers: system · pe · ntobj · winapi
   │                 exception handler · no-store middleware
   ├─ ntlab/core/pe       pefile / capstone / dbghelp → dict
   ├─ ntlab/core/ntobj    ctypes ntdll declarations, read-mode
   ├─ ntlab/core/winapi   catalog · runner · program · assembler
   └─ ntlab/core/errors   NtlabError → JSON body + HTTP status
   │
ntlab/web        static SPA · vendor elements · first-party elements
```

`ntlab/core` keeps its logic in plain Python and leaves HTTP concerns in `ntlab/api`, so a future CLI shell can reuse the same functions. `create_app()` in `ntlab/api/__init__.py` registers the four routers, mounts the exception handler, adds a `Cache-Control: no-store` middleware and serves `ntlab/web/` as static files. `NtlabError` and its subclasses (`NotFoundError`, `InvalidPathError`, `ParseError`, `NtStatusError`) form the single error model across layers.

## Installation

NTLab targets Windows 10 and 11 on x64, with Python 3.12 or newer. Create an isolated environment in the project directory, then install the pinned dependency set:

```bash
python -m venv .venv
.venv\Scripts\python.exe -m pip install -r requirements.txt
```

The dependency groups in `requirements.txt` cover the web framework (`fastapi`, `uvicorn[standard]`, `python-multipart`), PE parsing (`pefile`), disassembly and assembly (`capstone`, `keystone-engine`), system access (`pywin32`), event streaming (`websockets`) and testing (`pytest`). Project metadata lives in `pyproject.toml`.

## Running

```bash
.venv\Scripts\python.exe main.py
```

`main.py` prints the service URL and opens a browser tab during startup. Command-line options:

| Flag | Default | Behavior |
|---|---|---|
| `--host` | `127.0.0.1` | Listening address; custom values emit a startup warning |
| `--port` | `8765` | Listening port |
| `--no-browser` | off | Suppresses the browser tab |

```bash
.venv\Scripts\python.exe main.py --port 8765 --no-browser
.venv\Scripts\python.exe -m pytest
```

The port state is checked with `netstat -ano | findstr :8765`; a restart completes once a single listener is confirmed.

## Keyboard interaction

Keyboard interaction is component-scoped and follows the conventions of the Custom Element kit:

| Context | Keys | Action |
|---|---|---|
| `ui-tabs` | `ArrowLeft`, `ArrowRight`, `Home`, `End` | Move between tabs |
| `ui-slider`, `ui-range-slider` | Arrows, `PageUp`, `PageDown`, `Home`, `End` | Adjust the value |
| `ui-textbox` | `Enter` | Commit the input |
| `ui-textbox` (`history`) | `ArrowUp`, `ArrowDown` | Recall earlier inputs |
| `ui-checkbox`, `ui-hold-button` | `Space`, `Enter` | Toggle or confirm |
| `ui-reveal-overlay` | `Escape` | Close the overlay |

## Project layout

```
NTLab/
├── main.py                  single entry point (uvicorn)
├── ntlab/
│   ├── api/                 FastAPI routers and app factory
│   ├── core/
│   │   ├── pe/              parser, disassembly, hexdump, demangling
│   │   ├── ntobj/           ctypes NT API and namespace browser
│   │   ├── errors.py        NtlabError model
│   │   ├── winapi.py        whitelist, runner, program, assembler
│   │   ├── winapi_library.py  export scanner and classifier
│   │   └── winapi_worker.py   isolated ABI call process
│   └── web/
│       ├── pages/           home, pe-viewer, objdir, playground
│       ├── components/      ui-tree, ui-table, ui-codeview
│       └── vendor/          MIT reference kit, themes, i18n
├── docs/                    architecture and roadmap
├── legacy/                  archived ctypes experiment scripts
└── tests/                   pytest suites
```

## Roadmap

| Stage | Scope | State in this build |
|---|---|---|
| Phase 1 | Project skeleton, PE analyzer, ObjDir | Implemented |
| Phase 2 | WinAPI Playground: whitelist, program orchestrator, assembler, export library | Implemented |
| Phase 3 | Live observation: ETW dashboards, window message spy, timer-resolution experiments | Specified in [`docs/roadmap.md`](docs/roadmap.md) |

## Documentation

- [`docs/architecture.md`](docs/architecture.md) — layering, key decisions, error model and vendor assembly.
- [`docs/roadmap.md`](docs/roadmap.md) — staged feature plan and longer-term topics.
- [`AGENTS.md`](AGENTS.md) — repository conventions, directory responsibilities and the local port-check procedure.
- [`ntlab/web/vendor/LICENSE-MIT.txt`](ntlab/web/vendor/LICENSE-MIT.txt) — license for the vendored reference kit.
- [中文说明](README_zh.md)

## Image assets

The image references above are placeholders. The table below lists the intended asset paths under `assets/` and their content:

| File | Content |
|---|---|
| `assets/pe-viewer-summary.png` | PE Viewer summary tab with machine, subsystem and section overview |
| `assets/pe-viewer-imports.png` | PE Viewer imports table showing pagination and typed columns |
| `assets/pe-viewer-disasm.png` | PE Viewer disassembly with IAT comments and cross-reference panel |
| `assets/pe-viewer-strings-hex.png` | PE Viewer strings and hex view with the binary search panel |
| `assets/pe-viewer-signature.png` | PE Viewer signature and PDB information tab |
| `assets/objdir-tree.png` | ObjDir lazy tree, breadcrumb and symbolic-link detail panel |
| `assets/playground-blocks.png` | Playground block program with ordered call cards |
| `assets/playground-asm.png` | Playground assembly text view alongside the block program |
| `assets/playground-result.png` | Playground result panel with return value, GetLastError and NTSTATUS |

## Version and credits

- **Version** — `0.1.0` (`ntlab/__init__.py`, `pyproject.toml`).
- **Author** — wjz-s.
- **License** — GPL-3.0-or-later for NTLab code.
- **Vendored UI** — MIT, derived from ppy/osu reference components; the compatibility record is in `ntlab/web/vendor/licence-audit.json`.
