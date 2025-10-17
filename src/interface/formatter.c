// CLI text output helpers.

#include "interface.h"
#include <stdio.h>

// print_banner(): helper function.
void print_banner(void){
    printf("Sudoku CLI — commandes: nouveau <k> | afficher | placer l c v | indice | resoudre | trace on|off | sauvegarder [chemin] | charger [chemin] | aide | quitter\n");
}

// print_help(): helper function.
void print_help(void){
    printf("Commandes:\n");
    printf("  nouveau <k>   - nouveau puzzle avec k cases vides (0..60)\n");
    printf("  afficher      - afficher la grille\n");
    printf("  placer r c v  - placer valeur v a la position (r,c) (0 efface)\n");
    printf("  indice        - appliquer une etape logique\n");
    printf("  resoudre      - résoudre (DLX)\n");
    printf("  trace on|off  - activer/desactiver la trace\n");
    printf("  sauvegarder [chemin] - sauvegarder en JSON (defaut data/save.json)\n");
    printf("  charger [chemin]     - charger depuis JSON\n");
    printf("  aide          - afficher cette aide\n");
    printf("  quitter       - sortir\n");
}