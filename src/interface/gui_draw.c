// SDL2 drawing code for grid and UI widgets.

#include "gui.h"
#include <string.h>
#include <stdio.h>

// === Colors ===
static SDL_Color VIOLET       = {92, 52, 144, 255};
static SDL_Color VIOLET_LIGHT = {210, 190, 230, 255};
static SDL_Color BG           = {239, 233, 246, 255};
static SDL_Color TEXT         = {60, 40, 90, 255};
static SDL_Color TEXT_HL      = {120, 60, 180, 255};
static SDL_Color GRID         = {80, 40, 130, 255};
static SDL_Color CELL_HL      = {220, 200, 255, 255};

// === Small helpers ===
// clear_bg(): helper function.
static void clear_bg(SDL_Renderer* ren, SDL_Color c){
    SDL_SetRenderDrawColor(ren, c.r,c.g,c.b,c.a);
    SDL_RenderClear(ren);
}

// draw_rect(): helper function.
static void draw_rect(SDL_Renderer* ren, int x,int y,int w,int h, SDL_Color c, int fill){
    SDL_SetRenderDrawColor(ren, c.r,c.g,c.b,c.a);
    SDL_Rect r={x,y,w,h};
    if(fill) SDL_RenderFillRect(ren,&r);
    else SDL_RenderDrawRect(ren,&r);
}

// line(): helper function.
static void line(SDL_Renderer* ren, int x1,int y1,int x2,int y2, SDL_Color c){
    SDL_SetRenderDrawColor(ren, c.r,c.g,c.b,c.a);
    SDL_RenderDrawLine(ren,x1,y1,x2,y2);
}

// blit_text_sz(): helper function.
static void blit_text_sz(App* A, const char* txt, int x, int y, SDL_Color c, int pts){
    TTF_Font* f = A->font;
    TTF_Font* alt = NULL;
    if(pts>0){
        alt = TTF_OpenFontIndex("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", pts, 0);
        if(!alt) alt = TTF_OpenFont("DejaVuSans.ttf", pts);
        if(alt) f = alt;
    }
    SDL_Surface* surf = TTF_RenderUTF8_Blended(f, txt, c);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(A->ren, surf);
    SDL_Rect dst={x,y,0,0};
    SDL_QueryTexture(tex,NULL,NULL,&dst.w,&dst.h);
    SDL_RenderCopy(A->ren, tex, NULL, &dst);
    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
    if(alt) TTF_CloseFont(alt);
}

// blit_text(): helper function.
static void blit_text(App* A, const char* txt, int x, int y, SDL_Color c, int big){
    blit_text_sz(A, txt, x, y, c, big?36:20);
}

// forward decl
static void draw_candidates(App* A, int ox, int oy, int s);

// === UI pieces ===
// format_time(): helper function.
static void format_time(int ms, char* out, size_t n){
    int s = ms/1000; int m=s/60; s%=60;
    snprintf(out, n, "Temps: %02d:%02d", m, s);
}

// draw_topbar(): helper function.
static void draw_topbar(App* A){
    draw_rect(A->ren, 12, 12, 1170, 32, VIOLET_LIGHT, 1);
    char buf[64];
    Uint32 now = SDL_GetTicks();
    int ms = (A->show_modal?A->solved_time_ms:(int)(now - A->start_ticks));
    format_time(ms, buf, sizeof(buf));
    blit_text(A, buf, 20, 18, TEXT, 0);
}

