#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;

int n,ans; // 棋盘大小和总方案数喵
int l[14]; // l[i]表示第i行皇后所在的列喵
bool col[14],diag1[28],diag2[28]; // 列、主对角线、副对角线占用情况喵

inline void dfs(int row) {
    if(row>n) {
        ans++;
        if(ans<=3) {
            for(int i=1;i<=n;i++) {
                cout<<l[i];
                if(i<n) cout<<' ';
            }
            cout<<endl;
        }
        return;
    }
    for(int j=1;j<=n;j++) {
        if(!col[j] && !diag1[row+j] && !diag2[row-j+n]) {
            l[row]=j;
            col[j]=diag1[row+j]=diag2[row-j+n]=true;
            dfs(row+1);
            col[j]=diag1[row+j]=diag2[row-j+n]=false;
        }
    }
}

int main() {
    cin>>n;
    dfs(1);
    cout<<ans<<endl;
    return 0;
}