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
#include "../debug/debug.h"

// Defines
//****************************
#define TROPHY_VALUE_MAX        9
#define TROPHY_EXPIRATION_MAX   9 // 9 seconds

/* Verbose Debug Logging Flag */
bool trophy_D = false;

// Attributes
//****************************

/* Trophy Attributes */
int trophy_i;           // Trophy y value on screen
int trophy_j;           // Trophy x value on screen
int trophy_value;       // Trophy score value
int trophy_time;        // How long the trophy has been alive in milliseconds
int trophy_expiration;  // Trophy expiration boundary in milliseconds


/**
 * Check whether or not the snake head has reached the trophy.
 *
 * @author Rachel Liang
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
 *
 * @author Joseph Lumpkin
 */
void trophy_gen(int max_row, int max_col, int game_speed)
{
    // Generate a new trophy value
    trophy_value = 1 + rand() % (TROPHY_VALUE_MAX); // Range of 1-9
    // Generate a random x coordinate for the trophy
    trophy_i = 1 + (rand() % ((max_row-2)));// Range of 1 to (terminal_width-2)
    // Generate a random y coordinate of the trophy
    trophy_j = 1 + (rand() % ((max_col-2)));// Range of 1 to (terminal_height-2)
    // Reset the trophy's time alive
    trophy_time = 0;
    // Generate a trophy timeout between 1-9 seconds, as milliseconds
    trophy_expiration = ((rand() % TROPHY_EXPIRATION_MAX)+1) * 1000;
    if (trophy_expiration < 1000) {
      trophy_expiration = 1000;
    }
    if (trophy_D) {
      char str[64];
      sprintf(str, "trophy_expiration = %d milliseconds", trophy_expiration);
      debug_log("trophy::trophy_gen", str);
    }

    // Check if randomly generated trophy coordinates clash with existing snake
    // head and body if yes, regenerate the random coordinate,
    // otherwise exit loop.
    bool spaceTaken = false;

    for (int i = 0; i < snake_get_size() - 1; i++) {
        int snakeX = snake_get_j_at(i);
        int snakeY = snake_get_i_at(i);
        if (trophy_i == snakeY && trophy_j == snakeX) {
            spaceTaken = true;
            break;
        }
    }

    // Check to ensure the generated trophy is obtainable
    int distance, xDistance, yDistance; // Distance from the snake head that
    xDistance = abs(snake_get_curr_j() - trophy_j);
    yDistance = abs(snake_get_curr_i() - trophy_i);
    distance = xDistance + yDistance;
    // Calculate time needed to reach trophy in milliseconds
    int timeNeeded = distance * (game_speed / 1000);
    if (trophy_D) {
      char str[255];
      sprintf(str, "distance = %d, game_speed = %d, timeNeeded = %d, trophy_expiration = %d", distance, game_speed, timeNeeded, trophy_expiration);
      debug_log("trophy::trophy_gen", str);
    }
    // Ensure the generated trophy is valid
    if (spaceTaken || timeNeeded > trophy_expiration)
        trophy_gen(max_row, max_col, game_speed);
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


int trophy_get_expiration()
{
    return trophy_expiration;
}


int trophy_get_time()
{
    return trophy_time;
}

void trophy_set_time(int time)
{
    if (trophy_D) {
      char str[64];
      sprintf(str, "time = %d", time);
      debug_log("trophy::trophy_set_time", str);
    }
    trophy_time = time;
}


int trophy_get_value()
{
    return trophy_value;
}

int trophy_set_value()
{
    return trophy_value;
}
