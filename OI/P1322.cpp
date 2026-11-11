#include<bits/stdc++.h>
using std::string;
using std::cin;
using std::getline;
using std::stoi;
using std::abs;

string s;
int pos, len;

int parse() {
    int res = 0;
    while (pos < len && s[pos] != ']') {
        if (s[pos] == 'F' && s[pos+1] == 'D') {
            pos += 3; // skip "FD "
            int num = 0;
            while (pos < len && isdigit(s[pos])) {
                num = num * 10 + (s[pos] - '0');
                ++pos;
            }
            res += num;
        } else if (s[pos] == 'B' && s[pos+1] == 'K') {
            pos += 3; // skip "BK "
            int num = 0;
            while (pos < len && isdigit(s[pos])) {
                num = num * 10 + (s[pos] - '0');
                ++pos;
            }
            res -= num;
        } else if (s[pos] == 'R') {
            pos += 7; // skip "REPEAT "
            int times = 0;
            while (pos < len && isdigit(s[pos])) {
                times = times * 10 + (s[pos] - '0');
                ++pos;
            }
            ++pos; // skip '['
            int inner = parse();
            ++pos; // skip ']'
            res += times * inner;
        } else {
            ++pos; // skip spaces or other
        }
    }
    return res;
}

int main() {
    getline(cin, s);
    len = s.size();
    pos = 0;
    int ans = parse();
    std::cout << abs(ans) << "\n";
    return 0;
}