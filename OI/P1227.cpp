#include<bits/stdc++.h>
using std::pair;
using std::make_pair;
using std::sort;
using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;

inline pair<double,double> mid(pair<int,int> a,pair<int,int> b){
    return make_pair((a.first+b.first)/2.0,(a.second+b.second)/2.0);
}

int n;
pair<int,int> p[20005];

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(0);
    std::cin>>n;
    for(int i=0;i<n;i++) std::cin>>p[i].first>>p[i].second;
    sort(p,p+n);
    pair<double,double> c=mid(p[0],p[n-1]);
    for(int i=1;i<=n/2;i++){
        pair<double,double> t=mid(p[i],p[n-1-i]);
        if(t.first!=c.first||t.second!=c.second){
            cout<<"This is a dangerous situation!"<<endl;
            return 0;
        }
    }
    cout<<fixed<<setprecision(1);
    cout<<"V.I.P. should stay at ("<<c.first<<","<<c.second<<")."<<endl;
    return 0;
}