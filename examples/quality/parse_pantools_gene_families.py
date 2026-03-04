#!/usr/bin/python3
"""
Parses PanTools group_info CSV output and converts it to the PANPROVA ground
truth format for evaluation.

Input:
  csv_path     -- ${PANTOOLS_DB}/group_info/group_info.csv
  genomes_file -- ${PANTOOLS_OUTPUTDIR}/genomes.txt  (one fasta path per line,
                  1-based line number = PanTools genome index)

Output: JSON list of families in the same structure used by evaluate.py:
  [
    {
      "family_name": "...",
      "genes": ["(0,42)", "(1,7)", ...],
      "total_pairs": N,
      "pairs": [{"gene_1": "(0,42)", "gene_2": "(1,7)"}, ...]
    },
    ...
  ]

Conversion to PANPROVA gene format:
  genome_name (from genomes_file stem): "genome_0" -> genome_id = 0
  gene_id (CSV parts[0] strip "-gene"):  "42"        -> gene_id  = 42
  result: "(0,42)"
"""

import json
import sys
from collections import defaultdict
from itertools import combinations
from pathlib import Path


def load_genome_map(genomes_file):
    """
    Build a mapping from 1-based PanTools genome index to genome name.
    Each line in genomes_file is a path: /path/to/genome_0.fasta
    """
    genome_map = {}
    with open(genomes_file) as f:
        for i, line in enumerate(f, start=1):
            path = line.strip()
            name = Path(path).stem
            genome_map[str(i)] = name
    return genome_map


def gene_to_gt(gene_id, genome_name):
    """
    gene_id    (after stripping "-gene"): "42"       -> 42
    genome_name (from genomes_file stem): "genome_0" -> 0
    result: "(0,42)"
    """
    genome_idx = int(genome_name.replace("genome_", ""))
    gene_idx = int(gene_id)
    return f"({genome_idx},{gene_idx})"


def parse_pantools_csv(csv_path, genomes_file):
    genome_map = load_genome_map(genomes_file)

    family2genes = defaultdict(list)
    current_group = None

    with open(csv_path) as f:
        for line in f:
            line = line.strip()
            if line.startswith("#Homology group"):
                current_group = line.split("#Homology group ")[1]
                continue
            if line.startswith("Gene id") or not line or current_group is None:
                continue

            parts = line.split(",")
            if len(parts) < 5:
                continue

            gene_id = parts[0].replace("-gene", "")
            genome_number = parts[4].strip()
            genome_name = genome_map.get(genome_number, f"genome_{genome_number}")

            gt_gene = gene_to_gt(gene_id, genome_name)
            family2genes[current_group].append(gt_gene)

    families = []
    for family_name, genes in sorted(family2genes.items()):
        pairs = [
            {"gene_1": g1, "gene_2": g2}
            for g1, g2 in combinations(genes, 2)
        ]
        families.append({
            "family_name": family_name,
            "genes": genes,
            "total_pairs": len(pairs),
            "pairs": pairs,
        })

    return families


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: {sys.argv[0]} <group_info.csv> <genomes.txt> [output.json]")
        sys.exit(1)

    csv_path     = sys.argv[1]
    genomes_file = sys.argv[2]
    output_file  = sys.argv[3] if len(sys.argv) > 3 else "pantools_gene_families.json"

    families = parse_pantools_csv(csv_path, genomes_file)

    total_pairs = sum(f["total_pairs"] for f in families)
    print(f"Processed families: {len(families)}")
    print(f"Total pairs:        {total_pairs}")

    with open(output_file, "w") as f:
        json.dump(families, f, indent=2)

    print(f"Output written to {output_file}")
