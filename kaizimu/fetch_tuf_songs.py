# fetch_tuf_songs.py
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
TUF Songs Fetcher
抓取The Universal Forums的歌曲数据用于开字母游戏辅助
"""

import requests
import json
import time
import sys
from pathlib import Path

BASE_URL = "https://api.tuforums.com/v2"
OUTPUT_FILE = "tuf_songs.json"


def fetch_all_songs():
    """抓取所有歌曲数据"""
    all_songs = []
    page = 1
    limit = 100  # 每页数量，最大可以调到1000
    
    print(f"🎵 TUF Songs Fetcher")
    print(f"===================")
    print(f"Target: {OUTPUT_FILE}")
    print()
    
    while True:
        url = f"{BASE_URL}/database/songs?page={page}&limit={limit}"
        print(f"Fetching page {page}...", end=" ", flush=True)
        
        try:
            response = requests.get(url, timeout=30)
            response.raise_for_status()
            data = response.json()
            
            songs = data.get("songs", [])
            total = data.get("total", 0)
            
            if not songs:
                print("No more data.")
                break
            
            # 提取关键字段，减少文件体积
            for song in songs:
                song_info = {
                    "id": song.get("id"),
                    "name": song.get("name"),
                    "artist": ", ".join([c.get("artist", {}).get("name", "") 
                                        for c in song.get("credits", [])]),
                    "aliases": [a.get("alias", "") for a in song.get("aliases", [])],
                    "verificationState": song.get("verificationState")
                }
                all_songs.append(song_info)
            
            print(f"Got {len(songs)} songs (Total: {len(all_songs)}/{total})")
            
            # 检查是否还有更多
            if not data.get("hasMore", False) or len(songs) < limit:
                break
            
            page += 1
            time.sleep(0.3)  # 礼貌性延迟
            
        except requests.exceptions.RequestException as e:
            print(f"Error: {e}")
            break
        except KeyboardInterrupt:
            print("\nInterrupted by user.")
            break
    
    # 保存数据
    print(f"\n📊 Total songs fetched: {len(all_songs)}")
    
    with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
        json.dump(all_songs, f, ensure_ascii=False, indent=2)
    
    print(f"💾 Saved to: {OUTPUT_FILE}")
    
    # 显示示例
    print(f"\n📋 Sample entries:")
    for song in all_songs[:5]:
        print(f"  - {song['name']} (by {song['artist']})")
    
    return all_songs


if __name__ == "__main__":
    fetch_all_songs()
