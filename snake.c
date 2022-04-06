#include "snake.h"


snake_t* initSnake(){
    // allocate space and return one snake
}

int kbhit() {
    //https://stackoverflow.com/questions/448944/c-non-blocking-keyboard-input (thanks for including this)
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

void initboard(snake_t* s) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    s->maxrow = w.ws_row; 
    s->maxcol = w.ws_col; 
    s->currenti = s->maxrow/2; 
    s->currentj = s->maxcol/2; 
}

bool checkwon(snake_t* s) {
    return (s->snakesize >= ((s->maxcol+s->maxrow)*2)/2); //return true if size of snake is half of the terminal perimeter
}

void printsnakebod(snake_t* s) {
    for (int i = (s->snakesize-1); i > 0; i--) {
        mvprintw(s->snakebodyi[i], s->snakebodyj[i], "o");
    }
}

bool didsnakehitself(snake_t* s) {
    for (int i = 0; i < (s->snakesize-1); i++) {
        if (s->snakebodyi[i] == s->currenti && s->snakebodyj[i] == s->currentj) {
            return true;
        }
    }
    return false;
}






