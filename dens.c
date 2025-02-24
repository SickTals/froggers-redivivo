#include "dens.h"

bool den(bool dens[NDENS], obj frog)
{
    for (int i = 0; i < NDENS; i++) {
        int start_x = DEN_START_X(i) - 2;
        // Se ha superato il fiume (lanes 1-3)
        if (frog.y < 1 || frog.y > 4)
            continue;

        if (frog.x > start_x &&
            frog.x < start_x + DEN_WIDTH &&
            !dens[i]) {
            dens[i] = true;
            return true;
        }
    }
    return false;
}

void printDens(WINDOW **win, bool dens[NDENS])
{
	char sprite_den_open[DEN_HEIGHT][DEN_WIDTH] = {SPRITE_DEN_OPEN};
    char sprite_den_close[DEN_HEIGHT][DEN_WIDTH] = {SPRITE_DEN_CLOSE};

    wattron(*win, COLOR_PAIR(Dens));
    for (int i = 0; i < DEN_HEIGHT; i++) {
        for (int j = 0; j < NDENS; j++) {
            mvwprintw(*win, DEN_START_Y(i), DEN_START_X(j),
                      "%s", dens[j] ? sprite_den_close[i] : sprite_den_open[i]);
        }
    }
    wattroff(*win, COLOR_PAIR(Dens));
}
