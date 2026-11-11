#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::string;

int n;
string s;

int main() {
    cin >> n >> s;
    int ans = 0;
    bool cur = false; // 当前是否处于反转状态喵
    for (int i = 0; i < n; i += 2) {
        if (s[i] != s[i+1]) {
            bool need = (s[i] == 'G'); // 需要让更赛牛在偶数位置喵
            if (need != cur) {
                ++ans;
                cur = need;
            }
        }
    }
    cout << ans << "\n";
    return 0;
}