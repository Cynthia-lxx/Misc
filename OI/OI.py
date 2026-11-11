#!/usr/bin/env python3
"""
OI Code Generator CLI - 基于DeepSeek API的OI题目代码生成工具
"""

import os
import sys
import json
import re
from pathlib import Path
from typing import Optional, List
import click
from rich.console import Console
from rich.markdown import Markdown
from rich.syntax import Syntax
from rich.panel import Panel
from rich.prompt import Prompt, Confirm
from rich.table import Table
from rich.text import Text
from rich.style import Style
import pyperclip
from openai import OpenAI

# 初始化
console = Console()
CONFIG_FILE = Path.home() / ".oicoder_config.json"
HISTORY_FILE = Path.home() / ".oicoder_history.json"

# 默认配置（已包含API Key）
DEFAULT_CONFIG = {
    "api_key": "sk-e82625fc40f84e588312b3ddad5c8b70",
    "base_url": "https://api.deepseek.com/v1",
    "model": "deepseek-chat",
    "max_tokens": 8192,
    "temperature": 0.3,
}

class Config:
    def __init__(self):
        self.load()
    
    def load(self):
        if CONFIG_FILE.exists():
            with open(CONFIG_FILE, 'r') as f:
                self.data = json.load(f)
        else:
            self.data = DEFAULT_CONFIG.copy()
            self.save()
    
    def save(self):
        with open(CONFIG_FILE, 'w') as f:
            json.dump(self.data, f, indent=2)
    
    def get(self, key, default=None):
        return self.data.get(key, default)
    
    def set(self, key, value):
        self.data[key] = value
        self.save()

class History:
    def __init__(self):
        self.messages = []
        self.load()
    
    def load(self):
        if HISTORY_FILE.exists():
            with open(HISTORY_FILE, 'r') as f:
                self.messages = json.load(f)
    
    def save(self):
        with open(HISTORY_FILE, 'w') as f:
            json.dump(self.messages, f, indent=2)
    
    def add(self, role, content):
        self.messages.append({"role": role, "content": content})
        self.save()
    
    def clear(self):
        self.messages = []
        self.save()
    
    def get_context(self):
        # 只保留最近10轮对话
        return self.messages[-20:] if len(self.messages) > 20 else self.messages

config = Config()
history = History()

# 系统提示词
SYSTEM_PROMPT = """你是一个C++代码生成助手，专门为OI（信息学竞赛）题目生成代码。

用户的输入可能是题目描述，也可能是对已有代码的修改要求。

代码风格要求（极其重要）：
1. 保持程序的所有行为不变，除非用户明确要求修改
2. 使用 #include<bits/stdc++.h>
3. 不使用 using namespace std，而是对每个需要的std内容单独 using std::名称，一行一个
4. 变量命名：
   - 答案变量优先用 ans
   - 数组变量优先用 l
   - 循环索引优先用 i, j, k
   - 只用于占位而无用途的变量用 _
   - 其他变量尽量用单个字母表示功能（如 max_retries -> r/mr/mxr）
   - dp等专有名词保持原样
5. 调试输出统一用 cerr，但尽量不保留调试输出
6. gcd、lcm等小函数在开头用 inline 定义
7. 代码紧凑，不要多余换行（但分号后换行），空格适量保留美观
8. 移除所有非必要注释，除非变量用途不易理解，在定义后加 " //用途说明喵" 就可以了
9. 所有注释的结尾都要加上一个汉字"喵"
10. 大数组（10000+）用科学计数法+1145：int a[1e6+1145];
11. 同类型无初始值变量放一行：int m,s,n,dp[101][101];
    有初始值变量一行一个：int qmax=1007;
    多维数组适当换行缩进

请直接输出完整的C++14代码，不要有多余的解释。只有在用户明确询问时才解释。

如果用户说"继续"或输入中文文本，延续之前的对话上下文。"""

