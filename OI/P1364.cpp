#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::min;
using std::abs;
int n,w[105],l[105],r[105],dist[105][105];
int main(){
    cin>>n;
    for(int i=1;i<=n;i++){
        cin>>w[i]>>l[i]>>r[i];
        if(l[i]) dist[i][l[i]]=dist[l[i]][i]=1;
        if(r[i]) dist[i][r[i]]=dist[r[i]][i]=1;
    }
    for(int i=1;i<=n;i++)
        for(int j=1;j<=n;j++)
            if(i!=j&&!dist[i][j]) dist[i][j]=1e9;
    for(int k=1;k<=n;k++)
        for(int i=1;i<=n;i++)
            for(int j=1;j<=n;j++)
                if(dist[i][k]+dist[k][j]<dist[i][j])
                    dist[i][j]=dist[i][k]+dist[k][j];
    int ans=1e9;
    for(int i=1;i<=n;i++){
        int sum=0;
        for(int j=1;j<=n;j++) sum+=w[j]*dist[i][j];
        ans=min(ans,sum);
    }
    cout<<ans;
    return 0;
}