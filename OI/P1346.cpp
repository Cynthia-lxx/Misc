#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::priority_queue;
using std::greater;

const int INF = 0x3f3f3f3f;
int n,a,b,k,v,dist[105];
vector<pair<int,int>> g[105];
priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int>>> pq;

int main() {
    cin >> n >> a >> b;
    for (int i = 1; i <= n; ++i) {
        cin >> k;
        for (int j = 0; j < k; ++j) {
            cin >> v;
            if (j == 0) g[i].push_back({v, 0}); // 默认轨道不需要切换喵
            else g[i].push_back({v, 1}); // 其他轨道需要切换一次喵
        }
    }
    memset(dist, 0x3f, sizeof(dist));
    dist[a] = 0;
    pq.push({0, a});
    while (!pq.empty()) {
        auto [d, u] = pq.top(); pq.pop();
        if (d != dist[u]) continue;
        for (auto [to, w] : g[u]) {
            if (dist[to] > d + w) {
                dist[to] = d + w;
                pq.push({dist[to], to});
            }
        }
    }
    if (dist[b] == INF) cout << -1;
    else cout << dist[b];
    return 0;
}