// Small helper module.

#include "interface.h"
#include <assert.h>
#include <stdio.h>

// main(): helper function.
int main(void){
    Grid g; generator_new_k(50, &g);
    assert(grid_count_empty(&g)>=40); // rough check
    Grid g2=g; assert(count_solutions(&g2,2)==1);
    Grid s=g; assert(solver_solve(&s)==1);
    assert(grid_is_complete(&s));
    printf("OK\n");
    return 0;
}