#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::min;
using std::memset;
using std::pow;

int n,m;
long long a[25],b[25];
long long dp[205][25];
long long cost[25][205];

inline long long qpow(long long x,long long y) {
    long long res=1;
    while(y) {
        if(y&1) res*=x;
        x*=x;
        y>>=1;
    }
    return res;
}

int main() {
    cin>>n>>m;
    for(int i=1;i<=m;++i) {
        cin>>a[i]>>b[i];
    }
    for(int i=1;i<=m;++i) {
        for(int j=0;j<=n;++j) {
            cost[i][j]=a[i]*qpow(j,b[i]);
        }
    }
    memset(dp,0x3f,sizeof(dp));
    dp[0][0]=0;
    for(int i=1;i<=m;++i) {
        for(int j=0;j<=n;++j) {
            for(int k=0;k<=j;++k) {
                dp[j][i]=min(dp[j][i],dp[j-k][i-1]+cost[i][k]);
            }
        }
    }
    cout<<dp[n][m]<<"\n";
    return 0;
}