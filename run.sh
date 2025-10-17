#!/usr/bin/env bash
set -euo pipefail
echo "== Build GUI =="
make clean || true
make gui
echo "== Run GUI =="
if [ -x "./build/sudoku_gui" ]; then ./build/sudoku_gui; else echo "GUI not built"; fi
