import base64
import os

def encode_exe_to_base64(exe_file_path):
    with open(exe_file_path, 'rb') as file:
        exe_content = file.read()
        encoded_content = base64.b64encode(exe_content)
        payload_file_path = exe_file_path + '.payload'
        with open("C:\\Users\\11252\\Desktop\\notepad.exe.payload", 'wb') as payload_file:
            payload_file.write(encoded_content)

def decode_base64_to_exe(payload_file_path):
    with open(payload_file_path, 'rb') as file:
        encoded_content = file.read()
        decoded_content = base64.b64decode(encoded_content)
        exe_file_path = payload_file_path.replace('.payload', '')
        with open(exe_file_path, 'wb') as exe_file:
            exe_file.write(decoded_content)
"""
# 测试编码
exe_file_path = '11.exe'
encode_exe_to_base64(exe_file_path)
"""
"""
for i in os.listdir("."):
    if i.endswith(".exe"):
        encode_exe_to_base64(i)
        print(i)
"""
encode_exe_to_base64("C:\\Windows\\notepad.exe")
"""
# 测试解码
payload_file_path = 'example.exe.payload'
decode_base64_to_exe(payload_file_path)
"""
