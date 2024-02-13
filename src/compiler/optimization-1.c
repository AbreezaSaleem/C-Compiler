#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "mips.h"
#include "type.h"
#include "helpers/type-tree.h"
#include "ir.h"
#include "optimization-1.h"
#include "helpers/util-functions.h"
#include "helpers/hash-map.h"

extern struct hash_map *const_registers_map;
extern struct ir_list *ir_list;
extern int next_temporary;

static void ir_update_const_value(struct ir_instruction *instruction, int new_value) {
	assert(IR_CONST_INT == instruction->kind);
	ir_operand_number(instruction, 1, new_value);
	ir_update_constants_map(instruction->operands[0].data.temporary, new_value);
}

static struct ir_instruction *ir_create_shift_instruction(
	struct ir_instruction *original_instruction, struct ir_instruction *old_instruction,
	int constant, int is_add, enum ir_instruction_kind kind
) {
	struct ir_instruction *const_instruction = ir_instruction(IR_CONST_INT);
	ir_operand_temporary(const_instruction, 0);
	ir_operand_number(const_instruction, 1, mylog2(constant));
	ir_insert(old_instruction, const_instruction, NULL);

	struct ir_instruction *shift_instruction = ir_instruction(kind);
	ir_operand_temporary(shift_instruction, 0);
	ir_operand_copy(shift_instruction, 1, &original_instruction->operands[1]);
	ir_operand_copy(shift_instruction, 2, &const_instruction->operands[0]);
	ir_insert(const_instruction, shift_instruction, NULL);

	struct ir_instruction *add_instruction = ir_instruction(is_add ? IR_ADD_SIGNED_WORD : IR_SUB_SIGNED_WORD);
	ir_operand_temporary(add_instruction, 0);
	ir_operand_copy(add_instruction, 1, &old_instruction->operands[0]);
	ir_operand_copy(add_instruction, 2, &shift_instruction->operands[0]);
	ir_insert(shift_instruction, add_instruction, old_instruction);
	
	return add_instruction;
}

static void ir_change_instruction_to_shift(struct ir_instruction *instruction, enum ir_instruction_kind new_kind) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	instruction->kind = new_kind;
}

static void ir_convert_mult_to_shift(struct ir_instruction *instruction, int constant) {
	/* find nearest power of two and adjust the remaining with add/sub */
	int power_of_two = find_nearest_power_of_two(constant);
	int remaining = abs(constant - power_of_two);
	bool is_add = power_of_two < constant;
	ir_change_instruction_to_shift(instruction, IR_LEFT_SHIFT_WORD);
	struct ir_instruction *instruction_1 = ir_get_instruction_by_temporary(
		instruction->operands[2].data.temporary
	);
	ir_update_const_value(instruction_1, mylog2(power_of_two));
	struct ir_instruction *old_instruction = instruction;
	/*
	 * Here we apply the same left-shift logic to the leftover of 2^n
	 * we do this until we reach 0
	 * for example: a*50 => (a*64) - (a*14) => (a << 6) - (a*16 - (a*2)) => (a << 6) - ((a << 4) - (a << 1))
	*/
	while (remaining > 0) {
		power_of_two = find_nearest_power_of_two(remaining);
		remaining = abs(remaining - power_of_two);
		if (!power_of_two) break;
		struct ir_instruction *new_instruction = ir_create_shift_instruction(
			instruction,
			old_instruction,
			power_of_two,
			is_add,
			IR_LEFT_SHIFT_WORD
		);
		old_instruction = new_instruction;
		is_add = true;
	}
}

static void ir_perform_identity_sub_optimization(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	bool is_unsigned = IR_SUB_UNSIGNED_WORD == instruction->kind;

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_1 && NULL == constant_2) {
		int constant = atoi(constant_1);

		/* remove a +/- 0 */
		if (constant == 0) {
			ir_remove_instruction_from_list(instruction);
		}
	} 
	if (NULL != constant_2 && NULL == constant_1) {
		int constant = atoi(constant_2);

		/* remove a +/- 0 */
		if (constant == 0) {
			ir_remove_instruction_from_list(instruction);
		}
	}
	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		unsigned int constant_u = atoi(constant_1) - atoi(constant_2);
		int constant_s = atoi(constant_1) - atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, is_unsigned ? constant_u : constant_s);
	}
}

