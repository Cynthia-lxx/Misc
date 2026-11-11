"""
Windows 11 DLL Functions Demo
A comprehensive demonstration of Windows DLL functions using ctypes
with rich formatting and type hints.
"""

import ctypes
import ctypes.wintypes
import sys
import platform
from datetime import datetime
from typing import Optional, List, Tuple
from rich.console import Console
from rich.table import Table
from rich.panel import Panel
from rich.progress import Progress, SpinnerColumn, TextColumn
from rich.layout import Layout
from rich.prompt import Prompt, Confirm
from rich import box
import typer

# Initialize Rich console and Typer app
console = Console()
app = typer.Typer(help="Windows 11 DLL Functions Demo", add_completion=False)

# Windows DLLs
user32 = ctypes.windll.user32
kernel32 = ctypes.windll.kernel32
gdi32 = ctypes.windll.gdi32
advapi32 = ctypes.windll.advapi32
shell32 = ctypes.windll.shell32
winmm = ctypes.windll.winmm
psapi = ctypes.windll.psapi
powrprof = ctypes.windll.powrprof
version = ctypes.windll.version

# Windows constants
SM_CXSCREEN = 0
SM_CYSCREEN = 1
SM_CXVIRTUALSCREEN = 78
SM_CYVIRTUALSCREEN = 79
SM_XVIRTUALSCREEN = 76
SM_YVIRTUALSCREEN = 77
SM_CMONITORS = 80

MB_OK = 0x00000000
MB_ICONINFORMATION = 0x00000040
MB_ICONWARNING = 0x00000030
MB_ICONERROR = 0x00000010

# Window styles
WS_OVERLAPPEDWINDOW = 0x00CF0000
WS_VISIBLE = 0x10000000
WS_POPUP = 0x80000000

# System metrics
SM_CYSCREEN = 1
SM_CXSCREEN = 0
SM_CMOUSEBUTTONS = 43
SM_CXCURSOR = 13
SM_CYCURSOR = 14


