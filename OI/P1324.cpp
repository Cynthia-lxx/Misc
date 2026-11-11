#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::sort;
using std::greater;
using std::max;
using std::min;
using std::vector;
using std::pair;
using std::make_pair;
using std::priority_queue;

int n,m;
long long ans;
vector<int> h,v;

int main(){
    cin>>n>>m;
    h.resize(n-1);
    v.resize(m-1);
    for(int i=0;i<n-1;++i) cin>>h[i];
    for(int i=0;i<m-1;++i) cin>>v[i];
    sort(h.begin(),h.end(),greater<int>());
    sort(v.begin(),v.end(),greater<int>());
    int i=0,j=0;
    int hcnt=1,vcnt=1;
    while(i<n-1 && j<m-1){
        if(h[i]>=v[j]){
            ans+=1LL*h[i]*vcnt;
            ++i;
            ++hcnt;
        }else{
            ans+=1LL*v[j]*hcnt;
            ++j;
            ++vcnt;
        }
    }
    while(i<n-1){
        ans+=1LL*h[i]*vcnt;
        ++i;
    }
    while(j<m-1){
        ans+=1LL*v[j]*hcnt;
        ++j;
    }
    cout<<ans;
    return 0;
}