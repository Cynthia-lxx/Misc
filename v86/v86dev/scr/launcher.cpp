#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include<iostream>
#include<cstdio>
#include<conio.h>
#include<cstdlib>
#include<cstring>
#include<ctime>
#include<signal.h>
#include<stdio.h>
//#include<filesystem>
#include<sys/stat.h>
using namespace std;
// 自定义函数实现类似Python的ljust函数功能
std::string ljust(const std::string &s, int width, char fillchar=' ')
{
    if (width <= s.length()) {
        return s;
    } else {
        return s + std::string(width - s.length(), fillchar);
    }
} 
bool firsttimeuse() {
    struct stat buffer;
    if (stat("..\\log\\first.sav", &buffer) == 0) {
        return false;
    } else {
        std::ofstream f("..\\log\\first.sav");
        if (f.is_open()) {
            f << "1";
            f.close();
        }
        return true;
    }
}
void Browse(){
	printf("\033[1;32m绿色\033[0m表示该系统的大小可以在此计算机上流畅使用，\n");
	printf("\033[1;33m黄色\033[0m表示该系统的大小会要求您在使用之前进行一些等待，并且启动后会对您的计算机性能有一定的考验。\n");
	printf("\033[1;31m红色\033[0m表示启动该操作系统前可能需要等待超过1-2分钟，并且会有一定卡顿。\n");
	printf("1 |Arch Linux      |Terminal|12MB\n");
	printf("2 |Damn Small Linux|Desktop |\033[1;31m50MB\033[0m\n");
	printf("3 |Buildroot Linux |Terminal|5MB\n");
	printf("4 |ReactOS         |Desktop |\033[1;33m18MB\033[0m\n");
	printf("5 |Windows 2000    |Desktop |\033[1;33m22MB\033[0m\n");
	printf("6 |Windows 98      |Desktop |9.7MB\n");
	printf("7 |Windows 95      |Desktop |\033[1;32m4.6MB\033[0m\n");
	printf("8 |Windows 3.1     |Desktop |15MB\n");
	printf("9 |Windows 1.01    |Desktop |\033[1;32m0.6MB\033[0m\n");
	printf("10|MS-DOS 6.22     |Terminal|\033[1;32m4.4MB\033[0m\n");
	printf("11|FreeDOS         |Terminal|\033[1;32m0.5MB\033[0m\n");
	printf("12|FreeBSD         |Terminal|17MB\n");
	printf("13|OpenBSD         |Terminal|12MB\n");
	printf("14|9front          |Desktop |4.4MB\n");
	printf("15|Haiku           |Desktop |\033[1;33m38MB\033[0m\n");
	printf("16|SerenityOS      |Desktop |17MB\n");
	printf("17|HelenOS         |Desktop |7.9MB\n");
	printf("18|FiwixOS         |Terminal|15MB\n");
	printf("19|Android-x86     |Desktop |\033[1;31m42MB\033[0m\n");
	printf("20|Oberon          |Desktop |\033[1;32m1.2MB\033[0m\n");
	printf("21|KolibriOS       |Desktop |\033[1;32m1.4MB\033[0m\n");
	printf("22|QNX             |Desktop |\033[1;32m1.3MB\033[0m\n");
	printf("23|Snowdrop        |Terminal|\033[1;32m0.3MB\033[0m\n");
	printf("24|Solar OS        |Desktop |\033[1;32m0.3MB\033[0m\n");
	printf("25|Bootchess       |Terminal|\033[1m512B\033[0m\n");
	printf("26|SectorLISP      |Terminal|\033[1m512B\033[0m\n"); 
}
int Select(){
	int SChoice;
	printf("以下列出了可用的操作系统列表。输入每个系统所在的行左侧的数字以进行选择。\n");
	Browse(); 
	printf("请键入数字选择操作系统。\n");
	printf(">");
	scanf("%d",&SChoice);
	if(SChoice<1||SChoice>26){
		printf("选择无效。请重新输入。\n");
		printf(">");
		scanf("%d",&SChoice);
		while(SChoice<1||SChoice>26){
			printf("选择无效。请重新输入。\n");
			printf(">");
			scanf("%d",&SChoice);
		}
	}
	return SChoice;
}
void Verify(){
	;
} 
void Startup(){
	system("cls");
	printf("\033[7mv86 Virtual OS Manager\033[0m\n");
	printf("作者: \033[1;33m街角的猫_wjz\033[0m\n");
	printf("感谢\033[1;33m@PSPCrazy\033[0m提供的轻量系统镜像\n");
	printf("版本: \033[1;33m1.0.1dev\033[0m\n");
	printf("------------------------------------\n");
	printf("v86 Virtual OS是一款轻量级的虚拟机软件，相比于传统虚拟机，省去了配置\n");
	printf("、镜像等一系列繁琐的操作，易于上手，对于需要适应新的操作系统或者需要\n");
	printf("不同于自己计算机原装系统环境的用户非常有用，也可以用来体验一些早期系\n");
	printf("统。每个系统均不超过50MB，可以节省磁盘内存，对于下载、安装、配置、启\n");
	printf("动等一系列操作都实现了自动化。只需要选择一个系统，一键启动即可。\n");
	printf("------------------------------------\n");
	if(firsttimeuse()){
		printf("检测到您是首次启动本软件，因此在使用之前，我们需要进行一些setup操作，\n");
		printf("大概可以在2分钟之内完成。\n");
		printf("------------------------------------\n");
		printf("首先，本软件中的系统镜像全部依赖Windows SDK中的API，为了确保软件正常运\n");
		printf("行，我们需要校验SDK和PE镜像。\n");
		printf("请确保本软件正在以管理员身份运行，否则，文件校验可能失败。确认本软件以\n");
		printf("管理员身份运行之后，");
		system("pause");
		printf("\n正在启动文件校验......\n");
		system("pescan.exe");
	}
}
int main(){
	signal(SIGINT,SIG_IGN);
	system("cls");
	printf("%d",Select());
	Startup();
	system("pause");
    return 0;
}