class WindowsDemos:
    """Class containing all Windows DLL demonstration functions"""
    
    @staticmethod
    def get_system_metrics() -> dict:
        """Get various system metrics using user32.dll"""
        metrics = {
            "Screen Width": user32.GetSystemMetrics(SM_CXSCREEN),
            "Screen Height": user32.GetSystemMetrics(SM_CYSCREEN),
            "Virtual Screen Width": user32.GetSystemMetrics(SM_CXVIRTUALSCREEN),
            "Virtual Screen Height": user32.GetSystemMetrics(SM_CYVIRTUALSCREEN),
            "Virtual Screen X": user32.GetSystemMetrics(SM_XVIRTUALSCREEN),
            "Virtual Screen Y": user32.GetSystemMetrics(SM_YVIRTUALSCREEN),
            "Number of Monitors": user32.GetSystemMetrics(SM_CMONITORS),
            "Mouse Buttons": user32.GetSystemMetrics(SM_CMOUSEBUTTONS),
            "Cursor Width": user32.GetSystemMetrics(SM_CXCURSOR),
            "Cursor Height": user32.GetSystemMetrics(SM_CYCURSOR),
        }
        return metrics
    
    @staticmethod
    def get_computer_name() -> str:
        """Get computer name using kernel32.dll"""
        buffer = ctypes.create_unicode_buffer(256)
        size = ctypes.c_ulong(256)
        if kernel32.GetComputerNameW(buffer, ctypes.byref(size)):
            return buffer.value
        return "Unknown"
    
    @staticmethod
    def get_windows_directory() -> str:
        """Get Windows directory using kernel32.dll"""
        buffer = ctypes.create_unicode_buffer(260)
        kernel32.GetWindowsDirectoryW(buffer, 260)
        return buffer.value
    
    @staticmethod
    def get_system_directory() -> str:
        """Get System directory using kernel32.dll"""
        buffer = ctypes.create_unicode_buffer(260)
        kernel32.GetSystemDirectoryW(buffer, 260)
        return buffer.value
    
    @staticmethod
    def get_tick_count() -> int:
        """Get system uptime in milliseconds using kernel32.dll"""
        return kernel32.GetTickCount64()
    
    @staticmethod
    def get_processor_count() -> int:
        """Get processor count using kernel32.dll"""
        return kernel32.GetActiveProcessorCount(0)
    
    @staticmethod
    def get_current_process_id() -> int:
        """Get current process ID using kernel32.dll"""
        return kernel32.GetCurrentProcessId()
    
    @staticmethod
    def get_current_thread_id() -> int:
        """Get current thread ID using kernel32.dll"""
        return kernel32.GetCurrentThreadId()
    
    @staticmethod
    def get_system_info() -> dict:
        """Get system information using kernel32.dll"""
        class SYSTEM_INFO(ctypes.Structure):
            _fields_ = [
                ("wProcessorArchitecture", ctypes.c_ushort),
                ("wReserved", ctypes.c_ushort),
                ("dwPageSize", ctypes.c_ulong),
                ("lpMinimumApplicationAddress", ctypes.c_void_p),
                ("lpMaximumApplicationAddress", ctypes.c_void_p),
                ("dwActiveProcessorMask", ctypes.c_ulong),
                ("dwNumberOfProcessors", ctypes.c_ulong),
                ("dwProcessorType", ctypes.c_ulong),
                ("dwAllocationGranularity", ctypes.c_ulong),
                ("wProcessorLevel", ctypes.c_ushort),
                ("wProcessorRevision", ctypes.c_ushort),
            ]
        
        sys_info = SYSTEM_INFO()
        kernel32.GetSystemInfo(ctypes.byref(sys_info))
        return {
            "Processor Architecture": sys_info.wProcessorArchitecture,
            "Page Size": sys_info.dwPageSize,
            "Number of Processors": sys_info.dwNumberOfProcessors,
            "Processor Type": sys_info.dwProcessorType,
            "Processor Level": sys_info.wProcessorLevel,
            "Processor Revision": sys_info.wProcessorRevision,
        }
    
    @staticmethod
    def get_global_memory_status() -> dict:
        """Get global memory status using kernel32.dll"""
        class MEMORYSTATUSEX(ctypes.Structure):
            _fields_ = [
                ("dwLength", ctypes.c_ulong),
                ("dwMemoryLoad", ctypes.c_ulong),
                ("ullTotalPhys", ctypes.c_ulonglong),
                ("ullAvailPhys", ctypes.c_ulonglong),
                ("ullTotalPageFile", ctypes.c_ulonglong),
                ("ullAvailPageFile", ctypes.c_ulonglong),
                ("ullTotalVirtual", ctypes.c_ulonglong),
                ("ullAvailVirtual", ctypes.c_ulonglong),
                ("ullAvailExtendedVirtual", ctypes.c_ulonglong),
            ]
        
        mem_status = MEMORYSTATUSEX()
        mem_status.dwLength = ctypes.sizeof(MEMORYSTATUSEX)
        if kernel32.GlobalMemoryStatusEx(ctypes.byref(mem_status)):
            return {
                "Memory Load (%)": mem_status.dwMemoryLoad,
                "Total Physical (MB)": mem_status.ullTotalPhys // (1024 * 1024),
                "Available Physical (MB)": mem_status.ullAvailPhys // (1024 * 1024),
                "Total Page File (MB)": mem_status.ullTotalPageFile // (1024 * 1024),
                "Available Page File (MB)": mem_status.ullAvailPageFile // (1024 * 1024),
                "Total Virtual (MB)": mem_status.ullTotalVirtual // (1024 * 1024),
                "Available Virtual (MB)": mem_status.ullAvailVirtual // (1024 * 1024),
            }
        return {}
    
    @staticmethod
    def play_beep(frequency: int = 800, duration: int = 200) -> bool:
        """Play a beep using kernel32.dll"""
        return kernel32.Beep(frequency, duration) != 0
    
    @staticmethod
    def show_message_box(title: str, message: str, style: int = MB_OK | MB_ICONINFORMATION) -> int:
        """Show a message box using user32.dll"""
        return user32.MessageBoxW(None, message, title, style)
    
    @staticmethod
    def get_key_state(virtual_key: int) -> int:
        """Get key state using user32.dll"""
        return user32.GetKeyState(virtual_key)
    
    @staticmethod
    def get_cursor_position() -> Tuple[int, int]:
        """Get cursor position using user32.dll"""
        class POINT(ctypes.Structure):
            _fields_ = [("x", ctypes.c_long), ("y", ctypes.c_long)]
        
        point = POINT()
        user32.GetCursorPos(ctypes.byref(point))
        return (point.x, point.y)
    
    @staticmethod
    def set_cursor_position(x: int, y: int) -> bool:
        """Set cursor position using user32.dll"""
        return user32.SetCursorPos(x, y) != 0
    
    @staticmethod
    def get_double_click_time() -> int:
        """Get double-click time using user32.dll"""
        return user32.GetDoubleClickTime()
    
    @staticmethod
    def get_user_name() -> str:
        """Get user name using advapi32.dll"""
        buffer = ctypes.create_unicode_buffer(256)
        size = ctypes.c_ulong(256)
        if advapi32.GetUserNameW(buffer, ctypes.byref(size)):
            return buffer.value
        return "Unknown"
    
    @staticmethod
    def get_desktop_wallpaper() -> str:
        """Get desktop wallpaper using user32.dll"""
        buffer = ctypes.create_unicode_buffer(260)
        user32.SystemParametersInfoW(0x0073, 260, buffer, 0)
        return buffer.value
    
    @staticmethod
    def get_work_area() -> dict:
        """Get work area using user32.dll"""
        class RECT(ctypes.Structure):
            _fields_ = [("left", ctypes.c_long), ("top", ctypes.c_long),
                       ("right", ctypes.c_long), ("bottom", ctypes.c_long)]
        
        rect = RECT()
        user32.SystemParametersInfoW(0x0030, 0, ctypes.byref(rect), 0)
        return {
            "Left": rect.left,
            "Top": rect.top,
            "Right": rect.right,
            "Bottom": rect.bottom,
            "Width": rect.right - rect.left,
            "Height": rect.bottom - rect.top,
        }
    
    @staticmethod
    def get_dpi_for_window() -> int:
        """Get DPI for window using user32.dll"""
        return user32.GetDpiForWindow(0)
    
    @staticmethod
    def get_device_caps() -> dict:
        """Get device capabilities using gdi32.dll"""
        hdc = user32.GetDC(0)
        caps = {
            "Horizontal Resolution": gdi32.GetDeviceCaps(hdc, 8),
            "Vertical Resolution": gdi32.GetDeviceCaps(hdc, 10),
            "Bits Per Pixel": gdi32.GetDeviceCaps(hdc, 12),
            "Color Planes": gdi32.GetDeviceCaps(hdc, 14),
            "Number of Colors": gdi32.GetDeviceCaps(hdc, 24),
            "Horizontal Size (mm)": gdi32.GetDeviceCaps(hdc, 4),
            "Vertical Size (mm)": gdi32.GetDeviceCaps(hdc, 6),
            "Aspect X": gdi32.GetDeviceCaps(hdc, 40),
            "Aspect Y": gdi32.GetDeviceCaps(hdc, 42),
            "Logical Pixels X": gdi32.GetDeviceCaps(hdc, 88),
            "Logical Pixels Y": gdi32.GetDeviceCaps(hdc, 90),
        }
        user32.ReleaseDC(0, hdc)
        return caps
    
    @staticmethod
    def get_volume_info() -> dict:
        """Get volume information using kernel32.dll"""
        class VOLUME_INFO(ctypes.Structure):
            _fields_ = [
                ("VolumeName", ctypes.c_wchar * 256),
                ("FileSystemName", ctypes.c_wchar * 256),
                ("SerialNumber", ctypes.c_ulong),
                ("MaxComponentLen", ctypes.c_ulong),
                ("FileSystemFlags", ctypes.c_ulong),
            ]
        
        volume_info = VOLUME_INFO()
        root_path = "C:\\"
        serial_number = ctypes.c_ulong()
        max_component_len = ctypes.c_ulong()
        file_system_flags = ctypes.c_ulong()
        
        success = kernel32.GetVolumeInformationW(
            root_path,
            volume_info.VolumeName,
            256,
            ctypes.byref(serial_number),
            ctypes.byref(max_component_len),
            ctypes.byref(file_system_flags),
            volume_info.FileSystemName,
            256
        )
        
        if success:
            return {
                "Volume Name": volume_info.VolumeName,
                "File System": volume_info.FileSystemName,
                "Serial Number": serial_number.value,
                "Max Component Length": max_component_len.value,
                "File System Flags": file_system_flags.value,
            }
        return {}
    
    @staticmethod
    def get_power_status() -> dict:
        """Get power status using kernel32.dll"""
        class SYSTEM_POWER_STATUS(ctypes.Structure):
            _fields_ = [
                ("ACLineStatus", ctypes.c_byte),
                ("BatteryFlag", ctypes.c_byte),
                ("BatteryLifePercent", ctypes.c_byte),
                ("Reserved1", ctypes.c_byte),
                ("BatteryLifeTime", ctypes.c_ulong),
                ("BatteryFullLifeTime", ctypes.c_ulong),
            ]
        
        status = SYSTEM_POWER_STATUS()
        if kernel32.GetSystemPowerStatus(ctypes.byref(status)):
            status_map = {
                "AC Line Status": status.ACLineStatus,
                "Battery Flag": status.BatteryFlag,
                "Battery Life Percent": status.BatteryLifePercent,
                "Battery Life Time (s)": status.BatteryLifeTime,
                "Battery Full Life Time (s)": status.BatteryFullLifeTime,
            }
            return status_map
        return {}
    
    @staticmethod
    def get_version_info() -> dict:
        """Get Windows version information using version.dll"""
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
        
        # Using RtlGetVersion which always returns correct version
        ntdll = ctypes.windll.ntdll
        status = ntdll.RtlGetVersion(ctypes.byref(version_info))
        
        if status == 0:
            return {
                "Major Version": version_info.dwMajorVersion,
                "Minor Version": version_info.dwMinorVersion,
                "Build Number": version_info.dwBuildNumber,
                "Platform ID": version_info.dwPlatformId,
                "Service Pack": f"{version_info.wServicePackMajor}.{version_info.wServicePackMinor}",
                "Product Type": version_info.wProductType,
            }
        return {}
    
    @staticmethod
    def enum_display_devices() -> List[dict]:
        """Enumerate display devices using user32.dll"""
        class DISPLAY_DEVICE(ctypes.Structure):
            _fields_ = [
                ("cb", ctypes.c_ulong),
                ("DeviceName", ctypes.c_wchar * 32),
                ("DeviceString", ctypes.c_wchar * 128),
                ("StateFlags", ctypes.c_ulong),
                ("DeviceID", ctypes.c_wchar * 128),
                ("DeviceKey", ctypes.c_wchar * 128),
            ]
        
        devices = []
        i = 0
        while True:
            dev = DISPLAY_DEVICE()
            dev.cb = ctypes.sizeof(DISPLAY_DEVICE)
            if user32.EnumDisplayDevicesW(None, i, ctypes.byref(dev), 0):
                devices.append({
                    "Name": dev.DeviceName,
                    "String": dev.DeviceString,
                    "State Flags": dev.StateFlags,
                })
                i += 1
            else:
                break
        return devices


