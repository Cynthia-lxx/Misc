#include<bits/stdc++.h>
using std::vector;
using std::pair;
using std::min;
using std::cerr;

int n,m,ans;
vector<int> l[10005];
int col[10005]; // 0未染色 1黑色 2白色喵
int cnt[3]; // 统计两种颜色数量喵

bool dfs(int u,int c){
    col[u]=c;
    cnt[c]++;
    for(int v:l[u]){
        if(!col[v]){
            if(!dfs(v,3-c)) return false;
        }else if(col[v]==c){
            return false;
        }
    }
    return true;
}

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cin>>n>>m;
    for(int i=1;i<=m;i++){
        int u,v;
        std::cin>>u>>v;
        l[u].push_back(v);
        l[v].push_back(u);
    }
    for(int i=1;i<=n;i++){
        if(!col[i]){
            cnt[1]=cnt[2]=0;
            if(!dfs(i,1)){
                std::cout<<"Impossible\n";
                return 0;
            }
            ans+=min(cnt[1],cnt[2]);
        }
    }
    std::cout<<ans<<"\n";
    return 0;
}