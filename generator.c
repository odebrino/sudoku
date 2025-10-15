// generator.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 9

static int is_safe(int grid[N][N], int row, int col, int num) {
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num) return 0;
    for (int x = 0; x < N; x++)
        if (grid[x][col] == num) return 0;

    int r0 = row - row % 3, c0 = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[r0 + i][c0 + j] == num) return 0;
    return 1;
}

static int fill_grid(int grid[N][N], int row, int col) {
    if (row == N - 1 && col == N) return 1;
    if (col == N) { row++; col = 0; }
    if (grid[row][col] != 0) return fill_grid(grid, row, col + 1);

    int nums[N];
    for (int i = 0; i < N; i++) nums[i] = i + 1;
    for (int i = N - 1; i > 0; i--) { // shuffle
        int j = rand() % (i + 1);
        int t = nums[i]; nums[i] = nums[j]; nums[j] = t;
    }

    for (int i = 0; i < N; i++) {
        int num = nums[i];
        if (is_safe(grid, row, col, num)) {
            grid[row][col] = num;
            if (fill_grid(grid, row, col + 1)) return 1;
            grid[row][col] = 0;
        }
    }
    return 0;
}

static void remove_cells(int grid[N][N], int k) {
    if (k < 0) k = 0;
    if (k > 81) k = 81;
    int removed = 0;
    while (removed < k) {
        int cell = rand() % (N * N);
        int r = cell / N, c = cell % N;
        if (grid[r][c] != 0) {
            grid[r][c] = 0;
            removed++;
        }
    }
}

// --- Public API ---

void generate_sudoku_k(int grid[N][N], int k) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }

    // Start empty
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    // Optional: pre-fill diagonal 3x3 for speed
    for (int b = 0; b < N; b += 3) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int num;
                int r = b + i, c = b + j;
                int tries = 0;
                do {
                    num = (rand() % 9) + 1;
                    tries++;
                    if (tries > 50) break; // avoid rare stalls
                } while (!is_safe(grid, r, c, num));
                grid[r][c] = num;
            }
        }
    }

    // Complete full valid board
    fill_grid(grid, 0, 0);

    // Remove k cells (difficulty proxy)
    remove_cells(grid, k);
}

void generate_sudoku(int grid[N][N]) {
    generate_sudoku_k(grid, 70); // default ~medium-hard
}

void copy_grid(int src[N][N], int dest[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            dest[i][j] = src[i][j];
}
