#include<bits/stdc++.h>

using std::vector;
using std::string;
using std::pair;
using std::set;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int n;
struct FD {
    string left, right;
    int id;
} fds[105];

// 从字符串解析出字符集合
set<char> parse(const string& s) {
    set<char> res;
    for (char c : s) if (c != '-' && c != '>') res.insert(c);
    return res;
}

// 检查能否从 left 通过某些依赖推导出 target
// 返回使用的依赖编号序列（最短）
vector<int> can_derive(const set<char>& left, const set<char>& target, int skip_id) {
    set<char> cur = left;
    vector<int> used;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int i = 0; i < n; ++i) {
            if (fds[i].id == skip_id) continue;
            // 检查是否已经用过
            bool already = false;
            for (int u : used) if (u == fds[i].id) { already = true; break; }
            if (already) continue;
            // 检查左部是否全部在 cur 中
            set<char> lset = parse(fds[i].left);
            bool ok = true;
            for (char c : lset) if (cur.find(c) == cur.end()) { ok = false; break; }
            if (!ok) continue;
            // 应用这个依赖
            set<char> rset = parse(fds[i].right);
            for (char c : rset) cur.insert(c);
            used.push_back(fds[i].id);
            changed = true;
        }
    }
    // 检查是否包含 target
    for (char c : target) if (cur.find(c) == cur.end()) return {};
    return used;
}

int main() {
    cin >> n;
    for (int i = 0; i < n; ++i) {
        string s;
        cin >> s;
        fds[i].id = i + 1;
        size_t pos = s.find("->");
        fds[i].left = s.substr(0, pos);
        fds[i].right = s.substr(pos + 2);
    }
    vector<pair<int, vector<int>>> ans;
    for (int i = 0; i < n; ++i) {
        set<char> left = parse(fds[i].left);
        set<char> right = parse(fds[i].right);
        vector<int> seq = can_derive(left, right, fds[i].id);
        if (!seq.empty()) {
            ans.push_back({fds[i].id, seq});
        }
    }
    if (ans.empty()) {
        cout << "No redundant FDs" << endl;
        return 0;
    }
    for (auto& p : ans) {
        cout << "FD " << p.first << " is redundant using FDs:";
        for (int x : p.second) cout << " " << x;
        cout << endl;
    }
    return 0;
}