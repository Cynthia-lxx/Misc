"""
TbUI (Terminal-based UI)
基于curses的终端UI组件库
"""
import curses
import time
from typing import List, Tuple, Optional, Dict, Any


class UIElement:
    """
    UI元素基类
    """
    def __init__(self, x: int, y: int, width: int, height: int):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.visible = True

    def draw(self, stdscr) -> None:
        """
        绘制UI元素
        """
        pass

    def handle_input(self, ch: int) -> bool:
        """
        处理输入
        返回是否消费了该输入
        """
        return False


class Button(UIElement):
    """
    按钮组件
    """
    def __init__(self, x: int, y: int, text: str, width: Optional[int] = None, 
                 height: int = 3, on_click=None):
        # 如果未指定宽度，则根据文本长度计算
        calc_width = width if width is not None else len(text) + 2
        super().__init__(x, y, calc_width, height)
        self.text = text
        self.on_click = on_click
        self.is_pressed = False
        self.hovered = False

    def draw(self, stdscr) -> None:
        if not self.visible:
            return

        # 绘制按钮边框
        for i in range(self.height):
            for j in range(self.width):
                if i == 0:  # 顶部边框
                    if j == 0:
                        char = '╭'
                    elif j == self.width - 1:
                        char = '╮'
                    else:
                        char = '─'
                elif i == self.height - 1:  # 底部边框
                    if j == 0:
                        char = '╰'
                    elif j == self.width - 1:
                        char = '╯'
                    else:
                        char = '─'
                elif j == 0 or j == self.width - 1:  # 左右边缘
                    char = '│'
                else:
                    char = ' '

                # 设置颜色
                attr = curses.A_NORMAL
                if self.hovered:
                    attr = curses.color_pair(2)  # 高亮色
                else:
                    attr = curses.color_pair(1)  # 默认色

                if 0 <= self.y + i < curses.LINES and 0 <= self.x + j < curses.COLS:
                    try:
                        stdscr.addch(self.y + i, self.x + j, char, attr)
                    except:
                        pass  # 避免越界错误

        # 绘制按钮文本（居中）
        text_start_x = self.x + (self.width - len(self.text)) // 2
        text_y = self.y + self.height // 2
        if 0 <= text_y < curses.LINES and 0 <= text_start_x < curses.COLS:
            try:
                stdscr.addstr(text_y, text_start_x, self.text, 
                             curses.A_BOLD | (curses.color_pair(2) if self.hovered else curses.color_pair(1)))
            except:
                pass  # 避免越界错误


class TextBox(UIElement):
    """
    文本框组件
    """
    def __init__(self, x: int, y: int, width: int, height: int, text: str = ""):
        super().__init__(x, y, width, height)
        self.text = text
        # 处理转义序列，将字符串中的\n转换为实际换行
        processed_text = text.replace('\\n', '\n')
        self.lines = processed_text.split('\n') if processed_text else []

    def draw(self, stdscr) -> None:
        if not self.visible:
            return

        # 绘制边框
        for i in range(self.height):
            for j in range(self.width):
                char = ' '
                attr = curses.color_pair(3)  # 文本框颜色

                if i == 0:  # 顶部
                    if j == 0:
                        char = '┌'
                    elif j == self.width - 1:
                        char = '┐'
                    else:
                        char = '─'
                elif i == self.height - 1:  # 底部
                    if j == 0:
                        char = '└'
                    elif j == self.width - 1:
                        char = '┘'
                    else:
                        char = '─'
                elif j == 0 or j == self.width - 1:  # 左右边缘
                    char = '│'
                else:
                    # 显示文本内容
                    line_idx = i - 1
                    col_idx = j - 1
                    if line_idx < len(self.lines) and col_idx < len(self.lines[line_idx]):
                        char = self.lines[line_idx][col_idx]
                    else:
                        char = ' '

                if 0 <= self.y + i < curses.LINES and 0 <= self.x + j < curses.COLS:
                    try:
                        stdscr.addch(self.y + i, self.x + j, char, attr)
                    except:
                        pass  # 避免越界错误

