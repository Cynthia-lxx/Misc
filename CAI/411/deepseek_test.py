#!/usr/bin/env python3
"""
DeepSeek CLI 对话工具
直接在命令行与DeepSeek AI对话
"""

import requests
import json
import sys
import os
from pathlib import Path

class DeepSeekCLI:
    def __init__(self, api_key, model="deepseek-chat"):
        self.api_key = api_key
        self.model = model
        self.url = "https://api.deepseek.com/v1/chat/completions"
        self.conversation_history = []
        self.max_history = 20  # 保留最近20条对话
        
    def add_to_history(self, role, content):
        """添加对话到历史记录"""
        self.conversation_history.append({
            "role": role,
            "content": content
        })
        
        # 保持历史记录在限制范围内
        if len(self.conversation_history) > self.max_history:
            self.conversation_history = self.conversation_history[-self.max_history:]
    
    def clear_history(self):
        """清空对话历史"""
        self.conversation_history = []
        print("✓ 对话历史已清空")
    
    def send_message(self, user_input):
        """发送消息并获取回复"""
        # 添加用户消息到历史
        self.add_to_history("user", user_input)
        
        # 准备请求
        headers = {
            "Content-Type": "application/json",
            "Authorization": f"Bearer {self.api_key}"
        }
        
        payload = {
            "model": self.model,
            "messages": self.conversation_history,
            "temperature": 0.7,
            "max_tokens": 2000,
            "stream": True  # 启用流式输出
        }
        
        try:
            # 发送流式请求
            response = requests.post(
                self.url, 
                headers=headers, 
                json=payload, 
                stream=True,
                timeout=60
            )
            
            if response.status_code != 200:
                print(f"\n❌ 错误: {response.status_code}")
                print(response.text)
                return None
            
            print("\n🤖 DeepSeek: ", end="", flush=True)
            
            full_response = ""
            for line in response.iter_lines():
                if line:
                    line = line.decode('utf-8')
                    if line.startswith('data: '):
                        data = line[6:]
                        if data != '[DONE]':
                            try:
                                chunk = json.loads(data)
                                if 'choices' in chunk and chunk['choices']:
                                    content = chunk['choices'][0].get('delta', {}).get('content', '')
                                    if content:
                                        print(content, end='', flush=True)
                                        full_response += content
                            except json.JSONDecodeError:
                                pass
            
            print("\n")  # 额外换行
            # 添加AI回复到历史
            if full_response:
                self.add_to_history("assistant", full_response)
            
            return full_response
            
        except requests.exceptions.Timeout:
            print("\n❌ 请求超时，请重试")
            return None
        except requests.exceptions.ConnectionError:
            print("\n❌ 连接失败，请检查网络")
            return None
        except Exception as e:
            print(f"\n❌ 发生错误: {str(e)}")
            return None
    
    def show_help(self):
        """显示帮助信息"""
        print("\n" + "="*50)
        print("命令说明:")
        print("  /help     - 显示此帮助信息")
        print("  /clear    - 清空对话历史")
        print("  /history  - 查看对话历史")
        print("  /model    - 切换模型 (deepseek-chat/deepseek-coder)")
        print("  /quit 或 /exit - 退出程序")
        print("="*50 + "\n")
    
    def show_history(self):
        """显示对话历史"""
        if not self.conversation_history:
            print("\n📝 暂无对话历史\n")
            return
        
        print("\n" + "="*50)
        print("对话历史:")
        print("="*50)
        for i, msg in enumerate(self.conversation_history, 1):
            role = "👤 用户" if msg['role'] == 'user' else "🤖 DeepSeek"
            print(f"{i}. {role}: {msg['content'][:100]}{'...' if len(msg['content']) > 100 else ''}")
        print("="*50 + "\n")
    
    def switch_model(self):
        """切换模型"""
        print(f"\n当前模型: {self.model}")
        print("可选模型:")
        print("  1. deepseek-chat (通用对话)")
        print("  2. deepseek-coder (代码助手)")
        
        choice = input("请选择模型 (1/2): ").strip()
        if choice == "1":
            self.model = "deepseek-chat"
            print("✓ 已切换到 deepseek-chat 模型")
        elif choice == "2":
            self.model = "deepseek-coder"
            print("✓ 已切换到 deepseek-coder 模型")
        else:
            print("❌ 无效选择")
    
    def run(self):
        """运行主循环"""
        print("\n" + "="*50)
        print("🎯 DeepSeek CLI 对话工具")
        print("="*50)
        print(f"当前模型: {self.model}")
        self.show_help()
        
        while True:
            try:
                # 获取用户输入
                user_input = input("👤 你: ").strip()
                
                # 处理空输入
                if not user_input:
                    continue
                
                # 处理命令
                if user_input.startswith('/'):
                    cmd = user_input.lower()
                    if cmd in ['/quit', '/exit']:
                        print("\n👋 再见！")
                        break
                    elif cmd == '/help':
                        self.show_help()
                        continue
                    elif cmd == '/clear':
                        self.clear_history()
                        continue
                    elif cmd == '/history':
                        self.show_history()
                        continue
                    elif cmd == '/model':
                        self.switch_model()
                        continue
                    else:
                        print(f"❌ 未知命令: {user_input}")
                        print("输入 /help 查看可用命令")
                        continue
                
                # 发送消息并获取回复
                self.send_message(user_input)
                
            except KeyboardInterrupt:
                print("\n\n👋 再见！")
                break
            except EOFError:
                break
            except Exception as e:
                print(f"\n❌ 发生错误: {str(e)}")

def main():
    # 获取API密钥
    api_key = None
    
    # 优先从环境变量读取
    api_key = os.environ.get('DEEPSEEK_API_KEY')
    
    # 如果没有环境变量，尝试从配置文件读取
    if not api_key:
        config_file = Path.home() / '.deepseek_config.json'
        if config_file.exists():
            try:
                with open(config_file, 'r') as f:
                    config = json.load(f)
                    api_key = config.get('api_key')
            except:
                pass
    
    # 如果还是没有，提示用户输入
    if not api_key:
        print("="*50)
        print("DeepSeek CLI 对话工具")
        print("="*50)
        print("\n未找到API密钥")
        print("你可以:")
        print("  1. 设置环境变量: export DEEPSEEK_API_KEY='your-key'")
        print("  2. 创建配置文件: ~/.deepseek_config.json")
        print("  3. 现在手动输入")
        print()
        
        api_key = input("请输入你的DeepSeek API密钥: ").strip()
        
        if not api_key:
            print("❌ API密钥不能为空")
            return
        
        # 询问是否保存
        save = input("是否保存API密钥到配置文件？(y/n): ").strip().lower()
        if save == 'y':
            try:
                with open(config_file, 'w') as f:
                    json.dump({'api_key': api_key}, f)
                print(f"✓ API密钥已保存到 {config_file}")
            except Exception as e:
                print(f"⚠️  保存失败: {e}")
    
    # 选择模型
    print("\n选择模型:")
    print("  1. deepseek-chat (通用对话 - 推荐)")
    print("  2. deepseek-coder (代码助手)")
    model_choice = input("请选择 (1/2, 默认1): ").strip()
    
    model = "deepseek-chat"
    if model_choice == "2":
        model = "deepseek-coder"
    
    # 创建CLI实例并运行
    cli = DeepSeekCLI(api_key, model)
    cli.run()

if __name__ == "__main__":
    main()
