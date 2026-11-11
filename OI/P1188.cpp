#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;

int n,k;
vector<int> l;

int main() {
    cin >> n >> k;
    l.resize(n);
    for (int i = 0; i < n; ++i) l[i] = i + 1;
    while (k--) {
        int a,b,c;
        cin >> a >> b >> c;
        --a; --b;
        vector<int> seg(l.begin() + a, l.begin() + b + 1);
        l.erase(l.begin() + a, l.begin() + b + 1);
        int pos = c;
        if (c > a) pos = c - (b - a + 1); // 如果c在删除区间之后，要减去区间长度喵
        l.insert(l.begin() + pos, seg.begin(), seg.end());
    }
    for (int i = 0; i < 10; ++i) cout << l[i] << "\n";
    return 0;
}