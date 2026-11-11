#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
int n,a[505][505],ans;
int main(){
    cin>>n;
    for(int i=1;i<=n;i++)
        for(int j=i+1;j<=n;j++){
            cin>>a[i][j];
            a[j][i]=a[i][j];
        }
    for(int i=1;i<=n;i++){
        int mx1=0,mx2=0;
        for(int j=1;j<=n;j++){
            if(a[i][j]>mx1){
                mx2=mx1;
                mx1=a[i][j];
            }
            else if(a[i][j]>mx2)mx2=a[i][j];
        }
        ans=max(ans,mx2);
    }
    cout<<"1\n"<<ans;
    return 0;
}