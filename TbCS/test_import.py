"""
测试导入功能
"""
import os
import sys

# 添加System目录到路径
sys.path.append(os.path.join(os.path.dirname(__file__), '.'))

try:
    from System.TbUI import TerminalCanvas, Window, Button, TextBox, InputBox
    print("所有模块导入成功！")
    print("InputBox类已成功添加到TbUI模块中")
    
    # 简单测试InputBox实例化
    input_box = InputBox(10, 10, 20, "test", "placeholder")
    print(f"InputBox实例创建成功: {input_box.text}")
    
except ImportError as e:
    print(f"导入失败: {e}")
    
except Exception as e:
    print(f"创建实例时出错: {e}")