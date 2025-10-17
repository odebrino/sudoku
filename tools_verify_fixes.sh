#!/usr/bin/env bash
set -euo pipefail
echo "Verifying fixes..."
grep -n 'if (k < 0) { k = 0; }' src/generator/generator.c && echo "OK: generator.c bounds fixed"
grep -n '#include <sys/time.h>' src/benchmark/benchmark.c && echo "OK: benchmark uses gettimeofday"
if grep -n 'clock_gettime' src/benchmark/benchmark.c; then
  echo "ERROR: still using clock_gettime"; exit 1
fi
echo "All good."
