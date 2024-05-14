#!/usr/bin/python3

import sys


iList = sys.argv[1]

allLinks = ""
with open(iList, "r") as ilf:
    for line in ilf.readlines():
        if line[0] == "#":
            continue
        # print(line)
        split = line.strip().split("\t")
        # print(split)
        assembly, gBank, refSeq = split
        # print(assembly)
        # print(gBank)
        # print(refSeq)
        chars, numbers = gBank.split("_")
        numbers = numbers.split(".")[0]
        path = chars + "/" + numbers[0 : 3] + "/" + numbers[3 : 6] + "/" + numbers[6 : 9]
        # print(path)
        gbAss = gBank + "_" + assembly
        path = path + "/" + gbAss + "/" + gbAss

        link = "https://ftp.ncbi.nlm.nih.gov/genomes/all/" + path + "_genomic.gbff.gz"
        allLinks = allLinks + link + ";" + refSeq + ".gbk\n"

print(allLinks)

