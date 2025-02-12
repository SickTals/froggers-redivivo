#include "frog.h"
#include "common.h"

void frog(WINDOW **win, int pipefd[]){
    close(pipefd[0]);
    char user_input;
    msg msg_frog;
    msg_frog.shoots = false;
    while(true){ // Ciclo infinito chiuso dal padre
        msg_frog.id = Id_frog;
        msg_frog.p.y = 0;
        msg_frog.p.x = 0;
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
        msg_frog.shoots = false;
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
    for(int i = 0; i < 3; i++)
    {
        mvwaddch(*g_win, f.p.y - 1, f.p.x + i, sprite_frog[0][i]);
        mvwaddch(*g_win, f.p.y, f.p.x + i, sprite_frog[1][i]);
    }
}

void granade(int pipefd[], int pipefd_grenade[]) {
    msg g;
    close(pipefd[0]);
    
    while (true) {
        g.shoots = false;
        ssize_t bytes_read = read(pipefd_grenade[0], &g, sizeof(g));
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

        if (g.p.x >= GSIZE) 
            g.p.x = GSIZE - 1;
        if (g.sx_x < 0)
            g.sx_x = 0;

        while (g.p.x < GSIZE || g.sx_x > 0) {
            g.p.x++;
            g.sx_x--;

            (void)write(pipefd[1], &g, sizeof(g));
            usleep(UDELAY/3);
        }

        usleep(UDELAY);
    }
}


void printGranade(WINDOW **g_win, msg g)
{
    mvwaddch(*g_win, g.p.y, g.p.x, '>');
    mvwaddch(*g_win, g.p.y, g.sx_x, '<');
}
