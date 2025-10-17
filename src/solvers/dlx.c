// Dancing Links (DLX) exact cover solver for Sudoku.

#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "solver.h"

#define N 9
#define COLS 324             // 4 * 9 * 9
#define ROWS 729             // 9 * 9 * 9
#define MAX_NODES (ROWS*4 + COLS + 5)

// Row encoding helpers for (r,c,n)
// row_id(): helper function.
static inline int row_id(int r, int c, int n) { return (r*81 + c*9 + n); } // 0..728

// Column indexes for the 4 constraint families
// col_cell(): helper function.
static inline int col_cell(int r, int c)   { return r*9 + c; }                          // 0..80
// col_rownum(): helper function.
static inline int col_rownum(int r, int n) { return 81 + r*9 + n; }                     // 81..161
// col_colnum(): helper function.
static inline int col_colnum(int c, int n) { return 162 + c*9 + n; }                    // 162..242
// col_boxnum(): helper function.
static inline int col_boxnum(int b, int n) { return 243 + b*9 + n; }                    // 243..323
// box_index(): helper function.
static inline int box_index(int r, int c)  { return (r/3)*3 + (c/3); }

typedef struct Node {
    struct Node *L, *R, *U, *D;
    struct Node *C;    // column header
    int S;             // size (only for column headers)
    int row;           // which (r,c,n) row this node belongs to (for solution mapping)
    int col;           // column index (0..323)
} Node;

static Node node_pool[MAX_NODES];
static int   pool_top;

static Node header;           // head of the header list
static Node *col_hdr[COLS];   // column headers
static Node *solution[ROWS];  // holds chosen rows during search
static int   sol_depth;

// ---------- DLX core ops ----------

// dlx_init_headers(): helper function.
static void dlx_init_headers(void) {
    header.L = header.R = &header;
    header.U = header.D = &header;
    header.C = &header;
    header.S = -1;

    for (int c = 0; c < COLS; c++) {
        Node *h = &node_pool[pool_top++];
        h->L = header.L; h->R = &header;
        header.L->R = h; header.L = h;

        h->U = h->D = h;     // empty column initially
        h->C = h;
        h->S = 0;
        h->col = c;
        col_hdr[c] = h;
    }
}

// link_node_vertical(): helper function.
static void link_node_vertical(Node *col, Node *n) {
    n->D = col;
    n->U = col->U;
    col->U->D = n;
    col->U = n;
    n->C = col;
    col->S++;
}

// link_node_horizontal(): helper function.
static void link_node_horizontal(Node *row_head, Node *n) {
    if (row_head == NULL) {
        n->L = n->R = n;
    } else {
        n->L = row_head->L;
        n->R = row_head;
        row_head->L->R = n;
        row_head->L = n;
    }
}

// cover(): helper function.
static void cover(Node *c) {
    c->R->L = c->L;
    c->L->R = c->R;
    for (Node *i = c->D; i != c; i = i->D) {
        for (Node *j = i->R; j != i; j = j->R) {
            j->D->U = j->U;
            j->U->D = j->D;
            j->C->S--;
        }
    }
}

// uncover(): helper function.
static void uncover(Node *c) {
    for (Node *i = c->U; i != c; i = i->U) {
        for (Node *j = i->L; j != i; j = j->L) {
            j->C->S++;
            j->D->U = j;
            j->U->D = j;
        }
    }
    c->R->L = c;
    c->L->R = c;
}

// choose_column_minS(): helper function.
static Node* choose_column_minS(void) {
    Node *best = header.R;
    for (Node *c = best->R; c != &header; c = c->R)
        if (c->S < best->S) best = c;
    return best;
}

// ---------- Build the DLX matrix rows for Sudoku candidates ----------
// For each candidate (r,c,n) that is *allowed* by the current grid,
// we add 4 nodes linking to the 4 constraint columns.
// add_candidate_row(): helper function.
static void add_candidate_row(int r, int c, int n) {
    // map constraints:
    int b = box_index(r,c);
    int cols[4] = {
        col_cell(r,c),
        col_rownum(r,n),
        col_colnum(c,n),
        col_boxnum(b,n)
    };

    Node *row_head = NULL;
    for (int k = 0; k < 4; k++) {
        Node *col = col_hdr[cols[k]];
        Node *x   = &node_pool[pool_top++];
        x->row = row_id(r,c,n);
        x->col = cols[k];
        link_node_vertical(col, x);
        link_node_horizontal(row_head, x);
        if (!row_head) row_head = x;
    }
}

// Build rows only for currently legal candidates
// legal_in_grid(): helper function.
static int legal_in_grid(int grid[N][N], int r, int c, int n) {
    if (grid[r][c] != 0 && grid[r][c] != n+1) return 0;
    int val = n+1;
    for (int j = 0; j < 9; j++) if (grid[r][j] == val && j != c) return 0;
    for (int i = 0; i < 9; i++) if (grid[i][c] == val && i != r) return 0;
    int r0 = (r/3)*3, c0 = (c/3)*3;
    for (int i = r0; i < r0+3; i++)
        for (int j = c0; j < c0+3; j++)
            if (grid[i][j] == val && !(i==r && j==c)) return 0;
    return 1;
}

