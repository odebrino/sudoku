#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --- function prototypes from your modules ---

void generate_sudoku(int grid[9][9]);                  // from generator.c
void copy_grid(int src[9][9], int dest[9][9]);         // from generator.c

int solve_backtracking(int grid[9][9]);                // from backtracking.c
int solve_backtracking_mrv_bitmask(int grid[9][9]);    // from backtracking_mrv_bitmask.c
int solve_constraint_propagation(int grid[9][9]);      // from constrain_propagation.c
int solve_dlx(int grid[9][9]);                         // from dlx.c

// --- helper to print a Sudoku grid ---
static void print_grid(int grid[9][9]) {
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0) printf("+-------+-------+-------+\n");
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0) printf("| ");
            if (grid[i][j] == 0) printf(". ");
            else printf("%d ", grid[i][j]);
        }
        printf("|\n");
    }
    printf("+-------+-------+-------+\n");
}

// --- main benchmark ---
int main(void) {
    srand((unsigned)time(NULL));

    int original[9][9];
    generate_sudoku(original);

    printf("Original puzzle:\n");
    print_grid(original);
    printf("\n");

    // prepare solver copies
    int g_backtracking[9][9], g_mrv[9][9], g_propagation[9][9], g_dlx[9][9];
    copy_grid(original, g_backtracking);
    copy_grid(original, g_mrv);
    copy_grid(original, g_propagation);
    copy_grid(original, g_dlx);

    clock_t start, end;
    double t;

    // --- 1. Plain Backtracking ---
    start = clock();
    int solved1 = solve_backtracking(g_backtracking);
    end = clock();
    t = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[1] Backtracking:      %s (%.4f s)\n", solved1 ? "solved" : "failed", t);

    // --- 2. MRV + Bitmask ---
    start = clock();
    int solved2 = solve_backtracking_mrv_bitmask(g_mrv);
    end = clock();
    t = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[2] MRV + Bitmask:     %s (%.4f s)\n", solved2 ? "solved" : "failed", t);

    // --- 3. Constraint Propagation ---
    start = clock();
    int solved3 = solve_constraint_propagation(g_propagation);
    end = clock();
    t = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[3] Constraint Prop:   %s (%.4f s)\n", solved3 ? "solved" : "failed", t);

    // --- 4. DLX (Knuth Algorithm X + Dancing Links) ---
    start = clock();
    int solved4 = solve_dlx(g_dlx);
    end = clock();
    t = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[4] DLX Solver:        %s (%.4f s)\n", solved4 ? "solved" : "failed", t);

    // optional: print one solved grid to verify correctness
    printf("\nDLX solved grid:\n");
    print_grid(g_dlx);

    return 0;
}
