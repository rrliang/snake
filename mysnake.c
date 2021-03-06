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
#include <fcntl.h>
#include <ncurses.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

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

/* Inputs */
int     inputType;  // Type of input to expect
#define INPUT_TYPE_KEYBOARD 0
#define INPUT_TYPE_GAMEPAD  1

/* Input Device */
struct pollfd inputDevices[1];  // Input devices (Just one)
const int input_size = 16;      // Size of device input
unsigned char input_data[16];   // Array to hold input data
#define DEVICE_FILE_GAMEPAD "/dev/input/event0"
#define DEVICE_FILE_TIMEOUT 5

// Prototype Functions
//****************************
bool    checkwon();
void    initWindowAttributes();
char*   updateSnakeHead();
int     kbhit();
void    startsnakegame();
void    sig_int_handler(int);
int     quit();
int     getGamepadInput();
int     get_game_speed();
void    menu();

// Global Variables
//****************************

/* Verbose Debug Logging Flag */
bool D = false;

/* Window Attributes */
WINDOW *startWin;       // Start screen window
WINDOW *win;            // ncurses window struct
int     maxrow;         // Maximum number of rows in the game grid
int     maxcol;         // Maximum number of columns in the game grid
bool    resize;         // Window resizable flag

/* Key Press Trackers */
int inputChar, previousChar, lastvalidChar = 0;

/* Program Attributes */
bool    run = true;     // Whether or not to continue running the program

//menu
int snakesize_init;
char snake_speed_choice_primary;
int primary_custom;

/**
 * Snake program driver.
 * This contains the program's main loop function
 * and sets up / destroys the game.
 *
 * @author Jiaxin Jiang
 *
 * @param ac    - Number of arguments passed to the main function
 * @param av    - Array of additional arguments passed to the game
 */
int main(int ac, char *av[]) {
    if (D) debug_clear_log();  // Prepare the log file for a new program run
    // Get and use the gamepad device file for input
    // printf("Program started, waiting 3s\n");
    // usleep(3000000);
    //
    // inputDevices[0].fd = open(DEVICE_FILE_GAMEPAD, O_RDONLY|O_NONBLOCK);
    // // If we couldn't get the device file
    // if (inputDevices[0].fd < 0) {
    //     if (D) debug_log("mysnake::main", "Unable to open input device\n");
    // }
    // else {
    //     inputType = INPUT_TYPE_GAMEPAD;     // User wishes to use gamepad
    //     memset(input_data, 0, input_size);  // Prepare for input
    //     inputDevices[0].events = POLLIN;
    // }

    signal(SIGINT, sig_int_handler); // Prevent Ctrl+C presses from quitting.
    // Create and initialize windows
    initWindowAttributes();   // Obtain terminal size and initialize window values
    initscr();
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input

    // If the current terminal does NOT support colored text
    if(has_colors() == FALSE)
    {
        // Inform the user and abort
        printf("Your terminal does not support color\n");
        printf("Aborting %s...\n", av[0]);
        exit(EXIT_FAILURE);
    }

    // Initialize color pairs (PRIMARY, BACKGROUND)
    start_color();  // Enable the usage of colors
    init_pair(COLOR_GREEN_BLACK, COLOR_GREEN, COLOR_BLACK);     // Green with black background
    init_pair(COLOR_WHITE_BLACK, COLOR_WHITE, COLOR_BLACK);     // White with black background
    init_pair(COLOR_YELLOW_BLACK, COLOR_YELLOW, COLOR_BLACK);   // Yellow with black background

    snake_init();           // Initialize player snake values
    //menu
    menu();
    startWin = initscr();
    int yMax, xMax;
    char startGame_text[32];
    char exitGame_text[32];
    yMax = 5;
    xMax = 33;
    if (inputType == INPUT_TYPE_GAMEPAD) {
        strcpy(startGame_text, "======== Press   Start ========");
        strcpy(exitGame_text,  "===============================");
        mvprintw(1, 1, startGame_text);
        mvprintw(3, 1, exitGame_text);
    }
    else {
        strcpy(startGame_text, "===== Start Game: Press S =====");
        strcpy(exitGame_text,  "===== Exit Game:  Press Q =====");
        startWin = newwin(yMax,xMax,maxrow/2-yMax/2,maxcol/2-xMax/2);
        mvwprintw(startWin, 1, 1, startGame_text);
        mvwprintw(startWin, 3, 1, exitGame_text);
    }
    box(startWin, 0, 0);
    while(run){
        if(D) debug_log("mysnake::main", "Main game loop beginning new iteration.");
        refresh();
        if (inputType == INPUT_TYPE_GAMEPAD) {
            inputChar = getGamepadInput();
        }
        else {
            inputChar = wgetch(startWin);
        }
        switch (inputChar) {
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
            default:
                if (D) {
                    char str[64];
                    sprintf(str, "Unrecognized character int value= %u", inputChar);
                    debug_log("mysnake::main", str);
                }
                break;
        }
    }
    return quit();
}

