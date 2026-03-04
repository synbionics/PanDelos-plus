import json
from itertools import combinations
import sys

def parse_json_families(filepath):
    families = []
    
    with open(filepath, 'r') as f:
        data = json.load(f)
    
    for family in data:
        family_name = family["family-name"]
        
        genes = [gene["complete-identifier"] for gene in family["genes"]]
        
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


input_file = sys.argv[1] if len(sys.argv) > 1 else "families.json"
output_file = sys.argv[2] if len(sys.argv) > 2 else "json_family_pairs.json"

families = parse_json_families(input_file)

total_pairs = sum(f['total_pairs'] for f in families)
print(f"Processed families: {len(families)}")
print(f"Total pairs: {total_pairs}")

with open(output_file, 'w') as f:
    json.dump(families, f, indent=2)

print(f"Output written to {output_file}")