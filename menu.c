#include "menu.h"
#include "common.h"
#include "curses.h"

void initMenu(WINDOW **win, const char *sprite[])
{
    wattron(*win, COLOR_PAIR(Ui));
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int j = 1; j < GSIZE/2 - 1; j++)
        for (int i = 1; i < GSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');

    for (int x = BORDER_START_X + 1; x < BORDER_END_X - 1; x++) {
        mvwprintw(*win,  BORDER_START_Y , x, "%c" , '_');  // Top border
        mvwprintw(*win, BORDER_START_X, x, "%c" , '_');  // Bottom border
    }
    for (int y = BORDER_START_Y + 1; y <= BORDER_START_X ; y++) {
        mvwprintw(*win, y, BORDER_START_X , "%c" , '|');  // Left border
        mvwprintw(*win, y, BORDER_END_X - 1 , "%c" ,'|');  // Right border
    }
    wattroff(*win, COLOR_PAIR(Ui));

    wattron(*win, COLOR_PAIR(Evil_Ui));
    for(int i = 0; i < 6; i++)
        mvwprintw(*win, MENU_START_Y + i, GSIZE/8 , "%s", sprite[i]);
    wattroff(*win, COLOR_PAIR(Evil_Ui));
}
/* 
 * Stampa il menu
 * Restituisce
 *  false - Esce dal gioco
 *  true - Avvia gioco
 */
gstate menu(WINDOW **g_win, WINDOW **ui_win, int cursor, gstate flag)
{
    gstate state = flag;
    while(state == flag) {
        printMenuUi(ui_win);
        initMenu(g_win, (const char*[]){SPRITE_MTITLE});
        printMenu(g_win, cursor);
        state = handleMenu(g_win, &cursor);
    }
    return state;
}

void menuPrintSelector(WINDOW **win, int sel){
    switch(sel){
        case Msg_play:
        case Msg_difficulty:
        case Pair_one:
            mvwaddch(*win, GSIZE/4, GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2 - 2,
                     SPRITE_CURSOR);
            mvwprintw(*win, GSIZE/4 , GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2,
                      MSG_TO_STRING(sel));
            return;
        case Msg_opts:
        case Msg_sprite:
        case Pair_two:
            mvwaddch(*win, (GSIZE/4 - MENU_START_Y) + (STATIC_SPACE * 3)/2,
                     GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2 - 2,
                     SPRITE_CURSOR);
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + (STATIC_SPACE * 3)/2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2,
                      MSG_TO_STRING(sel));
            return;
        case Msg_quit:
        case Msg_back:
        case Pair_three:
            mvwaddch(*win, (GSIZE/4 - MENU_START_Y) + STATIC_SPACE * 2,
                     GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2 - 2,
                     SPRITE_CURSOR);
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + STATIC_SPACE * 2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2,
                      MSG_TO_STRING(sel));
            return;
        case Color_menu:
            mvwprintw(*win, GSIZE/4,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_play))/2,
                      MSG_TO_STRING(Msg_play));
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + (STATIC_SPACE * 3)/2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_opts))/2,
                      MSG_TO_STRING(Msg_opts));
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + STATIC_SPACE * 2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_quit))/2,
                      MSG_TO_STRING(Msg_quit));
            return;
        case Color_options:
            mvwprintw(*win, GSIZE/4,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_difficulty))/2,
                      MSG_TO_STRING(Msg_difficulty));
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + (STATIC_SPACE * 3)/2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_sprite))/2,
                      MSG_TO_STRING(Msg_sprite));
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + STATIC_SPACE * 2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Msg_back))/2,
                      MSG_TO_STRING(Msg_back));
            return;
        case Sprite_options:
            mvwprintw(*win, GSIZE/4,
                      GSIZE/2 - strlen(MSG_TO_STRING(Pair_one))/2,
                      MSG_TO_STRING(Pair_one));
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + (STATIC_SPACE * 3)/2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Pair_two))/2,
                      MSG_TO_STRING(Pair_two));
            mvwprintw(*win, (GSIZE/4 - MENU_START_Y) + STATIC_SPACE * 2,
                      GSIZE/2 - strlen(MSG_TO_STRING(Pair_three))/2,
                      MSG_TO_STRING(Pair_three));
            return;
    }
}