// menu
void menu(){
    WINDOW * menuwin = initscr();
    menuwin = newwin(12, 90, maxrow/2-6, maxcol/2-45);//row is 8, col is 30
    refresh();
    box(menuwin,0,0);
    wrefresh(menuwin);
    mvwprintw(menuwin, 2, 40,"MENU");
    refresh();
    mvwprintw(menuwin, 4, 10, "Please choose your preferred snake body size initially(1-9) \t");
    snakesize_init = wgetch(menuwin);
    refresh();
    mvwprintw(menuwin, 6, 10, "Please choose your preferred snake move speed initially");
    mvwprintw(menuwin, 7, 10,  "(s for slow, m for middle, f for fast)\t\t\t");
    snake_speed_choice_primary = wgetch(menuwin);
    refresh();
    if(snakesize_init-'0'>=1 && snakesize_init-'0' <=9){
        if(snake_speed_choice_primary == 's'){
            mvwprintw(menuwin, 9, 10, "Your snake size:%d Your snake speed: slow Press any key to continue...", snakesize_init-'0', snake_speed_choice_primary);
            refresh();
            primary_custom = PRIMARY_DELAY + 160000;
        }else if(snake_speed_choice_primary == 'm'){
            mvwprintw(menuwin, 9, 10, "Your snake size:%d Your snake speed: middle Press any key to continue...", snakesize_init-'0', snake_speed_choice_primary);
            refresh();
            primary_custom = PRIMARY_DELAY + 60000;
        }else if(snake_speed_choice_primary == 'f'){
            mvwprintw(menuwin, 9, 10, "Your snake size:%d Your snake speed: fast Press any key to continue...", snakesize_init-'0', snake_speed_choice_primary);
            refresh();
            primary_custom = PRIMARY_DELAY;
        }else {
            mvwprintw(menuwin, 9, 10, "Your choice of snake speed is invalid, default value 'fast' will be used");
            refresh();
            primary_custom = PRIMARY_DELAY;
        }
    }else{
        mvwprintw(menuwin, 9, 10, "Your choice of snake size is invalid, default value '5' will be used");
        refresh();
        snakesize_init=5+'0';
        primary_custom = 134000;
    }
    wgetch(menuwin);
    werase(menuwin);
    wrefresh(menuwin);
    endwin();
}

/**
 * Snake game driver
 * Contains the game's main while loop
 * and calls all functions necessary for
 * processing gameplay.
 *
 * @author Rachel Liang
 */
