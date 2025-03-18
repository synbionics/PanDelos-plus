
import sys
import matplotlib.pyplot as plt
import numpy as np

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

    pie_plot(sizes, labels, colors, msgs, "Core, accessory and singleton pie", ofolder, "pie", extension)

def write_files(parts: dict, ofolder: str):
    pass

def main():
    if len(sys.argv) < 4:
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.json <path_to_output_folder> <core_threshold> [save]")
        exit(1)

    ifile = sys.argv[1]
    ofolder = sys.argv[2]
    if ofolder[-1] != "/":
        ofolder += "/"
    core_threshold = int(sys.argv[3])
    if len(sys.argv) == 5 and sys.argv[4] != "save":
        print("Invalid argument")
        exit(1)
    if len(sys.argv) == 5 and sys.argv[4] == "save":
        save = True
    
    
    fhandler = FamiliesHandler(ifile)
    genomes = fhandler.get_genomes()
    families = fhandler.get_families_as_dict()
    
    parts = compute_parts(families, genomes, core_threshold)
    pie(parts, ofolder, "png")
    
    
    if save:
        write_files(parts, ofolder)

if __name__ == "__main__":
    main()