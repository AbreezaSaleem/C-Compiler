#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "mips.h"
#include "helpers/hash-map.h"

#define REG_EXHAUSTED   -1

#define FIRST_USABLE_REGISTER  8
#define LAST_USABLE_REGISTER  23
#define NUM_REGISTERS         32

extern struct symbol_table *file_scope_table;

extern int optimization;

static char* registers_list[] = {
	"", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"
};

static char* arguments_registers_list[] = {
	"", "a0", "a1", "a2", "a3"
};

const int T_REGISTERS_SIZE = 40;
const int S_REGISTERS_SIZE = 32;
const int A_REGISTERS_SIZE = 16;
const int RA_REGISTER_SIZE = 4;
const int OLD_FP_REGISTER_SIZE = 4;
const int STACK_SIZE =
	T_REGISTERS_SIZE + S_REGISTERS_SIZE + A_REGISTERS_SIZE +
	RA_REGISTER_SIZE + OLD_FP_REGISTER_SIZE;

int fp_offset_low = 0;
int fp_offset_high = 0;
int total_stack_size = STACK_SIZE;
int register_count = 0;
int arg_register_count = 0;
int fp_position = 0;
int alignment_word_added = 0;

/* temporary - register map */
struct hash_map *temp_register_map;

/* variable - address map */
struct hash_map *variable_address_map;

/* consts registers map */
extern struct hash_map *const_registers_map;


/****************************
 * MIPS TEXT SECTION OUTPUT *
 ****************************/

bool mips_is_optimization_low() {
	if (!optimization) return false;
	return optimization >= 1;
}

bool mips_is_optimization_mid() {
	if (!optimization) return false;
	return optimization >= 2;
}

void mips_initialise_temp_regsiter_map() {
	temp_register_map = initialise_hash_map();
}

void mips_initialise_variable_address_map() {
	variable_address_map = initialise_hash_map();
}

static int mips_round_up_to_double_word_boundry(int size) {
	return ((size+7) >> 3) << 3;
}

static int mips_round_variable_address(int fp_offset, int size) {
	switch (size) {
		case 4: {
			if (fp_offset % 4 == 0) return fp_offset - 4;
			return  (fp_offset >> 2) << 2;
		}
		case 2: {
			if (fp_offset % 2 == 0) return fp_offset - 2;
			return (fp_offset >> 1) << 1;
		}
		default:
			return fp_offset - 1;
	}
}

static void mips_calculate_offset(int size) {
	fp_offset_high = mips_round_variable_address(fp_offset_high, size);
}

static const char* mips_get_next_available_register(int temp_number) {
	int length = (sizeof(registers_list) / sizeof(char*));
	if (register_count >= (length - 1)) {
		assert(0);
	}

	const char* reg = get_value(temp_register_map, temp_number);
	if (NULL != reg) {
		return reg;
	}
	register_count++;
	/* map which temp gets matched to which actual register */
	add_key_value(temp_register_map, registers_list[register_count], temp_number);

	return registers_list[register_count];
}

char* mips_get_next_available_argument_register() {
	if (arg_register_count >= (sizeof(arguments_registers_list) / sizeof(char*))) {
		assert(0);
	}
	char* next_register = (char*)malloc(10); // +1 for null terminator
	next_register[0] = '\0';
	strcat(next_register, "a");
	char next_register_number[30];
	sprintf(next_register_number, "%d", arg_register_count);
	strcat(next_register, next_register_number);
	
	arg_register_count++;
	return next_register;
}

const char* mips_get_register_from_temp(int temp_number) {
	return get_value(temp_register_map, temp_number);  
}

static const char* mips_get_address_of_variable(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	const char *address =
		get_value(variable_address_map, instruction->operands[3].data.variable_id);

	if (NULL == address) {
		/* push to map */
		char * new_address = malloc(30);
		assert(NULL != new_address);
		new_address[0] = '\0';

		if (instruction->operands[5].data.number == -1) {
			/* not an argument - calculate offset from the high end */
			mips_calculate_offset(instruction->operands[4].data.number);
			char fp_offset_high_string[30];
			sprintf(fp_offset_high_string, "%d", fp_offset_high);
			strcat(new_address, fp_offset_high_string);
		} else {
			/* an argument - calculate offset from the low end */
			int argument_offset = 
				(instruction->operands[5].data.number * 4) + ( alignment_word_added ? 4: 0 );
			char fp_offset_low_string[30];
			sprintf(fp_offset_low_string, "%d", argument_offset);
			strcat(new_address, fp_offset_low_string);
		}
	 
		strcat(new_address, "($fp)");
		address = new_address;
		add_key_value(
			variable_address_map,
			address,
			instruction->operands[3].data.variable_id
		);
	}
	return address;
}

static int mips_get_syscall_code(char *name) {
	if (NULL != strstr(name, "print_string")) {
		return 4;
	} else if (NULL != strstr(name, "print_int")) {
		return 1;
	} else if (NULL != strstr(name, "read_int")) {
		return 5;
	} else if (NULL != strstr(name, "read_string")) {
		return 8;
	} else {
		assert(0);
	}
}

