# Architecture Overview (EN)
**Binôme:** ODE_BRINO_Rafael_RIZO_Maria_Eduarda — **Date:** 2025-10-17

## 1) Modules
- **include/**: public headers (Grid, Solver, Generator, GUI/CLI APIs).

- **src/utils/**: `grid.c` (board, validity, I/O), `io.c` (files/stdin/out), `rand.c`, `trace.c` (emit events).

- **src/solvers/**: `dlx.c`, `backtracking.c`, `backtracking_mrv_bitmask.c`, `constraint_propagation.c`, `rules.c`, `solver.c` (facade).

- **src/generator/**: `generator.c` (remove cells; DLX uniqueness check).

- **src/interface/**: `cli.c`, `formatter.c`, `save.c`, `gui.c`, `gui_draw.c`, `gui_events.c`, `gui_solver.c`.

- **build/**: outputs; **data/**: sample grids, `save.json`.

## 2) Data structures
- `Grid`: 9×9 array + candidate bitmasks; helpers to set/get/check.

- `TraceEvent`: enum + payload (cell, value, action). Observer pattern to GUI.

## 3) Build targets
- `make cli` → `build/sudoku_cli`.

- `make gui` → `build/sudoku_gui` (needs SDL2 installed).

- `make bench` (optional).

## 4) Extension points
- Add more human strategies under `rules.c`.

- Add difficulty estimator (rating) to generator.

- Add localization (EN/FR/PT) for GUI labels.

- Export images of solved grid (PNG) from GUI.
