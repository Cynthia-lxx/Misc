#include<bits/stdc++.h>
using std::string;
using std::cin;
using std::cout;
using std::endl;

string pre,post;
long long ans=1;

inline void solve(int l1,int r1,int l2,int r2) {
    if(l1>=r1) return;
    int pos=pre.find(post[r2-1]);
    int cnt=0;
    for(int i=l1;i<=r1;i++) {
        if(pre[i]==post[r2-1]) {
            cnt++;
            pos=i;
        }
    }
    if(cnt>1) {
        ans*=2;
    }
    int len=pos-l1;
    solve(l1,pos-1,l2,l2+len-1);
    solve(pos+1,r1,l2+len,r2-1);
}

int main() {
    cin>>pre>>post;
    int n=pre.size();
    solve(0,n-1,0,n-1);
    cout<<ans<<endl;
    return 0;
}