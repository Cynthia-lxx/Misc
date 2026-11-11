import os
with open("G:\\v86\\v.txt","w") as f:
    for a,b,c in os.walk("G:\\NT\\"):
        for i in c:
            f.write(os.path.join(a,i))
            f.write("\n")

