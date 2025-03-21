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


def add_gbff_informations(formatted_json:json, gbkFolder: str):
    from Bio import SeqIO
    
    genome2cdstag = dict()
    cdstag2genome = dict()
    cdstag2product = dict()
    genesCoords = dict()
    sequences = dict()

    genomes = set()
    for fam in formatted_json:
        for gene in fam["genes"]:
            genomes.add(gene["genome-name"])
    
    for genome_id in genomes:
        ifile = gbkFolder + genome_id + ".gbk"
        genome_cdslist = genome2cdstag.get(genome_id, list())
        
        for record in SeqIO.parse(ifile, "genbank"):
            sequence_id = record.id

            for feature in record.features:
                if (feature.type == 'CDS'):
                    if ('translation' in feature.qualifiers):

                        direction = "?"
                        strand = feature.location.strand
                        if strand == 1:
                            direction = "+"
                        elif strand == -1:
                            direction = "-"
                        tag = (genome_id, sequence_id, feature.qualifiers['locus_tag'][0])
                        
                        genome_cdslist.append(tag)
                        cdstag2genome[tag] = genome_id
                        cdstag2product[tag] = (feature.qualifiers['product'][0]).replace('\t','')
                        genesCoords[tag] = {
                            "strand": direction,
                            "start": feature.location.start,
                            "end": feature.location.end
                        }
                        # todo: save sequence
                        sequences[tag] = feature.qualifiers['translation'][0]

        genome2cdstag[genome_id] = genome_cdslist
    
    allGenesWithCoords = {}

    uniques = dict()

    for k in sorted(cdstag2genome.keys()):
        gen_id = k[0]+":"+k[1]
        if gen_id not in uniques:
            uniques[ gen_id ] = dict()
		
        uniques[ gen_id ][k[2]] = uniques[ gen_id ].get(k[2],0) + 1
        cc = uniques[ gen_id ][k[2]]

        acc = k[0]+":"+k[1]+":"+k[2]+":"+str(cc)

        # ridontant informations are used to check already computed values
        current = {
            "complete-identifier": acc,
            "genome-name": k[0],
            "locus-version": k[1],
            "locus-tag": k[2],
            "product": cdstag2product[k],
            "coordinates": genesCoords[k],
            "sequence": sequences[k]
        }
        allGenesWithCoords[acc] = current

    # print(json.dumps(allGenesWithCoords, indent=4))
    new_tags = ["locus-version", "locus-tag", "coordinates"]
    check_tags = ["genome-name", "product", "sequence"]
    for fam in formatted_json:
        for gene in fam["genes"]:
            complete_id = gene["complete-identifier"]
            for tag in check_tags:
                assert(gene[tag] == allGenesWithCoords[complete_id][tag])
            for tag in new_tags:
                gene[tag] = allGenesWithCoords[complete_id][tag]
    
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
        if gbkFolder[-1] != "/":
            gbkFolder += "/"
        js2 = add_gbff_informations(js2, gbkFolder)
        
    with open(ofile, "w") as f:
        json.dump(js2, f, indent=4)
    
if __name__ == "__main__":
    main()