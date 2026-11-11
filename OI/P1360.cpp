#include<bits/stdc++.h>
using std::map;
using std::vector;
using std::max;

inline int read() {
    int x=0; char c=getchar();
    while(c<'0'||c>'9') c=getchar();
    while(c>='0'&&c<='9') x=x*10+c-'0',c=getchar();
    return x;
}

int n,m,ans;
int sum[31]; // 每种能力的前缀和喵
map<vector<int>,int> mp; // 状态到最早出现位置的映射喵

int main() {
    n=read(); m=read();
    vector<int> init(m-1,0);
    mp[init]=0;
    for(int i=1;i<=n;++i) {
        int x=read();
        for(int j=0;j<m;++j)
            if(x>>j&1) sum[j]++;
        vector<int> cur(m-1);
        for(int j=0;j<m-1;++j)
            cur[j]=sum[j]-sum[m-1]; // 相对差值喵
        if(mp.count(cur))
            ans=max(ans,i-mp[cur]);
        else
            mp[cur]=i;
    }
    printf("%d\n",ans);
    return 0;
}