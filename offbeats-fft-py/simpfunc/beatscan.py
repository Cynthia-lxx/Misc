import librosa
import numpy as np

# 加载音频文件
y, sr = librosa.load('t.mp3')

# 使用tempo和beat_frames来提取节拍信息
tempo, beat_frames = librosa.beat.beat_track(y=y, sr=sr)

# 获取每个节拍的时间点
beat_times = librosa.frames_to_time(beat_frames, sr=sr)

# 打印节拍信息
print("乐曲BPM：", tempo)
print("节拍时间点（秒）：", beat_times)

# 识别多个节拍
beats = []
for i in range(len(beat_times)-1):
    beat_duration = beat_times[i+1] - beat_times[i]
    if beat_duration < 0.5:  # 自定义节拍时长阈值
        beats.append(beat_times[i])
beats.append(beat_times[-1])

# 打印识别到的多个节拍时间点
print("识别到的多个节拍时间点（秒）：", beats)
