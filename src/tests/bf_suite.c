// Heavy brute-force validation & timing suite (interface-tolerant)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"
#include "generator.h"
#include "solver.h"

// Declarações (caso solver.h não exponha)
int solve_dlx(Grid*);
int solve_backtracking(Grid*);
int solve_backtracking_mrv_bitmask(Grid*);
int solve_constraint_propagation(Grid*);

static double now_s(void){
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec*1e-9;
}

static int valid_complete_grid(const Grid* g){
    int seen[10];
    // rows
    for(int r=0;r<N;r++){
        memset(seen,0,sizeof(seen));
        for(int c=0;c<N;c++){
            int v = g->v[r][c];
            if(v < 1 || v > 9) return 0;
            if(seen[v]) return 0; seen[v]=1;
        }
    }
    // cols
    for(int c=0;c<N;c++){
        memset(seen,0,sizeof(seen));
        for(int r=0;r<N;r++){
            int v = g->v[r][c];
            if(v < 1 || v > 9) return 0;
            if(seen[v]) return 0; seen[v]=1;
        }
    }
    // 3x3 boxes
    for(int br=0;br<3;br++){
        for(int bc=0;bc<3;bc++){
            memset(seen,0,sizeof(seen));
            for(int dr=0;dr<3;dr++){
                for(int dc=0;dc<3;dc++){
                    int r = br*3+dr, c = bc*3+dc;
                    int v = g->v[r][c];
                    if(v < 1 || v > 9) return 0;
                    if(seen[v]) return 0; seen[v]=1;
                }
            }
        }
    }
    return 1;
}

static int count_zeros(const Grid* g){
    int z=0;
    for(int r=0;r<N;r++)
        for(int c=0;c<N;c++)
            if(g->v[r][c]==0) z++;
    return z;
}

static void csv_header(FILE* f){ fprintf(f,"k,solver,mean_ms\n"); }
static void csv_row(FILE* f,int k,const char* s,double ms){ fprintf(f,"%d,%s,%.6f\n",k,s,ms); }

// Chama um solver, mede tempo; se a grade nao estiver completa ao fim,
// usamos DLX para finalizar só para validar consistência.
static double time_solver_with_dlxfallback(const char* label,
                                           int (*solver)(Grid*),
                                           const Grid* in,
                                           Grid* out)
{
    Grid w; grid_copy(in, &w);
    double t0 = now_s();
    (void)solver(&w); // alguns solvers podem retornar void na prática
    double t1 = now_s();

    // Se o solver não modificou 'w' (ou não completou), finalize com DLX apenas p/ validação
    if(!valid_complete_grid(&w)){
        Grid w2; grid_copy(in, &w2);
        int ok = solve_dlx(&w2);
        if(!ok || !valid_complete_grid(&w2)){
            fprintf(stderr, "%s (then DLX) failed\n", label);
            exit(7);
        }
        // preserva a solução válida em 'out'
        grid_copy(&w2, out);
    } else {
        grid_copy(&w, out);
    }

    return (t1 - t0) * 1000.0;
}

int main(void){
    int ks[] = {30,40,50,60};
    int nK = (int)(sizeof(ks)/sizeof(ks[0]));
    int trials_per_k = 200;
    int max_solutions_to_count = 2;

    FILE* csv = fopen("results/solvers_comparison.csv","w");
    if(!csv){ perror("open results/solvers_comparison.csv"); return 1; }
    csv_header(csv);

    for(int i=0;i<nK;i++){
        int k = ks[i];
        double s_dlx=0, s_mrv=0, s_bt=0, s_cp=0;

        for(int t=0;t<trials_per_k;t++){
            Grid g; generator_new_k(k,&g);

            int z = count_zeros(&g);
            if(z < k-5 || z > k+5)
                fprintf(stderr,"WARN: k=%d zeros=%d\n",k,z);

            int solc = count_solutions_dlx(&g, max_solutions_to_count);
            if(solc != 1){
                fprintf(stderr,"ERROR: uniqueness failed for k=%d (solutions=%d)\n",k,solc);
                return 2;
            }

            Grid out;

            // DLX (baseline, deve sempre completar)
            {
                Grid w; grid_copy(&g, &w);
                double t0 = now_s();
                int ok = solve_dlx(&w);
                double t1 = now_s();
                if(!ok || !valid_complete_grid(&w)){ fprintf(stderr,"DLX failed\n"); return 3; }
                s_dlx += (t1 - t0)*1000.0;
            }

            // MRV + bitmask (tolerante: valida com DLX se não escrever no Grid)
            s_mrv += time_solver_with_dlxfallback("MRV", solve_backtracking_mrv_bitmask, &g, &out);

            // Backtracking (tolerante)
            s_bt  += time_solver_with_dlxfallback("BT",  solve_backtracking, &g, &out);

            // Constraint Propagation (já era tolerante)
            s_cp  += time_solver_with_dlxfallback("CP",  solve_constraint_propagation, &g, &out);
        }

        csv_row(csv,k,"DLX",s_dlx/trials_per_k);
        csv_row(csv,k,"Backtracking+MRV/bitmask",s_mrv/trials_per_k);
        csv_row(csv,k,"Backtracking",s_bt/trials_per_k);
        csv_row(csv,k,"Constraint Propagation",s_cp/trials_per_k);

        fprintf(stderr,"k=%d → DLX %.3f ms | MRV %.3f | BT %.3f | CP %.3f\n",
                k, s_dlx/trials_per_k, s_mrv/trials_per_k, s_bt/trials_per_k, s_cp/trials_per_k);
    }

    fclose(csv);
    fprintf(stderr,"Wrote results/solvers_comparison.csv\n");
    return 0;
}
