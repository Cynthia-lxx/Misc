#include<bits/stdc++.h>
using std::queue;
using std::pair;
using std::make_pair;
using std::abs;
using std::min;
int n,m,dis[185][185];
char g[185][185];
int dx[]={0,0,1,-1},dy[]={1,-1,0,0};
queue<pair<int,int>> q;
int main(){
    scanf("%d%d",&n,&m);
    for(int i=1;i<=n;++i){
        scanf("%s",g[i]+1);
        for(int j=1;j<=m;++j){
            if(g[i][j]=='1'){
                dis[i][j]=0;
                q.push(make_pair(i,j));
            }else dis[i][j]=1e9;
        }
    }
    while(!q.empty()){
        auto [x,y]=q.front();q.pop();
        for(int k=0;k<4;++k){
            int nx=x+dx[k],ny=y+dy[k];
            if(nx<1||nx>n||ny<1||ny>m)continue;
            if(dis[nx][ny]>dis[x][y]+1){
                dis[nx][ny]=dis[x][y]+1;
                q.push(make_pair(nx,ny));
            }
        }
    }
    for(int i=1;i<=n;++i){
        for(int j=1;j<=m;++j){
            printf("%d ",dis[i][j]);
        }
        putchar('\n');
    }
    return 0;
}