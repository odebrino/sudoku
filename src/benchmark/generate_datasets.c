#include "trace_noop.h"
#ifndef TR_INIT
#define TR_INIT(x) ((void)0)
#define TR_ENTER() ((void)0)
#define TR_LEAVE() ((void)0)
#define TR_CLOSE() ((void)0)
#endif

// Dataset generator for various k levels.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "generator.h"
#include "utils.h"

#ifndef N
#define N 9
#endif

// ensure_dir(): helper function.
static void ensure_dir(const char* path){
    #ifdef _WIN32
    _mkdir(path);
    #else
    struct stat st = {0};
    if (stat(path, &st) == -1){
        mkdir(path, 0755);
    }
    #endif
}

// main(): helper function.
int main(int argc, char** argv){ (void)argc; (void)argv;
    int ks[] = {5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80};
  TR_INIT(NULL);
  TR_ENTER();
    int num = sizeof(ks)/sizeof(ks[0]);
    int puzzles_per_k = 100;

    ensure_dir("data");
    ensure_dir("data/grids");

    for(int i=0;i<num;i++){
        int k = ks[i];
        char path[256];
        snprintf(path,sizeof(path),"data/grids/k%d_grids.txt",k);
        FILE* out = fopen(path,"w");
        if(!out){ perror("fopen"); return 1; }

        for(int t=0;t<puzzles_per_k;t++){
            Grid g;
            if(!generator_new_k(k, &g)){ fprintf(stderr,"gen fail k=%d\n",k); continue; }
            // emit 81-char line (0..9)
            for(int r=0;r<N;r++){
                for(int c=0;c<N;c++){
                    fputc('0'+(g.v[r][c]%10), out);
                }
            }
            fputc('\n', out);
        }
        fclose(out);
        printf("✅ wrote %s\n", path);
    }
    TR_LEAVE(); TR_CLOSE(); return 0;
}
