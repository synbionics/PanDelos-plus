#!/usr/bin/python3

import sys
import os
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.ticker import ScalarFormatter


tmpFile = sys.argv[1]
outPrefix = sys.argv[2]

plotsPath = "{}/plots/".format(outPrefix)
try:
    os.mkdir(plotsPath)
except FileExistsError:
    pass

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
modeGrid = {
    "time": [],
    "threads": [],
    "memory": [],
    "speedup" : []
}
noModeGrid = {
    "time": [],
    "threads": [],
    "memory": [],
    "speedup" : []
}

for l in interestedLines:
    splitted = l.split(",")
    if int(splitted[0]) == 1:
        if int(splitted[4]) == 0:
            mode["time"].append(float(splitted[1]))
            mode["memory"].append(int(splitted[2])/1024)
            mode["threads"].append(int(splitted[3]))
            mode["speedup"].append(mode["time"][0]/float(splitted[1]))
        else:
            modeGrid["time"].append(float(splitted[1]))
            modeGrid["memory"].append(int(splitted[2])/1024)
            modeGrid["threads"].append(int(splitted[3]))
            modeGrid["speedup"].append(modeGrid["time"][0]/float(splitted[1]))
    else:
        if int(splitted[4]) == 0:
            noMode["time"].append(float(splitted[1]))
            noMode["memory"].append(int(splitted[2])/1024)
            noMode["threads"].append(int(splitted[3]))
            noMode["speedup"].append(noMode["time"][0]/float(splitted[1]))
        else:
            noModeGrid["time"].append(float(splitted[1]))
            noModeGrid["memory"].append(int(splitted[2])/1024)
            noModeGrid["threads"].append(int(splitted[3]))
            noModeGrid["speedup"].append(noModeGrid["time"][0]/float(splitted[1]))
# print(mode)
# print(noMode)


idealSpeedup = np.linspace(1, mode["threads"][len(mode["threads"])-1], 100)


