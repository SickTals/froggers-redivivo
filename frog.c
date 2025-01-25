#include "frog.h"
#include "common.h"

void frog(WINDOW **win, int pipefd[]){
    close(pipefd[0]);
    char user_input;
    msg msg_frog;
    msg_frog.id = Id_frog;
    msg_frog.shoots = false;
    while(true){ // Ciclo infinito chiuso dal padre
        msg_frog.p.y = 0;
        msg_frog.p.x = 0;
        user_input = wgetch(*win);
        flushinp();
        switch(user_input){
            case Key_up: 
                msg_frog.p.y = (int)'+';
                break;
            case Key_down:
                msg_frog.p.y = (int)'-';
                break;
            case Key_left:
                msg_frog.p.x = (int)'-';
                break;
            case Key_right:
                msg_frog.p.x = (int)'+';
                break;
            case Key_shoot:
                msg_frog.shoots = true;
                break;
        // TODO: funzione della pausa
        //  case PAUSE_KEY: break;
            case Key_quit:
                msg_frog.id = Id_quit;
                break;
        }
        // Scrittura su pipe della posizione dell'oggetto
        (void)write(pipefd[1], &msg_frog, sizeof(msg_frog));
        msg_frog.shoots = false;
    }
    close(pipefd[1]);
}

msg handleFrog(pos p, msg f)
{
    if (p.y == '+')
        f.p.y -= Y_STEP;
    else if (p.y == '-')
        f.p.y += Y_STEP;
    if (p.x == '-')
        f.p.x -= X_STEP;
    else if (p.x == '+')
        f.p.x += X_STEP;

    if (f.p.y <= 1)
        f.p.y += Y_STEP;
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
    mvwprintw(*g_win, f.p.y, f.p.x, SPRITE_FROG);
    mvwprintw(*g_win, f.p.y - 1, f.p.x, SPRITE_FROG);
}

void granade(WINDOW **g_win, int pipefd[], int pipefd_projectiles[]) {
    msg g;
    close(pipefd[0]);
    close(pipefd_projectiles[1]);
    
    while (true) {
        g.shoots = false;
        ssize_t bytes_read = read(pipefd_projectiles[0], &g, sizeof(g));
        // Check if read was successful
        if (bytes_read != sizeof(g)) {
            perror("Read error on projectile pipe");
            continue;
        }

        if (!g.shoots)
            continue;
        g.id = Id_granade;
        g.sx_x = g.p.x - 1;
        g.p.x += strlen(SPRITE_FROG);

        while (g.p.x < GSIZE || g.sx_x > 0) {
            g.p.x++;
            g.sx_x--;

            (void)write(pipefd[1], &g, sizeof(g));
            usleep(UDELAY/3);
        }
    }
}

void printGranade(WINDOW **g_win, msg g)
{
    mvwaddch(*g_win, g.p.y, g.p.x, '>');
    mvwaddch(*g_win, g.p.y, g.sx_x, '<');
}
