# Define the compiler and compiler flags
CC = gcc
CFLAGS = -Wall

# Define the target executable
TARGET = compiler

# Define the source files
SRCS = compiler.c parser.tab.c scanner.yy.c node.c symbol.c type.c ir.c mips.c helpers/hash-map.c optimization-1.c optimization-2.c

# Define the lex and yacc commands
LEX = lex
YACC = bison
YFLAGS = -d -Wcounterexamples -Wall

# Define the lex and yacc source files
LEX_SRC = scanner.lex
YACC_SRC = parser.y

# Define the output files of lex and yacc
LEX_OUT = scanner.yy.c
YACC_OUT = parser.tab.c parser.tab.h

# Rules to build the target executable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -ll -o $(TARGET)

# Rules to build the lex and yacc sources
$(YACC_OUT): $(YACC_SRC)
	$(YACC)  $(YACC_SRC) $(YFLAGS)

$(LEX_OUT): $(LEX_SRC)
	$(LEX) -o $(LEX_OUT) $(LEX_SRC)

# Phony targets
.PHONY: clean

# Clean up generated files and the target executable
clean:
	rm -f $(TARGET) $(LEX_OUT) $(YACC_OUT)
