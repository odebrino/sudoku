// Puzzle generator using DLX to enforce uniqueness.

#include "generator.h"
#include "solver.h"
#include "utils.h"

int rnd_int(int lo, int hi);
void rnd_shuffle_int(int* a, int n);

// fill(): helper function.
static int fill(Grid* g){
    int r,c;
    if(!grid_find_next_empty(g,&r,&c)) return 1;
    int vals[9]; for(int i=0;i<9;i++) vals[i]=i+1;
    rnd_shuffle_int(vals,9);
    for(int i=0;i<9;i++){
        int v=vals[i];
        if(grid_is_safe_cell(g,r,c,v)){
            g->v[r][c]=v;
            if(fill(g)) return 1;
            g->v[r][c]=0;
        }
    }
    return 0;
}

// generator_full_solution(): helper function.
void generator_full_solution(Grid* out){
    grid_clear(out);
    fill(out);
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) out->fixed[r][c]=1;
}

// generator_new_k(): helper function.
int generator_new_k(int k, Grid* out){
    if (k < 0) { k = 0; }
    else if (k > 60) { k = 60; }
    Grid sol; generator_full_solution(&sol);
    Grid puzzle=sol;

    int idx[81]; for(int i=0;i<81;i++) idx[i]=i;
    rnd_shuffle_int(idx,81);

    int holes=0;
    for(int i=0;i<81 && holes<k;i++){
        int r=idx[i]/9, c=idx[i]%9;
        int bak=puzzle.v[r][c];
        if(bak==0) continue;
        puzzle.v[r][c]=0; puzzle.fixed[r][c]=0;
        Grid tmp=puzzle;
        int cnt = count_solutions_dlx(&tmp, 2);
        if(cnt!=1){ puzzle.v[r][c]=bak; puzzle.fixed[r][c]=1; }
        else holes++;
    }
    for(int r=0;r<9;r++) for(int c=0;c<9;c++)
        puzzle.fixed[r][c] = (puzzle.v[r][c]!=0);

    *out=puzzle;
    return 1;
}