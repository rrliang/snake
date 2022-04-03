#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define MAIN_WIN_COLOR 1
void board();
void initialize();

int main()
{
    // /*  Create and initialize window  */
    initscr();
    

    WINDOW * win;
    win = initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    // if ( (win = initscr()) == NULL ) {
    //     fputs("Could not initialize screen.", stderr);
    //     exit(EXIT_FAILURE);
    // }

    /*  Draw border  */
    box(win, 0, 0);
    wrefresh(win);

    /*  Wait for keypress before ending  */
    initialize();
    // getch();

    /*  Clean up and exit  */
    delwin(win);
    endwin();
    refresh();
    

    return EXIT_SUCCESS;
}

void initialize() {
    // initscr();
	// cbreak();
	// noecho();
	// keypad(stdscr, TRUE);
    int inputChar;
    inputChar = getch();
    while(inputChar != 'q') {
        switch(inputChar) {
            case KEY_UP:
                printw("arrow up");
                break;
            case KEY_DOWN:
                printw("arrow DOWN");
                break;
            case KEY_LEFT:
                printw("arrow LEFT");
                break;
            case KEY_RIGHT:
                printw("arrow RIGHT");
                break;
        }
        inputChar = getch();
    }
    //endwin();
}

void board(int y1, int x1, int y2, int x2) {
    mvhline(y1, x1, 0, '_');
    mvhline(y2, x1, 0, '_');
    mvvline(y1, x1, 0, '|');
    mvvline(y1, x2, 0, '|');
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

struct snakepit {
    int pit[5][5];
    //make the pit a 2d array
    //have a parts of the array be occupied by snake, 1 meaning snake, 0 meaning nothing

};