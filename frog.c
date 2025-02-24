#include "frog.h"
#include "common.h"
#include "river.h"

void frog(WINDOW **win, int pipefd[]){
    close(pipefd[0]);
    char user_input;
    msg frog_msg;
    obj frog;
    while(true){
        frog_msg.id = Id_frog;
        frog.y = 0;
        frog.x = 0;
        frog.shoots = false;
        user_input = wgetch(*win);
        flushinp();
        switch(user_input){
            case Key_up: 
                frog.y = Key_up;
                break;
            case Key_down:
                frog.y = Key_down;
                break;
            case Key_left:
                frog.x = Key_left;
                break;
            case Key_right:
                frog.x = Key_right;
                break;
            case Key_shoot:
                frog.shoots = true;
                break;
            case Key_pause:
                frog_msg.id = Id_pause;
                break;
            case Key_quit:
                frog_msg.id = Id_quit;
                break;
        }
        frog_msg.objs[0] = frog;
        write_message(frog_msg);
    }
}

msg handleFrog(obj incoming, obj frog)
{
    msg frog_msg;

    if (incoming.y == Key_up)
        frog.y -= SIZE_PIXEL;
    else if (incoming.y == Key_down)
        frog.y += SIZE_PIXEL;
    if (incoming.x == Key_left)
        frog.x -= SIZE_PIXEL;
    else if (incoming.x == Key_right)
        frog.x += SIZE_PIXEL;

    if (frog.y <= 1)
        frog.y = 2;
    else if (frog.y >= GSIZE/2 - 1)
        frog.y -= SIZE_PIXEL;
    if (frog.x < 1)
        frog.x = 1;
    else if (frog.x >= GSIZE - 2)
        frog.x = GSIZE - WIDTH_FROG - 1;
    frog_msg.objs[0] = frog;
    return frog_msg;
}

void printFrog(WINDOW **win, obj frog)
{
    char sprite_frog[SIZE_PIXEL][WIDTH_FROG] = {SPRITE_FROG};
    wattron(*win, COLOR_PAIR(Frog));
    for(int i = 0; i <= SIZE_PIXEL; i++){
        mvwaddch(*win, frog.y - 1, frog.x + i, sprite_frog[0][i]);
        mvwaddch(*win, frog.y, frog.x + i, sprite_frog[1][i]);
    }
    wattroff(*win, COLOR_PAIR(Frog));

}

void detectGrenadeCollision(obj grenade[N_grenades], int pipefd[])
{
    msg tmp;
        ssize_t n = read(pipefd[0], &tmp, sizeof(tmp));
        if (n == sizeof(tmp)) {
            if (tmp.objs[Idx_sx_grenade].x == INVALID_CROC)
                grenade[Idx_sx_grenade].x = 0;
            if (tmp.objs[Idx_dx_grenade].x == INVALID_CROC)
                grenade[Idx_dx_grenade].x = GSIZE;
        }
}


void granade(int pipefd[], int pipefd_grenade[]) {
    msg grenade_msg;


    close(pipefd[0]);
    int flags = fcntl(pipefd_grenade[0], F_GETFL, 0);
    fcntl(pipefd_grenade[0], F_SETFL, flags | O_NONBLOCK);

    while (true) {
        ssize_t bytes_read = read(pipefd_grenade[0], &grenade_msg, sizeof(msg));
        if (bytes_read != sizeof(msg) || !(grenade_msg.objs[0].shoots))
            continue;

        obj grenade[2] = {
            {
                .y = grenade_msg.objs[0].y,
                .x = grenade_msg.objs[0].x - 1
            },
            {
                .y = grenade_msg.objs[0].y,
                .x = grenade_msg.objs[0].x + WIDTH_FROG
            }
        };
        grenade_msg.id = Id_granade;

        while (grenade[Idx_sx_grenade].x > 0 || grenade[Idx_dx_grenade].x < GSIZE) {
            detectGrenadeCollision(grenade, pipefd_grenade);

            grenade[Idx_sx_grenade].x--;
            grenade[Idx_dx_grenade].x++;
            grenade_msg.objs[Idx_sx_grenade] = grenade[Idx_sx_grenade];
            grenade_msg.objs[Idx_dx_grenade] = grenade[Idx_dx_grenade];

            write(pipefd[1], &grenade_msg, sizeof(msg));
            usleep(UDELAY/3);
        }
        usleep(UDELAY);
    }
}

void printGranade(WINDOW **win, obj grenade[N_grenades])
{
    char sprite_gren[N_grenades] = {SPRITE_GREN};
    wattron(*win, COLOR_PAIR(Gren));
    mvwaddch(*win, grenade[Idx_sx_grenade].y, grenade[Idx_sx_grenade].x, sprite_gren[0]);
    mvwaddch(*win, grenade[Idx_sx_grenade].y - 1 , grenade[Idx_sx_grenade].x, sprite_gren[1]);
    mvwaddch(*win, grenade[Idx_dx_grenade].y, grenade[Idx_dx_grenade].x, sprite_gren[1]);
    mvwaddch(*win, grenade[Idx_dx_grenade].y - 1, grenade[Idx_dx_grenade].x, sprite_gren[0]);
    wattroff(*win, COLOR_PAIR(Gren));
}
