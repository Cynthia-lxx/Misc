#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::vector;
using std::string;

int n;
vector<string> a,b;

bool eq(const vector<string>& x, const vector<string>& y) {
    for(int i=0;i<n;i++) if(x[i]!=y[i]) return false;
    return true;
}

vector<string> rot90(const vector<string>& x) {
    vector<string> r(n,string(n,' '));
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) r[j][n-1-i]=x[i][j];
    return r;
}

vector<string> rot180(const vector<string>& x) {
    vector<string> r(n,string(n,' '));
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) r[n-1-i][n-1-j]=x[i][j];
    return r;
}

vector<string> rot270(const vector<string>& x) {
    vector<string> r(n,string(n,' '));
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) r[n-1-j][i]=x[i][j];
    return r;
}

vector<string> reflect(const vector<string>& x) {
    vector<string> r(n,string(n,' '));
    for(int i=0;i<n;i++) for(int j=0;j<n;j++) r[i][n-1-j]=x[i][j];
    return r;
}

int main() {
    cin>>n;
    a.resize(n); b.resize(n);
    for(int i=0;i<n;i++) cin>>a[i];
    for(int i=0;i<n;i++) cin>>b[i];
    
    if(eq(rot90(a),b)) {cout<<1; return 0;}
    if(eq(rot180(a),b)) {cout<<2; return 0;}
    if(eq(rot270(a),b)) {cout<<3; return 0;}
    
    auto ra=reflect(a);
    if(eq(ra,b)) {cout<<4; return 0;}
    
    if(eq(rot90(ra),b)||eq(rot180(ra),b)||eq(rot270(ra),b)) {cout<<5; return 0;}
    
    if(eq(a,b)) {cout<<6; return 0;}
    
    cout<<7;
    return 0;
}