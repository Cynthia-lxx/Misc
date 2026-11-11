#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::vector;

int n,m,k,p,x;
int l[100005],r[100005];
bool del[100005];

int main(){
    cin>>n;
    r[0]=1; l[1]=0; r[1]=0; // 0作为虚拟头节点喵
    for(int i=2;i<=n;i++){
        cin>>k>>p;
        if(p==0){
            // 插入到k左边
            l[i]=l[k];
            r[i]=k;
            r[l[k]]=i;
            l[k]=i;
        }else{
            // 插入到k右边
            r[i]=r[k];
            l[i]=k;
            l[r[k]]=i;
            r[k]=i;
        }
    }
    cin>>m;
    for(int i=0;i<m;i++){
        cin>>x;
        del[x]=true;
    }
    int cur=r[0];
    bool first=true;
    while(cur){
        if(!del[cur]){
            if(!first) cout<<' ';
            cout<<cur;
            first=false;
        }
        cur=r[cur];
    }
    cout<<endl;
    return 0;
}