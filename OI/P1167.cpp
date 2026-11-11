#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::sort;
using std::string;
using std::sscanf;

int n;
int t[5005];
int yy1,mm1,dd1,hh1,min1;
int yy2,mm2,dd2,hh2,min2;
int mdays[12]={31,28,31,30,31,30,31,31,30,31,30,31};

inline bool isleap(int y){
    return (y%400==0)||(y%4==0&&y%100!=0);
}

long long tomin(int y,int m,int d,int h,int mi){
    long long res=0;
    for(int i=0;i<y;i++){
        res+=365;
        if(isleap(i))res++;
    }
    for(int i=1;i<m;i++){
        res+=mdays[i-1];
        if(i==2&&isleap(y))res++;
    }
    res+=d-1;
    res*=1440;
    res+=h*60+mi;
    return res;
}

int main(){
    cin>>n;
    for(int i=0;i<n;i++)cin>>t[i];
    string s1,s2;
    cin>>s1>>s2;
    sscanf(s1.c_str(),"%d-%d-%d-%d:%d",&yy1,&mm1,&dd1,&hh1,&min1);
    sscanf(s2.c_str(),"%d-%d-%d-%d:%d",&yy2,&mm2,&dd2,&hh2,&min2);
    long long st=tomin(yy1,mm1,dd1,hh1,min1);
    long long ed=tomin(yy2,mm2,dd2,hh2,min2);
    long long tot=ed-st;
    sort(t,t+n);
    int ans=0;
    for(int i=0;i<n;i++){
        if(tot>=t[i]){
            tot-=t[i];
            ans++;
        }else break;
    }
    cout<<ans;
    return 0;
}