def get_client():
    """获取OpenAI客户端"""
    api_key = config.get("api_key")
    if not api_key:
        console.print(Text("错误: 未设置API Key，请使用 /setkey 设置", style="red"))
        return None
    
    base_url = config.get("base_url")
    return OpenAI(api_key=api_key, base_url=base_url)

def call_deepseek(user_input: str, is_new_topic: bool = True):
    """调用DeepSeek API"""
    client = get_client()
    if not client:
        return None
    
    messages = []
    if is_new_topic:
        messages.append({"role": "system", "content": SYSTEM_PROMPT})
        history.clear()
    
    # 添加历史上下文
    messages.extend(history.get_context())
    messages.append({"role": "user", "content": user_input})
    
    try:
        with console.status(Text("Processing...", style="bold green")):
            response = client.chat.completions.create(
                model=config.get("model", "deepseek-chat"),
                messages=messages,
                max_tokens=config.get("max_tokens", 8192),
                temperature=config.get("temperature", 0.3),
            )
        
        result = response.choices[0].message.content
        history.add("user", user_input)
        history.add("assistant", result)
        return result
    except Exception as e:
        console.print(Text(f"API call failed: {e}", style="red"))
        return None

def extract_code(text: str) -> Optional[str]:
    """从返回文本中提取代码"""
    # 匹配 ```cpp ... ``` 或 ```c++ ... ```
    pattern = r'```(?:cpp|c\+\+)\s*\n(.*?)\n```'
    matches = re.findall(pattern, text, re.DOTALL)
    if matches:
        return matches[0]
    
    # 如果没有代码块标记，尝试找#include开头的代码
    lines = text.split('\n')
    code_lines = []
    in_code = False
    for line in lines:
        if line.strip().startswith('#include'):
            in_code = True
        if in_code:
            code_lines.append(line)
    if code_lines:
        return '\n'.join(code_lines)
    
    return None

def get_last_code() -> Optional[str]:
    """获取最后生成的代码"""
    if history.messages:
        last = history.messages[-1]
        if last["role"] == "assistant":
            return extract_code(last["content"])
    return None

def resolve_save_path(path_str: str) -> Path:
    """解析保存路径"""
    path = Path(path_str)
    
    # 如果路径以 / 或 \ 开头，或者包含 :（Windows绝对路径），视为绝对路径
    if path.is_absolute():
        # 如果路径指向一个目录（没有扩展名或路径以 / 或 \ 结尾）
        if path_str.endswith(('/', '\\')) or not path.suffix:
            # 确保目录存在
            path.mkdir(parents=True, exist_ok=True)
            # 自动生成文件名
            filename = "solution.cpp"
            return path / filename
        else:
            # 完整文件路径
            path.parent.mkdir(parents=True, exist_ok=True)
            # 确保有 .cpp 扩展名
            if not path.suffix:
                path = path.with_suffix('.cpp')
            return path
    else:
        # 相对路径
        if path_str.endswith(('/', '\\')) or not path.suffix:
            # 是目录
            path.mkdir(parents=True, exist_ok=True)
            filename = "solution.cpp"
            return path / filename
        else:
            # 是文件路径
            path.parent.mkdir(parents=True, exist_ok=True)
            if not path.suffix:
                path = path.with_suffix('.cpp')
            return path

def display_code(code: str, title: str = "Code"):
    """显示带语法高亮的代码"""
    if code:
        syntax = Syntax(code, "cpp", theme="monokai", line_numbers=True)
        console.print(Panel(syntax, title=title, border_style="green"))
    else:
        console.print(Text("No code detected!", style="yellow"))

def display_text(content: str, title: str = "Response"):
    """显示纯文本内容"""
    console.print(Panel(content, title=title, border_style="blue"))

