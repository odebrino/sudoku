// SDL2 GUI setup/teardown helpers.

#include "gui.h"

// gui_init(): helper function.
int gui_init(App* A){
    if(SDL_Init(SDL_INIT_VIDEO)!=0) return -1;
    if(TTF_Init()!=0) return -2;
    A->win = SDL_CreateWindow("Sudoku (GUI) • 🎲 Theme Violet", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 860, 0);
    A->ren = SDL_CreateRenderer(A->win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    A->font = TTF_OpenFontIndex("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 20, 0);
    if(!A->font) A->font = TTF_OpenFont("DejaVuSans.ttf", 20);
    A->running=1; A->cell=-1; A->show_conflicts=1;
    A->mode = GUI_MENU; A->menu_sel=2; A->custom_k=50;
    A->start_ticks = SDL_GetTicks();
    A->show_modal=0; A->solved_time_ms=0; A->cand_mode=1; A->hover_btn=-1; A->trace_collapsed=0;
    grid_clear(&A->grid);
    return 0;
}

// gui_shutdown(): helper function.
void gui_shutdown(App* A){
    if(A->font) TTF_CloseFont(A->font);
    if(A->ren) SDL_DestroyRenderer(A->ren);
    if(A->win) SDL_DestroyWindow(A->win);
    TTF_Quit();
    SDL_Quit();
}


// gui_show_message(): helper function.
void gui_show_message(App* A, const char* msg, int ms){ (void)ms;
    // reuse window title for quick message in console and keep a timer via show_modal with 0-time?
    // We'll store the message in the window title briefly.
    char title[256]; snprintf(title,sizeof(title),"Sudoku (GUI) • %s", msg);
    SDL_SetWindowTitle(A->win, title);
    // We won't block; caller may set again later. Title persists.
}

// run_gui(): helper function.
int run_gui(void){

    App A;
    if(gui_init(&A)!=0) return 1;
    SDL_Event e;
    while(A.running){
        while(SDL_PollEvent(&e)){
            gui_handle_event(&A, &e);
        }
        gui_draw(&A);
        SDL_Delay(16);
    }
    gui_shutdown(&A);
    return 0;
}