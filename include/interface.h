// Interfaces shared by CLI and GUI.

#ifndef INTERFACE_H
#define INTERFACE_H
#include "utils.h"
#include "solver.h"
#include "generator.h"
#include "trace.h"

int save_game(const char* path, const Grid* g);
int load_game(const char* path, Grid* g);

void print_banner(void);
void print_help(void);

int run_gui(void);

#endif