#include<bits/stdc++.h>
using std::max;
using std::min;
using std::string;
using std::cin;
using std::cout;

int n,K,dp[505][105][105][2];
// dp[i][j][k][0/1]: 前i个字符，用了j次交换把j变成z，k次交换把z变成j，当前字符是j(0)或z(1)时的最大jz子串数喵
string s;

int main() {
    cin>>n>>K>>s;
    s=" "+s;
    memset(dp,-0x3f,sizeof dp);
    dp[0][0][0][0]=dp[0][0][0][1]=0;
    for(int i=1;i<=n;++i) {
        for(int j=0;j<=K;++j) {
            for(int k=0;k<=K;++k) {
                // 当前字符是j
                if(s[i]=='j') {
                    // 不交换
                    dp[i][j][k][0]=max(dp[i][j][k][0],dp[i-1][j][k][0]);
                    dp[i][j][k][0]=max(dp[i][j][k][0],dp[i-1][j][k][1]);
                    // 交换成z（消耗一次j->z）
                    if(j) {
                        dp[i][j][k][1]=max(dp[i][j][k][1],dp[i-1][j-1][k][0]+1);
                        dp[i][j][k][1]=max(dp[i][j][k][1],dp[i-1][j-1][k][1]);
                    }
                } else { // s[i]=='z'
                    // 不交换
                    dp[i][j][k][1]=max(dp[i][j][k][1],dp[i-1][j][k][0]+1);
                    dp[i][j][k][1]=max(dp[i][j][k][1],dp[i-1][j][k][1]);
                    // 交换成j（消耗一次z->j）
                    if(k) {
                        dp[i][j][k][0]=max(dp[i][j][k][0],dp[i-1][j][k-1][0]);
                        dp[i][j][k][0]=max(dp[i][j][k][0],dp[i-1][j][k-1][1]);
                    }
                }
            }
        }
    }
    int ans=0;
    for(int j=0;j<=K;++j) {
        for(int k=0;k<=K;++k) {
            if(j==k) ans=max(ans,dp[n][j][k][1]);
        }
    }
    cout<<ans;
    return 0;
}