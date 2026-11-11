#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::pair;
using std::make_pair;
using std::sort;
using std::vector;

int n,t;
struct Ant {
    int pos,dir,id;
} a[100005];
int ans[100005][2];

int main() {
    cin>>n>>t;
    for(int i=0;i<n;++i) {
        cin>>a[i].pos>>a[i].dir;
        a[i].id=i;
    }
    // 关键性质：蚂蚁相遇掉头等价于互相穿过，但蚂蚁的编号会交换
    // 所以最终位置就是每个蚂蚁沿原方向走t步后的位置集合
    // 而朝向由最终位置和初始顺序决定
    vector<pair<int,int>> final_pos; // 位置, 方向
    for(int i=0;i<n;++i) {
        int np = a[i].pos + a[i].dir * t;
        final_pos.push_back(make_pair(np, a[i].dir));
    }
    sort(final_pos.begin(), final_pos.end());
    // 按初始位置排序
    sort(a, a+n, [](const Ant &x, const Ant &y) {
        return x.pos < y.pos;
    });
    for(int i=0;i<n;++i) {
        int id = a[i].id;
        ans[id][0] = final_pos[i].first;
        ans[id][1] = final_pos[i].second;
    }
    // 处理相遇瞬间朝向为0的情况
    for(int i=0;i<n-1;++i) {
        if(ans[i][0] == ans[i+1][0]) {
            ans[i][1] = 0;
            ans[i+1][1] = 0;
        }
    }
    for(int i=0;i<n;++i) {
        cout<<ans[i][0]<<" "<<ans[i][1]<<"\n";
    }
    return 0;
}