void startsnakegame() {
    if(D) debug_log("mysnake::startsnakegame", "Starting the snake game.");
    win = initscr();        // Initialize game window
    cbreak();               // Break when ctrl ^ c is pressed
    noecho();               // Disable terminal echoing
    curs_set(FALSE);        // Hide text cursor
    keypad(stdscr, TRUE);   // Utilize keyboard for ncurses input
    initWindowAttributes(); // Obtain terminal size and initialize window values
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
    trophy_gen(maxrow, maxcol, get_game_speed());

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
            maxcol/2 - 20,          // Centered horizontally
            " Score: %d    Win when score is %d", snake_get_size(), (maxcol + maxrow)  // Size of snake (score)
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
            trophy_gen(maxrow, maxcol, get_game_speed());
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
            trophy_gen(maxrow, maxcol, get_game_speed());
            resize = true;
        } else {
            resize = false;
        }

        // Print the snake head
        wattron(win, COLOR_PAIR(COLOR_GREEN_BLACK));    // Change the color of the next drawn object
        char* snakeHead = updateSnakeHead();
        mvprintw
        (
            snake_get_curr_i(), // Current snake head y coord
            snake_get_curr_j(), // Current snake head x coord
            snakeHead           // Snake head
        );

        // If the user presses any button
        if (inputType == INPUT_TYPE_GAMEPAD) {
            int gamepadInput = getGamepadInput();
            if (gamepadInput != 'p') {
                previousChar = inputChar;   // Store the previous keypress/direction
                inputChar = gamepadInput;   // Grab the new keypress/snake head
                // If the new direction is directly opposite of the previous direction, user loses
                if ((previousChar == KEY_RIGHT && inputChar == KEY_LEFT) || (previousChar == KEY_LEFT && inputChar == KEY_RIGHT) ||
                    (previousChar == KEY_DOWN && inputChar == KEY_UP) || (previousChar == KEY_UP && inputChar == KEY_DOWN)) {
                    endingmsg = "YOU RAN INTO YOURSELF!"; // Change ending message appropriately
                    break; // Leave the loop, ending the game
                }
            }
        }
        else {
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
        int speed = get_game_speed(); // Delay of the main game loop
        unsigned int time_inc = speed / 1000; // How much to add to the trophy

        if (D) {
          char str[128];
          sprintf(str, "Program times calculated, speed = %d, time_inc = %d", speed, time_inc);
          debug_log("mysnake::startsnakegame", str);
        }
        // Update the trophy's time spent alive (current time + 100ms)
        trophy_set_time(trophy_get_time() + time_inc);
        if (D) debug_log("mysnake::startsnakegame", "usleep");
        usleep(speed);  // The speed of the snake game, 10 frames per second.
    }

    werase(win);                                    // Erase the screen
    endwin();
    refresh();
    char startagain[] = "==== Do you want to start again? ====";
    char startgame[32];
    char exitstring[32];
    if (inputType == INPUT_TYPE_GAMEPAD) {
        strcpy(startgame,  "======== Press   Start ========");
        strcpy(exitstring, "===============================");
    }
    else {
        strcpy(startgame,  "===== Start Game: Press S =====");
        strcpy(exitstring, "===== Exit Game:  Press Q =====");
    }
    int height = 7;
    int width = strlen(endingmsg)>strlen(startagain)?strlen(endingmsg)+2:strlen(startagain)+2;//get the widest one
    win = newwin(height,width,maxrow/2-height/2,maxcol/2-width/2);//create a new box
    wattron(win, COLOR_PAIR(COLOR_WHITE_BLACK));    // Change the color of the next drawn object
    box(win,0,0);                                   // Draw a white border
    attron(A_BLINK);                                // Blink the terminal screen
   // Prepare the ending message
    mvwprintw
    (   win,
        1,                                   // Center vertically
        width/2-(strlen(endingmsg)/2),           // Center horizontally
        endingmsg                                   // Message to display
    );

    mvwprintw
    (   win,
        2,                                   // Center vertically
        width/2-8,           // Center horizontally
        "Your score: %d", snake_get_size()                                   // display score
    );

    mvwprintw(win,3, width/2-(strlen(startagain)/2), startagain);
    mvwprintw(win,4, width/2-(strlen(startgame)/2), startgame);
    mvwprintw(win,5, width/2-(strlen(exitstring)/2), exitstring);
    while(run){
      int inputChar;
      if (inputType == INPUT_TYPE_GAMEPAD) {
          inputChar = getGamepadInput();
      }
      else {
          inputChar = wgetch(win);
      }
        switch (inputChar) {
            case 's':
            case 'S':
                if (D) debug_log("mysnake::startsnakegame", "S pressed to continue game.");
                attroff(A_BLINK);  // Turn off the terminal blinking
                snake_init();
                menu();
                startsnakegame();
                break;
            case 'q':
            case 'Q':
                if (D) debug_log("mysnake::startsnakegame", "Q pressed to quit game.");
                run = FALSE;
                break;
            default:
                if (D) {
                    char str[64];
                    sprintf(str, "Unrecognized character int value= %u", inputChar);
                    debug_log("mysnake::main", str);
                }
                break;
        }
    }
    return;
}


