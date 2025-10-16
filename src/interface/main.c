#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define N 9

// ---- Prototypes from your modules ----
// (generator.c)
void generate_sudoku_k(int grid[N][N], int k);
void copy_grid(int src[N][N], int dest[N][N]);
// (dlx.c — for validation/hints/solution)
int solve_dlx(int grid[N][N]);

// ---- Game state ----
static int initial[N][N];   // puzzle given cells (immutable)
static int board[N][N];     // current board (player edits)
static int solved[N][N];    // solution (precomputed at generation)
static int is_given[N][N];  // mask for fixed cells (1 = cannot edit)

// ---- Timer (with pause support) ----
static time_t start_time;          // when the current game started
static int paused = 0;             // 1 if paused
static time_t pause_start;         // when pause began
static int paused_accum_secs = 0;  // total paused seconds so far
static int game_running = 0;

// ---- Utils ----
static int in_range(int r, int c) { return r >= 1 && r <= 9 && c >= 1 && c <= 9; }

static int is_safe_move(int g[N][N], int r, int c, int v) {
    // r,c in 1..9 ; v in 1..9
    int i, j;
    // row
    for (j = 1; j <= 9; j++) if (g[r-1][j-1] == v) return 0;
    // column
    for (i = 1; i <= 9; i++) if (g[i-1][c-1] == v) return 0;
    // 3x3 box
    int r0 = ((r-1)/3)*3, c0 = ((c-1)/3)*3;
    for (i = r0; i < r0+3; i++)
        for (j = c0; j < c0+3; j++)
            if (g[i][j] == v) return 0;
    return 1;
}

static void print_grid(int g[N][N]) {
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0) printf("+=======+=======+=======+\n");
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0) printf("| ");
            if (g[i][j] == 0) printf(". ");
            else              printf("%d ", g[i][j]);
        }
        printf("|\n");
    }
    printf("+=======+=======+=======+\n");
}

static void print_help(void) {
    printf(
        "Commands:\n"
        "  show                      -> print the current board\n"
        "  set r c v                 -> put value v (1..9) at cell (r,c)\n"
        "  clear r c                 -> clear cell (r,c)\n"
        "  check                     -> check if the board is complete and correct\n"
        "  hint                      -> fill ONE correct cell for you\n"
        "  solve                     -> fill the whole board with the solution\n"
        "  time                      -> show elapsed time\n"
        "  pause                     -> pause the game (timer stops; moves are blocked)\n"
        "  resume                    -> resume the game\n"
        "  new                       -> start a new game (opens difficulty menu)\n"
        "  new <easy|medium|hard|expert>\n"
        "                            -> start a new game with a named difficulty\n"
        "  new <k>                   -> start a new game removing k cells (0..81)\n"
        "  help                      -> show this menu\n"
        "  quit                      -> exit the game\n"
        "\nCoordinates (r,c) are 1..9. Given cells from the puzzle cannot be modified.\n"
    );
}

// ---- Timer helpers (pause-aware) ----
static void start_timer(void) {
    start_time = time(NULL);
    paused = 0;
    paused_accum_secs = 0;
}

static int elapsed_seconds(void) {
    if (!game_running) return 0;
    time_t now = time(NULL);
    int base = (int)difftime(now, start_time);
    int paused_now = paused ? (int)difftime(now, pause_start) : 0;
    int e = base - paused_accum_secs - paused_now;
    return e < 0 ? 0 : e;
}

static void show_time(void) {
    if (!game_running) { printf("No active game.\n"); return; }
    int e = elapsed_seconds();
    int mm = e / 60, ss = e % 60;
    printf("Elapsed time: %02d:%02d%s\n", mm, ss, paused ? " (paused)" : "");
}

static void set_paused(int on) {
    if (on) {
        if (!paused) {
            paused = 1;
            pause_start = time(NULL);
            printf("Game paused. Type 'resume' to continue.\n");
        } else {
            printf("Game is already paused.\n");
        }
    } else {
        if (paused) {
            time_t now = time(NULL);
            paused_accum_secs += (int)difftime(now, pause_start);
            paused = 0;
            printf("Game resumed.\n");
        } else {
            printf("Game is not paused.\n");
        }
    }
}

