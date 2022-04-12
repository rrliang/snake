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
#include <stdbool.h>

// Prototype Functions
//****************************
bool checktrophy(int currenti, int currentj);
void trophy_gen(int max_row, int max_col);

/* Accessors and Mutators */
int trophy_get_i();
int trophy_get_j();
int trophy_get_expiration();
int trophy_get_time();
void trophy_set_time(int);
int trophy_get_value();
