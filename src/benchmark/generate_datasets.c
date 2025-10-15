#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define N 9
#define GRID_SIZE 81
#define DATASET_SIZE 100

// protótipo (definido em generator.c)
void generate_sudoku_k(int grid[N][N], int k);

static void grid_to_string(int grid[N][N], char *str) {
    int pos = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            str[pos++] = grid[i][j] + '0';
    str[pos] = '\0';
}

int main(int argc, char *argv[]) {
    int ks[] = {5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80};
    int num_ks = sizeof(ks) / sizeof(ks[0]);

    unsigned seed = (unsigned)time(NULL);
    if (argc >= 3) seed = (unsigned)atoi(argv[2]);
    srand(seed);

    if (argc >= 2) {
        ks[0] = atoi(argv[1]);
        num_ks = 1;
    }

    int grid[N][N];
    char str[GRID_SIZE + 1];
    char path[256];

    printf("🌱 RNG seeded with %u\n", seed);

    for (int idx = 0; idx < num_ks; idx++) {
        int k = ks[idx];
        snprintf(path, sizeof(path), "data/k%d_grids.txt", k);
        FILE *f = fopen(path, "w");
        if (!f) {
            fprintf(stderr, "❌ Erro ao abrir %s: %s\n", path, strerror(errno));
            continue;
        }

        printf("📘 Gerando dataset para k=%d...\n", k);

        for (int i = 0; i < DATASET_SIZE; i++) {
            generate_sudoku_k(grid, k);
            grid_to_string(grid, str);
            fprintf(f, "%s\n", str);
        }

        fclose(f);
        printf("✅ %d puzzles salvos → %s\n", DATASET_SIZE, path);
    }

    printf("🎉 Geração de datasets concluída.\n");
    return 0;
}
