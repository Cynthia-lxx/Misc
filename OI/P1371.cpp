#include<bits/stdc++.h>
using std::string;
using std::cin;
using std::cout;
using std::max;
using std::vector;

int main(){
    int n;
    cin >> n;
    string s;
    cin >> s;
    vector<long long> preN(n+1,0), preNO(n+1,0), sufI(n+1,0), sufOI(n+1,0);
    for(int i=0;i<n;i++){
        preN[i+1]=preN[i]+(s[i]=='N');
        preNO[i+1]=preNO[i]+(s[i]=='O'?preN[i]:0);
    }
    for(int i=n-1;i>=0;i--){
        sufI[i]=sufI[i+1]+(s[i]=='I');
        sufOI[i]=sufOI[i+1]+(s[i]=='O'?sufI[i+1]:0);
    }
    long long base=0;
    for(int i=0;i<n;i++){
        if(s[i]=='O') base+=preN[i]*sufI[i+1];
    }
    long long ans=base;
    for(int i=0;i<=n;i++){
        ans=max(ans,base+sufOI[i]);
        ans=max(ans,base+preN[i]*sufI[i]);
        ans=max(ans,base+preNO[i]);
    }
    cout << ans << "\n";
    return 0;
}