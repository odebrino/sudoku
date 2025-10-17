#!/usr/bin/env python3
"""
Plot solver runtimes from CSV (if available) or from a synthetic dataset.
By default we prefer the CSV at ./results/solvers_comparison.csv.
Use --force-synthetic to ignore CSV and plot a synthetic profile where
DLX is the fastest across all tested difficulties.
"""
import argparse
import csv
import os
from collections import defaultdict, OrderedDict

import matplotlib
matplotlib.use("Agg")  # headless
import matplotlib.pyplot as plt  # noqa: E402

BASE = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def parse_args():
    p = argparse.ArgumentParser(description="Plot solver comparison (CSV or synthetic)")
    p.add_argument("--csv", default=os.path.join(BASE, "results", "solvers_comparison.csv"),
                   help="Path to CSV (k,solver,mean_ms). Default: ./results/solvers_comparison.csv")
    p.add_argument("--out", default=os.path.join(BASE, "plots", "solvers_comparison.png"),
                   help="Output PNG path. Default: ./plots/solvers_comparison.png")
    p.add_argument("--force-synthetic", action="store_true",
                   help="Ignore CSV even if it exists and use synthetic dataset (DLX fastest everywhere)." )
    return p.parse_args()

SOLVER_ORDER = ["DLX", "Backtracking+MRV/bitmask", "Backtracking", "Constraint Propagation"]

def load_csv(path):
    data = defaultdict(dict)  # data[k][solver] = mean_ms
    if not os.path.exists(path):
        return None
    with open(path, newline="") as f:
        r = csv.DictReader(f)
        for row in r:
            try:
                k = int(row["k"])
                solver = row["solver"].strip()
                mean_ms = float(row["mean_ms"])
            except Exception:
                continue
            data[k][solver] = mean_ms
    return data if data else None

def synthetic_data():
    ks = [30, 40, 50, 60]
    base = {
        "DLX": [0.7, 1.1, 1.8, 3.0],
        "Backtracking+MRV/bitmask": [2.8, 4.2, 7.1, 12.5],
        "Backtracking": [6.0, 9.5, 16.0, 28.0],
        "Constraint Propagation": [15.0, 22.0, 36.0, 60.0],
    }
    data = defaultdict(dict)
    for i, k in enumerate(ks):
        for s in base:
            data[k][s] = base[s][i]
    return data

def normalize_and_order(data):
    ks_sorted = sorted(data.keys())
    ordered = OrderedDict((k, OrderedDict()) for k in ks_sorted)
    for k in ks_sorted:
        for s in SOLVER_ORDER:
            if s in data[k]:
                ordered[k][s] = data[k][s]
    return ordered

def plot(ordered, out_path, title_suffix="Empirical"):
    # Ensure output dir
    os.makedirs(os.path.dirname(out_path), exist_ok=True)

    ks = list(ordered.keys())
    solvers = [s for s in SOLVER_ORDER if any(s in ordered[k] for k in ks)]
    Y = [[ordered[k][s] for k in ks] for s in solvers]

    width = 0.18
    x = range(len(ks))
    fig, ax = plt.subplots(figsize=(9.5, 5.2))

    for idx, s in enumerate(solvers):
        x_pos = [xi + (idx - (len(solvers)-1)/2)*width for xi in x]
        ax.bar(x_pos, Y[idx], width=width, label=s)
        for xi, yi in zip(x_pos, Y[idx]):
            ax.text(xi, yi, f"{yi:.2f}", ha="center", va="bottom", fontsize=8)

    ax.set_xticks(list(x))
    ax.set_xticklabels([str(k) for k in ks])
    ax.set_xlabel("Number of holes (k) → harder")
    ax.set_ylabel("Mean runtime (ms) — lower is better")
    ax.set_title(f"Solver comparison — {title_suffix}")
    ax.legend()
    ax.grid(True, axis="y", alpha=0.3)

    fig.tight_layout()
    fig.savefig(out_path, dpi=180)
    print(f"Saved: {out_path}")

def main():
    args = parse_args()
    # Choose dataset
    if args.force_synthetic:
        data = synthetic_data()
        title = "Synthetic (DLX fastest everywhere)"
    else:
        data = load_csv(args.csv)
        if data is None:
            print("No CSV found or CSV empty — falling back to synthetic dataset.")
            data = synthetic_data()
            title = "Synthetic (DLX fastest everywhere)"
        else:
            title = "Empirical"

    ordered = normalize_and_order(data)

    # Sanity check about DLX rank
    for k, row in ordered.items():
        if "DLX" in row:
            best = min(row.values())
            if abs(row["DLX"] - best) > 1e-9:
                print(f"WARNING: DLX is not the fastest for k={k} in your data.")
        else:
            print(f"WARNING: Missing DLX data for k={k}; using available solvers only.")

    plot(ordered, args.out, title_suffix=title)

if __name__ == "__main__":
    main()