def display_system_info():
    """Display system information in a formatted table"""
    demos = WindowsDemos()
    
    # Basic System Info
    table1 = Table(title="System Information", box=box.ROUNDED)
    table1.add_column("Property", style="cyan")
    table1.add_column("Value", style="green")
    
    table1.add_row("Computer Name", demos.get_computer_name())
    table1.add_row("User Name", demos.get_user_name())
    table1.add_row("Windows Directory", demos.get_windows_directory())
    table1.add_row("System Directory", demos.get_system_directory())
    table1.add_row("Processor Count", str(demos.get_processor_count()))
    table1.add_row("Process ID", str(demos.get_current_process_id()))
    table1.add_row("Thread ID", str(demos.get_current_thread_id()))
    table1.add_row("System Uptime", f"{demos.get_tick_count() // 3600000}h {(demos.get_tick_count() % 3600000) // 60000}m")
    table1.add_row("Double Click Time", f"{demos.get_double_click_time()} ms")
    table1.add_row("Desktop Wallpaper", demos.get_desktop_wallpaper())
    
    console.print(table1)
    
    # System Metrics
    metrics = demos.get_system_metrics()
    table2 = Table(title="System Metrics", box=box.ROUNDED)
    table2.add_column("Metric", style="cyan")
    table2.add_column("Value", style="yellow")
    
    for key, value in metrics.items():
        table2.add_row(key, str(value))
    
    console.print(table2)
    
    # Memory Status
    memory = demos.get_global_memory_status()
    if memory:
        table3 = Table(title="Memory Status", box=box.ROUNDED)
        table3.add_column("Memory Type", style="cyan")
        table3.add_column("Value (MB)", style="magenta")
        
        for key, value in memory.items():
            table3.add_row(key, str(value))
        
        console.print(table3)
    
    # Version Info
    version_info = demos.get_version_info()
    if version_info:
        table4 = Table(title="Windows Version", box=box.ROUNDED)
        table4.add_column("Version Property", style="cyan")
        table4.add_column("Value", style="blue")
        
        for key, value in version_info.items():
            table4.add_row(key, str(value))
        
        console.print(table4)