static void ir_perform_identity_add_optimization(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_1 && NULL == constant_2) {
		int constant = atoi(constant_1);

		/* remove a +/- 0 */
		if (constant == 0) {
			ir_remove_instruction_from_list(instruction);
		}
	} 
	if (NULL != constant_2 && NULL == constant_1) {
		int constant = atoi(constant_2);

		/* remove a +/- 0 */
		if (constant == 0) {
			ir_remove_instruction_from_list(instruction);
		}
	}
	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		int constant = atoi(constant_1) + atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, constant);
	}

}

static void ir_perform_identity_multiply_optimization(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	bool is_unsigned = IR_MULT_UNSIGNED_WORD == instruction->kind;

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_1 && NULL == constant_2) {
		int constant = atoi(constant_1);

		/* change a*2 => a << 1 */
		if (is_power_of_two(constant)) {
			ir_change_instruction_to_shift(instruction, IR_LEFT_SHIFT_WORD);
			struct ir_instruction *instruction_1 = ir_get_instruction_by_temporary(
				instruction->operands[2].data.temporary
			);
			ir_update_const_value(instruction_1, mylog2(constant));
			return;
		} else {
			/* find nearest power of two and adjust the remaining with add/sub */
			ir_convert_mult_to_shift(instruction, constant);
			return;
		}

		/* remove a*1 */
		if (constant == 1) {
			ir_remove_instruction_from_list(instruction);
			return;
		}
	} 
	if (NULL != constant_2 && NULL == constant_1) {
		int constant = atoi(constant_2);

		/* change a*2 => a << 1 */
		if (is_power_of_two(constant)) {
			instruction->kind = IR_LEFT_SHIFT_WORD;
			struct ir_instruction *instruction_1 = ir_get_instruction_by_temporary(
				instruction->operands[2].data.temporary
			);
			ir_update_const_value(instruction_1, mylog2(constant));
			return;
		} else {
			/* find nearest power of two and adjust the remaining with add/sub */
			ir_convert_mult_to_shift(instruction, constant);
			return;
		}

		/* remove a*1 */
		if (constant == 1) {
			ir_remove_instruction_from_list(instruction);
			return;
		}
	}
	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		unsigned int constant_u = atoi(constant_1) * atoi(constant_2);
		int constant_s = atoi(constant_1) * atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, is_unsigned ? constant_u : constant_s);
	}

	return;
}

static void ir_perform_identity_gt_optimization(struct ir_instruction *instruction) {
	assert(NULL != instruction);

	bool is_unsigned = IR_GT_UNSIGNED_WORD == instruction->kind || IR_GE_UNSIGNED_WORD == instruction->kind;
	bool is_equal = IR_GE_UNSIGNED_WORD == instruction->kind || IR_GE_SIGNED_WORD == instruction->kind;

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		unsigned int constant_u = is_equal ? atoi(constant_1) >= atoi(constant_2) : atoi(constant_1) > atoi(constant_2);
		int constant_s = is_equal ? atoi(constant_1) >= atoi(constant_2) : atoi(constant_1) > atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, is_unsigned ? constant_u : constant_s);
	}
}

static void ir_perform_identity_lt_optimization(struct ir_instruction *instruction) {
	assert(NULL != instruction);

	bool is_unsigned = IR_LT_UNSIGNED_WORD == instruction->kind || IR_LE_UNSIGNED_WORD == instruction->kind;
	bool is_equal = IR_LE_UNSIGNED_WORD == instruction->kind || IR_LE_SIGNED_WORD == instruction->kind;

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		unsigned int constant_u = is_equal ? atoi(constant_1) <= atoi(constant_2) : atoi(constant_1) < atoi(constant_2);
		int constant_s = is_equal ? atoi(constant_1) <= atoi(constant_2) : atoi(constant_1) < atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, is_unsigned ? constant_u : constant_s);
	}
}

