#!/usr/bin/python3

"""
@file genomes_matrix.py
@brief Generates a presence/absence matrix for gene families across genomes
@details This script processes genomic clustering data from a .clus file to
         generate a presence/absence matrix showing the distribution of gene
         families across different genomes. The matrix is saved as a CSV file
         where rows represent gene families and columns represent genomes.

@author [Simone Colli]
"""
import sys
import csv
import json


DEBUG = False
debug_families_output_file = "output/debug_families.json"
debug_matrix_output_file = "output/debug_matrix.csv"

def compute_genome_families(ifile):
    """
    @brief Parses the input file to identify all genomes and gene families
    
    @details Reads the input clustering file and extracts information about
             gene families and their distribution across genomes. Each line
             in the input file represents a gene family, with the first gene
             serving as the family identifier.
    
    @param ifile: Path to the input clustering file
    @type ifile: str
    
    @return: A tuple containing:
             - List of all unique genome identifiers
             - Dictionary of family information with gene lists and genome distributions
    @rtype: tuple(list, dict)
    """
    
    genomes = set()
    # the first gene related to the family is the represent of the family
    families = dict()
    
    with open(ifile, "r") as f:
        reader = csv.reader(f, delimiter=" ")
        for line in reader:
            
            non_empty_columns = [col.strip() for col in line if col.strip() != ""]
            if len(non_empty_columns) == 0:
                continue
            
            fam_identifier = non_empty_columns[0]
            families[fam_identifier] = {
                "genes" : non_empty_columns,
                "genomes": set()
            }
            
            for gene in non_empty_columns:
                genome = gene.split(":")[0]
                families[fam_identifier]["genomes"].add(genome)
                genomes.add(genome)
                
            if DEBUG:
                print(f"fam_identifier: {fam_identifier}")
                print(f"genes: {non_empty_columns}")
                print(f"genomes: {families[fam_identifier]['genomes']}")
                print(f"genomes: {genomes}")
                print()
                families[fam_identifier]["genomes"] = list(families[fam_identifier]["genomes"])
    
    genomes = list(genomes)
    
    return genomes, families

def compute_matrix(genomes, families):
    """
    @brief Creates a binary presence/absence matrix for gene families across genomes
    
    @details Generates a 2D matrix where each row represents a gene family and
             each column represents a genome. A value of 1 indicates the presence
             of the genome in the family, and 0 indicates absence.
    
    @param genomes: List of genome identifiers
    @type genomes: list
    @param families: Dictionary of family information
    @type families: dict
    
    @return: 2D list representing the presence/absence matrix
    @rtype: list(list(int))
    """
    
    if DEBUG:
        for family in families:
            families[family]["genomes"] = set(families[family]["genomes"])

    # create the matrix with len(families.keys()) as row number and len (genomes) as column number
    matrix = [[0 for _ in range(len(genomes))] for _ in range(len(families.keys()))]
    
    # fill the matrix
    for i, genome in enumerate(genomes):
        for j, family in enumerate(families.keys()):
            if genome in families[family]["genomes"]:
                matrix[j][i] = 1
    return matrix

def save_matrix(genomes, families, matrix, ofile):
    """
    @brief Writes the presence/absence matrix to a CSV file
    
    @details Saves the generated matrix to a CSV file where the first row
             contains genome identifiers and the first column contains
             family identifiers.
    
    @param genomes: List of genome identifiers (used as column headers)
    @type genomes: list
    @param families: Dictionary of family information (keys used as row labels)
    @type families: dict
    @param matrix: 2D matrix of presence/absence values
    @type matrix: list(list(int))
    @param ofile: Path to output CSV file
    @type ofile: str
    """
    
    with open(ofile, "w") as f:
        writer = csv.writer(f)
        writer.writerow([""] + genomes)
        for i, family in enumerate(families.keys()):
            writer.writerow([family] + matrix[i])



def main():
    """
    @brief Main function that orchestrates the workflow
    
    @details This function:
             1. Parses command-line arguments
             2. Calls compute_genome_families() to process input data
             3. Calls compute_matrix() to generate the presence/absence matrix
             4. Calls save_matrix() to save results
    
    @return: None
    """
    
    if len(sys.argv) < 3:
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.clus <path_to_output_file>.csv")
        exit(1)
    
    ifile = sys.argv[1]
    ofile = sys.argv[2]
    
    # check input file is a .clus file
    genomes, families = compute_genome_families(ifile)
    
    if DEBUG:
        print(f"founded genomes: {genomes}")
        with open(debug_families_output_file, "w") as f:
            json.dump(families, f, indent=4)
        
    matrix = compute_matrix(genomes, families)
    
    if DEBUG:
        with open(debug_matrix_output_file, "w") as f:
            writer = csv.writer(f, delimiter=" ")
            writer.writerows(matrix)

    save_matrix(genomes, families, matrix, ofile)
    
if __name__ == "__main__":
    main()