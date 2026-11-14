toolchain/ - project-vendored local build tools

MinGW64/
  Source : F:/Software/Dev-Cpp/MinGW64 (Dev-C++ bundled TDM64 GCC)
  Version: g++.exe (tdm64-1) 4.9.2
  Size   : 336.4 MB / 5755 files
  Used by: web/analyzer/toolchain.py (compile -std=c++14 -O2, size.exe, gprof.exe)

AStyle.exe
  Source : F:/Software/Dev-Cpp/AStyle/AStyle.exe
  Used by: optional style diff preview only; never rewrites user code by default

Licensing
  TDM-GCC / MinGW-w64 runtime ships under GPL-3.0 with the GCC Runtime Library
  Exception; headers and runtime files carry their own LGPL/MIT-like notices.
  Artistic Style (AStyle) is MIT licensed. This directory is a verbatim local
  copy for offline use: keep every upstream notice inside MinGW64/ (share/, doc/).

Operational constraints
  - Runtime resolves executables ONLY from ROOT_DIR/toolchain/MinGW64/bin.
  - No dependency on F:/Software/Dev-Cpp at runtime.
  - Add toolchain/MinGW64/bin to the child process PATH (or link with
    -static-libgcc -static-libstdc++) so runtime DLLs resolve.
  - GCC 4.9.2: C++14 + bits/stdc++.h only; do not probe with C++17 syntax.

Optional trim list (kept verbatim by default)
  share/ (13.8 MB), doc/man pages, python27.dll, gdb/gdbserver symbols.