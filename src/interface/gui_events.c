// SDL2 event handling (mouse/keyboard).

#include "gui.h"

// local helper: copy grid and set fixed flags from non-zeros
// grid_make_initial(): helper function.
static void grid_make_initial(const Grid* src, Grid* dst){
    *dst = *src;
    for(int r=0;r<9;r++) for(int c=0;c<9;c++) dst->fixed[r][c] = (dst->v[r][c] != 0);
}

#include <stdio.h>

static App* g_trace_app = NULL;
void push_trace(App* A, const char* s);
// gui_trace_cb(): helper function.
static void gui_trace_cb(const char* line){ if(!g_trace_app||!line) return; push_trace(g_trace_app, line);}

// which_button(): helper function.
static int which_button(int mx, int my){
    int x=UI_RIGHT_X, y=UI_RIGHT_Y;
    for(int i=0;i<12;i++){
        int yy = y + i*(UI_BTN_H+UI_BTN_G);
        if(mx>=x && mx<=x+UI_BTN_W && my>=yy && my<=yy+UI_BTN_H) return i;
    }
    return -1;
}


// push_trace(): helper function.
void push_trace(App* A, const char* s){
    if(!s) return;
    if(A->trace_count<128){
        snprintf(A->trace_lines[A->trace_count], sizeof(A->trace_lines[0]), "%s", s);
        A->trace_count++;
    }else{
        for(int i=1;i<128;i++) snprintf(A->trace_lines[i-1], sizeof(A->trace_lines[0]), "%s", A->trace_lines[i]);
        snprintf(A->trace_lines[127], sizeof(A->trace_lines[0]), "%s", s);
    }
}


#define MAX_STACK 128
static Grid undo_stack[MAX_STACK];
static int undo_top=0;
static Grid redo_stack[MAX_STACK];
static int redo_top=0;

// push_undo(): helper function.
static void push_undo(const Grid* g){
    if(undo_top<MAX_STACK) undo_stack[undo_top++]=*g;
    else { for(int i=1;i<MAX_STACK;i++) undo_stack[i-1]=undo_stack[i]; undo_stack[MAX_STACK-1]=*g; }
    redo_top=0;
}
// pop_undo(): helper function.
static int pop_undo(Grid* out){
    if(undo_top<=0) return 0;
    *out = undo_stack[--undo_top];
    return 1;
}
// push_redo(): helper function.
static void push_redo(const Grid* g){
    if(redo_top<MAX_STACK) redo_stack[redo_top++]=*g;
    else { for(int i=1;i<MAX_STACK;i++) redo_stack[i-1]=redo_stack[i]; redo_stack[MAX_STACK-1]=*g; }
}
// pop_redo(): helper function.
static int pop_redo(Grid* out){
    if(redo_top<=0) return 0;
    *out = redo_stack[--redo_top];
    return 1;
}

// cell_at(): helper function.
static int cell_at(int x,int y){
    int ox=40, oy=80, s=60;
    if(x<ox || y<oy) return -1;
    int c = (x-ox)/s;
    int r = (y-oy)/s;
    if(r<0||r>8||c<0||c>8) return -1;
    return r*9+c;
}

// open_menu(): helper function.
static void open_menu(App* A){ A->mode=GUI_MENU; A->menu_sel=2; }

// start_game(): helper function.
static void start_game(App* A, int k){
    generator_new_k(k, &A->grid);
    A->mode = GUI_GAME;
    A->cell = -1;
    A->start_ticks = SDL_GetTicks();
    A->show_modal = 0;
    A->solved_time_ms = 0;
    undo_top=redo_top=0;
    A->trace_count=0; A->trace_scroll=0; A->msg_buf[0]=0;
    g_trace_app=NULL; trace_set_callback(NULL);
}

// is_valid_grid(): helper function.
static int is_valid_grid(const Grid* g){
    for(int i=0;i<9;i++){
        int row[10]={0}, col[10]={0};
        for(int j=0;j<9;j++){
            int vr=g->v[i][j];
            int vc=g->v[j][i];
            if(vr<1||vr>9 || vc<1||vc>9) return 0;
            if(row[vr] || col[vc]) return 0;
            row[vr]=col[vc]=1;
        }
    }
    for(int br=0;br<3;br++) for(int bc=0;bc<3;bc++){
        int seen[10]={0};
        for(int i=0;i<3;i++) for(int j=0;j<3;j++){
            int v=g->v[br*3+i][bc*3+j];
            if(v<1||v>9 || seen[v]) return 0;
            seen[v]=1;
        }
    }
    return 1;
}

