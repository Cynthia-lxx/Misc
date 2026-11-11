#include<bits/stdc++.h>
using std::string;
using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::make_pair;

int n;
string name[105], status[105];
int pos[105]; // 最终位置喵
vector<pair<int,int>> up, down; // (原位置, 最终位置)喵
int up_idx, down_idx;

inline void solve() {
    cin >> n;
    for (int i = 0; i < n; ++i) {
        cin >> name[i] >> status[i];
        if (status[i] == "SAME") pos[i] = i;
        else if (status[i] == "UP") up.push_back(make_pair(i, -1));
        else down.push_back(make_pair(i, -1));
    }
    
    // 从后往前填DOWN，从前往后填UP喵
    for (int i = n-1; i >= 0 && down_idx < (int)down.size(); --i) {
        if (pos[i] == -1) {
            pos[down[down_idx].first] = i;
            ++down_idx;
        }
    }
    for (int i = 0; i < n && up_idx < (int)up.size(); ++i) {
        if (pos[i] == -1) {
            pos[up[up_idx].first] = i;
            ++up_idx;
        }
    }
    
    string ans[105];
    for (int i = 0; i < n; ++i) ans[pos[i]] = name[i];
    for (int i = 0; i < n; ++i) cout << ans[i] << "\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    solve();
    return 0;
}