#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::max;
using std::map;

int n,m,R,Ta,Tb,Tc,Td,A,H,E;
int p[1005],s[1005];
map<int,map<int,int>> rec; // 记录每个学生每道题的最高分喵

int main() {
    cin>>n>>m;
    for(int i=1;i<=n;++i) cin>>p[i];
    for(int i=1;i<=m;++i) cin>>s[i];
    cin>>Ta>>Tb>>Tc>>Td>>A>>H>>E;
    cin>>R;
    long long trad=0,luogu=0;
    // 传统方法：装载所有题目
    trad+=1LL*n*Ta;
    // 洛谷：装载所有题目
    luogu+=1LL*n*Ta;
    // 统计每个学生每道题的评测次数和成绩变化
    map<int,map<int,int>> cnt; // 每个学生每道题的评测次数喵
    map<int,map<int,int>> best; // 每个学生每道题的最好成绩喵
    for(int i=1;i<=R;++i) {
        int pr,sr,sc;
        cin>>pr>>sr>>sc;
        cnt[sr][pr]++;
        if(sc>best[sr][pr]) {
            if(E==1 && best[sr][pr]!=0) trad+=Td; // 更新记录喵
            best[sr][pr]=sc;
        }
    }
    // 计算传统方法每个学生的耗时
    for(int i=1;i<=m;++i) {
        int stu=s[i];
        long long local_time=1LL*Ta; // 自己装载数据喵
        long long teacher_time=0;
        int total_cnt=0;
        for(auto &pr:cnt[stu]) {
            int prob=pr.first;
            int c=pr.second;
            total_cnt+=c;
            // 教师机：每次往返Tb
            teacher_time+=1LL*c*Tb;
            // 自己装载：一次Ta，每次Tc
            local_time+=1LL*c*Tc;
        }
        // 选择最优方式
        trad+=std::min(local_time, teacher_time);
    }
    // 洛谷：每次评测Tc
    luogu+=1LL*R*Tc;
    // 考虑稳定性和罚时
    luogu=luogu*100/A+H;
    // 输出
    cout<<trad<<endl;
    cout<<luogu<<endl;
    if(luogu<trad) cout<<"Use Luogu!"<<endl;
    else cout<<"Forget it..."<<endl;
    return 0;
}