#include <stdio.h>

#define N 9  // Sudoku grid size

// ------------------------------------------------------
// Utility functions
// ------------------------------------------------------

// Check if placing num at grid[row][col] is safe
static int is_safe(int grid[N][N], int row, int col, int num) {
    // Check row
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num)
            return 0;

    // Check column
    for (int x = 0; x < N; x++)
        if (grid[x][col] == num)
            return 0;

    // Check 3x3 box
    int start_row = row - row % 3;
    int start_col = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[i + start_row][j + start_col] == num)
                return 0;

    return 1;
}

// ------------------------------------------------------
// Recursive backtracking solver
// ------------------------------------------------------

static int solve_recursive(int grid[N][N], int row, int col) {
    // If we reached the end → puzzle solved
    if (row == N - 1 && col == N)
        return 1;

    // Move to next row when end of column is reached
    if (col == N) {
        row++;
        col = 0;
    }

    // Skip pre-filled cells
    if (grid[row][col] != 0)
        return solve_recursive(grid, row, col + 1);

    // Try digits 1 to 9
    for (int num = 1; num <= 9; num++) {
        if (is_safe(grid, row, col, num)) {
            grid[row][col] = num;

            if (solve_recursive(grid, row, col + 1))
                return 1;

            // Backtrack
            grid[row][col] = 0;
        }
    }

    return 0;  // trigger backtracking
}

// ------------------------------------------------------
// Public function — can be called from main.c
// ------------------------------------------------------

/*
 * Solve a Sudoku using plain backtracking.
 *
 * The solver prints the solution to stdout and returns 1 on success
 * or 0 if no solution exists. The input grid is left unmodified.
 */
int solve_backtracking(int grid[N][N]) {
    printf("Solving with Backtracking...\n");

    int temp[N][N];
    // Copy the grid so original stays intact
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            temp[i][j] = grid[i][j];
        }
    }

    if (solve_recursive(temp, 0, 0)) {
        printf("Solved Sudoku (Backtracking):\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                printf("%d ", temp[i][j]);
            }
            printf("\n");
        }
        return 1;
    } else {
        printf("No solution exists for this Sudoku.\n");
        return 0;
    }
}
