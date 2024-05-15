#!/usr/bin/python3

import sys
import os
import matplotlib.pyplot as plt
import numpy as np

tmpFile = sys.argv[1]
outPrefix = sys.argv[2]

plotsPath = "{}/plots/".format(outPrefix)
os.mkdir(plotsPath)

interestedLines = []

with open(tmpFile, "r") as f:
    for line in f.readlines():
        if "->" in line:
            line = line.replace("->", "")
            line = line.replace("{", "")
            line = line.replace("}", "")
            interestedLines.append(line)

mode = {
    "time": [],
    "threads": [],
    "memory": [],
    "speedup" : []
}
noMode = {
    "time": [],
    "threads": [],
    "memory": [],
    "speedup" : []
}

for l in interestedLines:
    splitted = l.split(",")
    if int(splitted[0]) == 1:
        mode["time"].append(float(splitted[1]))
        mode["memory"].append(int(splitted[2]))
        mode["threads"].append(int(splitted[3]))
        mode["speedup"].append(mode["time"][0]/float(splitted[1]))
    else:
        noMode["time"].append(float(splitted[1]))
        noMode["memory"].append(int(splitted[2]))
        noMode["threads"].append(int(splitted[3]))
        noMode["speedup"].append(noMode["time"][0]/float(splitted[1]))

print(mode)
print(noMode)


idealSpeedup = np.linspace(1, mode["threads"][len(mode["threads"])-1], 100)


plt.plot(mode["threads"], mode["time"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.tight_layout()
plt.savefig("{}threads_time_mode.png".format(plotsPath))
plt.clf()


plt.plot(mode["threads"], mode["memory"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Memory (KB)")
plt.tight_layout()
plt.savefig("{}threads_memory_mode.png".format(plotsPath))
plt.clf()

plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(mode["threads"], mode["speedup"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.tight_layout()
plt.savefig("{}speedup_mode.png".format(plotsPath))
plt.clf()



plt.plot(noMode["threads"], noMode["time"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.tight_layout()
plt.savefig("{}threads_time.png".format(plotsPath))
plt.clf()


plt.plot(noMode["threads"], noMode["memory"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Memory (KB)")
plt.tight_layout()
plt.savefig("{}threads_memory.png".format(plotsPath))
plt.clf()



plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(noMode["threads"], noMode["speedup"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.tight_layout()
plt.savefig("{}speedup.png".format(plotsPath))
plt.clf()



plt.plot(noMode["threads"], noMode["time"], marker="o", label="Without -m")
plt.plot(mode["threads"], mode["time"], marker="*", label="With -m")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.legend()
plt.tight_layout()
plt.savefig("{}threads_time_vs.png".format(plotsPath))
plt.clf()


plt.plot(noMode["threads"], noMode["memory"], marker="o", label="Without -m")
plt.plot(mode["threads"], mode["memory"], marker="*", label="With -m")
plt.xlabel("Threads")
plt.ylabel("Memory (KB)")
plt.legend()
plt.tight_layout()
plt.savefig("{}threads_memory_vs.png".format(plotsPath))
plt.clf()



plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(noMode["threads"], noMode["speedup"], marker="o", label="Without -m")
plt.plot(mode["threads"], mode["speedup"], marker="*", label="With -m")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}speedup_vs.png".format(plotsPath))
plt.clf()