/**
 * Detect and retrieve keypresses without
 * pausing the program.
 *
 * @author Rachel Liang
 * Work based on:
 *      https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input
 */
int kbhit() {
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}


/**
 * Get input from the gamepad.
 *
 * @author Joseph Lumpkin
 */
int getGamepadInput() {
    int returnChar = 0x10; // Default input (invalid)
    memset(input_data, 0, input_size); // Prepare to receive input
    if (poll(inputDevices, 1, DEVICE_FILE_TIMEOUT) > 0) { // If there was input
        if(inputDevices[0].revents) {
            // Read in the input
            ssize_t r = read(inputDevices[0].fd, input_data, input_size);

            // Find what input was entered
            if ((unsigned char)input_data[10] == 0x10) {
                if ((unsigned char)input_data[12] == 0xff) {
                    returnChar = KEY_LEFT;
                }
                else if ((unsigned char)input_data[12] == 0x01) {
                    returnChar = KEY_RIGHT;
                }
            }
            else if ((unsigned char)input_data[10] == 0x11) {
                if ((unsigned char)input_data[12] == 0xff) {
                    returnChar = KEY_UP;
                }
                else if ((unsigned char)input_data[12] == 0x01) {
                    returnChar = KEY_DOWN;
                }
            }
            else if ((unsigned char)input_data[10] == 0x3b) {
                if ((unsigned char)input_data[12] == 0x01) {
                    returnChar = 's';
                }
            }
            else if ((unsigned char)input_data[10] == 0x3a) {
                if ((unsigned char)input_data[12] == 0x01) {
                    returnChar = 's';
                }
            }
        }
    }
    else {
        if (D) debug_log("mysnake::getGamepadInput", "Unable to poll input devices\n");
    }

    return returnChar;
}


/**
 * Check to see if the player has won the game.
 *
 * @author Jiaxin Jiang
 */
bool checkwon() {
    // Return true if size of snake is half of the terminal perimeter
    return (snake_get_size() >= (maxcol + maxrow));
    //return (snake_get_size() >= 10);
}


/**
 * Change the coordinates of the snake head
 * depending on which direction/arrow key has been pressed.
 *
 * @author Rachel Liang
 */
char* updateSnakeHead() {
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


/**
 * Get the size of the terminal
 * and initialize global terminal variables.
 *
 * @author Rachel Liang
 */
void initWindowAttributes() {
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
 *
 * @author Joseph Lumpkin
 */
void sig_int_handler(int sig_num) {
    // Reset handler
    signal(SIGINT, sig_int_handler);
    fflush(stdout);
}

/**
 * Quit the program
 *
 * @author Joseph Lumpkin
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
  * @author Joseph Lumpkin
  *
  * @returns  int - Speed level of the game
  */
  int get_game_speed() {
      int score = snake_get_size();       // Current Score
      int winningScore = maxrow + maxcol; // Winning Score
      if (score >= (winningScore / 3) * 2) {
        return TERTIARY_DELAY;
      }
      else if (score >= winningScore / 3) {
        return SECONDARY_DELAY;
      }
      else {
        return primary_custom;
      }
  }
