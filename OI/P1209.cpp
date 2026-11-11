#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::sort;
using std::vector;
using std::greater;
using std::max;
using std::min;

int m,s,c,ans;
vector<int> l; // 有牛的牛棚编号喵
vector<int> gap; // 相邻有牛牛棚之间的间隔喵

int main() {
    cin>>m>>s>>c;
    for(int i=0;i<c;i++) {
        int x;
        cin>>x;
        l.push_back(x);
    }
    sort(l.begin(),l.end());
    ans=l.back()-l.front()+1;
    for(int i=1;i<c;i++) {
        gap.push_back(l[i]-l[i-1]-1);
    }
    sort(gap.begin(),gap.end(),greater<int>());
    for(int i=0;i<min(m-1,(int)gap.size());i++) {
        ans-=gap[i];
    }
    cout<<ans<<'\n';
    return 0;
}