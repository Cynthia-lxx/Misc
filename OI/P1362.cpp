#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;

inline int S(long long x) {
    int s=0;
    while(x) { s+=x%10; x/=10; }
    return s;
}

vector<long long> gen;
void dfs(long long x) {
    if(x>1e9) return;
    if(x) gen.push_back(x);
    // 关键性质：Rabbit Number的各位数字只可能是0,1,2,3
    // 因为平方后不会产生进位导致数字和性质变化
    for(int d=0;d<=3;++d) dfs(x*10+d);
}

int main() {
    long long L,R;
    cin>>L>>R;
    dfs(0);
    int ans=0;
    for(auto x:gen) {
        if(x<L||x>R) continue;
        if(S(x*x)==S(x)*S(x)) ++ans;
    }
    cout<<ans;
    return 0;
}