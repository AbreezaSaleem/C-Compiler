#include <stdio.h>
#include <assert.h>
#include "wordcount-list.h"
#include "wordcount-algorithm.h"

int main(int argc, char **argv) {
    char *filename = argv[argc - 1];
    struct wordCountSummary wcs;
    runWordCount(filename, stdout, &wcs);
    return 0;
}
