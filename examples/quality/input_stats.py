#!/usr/bin/env python3

import argparse
import os
import statistics

not_wanted = ["mycoplasma5"]

def parse_input_file(file_path):
    """Parse the input file and count genes per genome."""
    data = {}
    with open(file_path, "r") as f:
        lines = f.readlines()
    i = 0
    while i < len(lines):
        header_line = lines[i].strip()
        parts = header_line.split("\t")
        if parts:
            genome_id = parts[0]
            data[genome_id] = data.get(genome_id, 0) + 1
        i += 2  # skip sequence line
    return data


def compute_stats(counts):
    """Compute summary statistics from a list of counts."""
    num_genomes = len(counts)
    total_genes = sum(counts)
    min_genes = min(counts) if counts else 0
    max_genes = max(counts) if counts else 0
    mean_genes = statistics.mean(counts) if counts else 0
    std_genes = statistics.stdev(counts) if len(counts) > 1 else 0
    return {
        "num_genomes": num_genomes,
        "total_genes": total_genes,
        "min_genes": min_genes,
        "max_genes": max_genes,
        "mean_genes": mean_genes,
        "std_genes": std_genes,
    }


def iterate_datasets(dir_path):
    aggregated = {}
    for root, _, files in os.walk(dir_path):
        for fname in files:
            print(f"Processing {fname}")
            cut = fname.split(".")[0]
            if cut in not_wanted:
                print(f"Skipping {cut} as it is in the not_wanted list.")
                continue
            file_path = os.path.join(root, fname)
            data = parse_input_file(file_path)
            counts = list(data.values())
            stats = compute_stats(counts)
            aggregated[cut] = stats
    
    return aggregated



def print_latex(data, output_file="output.tex"):
    """Generate a LaTeX table from stats dict and write to a file."""
    lines = []
    lines.append("\\begin{table}[htbp]")
    lines.append("    \\centering")
    lines.append("    \\caption{Overview of datasets used in the tests.}")
    lines.append("    \\label{tab:ds-overview}")
    lines.append("    \\begin{tabularx}{\\textwidth}{L Y Y Y Y Y Y}")
    lines.append("        \\toprule")
    lines.append(
        "        Dataset   & Number of genomes & Total number of genes & Max. number of genes "
    )
    lines.append(
        "                  & Min. number of genes & Mean & Standard deviation \\\\"
    )
    lines.append("        \\midrule")
    for label, stats in data.items():
        num = stats['num_genomes']
        tot = stats['total_genes']
        mx = stats['max_genes']
        mn = stats['min_genes']
        m = stats['mean_genes']
        s = stats['std_genes']
        lines.append(
            f"        {label} & {num} & {tot} & {mx} & {mn} & {m:.2f} & {s:.2f} \\\\"
        )
    lines.append("        \\bottomrule")
    lines.append("    \\end{tabularx}")
    lines.append("\\end{table}")

    with open(output_file, "w") as f:
        f.write("\n".join(lines))
    print(f"LaTeX table written to {output_file}")

def main():

    dir_path = "./input_pdi/"
    out = iterate_datasets(dir_path)
    # print (out)
    print_latex(out)



if __name__ == "__main__":
    main()