static void mips_save_t_registers(FILE *output) {

	if (mips_is_optimization_low()) {
		/* perform optimization - only save the t registers which are being used! */

		/*
		 * in the registers_list array, 9+ indexes contain t_registers
		 * so if register_count > 9, we need to save the t registers
		*/

		int t_register_start = 9;

		while (t_register_start <= register_count) {
			fprintf(output, "\tsw\t$%s, %d($fp)\n", registers_list[t_register_start], fp_offset_low);
			fp_offset_low += 4;

			t_register_start++;
		}
	} else {
		/* save 't' registers */
		for(int i = 0; i < 10; i++) {
			fprintf(output, "\tsw\t$t%d, %d($fp)\n", i, fp_offset_low);
			fp_offset_low += 4;
		}
	}
}

static void mips_restore_t_registers(FILE *output) {

	if (mips_is_optimization_low()) {
		/* perform optimization - only restore the t registers which are being used! */

		/*
		 * in the registers_list array, 9+ indexes contain t_registers
		 * so if register_count > 9, we need to restore the t registers
		*/

		int t_register_start = 9;
		int temp_register_count = register_count;

		while (temp_register_count >= t_register_start) {
			fp_offset_low -= 4;
			fprintf(output, "\tlw\t$%s, %d($fp)\n", registers_list[temp_register_count], fp_offset_low);

			temp_register_count--;
		}
	} else {
		/* restore 't' registers */
		for(int j = 9; j >= 0; j--) {
			fp_offset_low -= 4;
			fprintf(output, "\tlw\t$t%d, %d($fp)\n", j, fp_offset_low);
		}
	}
}

static bool mips_convert_to_branch_instructions(
	FILE *output,
	struct ir_instruction *instruction,
	struct ir_list *ir_list
) {
	// get type of parent instruction
	struct ir_instruction parent_insrtuction = *ir_list->prev->data;
	enum ir_instruction_kind parent_kind = parent_insrtuction.kind;

	/*
		IR_LT_SIGNED_WORD,
		IR_LT_UNSIGNED_WORD,
		IR_LE_SIGNED_WORD,
		IR_LE_UNSIGNED_WORD,
		IR_GE_SIGNED_WORD,
		IR_GE_UNSIGNED_WORD,
		IR_GT_SIGNED_WORD,
		IR_GT_UNSIGNED_WORD,
		IR_EQ_WORD,
		IR_NE_WORD
	*/

	static char *opcodes[] = {
	  "ble",       /* branch on less than signed */
	  "bltu",      /* branch on less than unsigned */
	  "ble",       /* branch on less than equal signed */
	  "bleu",      /* branch on less than equal unsigned */
	  "bge",       /* branch on great than equal signed */
	  "bgeu",      /* branch on great than equal unsigned */
	  "bgt",       /* branch on great than unsigned */
	  "bgtu",      /* branch on great than unsigned */
	  "beq",       /* branch on equal */
	  "bne"        /* branch on not equal */
	};

	static char *opcodes_immediate[] = {
	  "bltz",      /* branch on less than zero */
	  "bltz",      /* branch on less than zero */
	  "blez",      /* branch on less than equal zero */
	  "blez",      /* branch on less than equal zero */
	  "bgez",      /* branch on great than equal zero */
	  "bgez",      /* branch on great than equal zero */
	  "bgtz",      /* branch on great than zero */
	  "bgtz",      /* branch on great than zero */
	  "beqz",      /* branch on equal zero */
	  "bnez",      /* branch on not equal zero */
	};

	/* check if any value in comparison is a 0 */
	const char *constant_1 = ir_get_const_map_entry(parent_insrtuction.operands[1].data.temporary);
	const char *constant_2 = ir_get_const_map_entry(parent_insrtuction.operands[2].data.temporary);

	if (NULL != constant_1 && NULL == constant_2) {
		/* constant_1 is a constant and constant_2 is not */
		int constant = atoi(constant_1);
		if (constant == 0) {
			fprintf(output, "\t%s\t$%s, %s\n",
				/* subtracting 22 to remove the offset comning from ir_instruction_kind */
				opcodes_immediate[parent_kind - 22],
				mips_get_register_from_temp(parent_insrtuction.operands[2].data.temporary),
				instruction->operands[1].data.name
			);
			return true;
		}
	} else if (NULL != constant_2 && NULL == constant_1) {
		/* constant_2 is a constant and constant_1 is not */
		int constant = atoi(constant_2);
		if (constant == 0) {
			fprintf(output, "\t%s\t$%s, %s\n",
				/* subtracting 22 to remove the offset comning from ir_instruction_kind */
				opcodes_immediate[parent_kind - 22],
				mips_get_register_from_temp(parent_insrtuction.operands[1].data.temporary),
				instruction->operands[1].data.name
			);
			return true;
		}
	}

	fprintf(output, "\t%s\t$%s, $%s, %s\n",
	  /* subtracting 22 to remove the offset comning from ir_instruction_kind */
	  opcodes[parent_kind - 22],
	  mips_get_register_from_temp(parent_insrtuction.operands[2].data.temporary),
	  mips_get_register_from_temp(parent_insrtuction.operands[1].data.temporary),
	  instruction->operands[1].data.name
	);

	return true;
}

