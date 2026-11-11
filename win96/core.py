try:
    import os
    import pyperclip
    os.system("cls")
    from flask import Flask, render_template_string
    import sys
    import time
    #raise Exception("Test")
    lport=8888
    t0=time.time()
    print("\033[1;7mWindows \"96\" server\033[0m")
    print("Author: 街角的猫_wjz")
    print("Version:")
    print("\tPython \033[1;33m3.11.2\033[0m")
    print("\tFlask \033[1;33m3.0.3\033[0m")
    print("\tCore \033[1;33m1.0.1dev\033[0m")
    print("-"*36)
    print("\033[1mWindows \"96\"\033[0m是一款类似于v86的虚拟系统。相比于v86，拥有更加便捷的启动方式和更快的速度。服务器将自动启动，并在启动后检测您的数据并自动进行文件校验操作。")
    print("访问方式与v86类似。Flask平台启动服务器之后（即当您见到\"\033[33mPress CTRL+C to quit\033[0m\"时），访问地址将自动复制到您的剪贴板。")
    print("请打开您的浏览器，并按Ctrl+V粘贴网址以访问。")
    print("要停止程序，请先关闭您的浏览器，然后在本窗口按Ctrl+C退出。")
    print("-"*36)
    print("正在启动Windows \"96\"服务。以下内容来自旧版v86 core输出...")
    print("-"*36)
    print("["+str(round((time.time()-t0)*1000))+"] Creating Flask app.\nThe following output is from Flask.(stop=\"-\")")
    app = Flask(__name__)
    html_code = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Windows 96 Server</title>
    </head>
    <body>
        <iframe src=\"https://windows96.net/\" style=\"width: 100%; height: 100vh; border: none;\"></iframe>
    </body>
    </html>
    """

    @app.route('/svr')
    def display_page():
        return render_template_string(html_code)

    if __name__ == '__main__':
        print("\033[0m\033[1;32mServer is running. To visit, please navigate \033[0m\033[1;7;33mlocalhost:"+str(lport)+"/svr\033[0m\033[1;32m. \033[0m\033[1;31m*(Don't add a slash to the end!!!)\033[0m")
        pyperclip.copy("localhost:"+str(lport)+"/svr")
        print("\033[0m\033[1;32m*Link copied to clipboard\033[0m")
        app.run(port=lport)
except Exception as e:
    import os
    print("\033[0m\033[1;31mA FATAL EXCEPTION HAS OCCURED. PROGRAM IS EXITING. THE EXCEPTION INFORMATION IS AS FOLLOWS:\033[0m")
    print(e)
    os.system("pause")

