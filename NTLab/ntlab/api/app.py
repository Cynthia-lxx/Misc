"""应用级端点：设置读写 + 终端日志镜像 SSE（客户端「调试」页用）。

POST 保存后由调用方决定如何生效：
- 客户端（pywebview）经 js_api 热应用（换端口=重启服务线程，终端开关=以 pythonw/py 重启进程）
- 纯浏览器模式：设置已落盘，重启 NTLab 后生效
"""

from __future__ import annotations

import asyncio
import json

from fastapi import APIRouter, Request
from fastapi.responses import StreamingResponse

from ntlab.core import appsettings

router = APIRouter()


@router.get("/settings")
def get_settings() -> dict:
    s = appsettings.load()
    return {
        "settings": s,
        "effective": {
            "console_attached": _console_attached(),
            "is_client": _in_client(),
            "current_port": _current_port(),
        },
    }


def _current_port() -> int | None:
    """运行中服务的端口（启动方注入 NTLAB_PORT；未注入则未知）。"""
    import os

    v = os.environ.get("NTLAB_PORT")
    try:
        return int(v) if v else None
    except ValueError:
        return None


@router.get("/logs/stream")
async def logs_stream(request: Request) -> StreamingResponse:
    """终端日志镜像（SSE）：回放最近 200 行后实时推送，ANSI 彩色文本原样下发。"""
    from ntlab.core.loghub import hub

    async def gen():
        seq = 0
        backlog = hub.after(0)[-200:]
        for s, _ts, text in backlog:
            seq = s
            yield f"data: {json.dumps({'seq': s, 'text': text}, ensure_ascii=False)}\n\n"
        while True:
            if await request.is_disconnected():
                return
            # hub.wait 是 threading.Event 阻塞调用，丢进线程池避免卡事件循环
            await asyncio.to_thread(hub.wait, seq, 0.4)
            for s, _ts, text in hub.after(seq):
                seq = s
                yield f"data: {json.dumps({'seq': s, 'text': text}, ensure_ascii=False)}\n\n"

    return StreamingResponse(gen(), media_type="text/event-stream",
                             headers={"X-Accel-Buffering": "no"})


@router.post("/settings")
def post_settings(body: dict) -> dict:
    saved = appsettings.save(body)
    return {"settings": saved, "message": "已保存"}


def _console_attached() -> bool:
    """当前进程是否带控制台窗口（py vs pyw 的实际状态）。"""
    try:
        import ctypes

        return ctypes.windll.kernel32.GetConsoleWindow() != 0
    except Exception:
        return True


def _in_client() -> bool:
    """是否运行于内嵌浏览器客户端内（由客户端注入的请求头判定不了，先以环境变量判定）。"""
    import os

    return os.environ.get("NTLAB_CLIENT") == "1"
