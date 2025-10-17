#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# Default: prefer CSV; pass --force-synthetic to override
python3 "$ROOT/plots/compare_solvers.py" "$@"
