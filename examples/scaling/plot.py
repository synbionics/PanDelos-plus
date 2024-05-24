#!/usr/bin/python3

import matplotlib.pyplot as plt


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
        mode["time"].append(float(splitted[2]))
        mode["memory"].append(int(splitted[3]))
        mode["genomes"].append(int(splitted[1]))
    else:
        noMode["time"].append(float(splitted[2]))
        noMode["memory"].append(int(splitted[3]))
        noMode["genomes"].append(int(splitted[1]))

print(mode)
print(noMode)

plt.plot(mode["genomes"], mode["time"], label="Mode", color="blue", marker='o')
plt.plot(noMode["genomes"], noMode["time"], label="No Mode", color="red", marker='x')

plt.xlabel("Genomes")
plt.ylabel("Time")
plt.title("Time x Genomes for Mode and No Mode")
plt.legend()
plt.grid(True)

# Show the plot
plt.savefig("time_plot.png")

plt.clf()

plt.plot(mode["genomes"], mode["memory"], label="Mode", color="blue", marker='o')
plt.plot(noMode["genomes"], noMode["memory"], label="No Mode", color="red", marker='x')

plt.xlabel("Genomes")
plt.ylabel("Memory")
plt.title("Memory x Genomes for Mode and No Mode")
plt.legend()
plt.grid(True)

# Show the plot
plt.savefig("memory_plot.png")
