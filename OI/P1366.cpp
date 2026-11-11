#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::ios;

int T,n,m;
unsigned long long a[10000000+1145],b[10000000+1145];

inline unsigned long long read() {
    unsigned long long x=0,f=1;
    char ch=getchar();
    while(ch<'0'||ch>'9') {
        if(ch=='-') f=-1;
        ch=getchar();
    }
    while(ch>='0'&&ch<='9') {
        x=x*10+ch-'0';
        ch=getchar();
    }
    return x*f;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    T=read();
    while(T--) {
        n=read(); m=read();
        for(int i=1;i<=n;++i) a[i]=read();
        for(int i=1;i<=m;++i) b[i]=read();
        unsigned long long ans=0;
        int p=1; //bµÄÖ¸Õëß÷
        for(int i=1;i<=n;++i) {
            unsigned long long cnt=0;
            while(p<=m && b[p]<a[i]) ++p;
            while(p<=m && b[p]==a[i]) {
                ++cnt;
                ++p;
            }
            ans^=cnt;
        }
        cout<<ans<<'\n';
    }
    return 0;
}