// draw_commands(): helper function.
static void draw_commands(App* A){
    // Right column title
    blit_text(A, "Commandes", UI_RIGHT_X, UI_RIGHT_Y-40, VIOLET, 1);

    int x = UI_RIGHT_X, y = UI_RIGHT_Y;
    SDL_Color btnBd = VIOLET;

    const char* labels[12];
    labels[0]  = "H • Indice (case)";
    labels[1]  = "C • Verifier";
    labels[2]  = "S • Resoudre";
    labels[3]  = "B • Backtracking (trace)";
    labels[4]  = "X • Effacer trace";
    labels[5]  = "N • Nouveau / Menu";
    labels[6]  = "Z • Annuler";
    labels[7]  = "Y • Refaire";
    labels[8]  = "K • Sauver";
    labels[9]  = "L • Charger";
    labels[10] = "M • Menu";
    static char cand_label[64];
    const char* mode = (A->cand_mode==0?"Off":(A->cand_mode==1?"Selection":"Tous"));
    snprintf(cand_label, sizeof(cand_label), "V • Candidats: %s", mode);
    labels[11] = cand_label;

    for(int i=0;i<12;i++){
        int yy = y + i*(UI_BTN_H+UI_BTN_G);
        SDL_Color bg = (i==A->hover_btn)? (SDL_Color){245,238,255,255} : (SDL_Color){230,220,245,255};
        draw_rect(A->ren, x, yy, UI_BTN_W, UI_BTN_H, bg, 1);
        draw_rect(A->ren, x, yy, UI_BTN_W, UI_BTN_H, btnBd, 0);
        blit_text(A, labels[i], x+12, yy+10, TEXT, 0);
    }

    // Trace panel
    // header acts as toggle: click in its area to collapse/expand (handled in events)
    draw_rect(A->ren, UI_TRACE_X-10, UI_TRACE_Y-36, UI_TRACE_W+20, 26, VIOLET_LIGHT, 1);
    draw_rect(A->ren, UI_TRACE_X-10, UI_TRACE_Y-36, UI_TRACE_W+20, 26, VIOLET, 0);
    blit_text(A, A->trace_collapsed? "Resolution (trace) — [afficher]" : "Resolution (trace) — [masquer]", UI_TRACE_X, UI_TRACE_Y-32, VIOLET, 0);
    if(!A->trace_collapsed){
        draw_rect(A->ren, UI_TRACE_X-10, UI_TRACE_Y-10, UI_TRACE_W+20, UI_TRACE_H+20, VIOLET_LIGHT, 1);
        draw_rect(A->ren, UI_TRACE_X-10, UI_TRACE_Y-10, UI_TRACE_W+20, UI_TRACE_H+20, VIOLET, 0);
        int lines = (A->trace_count<8)?A->trace_count:8;
        int start = (A->trace_count>8)?A->trace_count-8:0;
        for(int i=0;i<lines;i++){
            blit_text(A, A->trace_lines[start+i], UI_TRACE_X, UI_TRACE_Y + i*22, TEXT, 0);
        }
    } 
}

// draw_menu(): helper function.
static void draw_menu(App* A){
    clear_bg(A->ren, BG);
    blit_text(A, "Choisissez la difficulte", 240, 140, VIOLET, 1);
    const char* items[]={"Facile","Moyen","Difficile","Expert"};
    for(int i=0;i<4;i++){
        SDL_Color col = (i==A->menu_sel)?TEXT_HL:TEXT;
        blit_text(A, items[i], 420, 220 + i*48, col, 0);
    }
    blit_text(A, "Utilisez les fleches + Entrer  |  V: candidats", 240, 780, TEXT_HL, 0);
}

