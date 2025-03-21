#!/usr/bin/python3

import sys
import csv

from modules.FamiliesHandler import FamiliesHandler
from modules.presence_absence_matrix import compute_matrix


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
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.json <path_to_output_folder>")
        exit(1)
    
    ifile = sys.argv[1]
    if not ifile.endswith(".json"):
        print("Input file must be a JSON file")
        exit(1)
    
    
    ofolder = sys.argv[2]
    if ofolder[-1] != "/":
        ofolder += "/"
    
    ofile = ofolder + "matrix.csv"
    
    fhandler = FamiliesHandler(ifile)
    
    # read json from ifile
    families = fhandler.get_families_as_dict()
    genomes = fhandler.get_genomes()
    
    
    matrix = compute_matrix(genomes, families)
    
    save_matrix(genomes, families, matrix, ofile)
    
if __name__ == "__main__":
    main()