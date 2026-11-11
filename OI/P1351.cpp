#include<bits/stdc++.h>
using std::vector;
using std::max;
using std::pair;
using std::make_pair;
using std::cin;
using std::cout;

const int MOD = 10007;
int n, w[200005], ans_max, ans_sum;
vector<int> g[200005];

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    cin >> n;
    for (int i = 1; i < n; ++i) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
        g[v].push_back(u);
    }
    for (int i = 1; i <= n; ++i) cin >> w[i];
    for (int u = 1; u <= n; ++u) {
        int max1 = 0, max2 = 0, sum = 0, sum_sq = 0;
        for (int v : g[u]) {
            sum = (sum + w[v]) % MOD;
            sum_sq = (sum_sq + w[v] * w[v]) % MOD;
            if (w[v] > max1) {
                max2 = max1;
                max1 = w[v];
            } else if (w[v] > max2) {
                max2 = w[v];
            }
        }
        ans_max = max(ans_max, max1 * max2);
        ans_sum = (ans_sum + (sum * sum % MOD - sum_sq + MOD) % MOD) % MOD;
    }
    cout << ans_max << ' ' << ans_sum << '\n';
    return 0;
}