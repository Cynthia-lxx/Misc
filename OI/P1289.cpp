#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;
using std::pair;
using std::make_pair;
using std::min;
using std::max;
using std::swap;

int n,k,pos[100005],goal[100005],s[100005];
bool vis[100005];
vector<int> file[100005];

int main(){
    cin>>n>>k;
    int tot=0;
    for(int i=1;i<=k;++i){
        cin>>s[i];
        file[i].resize(s[i]);
        for(int j=0;j<s[i];++j){
            cin>>file[i][j];
            pos[file[i][j]]=i;
        }
        for(int j=0;j<s[i];++j){
            goal[++tot]=i;
        }
    }
    int ans=0;
    for(int i=1;i<=n;++i){
        if(pos[i]==goal[i])vis[i]=1;
    }
    for(int i=1;i<=n;++i){
        if(vis[i])continue;
        if(pos[i]==0)continue;
        int cur=i,len=0;
        while(!vis[cur]){
            vis[cur]=1;
            ++len;
            cur=goal[cur];
            while(cur&&pos[cur]!=goal[cur])cur=goal[cur];
        }
        if(len>0)ans+=len+1;
    }
    int empty=0;
    for(int i=1;i<=n;++i)if(pos[i]==0)empty=1;
    if(!empty)ans=-1;
    if(ans==0)cout<<"No optimization needed."<<endl;
    else cout<<"We need "<<ans<<" move operations."<<endl;
    return 0;
}