#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
TUF Letter Guessing Game - Host/Player System
开字母出题与游戏程序
"""

import json
import random
import re
import os
from pathlib import Path
from typing import List, Dict, Tuple, Optional
import sys


class LetterGuessingGame:
    def __init__(self):
        self.songs_db = []
        self.current_game = {
            "guessed_chars": set(),  # 已猜的字符（包括字母、数字、符号等）
            "revealed_songs": set(),  # 已揭晓的歌曲索引（从1开始）
            "patterns": [],  # 当前显示的模式
            "answers": [],   # 正确答案 [(name, artist, aliases), ...]
            "round": 0
        }
        self.load_database()
    
    def load_database(self):
        """加载歌曲数据库"""
        db_file = "tuf_songs.json"
        if not os.path.exists(db_file):
            print(f"❌ 错误：找不到数据库文件 {db_file}")
            print("请先运行 fetch_tuf_songs.py 抓取数据")
            sys.exit(1)
        
        with open(db_file, "r", encoding="utf-8") as f:
            self.songs_db = json.load(f)
        print(f"📚 已加载 {len(self.songs_db)} 首歌曲")
    
    def generate_initial_pattern(self, text: str) -> str:
        """生成初始星号模式，保留空格"""
        result = ""
        for char in text:
            if char == ' ':
                result += ' '
            else:
                result += '*'
        return result
    
    def reveal_pattern(self, original: str, guessed_chars: set) -> str:
        """根据已猜字符显示模式"""
        result = ""
        for char in original:
            if char == ' ':
                result += ' '
            elif char.lower() in guessed_chars or char.upper() in guessed_chars:
                result += char
            else:
                result += '*'
        return result
    
    def create_new_game(self, count: int) -> List[Dict]:
        """创建新游戏，随机选择count首歌曲"""
        if count > len(self.songs_db):
            print(f"❌ 错误：题库只有 {len(self.songs_db)} 首，无法出 {count} 题")
            return []
        
        selected = random.sample(self.songs_db, count)
        game_data = []
        
        for song in selected:
            name = song.get("name", "")
            artist = song.get("artist", "Unknown")
            aliases = song.get("aliases", [])
            
            game_data.append({
                "name": name,
                "artist": artist,
                "aliases": aliases,
                "pattern": self.generate_initial_pattern(name)
            })
        
        return game_data
    
    def save_game_answers(self, filename: str = None):
        """保存游戏答案（用于导入）"""
        if filename is None:
            filename = f"game_answers_{int(time.time())}.json"
        
        data = {
            "guessed_chars": list(self.current_game["guessed_chars"]),
            "revealed_songs": list(self.current_game["revealed_songs"]),
            "answers": [
                {
                    "name": ans[0],
                    "artist": ans[1],
                    "aliases": ans[2]
                }
                for ans in self.current_game["answers"]
            ]
        }
        
        with open(filename, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        
        print(f"💾 游戏答案已保存到: {filename}")
        return filename
    
    def load_game_answers(self, filename: str):
        """从文件导入游戏"""
        with open(filename, "r", encoding="utf-8") as f:
            data = json.load(f)
        
        self.current_game["guessed_chars"] = set(data.get("guessed_chars", []))
        self.current_game["revealed_songs"] = set(data.get("revealed_songs", []))
        self.current_game["answers"] = [
            (ans["name"], ans["artist"], ans.get("aliases", []))
            for ans in data["answers"]
        ]
        self.current_game["round"] = len(self.current_game["guessed_chars"])
        
        # 重新计算当前显示的模式
        self.current_game["patterns"] = []
        for i, (name, artist, aliases) in enumerate(self.current_game["answers"], 1):
            if i in self.current_game["revealed_songs"]:
                # 已揭晓的歌曲显示完整答案
                self.current_game["patterns"].append(f"{name} - {artist}")
            else:
                pattern = self.reveal_pattern(name, self.current_game["guessed_chars"])
                self.current_game["patterns"].append(pattern)
        
        print(f"📂 已导入游戏：{len(self.current_game['answers'])} 题")
        print(f"   已猜字符: {''.join(sorted(self.current_game['guessed_chars']))}")
        print(f"   已揭晓: {len(self.current_game['revealed_songs'])} 题")
    
    def display_current_state(self):
        """显示当前游戏状态"""
        guessed = "".join(sorted(self.current_game["guessed_chars"]))
        print(f"\nguessed: {guessed if guessed else '(none)'}")
        
        for i, pattern in enumerate(self.current_game["patterns"], 1):
            print(f"{i}. {pattern}")
        print()
    
    def guess_char(self, char: str) -> bool:
        """猜一个字符"""
        if len(char) != 1:
            print("❌ 请只输入一个字符")
            return False
        
        if char in self.current_game["guessed_chars"]:
            print(f"⚠️ 字符 '{char}' 已经被猜过了")
            return False
        
        self.current_game["guessed_chars"].add(char.lower())
        self.current_game["guessed_chars"].add(char.upper())
        self.current_game["round"] += 1
        
        # 更新所有未揭晓歌曲的显示
        for i, (name, artist, aliases) in enumerate(self.current_game["answers"], 1):
            if i not in self.current_game["revealed_songs"]:
                new_pattern = self.reveal_pattern(name, self.current_game["guessed_chars"])
                self.current_game["patterns"][i-1] = new_pattern
        
        # 检查是否完全揭开了某些歌
        newly_revealed = []
        for i, (name, artist, aliases) in enumerate(self.current_game["answers"], 1):
            if i not in self.current_game["revealed_songs"]:
                pattern = self.current_game["patterns"][i-1]
                if '*' not in pattern:
                    # 所有字符都被猜出来了
                    self.current_game["revealed_songs"].add(i)
                    self.current_game["patterns"][i-1] = f"{name} - {artist}"
                    newly_revealed.append(i)
        
        if newly_revealed:
            print(f"🎉 恭喜！第 {', '.join(map(str, newly_revealed))} 题已被完全揭开！")
        
        return True
    
    def guess_song(self, song_input: str, index: int) -> bool:
        """猜某首歌的全名"""
        if index < 1 or index > len(self.current_game["answers"]):
            print(f"❌ 无效的题号，请输入 1-{len(self.current_game['answers'])}")
            return False
        
        if index in self.current_game["revealed_songs"]:
            print("⚠️ 这首歌已经被猜出来了")
            return False
        
        name, artist, aliases = self.current_game["answers"][index - 1]
        
        # 检查是否匹配（支持原名和别名）
        correct_names = [name.lower()] + [a.lower() for a in aliases]
        if song_input.lower() in correct_names or song_input.lower() == name.lower():
            print(f"✅ 正确！第 {index} 题答案是：{name} - {artist}")
            self.current_game["revealed_songs"].add(index)
            self.current_game["patterns"][index - 1] = f"{name} - {artist}"
            return True
        else:
            print(f"❌ 错误，这不是第 {index} 题的答案")
            return False
    
    def check_game_complete(self) -> bool:
        """检查游戏是否完成"""
        return len(self.current_game["revealed_songs"]) == len(self.current_game["answers"])
    
    def get_hint(self) -> str:
        """获取提示（返回一个未猜过的常见字符）"""
        # 统计未揭晓歌曲中未猜字符的频率
        char_freq = {}
        for i, (name, artist, aliases) in enumerate(self.current_game["answers"], 1):
            if i not in self.current_game["revealed_songs"]:
                for char in name:
                    if char != ' ' and char.lower() not in self.current_game["guessed_chars"]:
                        char_lower = char.lower()
                        char_freq[char_lower] = char_freq.get(char_lower, 0) + 1
        
        if char_freq:
            best_char = max(char_freq, key=char_freq.get)
            return best_char
        return None


def print_help():
    """打印帮助信息"""
    print("""
