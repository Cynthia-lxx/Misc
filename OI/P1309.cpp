#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::sort;
using std::merge;
struct Player {
    int id, score, power;
} a[200005], win[100005], lose[100005];
int n,r,q;
bool cmp(const Player &x, const Player &y) {
    if(x.score != y.score) return x.score > y.score;
    return x.id < y.id;
}
int main() {
    cin >> n >> r >> q;
    int m = 2*n;
    for(int i=1;i<=m;i++) { cin >> a[i].score; a[i].id = i; }
    for(int i=1;i<=m;i++) cin >> a[i].power;
    sort(a+1, a+m+1, cmp);
    for(int _=0;_<r;_++) {
        for(int i=1;i<=n;i++) {
            int p1 = 2*i-1, p2 = 2*i;
            if(a[p1].power > a[p2].power) {
                a[p1].score++;
                win[i] = a[p1];
                lose[i] = a[p2];
            } else {
                a[p2].score++;
                win[i] = a[p2];
                lose[i] = a[p1];
            }
        }
        merge(win+1, win+n+1, lose+1, lose+n+1, a+1, cmp);
    }
    cout << a[q].id << "\n";
    return 0;
}