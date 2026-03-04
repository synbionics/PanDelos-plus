#!/usr/bin/python3
"""
Converts the JSON output of PanDelos-plus into the PANPROVA ground truth format.

Input format (predicted):
  gene: "genome_0:genome_0:42:1"  (complete-identifier)

Output format (aligned to GT):
  gene: "(0,42)"
  family_name: "genome_0:genome_0:0:1"  (unchanged)

Conversion:
  "genome_0:genome_0:42:1"
   parts = split(':')
   genome_id = int(parts[0].replace('genome_', ''))  -> 0
   gene_id   = int(parts[2])                          -> 42
   -> "(0,42)"
"""

import sys
import json
from itertools import combinations


def ci_to_gt_gene(ci):
    """
    "genome_0:genome_0:42:1" -> "(0,42)"
    """
    parts = ci.split(':')
    genome_id = int(parts[0].replace('genome_', ''))
    gene_id   = int(parts[2])
    return f"({genome_id},{gene_id})"


def parse_predicted_json(filepath):
    with open(filepath) as f:
        data = json.load(f)

    families = []
    for family in data:
        family_name = family.get('family_name', '')

        # convert each gene from complete-identifier to GT format
        genes = [ci_to_gt_gene(ci) for ci in family.get('genes', [])]

        pairs = [
            {"gene_1": g1, "gene_2": g2}
            for g1, g2 in combinations(genes, 2)
        ]

        families.append({
            "family_name": family_name,
            "genes": genes,
            "total_pairs": len(pairs),
            "pairs": pairs
        })

    return families


if __name__ == "__main__":
    input_file  = sys.argv[1] if len(sys.argv) > 1 else "families.json"
    output_file = sys.argv[2] if len(sys.argv) > 2 else "predicted_converted.json"

    families = parse_predicted_json(input_file)

    total_pairs = sum(f['total_pairs'] for f in families)
    print(f"Families processed: {len(families)}")
    print(f"Total pairs:        {total_pairs}")

    with open(output_file, 'w') as f:
        json.dump(families, f, indent=2)

    print(f"Output written to {output_file}")