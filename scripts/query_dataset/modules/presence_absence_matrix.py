def compute_matrix(genomes: set, families: dict) -> list:
    """
    @brief Creates a binary presence/absence matrix for gene families across genomes
    
    @details Generates a 2D matrix where each row represents a gene family and
             each column represents a genome. A value of 1 indicates the presence
             of the genome in the family, and 0 indicates absence.
    
    @param genomes: List of genome identifiers
    @type genomes: list
    @param families: Dictionary of family information, generated using FamiliesHandler.get_families_as_dict()
    @type families: dict
    
    @return: 2D list representing the presence/absence matrix
    @rtype: list(list(int))
    """

    # create the matrix with len(families.keys()) as row number and len (genomes) as column number
    matrix = [[0 for _ in range(len(genomes))] for _ in range(len(families.keys()))]
    
    # fill the matrix
    for i, genome in enumerate(genomes):
        for j, family in enumerate(families.keys()):
            if genome in families[family]["genomes"]:
                matrix[j][i] = 1
    return matrix
