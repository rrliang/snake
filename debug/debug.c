/*
 ============================================================================
 Name        : debug.c
 Author      : Rachel Liang
             : Jiaxin Jiang
             : Joseph Lumpkin
 Version     : 1.0
 Copyright   :
 Description : A class used to print debug information to a log file.
 ============================================================================
 */

// Library Includes
//****************************
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define DEBUG_LOG   "debug/log.txt"

/**
 * Log a message to the log file.
 * Ex:
 *     debug_log("testtag:", "Test message");
 *
 * @param tag       - Label identifying the calling function.
 *                      (Usually the file::functionname)
 * @param message   - Message to print to the log file.
 */
void debug_log(char* tag, char* message) {
    // Attempt to create a new log file
    FILE *log_file;
    log_file = fopen(DEBUG_LOG, "a");

    // Print the tag to the to the log file
    fwrite(tag, sizeof(char), strlen(tag), log_file);
    // Print a space on the same line
    fwrite(": ", sizeof(char), strlen(": "), log_file);
    // Print the message to the log file
    fwrite(message, sizeof(char), strlen(message), log_file);
    // New line
    fwrite("\n", sizeof(char), strlen("\n"), log_file);
    fclose(log_file); // Close the log file process
}


/**
 * Clear the log file in preparation
 * for a new run of the program.
 */
void debug_clear_log() {
    // Attempt to create a new log file
    FILE *log_file;
    log_file = fopen(DEBUG_LOG, "w");
    // Open the log file and clear all contained information
    fwrite(" ", sizeof(char), strlen(""), log_file);
    fclose(log_file); // Close the log file process
}
