# NTLab

Windows 系统底层「玩 / 测试 / 研究」工具集。A local-server toolkit for exploring Windows internals — user-mode friendly, read-only by design.

## Tools (Phase 1)

| Tool | What it does |
|---|---|
| **PE Viewer** | Drag in any exe/dll/sys — DOS header, sections, imports, exports, resources, signature & PDB info |
| **ObjDir** | Browse the NT object-manager namespace (`\Device`, `\BaseNamedObjects`, `\KernelObjects`…) with lazy-load tree + symlink targets |

## Run

```
python -m venv .venv
.venv\Scripts\python.exe -m pip install -r requirements.txt
.venv\Scripts\python.exe main.py        # → http://127.0.0.1:8765
```

Windows 10/11, x64. No admin required for most namespaces (some dirs will report ACCESS_DENIED — that is expected and part of the fun).

## Architecture

```
main.py → ntlab/api (FastAPI, thin) → ntlab/core (pe / ntobj, pure Python)
                                     → ntlab/web (SPA, osu!-style custom elements)
```

- Backend: FastAPI + pefile + ctypes NT-API calls. Read-only.
- Frontend: no bundler. UI components vendored from an osu!-derived reference kit (MIT, see `ntlab/web/vendor/LICENSE-MIT.txt`); `ui-tree` / `ui-table` are first-party.
- CLI shells can reuse `ntlab/core` directly (planned).

## Roadmap

- **Phase 1** (current): skeleton + PE Viewer + ObjDir
- **Phase 2**: WinAPI Playground — interactive, whitelisted API calls with live GetLastError/NTSTATUS rendering (whitelist enforced server-side)
- **Phase 3**: live observation — ETW dashboards (process/file/registry), window message spy, timer-resolution experiments

See `docs/architecture.md` and `docs/roadmap.md`.

## Licence

- NTLab code: GPL-3.0-or-later
- `ntlab/web/vendor/`: MIT, derived from ppy/osu (no osu! branding), audit in `licence-audit.json`