// draw_grid_and_board(): helper function.
static void draw_grid_and_board(App* A){
    clear_bg(A->ren, BG);
    draw_topbar(A);
    draw_rect(A->ren, 20, 60, 720, 12, VIOLET_LIGHT, 1);

    // grid geometry
    int ox=60, oy=110, s=64;

    // selection highlight
    if(A->cell>=0){
        int r=A->cell/9, c=A->cell%9;
        // row/col
        draw_rect(A->ren, ox, oy+r*s, s*9, s, CELL_HL, 1);
        draw_rect(A->ren, ox+c*s, oy, s, s*9, CELL_HL, 1);
        // 3x3 box
        int br=(r/3)*3, bc=(c/3)*3;
        draw_rect(A->ren, ox+bc*s, oy+br*s, s*3, s*3,(SDL_Color){235, 220, 255, 255},1);
    }

    // grid frame and lines
    draw_rect(A->ren, ox-8, oy-8, s*9+16, s*9+16, VIOLET, 0);
    for(int i=0;i<=9;i++){
        int thick = (i%3==0)?2:1;
        for(int t=0;t<thick;t++){
            line(A->ren, ox, oy+i*s+t, ox+9*s, oy+i*s+t, GRID);
            line(A->ren, ox+i*s+t, oy, ox+i*s+t, oy+9*s, GRID);
        }
    }

    // numbers
    for(int r=0;r<9;r++){
        for(int c=0;c<9;c++){
            int v=A->grid.v[r][c];
            if(v){
                // conflict check
                int conflict=0;
                for(int i=0;i<9;i++){
                    if(i!=c && A->grid.v[r][i]==v) { conflict=1; break; }
                    if(i!=r && A->grid.v[i][c]==v) { conflict=1; break; }
                }
                int br=(r/3)*3, bc=(c/3)*3;
                for(int i=0;i<3 && !conflict;i++) for(int j=0;j<3;j++){
                    int rr=br+i, cc=bc+j;
                    if(rr==r && cc==c) continue;
                    if(A->grid.v[rr][cc]==v){ conflict=1; break; }
                }
                char buf[2]={ (char)('0'+v), 0 };
                SDL_Color col = conflict ? (SDL_Color){200,0,0,255} : (A->grid.fixed[r][c]? VIOLET : TEXT);
                blit_text_sz(A, buf, ox+c*s + s/2 - 12, oy+r*s + s/2 - 18, col, 32);
            }
        }
    }

    // candidates
    draw_candidates(A, ox, oy, s);

    // right column
    draw_commands(A);

    // solved modal
    if(A->show_modal){
        SDL_Color overlay={0,0,0,128};
        draw_rect(A->ren, 0,0,1200,860, overlay, 1);
        draw_rect(A->ren, 260, 260, 620, 200, BG, 1);
        draw_rect(A->ren, 260, 260, 620, 200, VIOLET, 0);
        blit_text(A, "Bravo !", 490, 280,(SDL_Color){0,180,0,255},1);
        char t[64]; char tt[128]; 
        format_time(A->solved_time_ms, t, sizeof(t));
        snprintf(tt,sizeof(tt), "Temps total: %s", t+7);
        blit_text(A, tt, 460, 340, TEXT, 0);
        blit_text(A, "Appuyez sur Entree / Espace pour continuer", 320, 400, TEXT_HL, 0);
    }
}

// draw_candidates(): helper function.
static void draw_candidates(App* A, int ox, int oy, int s){
    if(A->cand_mode==0) return;
    SDL_Color candCol = {120,120,160,255};
    int r0=-1, c0=-1;
    if(A->cand_mode==1 && A->cell>=0){ r0=A->cell/9; c0=A->cell%9; }
    for(int r=0;r<9;r++){
        for(int c=0;c<9;c++){
            if(A->grid.v[r][c]==0){
                if(A->cand_mode==1 && !(r==r0 && c==c0)) continue;
                int x = ox + c*s;
                int y = oy + r*s;
                for(int v=1; v<=9; v++){
                    if(grid_is_safe_cell(&A->grid, r, c, v)){
                        char t[2]={(char)('0'+v),0};
                        int cx = x + ((v-1)%3)*(s/3) + 6;
                        int cy = y + ((v-1)/3)*(s/3) + 2;
                        blit_text_sz(A, t, cx, cy, candCol, 14);
                    }
                }
            }
        }
    }
}

// gui_draw(): helper function.
void gui_draw(App* A){
    if(A->mode==GUI_MENU) draw_menu(A);
    else draw_grid_and_board(A);
    SDL_RenderPresent(A->ren);
}