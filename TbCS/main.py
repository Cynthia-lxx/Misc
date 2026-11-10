"""
TbCS(Terminal-based Computer Simulator) - 主程序
"""
import os
import sys

# 添加System目录到路径
sys.path.append(os.path.join(os.path.dirname(__file__), '.'))

from System.TbUI import TerminalCanvas, Window, Button, TextBox


def demo_button_click():
    print("演示按钮被点击了！")


def main():
    """
    主函数：启动TbCS模拟器
    """
    print("欢迎使用TbCS - 终端式电脑模拟器")
    print("正在初始化...")
    
    # 创建终端画布
    canvas = TerminalCanvas()
    
    # 创建主窗口
    main_window = Window(5, 3, 50, 15, "TbCS 演示窗口")
    
    # 创建按钮
    button1 = Button(10, 8, "点击我!", 12, 3, demo_button_click)
    main_window.add_element(button1)
    
    # 创建文本框
    textbox1 = TextBox(10, 12, 30, 4, "这是一个\\n文本框示例")
    main_window.add_element(textbox1)
    
    canvas.add_window(main_window)
    
    # 创建第二个窗口
    second_window = Window(20, 8, 40, 10, "第二个窗口")
    button2 = Button(25, 12, "另一个按钮", 14, 3)
    second_window.add_element(button2)
    canvas.add_window(second_window)
    
    # 运行画布
    canvas.run()


if __name__ == "__main__":
    # 在Windows上清除屏幕
    os.system("cls")
    main()