static bool mips_convert_to_immediate_instructions(FILE *output, struct ir_instruction *instruction) {
	assert(NULL != instruction);

	static char *opcodes[] = {
		"", "", "", "", "", "", "", "", "", "", "", "", "",
		"addiu",
		"", "", "", "", "", "", "", "", "slti", "sltiu", "", "", "", "", "", "", "", "", "",
		"xori", "ori"
	};

	if (
		IR_ADD_SIGNED_WORD == instruction->kind ||
		IR_LT_SIGNED_WORD == instruction->kind ||
		IR_LT_UNSIGNED_WORD == instruction->kind ||
		IR_BITWISE_XOR_WORD == instruction->kind ||
		IR_BITWISE_OR_WORD == instruction->kind
	) {
		int temp_1 = instruction->operands[1].data.temporary;
		int temp_2 = instruction->operands[2].data.temporary;

		bool is_temp_1_const = NULL != ir_get_const_map_entry(temp_1);
		bool is_temp_2_const = NULL != ir_get_const_map_entry(temp_2);

		if (is_temp_1_const && is_temp_2_const) {
			/* both are constants */
			// !! TODO
			fprintf(output,
				"\tli\t$%s, %lu\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				instruction->operands[1].data.number + instruction->operands[2].data.number
			);
			return true;
		} else if (is_temp_1_const) {
			/* temp_1 is constant */
			fprintf(output,
				"\t%s\t$%s, $%s, %s\n",
				opcodes[instruction->kind],
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(temp_2),
				ir_get_const_map_entry(temp_1)
			);
			return true;
		} else if (is_temp_2_const) {
			/* temp_2 is constant */
			fprintf(output,
				"\t%s\t$%s, $%s, %s\n",
				opcodes[instruction->kind],
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(temp_1),
				ir_get_const_map_entry(temp_2)
			);
			return true;
		} else {
			return false;
		}
	}

	return false;
}

static bool mips_use_load_offset(FILE *output, struct ir_instruction *instruction) {
	assert(NULL != instruction);

	if (NULL == instruction->offset_variable) return false;

	switch (instruction->kind) {
		case IR_LOAD_WORD:
		case IR_LOAD_HALF_WORD:
		case IR_LOAD_SIGNED_HALF_WORD:
		case IR_LOAD_BYTE:
		case IR_LOAD_SIGNED_BYTE: {
			fprintf(output,
				"\tlw\t$%s, %s\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_address_of_variable(instruction->offset_variable)
			);
			return true;
		}
		case IR_STORE_WORD:
		case IR_STORE_HALF_WORD:
		case IR_STORE_BYTE: {
			fprintf(output,
				"\tsw\t$%s, %s\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_address_of_variable(instruction->offset_variable)
			);
			return true;
		}
		default:
			return false;
	}
	return false;
}

static bool mips_use_store_offset(FILE *output, struct ir_instruction *instruction) {
	assert(NULL != instruction);

	if (NULL == instruction->offset_variable) return false;
	switch(instruction->kind) {
		case IR_STORE_WORD: {
			fprintf(output,
				"\tsw\t$%s, %s\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_address_of_variable(instruction->offset_variable)
			);
			return true;
		}
		case IR_STORE_HALF_WORD: {
			fprintf(output,
				"\tsh\t$%s, %s\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_address_of_variable(instruction->offset_variable)
			);
			return true;
		}
		case IR_STORE_BYTE: {
			fprintf(output,
				"\tsb\t$%s, %s\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_address_of_variable(instruction->offset_variable)
			);
			return true;
		}
		default:
			assert(0);
			return false;
	}
	return false;
}

static char* mips_get_static_word_type(enum type_basic_kind type_kind) {
	char* str = malloc(7);
	assert(NULL != str);

	switch (type_kind) {
		case TYPE_BASIC_CHAR:
			strcpy(str, "byte");
			break;
		case TYPE_BASIC_SHORT:
			strcpy(str, "half");
			break;
		case TYPE_BASIC_INT:
		case TYPE_BASIC_LONG:
			strcpy(str, "word");
			break;
		default:
			assert(0);
			break;
	}

	return str;
}

void mips_print_temporary_operand(FILE *output, struct ir_operand *operand) {
	assert(OPERAND_TEMPORARY == operand->kind);

	fprintf(output, "%8s%02d", "$", operand->data.temporary + FIRST_USABLE_REGISTER);
}

void mips_print_number_operand(FILE *output, struct ir_operand *operand) {
	assert(OPERAND_NUMBER == operand->kind);

	fprintf(output, "%10lu", operand->data.number);
}

void mips_print_arithmetic(FILE *output, struct ir_instruction *instruction) {

	/*
		IR_MULT_SIGNED_WORD,
		IR_MULT_UNSIGNED_WORD,
		IR_DIV_SIGNED_WORD,
		IR_DIV_UNSIGNED_WORD,
		IR_ADD_SIGNED_WORD,
		IR_ADD_UNSIGNED_WORD,
		IR_SUB_SIGNED_WORD,
		IR_SUB_UNSIGNED_WORD,
		IR_REM_SIGNED_WORD,
		IR_REM_UNSIGNED_WORD,
		IR_LEFT_SHIFT_WORD,
		IR_RIGHT_SHIFT_SIGNED_WORD,
		IR_RIGHT_SHIFT_UNSIGNED_WORD
	*/

	static char *opcodes[] = {
		"mul",      /* mult signed */
		"mulou",    /* mult unsigned */
		"div",      /* divide signed */
		"divu",     /* divide unsigned */
		"addu",     /* add unsigned */
		"addu",     /* add unsigned */
		"sub",      /* sub signed */
		"subu",     /* sub unsigned */
		"rem",      /* rem signed */
		"remu",     /* rem unsigned */
		"sll",      /* left shift */
		"sra",      /* right shift signed */
		"srl"       /* right shift unsigned */
	};

	bool optimization_applied = false;
	if (mips_is_optimization_low()) {
		optimization_applied = mips_convert_to_immediate_instructions(output, instruction);
	}
	
	if (!optimization_applied) {
		fprintf(output, "\t%s\t$%s, $%s, $%s\n",
			/* subtracting 9 to remove the offset comning from ir_instruction_kind */
			opcodes[instruction->kind - 9],
			mips_get_next_available_register(instruction->operands[0].data.temporary),
			mips_get_register_from_temp(instruction->operands[1].data.temporary),
			mips_get_register_from_temp(instruction->operands[2].data.temporary)
		);
	}
}

