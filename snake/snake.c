#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ncurses.h>

#include "../trophy/trophy.h"

int snake_size;     // Current size of the snake (Player score)
int current_i;      // Current snake head y value
int current_j;      // Current snake head x value
int previous_i;     // Previous y value for the snake head
int previous_j;     // Previous x value for the snake head
int *snakebodyi;    // Snake body y values : index = distance from head, value = grid coord
int *snakebodyj;    // Snake body x values : index = distance from head, value = grid coord
int counter;        // Current snake body refresh index


void snake_grow() {
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
    // If the snake body is fully grown (Shows all body segments on screen)
        if (counter == (snake_size-1)) {
            // Move the snake body forward without growth
            for(int i=0; i<(snake_size); i++)
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
    for (int i = (snake_size-1); i > 0; i--) {
        if (snakebodyi[i] != 0 && snakebodyj[i] != 0) {
            mvprintw(snakebodyi[i], snakebodyj[i], "o");
            char text[29];
            sprintf(text, "%d %d\n", snakebodyi[i], snakebodyi[i]);
        }
    }
}

/* check to see if the snake hit itself */
bool snake_did_snake_hit_self() {
    for (int i = 0; i < (snake_size-1); i++) {
        if (snakebodyi[i] == current_i && snakebodyj[i] == current_j) { //if the head of the snake is in the same 
            return true;
        }
    }
    return false;
}


/* initialize the snakebody arrays */
void initsnakebodarrs() {
    snakebodyi = malloc((snake_size-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
    
    snakebodyj = malloc((snake_size-1)*sizeof(int *));
    if (snakebodyi == NULL)
        return;
}


void snake_reinitialize() {
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
    int *ptr = snakebodyi;
    for (int i = 0; i < index; i++) {
        ptr += 1;
    }
    return *ptr;
}


int snake_get_j_at(int index) {
    int *ptr = snakebodyj;
    for (int i = 0; i < index; i++) {
        ptr += 1;
    }
    return *ptr;
}


void snake_free_i_body() {
    free(snakebodyi);
}


void snake_free_j_body() {
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