def handle_command(cmd: str) -> bool:
    """处理命令"""
    if cmd in ["/help", "/?"]:
        show_help()
        return True
    
    if cmd == "/setkey":
        key = Prompt.ask("Please Enter DeepSeek API Key", password=True)
        config.set("api_key", key)
        console.print(Text("API Key saved", style="green"))
        return True
    
    if cmd == "/clear":
        history.clear()
        console.print(Text("History cleared!", style="green"))
        return True
    
    if cmd == "/show":
        if history.messages:
            console.print(Text("--- History ---", style="cyan"))
            for msg in history.messages[-10:]:
                role = "User" if msg["role"] == "user" else "Agent"
                content = msg["content"][:200] + "..." if len(msg["content"]) > 200 else msg["content"]
                console.print(Text(f"{role}: {content}", style="bold" if role == "用户" else "green"))
        else:
            console.print(Text("Nothing here", style="yellow"))
        return True
    
    if cmd.startswith("/save"):
        # 解析参数
        parts = cmd.split(maxsplit=1)
        if len(parts) > 1:
            path_str = parts[1].strip()
        else:
            path_str = Prompt.ask("Please Enter Path", default="solution.cpp")
        
        code = get_last_code()
        if code:
            try:
                save_path = resolve_save_path(path_str)
                save_path.write_text(code)
                console.print(Text(f"Code saved to {save_path}", style="green"))
            except Exception as e:
                console.print(Text(f"Save failed: {e}", style="red"))
        else:
            console.print(Text("No code to save", style="yellow"))
        return True
    
    if cmd in ["/copy", "/cp"]:
        code = get_last_code()
        if code:
            pyperclip.copy(code)
            console.print(Text("Code copied!", style="green"))
        else:
            console.print(Text("No code to copy!", style="yellow"))
        return True
    
    if cmd in ["/clipboard", "/cb"]:
        try:
            content = pyperclip.paste()
            if not content.strip():
                console.print(Text("Clipboard empty!", style="yellow"))
                return True
            
            console.print(Text("Clipboard:", style="green"))
            preview = content[:500] + ("..." if len(content) > 500 else "")
            console.print(Panel(preview, title="Clipboard Preview", border_style="blue"))
            
            if Confirm.ask("Is it correct?"):
                result = call_deepseek(content, is_new_topic=True)
                if result:
                    code = extract_code(result)
                    if code:
                        display_code(code)
                        console.print(Text("\nComplete!", style="bold green"))
                        console.print(Text("Commands: /copy to copy, /save to save", style="dim"))
                    else:
                        display_text(result)
            return True
        except Exception as e:
            console.print(Text(f"Failed to read clipboard: {e}", style="red"))
            return True
    
    if cmd.startswith("/file"):
        parts = cmd.split(maxsplit=1)
        if len(parts) > 1:
            filename = parts[1].strip()
        else:
            filename = Prompt.ask("Please enter path")
        
        try:
            content = Path(filename).read_text(encoding='utf-8')
            console.print(Text(f"Read: {filename}", style="green"))
            preview = content[:500] + ("..." if len(content) > 500 else "")
            console.print(Panel(preview, title="Preview", border_style="blue"))
            if Confirm.ask("Is it correct?"):
                result = call_deepseek(content, is_new_topic=True)
                if result:
                    code = extract_code(result)
                    if code:
                        display_code(code)
                        console.print(Text("\nComplete!", style="bold green"))
                        console.print(Text("Commands: /copy to copy, /save to save", style="dim"))
                    else:
                        display_text(result)
            return True
        except Exception as e:
            console.print(Text(f"Failed to read: {e}", style="red"))
            return True
    
    if cmd in ["/exit", "/quit", "/q"]:
        console.print(Text("See you next time", style="yellow"))
        sys.exit(0)
    
    if cmd == "/config":
        table = Table(title="当前配置")
        table.add_column("配置项", style="cyan")
        table.add_column("值", style="green")
        for k, v in config.data.items():
            if k == "api_key":
                v = "*" * 8 if v else "(未设置)"
            table.add_row(k, str(v))
        console.print(table)
        return True
    
    if cmd == "/model":
        model = Prompt.ask("请输入模型名称", default=config.get("model"))
        config.set("model", model)
        console.print(Text(f"模型已设置为: {model}", style="green"))
        return True
    
    return False

