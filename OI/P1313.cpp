#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
const int MOD=10007;
inline int qpow(int base,int exp){
    int res=1;
    while(exp){
        if(exp&1)res=res*base%MOD;
        base=base*base%MOD;
        exp>>=1;
    }
    return res;
}
int a,b,k,n,m,C[1005][1005];
int main(){
    cin>>a>>b>>k>>n>>m;
    for(int i=0;i<=k;i++){
        C[i][0]=C[i][i]=1;
        for(int j=1;j<i;j++)
            C[i][j]=(C[i-1][j-1]+C[i-1][j])%MOD;
    }
    int ans=C[k][n]*qpow(a,n)%MOD*qpow(b,m)%MOD;
    cout<<ans<<endl;
    return 0;
}