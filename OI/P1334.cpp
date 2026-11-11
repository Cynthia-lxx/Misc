#include<bits/stdc++.h>
using std::priority_queue;
using std::greater;
using std::vector;
using std::cin;
using std::cout;
using std::long long;

int n;
long long ans;

int main() {
    cin >> n;
    priority_queue<long long, vector<long long>, greater<long long>> pq; // Ð¡¸ù¶Ñß÷
    for (int i = 0; i < n; ++i) {
        long long x;
        cin >> x;
        pq.push(x);
    }
    while (pq.size() > 1) {
        long long a = pq.top(); pq.pop();
        long long b = pq.top(); pq.pop();
        long long s = a + b;
        ans += s;
        pq.push(s);
    }
    cout << ans << "\n";
    return 0;
}