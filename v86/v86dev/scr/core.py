try:
    import os
    import pyperclip
    os.system("cls")
    from flask import Flask, render_template_string
    import sys
    import time
    #raise Exception("Test")
    t0=time.time()
    print("Virtual OS server deployment core")
    print("Author: 街角的猫_wjz")
    print("Version:")
    print("\tPython \033[1;33m3.11.2\033[0m")
    print("\tFlask \033[1;33m3.0.3\033[0m")
    print("\tCore \033[1;33m1.0.1dev\033[0m")
    print("-"*36)
    try:
        profile=sys.argv[1]
    except:
        print("\033[0m\033[1;31mA profile required by the core wasn't found. Using Windows 95 as defaults...\033[0m")
        profile="windows95"
    try:
        lport=sys.argv[2]
        print("\033[0m\033[33mUsing custom port "+str(lport)+". Please be aware that port conflicts might happen.\033[0m")
    except:
        lport=8097
        print("\033[0m\033[33mUsing port 8097 as default.\033[0m")
    print("-"*36)
    print("["+str(round((time.time()-t0)*1000))+"] Creating Flask app.\nThe following output is from Flask.(stop=\"-\")")
    app = Flask(__name__)
    html_code = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>Virtual OS: """+profile+"""</title>
    </head>
    <body>
        <iframe src=\"https://copy.sh/v86/?profile="""+profile+"""\" style=\"width: 100%; height: 100vh; border: none;\"></iframe>
    </body>
    </html>
    """

    @app.route('/endpoint')
    def display_page():
        return render_template_string(html_code)

    if __name__ == '__main__':
        print("\033[0m\033[1;32mServer is running. To visit, please navigate \033[0m\033[1;7;33mlocalhost:"+str(lport)+"/endpoint\033[0m\033[1;32m. \033[0m\033[1;31m*(Don't add a slash to the end!!!)\033[0m")
        pyperclip.copy("localhost:"+str(lport)+"/endpoint")
        print("\033[0m\033[1;32m*Link copied to clipboard\033[0m")
        app.run(port=lport)
except Exception as e:
    import os
    print("\033[0m\033[1;31mA FATAL EXCEPTION HAS OCCURED. PROGRAM IS EXITING. THE EXCEPTION INFORMATION IS AS FOLLOWS:\033[0m")
    print(e)

