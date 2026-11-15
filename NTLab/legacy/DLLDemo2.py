"""
Windows 11 Complete DLL Functions Demo - WORKING VERSION
Comprehensive demonstration including GDI, GUI, and advanced Windows APIs
"""

import ctypes
import ctypes.wintypes
import sys
import platform
import time
from typing import Optional, List, Tuple, Dict, Any
from rich.console import Console
from rich.table import Table
from rich.panel import Panel
from rich.prompt import Confirm
from rich import box
import typer

# Initialize Rich console and Typer app
console = Console()
app = typer.Typer(help="Windows 11 Complete DLL Functions Demo", add_completion=False)

# Windows DLLs
user32 = ctypes.windll.user32
kernel32 = ctypes.windll.kernel32
gdi32 = ctypes.windll.gdi32
shell32 = ctypes.windll.shell32
winmm = ctypes.windll.winmm
ntdll = ctypes.windll.ntdll

# Windows Constants
GDI_PS_SOLID = 0
GDI_PS_DASH = 1
GDI_PS_DOT = 2
GDI_PS_DASHDOT = 3
GDI_PS_DASHDOTDOT = 4

# Windows Messages
WM_PAINT = 0x000F
WM_DESTROY = 0x0002
WM_CLOSE = 0x0010
WM_KEYDOWN = 0x0100
WM_CREATE = 0x0001
WM_COMMAND = 0x0111

# Window Styles
WS_OVERLAPPEDWINDOW = 0x00CF0000
WS_VISIBLE = 0x10000000
WS_CHILD = 0x40000000
WS_BORDER = 0x00800000
WS_VSCROLL = 0x00200000
WS_EX_CLIENTEDGE = 0x00000200

# Class Styles
CS_HREDRAW = 0x0002
CS_VREDRAW = 0x0001
CS_OWNDC = 0x0020

# Color Constants
RGB = lambda r, g, b: (r | (g << 8) | (b << 16))
WHITE_BRUSH = 0
BLACK_BRUSH = 4

# Control styles
BS_PUSHBUTTON = 0x00000000
BS_AUTOCHECKBOX = 0x00000003
BS_AUTORADIOBUTTON = 0x00000009
ES_LEFT = 0x00000000
ES_AUTOVSCROLL = 0x00000040
SS_LEFT = 0x00000000

# Listbox messages
LB_ADDSTRING = 0x0180

# Button messages
BN_CLICKED = 0
HIWORD = lambda x: (x >> 16) & 0xFFFF
LOWORD = lambda x: x & 0xFFFF


class RECT(ctypes.Structure):
    _fields_ = [
        ("left", ctypes.c_long),
        ("top", ctypes.c_long),
        ("right", ctypes.c_long),
        ("bottom", ctypes.c_long)
    ]


class POINT(ctypes.Structure):
    _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]


class PAINTSTRUCT(ctypes.Structure):
    _fields_ = [
        ("hdc", ctypes.c_void_p),
        ("fErase", ctypes.c_bool),
        ("rcPaint", RECT),
        ("fRestore", ctypes.c_bool),
        ("fIncUpdate", ctypes.c_bool),
        ("rgbReserved", ctypes.c_byte * 32)
    ]


class MSG(ctypes.Structure):
    _fields_ = [
        ("hwnd", ctypes.c_void_p),
        ("message", ctypes.c_uint),
        ("wParam", ctypes.c_void_p),
        ("lParam", ctypes.c_void_p),
        ("time", ctypes.c_ulong),
        ("pt", POINT)
    ]


class WNDCLASSEX(ctypes.Structure):
    _fields_ = [
        ("cbSize", ctypes.c_uint),
        ("style", ctypes.c_uint),
        ("lpfnWndProc", ctypes.c_void_p),
        ("cbClsExtra", ctypes.c_int),
        ("cbWndExtra", ctypes.c_int),
        ("hInstance", ctypes.c_void_p),
        ("hIcon", ctypes.c_void_p),
        ("hCursor", ctypes.c_void_p),
        ("hbrBackground", ctypes.c_void_p),
        ("lpszMenuName", ctypes.c_wchar_p),
        ("lpszClassName", ctypes.c_wchar_p),
        ("hIconSm", ctypes.c_void_p)
    ]


