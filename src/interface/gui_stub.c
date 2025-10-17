// Stub printed when GUI binary is not linked.

#include <stdio.h>
// run_gui(): helper function.
int run_gui(void){
    fprintf(stderr, "GUI is not linked in this binary. Build and run with `make gui && ./build/sudoku_gui`.\n");
    return 1;
}