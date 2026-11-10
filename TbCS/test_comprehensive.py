"""
TbCS 综合功能测试程序
测试修复后的所有功能
"""
import os
import sys

# 添加System目录到路径
sys.path.append(os.path.join(os.path.dirname(__file__), '.'))

from System.TbUI import TerminalCanvas, Window, Button, TextBox, InputBox


def demo_button_click():
    print("演示按钮被点击了！")


def main():
    """
    综合功能测试程序
    """
    print("TbCS 综合功能测试程序")
    print("正在初始化...")
    
    # 创建终端画布
    canvas = TerminalCanvas()
    
    # 创建第一个窗口，包含各种控件
    main_window = Window(5, 3, 50, 15, "TbCS 综合测试窗口")
    
    # 创建按钮
    button1 = Button(10, 8, "点击我!", 12, 3, demo_button_click)
    main_window.add_element(button1)
    
    # 创建文本框
    textbox1 = TextBox(10, 12, 30, 4, "这是一个\\n文本框示例")
    main_window.add_element(textbox1)
    
    # 创建输入框
    inputbox1 = InputBox(10, 17, 25, "默认文本", "提示文字")
    main_window.add_element(inputbox1)
    
    canvas.add_window(main_window)
    
    # 创建第二个窗口用于测试
    second_window = Window(20, 8, 40, 12, "第二个测试窗口")
    button2 = Button(25, 12, "另一个按钮", 14, 3)
    textbox2 = TextBox(25, 16, 20, 3, "测试\\n文本")
    second_window.add_element(button2)
    second_window.add_element(textbox2)
    canvas.add_window(second_window)
    
    print("综合功能测试程序已启动！")
    print("- 拖动标题栏可以移动窗口")
    print("- 点击右上角'×'可以关闭窗口")
    print("- 点击'□'可以最大化窗口")
    print("- 点击'─'可以最小化窗口")
    print("- 拖动窗口边框可以调整窗口大小")
    print("- 拖动窗口右下角可以同时调整宽度和高度")
    print("- 点击窗口可将其置于顶层")
    print("- 测试按钮点击响应和视觉反馈")
    print("- 测试输入框功能")
    print("- 按'q'或ESC键退出程序")
    print("按回车键继续...")
    input()
    
    # 运行画布
    canvas.run()


if __name__ == "__main__":
    # 在Windows上清除屏幕
    os.system("cls")
    main()