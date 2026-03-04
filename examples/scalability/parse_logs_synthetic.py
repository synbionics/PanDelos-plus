#!/usr/bin/env python3
# parse_logs_synthetic.py
#
# Reads PanDelos and PanDelos-plus logs from synthetic benchmarks
# and produces JSON, LaTeX tables and plots.
#
# Usage:
#   python parse_logs_synthetic.py <pd_log_dir> <pdp_log_dir> <output_dir>
#
# Expected naming convention:
#   pd_log_dir:   {name}-pd.log
#   pdp_log_dir:  {name}-{T}t.log       (thread scaling, fixed dataset)
#                 {name}-pdp.log         (scalability, fixed max threads)

import re
import os
import sys
import json
import math
import matplotlib.pyplot as plt
import pandas as pd
from typing import List, Dict, Any

# ----------------------------------------------------------------------
# plot configuration
# ----------------------------------------------------------------------
max_width_px = 1250
max_height_px = 1825
dpi = 300
width_in = max_width_px / dpi
height_in = max_height_px / dpi

plt.style.use("ggplot")

# ----------------------------------------------------------------------
# regex
# ----------------------------------------------------------------------
RE_METRICS    = re.compile(r"time\(seconds\):\s*([\d.]+).*memory\(KB\):\s*(\d+)", re.I)
RE_PD_FILE    = re.compile(r"^(.+)-pd\.log$")
RE_PDP_THREAD = re.compile(r"^(.+)-(\d+)t\.log$")
RE_PDP_SCALE  = re.compile(r"^(.+)-pdp\.log$")


# ----------------------------------------------------------------------
# parsing
# ----------------------------------------------------------------------
def parse_single_log(path: str) -> Dict[str, int]:
    """Extracts the first metrics line from a single log file."""
    with open(path, encoding="utf-8") as fh:
        for line in fh:
            m = RE_METRICS.search(line)
            if m:
                return {
                    "time (s)": int(float(m.group(1))),
                    "memory (KB)": int(m.group(2)),
                }
    return {}


def scan_pd_dir(log_dir: str) -> List[Dict[str, Any]]:
    """Scans pd_log_dir for {name}-pd.log files."""
    results: List[Dict[str, Any]] = []
    for fname in sorted(os.listdir(log_dir)):
        m = RE_PD_FILE.match(fname)
        if not m:
            continue
        metrics = parse_single_log(os.path.join(log_dir, fname))
        if metrics:
            metrics["ifile"] = m.group(1)
            results.append(metrics)
    return results


def scan_pdp_thread_dir(log_dir: str) -> List[Dict[str, Any]]:
    """Scans pdp_log_dir for {name}-{T}t.log files (thread scaling)."""
    results: List[Dict[str, Any]] = []
    for fname in sorted(os.listdir(log_dir)):
        m = RE_PDP_THREAD.match(fname)
        if not m:
            continue
        metrics = parse_single_log(os.path.join(log_dir, fname))
        if metrics:
            metrics["ifile"] = m.group(1)
            metrics["nthread"] = int(m.group(2))
            results.append(metrics)
    return results


def scan_pdp_scale_dir(log_dir: str) -> List[Dict[str, Any]]:
    """Scans pdp_log_dir for {name}-pdp.log files (scalability)."""
    results: List[Dict[str, Any]] = []
    for fname in sorted(os.listdir(log_dir)):
        m = RE_PDP_SCALE.match(fname)
        if not m:
            continue
        metrics = parse_single_log(os.path.join(log_dir, fname))
        if metrics:
            metrics["ifile"] = m.group(1)
            results.append(metrics)
    return results