static int moves_allowed(void) {
    if (paused) {
        printf("Game is paused. Type 'resume' to continue.\n");
        return 0;
    }
    return 1;
}

static int is_complete(int g[N][N]) {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (g[i][j] == 0) return 0;
    return 1;
}

static int equals(int a[N][N], int b[N][N]) {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (a[i][j] != b[i][j]) return 0;
    return 1;
}

// ---- Difficulty mapping ----
static int k_for_difficulty(const char *name) {
    // Higher k -> more removed -> harder (tune as you like)
    if (strcasecmp(name, "easy")   == 0) return 40;
    if (strcasecmp(name, "medium") == 0) return 50;
    if (strcasecmp(name, "hard")   == 0) return 60;
    if (strcasecmp(name, "expert") == 0) return 70;
    return -1; // unknown
}

static int choose_difficulty_menu(void) {
    // Simple terminal “window” (menu)
    printf("\n================= SUDOKU =================\n");
    printf("Choose difficulty:\n");
    printf("  1) Easy\n");
    printf("  2) Medium\n");
    printf("  3) Hard\n");
    printf("  4) Expert\n");
    printf("------------------------------------------\n");
    printf("Enter 1-4 or name (easy/medium/hard/expert): ");

    char buf[64];
    if (!fgets(buf, sizeof(buf), stdin)) return 60; // fallback
    // strip newline
    buf[strcspn(buf, "\r\n")] = 0;

    // numeric?
    if (strlen(buf) == 1 && buf[0] >= '1' && buf[0] <= '4') {
        int opt = buf[0] - '0';
        switch (opt) {
            case 1: return 40;
            case 2: return 50;
            case 3: return 60;
            case 4: return 70;
        }
    }

    int k = k_for_difficulty(buf);
    if (k > 0) return k;

    printf("Unknown choice. Defaulting to Hard.\n");
    return 60;
}

// ---- Start a new game with ~k removed cells (higher k = harder) ----
static int new_game(int k) {
    int puzzle[N][N];

    // Generate puzzle
    generate_sudoku_k(puzzle, k);
    memcpy(initial, puzzle, sizeof(initial));
    memcpy(board,   puzzle, sizeof(board));

    // Mark given cells
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            is_given[i][j] = (initial[i][j] != 0);

    // Compute solution via DLX (Algorithm X / Dancing Links)
    int temp[N][N];
    memcpy(temp, puzzle, sizeof(temp));
    if (!solve_dlx(temp)) {
        // Rare case (bad puzzle). Signal to retry.
        return 0;
    }
    memcpy(solved, temp, sizeof(solved));

    start_timer();
    game_running = 1;

    printf("\nNew Sudoku generated (k=%d removed):\n", k);
    print_grid(board);
    printf("Type 'help' to see the commands.\n\n");
    return 1;
}

// ---- Commands ----
static void cmd_set(int r, int c, int v) {
    if (!moves_allowed()) return;
    if (!in_range(r,c) || v < 1 || v > 9) {
        printf("Invalid parameters. Use: set r c v  (r,c in 1..9, v in 1..9)\n");
        return;
    }
    if (is_given[r-1][c-1]) {
        printf("That cell is fixed (given by the puzzle). You cannot change it.\n");
        return;
    }
    if (!is_safe_move(board, r, c, v)) {
        printf("Invalid move (violates row/column/box rule).\n");
        return;
    }
    board[r-1][c-1] = v;
    print_grid(board);
}

static void cmd_clear(int r, int c) {
    if (!moves_allowed()) return;
    if (!in_range(r,c)) { printf("Use: clear r c (1..9)\n"); return; }
    if (is_given[r-1][c-1]) { printf("Fixed cell; you cannot clear it.\n"); return; }
    board[r-1][c-1] = 0;
    print_grid(board);
}

