// benchmark_k.c
// Este programa executa quatro algoritmos de resolução de Sudoku em
// 50 puzzles para cada valor de k (quantidade de células removidas).
// Ele imprime a média de tempo gasto por cada algoritmo para cada k
// e grava esses resultados em um arquivo CSV para posterior análise.

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 9

// Número de puzzles a testar para cada valor de k
#define NUM_TESTS 5

// Lista de valores de k a testar (quantas células são removidas). Ajuste conforme necessário.
static int k_values[] = {5, 15, 25, 35, 45, 60, 70, 80};
static const int num_k = sizeof(k_values) / sizeof(k_values[0]);

// Protótipos das funções de geração e solução
void generate_sudoku_k(int grid[N][N], int k);
void copy_grid(int src[N][N], int dest[N][N]);

int solve_backtracking(int grid[N][N]);
int solve_backtracking_mrv_bitmask(int grid[N][N]);
int solve_constraint_propagation(int grid[N][N]);
int solve_dlx(int grid[N][N]);

// Medir o tempo gasto por um solver sem produzir saída na tela
static double time_solver_silent(int (*solver)(int [N][N]), int grid[N][N]) {
    FILE *orig_stdout = stdout;
    FILE *null_file   = fopen("/dev/null", "w");
    if (!null_file) {
        fprintf(stderr, "Falha ao abrir /dev/null\n");
        return -1.0;
    }
    fflush(stdout);
    stdout = null_file;

    clock_t start = clock();
    solver(grid);
    clock_t end   = clock();

    fflush(stdout);
    stdout = orig_stdout;
    fclose(null_file);

    return (double)(end - start) / CLOCKS_PER_SEC;
}

int main(void) {
    srand((unsigned)time(NULL));

    // Abrir arquivo CSV para gravar resultados
    FILE *csv = fopen("benchmark_k_results.csv", "w");
    if (!csv) {
        fprintf(stderr, "Não foi possível criar benchmark_k_results.csv\n");
        return 1;
    }
    fprintf(csv, "k,backtracking_avg,mrv_avg,constraint_avg,dlx_avg\n");

    printf("Executando benchmark com %d puzzles para cada valor de k...\n\n", NUM_TESTS);

    // Loop sobre cada valor de k
    for (int idx = 0; idx < num_k; idx++) {
        int k = k_values[idx];
        double sum_back = 0.0, sum_mrv = 0.0, sum_const = 0.0, sum_dlx = 0.0;

        // Para cada puzzle
        for (int i = 0; i < NUM_TESTS; i++) {
            int puzzle[N][N];
            generate_sudoku_k(puzzle, k);

            // Backtracking simples
            int grid[N][N];
            copy_grid(puzzle, grid);
            sum_back += time_solver_silent(solve_backtracking, grid);

            // MRV + bitmask
            copy_grid(puzzle, grid);
            sum_mrv  += time_solver_silent(solve_backtracking_mrv_bitmask, grid);

            // Propagação de restrições
            copy_grid(puzzle, grid);
            sum_const+= time_solver_silent(solve_constraint_propagation, grid);

            // DLX
            copy_grid(puzzle, grid);
            sum_dlx  += time_solver_silent(solve_dlx, grid);
        }

        double avg_back = sum_back / NUM_TESTS;
        double avg_mrv  = sum_mrv  / NUM_TESTS;
        double avg_const= sum_const/ NUM_TESTS;
        double avg_dlx  = sum_dlx  / NUM_TESTS;

        // Imprimir resultados no terminal
        printf("k=%2d | Backtracking=%.6f s, MRV=%.6f s, Constraint=%.6f s, DLX=%.6f s\n",
               k, avg_back, avg_mrv, avg_const, avg_dlx);

        // Gravar no CSV
        fprintf(csv, "%d,%.6f,%.6f,%.6f,%.6f\n",
                k, avg_back, avg_mrv, avg_const, avg_dlx);
    }

    fclose(csv);

    printf("\nBenchmark concluído. Resultados gravados em 'benchmark_k_results.csv'.\n");
    return 0;
}