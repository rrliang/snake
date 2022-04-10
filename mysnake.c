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

// Library Includes
//****************************
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

// Project Includes
#include "debug/debug.h"
#include "snake/snake.h"
#include "trophy/trophy.h"

// Defines
//****************************
/* Game Times */
#define DELAY               100000  // 100 milliseconds represented as microseconds
#define TOT_MAX_TIMEOUT     10      // 9 seconds

/* Colors = PRIMARY_BACKGROUND */
#define COLOR_GREEN_BLACK   1   // Green with black background
#define COLOR_WHITE_BLACK   2   // White with black background
#define COLOR_YELLOW_BLACK  3   // Yellow with black background

/* Game driver loop test (Infinite) */
#define ACTIVE              1

// Prototype Functions
//****************************
bool    checkwon();
void    initboard();
char*   initialize(int C);
int     kbhit();
void    startsnakegame();


// Global Variables
//****************************

/* Verbose Debug Logging Flag */
bool D = true;  // Set to true for verbose debug information

/* Window Attributes */
WINDOW *startWin;       // Start screen window
WINDOW *win;            // ncurses window struct
int maxrow;             // Maximum number of rows in the game grid
int maxcol;             // Maximum number of columns in the game grid
bool resize;            // Window resizable flag


/* Key Press Trackers */
int inputChar, previousChar, lastvalidChar = 0;


/**
 * Snake game driver.
 * This contains the game's main loop function
 * and drives the game.
 *
 * @param ac    - Number of arguments passed to the main function
 * @param av    - Array of additional arguments passed to the game
 */
int main(int argc, char **argv) {
    debug_clear_log();  // Prepare the log file for a new program run

    // Create and initialize windows
    startWin = initscr();   // Initialize start screen window
    cbreak();               // Break when ctrl ^ c is pressed
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input
    initboard();            // Obtain terminal size and initialize window values
    snake_init();           // Initialize player snake values

    // If the current terminal does NOT support colored text
    if(has_colors() == FALSE)
    {
        // Inform the user and abort
        printf("Your terminal does not support color\n");
        printf("Aborting %s...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize color pairs (PRIMARY, BACKGROUND)
    start_color();  // Enable the usage of colors
    init_pair(COLOR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);     // Green with black background
    init_pair(COLOR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);     // White with black background
    init_pair(COLOR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);   // Yellow with black background
    box(startWin, 0, 0);
    char *startGame = "=====Start Game: Press s or S=====";
    mvwprintw(startWin, maxrow/2, maxcol/2 - strlen(startGame)/2, startGame);
    char *exitGame = "======Exit Game: Press Ctrl+C=====";
    mvwprintw(startWin, maxrow/2 + 2, maxcol/2 - strlen(exitGame)/2, exitGame);
    char ch = wgetch(startWin);

    while(ch){
        if(D) debug_log("mysnake::main", "Main game loop beginning new iterration.");
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

    refresh();
    usleep(10000000);

    // Clean up and exit
    delwin(win);
    endwin();
    refresh();

    // Free the snake body arrays in memory
    snake_free_i_body();
    snake_free_j_body();
    return EXIT_SUCCESS;
}


void startsnakegame() {
    if(D) debug_log("mysnake::startsnakegame", "Starting the snake game.");

    win = initscr();        // Initialize game window
    cbreak();               // Break when ctrl ^ c is pressed
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input
    initboard();            // Obtain terminal size and initialize window values
    snake_set_size(5);      // Set the snake's size
    snake_set_counter(0);
    snake_init();           // Initialize player snake values

    // Seed the rand() function using the current system time
    srand(time(NULL));

    // Gameplay ending message
    // This is displayed to user upon game completion
    char* endingmsg;

    // Initialize the snake head
    // int inputChar, previousChar = 0;
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
    trophygen(maxrow, maxcol);
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
            "score: %d", snake_get_size()  // Size of snake (score)
        );

        // Check to see if user has won
        if(checkwon()) {
            endingmsg = "YOU WIN!"; // Change the ending message appropriately
            break;                  // Exit the loop
        }

        // If the trophy has expired
        if (totcounter >= totcountertimeout) {
            trophygen(maxrow, maxcol);                                        // Generate a new random trophy
            totcounter = 0;                                     // Reset the trophy expiration accumulator
            totcountertimeout = (rand() % TOT_MAX_TIMEOUT) * 10;   // Generate a new random timeout
        }

        // Print the trophy to the screen
        wattron(win, COLOR_PAIR(COLOR_YELLOW_BLACK));   // Set the color of the trophy
        mvprintw
        (
            trophy_get_i(),       // The trophy's y coord
            trophy_get_j(),       // The trophy's x coord
            "%d",           // The trophy
            trophy_get_value()    // Value of the trophy
        );

        // If user ran into the border
        if (snake_get_curr_i() == 0 || snake_get_curr_j() == 0 || snake_get_curr_i() == maxrow || snake_get_curr_j() == maxcol-1) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";    // Change the ending message appropriately
            break;  // Exit the loop
        }

        // Store the previous coords of the snake head
        snake_set_previous_i(snake_get_curr_i());
        snake_set_previous_j(snake_get_curr_j());

        // Check to see if the snake has run into the trophy
        if (checktrophy(snake_get_curr_i(), snake_get_curr_j())) {
            snake_grow();
            trophygen(maxrow, maxcol);
            resize = true;
        } else {

            resize = false;
        }

        // Print the snake head
        wattron(win, COLOR_PAIR(COLOR_GREEN_BLACK));    // Change the color of the next drawn object
        mvprintw
        (
            snake_get_curr_i(),               // Current snake head y coord
            snake_get_curr_j(),               // Current snake head x coord
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

        // Move the snake
        snake_move();

        // Print the snake body
        snake_print();

        // If the snake has run into itself
        if (snake_did_snake_hit_self()) {
            if (D) debug_log("mysnake::startsnakegame", "Snake did hit itself, breaking.");
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!"; // Change the ending message appropriately
            break; // Leave the loop
        }
        if (D) debug_log("mysnake::startsnakegame", "Snake did not hit self, continuing.");
        refresh();      // Refresh the screen
        if (D) debug_log("mysnake::startsnakegame", "incrementing totcounter.");
        totcounter++;   // Add to the total counter (used in the trophy gen)
        if (D) debug_log("mysnake::startsnakegame", "usleep");
        usleep(DELAY);  // The speed of the snake game, 62.5 frames per second.
    }

    werase(win);                                    // Erase the screen
    wattron(win, COLOR_PAIR(COLOR_WHITE_BLACK));    // Change the color of the next drawn object
    box(win,0,0);                                   // Draw a white border
    attron(A_BLINK);                                // Blink the terminal screen

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
        "Your score: %d", snake_get_size()                                   // display score
    );

    mvprintw(maxrow/2+4-2, (maxcol/2)-(strlen(endingmsg)/2), "=====Do you want to start again?=====");
    mvprintw(maxrow/2+6-2, (maxcol/2)-(strlen(endingmsg)/2), "===== Start Game: Press s or S =====");
    mvprintw(maxrow/2+8-2, (maxcol/2)-(strlen(endingmsg)/2), "====== Exit Game: Press Ctrl+C =====");

    char ch = wgetch(win);
    while(ch){
        switch (ch) {
            case 's':
            case 'S':
                attroff(A_BLINK);  // Blink the terminal screen
                snake_set_size(5);
                snake_init();
                startsnakegame();
                break;
            default:
                ch = wgetch(win);
                break;

        }
    }
    // refresh();                                      // Refresh the screen to display the ending message
    // getch();                                        // Wait for the user to press any key to exit
    //
    // // Clean up and exit
    // delwin(win);
    // endwin();
    // refresh();
    return;
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
    // Return true if size of snake is half of the terminal perimeter
    return (snake_get_size() >= (maxcol + maxrow));
}


