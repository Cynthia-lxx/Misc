#include<bits/stdc++.h>
using std::pair;
using std::make_pair;
using std::sort;
using std::cin;
using std::cout;
using std::min;

int n;
pair<int,int> a[1005];

inline void solve() {
    cin >> n;
    for (int i=0;i<n;++i) {
        cin >> a[i].second >> a[i].first;
    }
    sort(a,a+n);
    int ans = a[0].first - a[0].second;
    for (int i=1;i<n;++i) {
        ans = min(ans, a[i].first - a[i].second);
        if (ans < 0) {
            cout << -1;
            return;
        }
        a[i].first = min(a[i].first, a[i-1].first - a[i-1].second);
    }
    cout << ans;
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}