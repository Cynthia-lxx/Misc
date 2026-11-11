#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::string;
using std::vector;

int n;
long long ans;
int h[155][155]; // 每个位置向上连续白色高度喵
int stk[155],w[155]; // 单调栈和宽度喵

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(0);
    cin>>n;
    for(int i=1;i<=n;++i){
        string s;
        cin>>s;
        for(int j=1;j<=n;++j){
            if(s[j-1]=='W') h[i][j]=h[i-1][j]+1;
            else h[i][j]=0;
        }
        // 对每一行用单调栈计算以该行为底的矩形数量喵
        int top=0;
        for(int j=1;j<=n+1;++j){
            int cur_h = (j<=n ? h[i][j] : 0);
            int width=0;
            while(top>0 && stk[top]>=cur_h){
                int height=stk[top];
                width+=w[top];
                --top;
                ans+=1LL*height*width;
            }
            stk[++top]=cur_h;
            w[top]=width+1;
        }
    }
    cout<<ans<<"\n";
    return 0;
}