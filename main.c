#include "snake.h"


int main(int argc, char **argv){
    /**
        My goal here is to have AS little code in the main method as possible
		I'll do a second refactor and put the logic in more methods
		First I want to understand what it's doing so I know what to name the methods
    */

    char* endingmsg;

    srand(time(NULL)); 
    /*  Create and initialize window  */
    initscr();
    WINDOW * win;
    win = initscr();
    cbreak();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);

	// create instance of snake agent
	snake_t *s = initSnake(); // you can modify the init snake method to take params for what u want
    initboard(s);
    initsnakebodarrs(s);

    // put in method call it switchKeys()
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

    // check if terminal has colors
    if(has_colors() == FALSE){
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
    s->previoussize = 0;
    s->resize = false;

    while (1) {
        werase(win);
        wattron(win, COLOR_PAIR(2));
        box(win, 0, 0);
        if(checkwon(s)) {
            endingmsg = "YOU WIN!";
            break;
        }
        if (totcounter == -1) {
            refresh();
            trophygen(s);
        } else if (totcounter == rand() % (100-1 + 1 - 10) + 10) {
            refresh();
            trophygen(s);
            totcounter = 0;
        }

        wattron(win, COLOR_PAIR(3));
        mvprintw(s->trophyi, s->trophyj,"%d",s->trophyval);
        refresh();
        
        if (s->currenti == 0 || s->currentj == 0 || s->currenti == s->maxrow-1 || s->currentj == s->maxcol-1) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";
            break;
        }
        s->previousi = s->currenti;
        s->previousj = s->currentj;
        checktrophy(s);


        wattron(win, COLOR_PAIR(1));
        mvprintw(s->currenti, s->currentj, initialize(inputChar, s));
        
        refresh();
            
        if (kbhit()) {
            previousChar = inputChar;
            inputChar = getch();
            if (s->currenti == 0 || s->currentj == 0 || s->currenti == s->maxrow || s->currentj == s->maxcol) {
                endingmsg = "YOU LOST BECAUSE YOU RAN INTO THE BORDER!";
                break;
            }
            refresh();
        } else {
            refresh();
        }
        if (s->counter == (s->snakesize-1)) {
            for(i=0; i<(s->snakesize); i++)
            {
                s->snakebodyi[i]=s->snakebodyi[i+1];
                s->snakebodyj[i]=s->snakebodyj[i+1];
            }
            s->snakebodyi[(s->snakesize-1)] = s->previousi;
            s->snakebodyj[(s->snakesize-1)] = s->previousj;
            refresh();

        } else {
            s->snakebodyi[s->counter] = s->previousi;
            s->snakebodyj[s->counter] = s->previousj;
            s->counter++;
            refresh();
        }
        printsnakebod(s);
        if (didsnakehitself(s)) {
            endingmsg = "YOU LOST BECAUSE YOU RAN INTO YOURSELF!";
            break;
        }
        refresh();
        usleep(DELAY- (s->snakesize * 1000));
        totcounter++;
    }
    wattron(win, COLOR_PAIR(2));
    werase(win);
    printw("%s",endingmsg);
    getch();

    /*  Clean up and exit  */
    delwin(win);
    endwin();
    refresh();
    
    return EXIT_SUCCESS;
	


}
