# Algorithmic Justification (EN)
**Binôme:** ODE_BRINO_Rafael_RIZO_Maria_Eduarda — **Date:** 2025-10-17

## 1) Solvers
### 1.1 DLX (Algorithm X with Dancing Links)
- Maps Sudoku to exact cover with 4 constraint families (cell, row, col, box).

- DLX excels at sparse exact-cover; backtracking at matrix level with O(1) column/row remove/restore.

- Use-cases here: (i) fast solving; (ii) **uniqueness** oracle during generation.

**Pros:** very fast, exact; easy to count number of solutions.  
**Cons:** less pedagogical for GUI stepping (matrix-level ops).

### 1.2 Backtracking + MRV + Bitmasks
- Variables = cells; domain = digits 1–9.

- **MRV** selects cell with minimal candidates; **bitmasks** compact candidate sets (9 bits).

- Good for educational trace and for comparing against DLX.

**Pros:** simple to implement; great for trace.  
**Cons:** exponential in worst case; needs heuristics/pruning.

### 1.3 Constraint Propagation (Singles)
- Apply **naked singles** and **hidden singles** until fixpoint.

- Reduces branching; cheap pre-processing for both BT and DLX.

## 2) Complexity (informal)
- DLX: backtracking on exact cover; practical runtimes ~milliseconds for typical puzzles.

- BT+MRV: worst-case exponential; with singles+MRV, typically fast for standard puzzles.

- Propagation: O(1) per update per unit-constraint; negligible vs search.

## 3) Generation with Uniqueness
- Start from solved grid → remove cells to reach target k.

- After each removal, run DLX with early-stop if ≥2 solutions; revert if non-unique.

- Randomization: shuffle cell order; optional fixed seed for reproducibility.

## 4) Why multiple solvers?
- Pedagogy (trace with BT) + Performance/validation (DLX) + Better UX (singles cut).

- Cross-checking detects bugs; DLX as reference avoids wrong positives from BT.
