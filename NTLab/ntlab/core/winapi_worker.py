"""独立子进程：以通用指针签名调用本机 DLL 导出。

仅由 core/winapi.py 的 run_call() 通过 subprocess 启动； stdin 一行 JSON，
stdout 一行 JSON。崩溃/访问冲突只影响本进程，不波及 NTLab 服务器。

通用 ABI 约定（Windows x64）：
- 最多 4 个参数，一律指针宽度（c_void_p）；整数按地址传入，字符串 UTF-16 缓冲区。
- restype = c_void_p（返回指针/整数值）；不做任何解引用。
"""

from __future__ import annotations

import ctypes
import json
import sys


def main() -> None:
    try:
        req = json.loads(sys.stdin.readline())
        path = str(req["path"])
        name = str(req["name"])
        args = req.get("args") or []
        if not isinstance(args, list) or len(args) > 4:
            raise ValueError("generic call accepts at most 4 arguments")

        dll = ctypes.WinDLL(path, use_last_error=True)
        fn = getattr(dll, name)          # 导出名；序号导出不在通用调用范围内
        fn.restype = ctypes.c_void_p
        fn.argtypes = [ctypes.c_void_p] * len(args)

        buffers = []
        cargs = []
        for v in args:
            if v is None or v == "":
                cargs.append(None)
            elif isinstance(v, str):
                if v.lower() == "null":
                    cargs.append(None)
                else:
                    text = v[:4096]
                    buf = ctypes.create_unicode_buffer(text)
                    buffers.append(buf)
                    cargs.append(ctypes.cast(buf, ctypes.c_void_p))
            else:
                try:
                    n = int(v, 0) if isinstance(v, str) else int(v)
                except (TypeError, ValueError) as exc:
                    raise ValueError(f"argument {v!r} is not an integer/null/string") from exc
                cargs.append(ctypes.c_void_p(n & ((1 << 64) - 1)))

        ret = fn(*cargs)
        print(json.dumps({"ok": True, "ret": int(ret or 0),
                          "last_error": ctypes.get_last_error()}))
    except Exception as exc:  # noqa: BLE001 — worker 边界，任何异常都回传父进程
        print(json.dumps({"ok": False, "error": f"{type(exc).__name__}: {exc}"}))


if __name__ == "__main__":
    main()
