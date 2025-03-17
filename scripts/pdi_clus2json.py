#!/usr/bin/python3
import sys
import json
import csv

DEBUG = False

def compute_families(ifile: str):
    families = list()
    
    with open(ifile, "r") as f:
        reader = csv.reader(f, delimiter=" ")
        for line in reader:
            
            non_empty_columns = [col.strip() for col in line if col.strip() != ""]
            if len(non_empty_columns) == 0:
                continue
            
            currentFamily = dict()
            
            for gene in non_empty_columns:
                if "family-name" not in currentFamily:
                    currentFamily["family-name"] = gene
                    currentFamily["genes"] = list()
                
                splittedGene = gene.strip().split(":")
                currentGene = {
                    "complete-identifier": gene,
                    "genome-name": splittedGene[0]
                }
                currentFamily["genes"].append(currentGene)

            families.append(currentFamily)
        
    return families

def add_informations(formatted_json:json, ipdi_file: str):
    pdi_data = dict()
    
    with open(ipdi_file, "r") as f:
        reader = csv.reader(f, delimiter="\t")
        header = True
        last_header_gene = ""
        for line in reader:
            if len(line) == 0:
                continue
            
            if header:
                pdi_data[line[1]] = {
                    "sequence": "",
                    "product": line[2],
                }
                last_header_gene = line[1]
                header = False
            else:
                pdi_data[last_header_gene]["sequence"] = line[0]
                header = True
    
    # print(pdi_data)
    for family in formatted_json:
        for gene in family["genes"]:
            if gene["complete-identifier"] in pdi_data:
                gene["product"] = pdi_data[gene["complete-identifier"]]["product"]
                gene["sequence"] = pdi_data[gene["complete-identifier"]]["sequence"]

    return formatted_json

def main():
    if len(sys.argv) < 4:
        print(f"Usage: python3 {sys.argv[0]} <path_to_clus_file> <path_to_pdi_file> <path_to_output_file>.json")
        exit(1)
    
    iclus_file = sys.argv[1]
    ipdi_file = sys.argv[2]
    ofile = sys.argv[3]
    
    gbkFolder = ""
    if len(sys.argv) == 5:
        gbkFolder = sys.argv[4]
    
    
    families = compute_families(iclus_file)
    js2 = add_informations(families, ipdi_file)
    
    # print(families)
    if DEBUG:
        print(json.dumps(js2, indent=4))
    
    
    if gbkFolder and gbkFolder != "":
        # TODO add more informations taken from gbk and gbff
        # 
        pass
    
    with open(ofile, "w") as f:
        json.dump(js2, f, indent=4)
    
if __name__ == "__main__":
    main()