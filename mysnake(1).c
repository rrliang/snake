#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
// #include <time.h>

#define MAIN_WIN_COLOR 1
#define DELAY 100000

void initboard();
char* initialize(int C);
void printboard(char c, WINDOW *win);
void printsnakebod();
bool didsnakehitself();
unsigned int rand_interval(unsigned int min, unsigned int max);
struct snake;
int **boardArr;
int *snakebodyi;
int *snakebodyj;
int maxrow, maxcol;
int snakesize = 5;
int currenti = 1;
int currentj = 5;
bool running = true;
char* endingmsg;
int trophyi, trophyj;
int trophyval;
int winningper;

int kbhit() //https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

void trophygen(WINDOW *win)
{
    werase(win);
    trophyval = rand_interval(0, 9);
    
    trophyi = rand_interval(1, maxrow);
    trophyj = rand_interval(1, maxcol);
    bool inotsame = false;
    bool jnotsame = false;
    while (inotsame) {
        bool isamecurrent = (trophyi != currenti);
        bool inotsamebody = true;
        for (int i = 0; i < snakesize-1 ; i++) {
            if (trophyi == snakebodyi[i]) {
                inotsamebody = false;
                break;
            }
        }
        if (inotsame && inotsamebody) {
            inotsame = true;
        } else {
            trophyi = rand_interval(1, maxrow);
        }
    }
    while (jnotsame) {
        bool jsamecurrent = (trophyj != currentj);
        bool jnotsamebody = true;
        for (int i = 0; i < snakesize-1 ; i++) {
            if (trophyj == snakebodyj[i]) {
                jnotsamebody = false;
                break;
            }
        }
        if (inotsame && jnotsamebody) {
            jnotsame = true;
        } else {
            trophyj = rand_interval(1, maxcol);
        }
    }
    
    mvprintw(trophyi, trophyj,"%d",trophyval);
    usleep(900000);
}

unsigned int rand_interval(unsigned int min, unsigned int max) //https://stackoverflow.com/questions/2509679/how-to-generate-a-random-integer-number-from-within-a-range
{
    int r;
    const unsigned int range = 1 + max - min;
    const unsigned int buckets = RAND_MAX / range;
    const unsigned int limit = buckets * range;

    /* Create equal size buckets all in a row, then fire randomly towards
     * the buckets until you land in one of them. All buckets are equally
     * likely. If you land off the end of the line of buckets, try again. */
    do
    {
        r = rand();
    } while (r >= limit);

    return min + (r / buckets);
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
    int inputChar, previousChar = KEY_RIGHT;
    int i , y = 0;
    bool ranOnce = true;
    int counter = 0;
    while (1) {
        //struct snake snakebody[snakesize];
        werase(win);
        box(win, 0, 0);
        if (currenti == 0 || currentj == 0 || currenti == maxrow || currentj == maxcol) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";
            break;
        }
        int previousi = currenti;
        int previousj = currentj;
        mvprintw(currenti, currentj, initialize(inputChar));
        
        
        //trophygen(win);
        refresh();
            
        
        
        if (kbhit()) {
            previousChar = inputChar;
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
               
        if (counter == (snakesize-2)) {
            for(i=0; i<(snakesize-1); i++)
            {
                snakebodyi[i]=snakebodyi[i+1];
                snakebodyj[i]=snakebodyj[i+1];
            }
            snakebodyi[(snakesize-1)] = previousi;
            snakebodyj[(snakesize-1)] = previousj;
        } else {
            snakebodyi[counter] = previousi;
            snakebodyj[counter] = previousj;
            counter++;
        }
        printsnakebod();
        if (didsnakehitself()) {
            endingmsg = "YOU LOST BECAUSE YOU HIT YOURSELF!";
            break;
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

void printsnakebod() {
    for (int i = 0; i < (snakesize-1); i++) {
        if (snakebodyi[i] != 0 && snakebodyj[i] != 0) {
            mvprintw(snakebodyi[i], snakebodyj[i], "o");
        }
    }
}

bool didsnakehitself(int currenti, int currentj) {
    for (int i = 0; i < (snakesize-1); i++) {
        if (snakebodyi[i] == currenti && snakebodyj[i] == currentj) {
            return true;
        }
    }
    return false;
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

    snakebodyi = malloc((snakesize-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
    
    snakebodyj = malloc((snakesize-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;

    
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

struct snake 
{
    int i;
    int j;
};

struct trophy {
    //size of trophy
    //i and j of where the trophy is in the boardarr
    //how many seconds the trophy has left
};

void won() {

}