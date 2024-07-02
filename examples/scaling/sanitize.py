#!/usr/bin/python3
import os
import subprocess
from Bio import SeqIO

cwd = os.getcwd()

# scaling

scalingList = "scaling.list.txt"
lines = []
with open(scalingList, "r") as sl:
    for line in sl.readlines():
        l = line.strip()
        splitted = (l.split("\t"))
        lines.append(splitted)


# print(lines)
print(len(lines))

i = 0

while i < len(lines):
    try:
        os.system("rm -rf {}".format("./banks"))
    except:
        pass

    try:
        os.mkdir("banks")
    except:
        pass

    currentList = "current.list.txt"
    currentLines = []

    print("Downloading", len(currentLines), "files")
    print(currentLines)
    
    with open(currentList, "w") as cl:
        j = i
        while j < i+10 and j < len(lines):
            cl.write("{}\t{}\t{}\n".format(lines[j][0], lines[j][1], lines[j][2]))
            c = lines[j]
            c[2] += ".gbk"
            currentLines.append(c)
            j+=1

        i+=10


    print(f"--- Status {i}/{len(lines)}---")
    command = f"bash download.sh ./scaling/{currentList} ./scaling/banks"
    subprocess.call(command, shell=True, cwd="{}/..".format(cwd))

    notAllowed = []
    allFiles = os.listdir("./banks/")
    print(allFiles)
    for file in allFiles:
        path = f"./banks/{file}"
        print(path)
        try:
            for record in SeqIO.parse(path, "genbank"):
                for feature in record.features:
                    if feature.type == "CDS":
                        if ('translation' in feature.qualifiers):
                            qual = feature.qualifiers['locus_tag'][0]
        except:
            print("error: ",file)
            notAllowed.append(file)

    print(allFiles)
    print(notAllowed)

    newLines = []
    for line in currentLines:
        banned = False
        for na in notAllowed:
            if na == line[2]:
                banned = True
        if not banned:
            newLines.append(line)

    print(currentLines)
    print(len(currentLines))
    print(newLines)
    print(len(newLines))

    newFile = "newScaling.list.txt"
    with open(newFile, "a") as s:
        for line in newLines:
            print("writing line: {}\t{}\t{}".format(line[0], line[1], line[2].replace(".gbk", "")))
            s.write("{}\t{}\t{}\n".format(line[0], line[1], line[2].replace(".gbk", "")))