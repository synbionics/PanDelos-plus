#!/usr/bin/python3
"""
Evaluates gene family predictions against the ground truth.

Input:
  - gt_pairs.json:             output of parse_gene_families.py  (GT format)
  - predicted_converted.json:  output of parse_predicted_json.py (GT format)

Metrics computed on gene pairs:
  TP = pairs present in both
  FP = pairs in predicted but not in GT
  FN = pairs in GT but not in predicted
  Precision = TP / (TP + FP)
  Recall    = TP / (TP + FN)
  F1        = 2 * P * R / (P + R)
"""

import sys
import json
import matplotlib.pyplot as plt
import numpy as np
plt.style.use('ggplot')


def get_all_genes(gt_pairs, pred_pairs):
    """Collects all unique genes from both sets of pairs."""
    genes = set()
    for pair in gt_pairs | pred_pairs:
        genes.update(pair)
    return genes


def count_all_possible_pairs(genes):
    """Number of unique unordered pairs from n genes = n*(n-1)/2."""
    n = len(genes)
    return n * (n - 1) // 2

def load_pairs(filepath):
    """
    Loads a JSON of families and returns a set of frozenset{"(g1,gene1)", "(g2,gene2)"}.
    """
    with open(filepath) as f:
        families = json.load(f)

    pairs = set()
    for family in families:
        for pair in family.get('pairs', []):
            g1 = pair['gene_1']
            g2 = pair['gene_2']
            pairs.add(frozenset([g1, g2]))

    return pairs, len(families)


def evaluate(gt_pairs, pred_pairs):
    tp = len(gt_pairs & pred_pairs)
    fp = len(pred_pairs - gt_pairs)
    fn = len(gt_pairs - pred_pairs)

    
    
    precision = tp / (tp + fp) if (tp + fp) > 0 else 0.0
    recall    = tp / (tp + fn) if (tp + fn) > 0 else 0.0
    f1        = (2 * precision * recall / (precision + recall)
                 if (precision + recall) > 0 else 0.0)

    all_genes = get_all_genes(gt_pairs, pred_pairs)
    total_pairs = count_all_possible_pairs(all_genes)
    tn = total_pairs - tp - fp - fn
    
    return tp, fp, fn, tn, precision, recall, f1

import matplotlib.pyplot as plt
import numpy as np


def plot_confusion_matrix(tp, fp, fn, tn, output_path):
    """Plots and saves a 2x2 confusion matrix."""
    matrix = np.array([[tp, fn],
                       [fp, tn]])

    fig, ax = plt.subplots(figsize=(6, 5))
    im = ax.imshow(matrix, cmap="Blues")

    # Labels
    labels = [["TP", "FN"],
              ["FP", "TN"]]

    for i in range(2):
        for j in range(2):
            color = "white" if matrix[i, j] > matrix.max() / 2 else "black"
            ax.text(j, i, f"{labels[i][j]}\n{matrix[i][j]}",
                    ha="center", va="center", fontsize=14, color=color)

    ax.set_xticks([0, 1])
    ax.set_yticks([0, 1])
    ax.set_xticklabels(["Positive\n(Homologous)", "Negative\n(Not homologous)"])
    ax.set_yticklabels(["Positive\n(Homologs)", "Negative\n(Not homologous)"])
    ax.set_xlabel("Ground truth", fontsize=12)
    ax.set_ylabel("Predicted", fontsize=12)
    ax.set_title("Confusion matrix - gene pairs", fontsize=14)

    ax.grid(False)
    plt.colorbar(im)
    plt.tight_layout()
    plt.savefig(f"{output_path}.tiff", dpi=300, bbox_inches='tight', format='tiff')
    plt.savefig(f"{output_path}.png", dpi=300, bbox_inches='tight', format='png')
    plt.close()
    print(f"Confusion matrix saved to {output_path}")
    
if __name__ == "__main__":
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <gt_pairs.json> <predicted_converted.json> <image_output_path>")
        sys.exit(1)

    gt_file   = sys.argv[1]
    pred_file = sys.argv[2]
    output_path  = sys.argv[3]

    gt_pairs,   n_gt_families   = load_pairs(gt_file)
    pred_pairs, n_pred_families = load_pairs(pred_file)

    tp, fp, fn, tn, precision, recall, f1 = evaluate(gt_pairs, pred_pairs)
    plot_confusion_matrix(tp, fp, fn, tn, output_path=output_path)

    print(f"\nGround truth : {gt_file}   ({n_gt_families} families, {len(gt_pairs)} pairs)")
    print(f"Predictions  : {pred_file}   ({n_pred_families} families, {len(pred_pairs)} pairs)")
    print()
    print("── Results ──────────────────────────────────────────────────────────────────")
    print(f"  True Positives  (TP) : {tp}")
    print(f"  False Positives (FP) : {fp}")
    print(f"  False Negatives (FN) : {fn}")
    print(f"  True Negatives  (TN) : {tn}")
    print(f"  Precision            : {precision:.4f}")
    print(f"  Recall               : {recall:.4f}")
    print(f"  F1                   : {f1:.4f}")
    print("─────────────────────────────────────────────────────────────────────────────\n")