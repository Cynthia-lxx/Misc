"""
按钮组件
实现可在终端中显示和交互的按钮
"""
from typing import Callable, Optional
from src.core.canvas import TerminalCanvas


class Button:
    """
    按钮组件
    可以指定边框样式、粗细、颜色、按钮上的文本、映射的快捷键等
    """
    
    def __init__(self, canvas: TerminalCanvas, text: str = "", x: int = 0, y: int = 0, 
                 width: Optional[int] = None, height: Optional[int] = None,
                 border_style: str = 'rounded', fg_color: str = '\033[37m', 
                 bg_color: str = '\033[44m', hover_bg_color: str = '\033[46m',
                 on_click: Optional[Callable] = None, shortcut: Optional[str] = None):
        """
        初始化按钮
        
        Args:
            canvas: 终端画布对象
            text: 按钮文本
            x: 按钮X坐标
            y: 按钮Y坐标
            width: 按钮宽度（如果不指定则根据文本长度计算）
            height: 按钮高度（如果不指定则设为3）
            border_style: 边框样式 ('rounded', 'sharp', 'simple')
            fg_color: 默认前景色
            bg_color: 默认背景色
            hover_bg_color: 悬停时的背景色
            on_click: 点击回调函数
            shortcut: 快捷键
        """
        self.canvas = canvas
        self.text = text
        self.x = x
        self.y = y
        self.border_style = border_style
        self.fg_color = fg_color
        self.bg_color = bg_color
        self.hover_bg_color = hover_bg_color
        self.on_click = on_click
        self.shortcut = shortcut
        
        # 计算按钮尺寸
        text_width = len(text) + 2  # 加2是为了给边距
        self.width = width if width is not None else max(text_width, 6)
        self.height = height if height is not None else 3
        
        # 按钮状态
        self.hovered = False
        self.pressed = False
        
        # 注册到画布
        canvas.elements.append(self)
    
    def update(self):
        """更新按钮显示"""
        # 根据状态选择颜色
        current_bg_color = self.hover_bg_color if self.hovered else self.bg_color
        if self.pressed:
            # 按下的效果
            current_bg_color = '\033[42m'  # 使用绿色表示按下状态
        
        # 绘制按钮边框和背景
        self._draw_button(current_bg_color)
        
        # 绘制按钮文本
        self._draw_text()
    
    def _draw_button(self, bg_color: str):
        """绘制按钮边框和背景"""
        # 根据边框样式选择边框字符
        if self.border_style == 'rounded':
            corner_tl, corner_tr, corner_bl, corner_br = '╭', '╮', '╰', '╯'
            h_line, v_line = '─', '│'
        elif self.border_style == 'sharp':
            corner_tl, corner_tr, corner_bl, corner_br = '┌', '┐', '└', '┘'
            h_line, v_line = '─', '│'
        else:  # simple
            corner_tl, corner_tr, corner_bl, corner_br = '+', '+', '+', '+'
            h_line, v_line = '-', '|'
        
        # 绘制上下边框
        for i in range(self.width):
            if i == 0:
                self.canvas.draw_char(
                    self.x + i, self.y, 
                    corner_tl, 
                    fg_color=self.fg_color, bg_color=bg_color
                )
                self.canvas.draw_char(
                    self.x + i, self.y + self.height - 1, 
                    corner_bl, 
                    fg_color=self.fg_color, bg_color=bg_color
                )
            elif i == self.width - 1:
                self.canvas.draw_char(
                    self.x + i, self.y, 
                    corner_tr, 
                    fg_color=self.fg_color, bg_color=bg_color
                )
                self.canvas.draw_char(
                    self.x + i, self.y + self.height - 1, 
                    corner_br, 
                    fg_color=self.fg_color, bg_color=bg_color
                )
            else:
                self.canvas.draw_char(
                    self.x + i, self.y, 
                    h_line, 
                    fg_color=self.fg_color, bg_color=bg_color
                )
                self.canvas.draw_char(
                    self.x + i, self.y + self.height - 1, 
                    h_line, 
                    fg_color=self.fg_color, bg_color=bg_color
                )
        
        # 绘制左右边框
        for i in range(1, self.height - 1):
            self.canvas.draw_char(
                self.x, self.y + i, 
                v_line, 
                fg_color=self.fg_color, bg_color=bg_color
            )
            self.canvas.draw_char(
                self.x + self.width - 1, self.y + i, 
                v_line, 
                fg_color=self.fg_color, bg_color=bg_color
            )
        
        # 填充内部
        for row in range(1, self.height - 1):
            for col in range(1, self.width - 1):
                self.canvas.draw_char(
                    self.x + col, self.y + row, 
                    ' ', 
                    fg_color=self.fg_color, bg_color=bg_color
                )
    
    def _draw_text(self):
        """绘制按钮文本"""
        # 计算文本居中位置
        text_length = len(self.text)
        if text_length >= self.width - 2:  # 减2是为边框留出空间
            # 如果文本太长，截断文本
            display_text = self.text[:self.width - 5] + "..."
        else:
            display_text = self.text
        
        # 计算水平居中位置
        start_x = self.x + (self.width - text_length) // 2
        # 计算垂直居中位置
        start_y = self.y + self.height // 2
        
        # 绘制文本
        self.canvas.draw_text(
            start_x, start_y, 
            display_text, 
            fg_color=self.fg_color, 
            bg_color=self.hover_bg_color if self.hovered else self.bg_color
        )
    
    def check_collision(self, x: int, y: int) -> bool:
        """
        检查坐标是否在按钮范围内
        
        Args:
            x: X坐标
            y: Y坐标
            
        Returns:
            bool: 是否碰撞
        """
        return (
            self.x <= x < self.x + self.width and
            self.y <= y < self.y + self.height
        )
    
    def handle_mouse_event(self, event_type: str, x: int, y: int):
        """
        处理鼠标事件
        
        Args:
            event_type: 事件类型 ('hover', 'click', 'press', 'release')
            x: 鼠标X坐标
            y: 鼠标Y坐标
        """
        if self.check_collision(x, y):
            if event_type == 'hover':
                self.hovered = True
            elif event_type == 'press':
                self.pressed = True
            elif event_type == 'release':
                self.pressed = False
                if self.on_click:
                    self.on_click()
            elif event_type == 'click':
                if self.on_click:
                    self.on_click()
        else:
            # 鼠标不在按钮上
            if event_type in ['hover']:
                self.hovered = False
            elif event_type == 'release':
                self.pressed = False
    
    def handle_key_event(self, key: str):
        """
        处理键盘事件
        
        Args:
            key: 按键
        """
        if self.shortcut and key.lower() == self.shortcut.lower():
            if self.on_click:
                self.on_click()
    
    def set_position(self, x: int, y: int):
        """
        设置按钮位置
        
        Args:
            x: X坐标
            y: Y坐标
        """
        self.x = x
        self.y = y
    
    def set_text(self, text: str):
        """
        设置按钮文本
        
        Args:
            text: 新文本
        """
        self.text = text