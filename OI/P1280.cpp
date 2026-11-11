#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::max;

int n,k;
vector<pair<int,int>> tasks[10005];
int dp[10005]; // dp[i]表示从第i分钟到结束的最大空闲时间喵

int main() {
    cin >> n >> k;
    for (int i = 0; i < k; i++) {
        int p,t;
        cin >> p >> t;
        tasks[p].push_back({p, t});
    }
    for (int i = n; i >= 1; i--) {
        if (tasks[i].empty()) {
            dp[i] = dp[i+1] + 1;
        } else {
            for (auto &task : tasks[i]) {
                int end = task.first + task.second - 1;
                dp[i] = max(dp[i], dp[end+1]);
            }
        }
    }
    cout << dp[1];
    return 0;
}