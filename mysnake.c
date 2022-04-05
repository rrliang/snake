#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAIN_WIN_COLOR 1
#define DELAY 100000

void initboard();
char* initialize(int C);
void printboard(char c, WINDOW *win);
void printsnakebod();
bool didsnakehitself();
unsigned int rand_interval(unsigned int min, unsigned int max);
void initsnakebodarrs();
struct snake;
int **boardArr;
int *snakebodyi;
int *snakebodyj;
int maxrow, maxcol;
int snakesize = 5;
int currenti = 1;
int currentj = 1;
bool running = true;
char* endingmsg;
int trophyi, trophyj;
int trophyval;
int winningper;
bool resize;
int previoussize;
int previousi;
int previousj;
int counter = 0;
int count = 0;
int previoustrophyi;
int previoustrophyj;

int kbhit() //https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

bool checkwon() {
    return (snakesize >= ((maxcol+maxrow)*2)/2); //return true if size of snake is half of the terminal perimeter
}


/* trophy will have a value (1-9), and be randomly generated somewhere in the snakepit*/
void trophygen()
{
    trophyval = 1 + rand() % (9 + 1 - 1); //generate a random number from range of 1-9
    trophyi = 1 + (rand() % ((maxrow-2))); //generate a random number for the x coordinate of the trophy from range of 1 to (horizontal length of terminal-1)
    trophyj = 1 + (rand() % ((maxcol-2))); //generate a random number for the y coordinate of the trophy from range of 1 to (vertical length of terminal-1)
    
    /* check if randomly generated trophy coordinates clash with existing snake head and body
       if yes, regenerate the random coordinate, otherwise exit loop */
    
    bool inotsame = false;
    bool jnotsame = false;
    while (inotsame) {
        bool isamecurrent = (trophyi != currenti); //random x coord for trophy does not clash with snake head
        bool inotsamebody = true;
        for (int i = 0; i < snakesize-1 ; i++) {
            if (trophyi == snakebodyi[i] || trophyi >= maxrow) { //random x coord for trophy is not part of the snake body
                inotsamebody = false;
                break;
            }
        }
        if (inotsame && inotsamebody) { 
            inotsame = true;
        } else {
            trophyi = 1 + (rand() % ((maxrow-2))); 
        }
    }

    while (jnotsame) {
        bool jsamecurrent = (trophyj != currentj); //random y coord for trophy does not clash with snake head
        bool jnotsamebody = true;
        for (int i = 0; i < snakesize-1 ; i++) {
            if (trophyj == snakebodyj[i] || trophyj >= maxcol) { //random y coord for trophy is not part of the snake body
                jnotsamebody = false;
                break;
            }
        }
        if (jnotsame && jnotsamebody) {
            jnotsame = true;
        } else {
            trophyj = 1 + (rand() % ((maxcol-2)));
        }
    }

    // if (previoustrophyi != 0 && previoustrophyj != 0) {
    //     mvprintw(previoustrophyi, previoustrophyj, "o");
    // }
}

/*this function will check whether or not the snake head has gotten to the trophy*/
void checktrophy() {
    if (currenti == trophyi && currentj == trophyj) { //if snake head coords are same as the trophy coords
        int newsize = snakesize + trophyval; //newsize of the snake is original size + value of the trophy
        
        // previousi = trophyi;
        // previousj = trophyj;
        
        /*resize the snakebodyi and snakebodyj arrays to the newsize of the snake*/
        snakebodyi=realloc(snakebodyi, newsize * sizeof(int));
        if (snakebodyi == NULL)
            return;
        
        snakebodyj=realloc(snakebodyj, newsize * sizeof(int));
        if (snakebodyj == NULL)
            return;

        count = 0;
        previoustrophyi = trophyi;
        previoustrophyj = trophyj;        
        refresh();
        trophygen();
        refresh();
        previoussize = snakesize;
        snakesize = newsize;
        resize = true;
    } else {
        refresh();
        resize = false;
    }
}