// gui_handle_event(): helper function.
void gui_handle_event(App* A, SDL_Event* e){
    if(e->type==SDL_MOUSEMOTION){
        int mx=e->motion.x, my=e->motion.y;
        int b = which_button(mx,my);
        A->hover_btn = b;
        return;
    }

    if(e->type==SDL_QUIT){ A->running=0; return; }

    if(A->mode==GUI_MENU){
        if(e->type==SDL_KEYDOWN){
            SDL_Keycode sym=e->key.keysym.sym;
            if(sym==SDLK_ESCAPE){ A->running=0; return; }
            if(sym==SDLK_UP){ if(A->menu_sel>0) A->menu_sel--; return; }
            if(sym==SDLK_DOWN){ if(A->menu_sel<3) A->menu_sel++; return; }
            if(sym==SDLK_RETURN || sym==SDLK_KP_ENTER){
                int k=40;
                if(A->menu_sel==0) k=40;
                else if(A->menu_sel==1) k=50;
                else if(A->menu_sel==2) k=60;
                else if(A->menu_sel==3) k=64;
                start_game(A,k);
                return;
            }
        }
        return;
    }

    // If a modal is shown, only allow Enter/Space to dismiss
    if(A->show_modal){
        if(e->type==SDL_KEYDOWN){
            SDL_Keycode sym=e->key.keysym.sym;
            if(sym==SDLK_RETURN || sym==SDLK_SPACE){ A->show_modal=0; return; }
        }
        return;
    }

    // GAME MODE
    if(e->type==SDL_MOUSEBUTTONDOWN){
        int mx=e->button.x, my=e->button.y;
        int b = which_button(mx,my);
        if(b>=0){
            // map buttons to commands
            switch(b){
                case 0: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_h; gui_handle_event(A,&ke); } break;
                case 1: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_c; gui_handle_event(A,&ke); } break;
                case 2: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_s; gui_handle_event(A,&ke); } break;
                case 3: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_b; gui_handle_event(A,&ke); } break;
                case 4: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_x; gui_handle_event(A,&ke); } break;
                case 5: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_n; gui_handle_event(A,&ke); } break;
                case 6: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_z; gui_handle_event(A,&ke); } break;
                case 7: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_y; gui_handle_event(A,&ke); } break;
                case 8: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_k; gui_handle_event(A,&ke); } break;
                case 9: { SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_l; gui_handle_event(A,&ke); } break;
                case 10:{ SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_m; gui_handle_event(A,&ke); } break;
                case 11:{ SDL_Event ke={.type=SDL_KEYDOWN}; ke.key.keysym.sym=SDLK_v; gui_handle_event(A,&ke); } break;
            }
            return;
        }
        // not a button: maybe clicked grid
        // trace header area
        if(mx>=UI_TRACE_X-10 && mx<=UI_TRACE_X-10+UI_TRACE_W+20 && my>=UI_TRACE_Y-36 && my<=UI_TRACE_Y-10){
            A->trace_collapsed = !A->trace_collapsed;
            return;
        }
        int k = cell_at(mx, my);
        if(k>=0) A->cell=k;
        return;
    }
    if(e->type==SDL_KEYDOWN){
        SDL_Keycode sym=e->key.keysym.sym;
        if(sym==SDLK_ESCAPE){ A->running=0; return; }
        if(sym==SDLK_m){ open_menu(A); return; }
        if(sym==SDLK_v){ A->cand_mode=(A->cand_mode+1)%3; const char* m=(A->cand_mode==0?"Candidats: off":(A->cand_mode==1?"Candidats: selection":"Candidats: tous")); gui_show_message(A,m,1000); return; }
        if(sym==SDLK_n){ open_menu(A); return; }

        // Hint behavior: only fill if selected cell is empty; otherwise do nothing
        if(sym==SDLK_h){
            if(A->cell>=0){
                int r=A->cell/9, c=A->cell%9;
                if(A->grid.v[r][c]==0){
                    int val=0;
                    if(solve_get_cell(&A->grid, r, c, &val) && val>=1 && val<=9){
                        push_undo(&A->grid);
                        A->grid.v[r][c]=val; { char buf[64]; snprintf(buf,sizeof(buf),"Indice: (%d,%d) = %d", r+1,c+1,val); push_trace(A, buf);}
                    }
                }
            }
            return;
        }

        if(sym==SDLK_s){
            // Solve from the initial puzzle (ignore current mistakes), using DLX
            Grid base; grid_make_initial(&A->grid, &base);
            solver_solve(&base); // DLX path
            A->grid = base;
            push_trace(A, "SOLVE (DLX): depuis la grille initiale");
            return;
        }
        if(sym==SDLK_l){ load_game("data/save.json", &A->grid); undo_top=redo_top=0; return; }
        if(sym==SDLK_k){ save_game("data/save.json", &A->grid); return; }
        if(sym==SDLK_b){
            Grid tmp = A->grid;
            extern void trace_enable(int on);
            extern void trace_set_callback(void (*cb)(const char*));
            extern int solver_solve(Grid* g);
            A->trace_count = 0;
            g_trace_app = A;
            trace_set_callback(gui_trace_cb);
            trace_enable(1);
            solver_solve(&tmp);
            trace_enable(0);
            trace_set_callback(NULL);
            g_trace_app = NULL;
            return;
        }
        if(sym==SDLK_x){
            A->trace_count=0; return;
        }
        if(sym==SDLK_z){ Grid g; if(pop_undo(&g)){ push_redo(&A->grid); A->grid=g; } return; }
        if(sym==SDLK_y){ Grid g; if(pop_redo(&g)){ push_undo(&A->grid); A->grid=g; } return; }

        if(sym==SDLK_c){
            if(grid_is_complete(&A->grid) && is_valid_grid(&A->grid)){
                A->solved_time_ms = (int)(SDL_GetTicks() - A->start_ticks);
                A->show_modal=1;
            }else{
                // scan and report conflicts
                for(int r=0;r<9;r++) for(int c=0;c<9;c++){
                    int v=A->grid.v[r][c];
                    if(v==0) continue;
                    int conflict=0;
                    for(int i=0;i<9;i++){
                        if(i!=c && A->grid.v[r][i]==v) { conflict=1; push_trace(A,"Conflit ligne"); break; }
                        if(i!=r && A->grid.v[i][c]==v) { conflict=1; push_trace(A,"Conflit colonne"); break; }
                    }
                    int br=(r/3)*3, bc=(c/3)*3;
                    for(int i=0;i<3 && !conflict;i++) for(int j=0;j<3;j++){
                        int rr=br+i, cc=bc+j;
                        if(rr==r && cc==c) continue;
                        if(A->grid.v[rr][cc]==v){ conflict=1; push_trace(A,"Conflit bloc"); break; }
                    }
                }
            }
            return;
        }

        if(sym==SDLK_DELETE || sym==SDLK_BACKSPACE || sym==SDLK_KP_PERIOD){
            if(A->cell>=0){
                int r=A->cell/9, c=A->cell%9;
                if(!A->grid.fixed[r][c]){
                    push_undo(&A->grid);
                    A->grid.v[r][c]=0;
                }
            }
            return;
        }
    

        // Arrow navigation
        if(sym==SDLK_LEFT || sym==SDLK_RIGHT || sym==SDLK_UP || sym==SDLK_DOWN){
            if(A->cell<0) A->cell=0;
            int r=A->cell/9, c=A->cell%9;
            if(sym==SDLK_LEFT)  c = (c+8)%9;
            if(sym==SDLK_RIGHT) c = (c+1)%9;
            if(sym==SDLK_UP)    r = (r+8)%9;
            if(sym==SDLK_DOWN)  r = (r+1)%9;
            A->cell = r*9+c;
            return;
        }

        // digits (top row + numpad)
        int v=-1;
        if(sym>=SDLK_0 && sym<=SDLK_9) v = sym - SDLK_0;
        else if(sym>=SDLK_KP_0 && sym<=SDLK_KP_9) v = sym - SDLK_KP_0;
        if(v>=0 && A->cell>=0){
            int r=A->cell/9, c=A->cell%9;
            if(!A->grid.fixed[r][c]){
                push_undo(&A->grid);
                if(v==0){ A->grid.v[r][c]=0; }
                else { A->grid.v[r][c]=v; }
            }
            if(grid_is_complete(&A->grid) && is_valid_grid(&A->grid)){
                A->solved_time_ms = (int)(SDL_GetTicks() - A->start_ticks);
                A->show_modal=1;
            }
            return;
        }
    }
}