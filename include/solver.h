// Solver public API (DLX entry points).

#ifndef SOLVER_H
#define SOLVER_H
#include "utils.h"

int solve_backtracking(Grid* g);
int count_solutions(Grid* g, int limit);

int apply_naked_single(Grid* g);
int apply_hidden_single(Grid* g);

int solver_hint(Grid* g);
int solver_solve(Grid* g);

// Solve a copy of the grid and return the solved value for (r,c) if solvable.
// Returns 1 and writes *out if success; returns 0 if unsolvable or invalid args.
int solve_get_cell(const Grid* g_in, int r, int c, int* out);
#endif

// DLX-based counting (limit>=1). Returns number of solutions up to limit.
int count_solutions_dlx(const Grid* g, int limit);