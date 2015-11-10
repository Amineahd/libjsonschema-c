/**
 * @file utils.h
 * @author Amine Aouled Hamed
 * @date 04 Nov. 2015
 * @brief Utility functions for the schema validator
 * 
 * @see http://json-schema.org/latest/json-schema-validation.html
 */

#include <stdio.h>
#include <string.h>
#include "json.h"
#include "linkhash.h"


//terminal colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**
 * @brief Prints a colored message in the terminal.
 */
void
printf_colored(char* color,char* buff);

/**
 * @brief Sorts items of a JSON array. copied from test1.c
 */
int 
sort_fn (const void *j1, const void *j2);
