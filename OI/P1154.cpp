#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
int n,a[5005];
bool vis[1000005];
int main(){
    cin>>n;
    int mx=0;
    for(int i=1;i<=n;++i){
        cin>>a[i];
        mx=max(mx,a[i]);
    }
    for(int k=n;k<=mx+1;++k){
        bool ok=true;
        for(int i=1;i<=n&&ok;++i){
            int r=a[i]%k;
            if(vis[r]) ok=false;
            else vis[r]=true;
        }
        for(int i=1;i<=n;++i) vis[a[i]%k]=false;
        if(ok){
            cout<<k;
            return 0;
        }
    }
    return 0;
}