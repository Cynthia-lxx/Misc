#include<bits/stdc++.h>
using std::string;
using std::to_string;
using std::reverse;

inline string mul2(string s) {
    int carry = 0;
    for (int i = s.size() - 1; i >= 0; --i) {
        int cur = (s[i] - '0') * 2 + carry;
        s[i] = cur % 10 + '0';
        carry = cur / 10;
    }
    if (carry) s = to_string(carry) + s;
    return s;
}

inline string sub2(string s) {
    int i = s.size() - 1;
    if (s[i] >= '2') {
        s[i] -= 2;
    } else {
        s[i] = s[i] - '2' + 10;
        int j = i - 1;
        while (s[j] == '0') {
            s[j] = '9';
            --j;
        }
        s[j] -= 1;
    }
    if (s[0] == '0') s = s.substr(1);
    return s;
}

int main() {
    int n;
    std::cin >> n;
    string ans = "2";
    for (int i = 2; i <= n; ++i) {
        ans = mul2(ans);
        ans = mul2(ans);
        ans = sub2(ans);
        ans = "2" + ans;
    }
    std::cout << ans << std::endl;
    return 0;
}