class InputBox(UIElement):
    """
    输入框组件
    """
    def __init__(self, x: int, y: int, width: int, text: str = "", placeholder: str = ""):
        super().__init__(x, y, width, 3)  # 固定高度为3
        self.text = text
        self.placeholder = placeholder
        self.cursor_pos = len(text)
        self.focused = False
        self.max_length = width - 2  # 减去边框

    def draw(self, stdscr) -> None:
        if not self.visible:
            return

        # 绘制边框
        for i in range(self.height):
            for j in range(self.width):
                char = ' '
                # 焦点状态使用不同颜色
                attr = curses.color_pair(5) if self.focused else curses.color_pair(3)

                if i == 0:  # 顶部
                    if j == 0:
                        char = '┌'
                    elif j == self.width - 1:
                        char = '┐'
                    else:
                        char = '─'
                elif i == self.height - 1:  # 底部
                    if j == 0:
                        char = '└'
                    elif j == self.width - 1:
                        char = '┘'
                    else:
                        char = '─'
                elif j == 0 or j == self.width - 1:  # 左右边缘
                    char = '│'
                else:
                    # 显示文本内容
                    idx = j - 1  # 减去左边框
                    if idx < len(self.text):
                        char = self.text[idx]
                    elif idx == len(self.text) and self.focused and len(self.text) < self.max_length:
                        # 显示光标
                        char = ' '
                        if self.cursor_pos == len(self.text):
                            attr |= curses.A_REVERSE
                    elif idx < len(self.placeholder) and len(self.text) == 0:
                        char = self.placeholder[idx]
                        attr |= curses.A_DIM

                if 0 <= self.y + i < curses.LINES and 0 <= self.x + j < curses.COLS:
                    try:
                        stdscr.addch(self.y + i, self.x + j, char, attr)
                    except:
                        pass  # 避免越界错误

    def handle_input(self, ch) -> bool:
        """
        处理键盘输入
        """
        if ch == curses.KEY_BACKSPACE or ch == 127 or ch == 8:  # 退格键
            if self.cursor_pos > 0:
                self.text = self.text[:self.cursor_pos-1] + self.text[self.cursor_pos:]
                self.cursor_pos -= 1
            return True
        elif ch == curses.KEY_DC:  # 删除键
            if self.cursor_pos < len(self.text):
                self.text = self.text[:self.cursor_pos] + self.text[self.cursor_pos+1:]
            return True
        elif ch == curses.KEY_LEFT:  # 左箭头
            if self.cursor_pos > 0:
                self.cursor_pos -= 1
            return True
        elif ch == curses.KEY_RIGHT:  # 右箭头
            if self.cursor_pos < len(self.text):
                self.cursor_pos += 1
            return True
        elif 32 <= ch <= 126 or ch in [9, 10, 13]:  # 可打印字符，制表符，换行符
            if len(self.text) < self.max_length:
                self.text = self.text[:self.cursor_pos] + chr(ch) + self.text[self.cursor_pos:]
                self.cursor_pos += 1
            return True
        return False

