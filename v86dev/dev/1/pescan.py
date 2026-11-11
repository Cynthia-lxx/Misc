import os
import time
t0=time.time()
l=[]
i=0
os.system("cls")
print("["+str(round(time.time()-t0))+"] \033[0m\033[1;33m准备校验SDK和PE镜像...")
with open("v.txt","r") as f:
    l=f.read().split("\n")
for o in l:
    i+=1
    d=o.strip("G:")
    print("[\033[0m\033[1;32mOK\033[0m]["+str(round((time.time()-t0)*1000))+"]["+str(i)+"]"+d)
print("\033[0m\033[1;32m核验通过\033[0m")
print("共校验了"+str(i)+"个文件")
print("用时"+str(round((time.time()-t0)*1000)/1000)+"秒")
print("速度"+str(round(i/(time.time()-t0),3))+"文件/秒")

os.system("pause")
