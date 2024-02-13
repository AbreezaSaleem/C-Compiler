#include <memory.h>
#include <assert.h>
#include <assert.h>
#include "wordcount-algorithm.h"

char* createWordForNode(char * word, int capacity) {
	char *dest = malloc(capacity + 1);
	memcpy(dest, word, capacity + 1);
	if (dest == NULL) {
		fprintf(stderr, "Error allocating memory to dest\n");
		return "";
	}
	return dest;
}

void runWordCount(char *filename, FILE * outputFile, struct wordCountSummary * summary) {
	assert(summary);
	
	// Read file character by character
	if (filename == NULL) {
		fprintf(stderr, "File name not supplied.\n");
		exit(1);
	}

	char *ext = strrchr(filename, '.');

	if (ext == NULL || strcmp(ext, ".txt") != 0) {
		fprintf(stderr, "File name has no/invalud extension.\n");
		exit(1);
	}

	FILE* inputFile = fopen(filename, "r");
	if (inputFile == NULL) {
		fprintf(stderr, "Error opening file\n");
		exit(1);
	}

	setupWordCountSummary(summary);

	char c;
	int bufferSize = 50;
	int capacity = 0;
	char *buffer = malloc(sizeof(char) * bufferSize);

	if (buffer == NULL) {
		fprintf(stderr, "Error allocating memory to buffer\n");
		exit(1);
	}

	while ((c = fgetc(inputFile)) != EOF) {
		if (capacity >= bufferSize) {
			bufferSize *= 2;
			buffer = (char *) realloc(buffer, sizeof(char) * bufferSize);
		}

		if (c == ' ' || c == '\t' || c == '\n') {
			// create string from buffer
			char *dest = createWordForNode(buffer, capacity);
			if (strlen(dest) > 0) {
				insertNode(dest);
			}

			// clear the buffer
			memset(buffer, 0, bufferSize);
			capacity = 0;
			if (c == '\n') {
				insertNewlineNode();
			}
		} else {
			// add character to buffer
			buffer[capacity] = c;
			capacity++;
		}
	}

	// check if there's a leftover word in the buffer
	if (strlen(buffer) > 0) {
		char *dest = createWordForNode(buffer, capacity);
		if (strlen(dest) > 0) {
			insertNode(dest);
		}
	}


	fprintf(outputFile, "Lines: %d\n", getNewlineCount(summary));
	fprintf(outputFile, "Words: %d\n", getTotalWordCount(summary));
	fprintf(outputFile, "\nUnique words:\n");
	printList(summary, outputFile);


	free(buffer);
	fclose(inputFile);
}
