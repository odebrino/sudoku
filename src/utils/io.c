// Small I/O helpers for save/load.

#include "utils.h"
#include <string.h>

// read_text_grid(): helper function.
int read_text_grid(FILE* f, Grid* g){
    grid_clear(g);
    int r=0,c=0; int ch;
    while((ch=fgetc(f))!=EOF && r<9){
        if(ch=='.' || ch=='0'){ g->v[r][c]=0; c++; }
        else if(ch>='1' && ch<='9'){ g->v[r][c]=ch-'0'; g->fixed[r][c]=1; c++; }
        else if(ch=='\n' || ch==' ' || ch=='\t' || ch=='\r'){ }
        if(c==9){ r++; c=0; }
    }
    return (r==9 && c==0)?1:0;
}

// write_text_grid(): helper function.
int write_text_grid(FILE* f, const Grid* g){
    for(int r=0;r<9;r++){
        for(int c=0;c<9;c++){
            int v=g->v[r][c];
            fputc(v?('0'+v):'.', f);
        }
        fputc('\n', f);
    }
    return 1;
}