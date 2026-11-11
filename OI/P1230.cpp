#include<bits/stdc++.h>
using std::pair;
using std::make_pair;
using std::sort;
using std::greater;
using std::priority_queue;
using std::max;

int m,n,ans;
pair<int,int> a[505];
priority_queue<int,std::vector<int>,greater<int>> pq;

inline void solve(){
    sort(a+1,a+n+1);
    for(int i=1;i<=n;++i){
        if(pq.size()<a[i].first){
            pq.push(a[i].second);
            ans+=a[i].second;
        }else if(pq.top()<a[i].second){
            ans-=pq.top(); pq.pop();
            ans+=a[i].second; pq.push(a[i].second);
        }
    }
    ans=m-ans;
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cin>>m>>n;
    for(int i=1;i<=n;++i) std::cin>>a[i].first;
    for(int i=1;i<=n;++i) std::cin>>a[i].second;
    solve();
    std::cout<<ans<<'\n';
    return 0;
}