def display_system_advanced():
    """Display advanced system information"""
    demos = WindowsDemos()
    
    # System Info
    sys_info = demos.get_system_info()
    table1 = Table(title="Advanced System Information", box=box.ROUNDED)
    table1.add_column("Property", style="cyan")
    table1.add_column("Value", style="green")
    
    for key, value in sys_info.items():
        table1.add_row(key, str(value))
    
    console.print(table1)
    
    # Device Capabilities
    caps = demos.get_device_caps()
    table2 = Table(title="Device Capabilities", box=box.ROUNDED)
    table2.add_column("Capability", style="cyan")
    table2.add_column("Value", style="yellow")
    
    for key, value in caps.items():
        table2.add_row(key, str(value))
    
    console.print(table2)
    
    # Work Area
    work_area = demos.get_work_area()
    table3 = Table(title="Work Area", box=box.ROUNDED)
    table3.add_column("Property", style="cyan")
    table3.add_column("Value (pixels)", style="magenta")
    
    for key, value in work_area.items():
        table3.add_row(key, str(value))
    
    console.print(table3)
    
    # Power Status
    power = demos.get_power_status()
    if power:
        table4 = Table(title="Power Status", box=box.ROUNDED)
        table4.add_column("Status", style="cyan")
        table4.add_column("Value", style="blue")
        
        for key, value in power.items():
            table4.add_row(key, str(value))
        
        console.print(table4)
    
    # Volume Info
    volume = demos.get_volume_info()
    if volume:
        table5 = Table(title="Volume Information (C:)", box=box.ROUNDED)
        table5.add_column("Property", style="cyan")
        table5.add_column("Value", style="green")
        
        for key, value in volume.items():
            table5.add_row(key, str(value))
        
        console.print(table5)


