#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
using std::min;
using std::endl;

int m,k,a[505],dp[505][505],sum[505];

inline void print(int i,int j,int mx) {
    if(j==0) return;
    if(j==1) {
        cout<<1<<" "<<i<<endl;
        return;
    }
    int _=i;
    while(_>0 && sum[i]-sum[_-1]<=mx) _--;
    print(_,j-1,mx);
    cout<<_+1<<" "<<i<<endl;
}

int main() {
    cin>>m>>k;
    for(int i=1;i<=m;i++) {
        cin>>a[i];
        sum[i]=sum[i-1]+a[i];
    }
    memset(dp,0x3f,sizeof(dp));
    dp[0][0]=0;
    for(int i=1;i<=m;i++) {
        for(int j=1;j<=min(i,k);j++) {
            for(int p=j-1;p<i;p++) {
                dp[i][j]=min(dp[i][j],max(dp[p][j-1],sum[i]-sum[p]));
            }
        }
    }
    print(m,k,dp[m][k]);
    return 0;
}