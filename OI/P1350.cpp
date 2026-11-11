#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
using std::min;
const int MOD=100003;
int a,b,c,d,k,ans;
int C[2005][2005],fac[2005],inv[2005];
inline int qpow(int a,int b){
    int r=1;
    while(b){
        if(b&1)r=1ll*r*a%MOD;
        a=1ll*a*a%MOD;b>>=1;
    }
    return r;
}
int main(){
    cin>>a>>b>>c>>d>>k;
    int n=max(a+c,b+d)+5;
    fac[0]=1;
    for(int i=1;i<=n;++i)fac[i]=1ll*fac[i-1]*i%MOD;
    inv[n]=qpow(fac[n],MOD-2);
    for(int i=n;i;--i)inv[i-1]=1ll*inv[i]*i%MOD;
    for(int i=0;i<=n;++i)
        for(int j=0;j<=i;++j)
            C[i][j]=1ll*fac[i]*inv[j]%MOD*inv[i-j]%MOD;
    for(int i=0;i<=k;++i){
        int j=k-i;
        if(i>a||i>b||j>c||j>d)continue;
        ans=(ans+1ll*C[a][i]*C[b][i]%MOD*fac[i]%MOD*C[c][j]%MOD*C[d][j]%MOD*fac[j])%MOD;
    }
    cout<<ans<<'\n';
    return 0;
}