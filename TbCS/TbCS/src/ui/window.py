"""
窗口类
实现类似Windows的窗口功能
"""
from typing import List, Optional, Callable
from src.core.canvas import TerminalCanvas


class Window:
    """
    窗口类
    实现类似Windows的窗口功能，包括标题栏、内容区域、最小化/最大化/关闭按钮等
    """
    
    def __init__(self, canvas: TerminalCanvas, title: str = "", x: int = 5, y: int = 5, 
                 width: int = 40, height: int = 15, resizable: bool = True, 
                 always_on_top: bool = False):
        """
        初始化窗口
        
        Args:
            canvas: 终端画布对象
            title: 窗口标题
            x: 窗口X坐标
            y: 窗口Y坐标
            width: 窗口宽度
            height: 窗口高度
            resizable: 是否可调整大小
            always_on_top: 是否始终置顶
        """
        self.canvas = canvas
        self.title = title
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.resizable = resizable
        self.always_on_top = always_on_top
        
        # 窗口状态
        self.minimized = False
        self.maximized = False
        self.hidden = False
        self.focused = False
        
        # 窗口边界
        self.min_width = 20
        self.min_height = 8
        
        # 标题栏高度
        self.title_bar_height = 1
        
        # 按钮宽度
        self.button_width = 3
        
        # 注册到画布
        canvas.elements.append(self)
    
    def update(self):
        """更新窗口显示"""
        if self.hidden or self.minimized:
            return
            
        # 绘制窗口边框
        self._draw_border()
        
        # 绘制标题栏
        self._draw_title_bar()
        
        # 绘制内容区域
        self._draw_content_area()
    
    def _draw_border(self):
        """绘制窗口边框"""
        # 绘制窗口背景
        self.canvas.fill_rectangle(
            self.x, self.y, self.width, self.height,
            ' ', fg_color='', bg_color='\033[44m'  # 蓝色背景
        )
        
        # 绘制边框线条
        # 上下边框
        for i in range(self.width):
            self.canvas.draw_char(
                self.x + i, self.y, 
                '═' if i > 0 and i < self.width - 1 else ('╔' if i == 0 else '╗'),
                fg_color='\033[37m', bg_color='\033[44m'
            )
            self.canvas.draw_char(
                self.x + i, self.y + self.height - 1, 
                '═' if i > 0 and i < self.width - 1 else ('╚' if i == 0 else '╝'),
                fg_color='\033[37m', bg_color='\033[44m'
            )
        
        # 左右边框
        for i in range(1, self.height - 1):
            self.canvas.draw_char(self.x, self.y + i, '║', fg_color='\033[37m', bg_color='\033[44m')
            self.canvas.draw_char(self.x + self.width - 1, self.y + i, '║', fg_color='\033[37m', bg_color='\033[44m')
    
    def _draw_title_bar(self):
        """绘制标题栏"""
        # 标题栏背景
        for i in range(1, self.width - 1):
            self.canvas.draw_char(self.x + i, self.y, ' ', fg_color='\033[37m', bg_color='\033[44m')
        
        # 绘制标题文字
        title_max_len = self.width - 10  # 为按钮预留空间
        display_title = self.title[:title_max_len] if len(self.title) > title_max_len else self.title
        self.canvas.draw_text(self.x + 2, self.y, display_title, fg_color='\033[37m', bg_color='\033[44m')
        
        # 绘制控制按钮（如果空间足够）
        if self.width >= 15:  # 需要足够的空间来显示所有按钮
            right_pos = self.x + self.width - 3
            
            # 关闭按钮
            self.canvas.draw_text(right_pos, self.y, '×', fg_color='\033[37m', bg_color='\033[41m')
            right_pos -= self.button_width + 1
            
            # 最大化按钮
            self.canvas.draw_text(right_pos, self.y, '□', fg_color='\033[37m', bg_color='\033[42m')
            right_pos -= self.button_width + 1
            
            # 最小化按钮
            self.canvas.draw_text(right_pos, self.y, '_', fg_color='\033[37m', bg_color='\033[43m')
    
    def _draw_content_area(self):
        """绘制内容区域"""
        # 内容区域背景
        for row in range(self.title_bar_height, self.height - 1):
            for col in range(1, self.width - 1):
                self.canvas.draw_char(
                    self.x + col, self.y + row, 
                    ' ', fg_color='\033[37m', bg_color='\033[40m'  # 黑色背景
                )
        
        # 示例内容：显示窗口信息
        info_text = f"窗口: {self.title}"
        if self.x + 2 < self.x + self.width - 1 and self.y + 2 < self.y + self.height - 1:
            self.canvas.draw_text(self.x + 2, self.y + 2, info_text, fg_color='\033[37m', bg_color='\033[40m')
        
        pos_text = f"位置: ({self.x}, {self.y})"
        if self.x + 2 < self.x + self.width - 1 and self.y + 3 < self.y + self.height - 1:
            self.canvas.draw_text(self.x + 2, self.y + 3, pos_text, fg_color='\033[37m', bg_color='\033[40m')
        
        size_text = f"大小: {self.width}x{self.height}"
        if self.x + 2 < self.x + self.width - 1 and self.y + 4 < self.y + self.height - 1:
            self.canvas.draw_text(self.x + 2, self.y + 4, size_text, fg_color='\033[37m', bg_color='\033[40m')
    
    def move(self, new_x: int, new_y: int):
        """
        移动窗口到新位置
        
        Args:
            new_x: 新的X坐标
            new_y: 新的Y坐标
        """
        self.x = max(0, min(new_x, self.canvas.width - self.width))
        self.y = max(0, min(new_y, self.canvas.height - self.height))
    
    def resize(self, new_width: int, new_height: int):
        """
        调整窗口大小
        
        Args:
            new_width: 新宽度
            new_height: 新高度
        """
        if self.resizable:
            self.width = max(self.min_width, min(new_width, self.canvas.width - self.x))
            self.height = max(self.min_height, min(new_height, self.canvas.height - self.y))
    
    def minimize(self):
        """最小化窗口"""
        self.minimized = True
    
    def maximize(self):
        """最大化窗口"""
        if not self.maximized:
            self.x = 0
            self.y = 0
            self.width = self.canvas.width
            self.height = self.canvas.height
            self.maximized = True
        else:
            # 恢复之前的大小和位置
            self.maximized = False
    
    def close(self):
        """关闭窗口"""
        if self in self.canvas.elements:
            self.canvas.elements.remove(self)
    
    def show(self):
        """显示窗口"""
        self.hidden = False
    
    def hide(self):
        """隐藏窗口"""
        self.hidden = True
    
    def focus(self):
        """聚焦窗口（将其置于顶层）"""
        self.focused = True
        # 将窗口移到元素列表的末尾（表示在最顶层）
        if self in self.canvas.elements:
            self.canvas.elements.remove(self)
            self.canvas.elements.append(self)
    
    def unfocus(self):
        """取消聚焦"""
        self.focused = False