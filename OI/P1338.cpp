#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::vector;

inline long long calc(long long x) {
    return x*(x-1)/2;
}

int main() {
    int n,m;
    cin>>n>>m;
    vector<int> ans;
    int l=1,r=n;
    for(int i=1;i<=n;i++) {
        long long t=calc(n-i);
        if(t>=m) {
            ans.push_back(l);
            l++;
        } else {
            ans.push_back(r);
            r--;
            m-=n-i;
        }
    }
    for(int i=0;i<n;i++) {
        cout<<ans[i];
        if(i!=n-1) cout<<' ';
    }
    cout<<endl;
    return 0;
}