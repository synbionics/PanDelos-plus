#!/usr/bin/python3

import sys

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
    

def hist(diffs, ofolder, extension="png"):
    x = list()
    y = list()
    for dif, data in diffs.items():
        x.append(dif)
        y.append(data["number_of_families"])

    hist_plot(x,y,colors["med-blue"], "Number of genomes (diffusivity)", "Number of gene families", "Family diffusivity", ofolder, "hist_family_diffusivity", extension, x)

def main():

    if len(sys.argv) < 3:
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.json <path_to_output_folder>")
        exit(1)
    
    ifile = sys.argv[1]
    if not ifile.endswith(".json"):
        print("Input file must be a json file")
        exit(1)
    
    ofolder = sys.argv[2]
    if ofolder[-1] != "/":
        ofolder += "/"
    
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
    hist(dif, ofolder, "pdf")
    pie(dif, ofolder, "pdf")
    

if __name__ == "__main__":
    main()

