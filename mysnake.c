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

        /*generate a new trophy, refresh the screen, update the size of the snake to the newsize*/
        
        for(int i=0; i<(snakesize); i++)
        {
            snakebodyi[i]=snakebodyi[i+1];
            snakebodyj[i]=snakebodyj[i+1];
        }
        //add the trophy coordinate at the end of the snake body arrays
        snakebodyi[(snakesize-1)] = currenti;
        snakebodyj[(snakesize-1)] = currentj;
       
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
    /*Random seed so trophy gen is not the same random numbers every time*/
    srand(time(NULL)); 

    /*  Create and initialize window  */
    initscr();
    WINDOW * win;
    win = initscr();
    cbreak(); //break when ctrl ^ c is pressed
    noecho(); //don't show any commands given by user
    curs_set(FALSE); //no curser
    keypad(stdscr, TRUE); //keypad is going to be used

    /*  Draw border  */
    initboard(); //get the max rows and cols of the terminal screen
    initsnakebodarrs(); //re-size the snake body arrays with the actual size of the snake body at this time
    int inputChar, previousChar = 0;
    int randomstart = rand() % 4; //generate a random number 0-3 to see which direction the snake goes at initialization
    /* 0 = start going right, 1 = start going left, 2 = start going up, 3 = start going down */
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
    /* Check if terminal can display colors (google terminal is able) */
    if(has_colors() == FALSE)
    {
        printf("Your terminal does not support color\n");
        exit(1);
    }
    /* initialize color pairs where all backgrounds are black but any object using this color pair will appear either green (1), white (2), or yellow(3)*/
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);

    int totcounter = -1;
    previoussize = 0;
    resize = false;
    
    /* loop through the rest of the snake game to simulate movement*/
    while (1) {
        werase(win); //erase the screen
        wattron(win, COLOR_PAIR(2)); //change the color of the next drawn object to be white with a background of black
        box(win,0,0); //draw a white border
        //mvprintw(maxrow, 8, "score: %d", snakesize); //print size of snake (score)
        refresh();
        if(checkwon()) { //check to see if user has won
            endingmsg = "YOU WIN!"; //change the ending message appropriately
            break; //exit the loop
        }
        if (totcounter == -1) { //if it is the first loop
            refresh(); //refresh the screen
            trophygen(); //generate a random trophy
        } else if (totcounter == rand() % (100-1 + 1 - 10) + 10) { //trophy will expire from a random time between 1 and 9 seconds
            refresh(); //refresh the screen
            //trophygen(); //generate a new random trophy
            totcounter = 0; //reset the total counter to 0
        }

        wattron(win, COLOR_PAIR(3)); //set the color of the next drawn object to be yellow with a background of black
        mvprintw(trophyi, trophyj,"%d",trophyval); //set the color of the trophy to yellow
        refresh(); //refresh the screen
        
        /* check to see if user ran into the border */
        if (currenti == 0 || currentj == 0 || currenti == maxrow-1 || currentj == maxcol-1) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!"; //change the ending message appropriately
            break; //exit the loop
        }

        /* store the previous coords of the snake head*/
        previousi = currenti;
        previousj = currentj;

        /* check to see if the snake has run into the trophy*/
        // checktrophy();

        wattron(win, COLOR_PAIR(1)); //change the color of the next drawn object to be green with a background of black
        checktrophy();
        mvprintw(currenti, currentj, initialize(inputChar)); //draw the head of the snake that has been moved
        
        refresh(); //refresh the screen
            
        if (kbhit()) { //if the user presses any button, otherwise just keep looping/moving in current direction
            previousChar = inputChar; //store the previous keypress/direction
            inputChar = getch(); //grab the new keypress
            /* if the new direction is directly opposite of the previous direction, user loses*/
            if ((previousChar == KEY_RIGHT && inputChar == KEY_LEFT) || (previousChar == KEY_LEFT && inputChar == KEY_RIGHT) ||
                (previousChar == KEY_DOWN && inputChar == KEY_UP) || (previousChar == KEY_UP && inputChar == KEY_DOWN)) {
                endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!"; //change ending message appropriately
                break; //leave the loop
            } 
            refresh(); //refresh the screen
        } else {
            refresh(); //refresh the screen
        }
        
        /* store the previous coordinates (going up to the size of the snakesize) of the snake head into an i and j array for the snake body*/
        if (counter == (snakesize-1)) { //if not the first run (if the snake body arrays have been filled already)
            /* shift everything in the snake body arrays to the left*/
            for(int i=0; i<(snakesize); i++)
            {
                snakebodyi[i]=snakebodyi[i+1];
                snakebodyj[i]=snakebodyj[i+1];
            }
            //add the previous coordinate at the end of the snake body arrays
        
            snakebodyi[(snakesize-1)] = previousi;
            snakebodyj[(snakesize-1)] = previousj;
            

            refresh(); //refresh the screen

        } else { //if the snake body array has not been filled yet
            /* fill up the array with the previous coordinates */
            snakebodyi[counter] = previousi; 
            snakebodyj[counter] = previousj;
            counter++; //add to a counter of which this if statement is reading from
            refresh(); //refresh the screen
        }
        
        printsnakebod(); //print the snake body
        /* Check to see if the snake has run into itself */
        if (didsnakehitself()) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!"; //change the ending message appropriately
            break; //leave the loop
        }

        refresh(); //refresh the screen
        usleep(DELAY- (snakesize * 1000)); //the speed of the snake movement (the delay between each loop)
        totcounter++; //add to the total counter (used in the trophy gen)
    }
    
    /* Free the snake body arrays in memory */
    free(snakebodyi);
    free(snakebodyj);

    werase(win); //erase the screen
    wattron(win, COLOR_PAIR(2)); //change the color of the next drawn object to be white with a background of black
    box(win,0,0); //draw a white border
    attron(A_BLINK); //change the next drawn thing to be blinking on the terminal screen
    mvprintw(maxrow/2,(maxcol/2)-(strlen(endingmsg)/2),endingmsg); //print the message in the middle of the screen
    refresh(); //refresh the screen
    getch(); //read the next character

    /*  Clean up and exit  */
    delwin(win);
    endwin();
    refresh();
    return EXIT_SUCCESS;
}

