#include<bits/stdc++.h>
using std::priority_queue;
using std::greater;
using std::cin;
using std::cout;
using std::endl;

int n,ans;
priority_queue<int,std::vector<int>,greater<int>> q; // Ð¡¸ù¶Ñß÷

int main(){
    cin>>n;
    for(int i=1;i<=n;i++){
        int x;cin>>x;
        q.push(x);
    }
    while(q.size()>1){
        int a=q.top();q.pop();
        int b=q.top();q.pop();
        ans+=a+b;
        q.push(a+b);
    }
    cout<<ans<<endl;
    return 0;
}