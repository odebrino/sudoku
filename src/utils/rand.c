// Tiny PRNG utilities for the generator.

#include <stdlib.h>
#include <time.h>

static int seeded=0;
// ensure_seed(): helper function.
static void ensure_seed(void){ if(!seeded){ srand((unsigned)time(NULL)); seeded=1; } }

// rnd_int(): helper function.
int rnd_int(int lo, int hi){
    ensure_seed();
    if(hi<=lo) return lo;
    return lo + rand() % (hi-lo+1);
}

// rnd_shuffle_int(): helper function.
void rnd_shuffle_int(int* a, int n){
    ensure_seed();
    for(int i=n-1;i>0;i--){
        int j = rand() % (i+1);
        int t=a[i]; a[i]=a[j]; a[j]=t;
    }
}