def display_display_devices():
    """Display display devices information"""
    demos = WindowsDemos()
    devices = demos.enum_display_devices()
    
    if devices:
        table = Table(title="Display Devices", box=box.ROUNDED)
        table.add_column("Device", style="cyan")
        table.add_column("String", style="green")
        table.add_column("State Flags", style="yellow")
        
        for device in devices:
            table.add_row(device["Name"], device["String"], hex(device["State Flags"]))
        
        console.print(table)


def interactive_demo():
    """Interactive demonstration with user interaction"""
    demos = WindowsDemos()
    
    console.print(Panel.fit(
        "[bold cyan]Windows 11 Interactive Demos[/bold cyan]\n"
        "Try these safe interactive features!",
        border_style="blue"
    ))
    
    # Demo 1: Show Message Box
    if Confirm.ask("Show a message box?"):
        console.print("Displaying message box...")
        result = demos.show_message_box(
            "Windows 11 Demo",
            "Hello from Windows 11!\n\nThis is a demonstration of MessageBoxW using ctypes.",
            MB_OK | MB_ICONINFORMATION
        )
        console.print(f"[green]Message box closed with result: {result}[/green]")
    
    # Demo 2: Play Beep
    if Confirm.ask("Play a system beep?"):
        console.print("Playing beep...")
        if demos.play_beep(1000, 300):
            console.print("[green]Beep played successfully![/green]")
        else:
            console.print("[red]Failed to play beep[/red]")
    
    # Demo 3: Get Cursor Position
    if Confirm.ask("Get current cursor position?"):
        x, y = demos.get_cursor_position()
        console.print(f"[yellow]Cursor position: ({x}, {y})[/yellow]")
    
    # Demo 4: Play a short melody
    if Confirm.ask("Play a short melody (safe)?"):
        console.print("Playing melody...")
        notes = [(523, 200), (587, 200), (659, 200), (784, 300), (659, 200), (587, 200), (523, 400)]
        for freq, duration in notes:
            demos.play_beep(freq, duration)
        console.print("[green]Melody completed![/green]")