# ----------------------------------------------------------------------
# utilities
# ----------------------------------------------------------------------
def addMemoryMB_GB(data: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    for rec in data:
        kb = rec.get("memory (KB)")
        if kb is None:
            continue
        rec["memory (MB)"] = math.floor(kb / 1024)
        rec["memory (GB)"] = round(rec["memory (MB)"] / 1024, 1)
    return data


def filter_by_thread(rows: List[Dict[str, Any]], nthread: int) -> List[Dict[str, Any]]:
    return [r for r in rows if r.get("nthread") == nthread]


# ----------------------------------------------------------------------
# LaTeX tables
# ----------------------------------------------------------------------
def table_pd_vs_pdp(pd_rows: List[Dict[str, Any]],
                    pdp_rows: List[Dict[str, Any]],
                    caption: str = ("Comparison of Pandelos and "
                                    "Pandelos-plus performance metrics."),
                    label: str = "tab:pdp-pd-all-comparison") -> str:

    pd_by  = {d["ifile"]: d for d in pd_rows}
    pdp_by = {d["ifile"]: d for d in pdp_rows}
    common = sorted(pd_by.keys() & pdp_by.keys())
    if not common:
        raise ValueError("No common datasets between pd and pdp")

    def _fmt_gb(v: float) -> str:
        return f"{v:.1f}".rstrip("0").rstrip(".")

    body: List[str] = []
    for name in common:
        p, pp = pd_by[name], pdp_by[name]
        t_pd  = p["time (s)"]
        m_pd  = p.get("memory (GB)", p.get("memory (MB)", 0) / 1024)
        t_pdp = pp["time (s)"]
        m_pdp = pp.get("memory (GB)", pp.get("memory (MB)", 0) / 1024)

        speedup = round(t_pd / t_pdp, 1) if t_pdp else None
        mem_red = round((m_pd - m_pdp) / m_pd * 100, 1) if m_pd else None

        body.append(
            f"        {name} & {t_pd} & {_fmt_gb(m_pd)}"
            f" & {t_pdp} & {_fmt_gb(m_pdp)}"
            f" & {speedup if speedup is not None else '-'}"
            f" & {mem_red if mem_red is not None else '-'} \\\\"
        )

    lines = [
        r"\begin{table}[htbp]",
        r"    \centering",
        f"    \\caption{{{caption}}}",
        f"    \\label{{{label}}}",
        r"    \begin{tabularx}{\textwidth}{L Y Y Y Y Y Y}",
        r"        \toprule",
        (r"        Name of dataset"
         r" & \multicolumn{2}{c}{Pandelos}"
         r" & \multicolumn{2}{c}{Pandelos-plus}"
         r" & Runtime speedup & Memory reduction (\%) \\"),
        (r"        \cmidrule(lr){2-3}\cmidrule(lr){4-5}"
         r"\cmidrule(lr){6-6}\cmidrule(lr){7-7}"),
        (r"        & Time (s) & Memory (GB)"
         r" & Time (s) & Memory (GB)"
         r" & Speedup & Reduction \\"),
        r"        \midrule",
    ]
    lines.extend(body)
    lines += [r"        \bottomrule", r"    \end{tabularx}", r"\end{table}"]
    return "\n".join(lines)


def table_threads(records: List[Dict[str, Any]],
                  threads=(1, 2, 4, 8, 16, 32),
                  label="tab:pdp-threads",
                  caption="Per-thread memory and time for PanDelos-plus") -> str:
    df = pd.DataFrame(records)
    if "memory (GB)" not in df.columns:
        df["memory (GB)"] = df["memory (KB)"] / 1_048_576
    df["mem_fmt"]  = df["memory (GB)"].round(1).map(lambda x: f"{x:g}")
    df["time_fmt"] = df["time (s)"].astype(int)

    pivot = {(row.ifile, row.nthread): (row.mem_fmt, row.time_fmt)
             for row in df.itertuples()}
    datasets = sorted(df["ifile"].unique())

    cols_spec = "L " + " ".join("YY" for _ in threads)
    lines = [
        r"\begin{table}[htbp]",
        r"    \centering",
        f"    \\caption{{{caption}}}",
        f"    \\label{{{label}}}",
        f"    \\begin{{tabularx}}{{\\textwidth}}{{{cols_spec}}}",
        r"        \toprule",
        "        Name of dataset & " +
        " & ".join(f"\\multicolumn{{2}}{{c}}{{{t} thread}}" for t in threads) +
        r" \\",
        "        \\cmidrule(lr){2-3}" + "".join(
            f"\\cmidrule(lr){{{4+2*i}-{5+2*i}}}" for i in range(len(threads)-1)),
        "        & " + " & ".join(["Memory (GB) & Time (s)"] * len(threads)) + r" \\",
        r"        \midrule",
    ]
    for ds in datasets:
        cells = [ds]
        for t in threads:
            mem, time = pivot.get((ds, t), ("", ""))
            cells.extend([str(mem), str(time)])
        lines.append("        " + " & ".join(cells) + r" \\")
    lines += [r"        \bottomrule", r"    \end{tabularx}", r"\end{table}"]
    return "\n".join(lines)


def generate_latex_table_big_data(data):
    header = [
        "\\begin{table}[htbp]",
        "    \\centering",
        "    \\caption{Execution time (h) and memory (GB) for datasets.}",
        "    \\label{tab:exec-time-memory}",
        "    \\begin{tabular}{lrr}",
        "        \\toprule",
        "        Name of dataset & Execution time (h) & Memory (GB) \\\\",
        "        \\midrule"
    ]
    rows = []
    for entry in data:
        name = entry["ifile"]
        time_h = entry["time (s)"] / 3600
        mem_gb = entry["memory (GB)"]
        rows.append(f"        {name} & {time_h:.2f} & {mem_gb:.1f} \\\\")
    footer = [
        "        \\bottomrule",
        "    \\end{tabular}",
        "\\end{table}"
    ]
    return "\n".join(header + rows + footer)


# ----------------------------------------------------------------------
# plots
# ----------------------------------------------------------------------
def plot_threads_by_tool(pd_data: List[Dict[str, Any]],
                         pdp_data: List[Dict[str, Any]],
                         output_file: str = "threads_by_tool_plot.tif") -> None:
    """Compares PanDelos vs PanDelos-plus on the same axes.

    PanDelos: constant dashed line with central label.
    PanDelos-plus: solid line with markers and per-point labels.
    """
    MARKERS    = ["o", "s", "^", "D", "v", "P", "X", "*"]
    LINESTYLES = ["-", "--", "-.", ":", (0, (3, 1, 1, 1)), (0, (5, 1))]
    LABEL_BOX  = dict(facecolor="white", edgecolor="none",
                      boxstyle="round,pad=0.1", alpha=0.8)

    if not pd_data or not pdp_data:
        raise ValueError("Both pd_data and pdp_data are required")

    pdp_threads = sorted({r["nthread"] for r in pdp_data if r.get("nthread")})
    pd_by  = {d["ifile"]: d for d in pd_data}
    pdp_by_ds = {}
    for r in pdp_data:
        pdp_by_ds.setdefault(r["ifile"], []).append(r)

    common = sorted(pd_by.keys() & pdp_by_ds.keys())
    if not common:
        raise ValueError("No common datasets between pd and pdp")

    # build combined records with a "tool" column
    records: List[Dict[str, Any]] = []
    for ds in common:
        p = pd_by[ds]
        for t in pdp_threads:
            records.append({
                "nthread": t,
                "memory (GB)": p.get("memory (GB)", 0),
                "time (s)": p["time (s)"],
                "tool": "PanDelos",
            })
        for r in pdp_by_ds[ds]:
            records.append({
                "nthread": r["nthread"],
                "memory (GB)": r.get("memory (GB)", 0),
                "time (s)": r["time (s)"],
                "tool": "PanDelos-plus",
            })

    df = pd.DataFrame(records).sort_values("nthread")
    XTICKS = sorted(df["nthread"].unique())
    tools = df["tool"].unique()

    plt.rcParams.update({
        "font.family": "Arial", "font.size": 10,
        "axes.titlesize": 12, "axes.labelsize": 11,
        "xtick.labelsize": 10, "ytick.labelsize": 10,
        "legend.fontsize": 10,
    })

    fig, (ax_mem, ax_time) = plt.subplots(
        nrows=2, ncols=1, dpi=dpi, figsize=(width_in, height_in), sharex=False
    )
    fig.patch.set_facecolor("white")

    # ---- memory (top) ----
    for i, tool in enumerate(tools):
        sub = df[df["tool"] == tool]
        is_const = sub["memory (GB)"].nunique() == 1
        ls = "--" if is_const else LINESTYLES[i % len(LINESTYLES)]
        mk = None if is_const else MARKERS[i % len(MARKERS)]
        ax_mem.plot(sub["nthread"], sub["memory (GB)"],
                    linestyle=ls, marker=mk, alpha=0.8, linewidth=1.8, label=tool)

        if is_const:
            mid_x = sub["nthread"].median()
            y_val = sub["memory (GB)"].iloc[0]
            ax_mem.text(mid_x, y_val - 1.3, f"{y_val:.2f}", fontsize=8,
                        ha="center", va="bottom", color="black", bbox=LABEL_BOX)
        else:
            for idx, (x, y) in enumerate(zip(sub["nthread"], sub["memory (GB)"])):
                y_off = 0.9
                if idx == 0:
                    y_off = 5
                elif idx == 1:
                    y_off = 3
                ax_mem.text(x, y + y_off, f"{y:.2f}", fontsize=8,
                            ha="center", va="bottom", color="black", bbox=LABEL_BOX)

    ax_mem.set_ylabel("Memory (GB)")
    ax_mem.set_title("Memory usage vs number of threads", pad=10)
    ax_mem.set_facecolor("white")
    ax_mem.grid(True, color="lightgray", alpha=0.7)
    ax_mem.set_xticks(XTICKS)
    ax_mem.set_xticklabels([str(x) for x in XTICKS])
    ax_mem.legend(title="Tool")

    # ---- time (bottom) ----
    for i, tool in enumerate(tools):
        sub = df[df["tool"] == tool]
        is_const = sub["time (s)"].nunique() == 1
        ls = "--" if is_const else LINESTYLES[i % len(LINESTYLES)]
        mk = None if is_const else MARKERS[i % len(MARKERS)]
        ax_time.plot(sub["nthread"], sub["time (s)"],
                     linestyle=ls, marker=mk, alpha=0.8, linewidth=1.8, label=tool)

        if is_const:
            mid_x = sub["nthread"].median()
            y_val = sub["time (s)"].iloc[0]
            ax_time.text(mid_x, y_val + y_val / 30, f"{y_val:.0f}", fontsize=8,
                         ha="center", va="bottom", color="black", bbox=LABEL_BOX)
        else:
            for idx, (x, y) in enumerate(zip(sub["nthread"], sub["time (s)"])):
                x_off, y_off = x + 1.2, y
                if idx == 2:
                    x_off, y_off = x + 1.3, y + y / 8
                elif idx == 3:
                    x_off, y_off = x + 0.9, y + y / 3
                elif idx == 4:
                    x_off, y_off = x + 0.3, y + 5 * y / 8
                elif idx == 5:
                    x_off, y_off = x - 0.9, y + 6 * y / 8
                ax_time.text(x_off, y_off, f"{y:.0f}", fontsize=8,
                             ha="left", va="center", color="black", bbox=LABEL_BOX)

    ax_time.set_xlabel("Number of threads")
    ax_time.set_ylabel("Time (s)")
    ax_time.set_title("Execution time vs number of threads", pad=10)
    ax_time.set_facecolor("white")
    ax_time.grid(True, color="lightgray", alpha=0.7)
    ax_time.set_xticks(XTICKS)
    ax_time.set_xticklabels([str(x) for x in XTICKS])
    ax_time.legend(title="Tool")

    plt.setp(ax_mem.get_xticklabels(), visible=True)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"  Plot: {output_file}")


