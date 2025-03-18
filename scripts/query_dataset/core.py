
import sys
import matplotlib.pyplot as plt
import numpy as np
import os

from modules.FamiliesHandler import FamiliesHandler
from modules.plots import pie_plot

plt.style.use("ggplot")

def compute_parts(families: dict, genomes, core_threshold):
    parts = {
        "core": {
            "count": 0,
            "families": set()
            },
        "accessory": {
            "count": 0,
            "families": set()
            },
        "singleton": {
            "count": 0,
            "families": set()
            }
    }
    for key, val in families.items():
        n_genomes = len(val["genomes"])
        if n_genomes == 1:
            parts["singleton"]["count"] += 1
            parts["singleton"]["families"].add(key)
        elif n_genomes >= core_threshold:
            parts["core"]["count"] += 1
            parts["core"]["families"].add(key)
        else:
            parts["accessory"]["count"] += 1
            parts["accessory"]["families"].add(key)
    return parts

def pie (parts:dict, ofolder: str, extension: str = "png"):
    core_count = parts["core"]["count"]
    accessory_count = parts["accessory"]["count"]
    singleton_count = parts["singleton"]["count"]
    
    labels = ["Core", "Accessory", "Singleton"]
    sizes = [core_count, accessory_count, singleton_count]
    colors = ['#4C72B0', '#55A868', '#C44E52']
    msgs = ["", "", ""]

    pie_plot(sizes, labels, colors, msgs, "Core, accessory and singleton pie", ofolder, "gene_type", extension)


def save(parts: dict, ofolder: str, save_type: str, fhandler: FamiliesHandler):
    folder = ofolder + "types/"
    if not os.path.exists(folder):
        os.makedirs(folder)
        
    if save_type == "all":
        write_list(parts, folder)
        write_multifasta(parts, folder, fhandler)
    elif save_type == "list":
        write_list(parts, folder)
    elif save_type == "multifasta":
        write_multifasta(parts, folder, fhandler)



def write_list(parts: dict, ofolder: str):
    folder = ofolder + "list/"
    if not os.path.exists(folder):
        os.makedirs(folder)
    
    for key, value in parts.items():
        with open(f"{folder}{key}.txt", "w") as f:
            for gene in value["families"]:
                f.write(gene + "\n")
            
def write_multifasta(parts: dict, ofolder: str, fhandler: FamiliesHandler):
    folder = ofolder + "multifasta/"
    
    if not os.path.exists(folder):
        os.makedirs(folder)
    
    gene_seq = fhandler.get_gene_sequence_production_as_dict()
    
    for key, value in parts.items():
        with open(f"{folder}{key}.ffn", "w") as f:
            for gene in value["families"]:
                f.write(">" + gene + " " + gene_seq[gene]["product"] + "\n")
                seq = gene_seq[gene]["sequence"]
                for i in range(0, len(seq), 60):
                    end = min(i+60, len(seq))
                    f.write(seq[i:end] + "\n")
    

def main():
    if len(sys.argv) < 4:
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.json <path_to_output_folder> <core_threshold> [list/multifasta/all]")
        exit(1)

    ifile = sys.argv[1]
    if not ifile.endswith(".json"):
        print("Input file must be a json file")
        exit(1)
    
    ofolder = sys.argv[2]
    if ofolder[-1] != "/":
        ofolder += "/"
    core_threshold = int(sys.argv[3])
    
    if len(sys.argv) == 5 and sys.argv[4] != "all" and sys.argv[4] != "list" and sys.argv[4] != "multifasta":
        print("Invalid argument")
        exit(1)
    
    save_type = ""
    if len(sys.argv) == 5 and sys.argv[4] == "all":
        save_type = "all"
    elif len(sys.argv) == 5 and sys.argv[4] == "list":
        save_type = "list"
    elif len(sys.argv) == 5 and sys.argv[4] == "multifasta":
        save_type = "multifasta"


    fhandler = FamiliesHandler(ifile)
    genomes = fhandler.get_genomes()
    families = fhandler.get_families_as_dict()
    
    parts = compute_parts(families, genomes, core_threshold)
    pie(parts, ofolder, "png")
    
    
    if save_type and len(save_type) != 0:
        save(parts, ofolder, save_type, fhandler)

if __name__ == "__main__":
    main()