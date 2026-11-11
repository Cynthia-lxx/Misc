#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::sort;
using std::vector;

struct Edge {
    int u,v,w;
    bool operator<(const Edge& o) const {
        return w<o.w;
    }
};

int fa[1005];
int find(int x) {
    return fa[x]==x?x:fa[x]=find(fa[x]);
}
inline void unite(int x,int y) {
    fa[find(x)]=find(y);
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int n,m,k;
    cin>>n>>m>>k;
    vector<Edge> e(m);
    for(int i=0;i<m;++i) {
        cin>>e[i].u>>e[i].v>>e[i].w;
    }
    sort(e.begin(),e.end());
    for(int i=1;i<=n;++i) fa[i]=i;
    int cnt=n,ans=0;
    for(auto& ed:e) {
        if(cnt==k) break;
        int fu=find(ed.u),fv=find(ed.v);
        if(fu!=fv) {
            fa[fu]=fv;
            ans+=ed.w;
            --cnt;
        }
    }
    if(cnt!=k) cout<<"No Answer"<<endl;
    else cout<<ans<<endl;
    return 0;
}