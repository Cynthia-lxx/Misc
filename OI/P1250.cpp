#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
using std::min;
using std::sort;

int n,h,b[5005],e[5005],t[5005],l[30005];
bool used[30005];

int main(){
    cin>>n>>h;
    for(int i=1;i<=h;i++){
        cin>>b[i]>>e[i]>>t[i];
    }
    //∞¥”“∂Àµ„≈≈–Úﬂ˜
    for(int i=1;i<=h;i++){
        for(int j=i+1;j<=h;j++){
            if(e[i]>e[j]){
                std::swap(b[i],b[j]);
                std::swap(e[i],e[j]);
                std::swap(t[i],t[j]);
            }
        }
    }
    for(int i=1;i<=h;i++){
        int cnt=0;
        for(int j=b[i];j<=e[i];j++){
            if(used[j]) cnt++;
        }
        for(int j=e[i];j>=b[i] && cnt<t[i];j--){
            if(!used[j]){
                used[j]=true;
                cnt++;
            }
        }
    }
    int ans=0;
    for(int i=1;i<=n;i++){
        if(used[i]) ans++;
    }
    cout<<ans;
    return 0;
}