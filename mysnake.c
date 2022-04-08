/*
 ============================================================================
 Name        : mysnake.c
 Author      : Rachel Liang
             : Jiaxin Jiang
             : Joseph Lumpkin
 Version     : 1.0
 Copyright   :
 Description : An implementation of the classic snake game.
 ============================================================================
 */

// Includes
//****************************
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

// Defines
//****************************
/* Game Times */
#define DELAY               100000  // 100 milliseconds represented as microseconds
#define ONE_SECOND          1000000 // 1 second represented as microseconds
#define TOT_MAX_TIMEOUT     10      // 9 seconds

/* Colors = PRIMARY_BACKGROUND */
#define COLOR_GREEN_BLACK   1   // Green with black background
#define COLOR_WHITE_BLACK   2   // White with black background
#define COLOR_YELLOW_BLACK  3   // Yellow with black background

/* Game driver loop test (Infinite) */
#define ACTIVE              1

// Prototype Functions
//****************************
void    checktrophy();
bool    checkwon();
bool    didsnakehitself();
void    initboard();
char*   initialize(int C);
void    initsnakebodarrs();
int     kbhit();
void    printsnakebod();
void    trophygen();
int     startsnakegame();


// Global Variables
//****************************
/* Snake Attributes */
int currenti = 1;       // Current snake head y value
int currentj = 1;       // Current snake head x value
int previousi;          // Previous y value for the snake head
int previousj;          // Previous x value for the snake head
int *snakebodyi;        // Snake body y values : index = distance from head, value = grid coord
int *snakebodyj;        // Snake body x values : index = distance from head, value = grid coord
int snakesize = 5;      // Current size of the snake (Player score)
int counter;            // Current snake body refresh index

/* Window Attributes */
int maxrow;             // Maximum number of rows in the game grid
int maxcol;             // Maximum number of columns in the game grid
bool resize;            // Window resizable flag

/* Trophy Attributes */
int trophyi;            // Trophy y value
int trophyj;            // Trophy x value
int trophyval;          // Trophy score value

/* Game Attributes */
long gameclock;         // Current time value in microseconds


/**
 * Snake game driver.
 * This contains the game's main loop function
 * and drives the game.
 *
 * @author Rachel Liang
 *
 * @param ac    - Number of arguments passed to the main function
 * @param av    - Array of additional arguments passed to the game
 */
int main(int argc, char **argv) {
    initscr();
    noecho();
    curs_set(0);
    int yMax, xMax;
    getmaxyx(stdscr,yMax,xMax);
    WINDOW *win = newwin(yMax/2,xMax/2,yMax/4,xMax/4);
    // If the current terminal does NOT support colored text
    if(has_colors() == FALSE)
    {
        // Inform the user and abort
        printf("Your terminal does not support color\n");
        printf("Aborting %s...\n", argv[0]);
        exit(1);
    }

    // Initialize color pairs (PRIMARY, BACKGROUND)
    start_color();  // Enable the usage of colors
    init_pair(COLOR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);     // Green with black background
    init_pair(COLOR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);     // White with black background
    init_pair(COLOR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);   // Yellow with black background
    box(win,0,0);
    mvwprintw(win, 8-2, 20, "=====Start Game: Press s or S=====");
    mvwprintw(win, 10-2, 20, "======Exit Game: Press Ctrl+C=====");
    char ch = wgetch(win);
    while(ch){
        switch (ch) {
            case 's':
            case 'S':
                startsnakegame();
                break;
            default:
                ch = wgetch(win);
                break;
        }
    }
    
    endwin();
    return 0;
}


