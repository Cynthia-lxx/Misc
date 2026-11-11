#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::max;
using std::min;

int n,k,p,ans;
int a[200005],b[200005];
int last[55],cnt[55],sum[55];

int main() {
    cin>>n>>k>>p;
    for(int i=1;i<=n;++i) cin>>a[i]>>b[i];
    for(int i=1;i<=n;++i) {
        if(b[i]<=p) {
            for(int j=0;j<k;++j) sum[j]=cnt[j];
        }
        ans+=sum[a[i]];
        cnt[a[i]]++;
        if(b[i]<=p) sum[a[i]]=cnt[a[i]];
    }
    cout<<ans<<endl;
    return 0;
}