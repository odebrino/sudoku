#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 9
#define MAX_PUZZLES 100
#define MAX_LINE 128

// --- solver prototypes ---
int solve_backtracking(int grid[N][N]);
int solve_backtracking_mrv_bitmask(int grid[N][N]);
int solve_constraint_propagation(int grid[N][N]);
int solve_dlx(int grid[N][N]);
void copy_grid(int src[N][N], int dest[N][N]);

// --- timing utility ---
static double now_seconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

// --- comparison for median ---
static int cmp_double(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

// --- load puzzles from data/kX_grids.txt ---
static int load_puzzles(int k, int puzzles[MAX_PUZZLES][N][N]) {
    char path[256];
    snprintf(path, sizeof(path),
             "/home/odebrino/ENSTA/Language C/sudoku/data/k%d_grids.txt", k);
    FILE *f = fopen(path, "r");
    if (!f) {
        perror(path);
        return -1;
    }

    char line[MAX_LINE];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count < MAX_PUZZLES) {
        int len = strlen(line);
        if (line[len - 1] == '\n') line[len - 1] = '\0';
        if (strlen(line) != 81) continue;
        for (int i = 0; i < 81; i++)
            puzzles[count][i / 9][i % 9] = line[i] - '0';
        count++;
    }
    fclose(f);
    return count;
}

// --- main benchmark ---
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <k>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int k = atoi(argv[1]);
    int puzzles[MAX_PUZZLES][N][N];
    int count = load_puzzles(k, puzzles);
    if (count <= 0) {
        fprintf(stderr, "⚠️ No puzzles loaded for k=%d\n", k);
        return EXIT_FAILURE;
    }

    printf("📂 Running benchmark for k=%d with %d puzzles...\n", k, count);

    struct {
        const char *name;
        int (*solver)(int[N][N]);
    } solvers[] = {
        {"Backtracking", solve_backtracking},
        {"MRV_Bitmask", solve_backtracking_mrv_bitmask},
        {"Constraint", solve_constraint_propagation},
        {"DLX", solve_dlx},
    };

    char result_path[256];
    snprintf(result_path, sizeof(result_path),
             "/home/odebrino/ENSTA/Language C/sudoku/data/results_k%d.txt", k);

    FILE *out = fopen(result_path, "w");
    if (!out) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    fprintf(out, "Benchmark Results for k=%d\n", k);
    fprintf(out, "=========================================\n\n");

    int num_solvers = sizeof(solvers) / sizeof(solvers[0]);
    for (int s = 0; s < num_solvers; s++) {
        double times[MAX_PUZZLES];
        double total = 0.0, min = 1e9, max = 0.0;

        for (int i = 0; i < count; i++) {
            int grid[N][N];
            copy_grid(puzzles[i], grid);

            double start = now_seconds();
            solvers[s].solver(grid);
            double elapsed = now_seconds() - start;

            times[i] = elapsed;
            if (elapsed < min) min = elapsed;
            if (elapsed > max) max = elapsed;
            total += elapsed;
        }

        qsort(times, count, sizeof(double), cmp_double);
        double mean = total / count;
        double median = (count % 2 == 0)
                        ? (times[count/2 - 1] + times[count/2]) / 2.0
                        : times[count/2];

        fprintf(out, "Solver=%s\n", solvers[s].name);
        fprintf(out, "Min=%.10f\n", min);
        fprintf(out, "Max=%.10f\n", max);
        fprintf(out, "Median=%.10f\n", median);
        fprintf(out, "Mean=%.10f\n\n", mean);
        fflush(out);

        printf("   🧩 %-15s | min=%.10fs | max=%.10fs | median=%.10fs | mean=%.10fs\n",
               solvers[s].name, min, max, median, mean);
    }

    fclose(out);
    printf("✅ Results saved → %s\n", result_path);
    return EXIT_SUCCESS;
}
