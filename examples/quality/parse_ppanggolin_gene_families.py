#!/usr/bin/python3
"""
Parses PPanGGOLiN per-genome TSV files (ppanggolin write_genomes --table)
and converts them to the PANPROVA ground truth format for evaluation.

Input:  directory of TSV files (one per genome).
        Relevant columns:
          0: gene (locus_tag)
          5: family

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
  genome name (TSV filename stem): "genome_0" -> genome_id = 0
  locus_tag   (column 0):          "42"        -> gene_id  = 42
  result: "(0,42)"
"""

import json
import sys
from collections import defaultdict
from itertools import combinations
from pathlib import Path


def locus_to_gt_gene(genome_name, locus_tag):
    """
    "genome_0", "42" -> "(0,42)"
    """
    genome_id = int(genome_name.replace("genome_", ""))
    gene_id = int(locus_tag)
    return f"({genome_id},{gene_id})"


def parse_ppanggolin_tables(table_dir):
    family2genes = defaultdict(list)

    for tsv_file in sorted(Path(table_dir).glob("*.tsv")):
        genome_name = tsv_file.stem
        with open(tsv_file) as f:
            f.readline()  # skip header
            for line in f:
                parts = line.strip().split("\t")
                if len(parts) < 6:
                    continue
                locus_tag = parts[0]
                family = parts[5]
                gt_gene = locus_to_gt_gene(genome_name, locus_tag)
                family2genes[family].append(gt_gene)

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
    table_dir   = sys.argv[1] if len(sys.argv) > 1 else "table"
    output_file = sys.argv[2] if len(sys.argv) > 2 else "ppanggolin_gene_families.json"

    families = parse_ppanggolin_tables(table_dir)

    total_pairs = sum(f["total_pairs"] for f in families)
    print(f"Processed families: {len(families)}")
    print(f"Total pairs:        {total_pairs}")

    with open(output_file, "w") as f:
        json.dump(families, f, indent=2)

    print(f"Output written to {output_file}")
