// Shared utilities and types (Grid, helpers).

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>
#include <stdio.h>

#define N 9

typedef struct {
    int v[N][N];      // 0..9
    int fixed[N][N];  // 1 if clue
} Grid;

bool in_bounds(int r, int c);
void grid_copy(const Grid* src, Grid* dst);
void grid_clear(Grid* g);
int  grid_count_empty(const Grid* g);
int  grid_is_complete(const Grid* g);
int  grid_is_safe_cell(const Grid* g, int r, int c, int val);
int  grid_find_next_empty(const Grid* g, int* rr, int* cc);
void grid_print(const Grid* g, FILE* out);
int  grid_equals(const Grid* a, const Grid* b);

#endif