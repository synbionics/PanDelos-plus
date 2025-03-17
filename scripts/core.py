
import sys
import matplotlib.pyplot as plt
import numpy as np

plt.style.use("ggplot")

from query_dataset.compute_families import compute_genome_families


DEBUG = False


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

def pie (parts:dict, ofolder: str, extension = "png"):
    core_count = parts["core"]["count"]
    accessory_count = parts["accessory"]["count"]
    singleton_count = parts["singleton"]["count"]
    
    labels = ["Core", "Accessory", "Singleton"]
    sizes = [core_count, accessory_count, singleton_count]
    
    plt.figure(figsize=(8, 8))
    plt.pie(sizes, labels=labels, autopct="%1.1f%%", startangle=90)
    plt.title("Distribuzione delle Famiglie Genomiche")
    plt.axis("equal")  # Per mantenere il grafico a torta come un cerchio.
    
    # Salva il grafico nel folder di output
    plt.savefig(f"{ofolder}/pie_chart.{extension}")
    plt.close()
    
def main():
    if len(sys.argv) < 4:
        print(f"Usage: python3 {sys.argv[0]} <path_to_input_file>.clus <path_to_output_folder> <core_threshold>")
        exit(1)

    ifile = sys.argv[1]
    ofolder = sys.argv[2]
    if ofolder[-1] != "/":
        ofolder += "/"
    core_threshold = int(sys.argv[3])
    
    genomes, families = compute_genome_families(ifile, DEBUG)
    
    parts = compute_parts(families, genomes, core_threshold)
    pie(parts, ofolder, "pdf")

if __name__ == "__main__":
    main()