#include<bits/stdc++.h>

using std::vector;
using std::cerr;
using std::cin;
using std::cout;

int n;
vector<int> l; // 分解方案喵
int a[1e6+1145]; // 高精度存储喵

inline void mul(int x) {
    int carry = 0;
    for (int i = 1; i <= a[0]; ++i) {
        a[i] = a[i] * x + carry;
        carry = a[i] / 10;
        a[i] %= 10;
    }
    while (carry) {
        a[++a[0]] = carry % 10;
        carry /= 10;
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    cin >> n;
    
    // 贪心：从2开始取连续自然数，直到和超过n
    int sum = 0;
    for (int i = 2; sum + i <= n; ++i) {
        l.push_back(i);
        sum += i;
    }
    
    int rem = n - sum; // 剩余部分喵
    
    // 从后向前分配剩余部分
    for (int i = (int)l.size() - 1; i >= 0 && rem > 0; --i) {
        l[i]++;
        rem--;
    }
    
    // 如果还有剩余，从最后一个开始加
    if (rem > 0) {
        for (int i = (int)l.size() - 1; i >= 0 && rem > 0; --i) {
            l[i]++;
            rem--;
        }
    }
    
    // 输出分解方案
    for (int i = 0; i < (int)l.size(); ++i) {
        if (i) cout << ' ';
        cout << l[i];
    }
    cout << '\n';
    
    // 高精度计算乘积
    a[0] = a[1] = 1;
    for (int x : l) mul(x);
    
    // 输出乘积
    for (int i = a[0]; i >= 1; --i) cout << a[i];
    cout << '\n';
    
    return 0;
}