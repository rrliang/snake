#include "snake.h"


snake_t* initSnake(){
    // allocate space and return one snake
    snake_t *newSnake;
    newSnake = (snake_t*)malloc(sizeof(snake_t));
    newSnake->snakesize = 5; // this is what you initalized it to in the main
    newSnake->currenti = 1; // this is what you initalized it to in the main
    newSnake->currentj = 1; // this is what you initalized it to in the main
    newSnake->running = true; 
    newSnake->counter = 0; 
    newSnake->count = 0; 
    return newSnake; 
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


char* initialize(int inputChar, snake_t* s) {
    char* c = ">";
    switch(inputChar) {
        case KEY_UP:
            s->currenti -= 1;
            c = "^";
            break;
        case KEY_DOWN:
            s->currenti += 1;
            c = "V";
            break;
        case KEY_LEFT:
            s->currentj -= 1;
            c = "<";
            break;
        case KEY_RIGHT:
            s->currentj += 1;
            c = ">";
            break;
    }
    return c;
}

void initsnakebodarrs(snake_t* s) {
    s->snakebodyi = malloc((s->snakesize-1)*sizeof(int *));
    if (s->snakebodyi == NULL)
        return;
    
    s->snakebodyj = malloc((s->snakesize-1)*sizeof(int *));
    if (s->snakebodyi == NULL)
        return;
}


void trophygen(snake_t* s){
    s->trophyval = 1 + rand() % (9 + 1 - 1); //generate a random number from range of 1-9
    s->trophyi = 1 + (rand() % ((s->maxrow-2))); //generate a random number for the x coordinate of the trophy from range of 1 to (horizontal length of terminal-1)
    s->trophyj = 1 + (rand() % ((s->maxcol-2))); //generate a random number for the y coordinate of the trophy from range of 1 to (vertical length of terminal-1)
    
    /* check if randomly generated trophy coordinates clash with existing snake head and body
       if yes, regenerate the random coordinate, otherwise exit loop */
    
    bool inotsame = false;
    bool jnotsame = false;
    while (inotsame) {
        bool isamecurrent = (s->trophyi != s->currenti); //random x coord for trophy does not clash with snake head
        bool inotsamebody = true;
        for (int i = 0; i < s->snakesize-1 ; i++) {
            if (s->trophyi == s->snakebodyi[i] || s->trophyi >= s->maxrow) { //random x coord for trophy is not part of the snake body
                inotsamebody = false;
                break;
            }
        }
        if (inotsame && inotsamebody) { 
            inotsame = true;
        } else {
            s->trophyi = 1 + (rand() % ((s->maxrow-2))); 
        }
    }

    while (jnotsame) {
        bool jsamecurrent = (s->trophyj != s->currentj); //random y coord for trophy does not clash with snake head
        bool jnotsamebody = true;
        for (int i = 0; i < s->snakesize-1 ; i++) {
            if (s->trophyj == s->snakebodyj[i] || s->trophyj >= s->maxcol) { //random y coord for trophy is not part of the snake body
                jnotsamebody = false;
                break;
            }
        }
        if (jnotsame && jnotsamebody) {
            jnotsame = true;
        } else {
            s->trophyj = 1 + (rand() % ((s->maxcol-2)));
        }
    }

}


void checktrophy(snake_t* s) {
    if (s->currenti == s->trophyi && s->currentj == s->trophyj) { //if snake head coords are same as the trophy coords
        int newsize = s->snakesize + s->trophyval; //newsize of the snake is original size + value of the trophy
        
        // previousi = trophyi;
        // previousj = trophyj;
        
        /*resize the snakebodyi and snakebodyj arrays to the newsize of the snake*/
        s->snakebodyi=realloc(s->snakebodyi, newsize * sizeof(int));
        if (s->snakebodyi == NULL)
            return;
        
        s->snakebodyj=realloc(s->snakebodyj, newsize * sizeof(int));
        if (s->snakebodyj == NULL)
            return;

        s->count = 0;
        s->previoustrophyi = s->trophyi;
        s->previoustrophyj = s->trophyj;        
        refresh();
        trophygen(s);
        refresh();
        s->previoussize = s->snakesize;
        s->snakesize = newsize;
        s->resize = true;
    } else {
        refresh();
        s->resize = false;
    }
}








