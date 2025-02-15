#include "dens.h"

bool den(bool dens[NDENS], pos frog_pos)
{
    for (int i = 0; i < NDENS; i++) {
        int start_x = DEN_START_X(i) - 2;
        // Check vertical alignment (rows 1-3)
        if (frog_pos.y < 1 || frog_pos.y > 4)
            continue;

        // Check if any part of the frog's sprite (3 characters wide) overlaps the den
        if (frog_pos.x > start_x &&
            frog_pos.x < start_x + DEN_WIDTH &&
            !dens[i]) {
            dens[i] = true;
            return true; // Exit after first valid collision
        }
    }
    return false;
}

void printDens(WINDOW **win, bool dens[NDENS])
{
	char sprite_den_open[DEN_HEIGHT][DEN_WIDTH] = { SPRITE_DEN_OPEN };
    char sprite_den_close[DEN_HEIGHT][DEN_WIDTH] = { SPRITE_DEN_CLOSE };

    wattron(*win, COLOR_PAIR(Dens));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < NDENS; j++) {
            mvwprintw(*win, DEN_START_Y(i), DEN_START_X(j),
                      "%s", dens[j] ? sprite_den_close[i] : sprite_den_open[i]);
        }
    }
    wattroff(*win, COLOR_PAIR(Dens));
}
