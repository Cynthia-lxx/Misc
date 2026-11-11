# solve_letter_guess.py
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
TUF Letter Guessing Game Solver
根据开字母模式匹配TUF歌曲库
"""

import json
import re
import sys
from pathlib import Path
from collections import Counter


def pattern_to_regex(pattern, guessed_letters):
    """
    将开字母模式转换为正则表达式
    * 匹配任何非已猜字母的字符
    已知字母必须精确匹配
    """
    regex = "^"
    for char in pattern:
        if char == '*':
            # 星号：匹配任何字符，但不能是已猜字母（忽略大小写）
            excluded = "".join(set(guessed_letters.lower() + guessed_letters.upper()))
            regex += f"[^{re.escape(excluded)}]"
        elif char == ' ':
            regex += r"\s"
        elif char == '?':
            regex += "."
        else:
            # 已知字母，忽略大小写匹配
            regex += re.escape(char)
    regex += "$"
    return regex


def load_songs(filepath):
    """加载歌曲数据"""
    with open(filepath, "r", encoding="utf-8") as f:
        return json.load(f)


def find_matches(songs, pattern, guessed_letters):
    """根据模式查找匹配的歌曲"""
    regex_pattern = pattern_to_regex(pattern, guessed_letters)
    # print(f"Debug: Regex = {regex_pattern}")  # 调试时可取消注释
    
    matches = []
    for song in songs:
        name = song.get("name", "")
        if re.match(regex_pattern, name, re.IGNORECASE):
            # 额外验证：确保已猜字母确实在正确位置
            match = True
            for i, (p_char, n_char) in enumerate(zip(pattern, name)):
                if p_char not in ['*', '?', ' '] and p_char.lower() != n_char.lower():
                    match = False
                    break
            if match:
                matches.append(song)
    return matches


def suggest_letters(matches, patterns, guessed_letters):
    """根据匹配结果建议下一个猜测的字母"""
    candidates = []
    guessed_set = set(guessed_letters.lower())
    
    for song in matches:
        name = song.get("name", "")
        # 找到对应这个歌曲的模式（简化处理：假设所有模式都检查）
        for pattern in patterns:
            if len(pattern) != len(name):
                continue
            for i, (p_char, n_char) in enumerate(zip(pattern, name)):
                if p_char == '*' and n_char.isalpha():
                    char_lower = n_char.lower()
                    if char_lower not in guessed_set:
                        candidates.append(char_lower)
    
    if not candidates:
        return []
    
    # 统计频率
    freq = Counter(candidates)
    return freq.most_common(10)


def main():
    # 配置
    INPUT_FILE = "tuf_songs.json"
    GUESSED_LETTERS = "esao"
    
    # 你的8个谜题模式
    PATTERNS = [
        "***** o* ******s",
        "*s**a***e",
        "*e*so* *o Sa*e****e",
        "**a*****",
        "***s*** *o ***a*o",
        "*a*s**e **** *o S*o*s***s*",
        "***os******",
        "*e**o**e ***E**E**"
    ]
    
    print("🎮 TUF Letter Guessing Solver")
    print("=============================")
    print(f"Input: {INPUT_FILE}")
    print(f"Guessed letters: {GUESSED_LETTERS}")
    print(f"Patterns: {len(PATTERNS)}")
    print()
    
    # 加载数据
    try:
        songs = load_songs(INPUT_FILE)
        print(f"📚 Loaded {len(songs)} songs from database")
    except FileNotFoundError:
        print(f"❌ Error: {INPUT_FILE} not found. Run fetch_tuf_songs.py first.")
        sys.exit(1)
    print()
    
    # 分析每个模式
    all_matches = []
    for i, pattern in enumerate(PATTERNS, 1):
        print(f"🔍 Pattern {i}: '{pattern}'")
        
        matches = find_matches(songs, pattern, GUESSED_LETTERS)
        print(f"   Found {len(matches)} matches")
        
        if matches:
            # 显示前10个匹配
            for song in matches[:10]:
                name = song.get("name")
                artist = song.get("artist", "Unknown")
                print(f"   - {name} [by {artist}]")
            
            if len(matches) > 10:
                print(f"   ... and {len(matches) - 10} more")
        
        all_matches.extend(matches)
        print()
    
    # 建议下一个字母
    print("💡 Suggested Next Guesses:")
    suggestions = suggest_letters(all_matches, PATTERNS, GUESSED_LETTERS)
    if suggestions:
        print("   Top candidate letters (based on current matches):")
        for letter, count in suggestions:
            print(f"     {letter.upper()}: appears {count} times")
    else:
        print("   No clear candidates. Try common letters: r, t, n, i, l")
    
    # 保存结果
    results = {
        "guessedLetters": GUESSED_LETTERS,
        "patterns": PATTERNS,
        "totalMatches": len(all_matches),
        "matches": [
            {
                "pattern": PATTERNS[i % len(PATTERNS)],
                "song": song.get("name"),
                "artist": song.get("artist")
            }
            for i, song in enumerate(all_matches)
        ]
    }
    
    output_name = f"guessing_results_{int(time.time())}.json"
    with open(output_name, "w", encoding="utf-8") as f:
        json.dump(results, f, ensure_ascii=False, indent=2)
    print(f"\n💾 Results saved to: {output_name}")


if __name__ == "__main__":
    import time
    main()
