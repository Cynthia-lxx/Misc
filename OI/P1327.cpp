#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::sort;

int n,ans;
vector<pair<int,int>> a;

int main(){
    cin>>n;
    a.resize(n);
    for(int i=0;i<n;++i){
        cin>>a[i].first;
        a[i].second=i;
    }
    sort(a.begin(),a.end());
    vector<bool> vis(n,false);
    for(int i=0;i<n;++i){
        if(vis[i]||a[i].second==i) continue;
        int len=0,j=i;
        while(!vis[j]){
            vis[j]=true;
            j=a[j].second;
            ++len;
        }
        ans+=len-1;
    }
    cout<<ans;
    return 0;
}