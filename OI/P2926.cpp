#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
int n,mx,a[100005],cnt[1000005],ans[1000005];
int main(){
    cin>>n;
    for(int i=1;i<=n;++i){
        cin>>a[i];
        ++cnt[a[i]];
        mx=max(mx,a[i]);
    }
    for(int i=1;i<=mx;++i){
        if(!cnt[i]) continue;
        for(int j=i;j<=mx;j+=i){
            ans[j]+=cnt[i];
        }
    }
    for(int i=1;i<=n;++i){
        cout<<ans[a[i]]-1<<"\n";
    }
    return 0;
}