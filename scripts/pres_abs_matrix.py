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
from query_dataset.compute_families import compute_genome_families
from query_dataset.presence_absence_matrix import compute_matrix


DEBUG = True
debug_families_output_file = "output/debug_families.json"
debug_matrix_output_file = "output/debug_matrix.csv"


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
    genomes, families = compute_genome_families(ifile, DEBUG)
    
    if DEBUG:
        print(f"founded genomes: {genomes}")
        with open(debug_families_output_file, "w") as f:
            json.dump(families, f, indent=4)
        
    matrix = compute_matrix(genomes, families, DEBUG)
    
    if DEBUG:
        with open(debug_matrix_output_file, "w") as f:
            writer = csv.writer(f, delimiter=" ")
            writer.writerows(matrix)

    save_matrix(genomes, families, matrix, ofile)
    
if __name__ == "__main__":
    main()