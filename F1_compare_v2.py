from itertools import combinations
import sys
import os

def parse_gene_families(file_path):
    """Parse ground truth gene families file"""
    print("Parsing ground truth file")
    edges = set()
    
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"Ground truth file not found: {file_path}")
    
    try:
        with open(file_path) as file:
            for line_num, line in enumerate(file, 1):
                line = line.strip()
                if not line: 
                    continue
                    
                parts = line.split()[1:] 
                nodes = []
                
                for p in parts:
                    try:
                        
                        if p.startswith('(') and p.endswith(')'):
                            
                            coord_str = p[1:-1] 
                            coords = [int(x.strip()) for x in coord_str.split(',')]
                            if len(coords) == 2:
                                nodes.append(tuple(coords))
                        else:
                            raise ValueError(f"Invalid format: {p}")
                    except (ValueError, IndexError) as e:
                        print(f"Warning: Could not parse '{p}' on line {line_num}: {e}")
                        continue
                
               
                edges.update(frozenset(e) for e in combinations(nodes, 2))
                
    except Exception as e:
        raise Exception(f"Error parsing gene families file: {e}")
    
    return edges

def parse_clus(file_path):
    """Parse pandelos (.clus) output file"""
    print("Parsing pandelos (.clus) output file")
    edges = set()
    
    if not os.path.exists(file_path):
        raise FileNotFoundError(f"Prediction file not found: {file_path}")
    
    try:
        with open(file_path) as file:
            for line_num, line in enumerate(file, 1):
                line = line.strip()
                if not line: 
                    continue
                    
                parts = line.split()
                nodes = []
                
                for p in parts:
                    try:
                       
                        components = p.split(":")
                        if len(components) >= 3:
                            genome_part = components[0]
                            gene_id = components[2]
                            
                           
                            if "_" in genome_part:
                                genome_num = int(genome_part.split("_")[1])
                            else:
                                genome_num = int(genome_part)
                            
                            nodes.append((genome_num, int(gene_id)))
                        else:
                            raise ValueError(f"Invalid format: {p}")
                            
                    except (ValueError, IndexError) as e:
                        print(f"Warning: Could not parse '{p}' on line {line_num}: {e}")
                        continue
                
                
                edges.update(frozenset(e) for e in combinations(nodes, 2))
                
    except Exception as e:
        raise Exception(f"Error parsing clus file: {e}")
    
    return edges

def compute_f1(pred_edges, true_edges):
    """Compute precision, recall, and F1-score"""
    print("Computing F1-score")
    
    tp = len(pred_edges & true_edges)
    fp = len(pred_edges - true_edges)
    fn = len(true_edges - pred_edges)
    
    precision = tp / (tp + fp) if (tp + fp) > 0 else 0.0
    recall = tp / (tp + fn) if (tp + fn) > 0 else 0.0
    f1 = 2 * precision * recall / (precision + recall) if (precision + recall) > 0 else 0.0
    
    return precision, recall, f1

def main():
    if len(sys.argv) not in [2, 3]:
        print("Usage: python3 F1_compare.py <predicted_file.clus> [ground_truth_file]")
        print("If ground_truth_file is not provided, 'salmon80.gene_families' will be used")
        sys.exit(1)
    
    pred_file = sys.argv[1]
    true_file = sys.argv[2] if len(sys.argv) == 3 else "salmon80.gene_families"
    
    try:
        print(f"Ground truth file: {true_file}")
        print(f"Prediction file: {pred_file}")
        
        true_edges = parse_gene_families(true_file)
        pred_edges = parse_clus(pred_file)
        
        print(f"\nStatistics:")
        print(f"True edges: {len(true_edges)}")
        print(f"Predicted edges: {len(pred_edges)}")
        
        precision, recall, f1 = compute_f1(pred_edges, true_edges)
        
        print(f"\nResults:")
        print(f"Precision: {precision:.4f}")
        print(f"Recall:    {recall:.4f}")
        print(f"F1-score:  {f1:.4f}")
        
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()