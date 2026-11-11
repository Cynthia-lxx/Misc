#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
using std::min;
int n,m,a[100005];
bool check(int x){ //判断能否分成<=m段，每段和<=x喵
    int cnt=1,sum=0;
    for(int i=1;i<=n;++i){
        if(sum+a[i]>x){
            ++cnt;
            sum=a[i];
        }else sum+=a[i];
    }
    return cnt<=m;
}
int main(){
    cin>>n>>m;
    int l=0,r=0,ans;
    for(int i=1;i<=n;++i){
        cin>>a[i];
        l=max(l,a[i]);
        r+=a[i];
    }
    while(l<=r){
        int mid=(l+r)>>1;
        if(check(mid)){
            ans=mid;
            r=mid-1;
        }else l=mid+1;
    }
    cout<<ans;
    return 0;
}