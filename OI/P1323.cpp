#include<bits/stdc++.h>
using std::priority_queue;
using std::vector;
using std::greater;
using std::string;
using std::cin;
using std::cout;
using std::to_string;

int k,m;
string s;

inline void gen() {
    priority_queue<int,vector<int>,greater<int>> pq;
    pq.push(1);
    for(int i=0;i<k;++i) {
        int cur=pq.top(); pq.pop();
        s+=to_string(cur);
        pq.push(2*cur+1);
        pq.push(4*cur+5);
    }
}

int main() {
    cin>>k>>m;
    gen();
    cout<<s<<"\n";
    int del=m;
    string ans;
    for(char c:s) {
        while(del>0 && !ans.empty() && ans.back()<c) {
            ans.pop_back();
            --del;
        }
        ans.push_back(c);
    }
    while(del>0) {
        ans.pop_back();
        --del;
    }
    cout<<ans<<"\n";
    return 0;
}