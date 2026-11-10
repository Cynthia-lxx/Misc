"""
终端画布模块
提供基础的终端绘制功能
"""
import os
import sys
import time
from typing import Tuple, List, Optional

try:
    import colorama
    from colorama import Fore, Back, Style
    colorama.init()
except ImportError:
    print("请安装colorama库: pip install colorama")
    sys.exit(1)


class TerminalCanvas:
    """
    终端画布类
    提供在终端中绘制图形的基础功能
    """
    
    def __init__(self, width: int = 80, height: int = 24, refresh_rate: float = 10.0):
        """
        初始化画布
        
        Args:
            width: 画布宽度（字符数）
            height: 画布高度（字符数）
            refresh_rate: 刷新率（Hz）
        """
        self.width = width
        self.height = height
        self.refresh_rate = refresh_rate
        self.refresh_interval = 1.0 / refresh_rate
        
        # 获取实际终端尺寸
        try:
            terminal_size = os.get_terminal_size()
            self.width = min(terminal_size.columns, width)
            self.height = min(terminal_size.lines - 1, height)  # 减1是为了避免滚动
        except OSError:
            # 如果无法获取终端尺寸，则使用默认值
            pass
        
        # 初始化画布缓冲区
        self.buffer = [[' ' for _ in range(self.width)] for _ in range(self.height)]
        
        # 初始化颜色缓冲区
        self.color_buffer = [[(Fore.WHITE, Back.BLACK, Style.NORMAL) 
                             for _ in range(self.width)] for _ in range(self.height)]
        
        # 存储待绘制的元素
        self.elements = []
        
        # 控制主循环的标志
        self.running = False
    
    def clear(self):
        """清空画布"""
        self.buffer = [[' ' for _ in range(self.width)] for _ in range(self.height)]
        self.color_buffer = [[(Fore.WHITE, Back.BLACK, Style.NORMAL) 
                             for _ in range(self.width)] for _ in range(self.height)]
    
    def draw_char(self, x: int, y: int, char: str, fg_color: str = Fore.WHITE, 
                  bg_color: str = Back.BLACK, style: str = Style.NORMAL):
        """
        在指定位置绘制单个字符
        
        Args:
            x: X坐标
            y: Y坐标
            char: 要绘制的字符
            fg_color: 前景色
            bg_color: 背景色
            style: 样式
        """
        if 0 <= x < self.width and 0 <= y < self.height:
            self.buffer[y][x] = char[0] if len(char) > 0 else ' '
            self.color_buffer[y][x] = (fg_color, bg_color, style)
    
    def draw_text(self, x: int, y: int, text: str, fg_color: str = Fore.WHITE, 
                  bg_color: str = Back.BLACK, style: str = Style.NORMAL):
        """
        在指定位置绘制文本
        
        Args:
            x: 起始X坐标
            y: Y坐标
            text: 要绘制的文本
            fg_color: 前景色
            bg_color: 背景色
            style: 样式
        """
        for i, char in enumerate(text):
            if x + i >= self.width:
                break
            self.draw_char(x + i, y, char, fg_color, bg_color, style)
    
    def draw_rectangle(self, x: int, y: int, width: int, height: int, 
                       border_char: str = '#', fg_color: str = Fore.WHITE, 
                       bg_color: str = Back.BLACK, style: str = Style.NORMAL):
        """
        绘制矩形（仅边框）
        
        Args:
            x: 左上角X坐标
            y: 左上角Y坐标
            width: 矩形宽度
            height: 矩形高度
            border_char: 边框字符
            fg_color: 前景色
            bg_color: 背景色
            style: 样式
        """
        # 绘制上下边框
        for i in range(width):
            if 0 <= x + i < self.width:
                if 0 <= y < self.height:
                    self.draw_char(x + i, y, border_char, fg_color, bg_color, style)
                if 0 <= y + height - 1 < self.height:
                    self.draw_char(x + i, y + height - 1, border_char, fg_color, bg_color, style)
        
        # 绘制左右边框
        for i in range(height):
            if 0 <= y + i < self.height:
                if 0 <= x < self.width:
                    self.draw_char(x, y + i, border_char, fg_color, bg_color, style)
                if 0 <= x + width - 1 < self.width:
                    self.draw_char(x + width - 1, y + i, border_char, fg_color, bg_color, style)
    
    def fill_rectangle(self, x: int, y: int, width: int, height: int, 
                       fill_char: str = ' ', fg_color: str = Fore.WHITE, 
                       bg_color: str = Back.BLACK, style: str = Style.NORMAL):
        """
        填充矩形区域
        
        Args:
            x: 左上角X坐标
            y: 左上角Y坐标
            width: 矩形宽度
            height: 矩形高度
            fill_char: 填充字符
            fg_color: 前景色
            bg_color: 背景色
            style: 样式
        """
        for row in range(height):
            for col in range(width):
                if 0 <= x + col < self.width and 0 <= y + row < self.height:
                    self.draw_char(x + col, y + row, fill_char, fg_color, bg_color, style)
    
    def render(self):
        """渲染画布到终端"""
        # 移动光标到左上角
        print('\033[H', end='')
        
        # 渲染每一行
        for y in range(self.height):
            for x in range(self.width):
                fg, bg, style = self.color_buffer[y][x]
                char = self.buffer[y][x]
                print(f'{style}{fg}{bg}{char}', end='')
            
            # 换行（最后一行不需要）
            if y < self.height - 1:
                print()
        
        # 清除从当前位置到行尾的所有内容
        print('\033[K', end='')
        # 清除从当前位置到屏幕末尾的所有内容
        print('\033[J', end='')
    
    def run(self):
        """运行主循环"""
        self.running = True
        last_render_time = time.time()
        
        try:
            # 隐藏光标
            print('\033[?25l', end='', flush=True)
            
            while self.running:
                current_time = time.time()
                
                # 控制刷新频率
                if current_time - last_render_time >= self.refresh_interval:
                    self.clear()
                    
                    # 更新所有元素
                    for element in self.elements:
                        if hasattr(element, 'update'):
                            element.update()
                    
                    # 渲染画布
                    self.render()
                    
                    last_render_time = current_time
                
                time.sleep(0.01)  # 小延迟以减少CPU使用
        except KeyboardInterrupt:
            print("\n程序被用户中断")
        finally:
            # 显示光标
            print('\033[?25h', end='', flush=True)
    
    def stop(self):
        """停止主循环"""
        self.running = False