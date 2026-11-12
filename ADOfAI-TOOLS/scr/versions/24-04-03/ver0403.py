import threading
import keyboard
import random
import time

tabub_filename="tabub.txt"

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

def process_tabub(t_filename):
    print("[INFO] Tabub file preprocessing starts")
    legal_ch="0123456789BPM=."
    func_list=[None,multipress,pseudo,roll]
    ret=[]
    f=open(t_filename,"r")
    raw_data=f.read()
    line_data=[]
    print("[DEBUG] Splitting tabub data lines")
    for tabub_line in raw_data.split("\n"):
        if tabub_line.startswith("#"):
            print("[INFO] Skipped comment line: \""+str(tabub_line)[:30]+"\"")
        else:
            line_data.append(tabub_line)
    print("[DEBUG] Splitting single tabub in each line")
    tabub_list=[]
    for tabub_line_p in line_data:
        for i in tabub_line_p.split(" "):
            if i=="":
                print("[WARNING] Empty tabub, skipping")
                continue
            flag=True
            for c in i:
                if not (c in legal_ch):
                    flag=False
                    print("[WARNING] Invalid character \""+c+"\" in tabub \""+i+"\", replacing it with \"0\"")
            if flag==True:
                tabub_list.append(i)
            else:
                tabub_list.append("0")
    print("[DEBUG] Processing each tabub")
    CBPM=3000000
    t=0
    for i in tabub_list:
        if i.startswith("BPM="):
            CBPM=float(i[4:])
        elif i=="0":
            t+=60/CBPM
        elif i.startswith("1"):
            ret.append([t,multipress,[int(i[1]),int(i[2]),CBPM,t]] if len(i)==3 else [t,multipress,[int(i[1]),int(i[2]),CBPM,t,float(i[3:])]])
            t+=60/CBPM
        elif i.startswith("2"):
            ret.append([t,pseudo,[int(i[1]),int(i[2]),CBPM,t]] if len(i)==3 else [t,multipress,[int(i[1]),int(i[2]),CBPM,t,float(i[3:])]])
            t+=60/CBPM
        elif i.startswith("3"):
            ret.append([t,roll,[int(i[1]),int(i[2]),CBPM,t]] if len(i)==3 else [t,multipress,[int(i[1]),int(i[2]),CBPM,t,float(i[3:])]])
            t+=60/CBPM
        else:
            print("[WARNING] Invalid tabub: \""+i+"\"")
    print("[INFO] Tabub file preprocessing complete")
    return ret

def exec_(exec_list,t0=time.time()):
    print("[INFO] EXECUTION STARTS!!!") ###DELETE THIS !!
    for i in exec_list:
        print("[DEBUG] Executing "+str(i))
        delay(t0+i[0])
        threading.Thread(target=i[1],args=i[2]).start()

l=process_tabub(".\\chenzhen_cut.txt")
keyboard.wait("6")
exec_(l,time.time())
        
    
