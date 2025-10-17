// Sudoku constraint helpers (row/col/box checks).

#include "solver.h"
#include "trace.h"
#include <string.h>

// candidates_for(): helper function.
static void candidates_for(const Grid* g, int r, int c, int cand[10]){
    for(int i=0;i<10;i++) cand[i]=0;
    for(int v=1;v<=9;v++) if(grid_is_safe_cell(g,r,c,v)) cand[v]=1;
}

// apply_naked_single(): helper function.
int apply_naked_single(Grid* g){
    int changes=0;
    for(int r=0;r<9;r++){
        for(int c=0;c<9;c++){
            if(g->v[r][c]==0){
                int cand[10]; candidates_for(g,r,c,cand);
                int last=0, cnt=0;
                for(int v=1;v<=9;v++) if(cand[v]){ last=v; cnt++; }
                if(cnt==1){ g->v[r][c]=last; changes++; }
            }
        }
    }
    return changes;
}

// hidden_unit(): helper function.
static int hidden_unit(const Grid* g, int coords[][2], int len, int* rr,int* cc,int* vv){
    int present[10]={0};
    int possible[10]={0};
    int wr[10]={0}, wc[10]={0};
    for(int i=0;i<len;i++){
        int r=coords[i][0], c=coords[i][1];
        if(g->v[r][c]){ present[g->v[r][c]]=1; continue; }
        for(int v=1;v<=9;v++){
            if(grid_is_safe_cell(g,r,c,v)){ possible[v]++; wr[v]=r; wc[v]=c; }
        }
    }
    for(int v=1;v<=9;v++){
        if(!present[v] && possible[v]==1){ *rr=wr[v]; *cc=wc[v]; *vv=v; return 1; }
    }
    return 0;
}

// apply_hidden_single(): helper function.
int apply_hidden_single(Grid* g){
    int changes=0, r,c,v;
    for(int i=0;i<9;i++){
        int coords[9][2]; for(int j=0;j<9;j++){ coords[j][0]=i; coords[j][1]=j; }
        while(hidden_unit(g,coords,9,&r,&c,&v)){ g->v[r][c]=v; changes++; }
    }
    for(int j=0;j<9;j++){
        int coords[9][2]; for(int i=0;i<9;i++){ coords[i][0]=i; coords[i][1]=j; }
        while(hidden_unit(g,coords,9,&r,&c,&v)){ g->v[r][c]=v; changes++; }
    }
    for(int br=0;br<3;br++) for(int bc=0;bc<3;bc++){
        int coords[9][2], k=0;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++){ coords[k][0]=br*3+i; coords[k][1]=bc*3+j; k++; }
        while(hidden_unit(g,coords,9,&r,&c,&v)){ g->v[r][c]=v; changes++; }
    }
    return changes;
}