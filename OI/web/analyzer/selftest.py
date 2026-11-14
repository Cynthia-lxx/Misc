"""Pure-local smoke test: python web/analyzer/selftest.py"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from analyzer import engine, report  # noqa: E402

RICH = """#include<bits/stdc++.h>
using namespace std;
int l[1000000+1145];
int g[305][305]; //坐标表喵
int main(){
    ios::sync_with_stdio(false);
    int n; cin>>n;
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++)
            for(int k=1;k<=n;k++) l[i]+=g[j][k];
    vector<int> v;
    for(int i=0;i<n;i++) v.push_back(i);
    cout<<l[n]<<std::endl;
    return 0;
}
"""

UNKNOWN = """#include<bits/stdc++.h>
struct Node{int x,y;};
Node tree[N+5];
int f(int x){ if(x<2) return x; return f(x-1)+f(x-2); }
int main(){ int n; cin>>n; cout<<f(n); }
"""


def check(condition: bool, label: str) -> None:
    print(("PASS  " if condition else "FAIL  ") + label)
    if not condition:
        raise SystemExit(1)


def main() -> None:
    engine.clear_cache()
    rep, hit = engine.analyse(RICH, {"kind": "output", "name": "selftest"})
    dims = {f.dimension for f in rep.findings}
    check(dims == set(report.DIMENSIONS), f"all four dimensions present: {sorted(dims)}")
    check(rep.verdict in ("yellow", "red"), f"rich sample verdict = {rep.verdict}")
    check(bool(rep.assumptions), f"assumptions recorded = {len(rep.assumptions)}")
    check(any(f.key == "style.using_namespace_std" for f in rep.findings), "style rule 3 fires")
    check(any(f.key == "complexity.estimated_order" for f in rep.findings), "complexity estimate present")
    check(not hit, "first analyse is a cache miss")

    again, hit = engine.analyse(RICH, {"kind": "output", "name": "selftest"})
    check(hit and again is rep, "second analyse hits signature cache")

    unknown, _ = engine.analyse(UNKNOWN, {"kind": "output", "name": "selftest"})
    check(unknown.unknown_count >= 2, f"unknown findings = {unknown.unknown_count}")
    check(any(f.key == "complexity.recursion" for f in unknown.findings), "recursion is honestly unknown")
    check(any(f.key == "memory.unknown_arrays" for f in unknown.findings), "symbolic dimension is unknown")

    files = engine.list_cpp_files()
    check(len(files) >= 3, f"workspace .cpp files = {len(files)}")
    for item in files[:3]:
        path = Path(engine.ROOT_DIR) / item["path"]
        one, _ = engine.analyse(path.read_text(encoding="utf-8", errors="replace"),
                                {"kind": "file", "name": item["name"]})
        check(one.version == report.REPORT_VERSION and bool(one.findings),
              f"sample {item['name']}: {len(one.findings)} findings, verdict={one.verdict}")

    payload = rep.as_dict()
    check(payload["version"] == report.REPORT_VERSION and isinstance(payload["findings"], list),
          "report.as_dict() shape is stable")
    print(f"OK  cache={engine.cache_size()}  rich_verdict={rep.verdict}  unknown_verdict={unknown.verdict}")


if __name__ == "__main__":
    main()