class LOGFONT(ctypes.Structure):
    _fields_ = [
        ("lfHeight", ctypes.c_long),
        ("lfWidth", ctypes.c_long),
        ("lfEscapement", ctypes.c_long),
        ("lfOrientation", ctypes.c_long),
        ("lfWeight", ctypes.c_long),
        ("lfItalic", ctypes.c_byte),
        ("lfUnderline", ctypes.c_byte),
        ("lfStrikeOut", ctypes.c_byte),
        ("lfCharSet", ctypes.c_byte),
        ("lfOutPrecision", ctypes.c_byte),
        ("lfClipPrecision", ctypes.c_byte),
        ("lfQuality", ctypes.c_byte),
        ("lfPitchAndFamily", ctypes.c_byte),
        ("lfFaceName", ctypes.c_wchar * 32)
    ]


class WindowsCompleteDemos:
    """Complete Windows DLL demonstration class"""
    
    @staticmethod
    def get_system_metrics() -> dict:
        """Get all system metrics"""
        metrics = {}
        metric_ids = {
            "Screen Width": 0, "Screen Height": 1,
            "Virtual Screen Width": 78, "Virtual Screen Height": 79,
            "Virtual Screen X": 76, "Virtual Screen Y": 77,
            "Number of Monitors": 80, "Mouse Buttons": 43,
            "Cursor Width": 13, "Cursor Height": 14,
            "Border Width": 5, "Border Height": 6,
            "Caption Height": 4, "Caption Button Width": 54,
            "Caption Button Height": 55, "Menu Height": 15,
            "Scroll Width": 2, "Scroll Height": 3,
            "Small Icon Width": 49, "Small Icon Height": 50,
            "Large Icon Width": 11, "Large Icon Height": 12,
            "Mouse Present": 19, "Debug": 22, "Secure": 23,
        }
        
        for name, metric_id in metric_ids.items():
            metrics[name] = user32.GetSystemMetrics(metric_id)
        
        return metrics
    
    @staticmethod
    def get_device_caps_detailed() -> dict:
        """Get detailed device capabilities"""
        hdc = user32.GetDC(0)
        caps = {
            "Width": gdi32.GetDeviceCaps(hdc, 8),
            "Height": gdi32.GetDeviceCaps(hdc, 10),
            "Bits Per Pixel": gdi32.GetDeviceCaps(hdc, 12),
            "Color Planes": gdi32.GetDeviceCaps(hdc, 14),
            "Colors": gdi32.GetDeviceCaps(hdc, 24),
            "Horizontal Size (mm)": gdi32.GetDeviceCaps(hdc, 4),
            "Vertical Size (mm)": gdi32.GetDeviceCaps(hdc, 6),
            "Logical Pixels X": gdi32.GetDeviceCaps(hdc, 88),
            "Logical Pixels Y": gdi32.GetDeviceCaps(hdc, 90),
            "VRefresh": gdi32.GetDeviceCaps(hdc, 116),
        }
        user32.ReleaseDC(0, hdc)
        return caps
    
    @staticmethod
    def get_windows_info() -> dict:
        """Get Windows version and build information"""
        info = {}
        
        class OSVERSIONINFOEXW(ctypes.Structure):
            _fields_ = [
                ("dwOSVersionInfoSize", ctypes.c_ulong),
                ("dwMajorVersion", ctypes.c_ulong),
                ("dwMinorVersion", ctypes.c_ulong),
                ("dwBuildNumber", ctypes.c_ulong),
                ("dwPlatformId", ctypes.c_ulong),
                ("szCSDVersion", ctypes.c_wchar * 128),
                ("wServicePackMajor", ctypes.c_ushort),
                ("wServicePackMinor", ctypes.c_ushort),
                ("wSuiteMask", ctypes.c_ushort),
                ("wProductType", ctypes.c_byte),
                ("wReserved", ctypes.c_byte),
            ]
        
        version_info = OSVERSIONINFOEXW()
        version_info.dwOSVersionInfoSize = ctypes.sizeof(OSVERSIONINFOEXW)
        
        if ntdll.RtlGetVersion(ctypes.byref(version_info)) == 0:
            info["Major Version"] = version_info.dwMajorVersion
            info["Minor Version"] = version_info.dwMinorVersion
            info["Build Number"] = version_info.dwBuildNumber
            info["Platform ID"] = version_info.dwPlatformId
            info["Product Type"] = version_info.wProductType
        
        buffer = ctypes.create_unicode_buffer(260)
        kernel32.GetWindowsDirectoryW(buffer, 260)
        info["Windows Directory"] = buffer.value
        kernel32.GetSystemDirectoryW(buffer, 260)
        info["System Directory"] = buffer.value
        
        return info
    
    @staticmethod
    def get_system_colors() -> dict:
        """Get system colors"""
        colors = {}
        color_ids = {
            "Desktop": 1, "Window": 5, "Window Text": 8,
            "Menu": 4, "Menu Text": 7, "Active Title": 2,
            "Title Text": 9, "Button Face": 15, "Button Text": 18,
            "Highlight": 13, "Highlight Text": 14,
        }
        
        for name, color_id in color_ids.items():
            colors[name] = user32.GetSysColor(color_id)
        
        return colors
    
    @staticmethod
    def get_cursor_info() -> dict:
        """Get cursor information"""
        info = {}
        point = POINT()
        user32.GetCursorPos(ctypes.byref(point))
        info["Position X"] = point.x
        info["Position Y"] = point.y
        info["Blink Rate (ms)"] = user32.GetCaretBlinkTime()
        info["Cursor Width"] = user32.GetSystemMetrics(13)
        info["Cursor Height"] = user32.GetSystemMetrics(14)
        return info
    
    @staticmethod
    def get_desktop_info() -> dict:
        """Get desktop information"""
        info = {}
        desktop_hwnd = user32.GetDesktopWindow()
        info["Desktop Window Handle"] = desktop_hwnd
        
        rect = RECT()
        user32.GetWindowRect(desktop_hwnd, ctypes.byref(rect))
        info["Desktop Width"] = rect.right - rect.left
        info["Desktop Height"] = rect.bottom - rect.top
        
        buffer = ctypes.create_unicode_buffer(260)
        user32.SystemParametersInfoW(0x0073, 260, buffer, 0)
        info["Desktop Wallpaper"] = buffer.value
        
        return info
    
    @staticmethod
    def draw_graphics_examples(hdc: int, width: int, height: int):
        """Draw comprehensive graphics examples"""
        
        # Draw gradient background (using user32.FillRect)
        for i in range(height):
            color = int(100 + 155 * (i / height))
            brush = gdi32.CreateSolidBrush(RGB(100, color, 200))
            rect = RECT(0, i, width, i + 1)
            user32.FillRect(hdc, ctypes.byref(rect), brush)
            gdi32.DeleteObject(brush)
        
        # Draw colored circles
        colors = [RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), 
                 RGB(255, 255, 0), RGB(255, 0, 255), RGB(0, 255, 255)]
        
        for i, color in enumerate(colors):
            pen = gdi32.CreatePen(GDI_PS_SOLID, 2, color)
            old_pen = gdi32.SelectObject(hdc, pen)
            brush = gdi32.CreateSolidBrush(color)
            old_brush = gdi32.SelectObject(hdc, brush)
            
            cx = 50 + (i % 3) * 80
            cy = 50 + (i // 3) * 80
            radius = 30
            
            gdi32.Ellipse(hdc, cx - radius, cy - radius, cx + radius, cy + radius)
            
            gdi32.SelectObject(hdc, old_pen)
            gdi32.SelectObject(hdc, old_brush)
            gdi32.DeleteObject(pen)
            gdi32.DeleteObject(brush)
        
        # Draw lines with different styles
        line_styles = [GDI_PS_SOLID, GDI_PS_DASH, GDI_PS_DOT, GDI_PS_DASHDOT, GDI_PS_DASHDOTDOT]
        line_colors = [RGB(0, 0, 0), RGB(255, 0, 0), RGB(0, 255, 0), RGB(0, 0, 255), RGB(255, 255, 0)]
        
        for i, (style, color) in enumerate(zip(line_styles, line_colors)):
            pen = gdi32.CreatePen(style, 2, color)
            old_pen = gdi32.SelectObject(hdc, pen)
            
            x1 = 20 + i * 40
            y1 = height - 20
            x2 = 20 + i * 40
            y2 = height - 80
            
            gdi32.MoveToEx(hdc, x1, y1, None)
            gdi32.LineTo(hdc, x2, y2)
            
            gdi32.SelectObject(hdc, old_pen)
            gdi32.DeleteObject(pen)
        
        # Draw text with different fonts
        font_names = ["Arial", "Times New Roman", "Courier New", "Comic Sans MS"]
        for i, font_name in enumerate(font_names):
            lf = LOGFONT()
            lf.lfHeight = 16 + i * 4
            lf.lfWeight = 400 + i * 100
            lf.lfFaceName = font_name
            font = gdi32.CreateFontIndirectW(ctypes.byref(lf))
            old_font = gdi32.SelectObject(hdc, font)
            
            text = f"Font: {font_name}"
            gdi32.SetBkMode(hdc, 1)  # TRANSPARENT
            gdi32.SetTextColor(hdc, RGB(0, 0, 0))
            gdi32.TextOutW(hdc, 10 + i * 80, 10 + i * 30, text, len(text))
            
            gdi32.SelectObject(hdc, old_font)
            gdi32.DeleteObject(font)
    
    @staticmethod
    def create_window_with_graphics():
        """Create a window with GDI graphics"""
        global window_created
        
        # Define callback function type
        WNDPROC = ctypes.WINFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.c_uint, 
                                     ctypes.c_void_p, ctypes.c_void_p)
        
        def wnd_proc(hwnd, msg, wparam, lparam):
            if msg == WM_PAINT:
                ps = PAINTSTRUCT()
                hdc = user32.BeginPaint(hwnd, ctypes.byref(ps))
                
                rect = RECT()
                user32.GetClientRect(hwnd, ctypes.byref(rect))
                width = rect.right - rect.left
                height = rect.bottom - rect.top
                
                WindowsCompleteDemos.draw_graphics_examples(hdc, width, height)
                
                user32.EndPaint(hwnd, ctypes.byref(ps))
                return 0
            elif msg == WM_DESTROY:
                user32.PostQuitMessage(0)
                global window_created
                window_created = False
                return 0
            elif msg == WM_CLOSE:
                user32.DestroyWindow(hwnd)
                return 0
            elif msg == WM_KEYDOWN:
                if wparam == 0x1B:  # ESC key
                    user32.DestroyWindow(hwnd)
                return 0
            return user32.DefWindowProcW(hwnd, msg, wparam, lparam)
        
        wnd_proc_callback = WNDPROC(wnd_proc)
        
        class_name = "GraphicsDemoWindow"
        wc = WNDCLASSEX()
        wc.cbSize = ctypes.sizeof(WNDCLASSEX)
        wc.style = CS_HREDRAW | CS_VREDRAW
        wc.lpfnWndProc = ctypes.cast(wnd_proc_callback, ctypes.c_void_p)
        wc.hInstance = kernel32.GetModuleHandleW(None)
        wc.hCursor = user32.LoadCursorW(0, 32512)  # IDC_ARROW
        wc.hbrBackground = gdi32.GetStockObject(WHITE_BRUSH)
        wc.lpszClassName = class_name
        
        atom = user32.RegisterClassExW(ctypes.byref(wc))
        if not atom:
            console.print("[red]Failed to register window class![/red]")
            return False
        
        hwnd = user32.CreateWindowExW(
            0, class_name,
            "Windows 11 GDI Graphics Demo - Press ESC to close",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            100, 100, 800, 600,
            0, 0, wc.hInstance, 0
        )
        
        if not hwnd:
            console.print("[red]Failed to create window![/red]")
            return False
        
        window_created = True
        
        msg = MSG()
        while window_created:
            ret = user32.GetMessageW(ctypes.byref(msg), 0, 0, 0)
            if ret <= 0:
                break
            user32.TranslateMessage(ctypes.byref(msg))
            user32.DispatchMessageW(ctypes.byref(msg))
        
        return True
    
    @staticmethod
    def create_control_demo_window():
        """Create a window with various Windows controls"""
        global window_created
        
        WNDPROC = ctypes.WINFUNCTYPE(ctypes.c_int, ctypes.c_void_p, ctypes.c_uint, 
                                     ctypes.c_void_p, ctypes.c_void_p)
        
        def wnd_proc(hwnd, msg, wparam, lparam):
            if msg == WM_CREATE:
                h_instance = kernel32.GetModuleHandleW(None)
                
                # Button
                user32.CreateWindowExW(
                    0, "Button", "Click Me!",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    20, 20, 120, 30, hwnd, 100, h_instance, 0
                )
                
                # Checkbox
                user32.CreateWindowExW(
                    0, "Button", "Enable Feature",
                    WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
                    160, 20, 120, 30, hwnd, 101, h_instance, 0
                )
                
                # Radio buttons
                user32.CreateWindowExW(
                    0, "Button", "Option 1",
                    WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                    20, 60, 100, 25, hwnd, 102, h_instance, 0
                )
                user32.CreateWindowExW(
                    0, "Button", "Option 2",
                    WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                    140, 60, 100, 25, hwnd, 103, h_instance, 0
                )
                
                # Edit control
                user32.CreateWindowExW(
                    WS_EX_CLIENTEDGE, "Edit", "Type something here...",
                    WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL,
                    20, 100, 220, 60, hwnd, 104, h_instance, 0
                )
                
                # Static text
                user32.CreateWindowExW(
                    0, "Static", "Information Display:",
                    WS_CHILD | WS_VISIBLE | SS_LEFT,
                    20, 170, 150, 20, hwnd, 105, h_instance, 0
                )
                
                # Listbox
                h_listbox = user32.CreateWindowExW(
                    WS_EX_CLIENTEDGE, "ListBox", None,
                    WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
                    20, 200, 220, 100, hwnd, 106, h_instance, 0
                )
                
                items = ["Item 1", "Item 2", "Item 3", "Item 4", "Item 5", "Item 6"]
                for item in items:
                    user32.SendMessageW(h_listbox, LB_ADDSTRING, 0, item)
                
                return 0
            elif msg == WM_COMMAND:
                if HIWORD(wparam) == BN_CLICKED:
                    if LOWORD(wparam) == 100:
                        console.print("[green]Button clicked![/green]")
                        h_static = user32.GetDlgItem(hwnd, 105)
                        user32.SetWindowTextW(h_static, "Button was clicked!")
                return 0
            elif msg == WM_DESTROY:
                user32.PostQuitMessage(0)
                window_created = False
                return 0
            elif msg == WM_CLOSE:
                user32.DestroyWindow(hwnd)
                return 0
            elif msg == WM_KEYDOWN:
                if wparam == 0x1B:
                    user32.DestroyWindow(hwnd)
                return 0
            return user32.DefWindowProcW(hwnd, msg, wparam, lparam)
        
        wnd_proc_callback = WNDPROC(wnd_proc)
        
        class_name = "ControlDemoWindow"
        wc = WNDCLASSEX()
        wc.cbSize = ctypes.sizeof(WNDCLASSEX)
        wc.style = CS_HREDRAW | CS_VREDRAW
        wc.lpfnWndProc = ctypes.cast(wnd_proc_callback, ctypes.c_void_p)
        wc.hInstance = kernel32.GetModuleHandleW(None)
        wc.hCursor = user32.LoadCursorW(0, 32512)
        wc.hbrBackground = gdi32.GetStockObject(WHITE_BRUSH)
        wc.lpszClassName = class_name
        
        atom = user32.RegisterClassExW(ctypes.byref(wc))
        if not atom:
            console.print("[red]Failed to register window class![/red]")
            return False
        
        hwnd = user32.CreateWindowExW(
            0, class_name,
            "Windows 11 Controls Demo - Press ESC to close",
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            100, 100, 300, 400,
            0, 0, wc.hInstance, 0
        )
        
        if not hwnd:
            console.print("[red]Failed to create window![/red]")
            return False
        
        window_created = True
        
        msg = MSG()
        while window_created:
            ret = user32.GetMessageW(ctypes.byref(msg), 0, 0, 0)
            if ret <= 0:
                break
            user32.TranslateMessage(ctypes.byref(msg))
            user32.DispatchMessageW(ctypes.byref(msg))
        
        return True
    
    @staticmethod
    def play_sound_demo():
        """Play various Windows sounds"""
        try:
            sounds = [
                ("SystemAsterisk", "Asterisk"),
                ("SystemDefault", "Default"),
                ("SystemExclamation", "Exclamation"),
                ("SystemQuestion", "Question"),
                ("SystemStart", "Start"),
            ]
            
            for sound_id, name in sounds:
                winmm.PlaySoundW(sound_id, 0, 0x00020000)
                time.sleep(0.1)
                console.print(f"[yellow]Playing: {name}[/yellow]")
            
            return True
        except Exception as e:
            console.print(f"[red]Error playing sounds: {e}[/red]")
            return False


