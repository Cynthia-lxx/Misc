#include<bits/stdc++.h>

using std::string;
using std::getline;
using std::cin;
using std::cout;
using std::cerr;
using std::isalpha;
using std::tolower;
using std::max;
using std::min;

inline bool isLetter(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

int main() {
    string s, raw;
    string line;
    while (getline(cin, line)) {
        raw += line + '\n';
    }
    if (!raw.empty() && raw.back() == '\n') raw.pop_back(); // 去掉末尾多余换行喵
    
    int n = raw.size();
    int m = 0;
    int pos[20005]; // 映射原始位置喵
    for (int i = 0; i < n; ++i) {
        if (isLetter(raw[i])) {
            s += tolower(raw[i]);
            pos[m++] = i;
        }
    }
    
    int ans = 1, start = 0; // ans:长度, start:在s中的起始位置喵
    // 奇数长度回文
    for (int i = 0; i < m; ++i) {
        int l = i - 1, r = i + 1;
        while (l >= 0 && r < m && s[l] == s[r]) {
            --l; ++r;
        }
        int len = r - l - 1;
        if (len > ans) {
            ans = len;
            start = l + 1;
        }
    }
    // 偶数长度回文
    for (int i = 0; i < m - 1; ++i) {
        if (s[i] != s[i+1]) continue;
        int l = i, r = i + 1;
        while (l >= 0 && r < m && s[l] == s[r]) {
            --l; ++r;
        }
        int len = r - l - 1;
        if (len > ans) {
            ans = len;
            start = l + 1;
        }
    }
    
    cout << ans << '\n';
    int left = pos[start], right = pos[start + ans - 1];
    cout << raw.substr(left, right - left + 1) << '\n';
    
    return 0;
}