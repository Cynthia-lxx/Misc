#include<bits/stdc++.h>
using std::vector;
using std::pair;
using std::cin;
using std::cout;
using std::endl;

int m,n,sx,sy,ex,ey;
int a[20][20];
bool vis[20][20];
vector<pair<int,int>> path;
bool found=false;

int dx[4]={0,-1,0,1}; // вСиосроб
int dy[4]={-1,0,1,0};

void dfs(int x,int y){
    if(x==ex && y==ey){
        found=true;
        for(int i=0;i<(int)path.size();++i){
            if(i) cout<<"->";
            cout<<"("<<path[i].first<<","<<path[i].second<<")";
        }
        cout<<endl;
        return;
    }
    for(int i=0;i<4;++i){
        int nx=x+dx[i],ny=y+dy[i];
        if(nx>=1 && nx<=m && ny>=1 && ny<=n && a[nx][ny]==1 && !vis[nx][ny]){
            vis[nx][ny]=true;
            path.push_back({nx,ny});
            dfs(nx,ny);
            path.pop_back();
            vis[nx][ny]=false;
        }
    }
}

int main(){
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    cin>>m>>n;
    for(int i=1;i<=m;++i)
        for(int j=1;j<=n;++j)
            cin>>a[i][j];
    cin>>sx>>sy>>ex>>ey;
    vis[sx][sy]=true;
    path.push_back({sx,sy});
    dfs(sx,sy);
    if(!found) cout<<-1<<endl;
    return 0;
}