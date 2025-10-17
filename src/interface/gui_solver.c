// Glue between GUI buttons and solver/generator.

#include "gui.h"

// gui_new_k(): helper function.
void gui_new_k(App* A, int k){ generator_new_k(k, &A->grid); }
// gui_hint(): helper function.
void gui_hint(App* A){ solver_hint(&A->grid); }
// gui_solve(): helper function.
void gui_solve(App* A){ solver_solve(&A->grid); }