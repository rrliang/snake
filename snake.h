#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAIN_WIN_COLOR 1
#define DELAY 100000

/**

    YES a lot of the functions have WAY too many params
    If you want to fix it, make the snake a struct and put all the same vars in a struct and initialize it like an object
    Then it reduces the amount of params in the functions

    Also: the reason why I refactored this is because global vars outside of the main function are exploitable and can lead to memory leaks

*/

struct Snake {
    int maxrow; 
    int maxcol; 
    int *snakebodyi;
    int *snakebodyj; 
    int snakesize; 
    int currenti; 
    int currentj; 
    bool running; 
    int trophyi; 
    int trophyj; 
    int trophyval; 
    bool resize; 
    int previoussize;
    int previousi;
    int previousj;
    int previoustrophyi;
    int previoustrophyj;
};

typedef struct Snake snake_t;


int kbhit();
void initboard(snake_t* s); 
bool checkwon(snake_t* s); 
void printsnakebod(snake_t* s); 
bool didsnakehitself(); 
