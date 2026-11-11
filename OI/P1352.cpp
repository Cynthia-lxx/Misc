#include<bits/stdc++.h>
using std::max;
using std::vector;

int n, r[6005], dp[6005][2], root;
vector<int> son[6005];
bool has_fa[6005];

void dfs(int u) {
    dp[u][1] = r[u];
    for (int v : son[u]) {
        dfs(v);
        dp[u][0] += max(dp[v][0], dp[v][1]);
        dp[u][1] += dp[v][0];
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin >> n;
    for (int i = 1; i <= n; ++i) std::cin >> r[i];
    for (int i = 1; i < n; ++i) {
        int l, k;
        std::cin >> l >> k;
        son[k].push_back(l);
        has_fa[l] = true;
    }
    for (int i = 1; i <= n; ++i) if (!has_fa[i]) { root = i; break; }
    dfs(root);
    std::cout << max(dp[root][0], dp[root][1]) << '\n';
    return 0;
}