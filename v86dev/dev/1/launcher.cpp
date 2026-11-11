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
string IdToSystem(int q=7){
	if(q==1) return "archlinux";
	else if(q==2) return "dsl";
	else if(q==3) return "buildroot";
	else if(q==4) return "reactos";
	else if(q==5) return "windows2000";
	else if(q==6) return "windows98";
	else if(q==7) return "windows95";
	else if(q==8) return "windows31";
	else if(q==9) return "windows1";
	else if(q==10) return "msdos";
	else if(q==11) return "freedos";
	else if(q==12) return "freebsd";
	else if(q==13) return "openbsd";
	else if(q==14) return "9front";
	else if(q==15) return "haiku";
	else if(q==16) return "serenity";
	else if(q==17) return "helenos";
	else if(q==18) return "fiwix";
	else if(q==19) return "android";
	else if(q==20) return "oberon";
	else if(q==21) return "kolibrios";
	else if(q==22) return "qnx";
	else if(q==23) return "snowdrop";
	else if(q==24) return "solos";
	else if(q==25) return "bootchess";
	else if(q==26) return "sectorlisp";
	else return "windows95";
}
// 自定义函数实现类似Python的ljust函数功能
std::string ljust(const std::string &s, int width, char fillchar=' ')
{
    if (width <= s.length()) {
        return s;
    } else {
        return s + std::string(width - s.length(), fillchar);
    }
} 
std::string getFileNameFromPath(const std::string& path) {
    size_t found = path.find_last_of("/\\");
    if (found != std::string::npos) {
        return path.substr(found + 1);
    }
    return path;
}
bool firsttimeuse() {
    struct stat buffer;
    if (stat(".\\log\\first.sav", &buffer) == 0) {
        return false;
    } else {
        return true;
    }
}
void writefirstsav(){
	std::ofstream f(".\\log\\first.sav");
    if (f.is_open()) {
        f << "1";
        f.close();
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
	printf("\033[1;7mv86 Virtual OS Manager\033[0m\n");
	printf("作者: \033[1;33m街角的猫_wjz\033[0m\n");
	printf("作者B站：\033[1;33mhttps://space.bilibili.com/3493290083093446\033[0m\n");
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
		//system("cd .\\scr");
		system("pescan.exe");
		printf("\n------------------------------------\n已完成文件校验。接下来，您可以正常使用本软件。\n------------------------------------\n\n");
		//system("cd ..\\");
		writefirstsav();
	}
}
void bootsystem(){
	int q=Select();
	string profile=IdToSystem(q);
	system("cls");
	printf("\n您选择了\033[1m%d\033[0m号系统，profile alias为\033[1m\"",q);
	cout<<profile;
	printf("\"\033[0m。\n");
	printf("软件即将调用内核启用虚拟机，\033[7m请注意软件是否正以管理员身份运行。\033[0m\n");
	printf("------------------------------------\n");
	printf("\033[1;33m警告\033[0m\033[1;7;33m（首次启动请详阅）\033[0m\033[1;33m：\n");
	printf("本虚拟机依赖于Windows API运行，体积较小但占用CPU相对于常规虚拟机稍大，请慎\n");
	printf("重选择运行的操作系统，以免对您的物理计算机造成过大负载。\n");
	printf("为了防止非正常关闭造成的虚拟IDE控制器损坏，虚拟机不会保存数据，请不要尝试使\n");
	printf("用虚拟机保存数据。虚拟计算机使用独立沙盒的方式进行RAM内存读写，当超出系统预\n");
	printf("留的阈值之后会与物理计算机发生内存交换，为了防止数据丢失，请不要过度依赖虚拟\n");
	printf("机登录网络账号或存储生产性数据。如有数据损坏，作者概不承担任何赔偿责任。（本\n");
	printf("软件本身无恶意行为，正常使用不会造成负面影响）\n");
	printf("请不要同时启动多个基于同一操作系统的虚拟机，否则可能会造成隐性的不可预知的风\n");
	printf("险。\033[0m");
	printf("\n------------------------------------\n");
	printf("\033[1m注意：\n");
	printf("虚拟机启动后，您可以通过localhost进行访问。终端会显示localhost服务器的运行信\n");
	printf("息，并会在正常启动的情况下将访问地址复制到剪贴板。\n");
	printf("当本窗口出现黄色字体\"Press CTRL+C to quit\"后，请打开您的浏览器，并按Ctrl+V\n");
	printf("快捷键，然后按Enter键访问虚拟机。\n");
	printf("首次访问显示空白界面属于正常现象。当等待一段时间后窗口没有反应，您可以尝试刷\n");
	printf("新页面。页面显示黑底白字\"Fetching v86.wasm\"后显示\"Downloading images(N/N)\"\n");
	printf("时即可正常启动。您可能需要反复刷新页面直到能够正常启动。\n");
	printf("\033[7m请不要关闭本窗口。如果需要关闭服务器，请在启动服务器后按Ctrl+C！\033[0m");
	printf("\n------------------------------------\n");
	printf("(开发者选项) 您可以自行选择服务器的端口号。如果您不想进行此设置，请直接输入\033[1mq\033[0m\n");
	printf("跳过。如果您设置了端口号，请注意不要发生端口访问冲突。默认端口为\033[1m8097\033[0m。\n");
	printf("请键入端口以选择或\033[1mq\033[0m以跳过并按\033[1mEnter\033[0m >");
	string lport;
	cin>>lport;
	if(lport=="q") lport="";
	printf("\n------------------------------------\n接下来，虚拟机将启动。\n");
	system("pause");
	string cmd="core.exe "+profile+" "+lport;
	system(cmd.c_str());
	
}

string fname(){
	std::string fullPath = __FILE__;
    return getFileNameFromPath(fullPath);
}
int factory(){
	printf("此操作会彻底清空所有虚拟机数据并还原初始设置，确定要继续吗？\n");
	int a[3];
	for(int i=0;i<3;i++){
		int x=rand()%10;
		printf("(%d/3) 继续，请输入%d并按Enter。取消，请输入其他任何数字并按Enter。\n",i+1,x);
		printf("> ");
		scanf("%d",&a[i]);
		if(a[i]!=x) {
			printf("操作取消成功。\n");
			return 0;
		}
	}
	printf("正在清空...\n");
	system("del .\\log\\* /f /q");
	system("del .\\log\\*.* /f /q");
	printf("清空完成。正在重新启动软件...\n");
	system(fname().c_str());
	exit(0);
	return 0;
}
void choose(){
	printf("请按\033[1m1\033[0m启动虚拟机，\n");
	printf("按\033[1m2\033[0m浏览可用的虚拟机列表，\n");
	printf("按\033[1m3\033[0m启动DirectX诊断，\n");
	printf("按\033[1m4\033[0m清空数据并恢复原始设置，\n");
	printf("或按\033[1m0\033[0m退出软件。\n>");
	char c = getch();
	printf("\n");
	if(c=='1') bootsystem();
	else if(c=='2') {
		Browse();
		system("pause");
	}
	else if(c=='3') {
		system("dxdiag.exe");
		system("pause");
	}
	else if(c=='4') factory();
	else if(c=='0') exit(0);
}
int main(){
	signal(SIGINT,SIG_IGN);
	system("cls");
	while(1){
		system("cls");
		Startup();
		choose();
		system("cls");
	}
	system("pause");
    return 0;
}
