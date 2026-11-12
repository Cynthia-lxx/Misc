import threading
import keyboard
import random
import time

lk = [
    [],
    ["e"],
    ["2", "e"],
    ["1", "2", "e"],
    ["q", "1", "2", "e"],
    ["1", "2", "e", "c", "space"],
    ["q", "1", "2", "e", "c", "space"],
    ["f", "q", "1", "2", "e", "c", "space"],
    ["f", "caps lock", "z", "q", "1", "2", "e", "space"]
]

rk = [
    [],
    ["p"],
    ["-", "p"],
    ["=", "-", "p"],
    ["\\", "=", "-", "p"],
    ["\\", "=", "-", "p", ","],
    ["\\", "=", "-", "p", ",", "."],
    ["h", "\\", "=", "-", "p", ",", "."],
    ["h", ",", ".", "p", "-", "=", "\\", "enter", "m"]
]

tabub = [[6,6],[6,6],[8,8],[8,8],[6,8],[6,8],[5,5],[8,6],
         [8,6],[5,5],[8,6],[8,8],[6,8],[5,5],[8,6],[8,6],
         [5,5],[6,6],[6,6],[5,5],[5,6],[5,5],[6,6],[5,5],
         [6,6],[5,5],[8,8],[8,8],[6,6],[8,8],[5,8],[8,8],
         [8,8],[8,8],[5,5],[5,5],[4,5],[6,6],[6,6],[6,6],
         [6,6],[5,6],[6,6],[6,6],[6,6],[6,6],[6,6],[4,5],
         [6,6],[4,5],[6,6],[4,5],[4,5],[6,5],[4,8],[4,8],
         [4,8],[5,5],[8,8],[8,8],[8,8],[8,8]]

def delay(t):
    while time.time() < t:
        pass

def pseudo(hand, keys, BPM, t0, k=1.35):
    expected_time = 60.0 * k / BPM
    final_end_time = t0 + expected_time
    interv = max([60/BPM/4/keys,0.008])
    pause = max([60/BPM/4/keys,0.02])
    start_time = time.time()
    for key in (lk[keys] if hand == 0 else rk[keys]):
        keyboard.release(key)
        keyboard.press(key)
        elapsed = time.time() - start_time
        if elapsed + interv <= expected_time:
            time.sleep(interv)
    press_time = time.time() - start_time
    start_time_release = time.time()
    for key in (lk[keys] if hand == 0 else rk[keys]):
        keyboard.release(key)
        elapsed_release = time.time() - start_time_release
        if elapsed_release + interv <= expected_time:
            time.sleep(interv)
   
def roll(hand, keys, BPM, t0, k=1.1):
    sticked_keys=random.randint(1,keys-1)
    miss=max([2,min(10,5000/BPM)])
    miss=int(miss)
    interv = k*60/BPM/keys*0.95
    #pause = k*60/BPM-keys*interv
    for key in (lk[keys] if hand == 0 else rk[keys]):
        keyboard.release(key)
    for i in range(keys*2):
        if i%2==0:
            #print(i)
            keyboard.press(lk[keys][int(i/2)] if hand == 0 else rk[keys][int(i/2)])
            time.sleep(interv/2+random.randint(-miss,miss)/1000)
        else:
            if (i-1)/2-sticked_keys>=0:
                keyboard.release(lk[keys][int((i-1)/2-sticked_keys)] if hand == 0 else rk[keys][int((i-1)/2-sticked_keys)])
            time.sleep(interv/2+random.randint(-miss,miss)/1000)
    for i in range(sticked_keys,keys):
        keyboard.release(lk[keys][i] if hand == 0 else rk[keys][i])
        

def multipress(hand, keys, BPM, t0, k=1.35):
    for key in (lk[keys] if hand == 0 else rk[keys]):
        keyboard.release(key)
        time.sleep(0.000001)
    time.sleep(60*(k-0.2)/BPM)
    release_keys=(lk[keys] if hand==0 else rk[keys])
    random.shuffle(release_keys)
    for key in release_keys:
        keyboard.release(key)
        time.sleep(60*0.2/BPM/keys)

