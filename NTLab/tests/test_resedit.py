"""PE 资源编辑器（resedit）回归测试：提取 / 打包 / 替换（缩小 + 变大重建）往返。"""

from __future__ import annotations

import os
import shutil

import pytest

from ntlab.core.errors import NtlabError
from ntlab.core.pe import resedit
from ntlab.core.pe.parser import _load

WINDIR = os.environ.get("WINDIR", r"C:\Windows")
NOTEPAD = os.path.join(WINDIR, "System32", "notepad.exe")
# advapi32res.dll 的 .rsrc 位于文件尾，可触发整体重建（变大）路径
GROW_SRC = os.path.join(WINDIR, "System32", "advapi32res.dll")

requires_windows_binaries = pytest.mark.skipif(
    not (os.path.isfile(NOTEPAD) and os.path.isfile(GROW_SRC)),
    reason="Windows system binaries unavailable",
)


def _items(path: str) -> list[dict]:
    return resedit._walk(_load(path))


def test_reject_missing():
    with pytest.raises(NtlabError):
        resedit.extract(r"C:\definitely\not\here.exe", "RT", "N", 0)


@requires_windows_binaries
def test_extract_and_pack():
    items = _items(NOTEPAD)
    assert items, "notepad.exe should carry resources"
    it0 = items[0]
    data = resedit.extract(NOTEPAD, it0["type"], it0["name"], it0["lang"])
    assert data == it0["data"]

    blob, count, manifest = resedit.pack_zip(NOTEPAD)
    assert count == len(items) == len(manifest)
    assert blob[:2] == b"PK"
    assert {m["file"] for m in manifest} == {m["file"] for m in manifest}


@requires_windows_binaries
def test_replace_shrink_roundtrip(tmp_path):
    items = _items(NOTEPAD)
    it0 = items[0]
    src = tmp_path / "src.exe"
    shutil.copyfile(NOTEPAD, src)
    small = it0["data"][: max(1, len(it0["data"]) // 4)]

    r = resedit.replace_resources(str(src), [
        {"type": it0["type"], "name": it0["name"], "lang": it0["lang"],
         "data": small}], out_path=str(tmp_path / "out.exe"))
    assert not r["grew"]
    assert r["rsrc_size"] <= r["old_rsrc_size"]

    out_path = str(tmp_path / "out.exe")
    out_items = _items(out_path)
    assert len(out_items) == len(items)
    # 全部资源逐项字节一致（曾因 bytearray 切片钳制导致整体错位）
    by_key = {(x["type"], x["name"], x["lang"]): x for x in out_items}
    for it in items:
        key = (it["type"], it["name"], it["lang"])
        expect = small if key == (it0["type"], it0["name"], it0["lang"]) else it["data"]
        assert by_key[key]["data"] == expect, f"roundtrip mismatch {key}"


@requires_windows_binaries
def test_replace_grow_full_rebuild(tmp_path):
    gitems = _items(GROW_SRC)
    assert gitems
    git0 = gitems[0]
    src = tmp_path / "src.exe"
    shutil.copyfile(GROW_SRC, src)
    big = bytes(range(256)) * 800          # 204800 字节，超过整个旧 .rsrc

    r = resedit.replace_resources(str(src), [
        {"type": git0["type"], "name": git0["name"], "lang": git0["lang"],
         "data": big}], out_path=str(tmp_path / "out.exe"))
    assert r["grew"] and r["rsrc_size"] > r["old_rsrc_size"]

    out_path = str(tmp_path / "out.exe")
    out_items = _items(out_path)
    assert len(out_items) == len(gitems)
    assert resedit.extract(out_path, git0["type"], git0["name"], git0["lang"]) == big
    # 其余资源逐项原样保留
    for it in gitems:
        key = (it["type"], it["name"], it["lang"])
        if key == (git0["type"], git0["name"], git0["lang"]):
            continue
        assert resedit.extract(out_path, *key) == it["data"], f"kept mismatch {key}"


@requires_windows_binaries
def test_replace_in_place_creates_backup(tmp_path):
    items = _items(GROW_SRC)
    it0 = items[0]
    src = tmp_path / "inplace.exe"
    shutil.copyfile(GROW_SRC, src)
    orig = src.read_bytes()

    r = resedit.replace_resources(str(src), [
        {"type": it0["type"], "name": it0["name"], "lang": it0["lang"],
         "data": it0["data"]}], in_place=True)
    assert r["backup"] and os.path.isfile(r["backup"])
    assert open(r["backup"], "rb").read() == orig


@requires_windows_binaries
def test_replace_unknown_key_rejected(tmp_path):
    src = tmp_path / "u.exe"
    shutil.copyfile(GROW_SRC, src)
    with pytest.raises(NtlabError):
        resedit.replace_resources(str(src), [
            {"type": "NOPE", "name": "NOPE", "lang": 0, "data": b"x"}])
