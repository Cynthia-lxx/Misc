#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::min;
using std::memset;

int n,h,p[105],c[105],dp[55005];

int main(){
    cin>>n>>h;
    for(int i=1;i<=n;i++) cin>>p[i]>>c[i];
    memset(dp,0x3f,sizeof(dp));
    dp[0]=0;
    for(int i=1;i<=n;i++)
        for(int j=p[i];j<=h+5000;j++)
            dp[j]=min(dp[j],dp[j-p[i]]+c[i]);
    int ans=0x3f3f3f3f;
    for(int i=h;i<=h+5000;i++) ans=min(ans,dp[i]);
    cout<<ans;
    return 0;
}