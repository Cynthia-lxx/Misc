#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;
using std::min;
int L,N,ans_min,ans_max;
int main(){
    cin>>L>>N;
    ans_min=0;ans_max=0;
    for(int i=1;i<=N;i++){
        int x;cin>>x;
        int left=x,right=L-x+1;
        ans_min=max(ans_min,min(left,right));
        ans_max=max(ans_max,max(left,right));
    }
    cout<<ans_min<<" "<<ans_max;
    return 0;
}