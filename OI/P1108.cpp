#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;

int n, a[5005], dp[5005], cnt[5005]; // dp为最长下降子序列长度，cnt为方案数喵

int main(){
    cin>>n;
    for(int i=1;i<=n;++i) cin>>a[i];
    for(int i=1;i<=n;++i){
        dp[i]=1;
        for(int j=1;j<i;++j) if(a[j]>a[i]) dp[i]=std::max(dp[i], dp[j]+1);
    }
    for(int i=1;i<=n;++i){
        if(dp[i]==1) cnt[i]=1;
        for(int j=1;j<i;++j){
            if(a[j]>a[i] && dp[j]+1==dp[i]) cnt[i]+=cnt[j];
            else if(a[j]==a[i] && dp[j]==dp[i]) cnt[j]=0; // 去重喵
        }
    }
    int mx=0;
    for(int i=1;i<=n;++i) mx=std::max(mx, dp[i]);
    int ans=0;
    for(int i=1;i<=n;++i) if(dp[i]==mx) ans+=cnt[i];
    cout<<mx<<" "<<ans<<endl;
    return 0;
}
