import os
import json


class FamiliesHandler:
    """
    @brief Class for handling families data

    @details This class provides methods to load and manipulate families data.
    """
    def __init__(self, ifile: str):
        """
        @brief Initialize FamiliesHandler with a JSON file
        @param ifile: Path to the JSON file containing families data
        @type ifile: str
        """
        if not os.path.exists(ifile):
            raise FileNotFoundError(f"File {ifile} not found")
        self.ifile = ifile
        self.families = None
        self.genomes = None

    def get_families(self) -> json:
        
        if not self.families:
            self.load_families()
        
        return self.families
    
    def get_genomes(self) -> list:
        
        if not self.genomes:
            self.load_genomes()
        return self.genomes

    def load_genomes(self) -> None:
        
        if not self.families:
            self.load_families()
        
        genomes = set()
        
        for family in self.families:
            for gene in family["genes"]:
                genomes.add(gene["genome-name"])
        
        self.genomes = list(genomes)
        
    def load_families(self) -> None:
        
        if not os.path.exists(self.ifile):
            raise FileNotFoundError(f"File {self.ifile} not found")
        
        with open(self.ifile, "r") as f:
            families = json.load(f)
        if not families:
            raise ValueError(f"File {self.ifile} is empty")
        
        self.families = families
        
    def get_families_as_dict(self) -> dict:
        if not self.families:
            self.load_families()
        
        data = dict()
        for family in self.families:
            family_id = family["family-name"]
            if family_id not in data:
                data[family_id] = {
                    "genomes": set(),
                    "genes": set()
                }
            for gene in family["genes"]:
                data[family_id]["genomes"].add(gene["genome-name"])
                data[family_id]["genes"].add(gene["complete-identifier"])
        return data
