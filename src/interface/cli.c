// Minimal terminal CLI for Sudoku: generate/solve/print
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "generator.h"
#include "solver.h"

// Forward decls in case solver.h does not expose them
int solve_dlx(Grid*);
int solve_backtracking(Grid*);
int solve_backtracking_mrv_bitmask(Grid*);
int solve_constraint_propagation(Grid*);

// Pretty print a grid
static void print_grid(const Grid* g){
    for(int r=0;r<N;r++){
        for(int c=0;c<N;c++){
            int v = g->v[r][c];
            printf("%d%c", v, (c==N-1? '\n' : ' '));
        }
    }
}

// Read grid from text file: accepts digits 0-9 and '.'. Fills zeros for blanks.
// Expects at least 81 cells; extra characters are ignored.
static int read_grid_file(const char* path, Grid* g){
    for(int r=0;r<N;r++) for(int c=0;c<N;c++) g->v[r][c]=0;
    FILE* f = strcmp(path,"-")==0 ? stdin : fopen(path,"r");
    if(!f){ perror(path); return 0; }
    int r=0,c=0,ch;
    while((ch=fgetc(f))!=EOF && r<N){
        if(isdigit(ch) || ch=='.'){
            int v = (ch=='.') ? 0 : (ch-'0');
            g->v[r][c] = v;
            if(++c==N){ c=0; r++; }
        }
    }
    if(f!=stdin) fclose(f);
    return (r==N && c==0);
}

static void usage(const char* prog){
    fprintf(stderr,
        "Usage:\n"
        "  %s --generate <k>        # gera um puzzle com k buracos e imprime\n"
        "  %s --solve <file|->      # resolve puzzle do arquivo (ou '-' = stdin)\n"
        "  %s                        # gera k=40, resolve e imprime\n",
        prog, prog, prog);
}

int main(int argc, char** argv){
    Grid g;

    if(argc==3 && strcmp(argv[1],"--generate")==0){
        int k = atoi(argv[2]);
        if(k<0 || k>80){ fprintf(stderr,"k must be 0..80\n"); return 1; }
        generator_new_k(k, &g);
        print_grid(&g);
        return 0;
    }

    if(argc==3 && strcmp(argv[1],"--solve")==0){
        if(!read_grid_file(argv[2], &g)){
            fprintf(stderr,"Failed to read grid (need 81 cells 0-9 or '.')\n");
            return 2;
        }
        printf("Input grid:\n");
        print_grid(&g);
        Grid w = g;
        if(!solve_dlx(&w)){
            fprintf(stderr,"No solution found by DLX.\n");
            return 3;
        }
        printf("\nSolved grid (DLX):\n");
        print_grid(&w);
        return 0;
    }

    // Default demo: generate k=40 and solve with DLX
    int k = 40;
    generator_new_k(k, &g);
    printf("Generated grid (k=%d):\n", k);
    print_grid(&g);
    Grid w = g;
    if(!solve_dlx(&w)){
        fprintf(stderr,"No solution found by DLX.\n");
        return 3;
    }
    printf("\nSolved grid (DLX):\n");
    print_grid(&w);
    return 0;
}
