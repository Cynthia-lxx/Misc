#include<bits/stdc++.h>
using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int main() {
    string s;
    vector<string> rounds;
    while (cin >> s) {
        rounds.push_back(s);
    }
    int n = rounds.size();
    vector<int> scores(n, -1);
    vector<int> cum(n, 0);
    int total = 0;
    for (int i = 0; i < n; ++i) {
        if (rounds[i] == "/") {
            // strike
            int next1 = 0, next2 = 0;
            if (i + 1 < n) {
                if (rounds[i+1] == "/") {
                    next1 = 10;
                    if (i + 2 < n) {
                        if (rounds[i+2] == "/") next2 = 10;
                        else next2 = rounds[i+2][0] - '0';
                    }
                } else {
                    if (rounds[i+1].size() == 2) {
                        next1 = rounds[i+1][0] - '0' + (rounds[i+1][1] == '/' ? 10 - (rounds[i+1][0] - '0') : rounds[i+1][1] - '0');
                    } else {
                        next1 = rounds[i+1][0] - '0';
                    }
                }
            }
            scores[i] = 10 + next1 + next2;
            total += scores[i];
            cum[i] = total;
        } else if (rounds[i].size() == 2) {
            int first = rounds[i][0] - '0';
            int second = (rounds[i][1] == '/') ? 10 - first : rounds[i][1] - '0';
            if (first + second == 10) {
                // spare
                int next = 0;
                if (i + 1 < n) {
                    if (rounds[i+1] == "/") next = 10;
                    else next = rounds[i+1][0] - '0';
                }
                scores[i] = 10 + next;
            } else {
                scores[i] = first + second;
            }
            total += scores[i];
            cum[i] = total;
        } else {
            // single digit
            int val = rounds[i][0] - '0';
            scores[i] = val;
            total += val;
            cum[i] = total;
        }
    }
    for (int i = 0; i < n; ++i) {
        if (scores[i] != -1) cout << scores[i] << " ";
    }
    cout << endl;
    for (int i = 0; i < n; ++i) {
        if (scores[i] != -1) cout << cum[i] << " ";
    }
    cout << endl;
    return 0;
}