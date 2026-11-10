"""
TbCS 演示程序
展示基本的UI组件功能
"""
import time
import sys

# 添加项目根目录到Python路径
sys.path.append('.')

from src.core.canvas import TerminalCanvas
from src.ui.window import Window
from src.ui.button import Button


def demo_button_clicked():
    """演示按钮点击回调"""
    print("\n演示按钮被点击了！")
    time.sleep(0.5)  # 模拟处理时间


def main():
    """演示程序主函数"""
    print("TbCS 演示程序")
    print("正在启动...")
    
    # 创建终端画布
    canvas = TerminalCanvas(width=80, height=24, refresh_rate=10.0)
    
    # 创建主窗口
    main_window = Window(
        canvas, 
        title="TbCS 演示窗口", 
        x=2, y=2, 
        width=50, height=15, 
        resizable=True
    )
    
    # 创建一个按钮
    demo_button = Button(
        canvas,
        text="点击我!",
        x=5, y=5,
        width=12, height=3,
        on_click=demo_button_clicked,
        border_style='rounded'
    )
    
    # 创建第二个窗口作为对比
    second_window = Window(
        canvas,
        title="第二个窗口",
        x=25, y=8,
        width=40, height=12,
        resizable=False
    )
    
    # 创建第二个窗口上的按钮
    second_button = Button(
        canvas,
        text="另一个按钮",
        x=30, y=10,
        width=14, height=3,
        on_click=lambda: print("\\n第二个按钮被点击了!"),
        border_style='sharp'
    )
    
    print("演示程序已启动！请查看终端中的UI组件。")
    print("按 Ctrl+C 退出程序。")
    
    try:
        # 运行画布主循环
        canvas.run()
    except KeyboardInterrupt:
        print("\\n程序被用户中断，正在退出...")
        canvas.stop()


if __name__ == "__main__":
    main()