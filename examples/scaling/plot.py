#!/usr/bin/python3

import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter


tmpFile = "tmp.txt"


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
    "genomes": [],
    "memory": [],
}
noMode = {
    "time": [],
    "genomes": [],
    "memory": [],
}


for l in interestedLines:
    splitted = l.split(",")
    if int(splitted[0]) == 1:
        mode["genomes"].append(int(splitted[1]))
        mode["time"].append(float(splitted[2]))
        mode["memory"].append(int(int(splitted[3])/(1024*1)))
    else:
        noMode["genomes"].append(int(splitted[1]))
        noMode["time"].append(float(splitted[2]))
        noMode["memory"].append(int(int(splitted[3])/(1024*1)))

print(mode)
print(noMode)

fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 10))
# fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(15, 5))

ax1.plot(noMode["genomes"], noMode["time"], label="Normal mode", color="blue")
ax1.plot(mode["genomes"], mode["time"], label="Eco mode", color="red")

ax1.set_xlabel("Number of genomes")
ax1.set_ylabel("Time (seconds)")
ax1.set_title("Time x Genomes for normal mode and eco mode")
ax1.legend()
ax1.grid(True)

ax2.plot(noMode["genomes"], noMode["memory"], label="Normal mode", color="blue")
ax2.plot(mode["genomes"], mode["memory"], label="Eco mode", color="red")

ax2.set_xlabel("Number of genomes")
ax2.set_ylabel("Memory (MB)")
ax2.set_title("Memory x Genomes for normal mode and eco mode")
ax2.legend()
ax2.grid(True)

ax2.yaxis.set_major_formatter(ScalarFormatter(useOffset=False))
ax2.yaxis.get_major_formatter().set_scientific(False)

plt.tight_layout()
plt.savefig("combined_plot.png")


# plt.plot(noMode["genomes"], noMode["time"], label="Normal mode", color="blue")
# plt.plot(mode["genomes"], mode["time"], label="Eco mode", color="red")

# plt.xlabel("Number of genomes")
# plt.ylabel("Time (seconds)")
# plt.title("Time x Genomes for normal mode and eco mode")
# plt.legend()
# plt.grid(True)

# # Show the plot
# plt.savefig("time_plot.png")

# plt.clf()


# plt.plot(noMode["genomes"], noMode["memory"], label="Normal mode", color="blue")
# plt.plot(mode["genomes"], mode["memory"], label="Eco mode", color="red")

# plt.xlabel("Number of genomes")
# plt.ylabel("Memory (GB)")
# plt.title("Memory x Genomes for normal mode and eco mode")
# plt.legend()
# plt.grid(True)

# plt.gca().yaxis.set_major_formatter(ScalarFormatter(useOffset=False))
# plt.gca().yaxis.get_major_formatter().set_scientific(False)


# # Show the plot
# plt.savefig("memory_plot.png")
