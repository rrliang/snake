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
#include <signal.h>

// Project Includes
//****************************
#include "debug/debug.h"
#include "snake/snake.h"
#include "trophy/trophy.h"

// Defines
//****************************
/* Game Times */
#define PRIMARY_DELAY       134000  // 134 milliseconds as microseconds
#define SECONDARY_DELAY     100000  // 100 milliseconds as microseconds
#define TERTIARY_DELAY      66000   // 66 milliseconds as microseconds

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
char*   initialize(int);
int     kbhit();
void    startsnakegame();
void    sig_int_handler(int);
int     quit();
int     get_game_speed();


// Global Variables
//****************************

/* Verbose Debug Logging Flag */
bool D = true;

/* Window Attributes */
WINDOW *startWin;       // Start screen window
WINDOW *win;            // ncurses window struct
int maxrow;             // Maximum number of rows in the game grid
int maxcol;             // Maximum number of columns in the game grid
bool resize;            // Window resizable flag
bool run = true;        // Whether or not to continue running the program

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
    signal(SIGINT, sig_int_handler); // Prevent Ctrl+C presses from quitting.

    // Create and initialize windows
    initboard();   // Obtain terminal size and initialize window values
    startWin = initscr();
    cbreak();               // Break when ctrl ^ c is pressed
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input
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
    // The start screen is smaller and centered
    int yMax, xMax;
    char startGame_text[] = "=====Start Game: Press s or S=====";
    char exitGame_text[] = "======Exit Game: Press q or Q=====";
    yMax=5;
    xMax = (sizeof(startGame_text)>sizeof(exitGame_text)?sizeof(startGame_text):sizeof(exitGame_text)) +1;
    startWin = newwin(yMax,xMax,maxrow/3,maxcol/3);
    box(startWin,0,0);
    mvwprintw(startWin, yMax/2-1, 1, startGame_text);
    mvwprintw(startWin, yMax/2+1, 1,exitGame_text);

    while(run){
        if(D) debug_log("mysnake::main", "Main game loop beginning new iteration.");
        switch (wgetch(startWin)) {
            case 's':
            case 'S':
                if (D) debug_log("mysnake::main", "S pressed to start game.");
                startsnakegame(); // Start the snake game
                break;
            case 'q':
            case 'Q':
                if (D) debug_log("mysnake::main", "Q pressed to quit game.");
                run = FALSE;      // Quit the program
                break;
        }
    }
    return quit();
}


void startsnakegame() {
    if(D) debug_log("mysnake::startsnakegame", "Starting the snake game.");

    win = initscr();        // Initialize game window
    cbreak();               // Break when ctrl ^ c is pressed
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input
    initboard();            // Obtain terminal size and initialize window values
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
    trophy_gen(maxrow, maxcol);

    // Main snake game loop
    while (run) {
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
        if (trophy_get_time() >= trophy_get_expiration()) {
            if (D) debug_log("mysnake::startsnakegame", "trophy_time has reached it's expiration. Generating a new Trophy.");
            // Generate a new trophy
            trophy_gen(maxrow, maxcol);
            if (D) debug_log("mysnake::startsnakegame", "Finished generating a new trophy.");
        }

        // Print the trophy to the screen
        wattron(win, COLOR_PAIR(COLOR_YELLOW_BLACK));   // Set the color of the trophy
        mvprintw
        (
            trophy_get_i(),       // The trophy's y coord
            trophy_get_j(),       // The trophy's x coord
            "%d",                 // The trophy
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
            trophy_gen(maxrow, maxcol);
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
        // Get the game speed
        int speed_level = get_game_speed(); // Level of speed difficulty
        unsigned int delay;
        unsigned int time_inc;
        switch (speed_level) {
          case 0: // We're in the first 1/3 of the winning Score
              delay = PRIMARY_DELAY;  // Move every 100ms (10/1 second)
              time_inc = PRIMARY_DELAY/1000;
              break;
          case 1: // We're in the second 1/3 of the winning Score
              delay = SECONDARY_DELAY;  // Move every 66ms ~(15/1 second)
              time_inc = SECONDARY_DELAY/1000;
              break;
          case 2: // We're in the third 1/3 of the winning Score
              delay = TERTIARY_DELAY;  // Move every 50ms (20/1 second)
              time_inc = TERTIARY_DELAY/1000;
              break;
        }

        if (D) {
          char str[128];
          sprintf(str, "Program times calculated, speed_level = %d, delay = %d, time_inc = %d", speed_level, delay, time_inc);
          debug_log("mysnake::startsnakegame", str);
        }
        // Update the trophy's time spent alive (current time + 100ms)
        trophy_set_time(trophy_get_time() + time_inc);
        if (D) debug_log("mysnake::startsnakegame", "usleep");
        usleep(delay);  // The speed of the snake game, 10 frames per second.
    }

    werase(win);                                    // Erase the screen
    wattron(win, COLOR_PAIR(COLOR_WHITE_BLACK));    // Change the color of the next drawn object
    box(win,0,0);                                   // Draw a white border
    attron(A_BLINK);                                // Blink the terminal screen

   // Prepare the ending message
    mvprintw
    (
        maxrow/2-2,                                 // Center vertically
        (maxcol/2)-(strlen(endingmsg)/2),           // Center horizontally
        endingmsg                                   // Message to display
    );

    mvprintw
    (
        maxrow/2+2-2,                       // Center vertically
        (maxcol/2)-(14/2),                  // Center horizontally
        "Your score: %d", snake_get_size()  // display score
    );

    mvprintw(maxrow/2+4-2, (maxcol/2)-(strlen(endingmsg)/2), "=====Do you want to start again?=====");
    mvprintw(maxrow/2+6-2, (maxcol/2)-(strlen(endingmsg)/2), "===== Start Game: Press s or S =====");
    mvprintw(maxrow/2+8-2, (maxcol/2)-(strlen(endingmsg)/2), "====== Exit Game: Press q or Q =====");

    while(run){
        switch (wgetch(win)) {
            case 's':
            case 'S':
                if (D) debug_log("mysnake::startsnakegame", "S pressed to continue game.");
                attroff(A_BLINK);  // Turn off the terminal blinking
                snake_init();
                startsnakegame();
                break;
            case 'q':
            case 'Q':
                if (D) debug_log("mysnake::startsnakegame", "Q pressed to quit game.");
                run = FALSE;
                break;
        }
    }
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

/*
 * Signal Handler for SIGINT (Ctrl+C)
 * This receives any signaling for Ctrl+C and throws it out.
 * Effectively preventing the user from quitting the program
 * with Ctrl+C.
 */
void sig_int_handler(int sig_num) {
    // Reset handler
    signal(SIGINT, sig_int_handler);
    fflush(stdout);
}

/**
 * Quit the program
 */
 int quit() {
   // Clean up and exit
   delwin(win);
   delwin(startWin);
   endwin();

   // Free the snake body arrays in memory
   snake_free_i_body();
   snake_free_j_body();
   return EXIT_SUCCESS;
 }

 /**
  * Get the appropriate game speed level
  * This is based off of the player's score (snake length)
  *
  * @returns  int - Speed level of the game
  */
  int get_game_speed() {
      int score = snake_get_size();       // Current Score
      int winningScore = maxrow + maxcol; // Winning Score
      if (score >= (winningScore / 3) * 2) {
        return 2;
      }
      else if (score >= winningScore / 3) {
        return 1;
      }
      else {
        return 0;
      }
  }
