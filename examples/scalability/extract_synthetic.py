import sys
import os


def extract_ds(path, sizes, opath):
    genomes = dict() # the last line of each genome
    
    prev_genome = ""
    with open(path, 'r') as f:
        i = 0
        for line in f:
            if i % 2 == 0:
                genome = line.strip().split()[0].strip()
                if genome != prev_genome:
                    # print(f"Genome: {genome}")
                    if prev_genome not in genomes and prev_genome != "":
                        genomes[prev_genome] = i
                    prev_genome = genome
            i += 1
    genomes[genome] = i
    
    print(f"Number of genomes: {len(genomes)}")
    # print(f"{genomes}")
    
    genome_ends = list(genomes.values())
    genome_ends.sort()
        # genome_ends.remove(0)
    # print(f"{genome_ends}")
    # print(f"{len(genome_ends)}")
    
    for s in sizes:
        filename = f"{opath}{s}.pdi"
        if s - 1 < len(genome_ends):
            n = s - 1
        else:
            n = len(genome_ends) - 1
        
        nlines = genome_ends[n]
        with open(filename, 'w') as fout:

            with open(path, 'r') as fin:
                for l, linea in enumerate(fin):
                    if l < nlines:
                        fout.write(linea)
                    else:
                        break

def main():
    
    path = sys.argv[1]
    opath = sys.argv[2]
    if opath[-1] != "/":
        opath += "/"
    
    sizes = [50 * i for i in range(1, 13)]
    extract_ds(path, sizes, opath)

main()