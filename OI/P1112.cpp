#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::vector;
using std::swap;

int l,r,L,R,k;
int cnt[10000005+1145]; // 计数每个数字被多少种进制识别为波浪数喵

inline bool isWave(int x,int b){
    if(x<b) return true;
    int d[35], len=0;
    while(x){d[len++]=x%b; x/=b;}
    if(len==1) return true;
    if(d[0]==d[1]) return false;
    int p=d[0]>d[1]?1:-1; // 1上升 -1下降喵
    for(int i=2;i<len;++i){
        int np=(d[i-1]>d[i])?1:-1;
        if(np!=p || d[i-1]==d[i]) return false;
        p=np;
    }
    return true;
}

int main(){
    cin>>l>>r>>L>>R>>k;
    for(int b=l;b<=r;++b){
        for(int len=1;len<=7;++len){ // 10^7最多7位喵
            for(int x=1;x<=b-1;++x){ // 第一个数字喵
                for(int y=0;y<=b-1;++y){
                    if(x==y) continue;
                    int num=0;
                    for(int i=0;i<len;++i){
                        num=num*b + (i%2?y:x);
                        if(num>R) break;
                    }
                    if(num>=L && num<=R){
                        if(num>=b){
                            int tmp=num, ok=1;
                            int d[35], l2=0;
                            while(tmp){d[l2++]=tmp%b; tmp/=b;}
                            for(int i=1;i<l2;++i) if(d[i]==d[i-1]) {ok=0; break;}
                            if(!ok) continue;
                            int p2=d[0]>d[1]?1:-1;
                            for(int i=2;i<l2;++i){
                                int np=(d[i-1]>d[i])?1:-1;
                                if(np!=p2){ok=0; break;}
                            }
                            if(!ok) continue;
                        }
                        cnt[num]++;
                    }
                }
            }
        }
    }
    for(int i=L;i<=R;++i) if(cnt[i]>=k) cout<<i<<endl;
    return 0;
}
