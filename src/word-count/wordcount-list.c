#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <stdbool.h>
#include "wordcount-algorithm.h"

struct wordCountSummary *head = NULL;
struct wordCountSummary *current = NULL;
struct wordCountSummary *previous = NULL;

void setupWordCountSummary(struct wordCountSummary * node) {
	node->word = NULL;
	node->count = 0;
	node->next = NULL;

	head = node;
}

struct wordCountSummary * createNode(char * word) {
	struct wordCountSummary * newNode = malloc(sizeof(struct wordCountSummary));

	newNode->word = word;

	return newNode;
}

void insertNewlineNode() {
	/**
	 * As an optimazation, I will keep count of newlines in the Head node
	 * */
	head->count += 1;
}

bool isNewWordGreater(char * newWord, char * currentWord) {
	if (newWord == NULL || currentWord == NULL) {
		fprintf(stderr, "Error: Comparing word is NULL\n");
		exit(1);
	}
	return strcmp(newWord, currentWord) > 0;
}

void insertNode(char *word) {
	/*
	Traverse the linked list to check if a wordCountSummary with the given word already exists
	Note that we're not adding nodes to the Head because that's reserved for the line count
	*/

	if (head == NULL) {
		fprintf(stderr, "Error: Head node is NULL\n");
		exit(1);
	}

	current = head->next;
	previous = head;

	while (current != NULL) {
		/**
		 * CASE: The word already exists in the linked list
		 * Increment the count and exit the loop
		 */
		if (current->word != NULL && strcmp(current->word, word) == 0) {
			current->count += 1;
			return;
		} else {
			if (isNewWordGreater(word, current->word)) {
				/**
				 * CASE: Current wordCountSummary's word is NOT equal to new wordCountSummary's word
				 * Continue traversing the linked list
				 */
				previous = current;
				current = current->next;
			} else {
				/**
				 * CASE: Insert the word here
				 */
				struct wordCountSummary *newNode = createNode(word);
				previous->next = newNode;
				newNode->count = 1;
				newNode->next = current;
				return;
			}
		}
	}

	/**
	 * CASE: You've reached the end of the linked list
	 * Insert the new wordCountSummary at the end of the linked list and set count to 1
	 */
	struct wordCountSummary *newNode = createNode(word);
	previous->next = newNode;
	newNode->count = 1;
	newNode->next = NULL;
}

int getTotalWordCount(struct wordCountSummary * node) {
	struct wordCountSummary * current = node;
	int total = 0;

	if (current == NULL) {
		printf("Total words are 0\n");
		return 0;
	}

	do {
		// Head node will always contain the line count
		if (current->word != NULL && current->count > 0) {
			total += current->count;
		}
		current = current->next;
	} while (current != NULL);

	free(current);
	return total;
}

int getNewlineCount(struct wordCountSummary * node) {
	struct wordCountSummary * current = node;
	int total = 0;
	if (current == NULL) {
		printf("Total words are 0\n");
		return 0;
	} else {
		total = current->count;
	}

	return total + 1; // offsetting this to 1 because the current counting algorithm doesn't count the first line
}

void printList(struct wordCountSummary * node, FILE * outputFile) {
	struct wordCountSummary * current = node;
	if (current->next == NULL) {
		printf("List is empty!\n");
		return;
	}

	do {
		// Head node will always contain the line count
		if (current->word != NULL) {
			fprintf(outputFile, "%s: %d\n", current->word, current->count);
		}
		current = current->next;
	} while (current != NULL);
	free(current);
}

