#include <stdio.h>
#include <stdint.h>

#define N 9  // Board size

// ---------- Candidate bitmask (bits 1..9) ----------
#define BIT(d) (1u << (d))
#define FULL_MASK (BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9))

// ---------- Internal state ----------
static int grid[N][N];
static uint16_t candMask[N][N];
static int verbose = 0;

// ------------------------------------------------------
// Utility functions
// ------------------------------------------------------

static inline int boxIndex(int r, int c) { return (r / 3) * 3 + (c / 3); }

static inline int can_place(int r, int c, int d) {
    for (int cc = 0; cc < N; cc++) if (grid[r][cc] == d) return 0;
    for (int rr = 0; rr < N; rr++) if (grid[rr][c] == d) return 0;
    int r0 = (r / 3) * 3, c0 = (c / 3) * 3;
    for (int rr = r0; rr < r0 + 3; rr++)
        for (int cc = c0; cc < c0 + 3; cc++)
            if (grid[rr][cc] == d) return 0;
    return 1;
}

static inline void place(int r, int c, int d) {
    grid[r][c] = d;
    if (verbose) printf("Colocando %d em (%d,%d)\n", d, r, c);
}

static inline void unplace(int r, int c) {
    if (verbose) printf("Removendo %d de (%d,%d)\n", grid[r][c], r, c);
    grid[r][c] = 0;
}

static inline int count_bits(uint16_t m) {
    int cnt = 0;
    for (int d = 1; d <= 9; d++)
        if (m & BIT(d)) cnt++;
    return cnt;
}

static inline int singleton_digit(uint16_t m) {
    for (int d = 1; d <= 9; d++)
        if (m & BIT(d)) return d;
    return 0;
}

// ------------------------------------------------------
// Candidate recomputation and propagation
// ------------------------------------------------------

static int recompute_candidates(void) {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (grid[r][c] != 0) {
                candMask[r][c] = 0;
            } else {
                uint16_t mask = FULL_MASK;

                // Eliminate row, col, box digits
                for (int cc = 0; cc < N; cc++)
                    if (grid[r][cc]) mask &= ~BIT(grid[r][cc]);
                for (int rr = 0; rr < N; rr++)
                    if (grid[rr][c]) mask &= ~BIT(grid[rr][c]);
                int r0 = (r / 3) * 3, c0 = (c / 3) * 3;
                for (int rr = r0; rr < r0 + 3; rr++)
                    for (int cc = c0; cc < c0 + 3; cc++)
                        if (grid[rr][cc]) mask &= ~BIT(grid[rr][cc]);

                candMask[r][c] = mask;
                if (mask == 0) return 0; // Contradiction
            }
        }
    }
    return 1;
}

static int propagate(void) {
    if (!recompute_candidates()) return 0;
    int progress;
    do {
        progress = 0;
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                if (grid[r][c] == 0) {
                    uint16_t m = candMask[r][c];
                    int cnt = count_bits(m);
                    if (cnt == 0) return 0;
                    if (cnt == 1) {
                        int d = singleton_digit(m);
                        place(r, c, d);
                        progress = 1;
                    }
                }
            }
        }
        if (progress && !recompute_candidates()) return 0;
    } while (progress);
    return 1;
}

static int pick_cell(int *out_r, int *out_c) {
    for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++)
            if (grid[r][c] == 0) { *out_r = r; *out_c = c; return 1; }
    return 0;
}

// ------------------------------------------------------
// Recursive solver
// ------------------------------------------------------

static int solve_recursive(void) {
    if (!propagate()) return 0;

    int r, c;
    if (!pick_cell(&r, &c)) return 1; // Solved

    uint16_t m = candMask[r][c];
    for (int d = 1; d <= 9; d++) {
        if (m & BIT(d)) {
            if (!can_place(r, c, d)) continue;
            place(r, c, d);
            if (solve_recursive()) return 1;
            unplace(r, c);
        }
    }
    return 0;
}

// ------------------------------------------------------
// Public interface
// ------------------------------------------------------

/*
 * Solve a Sudoku using constraint propagation combined with backtracking.
 * The solved grid is printed to stdout. Returns 1 on success and
 * 0 if no solution exists. The input grid is left unmodified.
 */
int solve_constraint_propagation(int input_grid[N][N]) {
    printf("Solving with Constraint Propagation + Backtracking...\n");

    // Copy puzzle to internal grid
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            grid[r][c] = input_grid[r][c];
        }
    }

    if (!recompute_candidates()) {
        printf("Erro: tabuleiro inicial inválido.\n");
        return 0;
    }

    if (solve_recursive()) {
        printf("Solved Sudoku (Constraint Propagation):\n");
        for (int r = 0; r < N; r++) {
            for (int c = 0; c < N; c++) {
                printf("%d ", grid[r][c]);
            }
            printf("\n");
        }
        return 1;
    } else {
        printf("Sem solução encontrada.\n");
        return 0;
    }
}