static void ir_perform_identity_divide_optimization(struct ir_instruction *instruction) {
	assert(NULL != instruction);

	bool is_unsigned = IR_DIV_UNSIGNED_WORD == instruction->kind;

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_1 && NULL == constant_2) {
		int constant = atoi(constant_1);

		/* change a/2 => a >> 1 */
		if (is_unsigned) {
			if (is_power_of_two(constant)) {
				instruction->kind = IR_RIGHT_SHIFT_UNSIGNED_WORD;
				struct ir_instruction *instruction_1 = ir_get_instruction_by_temporary(
					instruction->operands[1].data.temporary
				);
				ir_update_const_value(instruction_1, mylog2(constant));
			}
		}

		/* change a/1 => a */
		if (constant == 1) {
			ir_remove_instruction_from_list(instruction);
		}
	}

	if (NULL != constant_2 && NULL == constant_1) {
		int constant = atoi(constant_2);

		/* change a/2 => a >> 1 */
		if (is_unsigned) {
			if (is_power_of_two(constant)) {
				instruction->kind = IR_RIGHT_SHIFT_UNSIGNED_WORD;
				struct ir_instruction *instruction_1 = ir_get_instruction_by_temporary(
					instruction->operands[2].data.temporary
				);
				ir_update_const_value(instruction_1, mylog2(constant));
			}
		}

		/* change a/1 => a */
		if (constant == 1) {
			ir_remove_instruction_from_list(instruction);
		}
	}

	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		unsigned int constant_u = atoi(constant_1) / atoi(constant_2);
		int constant_s = atoi(constant_1) / atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, is_unsigned ? constant_u : constant_s);
	}
}

