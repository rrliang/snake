#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAIN_WIN_COLOR 1
#define DELAY 100000

void initboard();
char* initialize(int C);
void printboard(char c, WINDOW *win);
int **boardArr;
int maxrow, maxcol;
int snakesize = 5;
int currenti = 1;
int currentj = 1;
bool running = true;
char* endingmsg;

int kbhit()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}


int main()
{
    // /*  Create and initialize window  */
    initscr();
    

    WINDOW * win;
    win = initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    // if ( (win = initscr()) == NULL ) {
    //     fputs("Could not initialize screen.", stderr);
    //     exit(EXIT_FAILURE);
    // }

    /*  Draw border  */
    initboard();
    //box(win, 0, 0);
    int inputChar=KEY_RIGHT;
    int i , y = 0;
    while (1) {
        werase(win);
        box(win, 0, 0);
        if (currenti == 0 || currentj == 0 || currenti == maxrow || currentj == maxcol) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";
            break;
        }
        mvprintw(currenti, currentj, initialize(inputChar));
        if (kbhit()) {
            int previousChar = inputChar;
            inputChar = getch();
            if ((previousChar == KEY_RIGHT && inputChar == KEY_LEFT) || (previousChar == KEY_LEFT && inputChar == KEY_RIGHT) ||
                (previousChar == KEY_DOWN && inputChar == KEY_UP) || (previousChar == KEY_UP && inputChar == KEY_DOWN)) {
                endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!";
                break;
            } 
            refresh();
        } else {
            refresh();
        }
        refresh();
        // wrefresh(win);
        usleep(DELAY);
        // initialize(inputChar, win);
        // nodelay(win, TRUE);
    }
    werase(win);
    printw("%s",endingmsg);
    getch();
    
    

    /*  Wait for keypress before ending  */
    // initialize(win);
        
    // getch();

    /*  Clean up and exit  */
    delwin(win);
    endwin();
    refresh();
    

    return EXIT_SUCCESS;
}

char* initialize(int inputChar) {
    // initscr();
	// cbreak();
	// noecho();
	// keypad(stdscr, TRUE);
    // while(inputChar != 'q') {
        // while (inputChar == KEY_UP) {
        //     wrefresh(win);
        //     //printw("%d", currenti);
        //     printboard('^',win);
        //     refresh();
        //     usleep(DELAY);
        //     currenti -= 1;
        //     // inputChar = getch();
        // } 
        // while (inputChar == KEY_DOWN) {
        //     wrefresh(win);
        //     //printw("%d", currenti);
        //     printboard('v',win);
        //     refresh();
        //     usleep(DELAY);
        //     currenti += 1;
        //     //inputChar = getch();
        // }
        // while(inputChar == KEY_LEFT) {
        //     wrefresh(win);
        //     //printw("%d", currentj);
        //     printboard('<',win);
        //     refresh();
        //     usleep(DELAY);
        //     currentj -= 1;
        //     //inputChar = getch();
        // } 
        // while (inputChar == KEY_RIGHT) {
        //     wrefresh(win);
        //     //printw("%d", currentj);
        //     printboard('>', win);
        //     usleep(DELAY);
        //     refresh();
        //     currentj += 1;
        //     //inputChar = getch();
        // }
        char* c = ">";
        switch(inputChar) {
            case KEY_UP:
                // wrefresh(win);
                currenti -= 1;
                //printw("%d", currenti);
                c = "^";
                break;
            case KEY_DOWN:
                // wrefresh(win);
                currenti += 1;
                //printw("%d", currenti);
                c = "V";
                break;
            case KEY_LEFT:
                // wrefresh(win);
                currentj -= 1;
                //printw("%d", currentj);
                //printboard('<',win);
                c = "<";
                break;
            case KEY_RIGHT:
                // wrefresh(win);
                //printw("%d", currentj);
                currentj += 1;
                //printboard('>', win);
                c = ">";
                break;
        // }
        //wrefresh(win);
        // inputChar = getch();
        
        }
        return c;
    //endwin();
}

void initboard() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    maxrow = w.ws_row;
    maxcol = w.ws_col;
    boardArr = malloc(maxrow*sizeof(int *));
    if (boardArr == NULL)
        return;
    for (int i = 0; i < maxrow ; i++)
    {
        boardArr[i] = malloc(maxcol*sizeof(int));
        if (boardArr[i] == NULL)
            return;
    }
}

void printboard(char c, WINDOW *win) {
    werase(win);
    for (int i = 0; i < maxrow; i++) {
        for (int j = 0; j < maxcol; j++) {
            if (i == currenti && j == currentj) {
                printw("%c",c);
            } else {
                printw(" ");
            }
        }
        //printw("\n");
    }
    box(win, 0, 0);
    wrefresh(win);
}

struct snakehead {
    //hold the i and j of the head of the snake
    //hold the size of the snake

};

struct trophy {
    //size of trophy
    //i and j of where the trophy is in the boardarr
    //how many seconds the trophy has left
};

void won() {

}