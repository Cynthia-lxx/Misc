#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
using std::min;

int n, h[10005], l[10005], r[10005], ans;

int main() {
    cin >> n;
    for (int i = 1; i <= n; ++i) cin >> h[i];
    l[0] = 0;
    for (int i = 1; i <= n; ++i) l[i] = max(l[i-1], h[i]);
    r[n+1] = 0;
    for (int i = n; i >= 1; --i) r[i] = max(r[i+1], h[i]);
    for (int i = 1; i <= n; ++i) ans += min(l[i], r[i]) - h[i];
    cout << ans;
    return 0;
}