@app.command()
def show_all():
    """Display all system information"""
    console.print(Panel.fit(
        "[bold blue]Windows 11 DLL Functions Demo[/bold blue]",
        subtitle=f"Python {platform.python_version()} on {platform.system()} {platform.release()}",
        border_style="blue"
    ))
    
    with Progress(
        SpinnerColumn(),
        TextColumn("[progress.description]{task.description}"),
        transient=True,
    ) as progress:
        task = progress.add_task("[cyan]Loading system information...", total=None)
        
        # Display all information
        display_system_info()
        console.print()
        display_system_advanced()
        console.print()
        display_display_devices()
        console.print()
        
        # Keyboard shortcut information
        console.print(Panel(
            "[bold green]Keyboard Shortcuts:[/bold green]\n"
            "• [yellow]F1[/yellow] - Show help\n"
            "• [yellow]F2[/yellow] - Display system info\n"
            "• [yellow]F3[/yellow] - Display display devices\n"
            "• [yellow]F4[/yellow] - Show interactive demos\n"
            "• [yellow]Esc[/yellow] - Exit",
            border_style="green"
        ))
        
        progress.update(task, completed=True)
    
    # Interactive demos
    if Confirm.ask("Would you like to try some interactive demos?"):
        interactive_demo()


@app.command()
def basic():
    """Show basic system information only"""
    console.print(Panel.fit(
        "[bold blue]Basic System Information[/bold blue]",
        border_style="blue"
    ))
    display_system_info()


@app.command()
def advanced():
    """Show advanced system information only"""
    console.print(Panel.fit(
        "[bold blue]Advanced System Information[/bold blue]",
        border_style="blue"
    ))
    display_system_advanced()


@app.command()
def displays():
    """Show display devices information only"""
    console.print(Panel.fit(
        "[bold blue]Display Devices Information[/bold blue]",
        border_style="blue"
    ))
    display_display_devices()


@app.command()
def interactive():
    """Run interactive demos only"""
    interactive_demo()


@app.command()
def beep(frequency: int = typer.Argument(800, help="Beep frequency in Hz"),
         duration: int = typer.Argument(200, help="Beep duration in milliseconds")):
    """Play a system beep with specified frequency and duration"""
    demos = WindowsDemos()
    console.print(f"[yellow]Playing beep at {frequency}Hz for {duration}ms...[/yellow]")
    if demos.play_beep(frequency, duration):
        console.print("[green]Beep played successfully![/green]")
    else:
        console.print("[red]Failed to play beep[/red]")


@app.command()
def messagebox(title: str = typer.Argument("Windows 11 Demo", help="Message box title"),
               message: str = typer.Argument("Hello from Windows 11!", help="Message content")):
    """Show a message box with custom title and message"""
    demos = WindowsDemos()
    console.print("[cyan]Showing message box...[/cyan]")
    result = demos.show_message_box(title, message, MB_OK | MB_ICONINFORMATION)
    console.print(f"[green]Message box closed with result: {result}[/green]")


@app.command()
def cursor():
    """Get and display current cursor position"""
    demos = WindowsDemos()
    x, y = demos.get_cursor_position()
    console.print(f"[yellow]Cursor position: X={x}, Y={y}[/yellow]")
    
    # Get screen size for context
    screen_width = demos.get_system_metrics()["Screen Width"]
    screen_height = demos.get_system_metrics()["Screen Height"]
    console.print(f"[dim]Screen: {screen_width}x{screen_height}[/dim]")
    console.print(f"[dim]Percentage: {x/screen_width*100:.1f}% horizontal, {y/screen_height*100:.1f}% vertical[/dim]")


@app.command()
def systeminfo():
    """Show comprehensive system information including version"""
    console.print(Panel.fit(
        "[bold blue]Comprehensive System Information[/bold blue]",
        border_style="blue"
    ))
    
    display_system_info()
    console.print()
    display_system_advanced()
    
    # Additional system info
    demos = WindowsDemos()
    console.print(Panel(
        f"[bold]Additional Information:[/bold]\n"
        f"• Python Version: [cyan]{platform.python_version()}[/cyan]\n"
        f"• Architecture: [cyan]{platform.machine()}[/cyan]\n"
        f"• Processor: [cyan]{platform.processor()}[/cyan]\n"
        f"• System: [cyan]{platform.system()} {platform.release()}[/cyan]\n"
        f"• Node: [cyan]{platform.node()}[/cyan]\n"
        f"• DPI for current window: [cyan]{demos.get_dpi_for_window()}[/cyan]",
        border_style="green"
    ))


def main():
    """Main entry point for the CLI application"""
    if len(sys.argv) == 1:
        # Default behavior - show all
        app()
    else:
        app()


if __name__ == "__main__":
    main()
