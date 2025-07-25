#ifndef DENS
#define DENS

#include <ncurses.h>
#include "common.h"

#define NDENS 5

#define DEN_START_Y(x) (x + 2)
#define DEN_START_X(x) (DEN_HEIGHT + x + ((GSIZE - 6) / NDENS) * x)

#define DEN_WIDTH 7
#define DEN_HEIGHT 3

bool den(bool dens[NDENS], obj frog);
void printDens(WINDOW **win, bool dens[NDENS]);

#endif
