#include<bits/stdc++.h>
using std::vector;
using std::queue;
using std::min;
using std::max;
using std::cerr;

const int MAXP = 30000 + 5;
const int MAXC = 100000 + 5;

int P, C, N;
vector<int> g[MAXP];
bool damaged[MAXP]; // 标记为受损的牧场喵
bool safe[MAXP];    // 确定安全的牧场喵
bool visited[MAXP];

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    std::cin >> P >> C >> N;
    for (int i = 0; i < C; ++i) {
        int a, b;
        std::cin >> a >> b;
        g[a].push_back(b);
        g[b].push_back(a);
    }
    
    // 初始时所有牧场都标记为受损，除了谷仓1喵
    for (int i = 2; i <= P; ++i) {
        damaged[i] = true;
    }
    damaged[1] = false;
    
    // 处理每个报告：报告中的牧场是安全的，但它的邻居必须受损喵
    for (int i = 0; i < N; ++i) {
        int r;
        std::cin >> r;
        safe[r] = true;
        damaged[r] = false; // 报告中的牧场未受损喵
        // 与报告牧场相邻的牧场必须受损（否则牛就能通过它们回去）喵
        for (int v : g[r]) {
            damaged[v] = true;
        }
    }
    
    // 从谷仓1开始BFS，只走安全的牧场喵
    queue<int> q;
    q.push(1);
    visited[1] = true;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : g[u]) {
            if (!visited[v] && !damaged[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    
    // 统计无法到达的牧场数量喵
    int ans = 0;
    for (int i = 1; i <= P; ++i) {
        if (!visited[i]) {
            ++ans;
        }
    }
    
    std::cout << ans << "\n";
    
    return 0;
}