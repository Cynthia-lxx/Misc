#include<bits/stdc++.h>
using std::pair;
using std::make_pair;
using std::sort;
using std::max;
using std::cin;
using std::cout;

int n,ans;
pair<int,int> a[5005];
int dp[5005];

int main(){
    cin>>n;
    for(int i=1;i<=n;++i){
        cin>>a[i].first>>a[i].second;
    }
    sort(a+1,a+n+1,[](pair<int,int> x,pair<int,int> y){
        if(x.first!=y.first)return x.first>y.first;
        return x.second>y.second;
    });
    for(int i=1;i<=n;++i){
        dp[i]=1;
        for(int j=1;j<i;++j){
            if(a[j].second<a[i].second){
                dp[i]=max(dp[i],dp[j]+1);
            }
        }
        ans=max(ans,dp[i]);
    }
    cout<<ans;
    return 0;
}