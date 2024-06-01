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
        mode["memory"].append(int(int(splitted[3])/1024))
    else:
        noMode["genomes"].append(int(splitted[1]))
        noMode["time"].append(float(splitted[2]))
        noMode["memory"].append(int(int(splitted[3])/1024))

print(mode)
print(noMode)

plt.plot(noMode["genomes"], noMode["time"], label="Normal mode", color="blue", marker='o')
plt.plot(mode["genomes"], mode["time"], label="Eco mode", color="red", marker='x')

plt.xlabel("Number of genomes")
plt.ylabel("Time (seconds)")
plt.title("Time x Genomes for normal mode and eco mode")
plt.legend()
plt.grid(True)

# Show the plot
plt.savefig("time_plot.png")

plt.clf()


plt.plot(noMode["genomes"], noMode["memory"], label="Normal mode", color="blue", marker='o')
plt.plot(mode["genomes"], mode["memory"], label="Eco mode", color="red", marker='x')

plt.xlabel("Number of genomes")
plt.ylabel("Memory (KB)")
plt.title("Memory x Genomes for normal mode and eco mode")
plt.legend()
plt.grid(True)

plt.gca().yaxis.set_major_formatter(ScalarFormatter(useOffset=False))
plt.gca().yaxis.get_major_formatter().set_scientific(False)


# Show the plot
plt.savefig("memory_plot.png")
