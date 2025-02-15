#include "frog.h"
#include "river.h"

void frog(WINDOW **win, int pipefd[]){
    close(pipefd[0]);
    char user_input;
    msg msg_frog;
    while(true){ // Ciclo infinito chiuso dal padre
        msg_frog.id = Id_frog;
        msg_frog.p.y = 0;
        msg_frog.p.x = 0;
        msg_frog.shoots = false;
        user_input = wgetch(*win);
        flushinp();
        switch(user_input){
            case Key_up: 
                msg_frog.p.y = Key_up;
                break;
            case Key_down:
                msg_frog.p.y = Key_down;
                break;
            case Key_left:
                msg_frog.p.x = Key_left;
                break;
            case Key_right:
                msg_frog.p.x = Key_right;
                break;
            case Key_shoot:
                msg_frog.shoots = true;
                break;
            case Key_pause:
                msg_frog.id = Id_pause;
                break;
            case Key_quit:
                msg_frog.id = Id_quit;
                break;
        }
        // Scrittura su pipe della posizione dell'oggetto
        (void)write(pipefd[1], &msg_frog, sizeof(msg_frog));
    }
}

msg handleFrog(pos p, msg f)
{
    if (p.y == Key_up)
        f.p.y -= Y_STEP;
    else if (p.y == Key_down)
        f.p.y += Y_STEP;
    if (p.x == Key_left)
        f.p.x -= X_STEP;
    else if (p.x == Key_right)
        f.p.x += X_STEP;

    if (f.p.y <= 1)
        f.p.y = 2;
    else if (f.p.y >= GSIZE/2 - 1)
        f.p.y -= Y_STEP;
    if (f.p.x < 0)
        f.p.x += X_STEP;
    else if (f.p.x > GSIZE - 2)
        f.p.x -= X_STEP;

    return f;
}

void printFrog(WINDOW **g_win, msg f)
{
    char sprite_frog[2][3]={
            "<M>",
            "/W\\"
    };
    wattron(*g_win, COLOR_PAIR(Grass_Frog));
    for(int i = 0; i < 3; i++){
        mvwaddch(*g_win, f.p.y - 1, f.p.x + i, sprite_frog[0][i]);
        mvwaddch(*g_win, f.p.y, f.p.x + i, sprite_frog[1][i]);
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
        if (bytes_read != sizeof(g) || !(g.shoots))
            continue;

        g.id = Id_granade;
        g.sx_x = g.p.x - 1;
        g.p.x += strlen(SPRITE_FROG);

        while (g.p.x < GSIZE || g.sx_x > 0) {
            msg tmp;
            ssize_t n = read(pipefd_grenade[0], &tmp, sizeof(tmp));
            if (n == sizeof(tmp)) {
                if (tmp.sx_x == INVALID_CROC)
                    g.sx_x = 0;
                if (tmp.p.x == INVALID_CROC)
                    g.p.x = GSIZE;
            }
            g.p.x++;
            g.sx_x--;
            write(pipefd[1], &g, sizeof(g));
            usleep(UDELAY/3);
        }
        usleep(UDELAY);
    }
}

void printGranade(WINDOW **g_win, msg g)
{
    wattron(*g_win, COLOR_PAIR(Gren));
    mvwaddch(*g_win, g.p.y - 1, g.p.x, '\\');
    mvwaddch(*g_win, g.p.y, g.p.x, '/');
    mvwaddch(*g_win, g.p.y - 1 , g.sx_x, '/');
    mvwaddch(*g_win, g.p.y, g.sx_x, '\\');
    wattroff(*g_win, COLOR_PAIR(Gren));
}