/* prints the snake body onto screen */
void printsnakebod() {
    for (int i = (snakesize-1); i > 0; i--) {
        if (snakebodyi[i] != 0 && snakebodyj[i] != 0) {
            mvprintw(snakebodyi[i], snakebodyj[i], "o");
        }
    }
}

/* check to see if the snake hit itself */
bool didsnakehitself() {
    for (int i = 0; i < (snakesize-1); i++) {
        if (snakebodyi[i] == currenti && snakebodyj[i] == currentj) { //if the head of the snake is in the same 
            return true;
        }
    }
    return false;
}

/* change the coordinates of the snake head depending on which direction/arrow key has been pressed */
char* initialize(int inputChar) {
    char* c;
    switch(inputChar) {
        case KEY_UP:
            currenti -= 1;
            c = "^"; //if going up, change the snake head to appropriate character
            break;
        case KEY_DOWN:
            currenti += 1;
            c = "V"; //if going down, change the snake head to appropriate character
            break;
        case KEY_LEFT:
            currentj -= 1;
            c = "<"; //if going left, change the snake head to appropriate character
            break;
        case KEY_RIGHT:
            currentj += 1;
            c = ">"; //if going right, change the snake head to appropriate character
            break;
    }
    return c;
}

/* get the size of the terminal */
void initboard() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    maxrow = w.ws_row-1;
    maxcol = w.ws_col;
    currenti = maxrow/2;
    currentj = maxcol/2;
}

/* initialize the snakebody arrays */
void initsnakebodarrs() {
    snakebodyi = malloc((snakesize-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
    
    snakebodyj = malloc((snakesize-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
}
