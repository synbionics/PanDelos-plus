#!/usr/bin/python3

import sys
import os

from modules.FamiliesHandler import FamiliesHandler
from modules.compute_diffusivity import compute_diffusivity
from modules.plots import pie_plot, hist_plot
from modules.colors import colors

def pie(diffs, ofolder, plot_type="png"):
    
    labels = []
    data = []
    for dif, content in diffs.items():
        labels.append(dif)
        data.append(content["number_of_families"])
    msgs = ["diffusivity"] * len(labels)
    
    pie_plot(data, labels, [], msgs, "Family diffusivity", ofolder, "pie_family_diffusivity", plot_type)
    

def hist(diffs: dict, ofolder: str, extension="png"):
    x = list()
    y = list()
    for dif, data in diffs.items():
        x.append(dif)
        y.append(data["number_of_families"])

    hist_plot(x,y,colors["med-blue"], "Number of genomes (diffusivity)", "Number of gene families", "Family diffusivity", ofolder, "hist_family_diffusivity", extension, x)




def save(diff: dict, ofolder: str, save_type: str, fhandler: FamiliesHandler):
    folder = ofolder + "diffusivity/"
    if not os.path.exists(folder):
        os.makedirs(folder)
        
    if save_type == "all":
        write_list(diff, folder)
        write_multifasta(diff, folder, fhandler)
    elif save_type == "list":
        write_list(diff, folder)
    elif save_type == "multifasta":
        write_multifasta(diff, folder, fhandler)



def write_list(diffs: dict, ofolder: str):
    folder = ofolder + "list/"
    if not os.path.exists(folder):
        os.makedirs(folder)
    # identifiers
    for key, value in diffs.items():
        file = f"{folder}diffusivity_{str(key)}.txt"
        with open(file, "w") as f:
            for gene in value["families"]:
                f.write(gene + "\n")

def write_multifasta(diffs: dict, ofolder: str, fhandler: FamiliesHandler):
    folder = ofolder + "multifasta/"
    
    if not os.path.exists(folder):
        os.makedirs(folder)
    
    gene_seq = fhandler.get_gene_sequence_production_as_dict()
    
    
    for key, value in diffs.items():
        file = f"{folder}diffusivity_{str(key)}.ffn"
        with open(file, "w") as f:
            for gene in value["families"]:
                f.write(">" + gene + " " + gene_seq[gene]["product"] + "\n")
                seq = gene_seq[gene]["sequence"]
                for i in range(0, len(seq), 60):
                    end = min(i+60, len(seq))
                    f.write(seq[i:end] + "\n")
    
    # # multifasta
    # for key, value in diffs.items():
    #     file = folder + str(key) + ".ffn"

def main():

    if len(sys.argv) < 3:
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.json <path_to_output_folder> [list/multifasta/all]")
        exit(1)
    
    ifile = sys.argv[1]
    if not ifile.endswith(".json"):
        print("Input file must be a json file")
        exit(1)
    
    ofolder = sys.argv[2]
    if ofolder[-1] != "/":
        ofolder += "/"
    
    if len(sys.argv) == 4 and sys.argv[3] != "all" and sys.argv[3] != "list" and sys.argv[3] != "multifasta":
        print("Invalid argument")
        exit(1)
    
    save_type = ""
    if len(sys.argv) == 4 and sys.argv[3] == "all":
        save_type = "all"
    elif len(sys.argv) == 4 and sys.argv[3] == "list":
        save_type = "list"
    elif len(sys.argv) == 4 and sys.argv[3] == "multifasta":
        save_type = "multifasta"

    fhandler = FamiliesHandler(ifile)
    genomes = fhandler.get_genomes()
    families = fhandler.get_families_as_dict()
    
    # 1. determine the diffussivity of each gene families, so the number of genomes in which it is present
    # 2. group family by diffusivity, so crate a histogram with on x axis number in range 1 to |genomes|,
    # the y axis number of families that appear in that number of genomes
    # example: 10 families with diffusivity 1, 100 families with diffusivity 2, the plot will
    # have the y value for diffusivity 1 at 10 and the y value for diffusivity 2 at 100
    _, dif =  compute_diffusivity(families, len(genomes))
    
    # 3. plot the histogram of how many families are present in how many genomes
    hist(dif, ofolder, "png")
    pie(dif, ofolder, "png")
    
    if save_type and len(save_type) != 0:
        save(dif, ofolder, save_type, fhandler)

if __name__ == "__main__":
    main()

