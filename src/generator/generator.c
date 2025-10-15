#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 9

// Verifica se um número pode ser colocado numa posição
static int is_safe(int grid[N][N], int row, int col, int num) {
    for (int x = 0; x < N; x++)
        if (grid[row][x] == num || grid[x][col] == num)
            return 0;

    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[startRow + i][startCol + j] == num)
                return 0;

    return 1;
}

// Preenche o grid completo de Sudoku via backtracking
static int fill_grid(int grid[N][N], int row, int col) {
    if (row == N - 1 && col == N)
        return 1;
    if (col == N)
        return fill_grid(grid, row + 1, 0);
    if (grid[row][col] != 0)
        return fill_grid(grid, row, col + 1);

    int nums[N];
    for (int i = 0; i < N; i++) nums[i] = i + 1;

    // Embaralha os números (Fisher-Yates)
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = nums[i];
        nums[i] = nums[j];
        nums[j] = tmp;
    }

    for (int i = 0; i < N; i++) {
        int num = nums[i];
        if (is_safe(grid, row, col, num)) {
            grid[row][col] = num;
            if (fill_grid(grid, row, col + 1))
                return 1;
            grid[row][col] = 0;
        }
    }
    return 0;
}

// Remove k células aleatoriamente (define o "nível" de dificuldade)
static void remove_cells(int grid[N][N], int k) {
    if (k < 0) k = 0;
    if (k > N * N) k = N * N;

    int cells[N * N];
    for (int i = 0; i < N * N; i++)
        cells[i] = i;

    for (int i = N * N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = cells[i];
        cells[i] = cells[j];
        cells[j] = tmp;
    }

    for (int i = 0; i < k; i++) {
        int r = cells[i] / N;
        int c = cells[i] % N;
        grid[r][c] = 0;
    }
}

// Função pública: gera Sudoku com k células removidas
void generate_sudoku_k(int grid[N][N], int k) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            grid[i][j] = 0;

    // Pré-preenche diagonais 3x3 (otimiza muito o backtracking)
    for (int b = 0; b < N; b += 3) {
        int used[10] = {0};
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int num;
                do { num = (rand() % 9) + 1; } while (used[num]);
                used[num] = 1;
                grid[b + i][b + j] = num;
            }
        }
    }

    fill_grid(grid, 0, 0);
    remove_cells(grid, k);
}

void copy_grid(int src[9][9], int dest[9][9]) {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            dest[i][j] = src[i][j];
}