#include<bits/stdc++.h>
using std::cin;
using std::cout;
using std::cerr;
using std::endl;

int Q;
long long r;

int m_d[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};

inline bool is_leap_julian(int y) {
    if(y<=0) {
        y=-y;
        return y%4==1;
    }
    return y%4==0;
}

inline bool is_leap_gregorian(int y) {
    if(y%400==0) return true;
    if(y%100==0) return false;
    return y%4==0;
}

int main() {
    std::ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cin>>Q;
    while(Q--) {
        cin>>r;
        // 先处理到公元前4713年1月1日
        // 儒略日0对应公元前4713年1月1日
        long long day=r;
        int y=-4713;
        int m=1;
        int d=1;
        // 处理到1582年10月15日之前
        // 先快速跳过整年
        // 儒略历从公元前4713年到公元1582年10月4日
        // 先处理到公元1年1月1日
        // 公元前4713年到公元前1年共4713年
        // 但公元前1年之后是公元1年，所以公元前年份从-4713到-1共4713年
        // 公元1年到1582年共1582年
        // 但1582年只到10月4日
        // 先处理到1582年1月1日
        // 公元前4713年到公元1582年共4713+1582-1=6294年? 不对
        // 公元前4713年1月1日到公元前1年12月31日共4713年
        // 公元1年1月1日到1582年1月1日共1581年
        // 所以总年数4713+1581=6294年
        // 但需要减去1582年10月5日到10月14日这10天
        // 先处理到1582年10月15日
        // 先快速跳到1582年
        // 计算从公元前4713年到公元1582年10月15日的总天数
        // 但直接算比较麻烦，我们分段处理
        // 先处理到公元1年1月1日
        // 公元前4713年到公元前1年共4713年
        // 其中闰年：能被4整除的年份，但公元前年份要特殊处理
        // 公元前1年、前5年...即-1,-5,-9... 这些是闰年
        // 即年份y满足(-y)%4==1
        // 从-4713到-1，共4713年
        // 闰年个数：从-4713到-1，每4年一个闰年
        // 第一个闰年是-4713? -4713%4=-1, -(-4713)%4=1, 是闰年
        // 最后一个闰年是-1? -(-1)%4=1, 是闰年
        // 所以闰年个数 = (4713+3)/4 = 1179
        // 平年个数 = 4713-1179 = 3534
        // 总天数 = 3534*365 + 1179*366 = 3534*365 + 1179*366
        // 计算一下
        long long days_to_1ad = 3534LL*365 + 1179LL*366;
        // 从公元1年1月1日到1582年1月1日共1581年
        // 公元1年到1581年，闰年：能被4整除的年份
        // 闰年个数：1581/4 = 395
        // 平年个数：1581-395 = 1186
        // 总天数 = 1186*365 + 395*366
        long long days_1ad_to_1582 = 1186LL*365 + 395LL*366;
        // 1582年1月1日到10月15日
        // 1月31,2月28,3月31,4月30,5月31,6月30,7月31,8月31,9月30,10月15
        // 到10月15日共31+28+31+30+31+30+31+31+30+15 = 288天
        long long days_1582_to_oct15 = 288;
        long long total_before_1582 = days_to_1ad + days_1ad_to_1582 + days_1582_to_oct15;
        // 但注意1582年10月5日到14日不存在，所以实际天数要减去10
        total_before_1582 -= 10;
        // 现在total_before_1582是从公元前4713年1月1日到1582年10月15日的天数
        // 但注意儒略日0对应公元前4713年1月1日，所以儒略日r对应的是经过r天后的日期
        // 如果r < total_before_1582，则日期在1582年10月15日之前
        if(day < total_before_1582) {
            // 儒略历
            y=-4713;
            m=1;
            d=1;
            // 先快速跳年
            // 从公元前4713年到公元1582年，但只到1582年10月4日
            // 先跳到公元1年
            long long days_to_1ad2 = 3534LL*365 + 1179LL*366;
            if(day >= days_to_1ad2) {
                day -= days_to_1ad2;
                y=1;
                // 再跳到1582年1月1日
                long long days_1ad_to_1582_2 = 1186LL*365 + 395LL*366;
                if(day >= days_1ad_to_1582_2) {
                    day -= days_1ad_to_1582_2;
                    y=1582;
                    // 1582年1月到10月4日
                    // 1582年不是闰年
                    // 1月31,2月28,3月31,4月30,5月31,6月30,7月31,8月31,9月30,10月4
                    // 到10月4日共31+28+31+30+31+30+31+31+30+4 = 277天
                    // 但注意10月5日到14日不存在，所以10月4日之后直接跳到10月15日
                    // 这里day是到10月4日的天数
                    // 如果day < 277，则在1582年10月4日之前
                    // 否则在1582年10月15日之后，但我们已经判断了day < total_before_1582，所以不会出现
                    // 直接按月处理
                    for(int i=1;i<=12;i++) {
                        int md = m_d[i];
                        if(i==2 && is_leap_julian(y)) md=29;
                        if(day < md) {
                            m=i;
                            d=day+1;
                            break;
                        }
                        day -= md;
                    }
                } else {
                    // 在公元1年到1581年之间
                    // 先跳整年
                    while(true) {
                        int days_in_year = is_leap_julian(y)?366:365;
                        if(day >= days_in_year) {
                            day -= days_in_year;
                            y++;
                        } else break;
                    }
                    // 按月处理
                    for(int i=1;i<=12;i++) {
                        int md = m_d[i];
                        if(i==2 && is_leap_julian(y)) md=29;
                        if(day < md) {
                            m=i;
                            d=day+1;
                            break;
                        }
                        day -= md;
                    }
                }
            } else {
                // 在公元前
                // 从-4713到-1
                while(true) {
                    int days_in_year = is_leap_julian(y)?366:365;
                    if(day >= days_in_year) {
                        day -= days_in_year;
                        y++;
                    } else break;
                }
                // 按月处理
                for(int i=1;i<=12;i++) {
                    int md = m_d[i];
                    if(i==2 && is_leap_julian(y)) md=29;
                    if(day < md) {
                        m=i;
                        d=day+1;
                        break;
                    }
                    day -= md;
                }
            }
        } else {
            // 格里高利历
            day -= total_before_1582;
            y=1582;
            m=10;
            d=15;
            // 先处理1582年剩下的日子
            // 10月15日到12月31日
            // 10月剩余16天(15到31),11月30,12月31
            // 共16+30+31=77天
            // 但day从0开始，所以如果day<77，则在1582年内
            if(day < 77) {
                // 1582年10月15日之后
                // 10月剩余16天
                if(day < 16) {
                    d=15+day;
                } else {
                    day -= 16;
                    m=11;
                    if(day < 30) {
                        d=1+day;
                    } else {
                        day -= 30;
                        m=12;
                        d=1+day;
                    }
                }
            } else {
                day -= 77;
                y=1583;
                // 快速跳年
                // 从1583年到10^9年
                // 每400年有97个闰年，共146097天
                // 先跳400年块
                long long block = 146097LL;
                long long blocks = day / block;
                y += blocks * 400;
                day %= block;
                // 再跳年
                while(true) {
                    int days_in_year = is_leap_gregorian(y)?366:365;
                    if(day >= days_in_year) {
                        day -= days_in_year;
                        y++;
                    } else break;
                }
                // 按月处理
                for(int i=1;i<=12;i++) {
                    int md = m_d[i];
                    if(i==2 && is_leap_gregorian(y)) md=29;
                    if(day < md) {
                        m=i;
                        d=day+1;
                        break;
                    }
                    day -= md;
                }
            }
        }
        if(y<=0) {
            cout<<d<<' '<<m<<' '<<-y<<" BC\n";
        } else {
            cout<<d<<' '<<m<<' '<<y<<'\n';
        }
    }
    return 0;
}