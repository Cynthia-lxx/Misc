#include<bits/stdc++.h>
using std::set;
using std::cin;
using std::cout;
using std::endl;

int a,b,c;
bool vis[21][21][21];
set<int> ans;

inline void dfs(int x,int y,int z){
    if(vis[x][y][z]) return;
    vis[x][y][z]=true;
    if(x==0) ans.insert(z);
    // x->y
    int pour=std::min(x,b-y);
    dfs(x-pour,y+pour,z);
    // x->z
    pour=std::min(x,c-z);
    dfs(x-pour,y,z+pour);
    // y->x
    pour=std::min(y,a-x);
    dfs(x+pour,y-pour,z);
    // y->z
    pour=std::min(y,c-z);
    dfs(x,y-pour,z+pour);
    // z->x
    pour=std::min(z,a-x);
    dfs(x+pour,y,z-pour);
    // z->y
    pour=std::min(z,b-y);
    dfs(x,y+pour,z-pour);
}

int main(){
    cin>>a>>b>>c;
    dfs(0,0,c);
    for(auto it=ans.begin();it!=ans.end();++it){
        if(it!=ans.begin()) cout<<' ';
        cout<<*it;
    }
    cout<<endl;
    return 0;
}