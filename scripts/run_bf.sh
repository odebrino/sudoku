#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

echo "== Build binaries =="
make -C "$ROOT" build/generate_datasets
make -C "$ROOT" build/bf_suite

echo "== Generate datasets (files under data/grids) =="
"$ROOT/build/generate_datasets" || true
ls -1 "$ROOT/data/grids" | head -n 8 || true

echo "== Run brute-force suite (heavy) =="
"$ROOT/build/bf_suite"

echo "== Plot comparison (DLX should be fastest across all k) =="
python3 "$ROOT/plots/compare_solvers.py" || true

PNG="$ROOT/plots/solvers_comparison.png"
test -f "$PNG" && echo "OK: plot generated at $PNG"

echo "== Quick check (DLX rank 1 everywhere) =="
csv="$ROOT/results/solvers_comparison.csv"
awk -F, 'NR>1 {print > "/tmp/byk_"$1".csv"}' "$csv"
fail=0
for f in /tmp/byk_*.csv; do
  min_line=$(sort -t, -k3,3g "$f" | head -n1)
  solver=$(echo "$min_line" | cut -d, -f2)
  k=$(basename "$f" | sed 's/[^0-9]//g')
  if [ "$solver" != "DLX" ]; then
    echo "WARNING: For k=$k fastest is $solver (not DLX)"
    fail=1
  else
    echo "k=$k OK (DLX fastest)"
  fi
done

if [ "$fail" -ne 0 ]; then
  echo "NOTE: empirical data shows non-DLX fastest for some k. Plot still generated; investigate."
else
  echo "ALL GOOD: DLX fastest in all tested ks."
fi
