#include <stdio.h>

#define N 9

// ------------------------------------------------------
// Bitmask helpers
// ------------------------------------------------------

// Set bit for number n (1–9)
static inline int bit(int n) {
    return 1 << (n - 1);
}

// ------------------------------------------------------
// Utility: find next cell with minimum remaining values
// ------------------------------------------------------
static int find_mrv_cell(int grid[N][N], int rows[N], int cols[N], int boxes[N],
                         int *out_r, int *out_c) {
    int min_count = 10; // start higher than possible
    int found = 0;

    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] != 0)
                continue;

            int box = (r / 3) * 3 + (c / 3);
            int used = rows[r] | cols[c] | boxes[box];
            int available = (~used) & 0x1FF; // bits 0–8 for numbers 1–9

            int count = 0;
            for (int n = 1; n <= 9; n++)
                if (available & bit(n))
                    count++;

            if (count < min_count) {
                min_count = count;
                *out_r = r;
                *out_c = c;
                found = 1;
            }
        }
    }

    return found;
}

// ------------------------------------------------------
// Recursive MRV + Bitmask solver
// ------------------------------------------------------
static int solve_mrv_recursive(int grid[N][N], int rows[N], int cols[N], int boxes[N]) {
    int r, c;
    if (!find_mrv_cell(grid, rows, cols, boxes, &r, &c))
        return 1; // no empty cell left -> solved

    int box = (r / 3) * 3 + (c / 3);
    int used = rows[r] | cols[c] | boxes[box];
    int available = (~used) & 0x1FF;

    while (available) {
        int lowest_bit = available & -available;
        int num = 1;
        while ((lowest_bit >> (num - 1)) != 1)
            num++;

        available &= ~lowest_bit;

        grid[r][c] = num;
        rows[r] |= bit(num);
        cols[c] |= bit(num);
        boxes[box] |= bit(num);

        if (solve_mrv_recursive(grid, rows, cols, boxes))
            return 1;

        // Backtrack
        grid[r][c] = 0;
        rows[r] &= ~bit(num);
        cols[c] &= ~bit(num);
        boxes[box] &= ~bit(num);
    }

    return 0;
}

// ------------------------------------------------------
// Public interface
// ------------------------------------------------------
/*
 * Solve a Sudoku using a backtracking algorithm enhanced with
 * Minimum Remaining Values (MRV) heuristic and bitmasking. Prints
 * the solved grid to stdout and returns 1 if a solution is found,
 * otherwise returns 0. The input grid is left unmodified.
 */
int solve_backtracking_mrv_bitmask(int grid[N][N]) {
    printf("Solving with MRV + Bitmask Backtracking...\n");

    int temp[N][N];
    int rows[N] = {0}, cols[N] = {0}, boxes[N] = {0};

    // Copy grid and initialize bitmasks
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            temp[r][c] = grid[r][c];
            int num = temp[r][c];
            if (num) {
                int b = (r / 3) * 3 + (c / 3);
                rows[r] |= bit(num);
                cols[c] |= bit(num);
                boxes[b] |= bit(num);
            }
        }
    }

    if (solve_mrv_recursive(temp, rows, cols, boxes)) {
        printf("Solved Sudoku (MRV + Bitmask):\n");
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                printf("%d ", temp[r][c]);
            }
            printf("\n");
        }
        return 1;
    } else {
        printf("No solution exists (MRV + Bitmask).\n");
        return 0;
    }
}
