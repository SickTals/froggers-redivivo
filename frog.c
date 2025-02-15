#include "frog.h"
#include "river.h"

void frog(WINDOW **win, int pipefd[]){
    close(pipefd[0]);
    char user_input;
    msg msg_frog;
    obj f;
    while(true){ // Ciclo infinito chiuso dal padre
        msg_frog.id = Id_frog;
        f.y = 0;
        f.x = 0;
        f.shoots = false;
        user_input = wgetch(*win);
        flushinp();
        switch(user_input){
            case Key_up: 
                f.y = Key_up;
                break;
            case Key_down:
                f.y = Key_down;
                break;
            case Key_left:
                f.x = Key_left;
                break;
            case Key_right:
                f.x = Key_right;
                break;
            case Key_shoot:
                f.shoots = true;
                break;
            case Key_pause:
                msg_frog.id = Id_pause;
                break;
            case Key_quit:
                msg_frog.id = Id_quit;
                break;
        }
        msg_frog.objs[0] = f;
        // Scrittura su pipe della posizione dell'oggetto
        (void)write(pipefd[1], &msg_frog, sizeof(msg_frog));
    }
}

msg handleFrog(obj incoming, obj frog)
{
    msg msg_frog;

    if (incoming.y == Key_up)
        frog.y -= Y_STEP;
    else if (incoming.y == Key_down)
        frog.y += Y_STEP;
    if (incoming.x == Key_left)
        frog.x -= X_STEP;
    else if (incoming.x == Key_right)
        frog.x += X_STEP;

    if (frog.y <= 1)
        frog.y = 2;
    else if (frog.y >= GSIZE/2 - 1)
        frog.y -= Y_STEP;
    if (frog.x < 0)
        frog.x += X_STEP;
    else if (frog.x > GSIZE - 2)
        frog.x -= X_STEP;

    msg_frog.objs[0] = frog;
    return msg_frog;
}

void printFrog(WINDOW **g_win, obj frog)
{
    char sprite_frog[2][3]={
            "<M>",
            "/W\\"
    };
    wattron(*g_win, COLOR_PAIR(Grass_Frog));
    for(int i = 0; i < 3; i++){
        mvwaddch(*g_win, frog.y - 1, frog.x + i, sprite_frog[0][i]);
        mvwaddch(*g_win, frog.y, frog.x + i, sprite_frog[1][i]);
    }
    wattroff(*g_win, COLOR_PAIR(Grass_Frog));

}

void granade(int pipefd[], int pipefd_grenade[]) {
    msg g;


    close(pipefd[0]);
    int flags = fcntl(pipefd_grenade[0], F_GETFL, 0);
    fcntl(pipefd_grenade[0], F_SETFL, flags | O_NONBLOCK);

    while (true) {
        ssize_t bytes_read = read(pipefd_grenade[0], &g, sizeof(g));
        if (bytes_read != sizeof(g) || !(g.objs[0].shoots))
            continue;

        obj left = {
            .y = g.objs[0].y,
            .x = g.objs[0].x - 1
        };
        obj right = {
            .y = g.objs[0].y,
            .x = g.objs[0].x + strlen(SPRITE_FROG)
        };
        g.id = Id_granade;

        while (right.x < GSIZE || left.x > 0) {
            msg tmp;
            ssize_t n = read(pipefd_grenade[0], &tmp, sizeof(tmp));
            if (n == sizeof(tmp)) {
                if (tmp.objs[0].x == INVALID_CROC)
                    left.x = 0;
                if (tmp.objs[1].x == INVALID_CROC)
                    right.x = GSIZE;
            }
            left.x--;
            right.x++;
            g.objs[0] = left;
            g.objs[1] = right;

            write(pipefd[1], &g, sizeof(g));
            usleep(UDELAY/3);
        }
        usleep(UDELAY);
    }
}

void printGranade(WINDOW **g_win, msg g)
{
    wattron(*g_win, COLOR_PAIR(Gren));
    mvwaddch(*g_win, g.objs[1].y - 1, g.objs[1].x, '\\');
    mvwaddch(*g_win, g.objs[1].y, g.objs[1].x, '/');
    mvwaddch(*g_win, g.objs[0].y - 1 , g.objs[0].x, '/');
    mvwaddch(*g_win, g.objs[0].y, g.objs[0].x, '\\');
    wattroff(*g_win, COLOR_PAIR(Gren));
}
