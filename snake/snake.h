/*
 ============================================================================
 Name        : snake.c
 Author      : Rachel Liang
             : Jiaxin Jiang
             : Joseph Lumpkin
 Version     : 1.0
 Copyright   :
 Description : Container for holding values and interacting with values
                associated with the snake.
 ============================================================================
 */

// Library Includes
//****************************
#include <stdbool.h>

// Function Prototypes
//****************************
bool snake_did_snake_hit_self();
void snake_free_i_body();
void snake_free_j_body();
void snake_grow();
void snake_init();
void snake_move();
void snake_print();

/* Accessors and Mutators */
int snake_get_i_at(int index);
int snake_get_j_at(int index);

int snake_get_curr_i();
void snake_set_curr_i(int i);

int snake_get_curr_j();
void snake_set_curr_j(int j);

int snake_get_size();
void snake_set_size(int size);

void snake_set_counter(int index);
int snake_get_counter();

int snake_get_previous_i();
void snake_set_previous_i(int value);

int snake_get_previous_j();
void snake_set_previous_j(int value);
