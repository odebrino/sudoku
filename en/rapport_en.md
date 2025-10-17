# Sudoku — CLI & GUI (C / SDL2)
**Binôme:** ODE_BRINO_Rafael_RIZO_Maria_Eduarda  
**Course:** Projet C 2025–2026  
**Date:** 2025-10-17

## 1. Goal
Build a complete Sudoku application in C with: (a) a textual CLI; (b) a SDL2 GUI; (c) a solver with trace; (d) a generator ensuring uniqueness; (e) save/load; (f) a “hint” feature.

## 2. What to test first (TL;DR)
- Build: `make cli` and `make gui`.
- Quick demo:

  ```bash
  ./build/sudoku_cli --generate 40 > k40.txt
  ./build/sudoku_cli --solve k40.txt
  ```
- GUI: `./build/sudoku_gui` → new game, enter digits, press **H** (hint), **S** (solve), **K** (save), **L** (load), **B** (trace/step).
- Validation sheet: see *INTEGRATION_VALIDATION.md* (English & French).

## 3. Functional scope
- CLI: generate N holes (k), load/solve/print, read from file/stdin, write to stdout.

- GUI (SDL2): grid editing, keyboard entry, hint, step-by-step solve with trace, save/load JSON.

- Generator: removes cells to target k; uniqueness checked by DLX.

- Solvers: DLX (Algorithm X), Backtracking (MRV + bitmasks), Constraint propagation (singles).

## 4. Design highlights
- Clear module split: *utils*, *solvers*, *generator*, *interface (cli/gui)*.

- Abstract `Grid` API to centralize I/O, validation and mutations.

- Deterministic RNG seed option for reproducible tests.

- Trace hooks decoupled from algorithms (emit events, GUI subscribes).

## 5. Algorithmic choices (why)
- **DLX**: fastest for exact cover + easy uniqueness check; good oracle to validate other solvers.

- **Backtracking + MRV/bitmask**: simple baseline; shows search tree and backtracks for pedagogy.

- **Constraint propagation**: cheap pruning (naked/hidden singles) to reduce search space before BT.

- Trade-offs and complexity details: see *ALGORITHMIC_JUSTIFICATION.md*.

## 6. Integration & validation (how)
- One-command recipes for TA.

- Datasets: auto-generate k∈{30,40,50} and verify (time, uniqueness, correctness).

- Save/Load round-trip test; GUI smoke tests (keyboard/mouse paths).

- Full list: see *INTEGRATION_VALIDATION.md*.

## 7. Known limits
- Generator difficulty≈k (holes) not true “human difficulty”.

- GUI: minimalistic skin; no localization at runtime.

- Constraint rules limited to singles (no advanced human strategies).

## 8. Structure (expected)
```
include/            # headers
src/utils/          # grid, io, rand, trace
src/solvers/        # dlx, backtracking, mrv_bitmask, constraint_propagation, rules
src/generator/      # generator
src/interface/      # cli, formatter, save, gui, gui_draw, gui_events, gui_solver
build/              # artifacts
data/               # sample grids, save.json
```
