#include<bits/stdc++.h>

using std::cin;
using std::cout;
using std::string;
using std::vector;

int R,C,N;
vector<string> mp;
int sx,sy;
int dx[4]={-1,1,0,0};
int dy[4]={0,0,-1,1};
int dir[1005];
bool vis[55][55][1005];

int getDir(const string& s){
    if(s=="NORTH") return 0;
    if(s=="SOUTH") return 1;
    if(s=="WEST") return 2;
    return 3;
}

int main(){
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    cin>>R>>C;
    mp.resize(R);
    for(int i=0;i<R;i++){
        cin>>mp[i];
        for(int j=0;j<C;j++){
            if(mp[i][j]=='*'){
                sx=i; sy=j;
                mp[i][j]='.';
            }
        }
    }
    
    cin>>N;
    string s;
    for(int i=0;i<N;i++){
        cin>>s;
        dir[i]=getDir(s);
    }
    
    vector<std::pair<int,int>> cur;
    cur.push_back({sx,sy});
    vis[sx][sy][0]=true;
    
    for(int step=0;step<N;step++){
        vector<std::pair<int,int>> nxt;
        int d=dir[step];
        for(auto p:cur){
            int x=p.first, y=p.second;
            int nx=x+dx[d], ny=y+dy[d];
            while(nx>=0 && nx<R && ny>=0 && ny<C && mp[nx][ny]=='.'){
                if(!vis[nx][ny][step+1]){
                    vis[nx][ny][step+1]=true;
                    nxt.push_back({nx,ny});
                }
                nx+=dx[d]; ny+=dy[d];
            }
        }
        cur.swap(nxt);
    }
    
    for(auto p:cur){
        mp[p.first][p.second]='*';
    }
    
    for(int i=0;i<R;i++){
        cout<<mp[i]<<'\n';
    }
    
    return 0;
}