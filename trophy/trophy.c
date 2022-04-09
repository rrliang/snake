#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../snake/snake.h"

/* Trophy Attributes */
int trophy_i;           // Trophy y value
int trophy_j;           // Trophy x value
int trophy_value;       // Trophy score value

/* This function will check whether or not the snake head has reached the trophy */
bool checktrophy(int currenti, int currentj) {
    // If the snake head coords are same as the trophy coords
    if (currenti == trophy_i && currentj == trophy_j) 
       return true;
    return false;
}


/* trophy will have a value (1-9), and be randomly generated somewhere in the snakepit*/
void trophygen(int max_row, int max_col)
{
    trophy_value = 1 + rand() % (9 + 1 - 1); //generate a random number from range of 1-9
    trophy_i = 1 + (rand() % ((max_row-2))); //generate a random number for the x coordinate of the trophy from range of 1 to (horizontal length of terminal-1)
    trophy_j = 1 + (rand() % ((max_col-2))); //generate a random number for the y coordinate of the trophy from range of 1 to (vertical length of terminal-1)
    
    /* check if randomly generated trophy coordinates clash with existing snake head and body
       if yes, regenerate the random coordinate, otherwise exit loop */
    
    bool inotsame = false;
    bool jnotsame = false;
    while (inotsame) {
        bool isamecurrent = (trophy_i != snake_get_curr_i()); //random x coord for trophy does not clash with snake head
        bool inotsamebody = true;
        for (int i = 0; i < snake_get_size() - 1 ; i++) {
            if (trophy_i == snake_get_i_at(i) || trophy_i >= max_row) { //random x coord for trophy is not part of the snake body
                inotsamebody = false;
                break;
            }
        }
        if (inotsame && inotsamebody) { 
            inotsame = true;
        } else {
            trophy_i = 1 + (rand() % ((max_row-2))); 
        }
    }

    while (jnotsame) {
        bool jsamecurrent = (trophy_j != snake_get_curr_j()); //random y coord for trophy does not clash with snake head
        bool jnotsamebody = true;
        for (int i = 0; i < snake_get_size() - 1 ; i++) {
            if (trophy_j == snake_get_j_at(i) || trophy_j >= max_col) { //random y coord for trophy is not part of the snake body
                jnotsamebody = false;
                break;
            }
        }
        if (jnotsame && jnotsamebody) {
            jnotsame = true;
        } else {
            trophy_j = 1 + (rand() % ((max_col-2)));
        }
    }
}


int trophy_get_i()
{
    return trophy_i;
}


int trophy_get_j()
{
    return trophy_j;
}


int trophy_get_value()
{
    return trophy_value;
}


int trophy_set_i()
{
    return trophy_i;
}


int trophy_set_j()
{
    return trophy_j;
}


int trophy_set_value()
{
    return trophy_value;
}