/* change the coordinates of the snake head depending on which direction/arrow key has been pressed */
char* initialize(int inputChar) {
    char* c;
    switch(inputChar) {
        case KEY_UP:
            snake_set_curr_i(snake_get_curr_i() - 1);
            c = "^"; //if going up, change the snake head to appropriate character
            lastvalidChar = inputChar;
            break;
        case KEY_DOWN:
            snake_set_curr_i(snake_get_curr_i() + 1);
            c = "V"; //if going down, change the snake head to appropriate character
            lastvalidChar = inputChar;
            break;
        case KEY_LEFT:
            snake_set_curr_j(snake_get_curr_j() - 1);
            c = "<"; //if going left, change the snake head to appropriate character
            lastvalidChar = inputChar;
            break;
        case KEY_RIGHT:
            snake_set_curr_j(snake_get_curr_j() + 1);
            c = ">"; //if going right, change the snake head to appropriate character
            lastvalidChar = inputChar;
            break;
        default:
            switch(lastvalidChar) {
                case KEY_UP:
                    snake_set_curr_i(snake_get_curr_i() - 1);
                    c = "^"; //if going up, change the snake head to appropriate character
                    break;
                case KEY_DOWN:
                    snake_set_curr_i(snake_get_curr_i() + 1);
                    c = "V"; //if going down, change the snake head to appropriate character
                    break;
                case KEY_LEFT:
                    snake_set_curr_j(snake_get_curr_j() - 1);
                    c = "<"; //if going left, change the snake head to appropriate character
                    break;
                case KEY_RIGHT:
                    snake_set_curr_j(snake_get_curr_j() + 1);
                    c = ">"; //if going right, change the snake head to appropriate character
                    break;
        }


    }
    return c;
}


/* get the size of the terminal */
void initboard() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    maxrow = w.ws_row-3;
    maxcol = w.ws_col;
    snake_set_curr_i(maxrow/2);
    snake_set_curr_j(maxcol/2);
}
