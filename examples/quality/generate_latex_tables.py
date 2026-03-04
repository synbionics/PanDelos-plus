#!/usr/bin/python3
"""
Parses evaluation output text files and generates LaTeX tables.

Usage:
    python generate_latex_tables.py <input_file.txt> [--output output.tex]

The input file should contain one or more evaluation blocks with this structure:

    Ground truth : <path>   (<N> families, <M> pairs)
    Predictions  : <path>   (<N> families, <M> pairs)

    ── Results ──...
      True Positives  (TP) : <val>
      False Positives (FP) : <val>
      False Negatives (FN) : <val>
      True Negatives  (TN) : <val>
      Precision            : <val>
      Recall               : <val>
      F1                   : <val>
    ────...
"""

import re
import sys
import argparse


def parse_results(text):
    """Parse evaluation output text into a list of result dicts."""
    results = []

    # Split into blocks by "Ground truth" occurrences
    blocks = re.split(r'(?=Ground truth\s*:)', text)
    blocks = [b.strip() for b in blocks if b.strip() and 'Ground truth' in b]

    for block in blocks:
        r = {}

        # Ground truth info
        gt_match = re.search(
            r'Ground truth\s*:\s*(\S+)\s+\((\d+)\s+families,\s+([\d]+)\s+pairs\)',
            block
        )
        if gt_match:
            r['gt_file'] = gt_match.group(1)
            r['gt_families'] = int(gt_match.group(2))
            r['gt_pairs'] = int(gt_match.group(3))

        # Predictions info
        pred_match = re.search(
            r'Predictions\s*:\s*(\S+)\s+\((\d+)\s+families,\s+([\d]+)\s+pairs\)',
            block
        )
        if pred_match:
            r['pred_file'] = pred_match.group(1)
            r['pred_families'] = int(pred_match.group(2))
            r['pred_pairs'] = int(pred_match.group(3))

        # Extract tool name from prediction file path
        # e.g. pandelos_plus_paralle_clus_parsed_families.json -> PanDelos-plus (parallel clus)
        if 'pred_file' in r:
            fname = r['pred_file'].split('/')[-1]
            # Remove common suffixes
            name = fname.replace('_parsed_families.json', '')
            name = name.replace('_families.json', '')
            r['tool_name'] = name

        # Metrics
        tp_match = re.search(r'True Positives\s+\(TP\)\s*:\s*([\d]+)', block)
        fp_match = re.search(r'False Positives\s+\(FP\)\s*:\s*([\d]+)', block)
        fn_match = re.search(r'False Negatives\s+\(FN\)\s*:\s*([\d]+)', block)
        tn_match = re.search(r'True Negatives\s+\(TN\)\s*:\s*([\d]+)', block)
        prec_match = re.search(r'Precision\s*:\s*([\d.]+)', block)
        rec_match = re.search(r'Recall\s*:\s*([\d.]+)', block)
        f1_match = re.search(r'F1\s*:\s*([\d.]+)', block)

        if tp_match: r['tp'] = int(tp_match.group(1))
        if fp_match: r['fp'] = int(fp_match.group(1))
        if fn_match: r['fn'] = int(fn_match.group(1))
        if tn_match: r['tn'] = int(tn_match.group(1))
        if prec_match: r['precision'] = float(prec_match.group(1))
        if rec_match: r['recall'] = float(rec_match.group(1))
        if f1_match: r['f1'] = float(f1_match.group(1))

        results.append(r)

    return results


def fmt_num(n):
    """Format large numbers with thousand separators for LaTeX."""
    return f"{n:,}".replace(",", "{,}")


def generate_table_families(results, gt_info):
    """Table 1: Families and pairs per tool."""
    n_tools = len(results)
    # Build column spec: first col + 2 sub-cols per tool
    col_spec = "L " + " ".join(["Y Y"] * n_tools)

    # Header row with tool names spanning 2 cols each
    header_tools = ""
    cmidrules = ""
    for i, r in enumerate(results):
        col_start = 2 + i * 2
        col_end = col_start + 1
        header_tools += f" & \\multicolumn{{2}}{{c}}{{\\makecell{{{r['tool_name']}}}}}"
        cmidrules += f"\\cmidrule(lr){{{col_start}-{col_end}}}"

    # Sub-header
    sub_header = ""
    for _ in results:
        sub_header += " & Families & Pairs"

    # Data row (single row: the dataset)
    data_row = "Dataset"
    for r in results:
        data_row += f" & {fmt_num(r['pred_families'])} & {fmt_num(r['pred_pairs'])}"

    # GT row
    gt_row = "Ground Truth"
    for _ in results:
        gt_row += f" & {fmt_num(gt_info['families'])} & {fmt_num(gt_info['pairs'])}"

    latex = f"""\\begin{{table}}[!ht]
    \\centering
    \\caption{{
        \\textbf{{
            Number of gene families and gene pairs identified by each tool.
        }}
    }}
    \\begin{{tabularx}}{{\\textwidth}}{{{col_spec}}}
        \\toprule
        {header_tools} \\\\
        {cmidrules}
        {sub_header} \\\\
        \\midrule
        {gt_row} \\\\
        {data_row} \\\\
        \\bottomrule
    \\end{{tabularx}}
    \\label{{tab:families_pairs}}
    \\begin{{flushleft}}
    Comparison of the number of gene families and total gene pairs
    identified by each tool against the ground truth.
    \\end{{flushleft}}
\\end{{table}}"""

    return latex


