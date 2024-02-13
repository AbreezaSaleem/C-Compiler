#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "mips.h"
#include "type.h"
#include "helpers/type-tree.h"
#include "ir.h"

extern struct hash_map *const_registers_map;
extern struct ir_list *ir_list;

bool optimization_applied = false;

static void ir_remove_dead_label_uncalled(struct ir_instruction *instruction) {
	assert(IR_GOTO_IF_FALSE == instruction->kind || IR_GOTO_IF_TRUE == instruction->kind);

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[0].data.temporary);

	struct ir_list *iter = NULL;

	if (NULL != constant_1) {
		int constant = atoi(constant_1);

		if (IR_GOTO_IF_FALSE == instruction->kind && constant == 1) {
			char label_scope[strlen(instruction->operands[0].data.name) + 1];
			strcpy(label_scope, instruction->operands[1].data.name);
			
			/* the label mentioned in this instruction will never be called */
			/* remove all instructions in that block */
			bool instructions_deleted = false;
			for (iter = ir_list; NULL != iter; iter = iter->next) {
				if (strcmp(iter->data->scope, label_scope) == 0) {
					ir_remove_instruction_from_list(iter->data);
					instructions_deleted = true;
				}
			}
			ir_remove_instruction_from_list(instruction);
			optimization_applied = instructions_deleted;

			/* cleanup */
			iter = NULL;
			free(iter);
		} else {
			/* remove true block */
			/* for this, you remove */
			char label_scope[strlen(instruction->operands[0].data.name) + 5 + 1];  /* 5 is coming from '_true' */
			strcpy(label_scope, instruction->operands[1].data.name);
			strcat(label_scope, "_true");

			bool instructions_deleted = false;
			for (iter = ir_list; NULL != iter; iter = iter->next) {
				if (strcmp(iter->data->scope, label_scope) == 0) {
					ir_remove_instruction_from_list(iter->data);
					instructions_deleted = true;
				}
			}
			ir_remove_instruction_from_list(instruction);
			optimization_applied = instructions_deleted;
		}
	}

	/* cleanup */
	iter = NULL;
	free(iter);
}

static void ir_remove_dead_label(struct ir_instruction *instruction) {
	assert(IR_LABEL == instruction->kind);
	/* remove label block if its got no goto statement */
	struct ir_list *iter = NULL;

	bool goto_found = false;
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		/* search entire list for a goto_statement */
		if (
			(IR_GOTO == iter->data->kind && strcmp(iter->data->operands[0].data.name, instruction->operands[0].data.name) == 0) ||
			(IR_GOTO_IF_FALSE == iter->data->kind && strcmp(iter->data->operands[1].data.name, instruction->operands[0].data.name) == 0) ||
			(IR_GOTO_IF_TRUE == iter->data->kind && strcmp(iter->data->operands[1].data.name, instruction->operands[0].data.name) == 0)
		) {
			goto_found = true;
			break;
		}
	}

	if (!goto_found) {
		/* remove all instructions in that block */
		char label_scope[strlen(instruction->scope) + 1];
		strcpy(label_scope, instruction->scope);
		for (iter = ir_list; NULL != iter; iter = iter->next) {
			if (strcmp(iter->data->scope, label_scope) == 0) {
				ir_remove_instruction_from_list(iter->data);
			}
		}
		optimization_applied = true;
	}

	/* cleanup */
	iter = NULL;
	free(iter);
}

static void ir_remove_dead_goto(struct ir_instruction *instruction) {
	assert(IR_GOTO == instruction->kind);
	assert(NULL != ir_list);
	struct ir_instruction *next_instruction = ir_get_next_instruction(instruction);
	assert(NULL != next_instruction);

	/* remove both goto and label if they immediately follow eachother */
	if (IR_LABEL == next_instruction->kind) {
		/* compare labels */
		if (strcmp(instruction->operands[0].data.name, next_instruction->operands[0].data.name) == 0) {
			/* remove both instructions */
			ir_remove_instruction_from_list(instruction);
			ir_remove_instruction_from_list(next_instruction);

			optimization_applied = true;
		}
	}
}

static void ir_remove_unused_const(struct ir_instruction *instruction) {
	assert(IR_CONST_INT == instruction->kind);
	if (!instruction->is_used) {
		ir_remove_instruction_from_list(instruction);
		optimization_applied = true;
	}
}

static void ir_perform_optimizations(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	switch (instruction->kind) {
		case IR_GOTO:
			ir_remove_dead_goto(instruction);
			break;
		case IR_LABEL:
			ir_remove_dead_label(instruction);
			break;
		case IR_GOTO_IF_FALSE: 
		case IR_GOTO_IF_TRUE: 
			ir_remove_dead_label_uncalled(instruction);
			break;
		case IR_CONST_INT: 
			ir_remove_unused_const(instruction);
			break;
		default:
			break;
	}
}

void ir_start_optimization_round(void) {
	struct ir_list *iter = NULL;
	struct ir_instruction *instruction_iter = NULL;

	/* iterate ir_list */
	optimization_applied = false;
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		assert(NULL != iter->data);
		instruction_iter = iter->data;
		ir_perform_optimizations(instruction_iter);
		if (optimization_applied) break;
	}

	/* cleanup */
	iter = NULL;
	free(iter);
	instruction_iter = NULL;
	free(instruction_iter);
}

void ir_start_optimization_2(void) {

	ir_start_optimization_round();

	while (optimization_applied) {
		ir_start_optimization_round();
	}
}
