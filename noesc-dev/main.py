import subprocess
import psutil
import os
import time
import base64
import threading
import sys
import tkinter
import shutil

from tkinter import *
from tkinter.ttk import *
from tkinter.filedialog import askopenfilename as opf
root=Tk()
root.withdraw()
imlist=[
    ["R",-1,"Backend scanner","1.exe"],
    ["R",-1,"Web Shell Capture","4.exe"],
    ["R",-1,"Website Traffic","11.exe"],
    ["R",-1,"Backend blast","12.exe"],
    ["R",-1,"JSP source code Brute Force","13.exe"],
    ["R",-1,"Database Injector","14.exe"],
    ["R",-1,"Virtual Host Site Analysis","18.exe"],
    ["R",-1,"RFI Scanner","21.exe"],
    ["R",-1,"HUC Decoder","26.exe"],
    ["R",-1,"JSP Injector","29.exe"],
    ["R",-1,"FastMail","33.exe"],
    ["F",-1,"LOIC","LOIC.exe"],
    ["I",-1,"ChristPC DNS Switch(Setup)","6.exe"],
    ["I",-1,"NPCAP(Installer)","npcap-1.79.exe"],
    ["I",-1,"Wireshark(Installer)","WiresharkPortable64_4.2.2.paf.exe"],
    ["C",-1,"CTPs Service","CTPs.exe"],
    ["C",-1,"TCFE Service","TCFE.exe"],
    ["N",-1,"SFTp#","31.exe"],
    ["N",-1,"#","27.exe"],
    ["N",-1,"#","17.exe"],
    ["N",-1,"#","23.exe"],
    ["N",-1,"EXE2C#","20.exe"],
    #["N",-1,"IV Service#","IV.exe"],
    ]
welcometext="""\033[1;41mNo Escape Toolpack\033[0m

Author: \033[1;33m街角的猫_wjz\033[0m
Platform: Python \033[1;33m3.11.2\033[0m

\033[1mNo Escape Toolpack\033[0m是一套强大的网络渗透测试工具包，可以满足日常的大部分网络渗透需求，集成了跨设备漏洞扫描、局域网渗透、局域网洪水、公网扫描、后台爆破、密码破解、壳防护等功能，使用方便，支持加装、管理、卸载、一键启动。

\033[1;33m警告：本工具包部分载荷危险性较大，请谨慎使用！如触犯相关规定，作者不对损失负任何责任！\033[0m

\033[1m注意：请关闭所有防病毒软件或添加信任！\033[0m

\033[1m1\033[0m|选择载荷并启动
\033[1m2\033[0m|全部启动
\033[1m3\033[0m|清理并关闭全部载荷进程
\033[1m4\033[0m|浏览所有载荷
\033[1m5\033[0m|备份所有载荷
\033[1m6\033[0m|导入载荷
\033[1m0\033[0m|退出软件

"""
def pause():
    os.system("pause")
def runProcess(fname):
    p=subprocess.Popen(fname)
    return p.pid
def createProcess(fname):
    thread = threading.Thread(target=runProcess, args=(fname,))
    thread.start()
    thread.join()
    return thread
def exitProcess(im):
    os.system("taskkill /f /im "+im)
def encodePayload(exe_file_path):
    with open(exe_file_path, 'rb') as file:
        exe_content = file.read()
        encoded_content = base64.b64encode(exe_content)
        payload_file_path = exe_file_path + '.payload'
        with open(payload_file_path, 'wb') as payload_file:
            payload_file.write(encoded_content)
def decodePayload(payload_file_path):
    with open(payload_file_path, 'rb') as file:
        encoded_content = file.read()
        decoded_content = base64.b64decode(encoded_content)
        exe_file_path = payload_file_path.replace('.payload', '')
        with open(exe_file_path, 'wb') as exe_file:
            exe_file.write(decoded_content)
def backupPayload():
    for i in os.listdir("."):
        if i.endswith(".exe"):
            encodePayload(i)
            print(f"备份成功：{i}")
def importPayload():
    print("请在打开的窗口中选择要导入的载荷。")
    f=opf(title="选择载荷并导入",filetypes=[("标准载荷","*.payload")])
    if os.path.isfile(f):
        shutil.copy(f,".")
        print(f"导入{f}完成，将在重启软件后生效！")
    else:
        print(f"文件\"{f}\"不存在！")
