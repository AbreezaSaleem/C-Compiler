#ifndef WORDCOUNT_WORDCOUNT_H
#define WORDCOUNT_WORDCOUNT_H
#include <stdio.h>
#include "wordcount-list.h"

char* createWordForNode(char * word, int capacity);
void runWordCount(char *filename, FILE * outputFile, struct wordCountSummary * summary);

#endif //WORDCOUNT_WORDCOUNT_H
