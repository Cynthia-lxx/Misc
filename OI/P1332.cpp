#include<bits/stdc++.h>
using std::queue;
using std::pair;
using std::make_pair;
using std::cin;
using std::cout;
using std::ios;

int n,m,a,b,dist[505][505];
int dx[4]={-1,1,0,0},dy[4]={0,0,-1,1};
queue<pair<int,int>> q;

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin>>n>>m>>a>>b;
    memset(dist,-1,sizeof(dist));
    for(int i=0;i<a;i++){
        int x,y;cin>>x>>y;
        dist[x][y]=0;
        q.push(make_pair(x,y));
    }
    while(!q.empty()){
        auto [x,y]=q.front();q.pop();
        for(int k=0;k<4;k++){
            int nx=x+dx[k],ny=y+dy[k];
            if(nx>=1&&nx<=n&&ny>=1&&ny<=m&&dist[nx][ny]==-1){
                dist[nx][ny]=dist[x][y]+1;
                q.push(make_pair(nx,ny));
            }
        }
    }
    for(int i=0;i<b;i++){
        int x,y;cin>>x>>y;
        cout<<dist[x][y]<<'\n';
    }
    return 0;
}