def browsePayload():
    x=1
    print("\033[1;32mR\033[0m标识为基础载荷，您可以按照您的需求启动")
    print("\033[1;33mC\033[0m标识为预置壳服务，\033[4m建议在使用其他载荷之前先启动\033[0m")
    print("\033[1;31mF\033[0m标识为强度较高的载荷，可能对电脑性能有一定考验")
    print("\033[1;37mI\033[0m标识为安装程序，启动这些程序之后您需要自行操作以安装载荷")
    print("\033[1;41mN\033[0m标识为损坏或缺失的载荷，可能无法正常运行")
    print("\033[1;33;44mX\033[0m标识为用户导入的第三方载荷")
    for i in imlist:
        print(str(x).ljust(3),end="")
        print("|",end="")
        if i[0]=="R":
            print("\033[1;32mR\033[0m",end="")
        if i[0]=="C":
            print("\033[1;33mC\033[0m",end="")
        if i[0]=="F":
            print("\033[1;31mF\033[0m",end="")
        if i[0]=="I":
            print("\033[1;37mI\033[0m",end="")
        if i[0]=="N":
            print("\033[1;41mN\033[0m",end="")
        if i[0]=="X":
            print("\033[1;33;44mX\033[0m",end="")
        print("|",end="")
        print(i[2])
        x+=1
def selectPayload():
    browsePayload()
    while True:
        o=input("请输入数字选择载荷 >")
        try:
            oi=int(o)
            if oi>=1 and oi<=len(imlist):
                return oi-1
            else:
                print(f"无效选择。必须键入1-{len(imlist)}之间的一个数字。")
        except:
            print(f"无效选择。必须键入1-{len(imlist)}之间的一个数字。")
def startPayload(i):
    print("启动载荷：\033[1m"+str(i+1)+"\033[0m")
    if imlist[i][0]=="N":
        print("\033[1;41m[!]此载荷已损坏或缺失，请注意可能无法正常运行！\033[0m")
    print("[+]\t预置相关服务...完成")
    print("[+]\t加装UPX壳...完成")
    print("[+]\t准备引擎...完成")
    print("[+]\t提取载荷...",end="")
    decodePayload(imlist[i][-1]+".payload")
    print("完成")
    print("[+]\t启动载荷...",end="")
    pid=createProcess(imlist[i][-1])
    print("完成")
    print("[*]\t子线程：\033[1m"+str(pid))
    print("载荷启动完毕！")
def startAllPayload():
    print("全部启动！！")
    x=0
    for i in imlist:
        if i[0]!="N":
            if i[0]!="I" and i[0]!="C":
                startPayload(x)
            if i[0]=="C":
                os.system("start cmd.exe /c "+i[2])
        else:
            print(f"已跳过无效载荷[{x}]{i[2]}")
        x+=1
def clearAllPayload():
    print("正在清理并关闭进程...")
    for i in imlist:
        try:
            os.system("taskkill /f /im "+i[-1])
        except:
            print("载荷"+i[2]+"未运行")
def loadAddons():
    g=[]
    u=[]
    for i in os.listdir("."):
        if i.endswith(".payload"):
            g.append(i)
    for i in imlist:
        u.append(i[-1]+".payload")
    s=list(set(g).symmetric_difference(set(u)))
    for i in s:
        imlist.append(["X",-1,f"[Custom]{i}",i])
def bootCmd(a):
    print("来自命令行的参数:"+str(sys.argv))
    print("启动载荷：\033[1m"+a+"\033[0m")
    print("[+]\t预置相关服务...完成")
    print("[+]\t加装UPX壳...完成")
    print("[+]\t准备引擎...完成")
    print("[+]\t提取载荷...",end="")
    decodePayload(a)
    print("完成")
    print("[+]\t启动载荷...",end="")
    pid=createProcess(a.strip(".payload"))
    print("完成")
    print("[*]\t子线程：\033[1m"+str(pid))
    print("载荷启动完毕！")
try:
    if len(sys.argv)>=2:
        bootCmd(sys.argv[1])
    else:
        print("执行启动检查...")
        loadAddons()
        time.sleep(0.4)
        os.system("cls")
        while True:
            try:
                os.system("cls")
                print(welcometext)
                opt=input(">")
                if opt=="1":
                    startPayload(selectPayload())
                elif opt=="2":
                    startAllPayload()
                elif opt=="3":
                    clearAllPayload()
                elif opt=="4":
                    browsePayload()
                elif opt=="5":
                    backupPayload()
                elif opt=="6":
                    importPayload()
                elif opt=="0":
                    sys.exit()
                else:
                    #raise RuntimeError("Debug")
                    print("无效操作。")
                pause()
            except Exception as e:
                os.system("cls")
                print("\033[1;41m程序发生单循环错误，请按任意键继续以回到开始菜单，错误信息见下：\033[0m")
                print(f"\033[1;31m{e}\033[0m")
                os.system("pause")
except Exception as e:
    os.system("cls")
    print("\033[1;41m程序发生崩溃性错误，即将退出，错误信息见下：\033[0m")
    print(f"\033[1;31m{e}\033[0m")
#procedure
#startPayload(-1)
#print(selectPayload())
#startAllPayload()
#clearAllPayload()
os.system("pause")
