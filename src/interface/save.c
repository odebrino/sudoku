// Serialize/deserialize game state (JSON-like).

#include "interface.h"
#include <stdio.h>

// write_json(): helper function.
static void write_json(FILE* f, const Grid* g){
    fprintf(f, "{\"grid\":[");
    for(int r=0;r<9;r++){
        fprintf(f, "[");
        for(int c=0;c<9;c++){
            fprintf(f, "%d", g->v[r][c]);
            if(c<8) fputc(',',f);
        }
        fprintf(f, "]");
        if(r<8) fputc(',',f);
    }
    fprintf(f, "],\"fixed\":[");
    for(int r=0;r<9;r++){
        fprintf(f, "[");
        for(int c=0;c<9;c++){
            fprintf(f, "%d", g->fixed[r][c]);
            if(c<8) fputc(',',f);
        }
        fprintf(f, "]");
        if(r<8) fputc(',',f);
    }
    fprintf(f, "]}");
}

// read_int(): helper function.
static int read_int(FILE* f, int* out){
    int ch; int sign=1; int val=0; int started=0;
    while((ch=fgetc(f))!=EOF){
        if(ch=='-'){ sign=-1; started=1; break; }
        if(ch>='0' && ch<='9'){ val=ch-'0'; started=1; break; }
    }
    if(!started) return 0;
    while((ch=fgetc(f))!=EOF && ch>='0' && ch<='9'){ val = val*10 + (ch-'0'); }
    *out = sign*val;
    return 1;
}

// save_game(): helper function.
int save_game(const char* path, const Grid* g){
    const char* p = path?path:"data/save.json";
    FILE* f = fopen(p, "w");
    if(!f) return 0;
    write_json(f, g);
    fclose(f);
    return 1;
}

// load_game(): helper function.
int load_game(const char* path, Grid* g){
    const char* p = path?path:"data/save.json";
    FILE* f = fopen(p, "r");
    if(!f) return 0;
    for(int r=0;r<9;r++) for(int c=0;c<9;c++){
        if(!read_int(f, &g->v[r][c])){ fclose(f); return 0; }
    }
    for(int r=0;r<9;r++) for(int c=0;c<9;c++){
        if(!read_int(f, &g->fixed[r][c])){ fclose(f); return 0; }
    }
    fclose(f);
    return 1;
}