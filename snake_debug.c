#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define DEBUG_LOG   "log.txt"

/**
 * Log a supplied string to the log.txt file.
 * Flags: 
 *   'w' = Overwrite
 *   'a' = Append
 */
void snake_log(char* str, char flag) {
    // Attempt to create a new log file
    FILE *log_file;
    log_file = fopen(DEBUG_LOG, &flag);

    fwrite(str, sizeof(char), strlen(str), log_file);

    fclose(log_file);
}