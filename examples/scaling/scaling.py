#!/usr/bin/python3
import os
import subprocess

cwd = os.getcwd()
subprocess.call("bash compile.sh", shell=True, cwd="{}/../../".format(cwd))

# scaling

scalingList = "scaling.list.txt"
lines = []
with open(scalingList, "r") as sl:
    lines = sl.readlines()

print(lines)
print(len(lines))
banks = "banks"

try:
    os.mkdir(banks)
except:
    pass
print(" ---- Download started ---- ")

#command = f"bash download.sh ./scaling/{scalingList} ./scaling/{banks}"
command = f"bash download.sh ./scaling/{scalingList} /run/media/simo/Ventoy/tmpBanks"

subprocess.call(command, shell=True, cwd="{}/..".format(cwd))


print(" ---- Download completed ---- ")

exit(0)

tmpBanks = "tmpBanks"
try:
    os.mkdir(tmpBanks)
except:
    pass

tmpNets = "tmpNets"
try:
    os.mkdir(tmpNets)
except:
    pass

i = 0

files = os.listdir("./{}".format(banks))

# currentStep = 0
increment = 10
totalTimes = 10

times = 0

while i < len(lines):
    
    if times >= totalTimes:
        increment *= 10
        times = 0

    j = i
    while j < i + increment and j < len(lines):
        print("Moving file", files[j])
        os.rename(f"./{banks}/{files[j]}", f"./{tmpBanks}/{files[j]}")
        j += 1
    i = j

    times += 1

    tmpFaa = "tmpFaa.pdi"
    subprocess.call("python3 ../gbk2ig.py ./{}/ ./{}".format(tmpBanks, tmpFaa), shell=True, cwd=cwd)

    command = "bash execute.sh -i {} -o {} -g {} -p ./".format(tmpFaa, f"./{tmpNets}/{i}", i)
    subprocess.call(command, shell=True, cwd=cwd)


subprocess.call("python plot.py", shell=True, cwd=cwd)