def plot_scalability(records: List[Dict[str, Any]],
                     output_file: str = "scalability_plot.tif") -> None:
    """Memory (GB) and Time (hours) vs number of genomes, with value labels."""
    LABEL_BOX = dict(facecolor="white", edgecolor="none",
                     boxstyle="round,pad=0.1", alpha=0.8)

    if not records:
        raise ValueError("Empty record list")

    df = pd.DataFrame(records)
    try:
        df["__input_int"] = df["ifile"].astype(int)
    except Exception:
        raise ValueError("Cannot convert 'ifile' to integers for sorting")

    df["time_h"] = df["time (s)"] / 3600
    df = df.sort_values("__input_int")

    plt.rcParams.update({
        "font.family": "Arial", "font.size": 10,
        "axes.titlesize": 12, "axes.labelsize": 11,
        "xtick.labelsize": 10, "ytick.labelsize": 10,
        "legend.fontsize": 10,
    })

    fig, (ax_mem, ax_time) = plt.subplots(
        nrows=2, ncols=1, dpi=dpi, figsize=(width_in, height_in), sharex=False
    )
    fig.patch.set_facecolor("white")

    xticks = df["__input_int"].tolist()
    xticklabels = [str(x) for x in xticks]

    # ---- memory ----
    ax_mem.plot(df["__input_int"], df["memory (GB)"],
                linestyle="-", marker="o", alpha=0.6, linewidth=1.5, label="Memory (GB)")
    ax_mem.set_xlabel("Number of genomes")
    ax_mem.set_ylabel("Memory (GB)")
    ax_mem.set_title("Memory vs number of genomes", pad=10)
    ax_mem.set_facecolor("white")
    ax_mem.grid(True, color="lightgray", alpha=0.7)
    ax_mem.legend()
    for x, y in zip(df["__input_int"], df["memory (GB)"]):
        ax_mem.text(x, y + 1.8, f"{y:.2f}", fontsize=8, ha="center", va="bottom",
                    color="black", alpha=0.9, bbox=LABEL_BOX)
    ax_mem.set_xticks(xticks)
    ax_mem.set_xticklabels(xticklabels, rotation=0)

    # ---- time ----
    ax_time.plot(df["__input_int"], df["time_h"],
                 linestyle="-", marker="o", alpha=0.6, linewidth=1.5, label="Time (hours)")
    ax_time.set_xlabel("Number of genomes")
    ax_time.set_ylabel("Time (hours)")
    ax_time.set_title("Execution time vs number of genomes", pad=10)
    ax_time.set_facecolor("white")
    ax_time.grid(True, color="lightgray", alpha=0.7)
    ax_time.legend()
    for x, y in zip(df["__input_int"], df["time_h"]):
        ax_time.text(x, y + 1.8, f"{y:.2f}", fontsize=8, ha="center", va="bottom",
                     color="black", alpha=0.9, bbox=LABEL_BOX)
    ax_time.set_xticks(xticks)
    ax_time.set_xticklabels(xticklabels, rotation=0)

    plt.setp(ax_mem.get_xticklabels(), visible=True)
    plt.tight_layout()
    plt.savefig(output_file, dpi=300, facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"  Plot: {output_file}")


