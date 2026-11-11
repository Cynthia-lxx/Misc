#include<bits/stdc++.h>
using std::string;
using std::vector;
using std::cout;
using std::cin;
using std::endl;
using std::pair;
using std::make_pair;
using std::min;

int n;
string target;
vector<pair<char,string>> ans; // 存储操作序列喵
bool found=false;

inline void dfs(int pos, string a, string b, string c, vector<pair<char,string>> cur) {
    if(found) return;
    if(pos==n) {
        if(a.empty() && b.empty() && c.empty()) {
            ans=cur;
            found=true;
        }
        return;
    }
    // 从A到B
    if(!a.empty()) {
        char h=a.back();
        string na=a; na.pop_back();
        string nb=b+h;
        vector<pair<char,string>> ncur=cur;
        ncur.push_back({h,"A B"});
        dfs(pos,na,nb,c,ncur);
        if(found) return;
    }
    // 从A到D
    if(!a.empty() && a.back()==target[pos]) {
        char h=a.back();
        string na=a; na.pop_back();
        vector<pair<char,string>> ncur=cur;
        ncur.push_back({h,"A D"});
        dfs(pos+1,na,b,c,ncur);
        if(found) return;
    }
    // 从B到C
    if(!b.empty()) {
        char h=b.back();
        string nb=b; nb.pop_back();
        string nc=c+h;
        vector<pair<char,string>> ncur=cur;
        ncur.push_back({h,"B C"});
        dfs(pos,a,nb,nc,ncur);
        if(found) return;
    }
    // 从B到D
    if(!b.empty() && b.back()==target[pos]) {
        char h=b.back();
        string nb=b; nb.pop_back();
        vector<pair<char,string>> ncur=cur;
        ncur.push_back({h,"B D"});
        dfs(pos+1,a,nb,c,ncur);
        if(found) return;
    }
    // 从C到D
    if(!c.empty() && c.back()==target[pos]) {
        char h=c.back();
        string nc=c; nc.pop_back();
        vector<pair<char,string>> ncur=cur;
        ncur.push_back({h,"C D"});
        dfs(pos+1,a,b,nc,ncur);
        if(found) return;
    }
}

int main() {
    cin>>n>>target;
    string a;
    for(int i=n-1;i>=0;--i) a+=target[i]; // 初始A栈，逆序喵
    // 实际上初始A是入口顺序，但为了栈操作方便，我们让a从栈顶到栈底是目标顺序的反向喵
    // 更简单的做法：初始A序列从左到右是a,b,c,d...，即第一个进入的是a，最后一个进入的是n喵
    // 重新构建：初始A栈底是第一个火车，栈顶是最后一个火车喵
    a.clear();
    for(int i=0;i<n;++i) a+=('a'+i); // 初始顺序a,b,c...喵
    // 但题目输入是目标序列，我们需要从初始顺序调度到目标顺序喵
    // 实际上初始A是入口顺序，从左到右是a,b,c...，即第一个火车是a，最后一个是第n个喵
    // 所以A栈应该是从栈底到栈顶依次是a,b,c...，即栈顶是最后一个火车喵
    // 但为了dfs方便，我们让a字符串表示栈，栈顶在末尾喵
    // 所以初始a="abc...n"喵
    a.clear();
    for(int i=0;i<n;++i) a+=('a'+i);
    
    vector<pair<char,string>> cur;
    dfs(0,a,"","",cur);
    
    if(!found) {
        cout<<"NO"<<endl;
        return 0;
    }
    for(auto &p:ans) {
        cout<<p.first<<" "<<p.second<<endl;
    }
    return 0;
}