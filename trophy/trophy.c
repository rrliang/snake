/*
 ============================================================================
 Name        : trophy.c
 Author      : Rachel Liang
             : Jiaxin Jiang
             : Joseph Lumpkin
 Version     : 1.0
 Copyright   :
 Description : Container for holding values and interacting with values
                associated with trophies.
 ============================================================================
 */

// Library Includes
//****************************
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// Project Includes
//****************************
#include "../snake/snake.h"

// Defines
//****************************
#define trophy_max_value    9

// Attributes
//****************************

/* Trophy Attributes */
int trophy_i;           // Trophy y value on screen
int trophy_j;           // Trophy x value on screen
int trophy_value;       // Trophy score value


/**
 * Check whether or not the snake head has reached the trophy.
 */
bool checktrophy(int currenti, int currentj) {
    // If the snake head coords are same as the trophy coords
    if (currenti == trophy_i && currentj == trophy_j)
       return true;
    return false;
}


/**
 * Generate a new trophy to replace the current trophy.
 * Trophy will have a value (1-9),
 * and be randomly generated somewhere in the snakepit.
 */
void trophygen(int max_row, int max_col)
{
    // Generate a new trophy value
    trophy_value = 1 + rand() % (trophy_max_value); // Range of 1-9
    // Generate a random x coordinate for the trophy
    trophy_i = 1 + (rand() % ((max_row-2)));// Range of 1 to (terminal_width-2)
    // Generate a random y coordinate of the trophy
    trophy_j = 1 + (rand() % ((max_col-2)));// Range of 1 to (terminal_height-2)

    // Check if randomly generated trophy coordinates clash with existing snake
    // head and body if yes, regenerate the random coordinate,
    // otherwise exit loop.
    bool inotsame = false;
    bool jnotsame = false;
    while (inotsame) {
        // Random x coord for trophy does not clash with snake head
        bool isamecurrent = (trophy_i != snake_get_curr_i());
        bool inotsamebody = true;
        for (int i = 0; i < snake_get_size() - 1 ; i++) {
            // If random x coord for trophy is not part of the snake body
            if (trophy_i == snake_get_i_at(i) || trophy_i >= max_row) {
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
        // Random y coord for trophy does not clash with snake head
        bool jsamecurrent = (trophy_j != snake_get_curr_j());
        bool jnotsamebody = true;
        for (int i = 0; i < snake_get_size() - 1 ; i++) {
            // Random y coord for trophy is not part of the snake body
            if (trophy_j == snake_get_j_at(i) || trophy_j >= max_col) {
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

// Accessors and Mutators
//****************************

int trophy_get_i()
{
    return trophy_i;
}

int trophy_set_i()
{
    return trophy_i;
}


int trophy_get_j()
{
    return trophy_j;
}

int trophy_set_j()
{
    return trophy_j;
}


int trophy_get_value()
{
    return trophy_value;
}

int trophy_set_value()
{
    return trophy_value;
}