class Window:
    """
    窗口类
    """
    def __init__(self, x: int, y: int, width: int, height: int, title: str = ""):
        self.x = x
        self.y = y
        self.width = width
        self.height = height
        self.title = title
        self.elements: List[UIElement] = []
        self.element_original_positions = {}  # 存储元素相对于窗口的原始位置
        self.active = True
        self.dragging = False
        self.drag_offset_x = 0
        self.drag_offset_y = 0
        self.resizable = True
        self.minimized = False
        self.maximized = False
        self.resizing = False
        self.resizing_width = False
        self.resizing_height = False
        self.resize_start_x = 0
        self.resize_start_y = 0
        self.resize_start_width = 0
        self.resize_start_height = 0

    def add_element(self, element: UIElement):
        """
        添加UI元素到窗口
        """
        self.elements.append(element)
        # 记录元素相对于窗口的原始位置
        self.element_original_positions[element] = (element.x - self.x, element.y - self.y)

    def draw(self, stdscr) -> None:
        """
        绘制窗口及其所有元素
        """
        if not self.active:
            return

        # 绘制窗口边框
        for i in range(self.height):
            for j in range(self.width):
                char = ' '
                attr = curses.color_pair(4)  # 窗口颜色

                if i == 0:  # 顶部边框（标题栏）
                    if j == 0:
                        char = '╔'
                    elif j == self.width - 1:
                        char = '╗'
                    else:
                        # 首先检查是否是控制按钮位置（从右边开始预留4个字符位置：3个按钮+1个空格）
                        if self.width >= 5 and j >= self.width - 4:  # 控制按钮区域（预留4个字符：空格+3个按钮）
                            if j == self.width - 4:  # 预留空格
                                char = ' '
                            elif j == self.width - 3:  # 最小化按钮
                                char = '─'
                            elif j == self.width - 2:  # 最大化按钮
                                char = '□'
                            elif j == self.width - 1:  # 关闭按钮
                                char = '×'
                        else:
                            # 绘制标题文本（留出左侧1个和右侧4个字符空间）
                            title_pos = j - 1  # 留出左边框的空间
                            if title_pos < len(self.title) and j < self.width - 4:  # 确保标题不会覆盖按钮区域
                                char = self.title[title_pos]
                            else:
                                char = ' '
                elif i == self.height - 1:  # 底部边框
                    if j == 0:
                        char = '╚'
                    elif j == self.width - 1:
                        char = '╝'
                    else:
                        char = '═'
                elif j == 0 or j == self.width - 1:  # 左右边缘
                    char = '║'
                else:
                    # 内容区域
                    char = ' '

                if 0 <= self.y + i < curses.LINES and 0 <= self.x + j < curses.COLS:
                    try:
                        stdscr.addch(self.y + i, self.x + j, char, attr)
                    except:
                        pass  # 避免越界错误

        # 绘制窗口内的所有元素，调整它们的位置以跟随窗口
        for element in self.elements:
            # 保存原始位置
            orig_x, orig_y = element.x, element.y
            
            # 如果已记录相对位置，则根据窗口位置调整元素位置
            if element in self.element_original_positions:
                rel_x, rel_y = self.element_original_positions[element]
                element.x = self.x + rel_x
                element.y = self.y + rel_y
            
            element.draw(stdscr)
            
            # 恢复原始位置
            element.x, element.y = orig_x, orig_y

    def handle_mouse(self, mouse_event) -> bool:
        """
        处理鼠标事件
        """
        id, x, y, z, bstate = mouse_event
        
        # 检查是否在标题栏点击
        if (self.x <= x < self.x + self.width and 
            self.y <= y < self.y + 1):  # 标题栏在第一行
            
            # 检查是否点击了控制按钮（从右边起预留4个字符：空格+3个按钮）
            if x == self.x + self.width - 1:  # 关闭按钮 (×)
                if bstate & curses.BUTTON1_PRESSED:
                    self.active = False
                    return True
            elif x == self.x + self.width - 2:  # 最大化按钮 (□)
                if bstate & curses.BUTTON1_PRESSED:
                    if not self.maximized:
                        self.maximized = True
                        self.original_x, self.original_y = self.x, self.y
                        self.original_width, self.original_height = self.width, self.height
                        self.x, self.y = 0, 0
                        self.width, self.height = curses.COLS - 1, curses.LINES - 1
                    else:
                        self.maximized = False
                        self.x, self.y = self.original_x, self.original_y
                        self.width, self.height = self.original_width, self.original_height
                    return True
            elif x == self.x + self.width - 3:  # 最小化按钮 (─)
                if bstate & curses.BUTTON1_PRESSED:
                    self.minimized = not self.minimized
                    return True
            
            # 如果不是点击控制按钮，则进行拖拽
            if not (x >= self.x + self.width - 4):  # 不是在控制按钮区域（预留4个字符：空格+3个按钮）
                if bstate & curses.BUTTON1_PRESSED:
                    self.dragging = True
                    self.drag_offset_x = x - self.x
                    self.drag_offset_y = y - self.y
                    return True
                elif bstate & curses.BUTTON1_RELEASED:
                    self.dragging = False
                    return True
        
        # 检查是否在窗口右下角点击（用于调整大小）
        elif (self.x + self.width - 1 <= x <= self.x + self.width and
              self.y + self.height - 1 <= y <= self.y + self.height and self.resizable):
            if bstate & curses.BUTTON1_PRESSED:
                self.resizing = True
                self.resize_start_x = x
                self.resize_start_y = y
                self.resize_start_width = self.width
                self.resize_start_height = self.height
                return True
            elif bstate & curses.BUTTON1_RELEASED:
                self.resizing = False
                return True
        
        # 检查是否在窗口右边框点击（用于调整宽度）
        elif (self.x + self.width - 1 <= x <= self.x + self.width and
              self.y < y < self.y + self.height - 1 and self.resizable):
            if bstate & curses.BUTTON1_PRESSED:
                self.resizing_width = True
                self.resize_start_x = x
                self.resize_start_width = self.width
                return True
            elif bstate & curses.BUTTON1_RELEASED:
                self.resizing_width = False
                return True
        
        # 检查是否在窗口底边框点击（用于调整高度）
        elif (self.x < x < self.x + self.width - 1 and
              self.y + self.height - 1 <= y <= self.y + self.height and self.resizable):
            if bstate & curses.BUTTON1_PRESSED:
                self.resizing_height = True
                self.resize_start_y = y
                self.resize_start_height = self.height
                return True
            elif bstate & curses.BUTTON1_RELEASED:
                self.resizing_height = False
                return True
        
        # 检查是否点击了窗口内的按钮
        for element in self.elements:
            if isinstance(element, Button):
                if (element.x <= x < element.x + element.width and
                    element.y <= y < element.y + element.height):
                    if bstate & curses.BUTTON1_PRESSED:
                        element.is_pressed = True
                        element.hovered = True
                    elif bstate & curses.BUTTON1_RELEASED:
                        if element.is_pressed and element.on_click:
                            element.on_click()
                        element.is_pressed = False
                    # 保持悬停状态检测
                    element.hovered = (element.x <= x < element.x + element.width and
                                      element.y <= y < element.y + element.height)
                    return True  # 按钮事件已被处理
        
        return False

    def update_position(self, mouse_x: int, mouse_y: int):
        """
        更新窗口位置（在拖拽时）
        """
        # 处理调整窗口大小
        if self.resizing:
            # 同时调整宽度和高度
            new_width = max(10, self.resize_start_width + (mouse_x - self.resize_start_x))
            new_height = max(5, self.resize_start_height + (mouse_y - self.resize_start_y))
            
            # 检查是否有元素会超出窗口边界
            if self._would_elements_fit(new_width, new_height):
                self.width = new_width
                self.height = new_height
        elif self.resizing_width:
            # 只调整宽度
            new_width = max(10, self.resize_start_width + (mouse_x - self.resize_start_x))
            
            # 检查是否有元素会超出窗口边界
            if self._would_elements_fit(new_width, self.height):
                self.width = new_width
        elif self.resizing_height:
            # 只调整高度
            new_height = max(5, self.resize_start_height + (mouse_y - self.resize_start_y))
            
            # 检查是否有元素会超出窗口边界
            if self._would_elements_fit(self.width, new_height):
                self.height = new_height
        elif self.dragging:
            # 处理窗口拖拽
            self.x = mouse_x - self.drag_offset_x
            self.y = mouse_y - self.drag_offset_y
            # 确保窗口不会完全移出屏幕
            self.x = max(0, min(self.x, curses.COLS - self.width))
            self.y = max(0, min(self.y, curses.LINES - self.height))
        
        # 如果窗口被最大化，则不进行位置更新
        if self.maximized:
            self.x, self.y = 0, 0

    def _would_elements_fit(self, new_width: int, new_height: int) -> bool:
        """
        检查在新的窗口尺寸下，所有元素是否仍然适应窗口
        """
        # 计算内容区域的边界（排除标题栏和底部边框）
        content_start_y = self.y + 1  # 标题栏占用一行
        content_end_y = self.y + new_height - 1  # 底部边框上方
        
        for element, (rel_x, rel_y) in self.element_original_positions.items():
            # 计算元素在新窗口中的绝对位置
            abs_x = self.x + rel_x
            abs_y = self.y + rel_y
            
            # 计算元素的右边界和底边界
            element_right = abs_x + element.width
            element_bottom = abs_y + element.height
            
            # 检查元素是否完全在内容区域内
            # 内容区域范围：x方向[self.x, self.x + new_width)，y方向[content_start_y, content_end_y)
            if (abs_x < self.x or                    # 元素左边界超出窗口左边界
                element_right > self.x + new_width or  # 元素右边界超出窗口右边界
                abs_y < content_start_y or             # 元素上边界超出内容区域上边界（即标题栏下方）
                element_bottom > content_end_y):       # 元素下边界超出内容区域下边界（即底部边框上方）
                return False  # 有任何元素超出内容区域，则不适合
        
        return True


