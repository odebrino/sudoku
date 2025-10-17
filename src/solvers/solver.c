// Solver entry points used by GUI/CLI (calls DLX).

#include "solver.h"

// forward decl from dlx.c
int solve_dlx(int grid[9][9]);

// solver_hint(): helper function.
int solver_hint(Grid* g){
    int n=0;
    n += apply_naked_single(g);
    if(n) return 1;
    n += apply_hidden_single(g);
    return n>0;
}

// solve_dlx_grid(): helper function.
static int solve_dlx_grid(Grid* g){
    int a[9][9];
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) a[r][c] = g->v[r][c];
    int ok = solve_dlx(a);
    if(!ok) return 0;
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) g->v[r][c] = a[r][c];
    return 1;
}

// solver_solve(): helper function.
int solver_solve(Grid* g){

    while(solver_hint(g)){}
    if (!solve_dlx_grid(g)) return solve_dlx_grid(g);
    return 1;
}


// solve_get_cell(): helper function.
int solve_get_cell(const Grid* g_in, int r, int c, int* out){
    if(!g_in || !out) return 0;
    if(r<0||r>=9||c<0||c>=9) return 0;
    Grid g = *g_in;
    // run logic then backtracking
    while(solver_hint(&g)) {}
    if(!solve_dlx_grid(&g)) return 0;
    *out = g.v[r][c];
    return 1;
}