int main()
{
    srand(time(NULL)); 
    /*  Create and initialize window  */
    initscr();
    WINDOW * win;
    win = initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

    /*  Draw border  */
    initboard();
    initsnakebodarrs();
    int inputChar, previousChar = 0;
    int randomstart = rand() % 4;
    switch(randomstart) {
        case 0:
            inputChar = previousChar = KEY_RIGHT;
            break;
        case 1:
            inputChar = previousChar = KEY_LEFT;
            break;
        case 2:
            inputChar = previousChar = KEY_UP;
            break;
        case 3:
            inputChar = previousChar = KEY_DOWN;
            break;
    }
    if(has_colors() == FALSE)
    {
        printf("Your terminal does not support color\n");
        exit(1);
    }
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    
    int i , y = 0;
    bool ranOnce = true;
    int totcounter = -1;
    previoussize = 0;
    resize = false;
    while (1) {
        werase(win);
        wattron(win, COLOR_PAIR(2));
        box(win, 0, 0);
        if(checkwon()) {
            endingmsg = "YOU WIN!";
            break;
        }
        if (totcounter == -1) {
            refresh();
            trophygen();
        } else if (totcounter == rand() % (100-1 + 1 - 10) + 10) {
            refresh();
            trophygen();
            totcounter = 0;
        }

        wattron(win, COLOR_PAIR(3));
        mvprintw(trophyi, trophyj,"%d",trophyval);
        refresh();
        
        if (currenti == 0 || currentj == 0 || currenti == maxrow-1 || currentj == maxcol-1) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";
            break;
        }
        previousi = currenti;
        previousj = currentj;
        checktrophy();


        wattron(win, COLOR_PAIR(1));
        mvprintw(currenti, currentj, initialize(inputChar));
        
        refresh();
            
        if (kbhit()) {
            previousChar = inputChar;
            inputChar = getch();
            if (currenti == 0 || currentj == 0 || currenti == maxrow || currentj == maxcol) {
                endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";
                break;
            }
            refresh();
        } else {
            refresh();
        }
        if (counter == (snakesize-1)) {
            for(i=0; i<(snakesize); i++)
            {
                snakebodyi[i]=snakebodyi[i+1];
                snakebodyj[i]=snakebodyj[i+1];
            }
            snakebodyi[(snakesize-1)] = previousi;
            snakebodyj[(snakesize-1)] = previousj;
            refresh();

        } else {
            snakebodyi[counter] = previousi;
            snakebodyj[counter] = previousj;
            counter++;
            refresh();
        }
        printsnakebod();
        if (didsnakehitself()) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!";
            break;
        }
        refresh();
        usleep(DELAY- (snakesize * 1000));
        totcounter++;
    }
    free(snakebodyi);
    free(snakebodyj);
    wattron(win, COLOR_PAIR(2));
    werase(win);
    box(win,0,0);
    attron(A_BLINK);
    mvprintw(maxrow/2,(maxcol/2)-(strlen(endingmsg)/2),endingmsg);
    refresh();
    getch();

    /*  Clean up and exit  */
    delwin(win);
    endwin();
    refresh();
    return EXIT_SUCCESS;
}

void printsnakebod() {
    for (int i = (snakesize-1); i > 0; i--) {
        mvprintw(snakebodyi[i], snakebodyj[i], "o");
    }
}

bool didsnakehitself() {
    for (int i = 0; i < (snakesize-1); i++) {
        if (snakebodyi[i] == currenti && snakebodyj[i] == currentj) {
            return true;
        }
    }
    return false;
}

char* initialize(int inputChar) {
    char* c = ">";
    switch(inputChar) {
        case KEY_UP:
            currenti -= 1;
            c = "^";
            break;
        case KEY_DOWN:
            currenti += 1;
            c = "V";
            break;
        case KEY_LEFT:
            currentj -= 1;
            c = "<";
            break;
        case KEY_RIGHT:
            currentj += 1;
            c = ">";
            break;
    }
    return c;
}

void initboard() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    maxrow = w.ws_row;
    maxcol = w.ws_col;
    currenti = maxrow/2;
    currentj = maxcol/2;
}

void initsnakebodarrs() {
    snakebodyi = malloc((snakesize-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
    
    snakebodyj = malloc((snakesize-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
}
