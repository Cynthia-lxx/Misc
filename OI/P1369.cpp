#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::cerr;
using std::max;
using std::min;
using std::sort;
using std::unique;
int n;
int x[305],y[305];
int g[105][105]; //坐标上的点数喵
int row[105][105]; //行前缀和喵
int col[105][105]; //列前缀和喵
int main(){
    cin>>n;
    for(int i=1;i<=n;i++){
        cin>>x[i]>>y[i];
        g[x[i]][y[i]]++;
    }
    for(int i=1;i<=100;i++)
        for(int j=1;j<=100;j++)
            row[i][j]=row[i][j-1]+g[i][j];
    for(int j=1;j<=100;j++)
        for(int i=1;i<=100;i++)
            col[i][j]=col[i-1][j]+g[i][j];
    int ans=0;
    for(int x1=1;x1<=100;x1++)
        for(int x2=x1;x2<=100;x2++)
            for(int y1=1;y1<=100;y1++)
                for(int y2=y1;y2<=100;y2++){
                    int s=0;
                    s+=row[y1][x2]-row[y1][x1-1];
                    if(y2!=y1) s+=row[y2][x2]-row[y2][x1-1];
                    s+=col[y2-1][x1]-col[y1][x1];
                    if(x2!=x1) s+=col[y2-1][x2]-col[y1][x2];
                    ans=max(ans,s);
                }
    cout<<ans<<"\n";
    return 0;
}