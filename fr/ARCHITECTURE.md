# Architecture (FR)
**Binôme :** ODE_BRINO_Rafael_RIZO_Maria_Eduarda — **Date :** 2025-10-17

## 1) Modules
- **include/** : en-têtes publics (Grid, Solver, Generator, GUI/CLI).

- **src/utils/** : `grid.c`, `io.c`, `rand.c`, `trace.c`.

- **src/solvers/** : `dlx.c`, `backtracking.c`, `backtracking_mrv_bitmask.c`, `constraint_propagation.c`, `rules.c`, `solver.c`.

- **src/generator/** : `generator.c`.

- **src/interface/** : `cli.c`, `formatter.c`, `save.c`, `gui.c`, `gui_draw.c`, `gui_events.c`, `gui_solver.c`.

- **build/** : artefacts ; **data/** : grilles, `save.json`.

## 2) Structures de données
- `Grid` : 9×9 + bitmasks candidats ; helpers set/get/check.

- `TraceEvent` : enum + payload (cellule, valeur, action), observé par la GUI.

## 3) Cibles
- `make cli` → `build/sudoku_cli`.

- `make gui` → `build/sudoku_gui` (SDL2 requis).

- `make bench` (optionnel).

## 4) Extensions
- Stratégies “humaines” supplémentaires dans `rules.c`.

- Estimation de difficulté dans le générateur.

- Localisation EN/FR/PT.

- Export PNG de la grille.
