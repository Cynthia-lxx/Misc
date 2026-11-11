#include<bits/stdc++.h>
using std::pair;
using std::make_pair;
using std::sort;
using std::max;
using std::min;
using std::cin;
using std::cout;

int xa,ya,xb,yb,n;
pair<int,int> d[100005]; // 第一关键字：到系统1距离平方，第二关键字：到系统2距离平方喵

inline int sq(int x){return x*x;}
inline int dist2(int x1,int y1,int x2,int y2){return sq(x1-x2)+sq(y1-y2);}

int main(){
    cin>>xa>>ya>>xb>>yb>>n;
    for(int i=0;i<n;++i){
        int x,y;cin>>x>>y;
        d[i]=make_pair(dist2(xa,ya,x,y),dist2(xb,yb,x,y));
    }
    sort(d,d+n);
    int ans=1e9+7, mr2=0; // mr2: 当前已考虑导弹中到系统2的最大距离平方喵
    for(int i=n-1;i>=0;--i){
        ans=min(ans,d[i].first+mr2);
        mr2=max(mr2,d[i].second);
    }
    ans=min(ans,mr2); // 全部由系统2拦截喵
    cout<<ans;
    return 0;
}