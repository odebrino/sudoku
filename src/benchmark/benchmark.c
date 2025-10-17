// Benchmark runner to time solvers on datasets.

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "solver.h"
#include "utils.h"

#define N 9
#define MAX_PUZZLES 100
#define MAX_LINE 128

// now_seconds(): helper function.
static double now_seconds(void){
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec/1e9;
}

// parse_line(): helper function.
static void parse_line(const char* s, int a[N][N]){
    int idx=0;
    for(int r=0;r<N;r++) for(int c=0;c<N;c++){
        char ch = s[idx++];
        while(ch==' '||ch=='\t'||ch=='\n'||ch=='\r') ch=s[idx++];
        if(ch<'0'||ch>'9') { a[r][c]=0; continue; }
        a[r][c]=ch-'0';
    }
}

// load_puzzles(): helper function.
static int load_puzzles(int k, int puzzles[MAX_PUZZLES][N][N]){
    char path[256];
    snprintf(path,sizeof(path),"data/grids/k%d_grids.txt",k);
    FILE* f=fopen(path,"r");
    if(!f) return 0;
    char line[MAX_LINE];
    int count=0;
    while(count<MAX_PUZZLES && fgets(line,sizeof(line),f)){
        if(strlen(line)<81) continue;
        parse_line(line, puzzles[count]);
        count++;
    }
    fclose(f);
    return count;
}

extern int solve_dlx(int grid[9][9]); // from dlx.c

// grid_copy_arr(): helper function.
static void grid_copy_arr(int src[9][9], int dst[9][9]){
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) dst[r][c]=src[r][c];
}

// main(): helper function.
int main(int argc, char** argv){
    if(argc<2){ fprintf(stderr,"usage: %s <k>\n", argv[0]); return 1; }
    int k = atoi(argv[1]);
    int puzzles[MAX_PUZZLES][N][N];
    int count = load_puzzles(k, puzzles);
    if(count<=0){ fprintf(stderr,"No puzzles for k=%d\n",k); return 1; }

    char result_path[256];
    snprintf(result_path,sizeof(result_path),"results/results_k%d.txt",k);
    FILE* out=fopen(result_path,"w");
    if(!out){ perror("fopen results"); return 1; }

    fprintf(out,"Benchmark Results for k=%d\n",k);
    fprintf(out,"name,time_ms,solved\n");

    double total=0.0; int solved_all=0;
    for(int i=0;i<count;i++){
        int g[9][9]; grid_copy_arr(puzzles[i], g);
        double t0=now_seconds();
        int ok = solve_dlx(g);
        double t1=now_seconds();
        double ms=(t1-t0)*1000.0;
        total+=ms; solved_all += ok?1:0;
        fprintf(out,"dlx,%.3f,%d\n", ms, ok?1:0);
    }
    fclose(out);
    printf("✅ Results saved → %s (avg=%.3f ms over %d)\n", result_path, total/(double)count, count);
    return 0;
}