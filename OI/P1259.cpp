#include<bits/stdc++.h>
using std::cout;
using std::cin;
using std::endl;
using std::string;

int n;
string s;

inline void print() {
    cout << s << endl;
}

int main() {
    cin >> n;
    int len = 2 * n + 2;
    s = string(n, 'o') + string(n, '*') + "--";
    print();
    int space = 2 * n; // 空位起始位置（第一个'-'的索引）
    for (int i = n; i > 1; --i) {
        // 移动 i-1 和 i 到空位
        s[space] = s[i-1];
        s[space+1] = s[i];
        s[i-1] = '-';
        s[i] = '-';
        space = i-1;
        print();
        // 移动 i+1 和 i+2 到空位（如果还有）
        if (i > 2) {
            s[space] = s[2*i-2];
            s[space+1] = s[2*i-1];
            s[2*i-2] = '-';
            s[2*i-1] = '-';
            space = 2*i-2;
            print();
        }
    }
    // 最后一步特殊处理
    s[space] = s[0];
    s[space+1] = s[1];
    s[0] = '-';
    s[1] = '-';
    print();
    return 0;
}