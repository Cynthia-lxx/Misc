#include<bits/stdc++.h>
using std::string;
using std::cin;
using std::cout;
using std::endl;

inline int C(int n,int m){
    if(m>n) return 0;
    long long res=1;
    for(int i=1;i<=m;++i) res=res*(n-i+1)/i;
    return (int)res;
}

int main(){
    string s;
    cin>>s;
    int n=s.size();
    for(int i=1;i<n;++i)
        if(s[i]<=s[i-1]){
            cout<<0<<endl;
            return 0;
        }
    int ans=0;
    for(int i=1;i<n;++i) ans+=C(26,i);
    for(int i=0;i<n;++i){
        int start=(i==0?1:s[i-1]-'a'+2);
        for(int j=start;j<s[i]-'a'+1;++j)
            ans+=C(26-j,n-i-1);
    }
    cout<<ans+1<<endl;
    return 0;
}