def show_help():
    """显示帮助信息"""
    help_lines = [
        ("可用命令:", "bold cyan"),
        ("  /help, /?    显示此帮助信息", ""),
        ("  /setkey      设置DeepSeek API Key", ""),
        ("  /config      显示当前配置", ""),
        ("  /model       切换模型", ""),
        ("  /clear       清空对话历史", ""),
        ("  /show        显示最近的对话", ""),
        ("  /copy, /cp   复制最后生成的代码到剪贴板", ""),
        ("  /clipboard, /cb 从剪贴板读取题目", ""),
        ("  /save [路径] 保存代码到文件（自动补全.cpp扩展名）", ""),
        ("  /file [路径] 从文件读取题目", ""),
        ("  /exit, /quit, /q 退出程序", ""),
        ("", ""),
        ("使用方式:", "bold yellow"),
        ("  - 直接输入中文文本 → 发送给DeepSeek生成/修改代码", ""),
        ("  - 以 / 开头的命令 → 执行相应操作", ""),
        ("  - 首次使用请先 /setkey 设置API Key（已内置默认Key）", ""),
        ("", ""),
        ("保存路径示例:", "bold cyan"),
        ("  /save              → 保存为 solution.cpp", ""),
        ("  /save P1007        → 保存为 P1007.cpp", ""),
        ("  /save ./src/       → 保存到 src/ 目录下，文件名为 solution.cpp", ""),
        ("  /save ./src/P1007  → 保存到 src/ 目录下，文件名为 P1007.cpp", ""),
        ("  /save C:/code/P1007 → 保存到 C:/code/P1007.cpp（自动创建目录）", ""),
    ]
    
    for line in help_lines:
        if len(line) == 2:
            label, style = line
            console.print(Text(label, style=style))
        else:
            console.print(line)

def main():
    """主程序入口"""
    # 创建标题
    title = Text("🐱 OI Code Helper CLI", style="bold cyan")
    subtitle = Text("OI Helper Agent", style="dim")
    console.print(Panel(Text.assemble(title, "\n", subtitle), border_style="cyan"))
    
    # 检查配置
    if not config.get("api_key"):
        console.print(Text("首次使用，请先设置API Key: /setkey", style="yellow"))
        console.print(Text("或访问 https://platform.deepseek.com/ 获取API Key\n", style="dim"))
    else:
        console.print(Text("API Key OK ✓", style="green"))
    
    console.print(Text("Enter /help to view available commands\n", style="dim"))
    
    is_new_topic = True
    
    while True:
        try:
            user_input = Prompt.ask(Text("OI>", style="bold cyan"), show_default=False)
            
            if not user_input.strip():
                continue
            
            # 处理命令
            if user_input.startswith('/'):
                if handle_command(user_input):
                    continue
                else:
                    console.print(Text(f"Invalid command: {user_input}, try /help", style="red"))
                    continue
            
            # 调用API
            result = call_deepseek(user_input, is_new_topic=is_new_topic)
            if result:
                # 提取并显示代码
                code = extract_code(result)
                if code:
                    display_code(code)
                    console.print(Text("\n/copy to copy, /save to save", style="dim"))
                else:
                    # 不是代码，显示纯文本
                    display_text(result)
            
            is_new_topic = False
            
        except KeyboardInterrupt:
            console.print(Text("\nPress Ctrl+C again to exit", style="yellow"))
            try:
                input()
            except KeyboardInterrupt:
                console.print(Text("\nSee you next time...", style="yellow"))
                sys.exit(0)
        except EOFError:
            console.print(Text("\nSee you next time...", style="yellow"))
            sys.exit(0)
        except Exception as e:
            console.print(Text(f"Error: {e}", style="red"))

if __name__ == "__main__":
    main()