def display_complete_info():
    """Display comprehensive system information"""
    demos = WindowsCompleteDemos()
    
    console.print(Panel.fit(
        "[bold cyan]Windows 11 Complete System Information[/bold cyan]",
        border_style="blue"
    ))
    
    # System Metrics
    metrics = demos.get_system_metrics()
    table1 = Table(title="System Metrics", box=box.ROUNDED)
    table1.add_column("Metric", style="cyan")
    table1.add_column("Value", style="green")
    for key, value in metrics.items():
        table1.add_row(key, str(value))
    console.print(table1)
    
    # Windows Info
    win_info = demos.get_windows_info()
    table2 = Table(title="Windows Information", box=box.ROUNDED)
    table2.add_column("Property", style="cyan")
    table2.add_column("Value", style="yellow")
    for key, value in win_info.items():
        table2.add_row(key, str(value))
    console.print(table2)
    
    # System Colors
    colors = demos.get_system_colors()
    table3 = Table(title="System Colors", box=box.ROUNDED)
    table3.add_column("Color Name", style="cyan")
    table3.add_column("RGB Value", style="magenta")
    for name, color in colors.items():
        rgb = (color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF)
        table3.add_row(name, f"RGB({rgb[0]}, {rgb[1]}, {rgb[2]})")
    console.print(table3)
    
    # Cursor Info
    cursor_info = demos.get_cursor_info()
    table4 = Table(title="Cursor Information", box=box.ROUNDED)
    table4.add_column("Property", style="cyan")
    table4.add_column("Value", style="blue")
    for key, value in cursor_info.items():
        table4.add_row(key, str(value))
    console.print(table4)
    
    # Desktop Info
    desktop_info = demos.get_desktop_info()
    table5 = Table(title="Desktop Information", box=box.ROUNDED)
    table5.add_column("Property", style="cyan")
    table5.add_column("Value", style="green")
    for key, value in desktop_info.items():
        table5.add_row(key, str(value))
    console.print(table5)