# ----------------------------------------------------------------------
# main
# ----------------------------------------------------------------------
def main():
    if len(sys.argv) != 4:
        print(f"Usage: python {sys.argv[0]} <pd_log_dir> <pdp_log_dir> <output_dir>",
              file=sys.stderr)
        sys.exit(1)

    pd_dir  = os.path.abspath(sys.argv[1])
    pdp_dir = os.path.abspath(sys.argv[2])
    out_dir = os.path.abspath(sys.argv[3])
    os.makedirs(out_dir, exist_ok=True)

    # ---- parsing ----
    pd_data        = addMemoryMB_GB(scan_pd_dir(pd_dir))
    pdp_thread_data = addMemoryMB_GB(scan_pdp_thread_dir(pdp_dir))
    pdp_scale_data  = addMemoryMB_GB(scan_pdp_scale_dir(pdp_dir))

    if not pd_data and not pdp_thread_data and not pdp_scale_data:
        print(f"No logs found in {pd_dir} / {pdp_dir}", file=sys.stderr)
        sys.exit(1)

    # ---- save JSON ----
    for name, data in [("pd.json", pd_data),
                       ("pdp_threads.json", pdp_thread_data),
                       ("pdp_scalability.json", pdp_scale_data)]:
        path = os.path.join(out_dir, name)
        with open(path, "w") as f:
            json.dump(data, f, indent=2)
        print(f"  JSON: {path}  ({len(data)} records)")

    # ---- summary ----
    pd_datasets   = sorted({r["ifile"] for r in pd_data})
    pdp_t_datasets = sorted({r["ifile"] for r in pdp_thread_data})
    pdp_s_datasets = sorted({r["ifile"] for r in pdp_scale_data})
    pdp_threads   = sorted({r["nthread"] for r in pdp_thread_data if r.get("nthread")})
    print(f"\n  PanDelos datasets:        {pd_datasets}")
    print(f"  PDP thread-scaling sets:  {pdp_t_datasets}  threads: {pdp_threads}")
    print(f"  PDP scalability sets:     {pdp_s_datasets}")

    # ---- pd vs pdp table (uses max thread count) ----
    if pd_data and pdp_thread_data and pdp_threads:
        cmp_t = max(pdp_threads)
        pdp_cmp = filter_by_thread(pdp_thread_data, cmp_t)
        if pdp_cmp:
            try:
                tex = table_pd_vs_pdp(pd_data, pdp_cmp)
                p = os.path.join(out_dir, "pd_vs_pdp.tex")
                with open(p, "w") as f:
                    f.write(tex)
                print(f"  pd vs pdp table ({cmp_t}t): {p}")
            except ValueError as e:
                print(f"  pd vs pdp table skipped: {e}")

    # ---- per-thread table ----
    if pdp_thread_data and pdp_threads:
        tex = table_threads(pdp_thread_data, threads=tuple(pdp_threads))
        p = os.path.join(out_dir, "threads.tex")
        with open(p, "w") as f:
            f.write(tex)
        print(f"  Per-thread table: {p}")

    # ---- scalability table ----
    if pdp_scale_data:
        tex = generate_latex_table_big_data(pdp_scale_data)
        p = os.path.join(out_dir, "scalability.tex")
        with open(p, "w") as f:
            f.write(tex)
        print(f"  Scalability table: {p}")

    # ---- pd vs pdp by-tool plot ----
    if pd_data and pdp_thread_data and pdp_threads:
        try:
            plot_threads_by_tool(pd_data, pdp_thread_data,
                                 output_file=os.path.join(out_dir, "threads_by_tool_plot.tif"))
        except ValueError as e:
            print(f"  by-tool plot skipped: {e}")

    # ---- scalability plot (only numeric ifile values) ----
    numeric_scale = [r for r in pdp_scale_data if r["ifile"].isdigit()]
    if numeric_scale:
        try:
            plot_scalability(numeric_scale,
                             output_file=os.path.join(out_dir, "scalability_plot.tif"))
        except ValueError as e:
            print(f"  scalability plot skipped: {e}")

    print("\nDone.")


if __name__ == "__main__":
    main()
