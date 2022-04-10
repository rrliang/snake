#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>
#include <string.h>

#include "../trophy/trophy.h"
#include "../debug/debug.h"

/* Verbose Debug Logging Flag */
bool snake_D = false;  // Set to true for verbose debug information

/* Snake Attributes */
int snake_size;     // Current size of the snake (Player score)
int current_i;      // Current snake head y value
int current_j;      // Current snake head x value
int previous_i;     // Previous y value for the snake head
int previous_j;     // Previous x value for the snake head
int *snakebodyi;    // Snake body y values : index = distance from head, value = grid coord
int *snakebodyj;    // Snake body x values : index = distance from head, value = grid coord
int counter;        // Current snake body refresh index


void snake_grow() {
    if(snake_D) debug_log("snake::snake_grow", "Growing the snake");
    /*resize the snakebodyi and snakebodyj arrays to the newsize of the snake*/
    int newsize = snake_size + trophy_get_value(); // Newsize of the snake is original size + value of the trophy

    /*resize the snakebodyi and snakebodyj arrays to the newsize of the snake*/
    snakebodyi=realloc(snakebodyi, newsize * sizeof(int));
    if (snakebodyi == NULL)
        return;
    snakebodyj=realloc(snakebodyj, newsize * sizeof(int));
    if (snakebodyj == NULL)
        return;
    for(int i=0; i<(snake_size); i++)
    {
        snakebodyi[i]=snakebodyi[i+1];
        snakebodyj[i]=snakebodyj[i+1];
    }
    // Add the trophy coordinate at the end of the snake body arrays
    snakebodyi[(snake_size-1)] = current_i;
    snakebodyj[(snake_size-1)] = current_j;

    snake_size = newsize;
}


void snake_move() {
    if(snake_D) debug_log("snake::snake_move", "Moving the snake");
    // If the snake body is fully grown (Shows all body segments on screen)
        if (counter == (snake_size-1)) {
            // Move the snake body forward without growth
            for(int i=0; i<snake_size; i++)
            {
                // Shift body values left in their arrays
                snakebodyi[i]=snakebodyi[i+1];
                snakebodyj[i]=snakebodyj[i+1];
            }
            // Add the previous snake head coordinate at the end of the snake body arrays
            snakebodyi[(snake_size-1)] = previous_i;
            snakebodyj[(snake_size-1)] = previous_j;
        }
        // Else the snake body array has not been filled yet
        else
        {
            // Grow the snake by filling up the array with the previous coordinates one at a time
            snakebodyi[counter] = previous_i;
            snakebodyj[counter] = previous_j;
            counter++;  // Increment snake size counter
        }
}


/* prints the snake body onto screen */
void snake_print() {
    if(snake_D) {
        debug_log("snake::snake_print", "Printing the snake");
        char str[25];
        sprintf(str, "%d", snake_size);
        strcat(str, " = snake_size.");
        debug_log("snake::snake_print", str);
    }

    for (int i = (snake_size-1); i >= 0; i--) {
        if (snakebodyi[i] != 0 && snakebodyj[i] != 0) {
            if(snake_D) debug_log("snake::snake_print", "Able to print snake body.");
            mvprintw(snakebodyi[i], snakebodyj[i], "o");
        }
        else
            if(snake_D) debug_log("snake::snake_print", "Unable to print snake body.");
    }
}

/* check to see if the snake hit itself */
bool snake_did_snake_hit_self() {
    if(snake_D) debug_log("snake::snake_did_snake_hit_self", "Checking if the snake hit itself.");
    for (int i = 0; i < (snake_size-1); i++) {
        if (snakebodyi[i] == current_i && snakebodyj[i] == current_j) { //if the head of the snake is in the same
            return true;
        }
    }
    return false;
}


/* initialize the snakebody arrays */
void snake_init() {
    snake_size = 5;
    if(snake_D) debug_log("snake::snake_init", "Initializing the snake body arrays.");
    snakebodyi = malloc(snake_size * sizeof(int));
    if (snakebodyi == NULL)
        return;

    snakebodyj = malloc(snake_size * sizeof(int));
    if (snakebodyi == NULL)
        return;
}


void snake_reinitialize() {
    if(snake_D) debug_log("snake::snake_reinitialize", "Reinitializing the snake body arrays.");
    counter = 0;
    snakebodyi=realloc(snakebodyi, snake_size * sizeof(int));
        if (snakebodyi == NULL)
            return;

        snakebodyj=realloc(snakebodyj, snake_size * sizeof(int));
        if (snakebodyj == NULL)
            return;
}


int snake_get_size() {
    return snake_size;
}


void snake_set_size(int size) {
    snake_size = size;
}


int snake_get_curr_i() {
    return current_i;
}


void snake_set_curr_i(int i) {
    current_i = i;
}


void snake_set_curr_j(int j) {
    current_j = j;
}


int snake_get_curr_j() {
    return current_j;
}


int snake_get_i_at(int index) {
    if(snake_D) debug_log("snake::snake_get_i_at", "Retrieving snake i coord.");
    int *ptr = snakebodyi;
    for (int i = 0; i < index; i++) {
        ptr += 1;
    }
    return *ptr;
}


int snake_get_j_at(int index) {
    if(snake_D) debug_log("snake::snake_get_j_at", "Retrieving snake j coord.");
    int *ptr = snakebodyj;
    for (int i = 0; i < index; i++) {
        ptr += 1;
    }
    return *ptr;
}


void snake_free_i_body() {
    if(snake_D) debug_log("snake::snake_free_i_body", "Freeing the snakebody's i array.");
    free(snakebodyi);
}


void snake_free_j_body() {
    if(snake_D) debug_log("snake::snake_free_i_body", "Freeing the snakebody's j array.");
    free(snakebodyj);
}


int snake_get_previous_i() {
    return previous_i;
}


int snake_get_previous_j() {
    return previous_j;
}


void snake_set_previous_i(int value) {
    previous_i = value;
}


void snake_set_previous_j(int value) {
    previous_j = value;
}

/**
 * Convert the Snake Head, and both body arrays to
 * a string and return the string's pointer.
 *
 * @return
 */
// char* snake_to_string() {
//     char *snake_body_i[snake_size * 3];
//     char *snake_body_j[snake_size * 3];
//
//     int index = 0;
//     for (int i=0; i < snake_size; i++) {
//         index += sprintf(&snake_body_i[index], "%d", snakebodyi[i]);
//         index += sprintf(&snake_body_i[index], "%c", ',');
//         index += sprintf(&snake_body_i[index], "%c", ' ');
//     }
//
//     return *snake_body_i;
// }
