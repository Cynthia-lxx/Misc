#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::endl;
using std::min;
using std::max;
using std::swap;
using std::abs;

inline void solve() {
    long long s,t,n;
    while(cin>>s>>t>>n) {
        long long mx=0,mn=0;
        // 最大得分：尽量赢，赢不了就平
        // 赢一场需要净胜球至少1，且总进球>总失球
        long long wins = min(n, min(s, t + (s - t))); // 实际上赢场数受限于进球和失球
        // 更准确：赢一场需要至少1个净胜球，且进球数>失球数
        // 最大赢场数 = min(n, min(s, t + (s - t))) 不对
        // 赢一场需要进球比失球多至少1，所以赢w场需要s>=w, t>=0, 且s - t >= w
        // 但失球可以分配到输的场次
        long long max_wins = min(n, min(s, s - t)); // s-t可能为负
        if(s - t < 0) max_wins = 0;
        else max_wins = min(n, s - t);
        // 但还要保证每场赢的进球至少1，失球0，所以s>=max_wins, t任意
        // 实际上赢w场，需要s>=w, 且剩余进球和失球可以分配
        // 更精确：赢w场，消耗w个进球，0个失球，剩余s-w个进球，t个失球
        // 剩余场次n-w，可以平局或输
        // 平局需要进球=失球，所以剩余进球和失球可以配对
        // 最大得分 = 3*max_wins + min(n - max_wins, (s - max_wins) + t) // 不对
        // 平局一场消耗1进球1失球，得1分
        long long draws = min(n - max_wins, min(s - max_wins, t));
        mx = 3 * max_wins + draws;
        
        // 最小得分：尽量输，输不了就平
        // 输一场需要进球<失球，即至少失球比进球多1
        // 输一场消耗0进球，1失球（相对），但实际失球可以更多
        // 最小化得分：先尽量输，再尽量平
        // 输l场需要t >= l, 且s < t - (l-1) 即剩余场次中进球<失球
        // 更简单：最小得分 = 0 如果可能全输
        // 全输条件：每场失球>进球，总失球>总进球，且场次足够
        // 但进球和失球可以分配
        // 输l场，消耗0进球，l个失球（每场至少1失球），剩余s进球，t-l失球
        // 剩余场次n-l，可以平局或赢
        // 平局需要进球=失球，赢需要进球>失球
        // 最小得分：先尽量输，然后尽量平，最后赢
        long long max_losses = min(n, t); // 每场至少失1球
        // 但输的场次中进球必须小于失球，所以每场进球0，失球>=1
        // 消耗0进球，max_losses失球
        // 剩余s进球，t-max_losses失球，n-max_losses场
        // 剩余场次中，平局需要进球=失球，赢需要进球>失球
        // 最小得分：先平局，再赢
        long long rem_s = s, rem_t = t - max_losses, rem_n = n - max_losses;
        if(rem_n < 0) {
            // 输太多，调整
            max_losses = n;
            rem_s = s;
            rem_t = t - max_losses;
            rem_n = 0;
        }
        // 平局场次 = min(rem_n, min(rem_s, rem_t))
        long long draws2 = min(rem_n, min(rem_s, rem_t));
        rem_s -= draws2;
        rem_t -= draws2;
        rem_n -= draws2;
        // 赢场次 = min(rem_n, rem_s - rem_t) 但需要rem_s > rem_t
        long long wins2 = 0;
        if(rem_s > rem_t) {
            wins2 = min(rem_n, rem_s - rem_t);
        }
        mn = draws2 + 3 * wins2;
        
        // 特殊情况：如果全输不可能（比如s太大），需要重新考虑
        // 实际上最小得分可能为0，如果存在全输方案
        // 全输条件：每场进球<失球，总进球<总失球，且场次足够
        // 如果s < t + n - 1? 更简单：最小得分至少为0
        // 但上述计算可能得到非0最小值
        // 检查是否可能得0分：需要每场都输，即每场进球<失球
        // 如果s >= t + n，则不可能全输，因为总进球>=总失球+n
        // 此时最小得分至少为1
        // 更精确：最小得分 = max(0, 3*(s - t - n) + ...) 复杂
        // 直接用上述贪心，但需要处理边界
        // 如果max_losses = n，则全输，得0分
        if(max_losses == n) {
            mn = 0;
        } else {
            // 上述计算已经考虑了
        }
        
        cout << mx << " " << mn << endl;
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    solve();
    return 0;
}