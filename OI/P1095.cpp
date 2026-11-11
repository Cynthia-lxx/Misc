#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::max;

int M,S,T;

int main(){
    cin>>M>>S>>T;
    int m=M,dist=0;
    for(int i=1;i<=T;++i){
        if(m>=10){
            dist+=60;
            m-=10;
        }else{
            m+=4;
        }
        if(dist>=S){
            cout<<"Yes\n"<<i;
            return 0;
        }
    }
    // 如果纯闪烁不行，考虑混合策略：在能闪烁时闪烁，否则跑步
    // 重新计算最大距离
    int maxdist=0;
    m=M;
    dist=0;
    for(int i=1;i<=T;++i){
        if(m>=10){
            dist+=60;
            m-=10;
        }else{
            // 比较跑步和回蓝哪个更优
            // 跑步：+17，回蓝：+4，但回蓝后未来可闪烁
            // 贪心：如果剩余时间足够回蓝并闪烁，则回蓝；否则跑步
            int remain=T-i;
            if(remain>=1 && m+4>=10){
                // 回蓝后能闪烁一次，比跑步17远
                m+=4;
            }else{
                dist+=17;
            }
        }
        maxdist=max(maxdist,dist);
        if(dist>=S){
            cout<<"Yes\n"<<i;
            return 0;
        }
    }
    cout<<"No\n"<<maxdist;
    return 0;
}