#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::istream;
using std::pair;
using std::make_pair;
using std::abs;
using std::endl;

typedef long long ll;
typedef pair<int,int> pii;

inline ll cross(pii a, pii b, pii c) {
    return (ll)(b.first - a.first) * (c.second - a.second) - (ll)(b.second - a.second) * (c.first - a.first);
}

inline bool onSegment(pii p, pii a, pii b) {
    if (cross(a, b, p) != 0) return false;
    int minx = std::min(a.first, b.first), maxx = std::max(a.first, b.first);
    int miny = std::min(a.second, b.second), maxy = std::max(a.second, b.second);
    return p.first >= minx && p.first <= maxx && p.second >= miny && p.second <= maxy;
}

pii readPoint(istream& is) {
    int x, y;
    char c;
    is >> c >> x >> c >> y >> c;
    return make_pair(x, y);
}

int main() {
    pii A = readPoint(cin);
    pii B = readPoint(cin);
    pii C = readPoint(cin);
    pii P = readPoint(cin);

    // 检查是否在顶点上
    if (P == A || P == B || P == C) {
        cout << 4 << endl;
        return 0;
    }

    // 检查是否在边上
    if (onSegment(P, A, B) || onSegment(P, B, C) || onSegment(P, C, A)) {
        cout << 3 << endl;
        return 0;
    }

    // 检查是否在内部（用叉积方向判断）
    ll c1 = cross(A, B, P);
    ll c2 = cross(B, C, P);
    ll c3 = cross(C, A, P);

    // 如果三个叉积同号（都正或都负），则在内部
    if ((c1 > 0 && c2 > 0 && c3 > 0) || (c1 < 0 && c2 < 0 && c3 < 0)) {
        cout << 1 << endl;
    } else {
        cout << 2 << endl;
    }

    return 0;
}