static void ir_perform_identity_remainder_optimization(struct ir_instruction *instruction) {
	assert(IR_REM_SIGNED_WORD == instruction->kind || IR_REM_UNSIGNED_WORD == instruction->kind);

	bool is_unsigned = IR_REM_UNSIGNED_WORD == instruction->kind;

	const char *constant_1 = ir_get_const_map_entry(instruction->operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(instruction->operands[2].data.temporary);

	if (NULL != constant_1 && NULL == constant_2) {
		int constant = atoi(constant_1);
		/* change a%1 => 0 */
		if (constant == 1) {
			instruction->kind = IR_CONST_INT;
			ir_update_const_value(instruction, 0);
		}
	}

	if (NULL != constant_2 && NULL == constant_1) {
		int constant = atoi(constant_2);
		/* change a%1 => 0 */
		if (constant == 1) {
			instruction->kind = IR_CONST_INT;
			ir_update_const_value(instruction, 0);
		}
	}

	if (NULL != constant_2 && NULL != constant_1) {
		/* you don't need these constants now */
		ir_mark_constant_as_obselete(instruction->operands[1].data.temporary);		
		ir_mark_constant_as_obselete(instruction->operands[2].data.temporary);
	
		/* just compute the expression */
		unsigned int constant_u = atoi(constant_1) % atoi(constant_2);
		int constant_s = atoi(constant_1) % atoi(constant_2);
		instruction->kind = IR_CONST_INT;
		ir_update_const_value(instruction, is_unsigned ? constant_u : constant_s);
	}

}

static void ir_use_offset_for_load_store(struct ir_instruction *instruction, bool is_load) {
	assert(NULL != instruction);

	/* traverse from here till you find the addressOf for the variable you're trying to load
	 * check if that register is used anywhere else
	 * if it is, then you can't use the offset 
	 */
	int temp = instruction->operands[is_load].data.temporary;
	bool can_use_offset = true;
	struct ir_list *iter = NULL;

	for (
		iter = ir_list;
		iter->next->data->position != instruction->position;
		iter = iter->next
	);

	for (; iter != NULL; iter = iter->prev) {
		int kind = iter->data->kind;

		if (IR_ADDRESS_OF == iter->data->kind && temp == iter->data->operands[0].data.temporary) {
			break;
		}

		for (int i = 0; i < 6; i++) {
			if (iter->data->operands[i].kind == OPERAND_TEMPORARY) {
				/* return + result instruction do use the 0th temporary! */
				if (i == 0) {
					if (
						IR_RESULT_WORD == kind ||
						IR_RESULT_HALF_WORD == kind ||
						IR_RESULT_BYTE == kind ||
						IR_RETURN_WORD == kind ||
						IR_RETURN_HALF_WORD == kind ||
						IR_RETURN_BYTE == kind ||
						IR_STORE_WORD == kind ||
						IR_STORE_BYTE == kind ||
						IR_STORE_HALF_WORD == kind
					) {
						if (iter->data->operands[i].data.temporary == temp) {
							can_use_offset = false;
							break;
						}
					}
				}
				if (iter->data->operands[i].data.temporary == temp) {
					can_use_offset = false;
					break;
				}
			}
		}
	}

	if (can_use_offset) {
		/* mark the addressOf instruction as obselete */
		assert(NULL != iter);
		assert(IR_ADDRESS_OF == iter->data->kind);
		iter->data->is_used = false;
		instruction->offset_variable = iter->data;
	} else {
		iter->data->is_used = true;
		instruction->offset_variable = NULL;
	}
}

static void ir_perform_optimizations(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	switch (instruction->kind) {
			case IR_ADD_SIGNED_WORD:
			case IR_ADD_UNSIGNED_WORD:
				ir_perform_identity_add_optimization(instruction);
				break;
			case IR_SUB_SIGNED_WORD:
			case IR_SUB_UNSIGNED_WORD:
				ir_perform_identity_sub_optimization(instruction);
				break;
			case IR_MULT_SIGNED_WORD:
			case IR_MULT_UNSIGNED_WORD: 
				ir_perform_identity_multiply_optimization(instruction);
				break;
			case IR_DIV_SIGNED_WORD: 
			case IR_DIV_UNSIGNED_WORD: 
				ir_perform_identity_divide_optimization(instruction);
				break;
			case IR_GT_SIGNED_WORD:
			case IR_GT_UNSIGNED_WORD:
			case IR_GE_SIGNED_WORD:
			case IR_GE_UNSIGNED_WORD:
				ir_perform_identity_gt_optimization(instruction);
				break;
			case IR_LT_SIGNED_WORD:
			case IR_LT_UNSIGNED_WORD:
			case IR_LE_SIGNED_WORD:
			case IR_LE_UNSIGNED_WORD:
				ir_perform_identity_lt_optimization(instruction);
				break;
			case IR_REM_SIGNED_WORD:
			case IR_REM_UNSIGNED_WORD:
				ir_perform_identity_remainder_optimization(instruction);
				break;
			case IR_LOAD_WORD:
			case IR_LOAD_HALF_WORD:
			case IR_LOAD_SIGNED_HALF_WORD:
			case IR_LOAD_BYTE:
			case IR_LOAD_SIGNED_BYTE:
				ir_use_offset_for_load_store(instruction, true);
				break;
			case IR_STORE_WORD:
			case IR_STORE_HALF_WORD:
			case IR_STORE_BYTE:
				ir_use_offset_for_load_store(instruction, false);
				break;
			default:
				break;
	}
}

void ir_start_optimization_1(void) {
	struct ir_list *iter = NULL;
	struct ir_instruction *instruction_iter = NULL;

	/* iterate ir_list */
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		assert(NULL != iter->data);
		instruction_iter = iter->data;
		while (NULL != instruction_iter) {
			ir_perform_optimizations(instruction_iter);

			instruction_iter = instruction_iter->next;
		}
	}

	// ir_mark_registers_as_dead_or_live();

	/* cleanup */
	iter = NULL;
	free(iter);
	instruction_iter = NULL;
	free(instruction_iter);
}