🎮 命令说明：
  [字符]          - 开一个字母或符号（如：e, E, 3, !, 中）
  [数字] [歌名]   - 猜某首歌（如：1 Hyacinth）
  /save           - 保存当前游戏状态
  /load [文件名]  - 导入游戏
  /new [数量]     - 开始新游戏（如：/new 8）
  /hint           - 获取提示
  /answers        - 显示所有答案（作弊）
  /quit           - 退出游戏
  /help           - 显示此帮助
""")


def main():
    print("🎵 TUF 开字母游戏系统")
    print("=====================")
    
    game = LetterGuessingGame()
    
    # 初始菜单
    print("\n请选择：")
    print("1. 开始新游戏")
    print("2. 导入已有游戏")
    print("3. 退出")
    
    choice = input("> ").strip()
    
    if choice == "1":
        count = input("请输入题目数量：").strip()
        try:
            count = int(count)
            game_data = game.create_new_game(count)
            if not game_data:
                return
            
            game.current_game["answers"] = [
                (d["name"], d["artist"], d["aliases"]) for d in game_data
            ]
            game.current_game["patterns"] = [d["pattern"] for d in game_data]
            game.current_game["round"] = 0
            
            print(f"\n🎯 新游戏开始！共 {count} 题")
            print("初始状态（全星号）：")
            game.display_current_state()
            
        except ValueError:
            print("❌ 无效的数字")
            return
            
    elif choice == "2":
        files = list(Path(".").glob("game_answers_*.json"))
        if not files:
            print("❌ 没有找到可导入的游戏文件")
            return
        
        print("\n可导入的文件：")
        for i, f in enumerate(files, 1):
            print(f"{i}. {f}")
        
        file_choice = input("选择文件编号或输入完整路径：").strip()
        try:
            if file_choice.isdigit():
                filename = str(files[int(file_choice) - 1])
            else:
                filename = file_choice
            game.load_game_answers(filename)
            game.display_current_state()
        except (ValueError, IndexError, FileNotFoundError) as e:
            print(f"❌ 导入失败：{e}")
            return
    else:
        return
    
    print_help()
    
    # 游戏主循环
    while True:
        if game.check_game_complete():
            print("\n🎉🎉🎉 恭喜！你已经完成了所有题目！")
            game.display_current_state()
            break
        
        cmd = input("> ").strip()
        
        if not cmd:
            continue
        
        if cmd == "/quit":
            print("👋 再见！")
            break
        
        elif cmd == "/help":
            print_help()
        
        elif cmd == "/save":
            game.save_game_answers()
        
        elif cmd.startswith("/load "):
            filename = cmd[6:].strip()
            try:
                game.load_game_answers(filename)
                game.display_current_state()
            except Exception as e:
                print(f"❌ 导入失败：{e}")
        
        elif cmd.startswith("/new "):
            try:
                count = int(cmd[5:].strip())
                game_data = game.create_new_game(count)
                if game_data:
                    game.current_game = {
                        "guessed_chars": set(),
                        "revealed_songs": set(),
                        "patterns": [d["pattern"] for d in game_data],
                        "answers": [(d["name"], d["artist"], d["aliases"]) for d in game_data],
                        "round": 0
                    }
                    print(f"\n🎯 新游戏开始！共 {count} 题")
                    game.display_current_state()
            except ValueError:
                print("❌ 无效的数字")
        
        elif cmd == "/hint":
            hint = game.get_hint()
            if hint:
                print(f"💡 建议尝试：'{hint}'")
            else:
                print("💡 没有可用的提示")
        
        elif cmd == "/answers":
            print("\n⚠️ 作弊模式 - 所有答案：")
            for i, (name, artist, aliases) in enumerate(game.current_game["answers"], 1):
                alias_str = f" (别名: {', '.join(aliases)})" if aliases else ""
                print(f"  {i}. {name} - {artist}{alias_str}")
            print()
        
        elif ' ' in cmd:
            # 猜歌模式：[数字] [歌名]
            parts = cmd.split(' ', 1)
            if parts[0].isdigit():
                index = int(parts[0])
                song_name = parts[1].strip()
                game.guess_song(song_name, index)
                game.display_current_state()
            else:
                # 可能是单字符，但包含空格，视为无效
                print("❌ 无效命令。输入 /help 查看帮助")
        
        elif len(cmd) == 1:
            # 单字符模式
            if game.guess_char(cmd):
                game.display_current_state()
        
        else:
            print("❌ 无效命令。输入 /help 查看帮助")


if __name__ == "__main__":
    import time
    main()
