// Small helper module.

#include "solver.h"
#include "trace.h"

// bt(): helper function.
static int bt(Grid* g){
    int r,c;
    if(!grid_find_next_empty(g,&r,&c)) return 1;
    for(int v=1;v<=9;v++){
        if(grid_is_safe_cell(g,r,c,v)){
            trace_emit(TRACE_TRY,r,c,v);
            g->v[r][c]=v;
            trace_emit(TRACE_ASSIGN,r,c,v);
            if(bt(g)) return 1;
            g->v[r][c]=0;
            trace_emit(TRACE_BACKTRACK,r,c,v);
        }
    }
    return 0;
}

// solve_backtracking(): helper function.
int solve_backtracking(Grid* g){ return bt(g); }

// count_bt(): helper function.
static int count_bt(Grid* g, int limit, int* count){
    if(*count>=limit) return 1;
    int r,c;
    if(!grid_find_next_empty(g,&r,&c)){
        (*count)++;
        return (*count>=limit);
    }
    for(int v=1;v<=9;v++){
        if(grid_is_safe_cell(g,r,c,v)){
            g->v[r][c]=v;
            if(count_bt(g,limit,count)) return 1;
            g->v[r][c]=0;
        }
    }
    return 0;
}

// count_solutions(): helper function.
int count_solutions(Grid* g, int limit){
    int cnt=0; Grid tmp=*g; count_bt(&tmp, limit, &cnt); return cnt;
}