int startsnakegame() {
    // Seed the rand() function using the current system time
    srand(time(NULL)); 
    // Gameplay ending message
    // This is displayed to user upon game completion    
    char* endingmsg;
    
    // Create and initialize ncurses window
    WINDOW * win;           // ncurses window struct
    win = initscr();        // Initialize ncurses's window
    cbreak();               // Break when ctrl ^ c is pressed
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input
    initboard();            // Obtain terminal size and initialize window size values
    initsnakebodarrs();     // Initialize player snake values
    
    // Initialize the snake head
    int inputChar, previousChar = 0;
    // Generate an initial snake direction
    int randomstart = rand() % 4;
    // Translate random int to direction
    switch(randomstart) {
        case 0:
            inputChar = previousChar = KEY_RIGHT;   // Snake faces right
            break;
        case 1:
            inputChar = previousChar = KEY_LEFT;    // Snake faces left
            break;
        case 2:
            inputChar = previousChar = KEY_UP;      // Snake faces up
            break;
        case 3:
            inputChar = previousChar = KEY_DOWN;    // Snake faces down
            break;
    }

    // Generate an initial trophy
    trophygen();
    // Initialize a trophy expiration accumulation timer
    int totcounter = 0;
    int totcountertimeout = (rand() % TOT_MAX_TIMEOUT) * 10;   // Do not exceed 9 seconds
    
    // Main snake game loop
    while (ACTIVE) {
        // Draw game borders
        werase(win);                                    // Erase the screen
        wattron(win, COLOR_PAIR(COLOR_WHITE_BLACK));    // Change the color of the next drawn object
        box(win,0,0);                                   // Draw a white border
        move(maxrow,1);                                 // Move the cursor near the bottom of the screen
        hline('-', maxcol-2);                           // Make a horizontal screen divisor
        
        // Print the current score
        mvprintw
        (
            maxrow + 1,             // Bottom of the screen
            maxcol/2 - 6,           // Centered horizontally
            "score: %d", snakesize  // Size of snake (score)
        );

        // Check to see if user has won
        if(checkwon()) {
            endingmsg = "YOU WIN!"; // Change the ending message appropriately
            break;                  // Exit the loop
        }

        // If the trophy has expired
        if (totcounter >= totcountertimeout) {
            trophygen();                                        // Generate a new random trophy
            totcounter = 0;                                     // Reset the trophy expiration accumulator
            totcountertimeout = (rand() % TOT_MAX_TIMEOUT) * 10;   // Generate a new random timeout
        }

        // Print the trophy to the screen        
        wattron(win, COLOR_PAIR(COLOR_YELLOW_BLACK));   // Set the color of the trophy
        mvprintw
        (
            trophyi,    // The trophy's y coord
            trophyj,    // The trophy's x coord
            "%d",       // The trophy
            trophyval   // Value of the trophy
        );
        
        // If user ran into the border
        if (currenti == 0 || currentj == 0 || currenti == maxrow || currentj == maxcol-1) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";    // Change the ending message appropriately
            break;  // Exit the loop
        }

        // Store the previous coords of the snake head
        previousi = currenti;
        previousj = currentj;

        // Check to see if the snake has run into the trophy
        checktrophy();

        // Print the snake head
        wattron(win, COLOR_PAIR(COLOR_GREEN_BLACK));    // Change the color of the next drawn object
        mvprintw
        (
            currenti,               // Current snake head y coord
            currentj,               // Current snake head x coord
            initialize(inputChar)   // Snake head
        );

        // If the user presses any button
        if (kbhit()) {
            previousChar = inputChar;   // Store the previous keypress/direction
            inputChar = getch();        // Grab the new keypress/snake head
            // If the new direction is directly opposite of the previous direction, user loses
            if ((previousChar == KEY_RIGHT && inputChar == KEY_LEFT) || (previousChar == KEY_LEFT && inputChar == KEY_RIGHT) ||
                (previousChar == KEY_DOWN && inputChar == KEY_UP) || (previousChar == KEY_UP && inputChar == KEY_DOWN)) {
                endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!"; // Change ending message appropriately
                break; // Leave the loop, ending the game
            }
        }
        
        // If the snake body is fully grown (Shows all body segments on screen)
        if (counter == (snakesize-1)) {
            // Move the snake body forward without growth
            for(int i=0; i<(snakesize); i++)
            {
                // Shift body values left in their arrays
                snakebodyi[i]=snakebodyi[i+1];
                snakebodyj[i]=snakebodyj[i+1];
            }
            // Add the previous snake head coordinate at the end of the snake body arrays
            snakebodyi[(snakesize-1)] = previousi;
            snakebodyj[(snakesize-1)] = previousj;
        }
        // Else the snake body array has not been filled yet
        else
        {
            // Grow the snake by filling up the array with the previous coordinates one at a time
            snakebodyi[counter] = previousi;
            snakebodyj[counter] = previousj;
            counter++;  // Increment snake size counter
        }

        // Print the snake body
        printsnakebod();

        // If the snake has run into itself
        if (didsnakehitself()) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!"; // Change the ending message appropriately
            break; // Leave the loop
        }

        refresh();      // Refresh the screen
        totcounter++;   // Add to the total counter (used in the trophy gen)
        usleep(DELAY);  // The speed of the snake game, 62.5 frames per second.
    }
    
    // Free the snake body arrays in memory
    free(snakebodyi);
    free(snakebodyj);

    werase(win);                                    // Erase the screen
    wattron(win, COLOR_PAIR(COLOR_WHITE_BLACK));    // Change the color of the next drawn object
    box(win,0,0);                                   // Draw a white border
    attron(A_BLINK);                                // Blink the next drawn thing on the terminal screen

   // Prepare the ending message
    mvprintw
    (
        maxrow/2-2,                                   // Center vertically
        (maxcol/2)-(strlen(endingmsg)/2),           // Center horizontally
        endingmsg                                   // Message to display
    ); 

    mvprintw
    (
        maxrow/2+2-2,                                   // Center vertically
        (maxcol/2)-(14/2),           // Center horizontally
        "Your score: %d",snakesize                                   // display score
    );     

    mvprintw(maxrow/2+4-2, (maxcol/2)-(strlen(endingmsg)/2), "=====Do you want to start again?=====");
    mvprintw(maxrow/2+6-2, (maxcol/2)-(strlen(endingmsg)/2), "===== Start Game: Press s or S =====");
    mvprintw(maxrow/2+8-2, (maxcol/2)-(strlen(endingmsg)/2), "====== Exit Game: Press Ctrl+C =====");

    char ch = wgetch(win);
    while(ch){
        switch (ch) {
            case 's'|'S':
                snakesize = 5;
                startsnakegame();
                break;
            default:
                ch = wgetch(win);
                break;
               
        }
    } 
    refresh();                                      // Refresh the screen to display the ending message
    getch();                                        // Wait for the user to press any key to exit

    // Clean up and exit
    delwin(win);
    endwin();
    refresh();
    return EXIT_SUCCESS;
}


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
        snakesize = newsize;
        resize = true;
    } else {
        refresh();
        resize = false;
    }
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
        default:
            break;
            
    }
    return c;
}

/* get the size of the terminal */
void initboard() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    maxrow = w.ws_row-3;
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
