#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define MAIN_WIN_COLOR 1
#define DELAY 100000

// snake struct
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
    int counter;
    int count;
};

typedef struct Snake snake_t;

snake_t* initSnake(); 
int kbhit();
void initboard(snake_t* s); 
bool checkwon(snake_t* s); 
void printsnakebod(snake_t* s); 
bool didsnakehitself(); 
char* initialize(int inputChar, snake_t* s); 
void initsnakebodarrs(snake_t* s); 
void trophygen(snake_t* s); 
void checktrophy(snake_t* s) ; 
