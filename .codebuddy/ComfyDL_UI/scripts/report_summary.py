"""report_summary.py — 汇总 import_scan 的 JSON 报告，输出脱水清单所需的分类统计。"""
import collections
import json
import sys


def main():
    report = sys.argv[1] if len(sys.argv) > 1 else r"p:\Dev\ComfyUI_Refs\.codebuddy\scripts\import_report.json"
    with open(report, encoding="utf-8") as f:
        d = json.load(f)
    keep = set(d["keep_closure"])
    dele = set(d["delete_candidates"])

    print(f"闭合集: {len(keep)}  删除候选: {len(dele)}")

    cnt = collections.Counter()
    for m in dele:
        cnt[m.split(".")[0]] += 1
    print("\n--- 删除候选按顶层分布 ---")
    for k, v in cnt.most_common():
        print(f"  {k}: {v}")

    for prefix in ("comfy", "comfy_extras", "comfy_api_nodes", "comfy_execution", "comfy_api", "app", "api_server", "middleware", "utils", "comfy_config", "tests", "tests-unit", "script_examples", "custom_nodes", "alembic_db"):
        ke = [m for m in keep if m == prefix or m.startswith(prefix + ".")]
        de = [m for m in dele if m == prefix or m.startswith(prefix + ".")]
        print(f"\n[{prefix}] 保留 {len(ke)} / 删除 {len(de)}")
        if prefix in ("comfy_extras",) and ke:
            print("  保留列表:")
            for m in sorted(ke):
                print(f"    {m}")


if __name__ == "__main__":
    main()
