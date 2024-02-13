/* https://www.scaler.com/topics/stack-in-c/ */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../symbol.h"

// N will be the capacity of the Static Stack
#define N 15

// Initializing the top of the stack to be -1
int top = -1;

// Initializing the stack using an array
struct symbol_table *symbol_table_stack[N];

// Function prototypes
void push_to_stack(struct symbol_table *symbol_table);
struct symbol_table* pop_from_stack();
struct symbol_table* peek_into_stack();
bool is_stack_empty();
bool is_stack_full();

void push_to_stack(struct symbol_table *symbol_table) {
	// Checking overflow state
	if (top == N-1)
		printf("Overflow State: can't add more elements into the stack\n");
	else {
		top+=1;
		symbol_table_stack[top] = symbol_table;
	}
}

struct symbol_table* pop_from_stack(){
	// Checking underflow state
	if(top == -1)
		printf("Underflow State: Stack already empty, can't remove any element\n");
	else{
		struct symbol_table *symbol_table = symbol_table_stack[top];
		top-=1;
		return symbol_table;
	}
	return NULL;
}

struct symbol_table* peek_into_stack(){
	return symbol_table_stack[top];
}

bool is_stack_empty(){
	if(top == -1){
		printf("Stack is empty: Underflow State\n");
		return true;
	}
	printf("Stack is not empty\n");
	return false;
}

bool is_stack_full(){
	if(top == N-1){
		printf("Stack is full: Overflow State\n");
		return true;
	}
	printf("Stack is not full\n");
	return false;
}
