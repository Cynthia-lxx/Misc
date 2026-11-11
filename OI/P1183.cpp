#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::abs;

int n, x[105], y[105], ans;

int main() {
    cin >> n;
    for (int i = 1; i <= n; ++i) cin >> x[i] >> y[i];
    x[n+1] = x[1]; y[n+1] = y[1];
    for (int i = 1; i <= n; ++i)
        ans += x[i] * y[i+1] - x[i+1] * y[i];
    ans = abs(ans) / 2;
    cout << ans;
    return 0;
}