static void cmd_check(void) {
    if (!is_complete(board)) {
        printf("There are still empty cells. Keep going!\n");
        return;
    }
    if (equals(board, solved)) {
        int e = elapsed_seconds();
        int mm = e / 60, ss = e % 60;
        printf("Congratulations! Correct solution. Time: %02d:%02d\n", mm, ss);
    } else {
        printf("The board is full, but there are mistakes. You can use 'hint' or 'solve'.\n");
    }
}

static void cmd_hint(void) {
    if (!moves_allowed()) return;
    // Fill the first empty cell with the correct value from the solution
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i][j] == 0) {
                board[i][j] = solved[i][j];
                printf("Hint: cell (%d,%d) = %d\n", i+1, j+1, solved[i][j]);
                print_grid(board);
                return;
            }
        }
    }
    printf("No hint available (board is already complete).\n");
}

static void cmd_solve(void) {
    if (!moves_allowed()) return;
    memcpy(board, solved, sizeof(board));
    print_grid(board);
    printf("Solved with the precomputed solution.\n");
}

// parse "new" arguments: either <k> or difficulty name
static int parse_k_from_new(const char *args, int *out_k) {
    if (!args) return 0;
    // skip leading spaces
    while (*args && isspace((unsigned char)*args)) args++;
    if (!*args) return 0;

    // try number
    int kk; char extra;
    if (sscanf(args, "%d %c", &kk, &extra) >= 1) {
        if (kk < 0) kk = 0;
        if (kk > 81) kk = 81;
        *out_k = kk;
        return 1;
    }
    // try difficulty name
    int k = k_for_difficulty(args);
    if (k > 0) { *out_k = k; return 1; }
    return 0;
}

static void start_new_named(const char *name) {
    int k = k_for_difficulty(name);
    if (k < 0) {
        printf("Unknown difficulty. Use: new <easy|medium|hard|expert>\n");
        return;
    }
    while (!new_game(k)) { /* retry if needed */ }
}

// ---- Main ----
int main(void) {
    srand((unsigned)time(NULL));

    // Difficulty menu before the first game
    int k = choose_difficulty_menu();
    while (!new_game(k)) { /* retry if needed */ }

    char line[128];
    print_help();

    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        // strip newline
        line[strcspn(line, "\r\n")] = 0;

        // skip leading spaces
        char *p = line; while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;

        if (strncmp(p, "show", 4) == 0) {
            print_grid(board);
        } else if (strncmp(p, "help", 4) == 0) {
            print_help();
        } else if (strncmp(p, "time", 4) == 0) {
            show_time();
        } else if (strncmp(p, "check", 5) == 0) {
            cmd_check();
        } else if (strncmp(p, "hint", 4) == 0) {
            cmd_hint();
        } else if (strncmp(p, "solve", 5) == 0) {
            cmd_solve();
        } else if (strncmp(p, "pause", 5) == 0) {
            set_paused(1);
        } else if (strncmp(p, "resume", 6) == 0) {
            set_paused(0);
        } else if (strncmp(p, "new", 3) == 0) {
            // after "new", check for arguments
            char *arg = p + 3;
            while (*arg && isspace((unsigned char)*arg)) arg++;
            if (*arg == '\0') {
                // open difficulty menu
                int nk = choose_difficulty_menu();
                while (!new_game(nk)) { /* retry if needed */ }
            } else {
                int nk;
                if (parse_k_from_new(arg, &nk)) {
                    while (!new_game(nk)) { /* retry if needed */ }
                } else {
                    printf("Usage:\n");
                    printf("  new                       -> open difficulty menu\n");
                    printf("  new <easy|medium|hard|expert>\n");
                    printf("  new <k>                   -> k in 0..81\n");
                }
            }
        } else if (strncmp(p, "quit", 4) == 0 || strncmp(p, "exit", 4) == 0) {
            printf("Bye!\n");
            break;
        } else {
            printf("Unknown command. Type 'help'.\n");
        }
    }
    return 0;
}