static int mips_get_max_block_size(struct symbol_table *table, int size) {
	if (NULL == table->children->self) return mips_round_up_to_double_word_boundry(size);

	struct symbol_table_children *children_iter;
	struct symbol_list *children_iter_variables_iter;
	int max_block_size = _SIGNED_LONG_MIN;

	for (children_iter = table->children; children_iter != NULL; children_iter = children_iter->next) {
		int block_size = 0;
		for (
			children_iter_variables_iter = children_iter->self->variables;
			children_iter_variables_iter != NULL;
			children_iter_variables_iter = children_iter_variables_iter->next) {
			block_size += type_get_basic_size(children_iter_variables_iter->symbol.type_tree->type);
		}
		if (block_size > max_block_size) max_block_size = block_size;
	}

	/* cleanup */
	children_iter = NULL;
	free(children_iter);
	children_iter_variables_iter = NULL;
	free(children_iter_variables_iter);

	return mips_round_up_to_double_word_boundry(size + max_block_size);
}

void mips_print_load_address(FILE *output, struct ir_instruction *instruction) {
	assert(IR_ADDRESS_OF == instruction->kind);

	bool is_string = instruction->operands[2].data.is_string;

	if (is_string) {
		fprintf(output,
			"\tla\t$%s, %s\n",
			mips_get_next_available_register(instruction->operands[0].data.temporary),
			instruction->operands[2].data.name
		);
	} else {
		bool is_global = instruction->operands[6].data.number;
		if (is_global) {
			fprintf(output,
				"\tla\t$%s, %s\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				instruction->operands[1].data.name
			);
		} else {
			fprintf(output,
				"\tla\t$%s, %s\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_address_of_variable(instruction)
			);
		}
	}
}

void mips_print_load_immediate(FILE *output, struct ir_instruction *instruction) {
	assert(IR_CONST_INT == instruction->kind);

	fprintf(output,
		"\tli\t$%s, %lu\n",
		mips_get_next_available_register(instruction->operands[0].data.temporary),
		instruction->operands[1].data.number
	);
}

void mips_print_store_word(FILE *output, struct ir_instruction *instruction) {
	assert(
		IR_STORE_WORD == instruction->kind ||
		IR_STORE_HALF_WORD == instruction->kind ||
		IR_STORE_BYTE == instruction->kind
	);

	bool optimization_applied = false;
	if (mips_is_optimization_low()) {
		optimization_applied = mips_use_store_offset(output, instruction);
	}

	if (optimization_applied == 1) return;

	switch(instruction->kind) {
		case IR_STORE_WORD: {
			fprintf(output,
				"\tsw\t$%s, 0($%s)\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_register_from_temp(instruction->operands[0].data.temporary)
			);
			break;
		}
		case IR_STORE_HALF_WORD: {
			fprintf(output,
				"\tsh\t$%s, 0($%s)\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_register_from_temp(instruction->operands[0].data.temporary)
			);
			break;
		}
		case IR_STORE_BYTE: {
			fprintf(output,
				"\tsb\t$%s, 0($%s)\n",
				mips_get_register_from_temp(instruction->operands[1].data.temporary),
				mips_get_register_from_temp(instruction->operands[0].data.temporary)
			);
			break;
		}
		default:
			assert(0);
			break;
	}
}