void printMenu(WINDOW **win, int cursor)
{

    wattron(*win, COLOR_PAIR(Ui));
    if (IS_OPTIONS(cursor)) {
        menuPrintSelector(win, Color_options);
    }
    else if(IS_SPRITE_OPTIONS(cursor)){
        menuPrintSelector(win, Sprite_options);
    }
    else {
        menuPrintSelector(win, Color_menu);
    }
    wattron(*win, COLOR_PAIR(Ui));

    wattron(*win, COLOR_PAIR(Evil_Ui));
    menuPrintSelector(win, cursor);
    wattroff(*win, COLOR_PAIR(Evil_Ui));
    wrefresh(*win);
}

gstate handleSelection(int cursor)
{
    switch(cursor) {
        case Msg_play:
            return Game;
        case Msg_opts:
            return Options;
        case Msg_quit:
            return Exit;
        case Msg_difficulty:
            //return difficulty();
        case Msg_sprite:
            return SprOpt;
        case Msg_back:
            return Menu;
        case Pair_one:
            return colorChangeFrog(1);
        case Pair_two:
            return colorChangeFrog(2);
        case Pair_three:
            return colorChangeFrog(3);
        default:
            return Exit;
    }
}


gstate handleMenu(WINDOW **win, int *cursor)
{
    bool isSprOpt = IS_SPRITE_OPTIONS(*cursor);
    bool isOptions = IS_OPTIONS(*cursor);
    char user_input = wgetch(*win);

    switch (user_input) {
        case Key_quit:
            return Exit;
        case (char)KEY_UP:
        case Key_up:
            *cursor -= 1;
            break;
        case (char)KEY_DOWN:
        case Key_down:
            *cursor += 1;
            break;
        case '\n':
            return handleSelection(*cursor);
    }

    if(isOptions) {
        if (*cursor < Msg_difficulty)
            *cursor = Msg_back;
        if (*cursor > Msg_back)
            *cursor = Msg_difficulty;

        return Options;
    } else if(isSprOpt){
        if (*cursor < Pair_one)
            *cursor = Pair_three;
        if (*cursor > Pair_three)
            *cursor = Pair_one;

        return SprOpt;
    }else {
        if (*cursor < Msg_play)
            *cursor = Msg_quit;
        if (*cursor > Msg_quit)
            *cursor = Msg_play;

        return Menu;
    }
}

void pauseMenu(WINDOW **win)
{
    bool flag = true;

    while(flag){
        wclear(*win);
        printPauseMenu(win, (const char*[]){SPRITE_PAUSE});
        wrefresh(*win);
        char user_input = wgetch(*win);
        switch (user_input) {
            case Key_pause:
            case '\n':
            case ' ':
                flag = false;
                break;
            default:
                break;
        }
    }
}

gstate colorChangeFrog(int pair){
    if (pair == 1){
    }else if (pair == 2){
        init_pair(Frog, COLOR_BROWN, COLOR_WHITE);
    }
    switch (pair) {
        case 1:
            init_pair(Frog, COLOR_BROWN, COLOR_WHITE);
            break;
        case 2:
            init_pair(Frog, COLOR_MAGENTA, COLOR_GREEN);
            break;
        case 3:
            init_pair(Frog, COLOR_YELLOW, COLOR_BROWN);
            break;
    }

    return Options;
}
void printPauseMenu(WINDOW **win, const char *sprite[])
{
    wattron(*win, COLOR_PAIR(Ui));
    box(*win, ACS_VLINE, ACS_HLINE);
    for(int j = 1; j < PSIZE/3 - 1; j++)
        for (int i = 1; i < PSIZE - 1; i++)
            mvwaddch(*win, j, i, ' ');

    wattron(*win, COLOR_PAIR(Alt_E_Ui));
    for(int i = 0; i < LENGHT_PAUSE; i++)
        mvwprintw(*win, MENU_START_Y + i, PSIZE/6, "%s", sprite[i]); // Update row to 4 + i // Ensure the window is refreshed

    wattroff(*win, COLOR_PAIR(Alt_E_Ui));
    wattroff(*win, COLOR_PAIR(Ui));
}
