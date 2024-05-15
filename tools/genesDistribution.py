#!/usr/bin/python3

import sys
import os
import matplotlib.pyplot as plt



filePath = sys.argv[1]
fileName = os.path.splitext(os.path.basename(filePath))[0]

genomes = dict()
genome_genes = dict()

with open(filePath, "r") as file:
    i = 0
    lastGenome = ""
    lastGenomeId = -1
    
    lastGene = ""
    for line in file:
        
        if i % 2 == 0: # info line
            parts = line.split("\t")
            # print(parts)

            if lastGenome != parts[0]: # new genome
                lastGenome = parts[0]
                lastGenomeId = lastGenomeId + 1
                genomes[lastGenome] = lastGenomeId
                genome_genes[lastGenomeId] = 0
                lastGene = ""
            
            if lastGene != parts[1]:
                genome_genes[lastGenomeId] += 1
        
        i += 1

# print(genomes)
# print()
# print()
# print(genome_genes)

x = list(genomes.keys())
y = list(genome_genes.values())

# print(x)
# print(y)

plt.title("Genes distributions")
plt.ylabel("Genes number")
plt.xlabel("Genomes")
plt.bar(x, y)

# plt.yticks(y)
plt.yticks(fontsize=8)

plt.xticks(rotation=90)
plt.xticks(range(len(x)), x, fontsize=8, ha='right')

plt.tight_layout()
plt.savefig(fileName + ".png")