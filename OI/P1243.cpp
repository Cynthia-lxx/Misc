#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::min;

inline long long C(int n, int m) {
    if(m < 0 || m > n) return 0;
    long long res = 1;
    for(int i = 1; i <= m; ++i) {
        res = res * (n - m + i) / i;
    }
    return res;
}

int main() {
    int n; long long k;
    cin >> n >> k;
    if(k == 0) {
        cout << 0;
        return 0;
    }
    vector<int> ans;
    int cur = 1;
    while(k > 0) {
        for(int i = cur; i <= n; ++i) {
            long long cnt = C(n - i, ans.size());
            if(k > cnt) {
                k -= cnt;
            } else {
                ans.push_back(i);
                cur = i + 1;
                break;
            }
        }
        if(cur > n) break;
    }
    for(int i = 0; i < (int)ans.size(); ++i) {
        if(i) cout << ' ';
        cout << ans[i];
    }
    return 0;
}