def generate_table_metrics(results):
    """Table 2: Precision, Recall, F1 per tool."""
    n_tools = len(results)
    col_spec = "L " + " ".join(["Y Y Y"] * n_tools)

    header_tools = ""
    cmidrules = ""
    for i, r in enumerate(results):
        col_start = 2 + i * 3
        col_end = col_start + 2
        header_tools += f" & \\multicolumn{{3}}{{c}}{{\\makecell{{{r['tool_name']}}}}}"
        cmidrules += f"\\cmidrule(lr){{{col_start}-{col_end}}}"

    sub_header = ""
    for _ in results:
        sub_header += " & Precision & Recall & F1"

    data_row = "Results"
    for r in results:
        data_row += f" & {r['precision']:.4f} & {r['recall']:.4f} & {r['f1']:.4f}"

    latex = f"""\\begin{{table}}[!ht]
    \\centering
    \\caption{{
        \\textbf{{
            Precision, Recall, and F1-score for each tool.
        }}
    }}
    \\begin{{tabularx}}{{\\textwidth}}{{{col_spec}}}
        \\toprule
        {header_tools} \\\\
        {cmidrules}
        {sub_header} \\\\
        \\midrule
        {data_row} \\\\
        \\bottomrule
    \\end{{tabularx}}
    \\label{{tab:metrics}}
    \\begin{{flushleft}}
    Classification metrics computed on gene pairs. Precision measures the
    fraction of predicted homologous pairs that are correct; Recall measures
    the fraction of true homologous pairs that were identified; F1 is the
    harmonic mean of Precision and Recall.
    \\end{{flushleft}}
\\end{{table}}"""

    return latex


def generate_table_confusion(results):
    """Table 3: TP, FP, FN, TN per tool."""
    n_tools = len(results)
    col_spec = "L " + " ".join(["Y Y Y Y"] * n_tools)

    header_tools = ""
    cmidrules = ""
    for i, r in enumerate(results):
        col_start = 2 + i * 4
        col_end = col_start + 3
        header_tools += f" & \\multicolumn{{4}}{{c}}{{\\makecell{{{r['tool_name']}}}}}"
        cmidrules += f"\\cmidrule(lr){{{col_start}-{col_end}}}"

    sub_header = ""
    for _ in results:
        sub_header += " & TP & FP & FN & TN"

    data_row = "Results"
    for r in results:
        data_row += f" & {fmt_num(r['tp'])} & {fmt_num(r['fp'])} & {fmt_num(r['fn'])} & {fmt_num(r['tn'])}"

    latex = f"""\\begin{{table}}[!ht]
    \\centering
    \\caption{{
        \\textbf{{
            Confusion matrix values for each tool.
        }}
    }}
    \\begin{{tabularx}}{{\\textwidth}}{{{col_spec}}}
        \\toprule
        {header_tools} \\\\
        {cmidrules}
        {sub_header} \\\\
        \\midrule
        {data_row} \\\\
        \\bottomrule
    \\end{{tabularx}}
    \\label{{tab:confusion}}
    \\begin{{flushleft}}
    True Positives (TP): homologous pairs correctly identified.
    False Positives (FP): non-homologous pairs incorrectly predicted as homologous.
    False Negatives (FN): homologous pairs missed by the tool.
    True Negatives (TN): non-homologous pairs correctly excluded.
    \\end{{flushleft}}
\\end{{table}}"""

    return latex


def main():
    parser = argparse.ArgumentParser(
        description="Generate LaTeX tables from evaluation output."
    )
    parser.add_argument("input_file", help="Text file with evaluation results")
    parser.add_argument(
        "--output", "-o", default="tables.tex",
        help="Output .tex file (default: tables.tex)"
    )
    args = parser.parse_args()

    with open(args.input_file) as f:
        text = f.read()

    results = parse_results(text)

    if not results:
        print("ERROR: No results found in input file.", file=sys.stderr)
        sys.exit(1)

    print(f"Found {len(results)} evaluation block(s).")
    for i, r in enumerate(results):
        print(f"  [{i+1}] {r.get('tool_name', 'unknown')} — "
              f"P={r.get('precision', 'N/A')}, R={r.get('recall', 'N/A')}, "
              f"F1={r.get('f1', 'N/A')}")

    # GT info from first block (assumed same for all)
    gt_info = {
        'families': results[0].get('gt_families', 0),
        'pairs': results[0].get('gt_pairs', 0),
    }

    # Generate tables
    table1 = generate_table_families(results, gt_info)
    table2 = generate_table_metrics(results)
    table3 = generate_table_confusion(results)

    output = f"""% Auto-generated LaTeX tables from evaluation results
% Requires: booktabs, tabularx, makecell packages
% Custom column types needed in preamble:
%   \\newcolumntype{{L}}{{>{{\\raggedright\\arraybackslash}}X}}
%   \\newcolumntype{{Y}}{{>{{\\centering\\arraybackslash}}X}}

{table1}

{table2}

{table3}
"""

    with open(args.output, 'w') as f:
        f.write(output)

    print(f"\nLaTeX tables written to: {args.output}")


if __name__ == "__main__":
    main()