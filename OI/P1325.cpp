#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::pair;
using std::make_pair;
using std::sort;
using std::sqrt;
using std::abs;

int n,d,ans;
pair<double,double> seg[1005]; // 每个岛屿对应的雷达区间喵

int main() {
    cin>>n>>d;
    bool ok=true;
    for(int i=0;i<n;i++) {
        int x,y;
        cin>>x>>y;
        if(y>d) { ok=false; break; }
        double dx=sqrt(1.0*d*d-1.0*y*y);
        seg[i]=make_pair(x-dx, x+dx);
    }
    if(!ok) { cout<<-1<<endl; return 0; }
    sort(seg, seg+n);
    double cur=-1e18;
    for(int i=0;i<n;i++) {
        if(seg[i].first > cur+1e-9) {
            ans++;
            cur=seg[i].second;
        } else {
            if(seg[i].second < cur) cur=seg[i].second;
        }
    }
    cout<<ans<<endl;
    return 0;
}