void mips_print_load_word(FILE *output, struct ir_instruction *instruction) {
	assert(
		IR_LOAD_WORD == instruction->kind ||
		IR_LOAD_HALF_WORD == instruction->kind ||
		IR_LOAD_SIGNED_HALF_WORD == instruction->kind ||
		IR_LOAD_BYTE == instruction->kind ||
		IR_LOAD_SIGNED_BYTE == instruction->kind
	);

	bool optimization_applied = false;
	if (mips_is_optimization_low()) {
		optimization_applied = mips_use_load_offset(output, instruction);
	}

	if (optimization_applied) return;

	switch(instruction->kind) {
		case IR_LOAD_WORD: {
			fprintf(output,
				"\tlw\t$%s, 0($%s)\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			break;
		}
		case IR_LOAD_HALF_WORD:
		case IR_LOAD_SIGNED_HALF_WORD: {
			fprintf(output,
				"\tlh\t$%s, 0($%s)\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			break;
		}
		case IR_LOAD_BYTE:
		case IR_LOAD_SIGNED_BYTE: {
			fprintf(output,
				"\tlb\t$%s, 0($%s)\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			break;
		}
		default:
			assert(0);
			break;
	}
}

void mips_print_cast(FILE *output, struct ir_instruction *instruction) {
	assert(NULL != instruction);

	/*
		IR_CAST_WORD_TO_HALF_WORD,
		IR_CAST_WORD_TO_BYTE,
		IR_CAST_HALF_WORD_TO_BYTE,
		IR_CAST_UNSIGNED_HALF_WORD_TO_WORD,
		IR_CAST_SIGNED_HALF_WORD_TO_WORD,
		IR_CAST_UNSIGNED_BYTE_TO_HALF_WORD,
		IR_CAST_SIGNED_BYTE_TO_HALF_WORD,
		IR_CAST_UNSIGNED_BYTE_TO_WORD,
		IR_CAST_SIGNED_BYTE_TO_WORD
	*/

	switch(instruction->kind) {
		case IR_CAST_WORD_TO_HALF_WORD:
		case IR_CAST_WORD_TO_BYTE:
		case IR_CAST_HALF_WORD_TO_BYTE: {
			fprintf(output, "\tmove\t$%s, $%s\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			break;
		}
		case IR_CAST_UNSIGNED_HALF_WORD_TO_WORD: {
			fprintf(output, "\tandi\t$%s, $%s, 0xff\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			break;
		}
		case IR_CAST_UNSIGNED_BYTE_TO_HALF_WORD:
		case IR_CAST_UNSIGNED_BYTE_TO_WORD: {
			fprintf(output, "\tandi\t$%s, $%s, 0xffff\n",
				mips_get_next_available_register(instruction->operands[0].data.temporary),
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			break;
		}
		case IR_CAST_SIGNED_HALF_WORD_TO_WORD: {
			const char* next_register = mips_get_next_available_register(instruction->operands[0].data.temporary);
			fprintf(output, "\tsll\t$%s, $%s, 16\n",
				next_register,
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			fprintf(output, "\tsra\t$%s, $%s, 16\n",
				next_register, next_register
			);
			break;
		}
		case IR_CAST_SIGNED_BYTE_TO_HALF_WORD: {
			const char* next_register = mips_get_next_available_register(instruction->operands[0].data.temporary);
			fprintf(output, "\tsll\t$%s, $%s, 8\n",
				next_register,
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			fprintf(output, "\tsra\t$%s, $%s, 8\n",
				next_register, next_register
			);
			break;
		}
		case IR_CAST_SIGNED_BYTE_TO_WORD: {
			const char* next_register = mips_get_next_available_register(instruction->operands[0].data.temporary);
			fprintf(output, "\tsll\t$%s, $%s, 24\n",
				next_register,
				mips_get_register_from_temp(instruction->operands[1].data.temporary)
			);
			fprintf(output, "\tsra\t$%s, $%s, 24\n",
				next_register, next_register
			);
			break;
		}
		default:
			assert(0);
			break;
	}
}

void mips_print_unary_operaions(FILE *output, struct ir_instruction *instruction, struct ir_list *ir_list) {
	assert(NULL != instruction);

	/*
		IR_UNARY_MINUS,
		IR_UNARY_LOGICAL_NEGATION,
		IR_UNARY_BITWISE_NEGATION
	*/

	if (IR_UNARY_LOGICAL_NEGATION == instruction->kind) {
		fprintf(output, "\tsleu\t$%s, $%s, $0\n",
			mips_get_next_available_register(instruction->operands[0].data.temporary),
			mips_get_register_from_temp(instruction->operands[1].data.number)
		);
	} else {
		static char *opcodes[] = {
			"neg",      /* negate value */
			"",         /* handling logical negation separately */
			"not",      /* not */
		};

		fprintf(output, "\t%s\t$%s, $%s\n",
			/* subtracting 35 to remove the offset comning from ir_instruction_kind */
			opcodes[instruction->kind - 35],
			mips_get_next_available_register(instruction->operands[0].data.temporary),
			mips_get_register_from_temp(instruction->operands[1].data.temporary)
		);
	}
}

void mips_print_return_word(FILE *output, struct ir_instruction *instruction, struct ir_list *ir_list) {
	assert(
		IR_RETURN_WORD == instruction->kind ||
		IR_RETURN_HALF_WORD == instruction->kind ||
		IR_RETURN_BYTE == instruction->kind
	);

	assert(NULL != ir_list->prev);

	fprintf(output,
		"\tmove\t$v0, $%s\n",
		mips_get_register_from_temp(instruction->operands[0].data.temporary)
	);
	return;

	/* not using this code for now */
	if(IR_CONST_INT == ir_list->prev->data->kind) {
		fprintf(output,
			"\tmove\t$v0, $%s\n",
			mips_get_register_from_temp(instruction->operands[0].data.temporary)
		);
		return;
	}

	switch(instruction->kind) {
		case IR_RETURN_WORD: {
			fprintf(output,
				"\tlw\t$v0, 0($%s)\n",
				mips_get_register_from_temp(instruction->operands[0].data.temporary)
			);
			break;
		}
		case IR_RETURN_HALF_WORD: {
			fprintf(output,
				"\tlh\t$v0, 0($%s)\n",
				mips_get_register_from_temp(instruction->operands[0].data.temporary)
			);
			break;
		}
		case IR_RETURN_BYTE: {
			fprintf(output,
				"\tlb\t$v0, 0($%s)\n",
				mips_get_register_from_temp(instruction->operands[0].data.temporary)
			);
			break;
		}
		default:
			assert(0);
			break;
	}

}

void mips_print_result(FILE *output, struct ir_instruction *instruction) {
	assert(
		IR_RESULT_WORD == instruction->kind ||
		IR_RESULT_HALF_WORD == instruction->kind ||
		IR_RESULT_BYTE == instruction->kind
	);

	/* move contents of v0 in a new register */
	fprintf(output,
		"\tmove\t$%s, $v0\n",
		mips_get_next_available_register(instruction->operands[0].data.temporary)
	);
}

void mips_print_function(FILE *output, struct ir_instruction *instruction) {
	assert(IR_PROC_BEGIN == instruction->kind);

	mips_initialise_temp_regsiter_map();
	mips_initialise_variable_address_map();

	fprintf(output, "%s:\n", instruction->operands[0].data.name);

	/* push frame pointer back */
	struct symbol_table *table = symbol_get_procedure_table(instruction->operands[1].data.name);

	assert(NULL != table);

	/* iterate the table variables and sum the type sizes */
	int size = 0;
	struct symbol_list *iter;
	
	/* create pair of (variable,size) so that we can save them in the stack */
	int initial_size = 30;
	char** variables_names = malloc(sizeof(char) * initial_size);
	int* variables_sizes = malloc(sizeof(int) * initial_size);
	int variables_count = 0;

	for (iter = table->variables; iter != NULL; iter = iter->next) {
		int variable_size = type_get_basic_size(iter->symbol.type_tree->type);
		size += variable_size;

		/* populate pairs */
		if (variables_count > initial_size) {
			initial_size *= 2;
			variables_names = malloc(sizeof(char) * initial_size);
			variables_sizes = malloc(sizeof(int) * initial_size);
		}
		variables_names[variables_count] = iter->symbol.name;
		variables_sizes[variables_count] = variable_size;
		variables_count++;
	}

	int final_size = mips_get_max_block_size(table, size);
	
	total_stack_size = STACK_SIZE + final_size;

	/* reset the offset since we've entered a new function */
	fp_offset_low = 0;
	fp_offset_high = total_stack_size;

	/* push space for our stack frame onto the stack */
	fprintf(output, "\taddiu\t$sp, $sp, -%d\n", total_stack_size);

	/* adjust offset to cater for the double-word boundry padding */
	alignment_word_added = final_size > size;
	fp_offset_low += alignment_word_added == 1 ? 4 : 0;

	/* save the old fp */
	fp_position = fp_offset_low + A_REGISTERS_SIZE + S_REGISTERS_SIZE;
	fprintf(output, "\tsw\t$fp, %d($sp)\n", fp_position);

	/* point frame pointer to new stack pointer */
	fprintf(output, "\tor\t$fp, $sp, $0\n");

	/* save 'a' registers */
	for(int i = 0; i < 4; i++) {
		fprintf(output, "\tsw\t$a%d, %d($fp)\n", i, fp_offset_low);
		fp_offset_low += 4;
	}

	/* save 's' registers */
	for(int j = 0; j < 8; j++) {
		fprintf(output, "\tsw\t$s%d, %d($fp)\n", j, fp_offset_low);
		fp_offset_low += 4;
	}

	/* save 'old fp' registers */
	fp_offset_low += 4;

	/* save ra register */
	fprintf(output, "\tsw\t$ra, %d($fp)\n", fp_offset_low);
	fp_offset_low += 4;

	/* save local variables */
	for(int k = 0; k < variables_count; k++) {
		
	}

	fprintf(output, "\n");

	/* cleanup */
	iter = NULL;
	free(iter);
}

void mips_print_function_reset(FILE *output, struct ir_instruction *instruction) {
	assert(IR_PROC_END == instruction->kind);

	fprintf(output, "\n");

	/* restore 'ra' register */
	fp_offset_low -= 4;
	fprintf(output, "\tlw\t$ra, %d($fp)\n", fp_offset_low);

	fp_offset_low -= 4;

	/* restore 's' registers */
	for(int j = 7; j >= 0; j--) {
		fp_offset_low -= 4;
		fprintf(output, "\tlw\t$s%d, %d($fp)\n", j, fp_offset_low);
	}

	/* restore old fp */
	fprintf(output, "\tlw\t$fp, %d($fp)\n", fp_position);

	/* pop off our stack frame */
	fprintf(output, "\taddiu\t$sp, $sp, %d\n", total_stack_size);

	/* return to caller */
	fprintf(output, "\tjr\t$ra\n");

	free_hash_map(temp_register_map);
	register_count = 0;
	free_hash_map(variable_address_map);
}

void mips_print_function_call(FILE *output, struct ir_instruction *instruction) {
	assert(IR_CALL == instruction->kind);

	/* reset a registers */
	arg_register_count = 0;

	/* save 't' registers */
	mips_save_t_registers(output);

	/* call function */
	fprintf(output, "\tjal\t%s\n", instruction->operands[0].data.name);

	/* restore 't' registers */
	mips_restore_t_registers(output);
}

void mips_print_sys_function_call(FILE *output, struct ir_instruction *instruction) {
	assert(IR_SYS_CALL == instruction->kind);

	fprintf(output, "\n");

	/* call function */
	fprintf(output, "\tli\t$v0, %d\n", mips_get_syscall_code(instruction->operands[0].data.name));
	fprintf(output, "\tsyscall\n");

	/* reset a registers */
	arg_register_count = 0;

	fprintf(output, "\n");
}

void mips_print_parameter(FILE *output, struct ir_instruction *instruction, struct ir_list *ir_list) {
	assert(IR_PARAMETER == instruction->kind);

	struct ir_instruction parent_instruction = *ir_list->prev->data;

	printf("\n");

	if (
		IR_STORE_WORD  == parent_instruction.kind ||
		IR_STORE_HALF_WORD == parent_instruction.kind ||
		IR_STORE_BYTE == parent_instruction.kind
	) {
		fprintf(output,
			"\tlw\t$%s, 0($%s)\n",
			mips_get_next_available_argument_register(),
			mips_get_register_from_temp(instruction->operands[1].data.number)
		);
	} else {
		fprintf(output,
			"\tmove\t$%s, $%s\n",
			mips_get_next_available_argument_register(),
			mips_get_register_from_temp(instruction->operands[1].data.number)
		);
	}
	printf("\n");
}

void mips_print_label(FILE *output, struct ir_instruction *instruction) {
	assert(IR_LABEL == instruction->kind);

	fprintf(output, "\n%s:\n", instruction->operands[0].data.name);
}

void mips_print_goto(FILE *output, struct ir_instruction *instruction) {
	assert(IR_GOTO == instruction->kind);

	fprintf(output, "\n\tb %s\n", instruction->operands[0].data.name);
}

void mips_print_comparison(FILE *output, struct ir_instruction *instruction, struct ir_list *ir_list) {
	/*
		IR_LT_SIGNED_WORD,
		IR_LT_UNSIGNED_WORD,
		IR_LE_SIGNED_WORD,
		IR_LE_UNSIGNED_WORD,
		IR_GE_SIGNED_WORD,
		IR_GE_UNSIGNED_WORD,
		IR_GT_SIGNED_WORD,
		IR_GT_UNSIGNED_WORD,
		IR_EQ_WORD,
		IR_NE_WORD,
		IR_BITWISE_AND_WORD,
		IR_BITWISE_XOR_WORD,
		IR_BITWISE_OR_WORD
	*/

	if (mips_is_optimization_mid()) {
		/* check if its followed by an if/loop condition */
		if (NULL != ir_list->next) {
			if (
				IR_GOTO_IF_FALSE == ir_list->next->data->kind ||
				IR_GOTO_IF_TRUE == ir_list->next->data->kind
			) {
				return;
			}
		}
	}

	static char *opcodes[] = {
		"slt",       /* set less than signed */
		"sltu",      /* set less than unsigned */
		"sle",       /* set less than equal signed */
		"sleu",      /* set less than equal unsigned */
		"sge",       /* set greater than equal signed */
		"sgeu",      /* set greater than equal unsigned */
		"sgt",       /* set greater than signed */
		"sgtu",      /* set greater than unsigned */
		"seq",       /* set equal */
		"sne",       /* set not equal */
		"and",       /* bitwise and */
		"xor",       /* bitwise xor */
		"or"         /* bitwise or */
	};

	bool optimization_applied = false;
	if (mips_is_optimization_low()) {
		optimization_applied = mips_convert_to_immediate_instructions(output, instruction);
	}

	if (!optimization_applied) {
		fprintf(output, "\t%s\t$%s, $%s, $%s\n",
			/* subtracting 22 to remove the offset comning from ir_instruction_kind */
			opcodes[instruction->kind - 22],
			mips_get_next_available_register(instruction->operands[0].data.temporary),
			mips_get_register_from_temp(instruction->operands[1].data.temporary),
			mips_get_register_from_temp(instruction->operands[2].data.temporary)
		);
	}
}

void mips_print_branch_instruction(FILE *output, struct ir_instruction *instruction, struct ir_list *ir_list) {
	assert(IR_GOTO_IF_FALSE == instruction->kind || IR_GOTO_IF_TRUE == instruction->kind);

	bool optimization_applied = false;
	if (mips_is_optimization_mid()) {
		optimization_applied = mips_convert_to_branch_instructions(output, instruction, ir_list);
	}

	if (!optimization_applied) {
		if (IR_GOTO_IF_FALSE == instruction->kind) {
			// beqz
			fprintf(output, "\n\tbeqz\t$%s %s\n",
				mips_get_register_from_temp(instruction->operands[0].data.number),
				instruction->operands[1].data.name
			);
		}
		if (IR_GOTO_IF_TRUE == instruction->kind) {
			fprintf(output, "\n\tbgtz\t$%s %s\n",
				mips_get_register_from_temp(instruction->operands[0].data.number),
				instruction->operands[1].data.name
			);
		}
	}
}

void mips_print_instruction(FILE *output, struct ir_list *ir_list) {
	struct ir_instruction *instruction = ir_list->data;

	if (!instruction->is_used) return;

	switch (instruction->kind) {
		case IR_PROC_BEGIN: 
			mips_print_function(output, instruction);
			break;
		case IR_PROC_END: 
			mips_print_function_reset(output, instruction);
			break;
		case IR_PARAMETER: 
			mips_print_parameter(output, instruction, ir_list);
			break;
		case IR_CALL: 
			mips_print_function_call(output, instruction);
			break;
		case IR_SYS_CALL: 
			mips_print_sys_function_call(output, instruction);
			break;
		case IR_ADDRESS_OF:
			mips_print_load_address(output, instruction);
			break;
		case IR_CONST_INT:
			mips_print_load_immediate(output, instruction);
			break;
		case IR_STORE_WORD:
		case IR_STORE_HALF_WORD:
		case IR_STORE_BYTE:
			mips_print_store_word(output, instruction);
			break;
		case IR_MULT_SIGNED_WORD:
		case IR_MULT_UNSIGNED_WORD:
		case IR_DIV_SIGNED_WORD:
		case IR_DIV_UNSIGNED_WORD:
		case IR_ADD_SIGNED_WORD:
		case IR_ADD_UNSIGNED_WORD:
		case IR_SUB_SIGNED_WORD:
		case IR_SUB_UNSIGNED_WORD:
		case IR_REM_SIGNED_WORD:
		case IR_REM_UNSIGNED_WORD:
		case IR_LEFT_SHIFT_WORD:
		case IR_RIGHT_SHIFT_SIGNED_WORD:
		case IR_RIGHT_SHIFT_UNSIGNED_WORD:
			mips_print_arithmetic(output, instruction);
			break;
		case IR_LOAD_WORD:
		case IR_LOAD_HALF_WORD:
		case IR_LOAD_SIGNED_HALF_WORD:
		case IR_LOAD_BYTE:
		case IR_LOAD_SIGNED_BYTE:
			mips_print_load_word(output, instruction);
			break;
		case IR_RETURN_WORD:
		case IR_RETURN_HALF_WORD:
		case IR_RETURN_BYTE:
			mips_print_return_word(output, instruction, ir_list);
			break;
		case IR_LABEL: 
			mips_print_label(output, instruction);
			break;
		case IR_GOTO: 
			mips_print_goto(output, instruction);
			break;
		case IR_RESULT_WORD:
		case IR_RESULT_HALF_WORD:
		case IR_RESULT_BYTE:
			mips_print_result(output, instruction);
			break;
		case IR_LT_SIGNED_WORD:
		case IR_LT_UNSIGNED_WORD:
		case IR_LE_SIGNED_WORD:
		case IR_LE_UNSIGNED_WORD:
		case IR_GE_SIGNED_WORD:
		case IR_GE_UNSIGNED_WORD:
		case IR_GT_SIGNED_WORD:
		case IR_GT_UNSIGNED_WORD:
		case IR_EQ_WORD:
		case IR_NE_WORD:
		case IR_BITWISE_AND_WORD:
		case IR_BITWISE_XOR_WORD:
		case IR_BITWISE_OR_WORD: 
			mips_print_comparison(output, instruction, ir_list);
			break;
		case IR_GOTO_IF_FALSE: 
		case IR_GOTO_IF_TRUE: 
			mips_print_branch_instruction(output, instruction, ir_list);
			break;
		case IR_UNARY_MINUS:
		case IR_UNARY_LOGICAL_NEGATION:
		case IR_UNARY_BITWISE_NEGATION:
			mips_print_unary_operaions(output, instruction, ir_list);
			break;
		case IR_CAST_WORD_TO_HALF_WORD:
		case IR_CAST_WORD_TO_BYTE:
		case IR_CAST_HALF_WORD_TO_BYTE:
		case IR_CAST_UNSIGNED_HALF_WORD_TO_WORD:
		case IR_CAST_SIGNED_HALF_WORD_TO_WORD:
		case IR_CAST_UNSIGNED_BYTE_TO_HALF_WORD:
		case IR_CAST_SIGNED_BYTE_TO_HALF_WORD:
		case IR_CAST_UNSIGNED_BYTE_TO_WORD:
		case IR_CAST_SIGNED_BYTE_TO_WORD:
			mips_print_cast(output, instruction);
			break;
		default:
			// printf("instruction kind: %d\n", instruction->kind);
			// assert(0);
			break;
	}
}

void mips_print_data_section(FILE *output) {
	struct string_symbol_list* string_table = symbol_get_string_table();
	struct symbol_table* global_symbol_table = symbol_get_global_symbol_table();

	fputs("\t.data\n", output);

	/* traverse global symbol table and print each variable */
	struct symbol_list *global_table_iter;
	for (
		global_table_iter = global_symbol_table->variables;
		global_table_iter != NULL;
		global_table_iter = global_table_iter->next
	) {
		if (!global_table_iter->symbol.is_only_for_saving_type) {
			int kind = global_table_iter->symbol.type_tree->type->kind;
			int datatype = global_table_iter->symbol.type_tree->type->data.basic.datatype;
			if (datatype == TYPE_BASIC_CHAR || kind == TYPE_FUNCTION) continue;
			char *word_type = mips_get_static_word_type(datatype);
			fprintf(output, "%s:\t.%s\t%d\n", global_table_iter->symbol.name, word_type, 0);
		}
	}

	/* traverse string_table and print each string */
	struct string_symbol_list *iter;
	for (iter = string_table; iter != NULL; iter = iter->next) {
		fprintf(output, "%s:\t.asciiz\t%s\n", iter->string_symbol.id, iter->string_symbol.string);
	}


	/* cleanup */
	iter = NULL;
	free(iter);
	global_table_iter = NULL;
	free(global_table_iter);
}

void mips_print_text_section(FILE *output) {
	struct ir_list* ir_list = ir_get_ir_list();

	fprintf(output, "\t.text\n");
	fprintf(output, "\t.globl main\n");
	
	struct ir_list *ir_list_iter;
	for (ir_list_iter = ir_list; ir_list_iter != NULL; ir_list_iter = ir_list_iter->next) {
		mips_print_instruction(output, ir_list_iter);
	}

	/* cleanup */
	ir_list_iter = NULL;
	free(ir_list_iter);
}

void mips_print_program(FILE *output) {
		
	mips_print_data_section(output);

	mips_print_text_section(output);
}
