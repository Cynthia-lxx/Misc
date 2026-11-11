#include<bits/stdc++.h>
using std::string;
using std::cin;
using std::cout;
using std::min;
using std::abs;

inline int min3(int a,int b,int c){
    return min(a,min(b,c));
}

int dp[2005][2005];
char a[2005],b[2005];
int main(){
    string s1,s2;
    int K;
    cin>>s1>>s2>>K;
    int n=s1.size(),m=s2.size();
    for(int i=1;i<=n;++i) a[i]=s1[i-1];
    for(int i=1;i<=m;++i) b[i]=s2[i-1];
    for(int i=1;i<=n;++i) dp[i][0]=dp[i-1][0]+K;
    for(int j=1;j<=m;++j) dp[0][j]=dp[0][j-1]+K;
    for(int i=1;i<=n;++i){
        for(int j=1;j<=m;++j){
            dp[i][j]=min3(
                dp[i-1][j]+K,
                dp[i][j-1]+K,
                dp[i-1][j-1]+abs(a[i]-b[j])
            );
        }
    }
    cout<<dp[n][m];
    return 0;
}