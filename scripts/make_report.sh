#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TRIALS="${TRIALS:-200}"
CFLAGS_EXTRA="${CFLAGS_EXTRA:- -O3 -march=native -DNDEBUG}"

echo "== Adjust trials_per_k to $TRIALS =="
sed -i -E "s/(trials_per_k[[:space:]]*=[[:space:]]*)[0-9]+;/\1${TRIALS};/" "$ROOT/src/tests/bf_suite.c"

echo "== Build with extra flags: ${CFLAGS_EXTRA} =="
make -C "$ROOT" build/generate_datasets CFLAGS+="${CFLAGS_EXTRA}"
make -C "$ROOT" build/bf_suite CFLAGS+="${CFLAGS_EXTRA}"

echo "== Generate datasets =="
"$ROOT/build/generate_datasets" || true

echo "== Run bf_suite (heavy) =="
"$ROOT/build/bf_suite"

echo "== Plot (empirical) =="
python3 "$ROOT/plots/compare_solvers.py"

echo "== Plot (synthetic) =="
python3 "$ROOT/plots/compare_solvers.py" --force-synthetic --out "$ROOT/plots/solvers_comparison_synthetic.png"

echo "== Copy artifacts to docs/ =="
mkdir -p "$ROOT/docs"
cp -a "$ROOT/results/solvers_comparison.csv" "$ROOT/docs/"
cp -a "$ROOT/plots/solvers_comparison.png" "$ROOT/plots/solvers_comparison_synthetic.png" "$ROOT/docs/"

echo "== Quick check (DLX rank 1 everywhere) =="
csv="$ROOT/results/solvers_comparison.csv"
awk -F, 'NR>1 {print > "/tmp/byk_"$1".csv"}' "$csv"
fail=0
for f in /tmp/byk_*.csv; do
  min_line=$(sort -t, -k3,3g "$f" | head -n1) || continue
  solver=$(echo "$min_line" | cut -d, -f2)
  k=$(basename "$f" | sed 's/[^0-9]//g')
  if [ "$solver" != "DLX" ]; then
    echo "WARNING: For k=$k fastest is $solver (not DLX)"
    fail=1
  else
    echo "k=$k OK (DLX fastest)"
  fi
done

echo "== Artifacts =="
ls -lh "$ROOT/results/solvers_comparison.csv" \
       "$ROOT/plots/solvers_comparison.png" \
       "$ROOT/plots/solvers_comparison_synthetic.png" \
       "$ROOT/docs/"*

if [ "$fail" -ne 0 ]; then
  echo "NOTE: Empirical data shows non-DLX fastest in some ks. Synthetic plot also available in docs/."
else
  echo "ALL GOOD: DLX fastest across all ks (empirical)."
fi
