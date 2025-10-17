// GUI public API and shared types.

#ifndef GUI_H
#define GUI_H
#include <SDL.h>
#include <SDL_ttf.h>
#include "interface.h"

typedef enum { GUI_MENU=0, GUI_GAME=1 } GuiMode;

typedef struct {
    // trace & messages
    char trace_lines[128][96];
    int trace_count;
    int trace_scroll;
    char msg_buf[128];

    SDL_Window* win;
    SDL_Renderer* ren;
    TTF_Font* font;
    int cell;              // selected cell 0..80, -1 = none
    Grid grid;
    int running;
    int show_conflicts;
    GuiMode mode;
    int menu_sel;          // 0 Facile, 1 Moyen, 2 Difficile, 3 Expert
    int custom_k;          // not used in this theme (kept for compat)
    Uint32 start_ticks;    // ms since SDL init when game started
    int show_modal;        // 1 if "Bravo!" modal visible
    int cand_mode;         // 0: off, 1: selected only, 2: all
    int hover_btn;         // -1 or index of hovered button
    int trace_collapsed;   // 0 visible, 1 collapsed
    int solved_time_ms;    // total time when solved
    
 } App;

// UI layout constants (right column buttons)
#define UI_RIGHT_X 760
#define UI_RIGHT_Y 120
#define UI_BTN_W   360
#define UI_BTN_H   42
#define UI_BTN_G   12
#define UI_TRACE_X 760
#define UI_TRACE_Y 640
#define UI_TRACE_W 360
#define UI_TRACE_H 180

// UI helpers
void gui_show_message(App* A, const char* msg, int ms);


int gui_init(App* A);
void gui_shutdown(App* A);
void gui_draw(App* A);
void gui_handle_event(App* A, SDL_Event* e);
void gui_new_k(App* A, int k);
void gui_hint(App* A);
void gui_solve(App* A);

#endif