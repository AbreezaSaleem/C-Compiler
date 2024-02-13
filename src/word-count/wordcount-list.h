#ifndef WORDCOUNTLIST_H
#define WORDCOUNTLIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct wordCountSummary {
	char * word;
	int count;
	struct wordCountSummary * next;
};

struct wordCountSummary *  createNode(char * word);
void setupWordCountSummary(struct wordCountSummary * node);
void insertNode(char * word);
void insertNewlineNode();
int getTotalWordCount(struct wordCountSummary * node);
int getNewlineCount(struct wordCountSummary * node);
void printList(struct wordCountSummary * node, FILE * outputFile);

#endif //WORDCOUNTLIST_H