@app.command()
def graphics():
    """Open a window with GDI graphics demonstration"""
    console.print(Panel.fit(
        "[bold green]GDI Graphics Demonstration[/bold green]\n"
        "Press [yellow]ESC[/yellow] to close the window.",
        border_style="green"
    ))
    
    demos = WindowsCompleteDemos()
    if Confirm.ask("Open graphics window?"):
        console.print("[cyan]Creating graphics window...[/cyan]")
        demos.create_window_with_graphics()
        console.print("[green]Graphics window closed![/green]")


@app.command()
def controls():
    """Open a window with Windows controls demonstration"""
    console.print(Panel.fit(
        "[bold blue]Windows Controls Demonstration[/bold blue]\n"
        "Press [yellow]ESC[/yellow] to close the window.",
        border_style="blue"
    ))
    
    demos = WindowsCompleteDemos()
    if Confirm.ask("Open controls window?"):
        console.print("[cyan]Creating controls window...[/cyan]")
        demos.create_control_demo_window()
        console.print("[green]Controls window closed![/green]")


@app.command()
def advanced():
    """Show advanced GDI and GUI information"""
    demos = WindowsCompleteDemos()
    
    console.print(Panel.fit(
        "[bold magenta]Advanced GDI and GUI Information[/bold magenta]",
        border_style="magenta"
    ))
    
    caps = demos.get_device_caps_detailed()
    table = Table(title="Device Capabilities", box=box.ROUNDED)
    table.add_column("Capability", style="cyan")
    table.add_column("Value", style="yellow")
    for key, value in caps.items():
        table.add_row(key, str(value))
    console.print(table)