class TerminalCanvas:
    """
    终端画布类
    使用curses库实现高效的终端UI渲染
    """
    def __init__(self):
        self.windows: List[Window] = []
        self.running = False
        self.stdscr = None

    def add_window(self, window: Window):
        """
        添加窗口到画布
        """
        self.windows.append(window)

    def init_curses(self):
        """
        初始化curses环境
        """
        self.stdscr = curses.initscr()
        curses.start_color()
        curses.use_default_colors()
        
        # 初始化颜色对
        curses.init_pair(1, curses.COLOR_WHITE, curses.COLOR_BLUE)    # 按钮默认
        curses.init_pair(2, curses.COLOR_BLACK, curses.COLOR_CYAN)    # 按钮高亮
        curses.init_pair(3, curses.COLOR_WHITE, curses.COLOR_BLACK)   # 文本框
        curses.init_pair(4, curses.COLOR_WHITE, curses.COLOR_MAGENTA) # 窗口边框
        curses.init_pair(5, curses.COLOR_WHITE, curses.COLOR_YELLOW)  # 输入框焦点
        
        curses.noecho()
        curses.cbreak()
        self.stdscr.keypad(True)
        
        # 启用鼠标支持
        curses.mousemask(curses.ALL_MOUSE_EVENTS)
        
        # 隐藏光标
        try:
            curses.curs_set(0)
        except:
            pass  # 某些终端不支持隐藏光标

    def cleanup(self):
        """
        清理curses环境
        """
        if self.stdscr:
            self.stdscr.keypad(False)
            curses.nocbreak()
            curses.echo()
            curses.endwin()

    def run(self):
        """
        运行主循环
        """
        self.running = True
        self.init_curses()
        
        try:
            while self.running:
                # 清屏
                self.stdscr.clear()
                
                # 绘制所有窗口（按顺序，后面的在前面）
                for window in self.windows:
                    window.draw(self.stdscr)
                
                # 刷新显示
                self.stdscr.refresh()
                
                # 处理输入
                ch = self.stdscr.getch()
                
                if ch == ord('q') or ch == 27:  # 'q' 或 ESC 键退出
                    self.running = False
                elif ch == curses.KEY_MOUSE:
                    # 处理鼠标事件
                    try:
                        mouse_event = curses.getmouse()
                        clicked_window = None
                        
                        for window in reversed(self.windows):  # 从最上层窗口开始检测
                            if window.handle_mouse(mouse_event):
                                clicked_window = window
                                break
                        
                        # 如果点击了某个窗口（且不是控制按钮相关的操作），将其置于顶层
                        if clicked_window and not (clicked_window.minimized or not clicked_window.active):
                            # 将被点击的窗口移到列表末尾（最上层）
                            self.windows.remove(clicked_window)
                            self.windows.append(clicked_window)
                        
                        # 更新所有窗口位置（如果有拖拽或调整大小的窗口）
                        for window in self.windows:
                            if window.dragging or window.resizing or window.resizing_width or window.resizing_height:
                                _, x, y, _, _ = mouse_event
                                window.update_position(x, y)
                    except:
                        pass  # 忽略鼠标事件错误
                
                time.sleep(0.01)  # 控制帧率
        except KeyboardInterrupt:
            pass
        finally:
            self.cleanup()


# 示例使用
def demo_button_click():
    print("按钮被点击了！")


def main(stdscr):
    """
    示例主函数
    """
    # 创建画布
    canvas = TerminalCanvas()
    
    # 创建窗口1
    win1 = Window(5, 3, 50, 15, "TbCS 演示窗口")
    
    # 创建按钮
    button1 = Button(10, 8, "点击我!", 12, 3, demo_button_click)
    win1.add_element(button1)
    
    # 创建文本框
    textbox1 = TextBox(10, 12, 30, 4, "这是一个\n文本框示例")
    win1.add_element(textbox1)
    
    canvas.add_window(win1)
    
    # 创建窗口2
    win2 = Window(20, 8, 40, 10, "第二个窗口")
    button2 = Button(25, 12, "另一个按钮", 14, 3)
    win2.add_element(button2)
    canvas.add_window(win2)
    
    # 运行画布
    canvas.run()


if __name__ == "__main__":
    curses.wrapper(main)