// Grid helpers for values, validity and small utilities.

#include "utils.h"
#include <string.h>

// in_bounds(): helper function.
bool in_bounds(int r, int c){ return r>=0 && r<9 && c>=0 && c<9; }

// grid_copy(): helper function.
void grid_copy(const Grid* src, Grid* dst){ memcpy(dst, src, sizeof(Grid)); }

// grid_clear(): helper function.
void grid_clear(Grid* g){ memset(g, 0, sizeof(Grid)); }

// grid_count_empty(): helper function.
int grid_count_empty(const Grid* g){
    int k=0;
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) if(g->v[r][c]==0) k++;
    return k;
}

// grid_is_complete(): helper function.
int grid_is_complete(const Grid* g){ return grid_count_empty(g)==0; }

// grid_is_safe_cell(): helper function.
int grid_is_safe_cell(const Grid* g, int r, int c, int val){
    if(val<1 || val>9) return 0;
    for(int i=0;i<9;i++){
        if(g->v[r][i]==val) return 0;
        if(g->v[i][c]==val) return 0;
    }
    int br=(r/3)*3, bc=(c/3)*3;
    for(int i=0;i<3;i++) for(int j=0;j<3;j++)
        if(g->v[br+i][bc+j]==val) return 0;
    return 1;
}

// grid_find_next_empty(): helper function.
int grid_find_next_empty(const Grid* g, int* rr, int* cc){
    int best=10, br=-1, bc=-1;
    for(int r=0;r<9;r++) for(int c=0;c<9;c++){
        if(g->v[r][c]==0){
            int cnt=0;
            for(int v=1;v<=9;v++) if(grid_is_safe_cell(g,r,c,v)) cnt++;
            if(cnt<best){ best=cnt; br=r; bc=c; if(cnt==1) goto done; }
        }
    }
done:
    if(br<0) return 0;
    *rr=br; *cc=bc; return 1;
}

// grid_print(): helper function.
void grid_print(const Grid* g, FILE* out){
    fprintf(out, "+-------+-------+-------+\n");
    for(int r=0;r<9;r++){
        fprintf(out, "| ");
        for(int c=0;c<9;c++){
            int v=g->v[r][c];
            fprintf(out, "%c ", v?('0'+v):'.');
            if(c%3==2) fprintf(out, "| ");
        }
        fprintf(out, "\n");
        if(r%3==2) fprintf(out, "+-------+-------+-------+\n");
    }
}

// grid_equals(): helper function.
int grid_equals(const Grid* a, const Grid* b){ return memcmp(a,b,sizeof(Grid))==0; }

// grid_make_initial(): helper function.
void grid_make_initial(const Grid* src, Grid* out){
    *out = *src;
    for(int r=0;r<9;r++){
        for(int c=0;c<9;c++){
            if(!src->fixed[r][c]) out->v[r][c] = 0;
        }
    }
}