@app.command()
def play_sounds():
    """Play various Windows system sounds"""
    console.print("[yellow]Playing Windows system sounds...[/yellow]")
    demos = WindowsCompleteDemos()
    demos.play_sound_demo()
    console.print("[green]Sounds played![/green]")


@app.command()
def all_demos():
    """Run all demos sequentially"""
    console.print(Panel.fit(
        "[bold cyan]Windows 11 Complete Demo Showcase[/bold cyan]",
        border_style="cyan"
    ))
    
    display_complete_info()
    
    console.print("\n[bold]Advanced GDI Information[/bold]")
    advanced()
    
    console.print("\n[bold]Interactive Demos[/bold]")
    demos = WindowsCompleteDemos()
    
    if Confirm.ask("Play Windows sounds?"):
        demos.play_sound_demo()
    
    if Confirm.ask("Open graphics window demo?"):
        demos.create_window_with_graphics()
    
    if Confirm.ask("Open controls window demo?"):
        demos.create_control_demo_window()
    
    console.print("[green]All demos completed![/green]")


@app.command()
def info():
    """Display comprehensive system information only"""
    display_complete_info()


def main():
    if len(sys.argv) == 1:
        console.print(Panel.fit(
            "[bold blue]Windows 11 Complete DLL Functions Demo[/bold blue]",
            subtitle=f"Python {platform.python_version()} on {platform.system()}",
            border_style="blue"
        ))
        app()
    else:
        app()


if __name__ == "__main__":
    main()
