#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdbool.h>

#define ESCAPE_CHAR_LENGTH 11

char escapeCodes[] = {'a', 'b', 'f', 'n', 'r', 't', 'v', '\'', '\"', '0'};

int escapeCharactersASCII[] = {7, 8, 12, 10, 13, 9, 11, 39, 34, 0};

#endif /* _UTILS_H */
