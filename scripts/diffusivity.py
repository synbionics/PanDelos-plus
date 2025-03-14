#!/usr/bin/python3
DEBUG = False

import sys
import json
import matplotlib.pyplot as plt
import numpy as np

plt.style.use('ggplot')

from query_dataset.compute_families import compute_genome_families
from query_dataset.colors import colors

debug_families_diffusivity_output_file = "debug_families_diffusivity.json"
debug_diffusivity_output_file = "debug_diffusivity.json"

def compute_diffusivity(families, number_of_genomes):
    fam_diffusivity = dict()
    diffusivity = dict()
    for i in range(number_of_genomes + 1):
        diffusivity[i] = {
            "number_of_families": 0,
            "families": list()
        }
    
    # print(type(families))
    for family_id, families in families.items():
        d = len(families["genomes"])
        fam_diffusivity[family_id] = d
        diffusivity[d]["number_of_families"] += 1
        diffusivity[d]["families"].append(family_id)

    if diffusivity[0]["number_of_families"] == 0:
        del diffusivity[0]
    
    return [fam_diffusivity, diffusivity]

def pie(diffs, ofolder, plot_type="png"):
    fig, ax = plt.subplots(figsize=(6, 3), subplot_kw=dict(aspect="equal"))
    
    labels = []
    data = []
    for dif, content in diffs.items():
        labels.append(dif)
        data.append(content["number_of_families"])
    
    # pie chart, con bordo per ciascuna fetta
    wedges, texts = ax.pie(
        data,
        wedgeprops=dict(width=0.5, edgecolor='black', linewidth=0.3),
        startangle=-40
    )
    
    total = sum(data)
    bbox_props = dict(boxstyle="square,pad=0.3", fc="w", ec="k", lw=0.72)
    kw = dict(arrowprops=dict(arrowstyle="-"),
              bbox=bbox_props, zorder=0, va="center")
    
    # per ogni fetta, calcolo della percentuale e creazione label
    for i, p in enumerate(wedges):
        # angolo medio per posizionare la label
        ang = (p.theta2 - p.theta1) / 2. + p.theta1
        y = np.sin(np.deg2rad(ang))
        x = np.cos(np.deg2rad(ang))
        horizontalalignment = {-1: "right", 1: "left"}[int(np.sign(x))]
        connectionstyle = f"angle,angleA=0,angleB={ang}"
        kw["arrowprops"].update({"connectionstyle": connectionstyle})
        
        # percentuale
        perc = data[i] / total * 100
        new_label = f"{perc:.2f}% diffusivity {labels[i]}"
        
        ax.annotate(new_label, xy=(x, y), xytext=(1.35 * np.sign(x), 1.4 * y),
                    horizontalalignment=horizontalalignment, **kw)
    
    ax.set_title("Family diffusivity")
    plt.savefig(ofolder + "pie_family_diffusivity." + plot_type)

def hist(diffs, ofolder, plot_type="png"):
    x = list()
    y = list()
    for dif, data in diffs.items():
        x.append(dif)
        y.append(data["number_of_families"])

    plt.bar(x, y, width=0.8, color=colors["med-blue"],linewidth=0.3, edgecolor="black")
    plt.xlabel("Number of genomes")
    plt.ylabel("Number of gene families")
    plt.savefig(ofolder + "hist_family_diffusivity." + plot_type)
    

def main():

    # ???? Keep the number of genomes as an argument ????
    # If i keep i can use to create a pie chart to show the percentage of
    # genomes that have a certain number of families -> can be known as genome coverage
    
    if len(sys.argv) < 4:
        print(f"Usage: python3 {sys.argv[0]} <number_of_genomes> <path_to_input_file>.clus <path_to_output_folder>")
        exit(1)
    
    number_of_genomes = int(sys.argv[1])
    ifile = sys.argv[2]
    ofolder = sys.argv[3]
    if ofolder[-1] != "/":
        ofolder += "/"
    
    genomes, families = compute_genome_families(ifile, DEBUG)

    
    # 1. determine the diffussivity of each gene families, so the number of genomes in which it is present
    # 2. group family by diffusivity, so crate a histogram with on x axis number in range 1 to |genomes|,
    # the y axis number of families that appear in that number of genomes
    # example: 10 families with diffusivity 1, 100 families with diffusivity 2, the plot will
    # have the y value for diffusivity 1 at 10 and the y value for diffusivity 2 at 100
    fam_dif, dif =  compute_diffusivity(families, number_of_genomes)
    
    if DEBUG:
        with open(ofolder + debug_families_diffusivity_output_file, "w") as f:
            json.dump(fam_dif, f, indent=4)
            
        with open(ofolder + debug_diffusivity_output_file, "w") as f:
            json.dump(dif, f, indent=4)
    
    # 3. plot the histogram of how many families are present in how many genomes
    hist(dif, ofolder, "pdf")
    pie(dif, ofolder, "pdf")
    

if __name__ == "__main__":
    main()

