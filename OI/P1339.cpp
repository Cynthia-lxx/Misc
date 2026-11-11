#include<bits/stdc++.h>
using std::vector;
using std::pair;
using std::priority_queue;
using std::greater;
using std::make_pair;
using std::cin;
using std::cout;

inline void add_edge(vector<pair<int,int>> g[], int u, int v, int w) {
    g[u].push_back(make_pair(v, w));
    g[v].push_back(make_pair(u, w));
}

int main() {
    int n,m,s,t;
    cin >> n >> m >> s >> t;
    vector<pair<int,int>> g[n+1];
    for (int _=0; _<m; ++_) {
        int u,v,w;
        cin >> u >> v >> w;
        add_edge(g, u, v, w);
    }
    int dist[n+1];
    bool vis[n+1];
    for (int i=1; i<=n; ++i) dist[i]=1e9, vis[i]=false;
    dist[s]=0;
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    pq.push(make_pair(0, s));
    while (!pq.empty()) {
        int d = pq.top().first, u = pq.top().second;
        pq.pop();
        if (vis[u]) continue;
        vis[u] = true;
        for (auto &e : g[u]) {
            int v = e.first, w = e.second;
            if (dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                pq.push(make_pair(dist[v], v));
            }
        }
    }
    cout << dist[t] << "\n";
    return 0;
}