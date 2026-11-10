"""
TbCS (Terminal-based Computer Simulator) - 主程序入口
"""

from src.core.canvas import TerminalCanvas
from src.ui.window import Window


def main():
    """
    主函数：启动TbCS模拟器
    """
    print("欢迎使用TbCS - 终端式电脑模拟器")
    print("正在初始化...")
    
    # 创建终端画布
    canvas = TerminalCanvas()
    
    # 初始化基本窗口系统
    window = Window(canvas, title="TbCS 欢迎界面", x=1, y=1, width=50, height=20)
    
    # 运行主循环
    canvas.run()


if __name__ == "__main__":
    main()