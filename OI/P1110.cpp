#include<bits/stdc++.h>
using std::multiset;
using std::set;
using std::pair;
using std::make_pair;
using std::min;
using std::abs;
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ios;

inline int read() {
    int x=0,f=1;char ch=getchar();
    while(ch<'0'||ch>'9'){if(ch=='-')f=-1;ch=getchar();}
    while(ch>='0'&&ch<='9'){x=x*10+ch-'0';ch=getchar();}
    return x*f;
}

int n,m,a[500005],last[500005],ans_gap=2e9,ans_sort=2e9;
multiset<int> gap,val;
set<int> s;

void add_gap(int x) {
    if(x<ans_gap) ans_gap=x;
    gap.insert(x);
}

void del_gap(int x) {
    gap.erase(gap.find(x));
    if(gap.empty()) ans_gap=2e9;
    else ans_gap=*gap.begin();
}

void add_sort(int x) {
    auto it=s.lower_bound(x);
    if(it!=s.end()) {
        int d=*it-x;
        if(d<ans_sort) ans_sort=d;
        val.insert(d);
    }
    if(it!=s.begin()) {
        --it;
        int d=x-*it;
        if(d<ans_sort) ans_sort=d;
        val.insert(d);
    }
    s.insert(x);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(0);
    n=read();m=read();
    for(int i=1;i<=n;i++) {
        a[i]=read();
        last[i]=a[i];
        s.insert(a[i]);
    }
    for(int i=2;i<=n;i++) {
        int d=abs(a[i]-a[i-1]);
        gap.insert(d);
        if(d<ans_gap) ans_gap=d;
    }
    for(int i=1;i<=n;i++) {
        auto it=s.find(a[i]);
        if(it!=s.begin()) {
            int d=a[i]-*prev(it);
            val.insert(d);
            if(d<ans_sort) ans_sort=d;
        }
        if(next(it)!=s.end()) {
            int d=*next(it)-a[i];
            val.insert(d);
            if(d<ans_sort) ans_sort=d;
        }
    }
    while(m--) {
        string op;
        cin>>op;
        if(op[0]=='I') {
            int i=read(),k=read();
            int pre=last[i];
            int nxt=(i==n?2e9:a[i+1]);
            if(i<n) {
                int old=abs(nxt-pre);
                del_gap(old);
                int d1=abs(k-pre);
                add_gap(d1);
                int d2=abs(nxt-k);
                add_gap(d2);
            } else {
                int d=abs(k-pre);
                add_gap(d);
            }
            last[i]=k;
            add_sort(k);
        } else if(op[4]=='G') {
            cout<<ans_gap<<endl;
        } else {
            cout<<ans_sort<<endl;
        }
    }
    return 0;
}