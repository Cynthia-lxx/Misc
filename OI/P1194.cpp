#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::pair;
using std::make_pair;
using std::sort;

int A,B,fa[505],ans;
vector<pair<int,pair<int,int>>> e; // 边集

inline int find(int x) {
    return fa[x]==x?x:fa[x]=find(fa[x]);
}

int main() {
    cin>>A>>B;
    for(int i=1;i<=B;++i) fa[i]=i;
    for(int i=1;i<=B;++i) {
        for(int j=1;j<=B;++j) {
            int k; cin>>k;
            if(i<j && k) e.push_back(make_pair(k,make_pair(i,j)));
        }
    }
    // 添加虚拟节点0，连接每个物品，边权为A
    for(int i=1;i<=B;++i) e.push_back(make_pair(A,make_pair(0,i)));
    sort(e.begin(),e.end());
    for(auto &p:e) {
        int w=p.first, u=p.second.first, v=p.second.second;
        int ru=find(u), rv=find(v);
        if(ru!=rv) {
            fa[ru]=rv;
            ans+=w;
        }
    }
    cout<<ans;
    return 0;
}