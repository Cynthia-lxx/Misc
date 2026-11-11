#include<bits/stdc++.h>
using std::vector;
using std::pair;
using std::make_pair;
using std::priority_queue;
using std::greater;
using std::cin;
using std::cout;
using std::endl;
using std::fill;
using std::max;

typedef long long ll;
typedef pair<ll,int> pli;

const ll INF = 1e18;

int n,m;
vector<pair<int,int>> g[2][1000005];
ll d[2][1000005];
bool vis[1000005];

void dijkstra(int s, int t) {
    fill(d[t], d[t]+n+1, INF);
    fill(vis, vis+n+1, false);
    priority_queue<pli, vector<pli>, greater<pli>> pq;
    d[t][s] = 0;
    pq.push(make_pair(0, s));
    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        if (vis[u]) continue;
        vis[u] = true;
        for (auto &e : g[t][u]) {
            int v = e.first, w = e.second;
            if (d[t][v] > d[t][u] + w) {
                d[t][v] = d[t][u] + w;
                pq.push(make_pair(d[t][v], v));
            }
        }
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    cin >> n >> m;
    for (int i = 0; i < m; i++) {
        int u,v,w;
        cin >> u >> v >> w;
        g[0][u].push_back(make_pair(v, w));
        g[1][v].push_back(make_pair(u, w));
    }
    dijkstra(1, 0);
    dijkstra(1, 1);
    ll ans = 0;
    for (int i = 1; i <= n; i++) {
        ans += d[0][i] + d[1][i];
    }
    cout << ans << endl;
    return 0;
}