// build_structure_from_grid(): helper function.
static void build_structure_from_grid(int grid[N][N]) {
    pool_top = 0;
    sol_depth = 0;
    dlx_init_headers();

    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            for (int n = 0; n < 9; n++) {
                if (legal_in_grid(grid, r, c, n)) {
                    add_candidate_row(r, c, n);
                }
            }
        }
    }
}

// “Cover” the givens to pre-restrict the search.
// apply_givens(): helper function.
static int apply_givens(int grid[N][N]) {
    for (int r = 0; r < 9; r++) {
        for (int c = 0; c < 9; c++) {
            int v = grid[r][c];
            if (v == 0) continue;

            // Find any row with row_id matching (r,c,v-1)
            int want = row_id(r,c,v-1);
            Node *found = NULL;

            // Scan the column for the "cell (r,c)" constraint to find that row quickly:
            int b = box_index(r,c);
            int cols[4] = {
                col_cell(r,c),
                col_rownum(r,v-1),
                col_colnum(c,v-1),
                col_boxnum(b,v-1)
            };

            // We can search in any of these columns; pick the cell constraint column
            Node *col = col_hdr[cols[0]];
            for (Node *n = col->D; n != col; n = n->D) {
                if (n->row == want) { found = n; break; }
            }
            if (!found) return 0; // contradiction (illegal given)

            // Cover the columns for that chosen row
            cover(found->C);
            for (Node *j = found->R; j != found; j = j->R) cover(j->C);

            solution[sol_depth++] = found;
        }
    }
    return 1;
}

// ---------- Search ----------
// search(): helper function.
static int search(void) {
    if (header.R == &header) return 1; // solved
    Node *c = choose_column_minS();
    if (c->S <= 0) return 0;

    cover(c);
    for (Node *r = c->D; r != c; r = r->D) {
        solution[sol_depth++] = r;

        for (Node *j = r->R; j != r; j = j->R) cover(j->C);

        if (search()) return 1;

        for (Node *j = r->L; j != r; j = j->L) uncover(j->C);
        solution[sol_depth-1] = NULL;
        sol_depth--;
    }
    uncover(c);
    return 0;
}

// Map chosen rows back to the grid
// write_solution(): helper function.
static void write_solution(int grid[N][N]) {
    for (int k = 0; k < sol_depth; k++) {
        int rid = solution[k]->row; // (r,c,n) encoded
        int r = rid / 81; rid %= 81;
        int c = rid / 9;  rid %= 9;
        int n = rid;      // 0..8
        grid[r][c] = n + 1;
    }
}

// ---------- Public API ----------

// solve_dlx(): helper function.
int solve_dlx(int grid[N][N]) {
    // Build DLX structure respecting current givens
    build_structure_from_grid(grid);
    if (!apply_givens(grid)) return 0;

    int ok = search();
    if (ok) {
        // Fill back
        // first clear (in case there are zeros), then write solution (givens are included)
        int out[9][9] = {0};
        write_solution(out);
        // copy out -> grid
        for (int i = 0; i < 9; i++)
            for (int j = 0; j < 9; j++)
                grid[i][j] = out[i][j];
        return 1;
    }
    return 0;
}


// Count solutions using DLX by exploring all exact covers (with early cap at e.g. 2)


/* Simple DLX-based solution counter: returns 1 if solvable, 0 otherwise.
   NOTE: Proper multi-solution counting would require enumerating multiple solutions in DLX.
*/


/* DLX-based solution counter (approx): tries to detect >1 solution by
   finding one DLX solution, then attempting to force a different value
   in one originally-empty cell and solving again.
   Returns: 0 if unsolvable, 1 if exactly one found (approx), 2 if a second
   solution is detected (early stop). */
// count_solutions_dlx(): helper function.
int count_solutions_dlx(const Grid* g, int limit){
    if(!g || limit<=0) return 0;
    int a[9][9];
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) a[r][c]=g->v[r][c];
    int solved = solve_dlx(a);
    if(!solved) return 0;
    if(limit==1) return 1;
    // Try to find a different solution by flipping one empty cell to a different value
    // from the first solution.
    for(int r=0;r<9;r++){
        for(int c=0;c<9;c++){
            if(g->v[r][c]!=0) continue; // only cells that were empty
            int v=a[r][c];
            for(int alt=1; alt<=9; alt++){
                if(alt==v) continue;
                int b[9][9];
                for(int i=0;i<9;i++) for(int j=0;j<9;j++) b[i][j]=g->v[i][j];
                b[r][c]=alt;
                if(solve_dlx(b)){
                    return 2; // found another solution
                }
            }
        }
    }
    return 1;
}