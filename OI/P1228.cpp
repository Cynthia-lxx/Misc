#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int k,x,y;

void solve(int x1,int y1,int x2,int y2,int spx,int spy) {
    // x1,y1 左上角, x2,y2 右下角, spx,spy 特殊点坐标
    if(x1==x2 && y1==y2) return;
    int mx=(x1+x2)/2, my=(y1+y2)/2;
    // 确定特殊点所在象限
    int q;
    if(spx<=mx && spy<=my) q=1;
    else if(spx<=mx && spy>my) q=2;
    else if(spx>mx && spy<=my) q=3;
    else q=4;
    // 根据特殊点位置放置地毯
    if(q==1) {
        cout << mx+1 << " " << my+1 << " 1" << endl;
        solve(x1,y1,mx,my,spx,spy);
        solve(x1,my+1,mx,y2,mx,my+1);
        solve(mx+1,y1,x2,my,mx+1,my);
        solve(mx+1,my+1,x2,y2,mx+1,my+1);
    } else if(q==2) {
        cout << mx+1 << " " << my << " 2" << endl;
        solve(x1,y1,mx,my,mx,my);
        solve(x1,my+1,mx,y2,spx,spy);
        solve(mx+1,y1,x2,my,mx+1,my);
        solve(mx+1,my+1,x2,y2,mx+1,my+1);
    } else if(q==3) {
        cout << mx << " " << my+1 << " 3" << endl;
        solve(x1,y1,mx,my,mx,my);
        solve(x1,my+1,mx,y2,mx,my+1);
        solve(mx+1,y1,x2,my,spx,spy);
        solve(mx+1,my+1,x2,y2,mx+1,my+1);
    } else {
        cout << mx << " " << my << " 4" << endl;
        solve(x1,y1,mx,my,mx,my);
        solve(x1,my+1,mx,y2,mx,my+1);
        solve(mx+1,y1,x2,my,mx+1,my);
        solve(mx+1,my+1,x2,y2,spx,spy);
    }
}

int main() {
    cin >> k >> x >> y;
    int n=1<<k;
    solve(1,1,n,n,x,y);
    return 0;
}