import json
from itertools import combinations
import sys

def parse_gene_families(filepath):
    families = []
    
    with open(filepath, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            parts = line.split()
            family_name = parts[0]
            
            # parse "(genome_id,gene_id)"
            genes = []
            for token in parts[1:]:
                token = token.strip('()')
                genome_id, gene_id = token.split(',')
                genes.append(f"({genome_id},{gene_id})")
            
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


clus_file = sys.argv[1] if len(sys.argv) > 1 else "example.gene_families"
output_file = sys.argv[2] if len(sys.argv) > 2 else "gene_family_pairs.json"

families = parse_gene_families(clus_file)

total_pairs = sum(f['total_pairs'] for f in families)
print(f"Processed families: {len(families)}")
print(f"Total pairs: {total_pairs}")

with open(output_file, 'w') as f:
    json.dump(families, f, indent=2)

print(f"Output written to {output_file}")