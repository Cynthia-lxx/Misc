"""
TbCS 增强版演示程序
展示TbCS的完整功能
"""
import os
import sys

# 添加System目录到路径
sys.path.append(os.path.join(os.path.dirname(__file__), '.'))

from System.TbUI import TerminalCanvas, Window, Button, TextBox, InputBox


def demo_button_click():
    print("演示按钮被点击了！")


def exit_program(canvas):
    """退出程序的回调函数"""
    canvas.running = False


def main():
    """
    增强版演示程序主函数
    """
    print("TbCS 增强版演示程序")
    print("正在初始化...")
    
    # 创建终端画布
    canvas = TerminalCanvas()
    
    # 创建主窗口
    main_window = Window(5, 2, 60, 18, "TbCS 增强版演示")
    
    # 创建按钮
    button1 = Button(12, 8, "点击我!", 12, 3, demo_button_click)
    main_window.add_element(button1)
    
    # 创建退出按钮
    exit_btn = Button(35, 8, "退出", 8, 3, lambda: exit_program(canvas))
    main_window.add_element(exit_btn)
    
    # 创建文本框
    textbox1 = TextBox(12, 12, 35, 4, "欢迎使用\\nTbCS终端模拟器！\\n这是一个演示。")
    main_window.add_element(textbox1)
    
    canvas.add_window(main_window)
    
    # 创建功能说明窗口
    help_window = Window(35, 5, 50, 15, "功能说明")
    help_text = TextBox(37, 8, 45, 8, "TbCS (Terminal-based Computer Simulator)\\n\\n- 拖动标题栏移动窗口\\n- 点击×关闭窗口\\n- 点击□最大化窗口\\n- 点击─最小化窗口\\n- 拖动边框调整大小\\n- 点击窗口切换层级")
    help_window.add_element(help_text)
    canvas.add_window(help_window)
    
    # 创建输入测试窗口
    input_window = Window(10, 15, 45, 12, "输入测试")
    input_label = TextBox(12, 18, 20, 2, "输入框示例:")
    input_box = InputBox(12, 21, 25, "", "请输入文本...")
    input_window.add_element(input_label)
    input_window.add_element(input_box)
    canvas.add_window(input_window)
    
    print("TbCS 增强版演示已启动！")
    print("功能包括：窗口管理、按钮交互、文本显示、输入框等")
    print("按'q'或ESC键退出程序")
    
    # 运行画布
    canvas.run()


if __name__ == "__main__":
    # 在Windows上清除屏幕
    os.system("cls")
    main()