plt.plot(mode["threads"], mode["time"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.tight_layout()
plt.savefig("{}threads_time_mode.png".format(plotsPath))
plt.clf()

plt.plot(modeGrid["threads"], modeGrid["time"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.tight_layout()
plt.savefig("{}threads_time_mode_grid.png".format(plotsPath))
plt.clf()



plt.plot(mode["threads"], mode["memory"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Memory (MB)")
plt.tight_layout()
plt.savefig("{}threads_memory_mode.png".format(plotsPath))
plt.clf()


plt.plot(modeGrid["threads"], modeGrid["memory"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Memory (MB)")
plt.tight_layout()
plt.savefig("{}threads_memory_mode_grid.png".format(plotsPath))
plt.clf()


plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(mode["threads"], mode["speedup"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.tight_layout()
plt.savefig("{}speedup_mode.png".format(plotsPath))
plt.clf()



plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(modeGrid["threads"], modeGrid["speedup"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.tight_layout()
plt.savefig("{}speedup_mode_grid.png".format(plotsPath))
plt.clf()




plt.plot(noMode["threads"], noMode["time"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.tight_layout()
plt.savefig("{}threads_time.png".format(plotsPath))
plt.clf()


plt.plot(noMode["threads"], noMode["memory"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Memory (MB)")
plt.tight_layout()
plt.savefig("{}threads_memory.png".format(plotsPath))
plt.clf()


plt.plot(noModeGrid["threads"], noModeGrid["time"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Time (s)")
plt.tight_layout()
plt.savefig("{}threads_time_grid.png".format(plotsPath))
plt.clf()


plt.plot(noModeGrid["threads"], noModeGrid["memory"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Memory (MB)")
plt.tight_layout()
plt.savefig("{}threads_memory_grid.png".format(plotsPath))
plt.clf()




plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(noMode["threads"], noMode["speedup"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.tight_layout()
plt.savefig("{}speedup.png".format(plotsPath))
plt.clf()

plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(noModeGrid["threads"], noModeGrid["speedup"], marker="o")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.tight_layout()
plt.savefig("{}speedup_grid.png".format(plotsPath))
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
plt.ylabel("Memory (MB)")
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


plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(noModeGrid["threads"], noModeGrid["speedup"], marker="o", label="Normal mode grid")
plt.plot(modeGrid["threads"], modeGrid["speedup"], marker="*", label="Eco mode grid")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}speedup_vs_grids.png".format(plotsPath))
plt.clf()





plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(noModeGrid["threads"], noModeGrid["speedup"], marker="o", label="Normal mode grid")
plt.plot(noMode["threads"], noMode["speedup"], marker="*", label="Normal mode no grid")

plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}speedup_vs_noMode_grid.png".format(plotsPath))
plt.clf()

plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(mode["threads"], mode["speedup"], marker="o", label="Eco mode no grid")
plt.plot(modeGrid["threads"], modeGrid["speedup"], marker="*", label="Eco mode grid")
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}speedup_vs_mode_grid.png".format(plotsPath))
plt.clf()



colors = ['blue', 'orange']


plt.plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup")
plt.plot(mode["threads"], mode["speedup"], label="Eco mode", color=colors[0])
# plt.plot(modeGrid["threads"], modeGrid["speedup"], label="Eco mode balanced")
# plt.plot(noModeGrid["threads"], noModeGrid["speedup"], label="Normal mode balanced")
plt.plot(noMode["threads"], noMode["speedup"], label="Normal mode", color=colors[1])
plt.xlabel("Threads")
plt.ylabel("Speedup")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}complete_speedup.png".format(plotsPath))
plt.clf()



plt.gca().yaxis.set_major_formatter(ScalarFormatter(useOffset=False))
plt.gca().yaxis.get_major_formatter().set_scientific(False)

plt.plot(mode["threads"], mode["memory"], label="Eco mode", color=colors[0])
# plt.plot(modeGrid["threads"], modeGrid["memory"], label="Eco mode balanced")
# plt.plot(noModeGrid["threads"], noModeGrid["memory"], label="Normal mode balanced")
plt.plot(noMode["threads"], noMode["memory"], label="Normal mode", color=colors[1])
plt.xlabel("Threads")
plt.ylabel("Memory (MB)")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}complete_memory.png".format(plotsPath))
plt.clf()





plt.plot(mode["threads"], mode["time"], label="Eco mode", color=colors[0])
# plt.plot(modeGrid["threads"], modeGrid["time"], label="Eco mode balanced")
# plt.plot(noModeGrid["threads"], noModeGrid["time"], label="Normal mode balanced")
plt.plot(noMode["threads"], noMode["time"], label="Normal mode", color=colors[1])

plt.xlabel("Threads")
plt.ylabel("Time (seconds)")
plt.legend()
plt.grid()
plt.tight_layout()
plt.savefig("{}complete_time.png".format(plotsPath))
plt.clf()



fig, axs = plt.subplots(1, 3, figsize=(18, 6))

axs[0].plot(idealSpeedup, idealSpeedup, linestyle='--', label="Ideal speedup", color='gray')
axs[0].plot(mode["threads"], mode["speedup"], label="Eco mode", color=colors[0])
# axs[0].plot(noMode["threads"], noMode["speedup"], label="Normal mode", color=colors[1])

# axs[0].plot(modeGrid["threads"], modeGrid["speedup"], label="Eco mode balanced", color=colors[0])
# axs[0].plot(noModeGrid["threads"], noModeGrid["speedup"], label="Normal mode balanced", color=colors[1])
axs[0].plot(noMode["threads"], noMode["speedup"], label="Normal mode", color=colors[1])
axs[0].set_xlabel("Threads")
axs[0].set_ylabel("Speedup")
axs[0].legend()
axs[0].grid()

axs[1].yaxis.set_major_formatter(ScalarFormatter(useOffset=False))
axs[1].yaxis.get_major_formatter().set_scientific(False)
axs[1].plot(mode["threads"], mode["memory"], label="Eco mode", color=colors[0])
# axs[1].plot(modeGrid["threads"], modeGrid["memory"], label="Eco mode balanced", color=colors[0])
# axs[1].plot(noModeGrid["threads"], noModeGrid["memory"], label="Normal mode balanced", color=colors[1])
axs[1].plot(noMode["threads"], noMode["memory"], label="Normal mode", color=colors[1])
axs[1].set_xlabel("Threads")
axs[1].set_ylabel("Memory (MB)")
axs[1].legend()
axs[1].grid()

axs[2].plot(mode["threads"], mode["time"], label="Eco mode", color=colors[0])
# axs[2].plot(modeGrid["threads"], modeGrid["time"], label="Eco mode balanced", color=colors[0])
# axs[2].plot(noModeGrid["threads"], noModeGrid["time"], label="Normal mode balanced", color=colors[1])
axs[2].plot(noMode["threads"], noMode["time"], label="Normal mode", color=colors[1])
axs[2].set_xlabel("Threads")
axs[2].set_ylabel("Time (seconds)")
axs[2].legend()
axs[2].grid()

# Miglioramento layout e salvataggio
plt.tight_layout()
plt.savefig("{}complete_combined.png".format(plotsPath))
# plt.show()