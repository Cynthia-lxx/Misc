#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::pair;

const int MOD = 20123;

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    cin >> N >> M;

    vector<vector<pair<int,int>>> rooms(N, vector<pair<int,int>>(M));
    vector<int> stairCount(N, 0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < M; ++j) {
            int hasStair, num;
            cin >> hasStair >> num;
            rooms[i][j] = {hasStair, num};
            if (hasStair) stairCount[i]++;
        }
    }

    int start;
    cin >> start;

    int ans = 0;
    int cur = start;

    for (int i = 0; i < N; ++i) {
        int num = rooms[i][cur].second;
        ans = (ans + num) % MOD;

        int need = (num - 1) % stairCount[i] + 1;
        int cnt = 0;
        while (true) {
            if (rooms[i][cur].first) {
                cnt++;
                if (cnt == need) break;
            }
            cur = (cur + 1) % M;
        }
    }

    cout << ans << "\n";
    return 0;
}