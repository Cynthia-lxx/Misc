import subprocess
import psutil
import os
import time
# 启动EXE文件
process = subprocess.Popen('11.exe')

# 获取PID
pid = process.pid

print("PID:", pid)
time.sleep(3)
os.system(f"taskkill /f /pid {pid}")
