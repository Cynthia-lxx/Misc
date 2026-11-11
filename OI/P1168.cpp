#include<bits/stdc++.h>
using std::priority_queue;
using std::vector;
using std::greater;
using std::less;
using std::cin;
using std::cout;

int n,x;
priority_queue<int> l; // 大根堆存较小的一半喵
priority_queue<int,vector<int>,greater<int>> r; // 小根堆存较大的一半喵

int main(){
    cin>>n;
    for(int i=1;i<=n;++i){
        cin>>x;
        if(l.empty()||x<=l.top()) l.push(x);
        else r.push(x);
        // 调整使l.size()==r.size()或l.size()==r.size()+1
        if(l.size()>r.size()+1){
            r.push(l.top());
            l.pop();
        }else if(r.size()>l.size()){
            l.push(r.top());
            r.pop();
        }
        if(i&1) cout<<l.top()<<'\n';
    }
    return 0;
}