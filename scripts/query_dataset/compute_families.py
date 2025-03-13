import csv
def compute_genome_families(ifile, debug_flag = False):
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
                
            if debug_flag:
                print(f"fam_identifier: {fam_identifier}")
                print(f"genes: {non_empty_columns}")
                print(f"genomes: {families[fam_identifier]['genomes']}")
                print(f"genomes: {genomes}")
                print()
                families[fam_identifier]["genomes"] = list(families[fam_identifier]["genomes"])
    
    genomes = list(genomes)
    
    return genomes, families
