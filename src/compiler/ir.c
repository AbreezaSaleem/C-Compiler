#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "symbol.h"
#include "type.h"
#include "ir.h"
#include "helpers/type-tree.h"
#include "helpers/hash-map.h"

extern bool reset_registers;

struct ir_list *ir_list = NULL;
unsigned int label_count = 1;
unsigned int parameter_count = 0;
int instruction_position = 1;
int next_temporary = 1;
int variable_count = 1;

char* current_scope = "main";

/* consts registers map */
struct hash_map *const_registers_map;

/************************
 * CREATE IR STRUCTURES *
 ************************/

/*
 * An IR section is just a list of IR instructions. Each node has an associated
 * IR section if any code is required to implement it.
 */

void ir_initialise_const_registers_map() {
	const_registers_map = initialise_hash_map();
}

const char* ir_get_const_map_entry(int temp_number) {
	return get_value(const_registers_map, temp_number);  
}

void ir_update_constants_map(int key, int value) {
	char value_int[100];
	sprintf(value_int, "%d", value);

	add_key_value(const_registers_map, value_int, key);
}

static void ir_add_const_to_map(struct ir_instruction *instruction) {
	assert(NULL != instruction);
	assert(NULL != ir_list);

	ir_update_constants_map(instruction->operands[0].data.temporary, instruction->operands[1].data.number);
}

static void ir_reset_register() {
	if (reset_registers) {
		next_temporary = 1;
	}
}

static char* ir_update_scope(char *new_scope) {
	char* previous_scope = current_scope;
	current_scope = new_scope;
	return previous_scope;
}

static char* ir_create_label_name() {
	char base_label[] = "_GeneratedLabel_";
	char label_count_char[100];
	sprintf(label_count_char, "%d", label_count);
	char* label = (char*)malloc(strlen(base_label) + strlen(label_count_char) + 1); // +1 for null terminator
	strcpy(label, base_label);
	strcat(label, label_count_char);
	label_count++;

	return label;
}

static char* ir_create_function_name(char *func_name) {
	if (strcmp(func_name, "main") == 0) return func_name;

	if (strstr(func_name, "_Global_") == NULL) {
		char base_label[] = "_Global_";
		char* label = (char*)malloc(strlen(base_label) + strlen(func_name) + 1); // +1 for null terminator
		strcpy(label, base_label);
		strcat(label, func_name);
		return label;
	}

	return func_name;
}

static enum ir_instruction_kind ir_get_instruction_type(
	struct node *node, enum ir_instruction_kind_generic kind
) {
	assert(NULL != node);

	enum type_basic_kind type = get_type_from_expression(node)->data.basic.datatype;
	bool is_unsigned = get_type_from_expression(node)->data.basic.is_unsigned;

	switch(kind) {
		case IR_BITWISE_OR:
			return IR_BITWISE_OR_WORD;
		case IR_BITWISE_XOR:
			return IR_BITWISE_XOR_WORD;
		case IR_BITWISE_AND:
			return IR_BITWISE_AND_WORD;
		case IR_EQ:
			return IR_EQ_WORD;
		case IR_NE: 
			return IR_NE_WORD;
		case IR_LOAD: {
			switch(type) {
				case TYPE_BASIC_CHAR:
					return is_unsigned ? IR_LOAD_BYTE : IR_LOAD_SIGNED_BYTE;
				case TYPE_BASIC_SHORT:
					return is_unsigned ? IR_LOAD_HALF_WORD : IR_LOAD_SIGNED_HALF_WORD;
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return IR_LOAD_WORD;
				default:
					assert(0);
					break;
			}
			break;
		}
		case IR_STORE: {
			switch(type) {
				case TYPE_BASIC_CHAR:
					return IR_STORE_BYTE;
				case TYPE_BASIC_SHORT:
					return IR_STORE_HALF_WORD;
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return IR_STORE_WORD;
				default:
					assert(0);
					break;
			}
			break;
		}
		case IR_MULT: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_MULT_UNSIGNED_WORD : IR_MULT_SIGNED_WORD;
				default:
					assert(0);
					break;
			}
			break;
		}
		case IR_DIV: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_DIV_UNSIGNED_WORD : IR_DIV_SIGNED_WORD;
				default:
					assert(0);
					break;
			}
			break;
		}
		case IR_ADD: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_ADD_UNSIGNED_WORD : IR_ADD_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_SUB: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_SUB_UNSIGNED_WORD : IR_SUB_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_REM: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_REM_UNSIGNED_WORD : IR_REM_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_LEFT_SHIFT: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return IR_LEFT_SHIFT_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_RIGHT_SHIFT: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_RIGHT_SHIFT_UNSIGNED_WORD : IR_RIGHT_SHIFT_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_LT: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_LT_UNSIGNED_WORD : IR_LT_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_LE: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_LE_UNSIGNED_WORD : IR_LE_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_GT: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_GT_UNSIGNED_WORD : IR_GT_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_GE: {
			switch(type) {
				case TYPE_BASIC_CHAR:
				case TYPE_BASIC_SHORT:
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return is_unsigned ? IR_GE_UNSIGNED_WORD : IR_GE_SIGNED_WORD;
				default:
					assert(0);
					break;  
			}
		}
		case IR_RETURN: {
			switch(type) {
				case TYPE_BASIC_CHAR:
					return IR_RETURN_BYTE;
				case TYPE_BASIC_SHORT:
					return IR_RETURN_HALF_WORD;
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return IR_RETURN_WORD;
				default:
					assert(0);
					break;
			}
			break;
		}
		case IR_RESULT: {
			switch(type) {
				case TYPE_BASIC_CHAR:
					return IR_RESULT_BYTE;
				case TYPE_BASIC_SHORT:
					return IR_RESULT_HALF_WORD;
				case TYPE_BASIC_INT:
				case TYPE_BASIC_LONG:
					return IR_RESULT_WORD;
				default:
					assert(0);
					break;
			}
			break;
		}
		default:
			assert(0);
			break;
	}
}

static struct ir_list *ir_list_create(struct ir_instruction *node) {
	assert(node != NULL);

	struct ir_list *code;
	code = malloc(sizeof(struct ir_list));
	assert(NULL != code);

	code->prev = NULL;
	code->next = NULL;
	code->data = node;
	code->data->position = instruction_position++;
	return code;
}

static void ir_append_to_main_list(struct ir_instruction *section) {
	if (NULL == section) return;

	if (NULL == ir_list) {
		ir_list = ir_list_create(section);
	} else {
		struct ir_list *iter;
		for (iter = ir_list; NULL != iter->next; iter = iter->next) {
			// do nothing
		}
		struct ir_list *new_node = ir_list_create(section);
		iter->next = new_node;
		new_node->prev = iter;
		new_node->next = NULL;
	}
}

void ir_append(struct node *node, struct ir_instruction *instruction) {
	ir_append_to_main_list(instruction);
	if (NULL == node->ir_list) {
		node->ir_list = ir_list_create(instruction);
	} else {
		struct ir_list *iter;
		for (iter = node->ir_list; NULL != iter->next; iter = iter->next) {
			// do nothing
		}
		struct ir_list *new_node = ir_list_create(instruction);
		iter->next = new_node;
		new_node->prev = iter;
		new_node->next = NULL;
	}
}

void ir_insert(
	struct ir_instruction *previous_instruction,
	struct ir_instruction *new_instruction,
	struct ir_instruction *original_instruction
) {
	assert(NULL != previous_instruction);
	assert(NULL != new_instruction);

	struct ir_list *iter;
	for (iter = ir_list; NULL != iter->next; iter = iter->next) {
		if (iter->data->position == previous_instruction->position) {
			/* check if its resulting register is being used in next instruction */
			if (NULL != original_instruction) {
				int is_used = -1;
				for (int i = 0; i < 7; i++) {
					if (iter->next->data->operands[i].kind == OPERAND_TEMPORARY) {
						if (iter->next->data->operands[i].data.number == original_instruction->operands[0].data.temporary) {
							is_used = i;
							break;
						}
					}
				}
				/* update next instructions register */
				iter->next->data->operands[is_used].data.number = new_instruction->operands[0].data.temporary;
			}

			/* add new instruction */
			struct ir_list *new_node = ir_list_create(new_instruction);
			new_node->next = iter->next;
			new_node->prev = iter;
			iter->next->prev = new_node;
			iter->next = new_node;

			return;
		}
	}
}

/*
 * An IR instruction represents a single 3-address statement.
 */
struct ir_instruction *ir_instruction(enum ir_instruction_kind kind) {
	struct ir_instruction *instruction;

	instruction = malloc(sizeof(struct ir_instruction));
	assert(NULL != instruction);

	instruction->kind = kind;

	instruction->next = NULL;
	instruction->prev = NULL;
	instruction->scope = current_scope;
	instruction->is_used = true;
	instruction->offset_variable = NULL;

	return instruction;
}

void ir_operand_number(struct ir_instruction *instruction, int position, signed long number) {
	instruction->operands[position].kind = OPERAND_NUMBER;
	instruction->operands[position].data.number = number;
}

static void ir_operand_variable_id(struct ir_instruction *instruction, int position, signed long number) {
	instruction->operands[position].kind = OPERAND_NUMBER;
	instruction->operands[position].data.variable_id = number;
}

static void ir_operand_size(struct ir_instruction *instruction, int position, signed long number) {
	instruction->operands[position].kind = OPERAND_NUMBER;
	instruction->operands[position].data.number = number;
}

static void ir_operand_string(struct ir_instruction *instruction, int position, char *name) {
	instruction->operands[position].kind = OPERAND_STRING;
	strncpy(instruction->operands[position].data.name, name, IDENTIFIER_MAX + 1);
}

static void ir_operand_bool(struct ir_instruction *instruction, int position, bool is_string) {
	instruction->operands[position].kind = OPERAND_BOOL;
	instruction->operands[position].data.is_string = is_string;
}

void ir_operand_temporary(struct ir_instruction *instruction, int position) {
	instruction->operands[position].kind = OPERAND_TEMPORARY;
	instruction->operands[position].data.temporary = next_temporary++;
}

void ir_operand_copy(struct ir_instruction *instruction, int position, struct ir_operand *operand) {
	if (NULL != operand) {
		instruction->operands[position] = *operand;
	}
}

static void ir_assign_final_register_conditional() {
	struct ir_list *last_iter_element;
	for (last_iter_element = ir_list; NULL != last_iter_element->next; last_iter_element = last_iter_element->next) {
		// do nothing
	}

	/* here we'll replace the final registers of true + false
	 * blocks of the ternary operator with the final register 
	 * of the phi instruction
	*/

	if (true) {
		bool true_block = false;
		bool false_block = false;
		struct ir_list *iter;
		for (
			iter = last_iter_element->prev;
			IR_GOTO_IF_FALSE != iter->data->kind && IR_GOTO_IF_TRUE != iter->data->kind;
			iter = iter->prev
		) {
			if (IR_LABEL == iter->data->kind && !false_block) {
				iter = iter->prev;
				iter->data->operands[0].data.temporary = last_iter_element->data->operands[0].data.temporary;
				false_block = true;
			}
			if (IR_GOTO == iter->data->kind && !true_block) {
				iter = iter->prev;
				iter->data->operands[0].data.temporary = last_iter_element->data->operands[0].data.temporary;
				true_block = true;
			}
		}
		/* cleanup */
		iter = NULL;
		free(iter);
		last_iter_element = NULL;
		free(last_iter_element);
	}
}

static void ir_assign_final_register_short_circuit(
	struct ir_instruction *phi_instruction,
	struct ir_instruction *true_instruction,
	struct ir_instruction *false_instruction
) {
	
	if (reset_registers) {
		true_instruction->operands[0].data.temporary = phi_instruction->operands[0].data.temporary;
		false_instruction->operands[0].data.temporary = phi_instruction->operands[0].data.temporary;
	}
}

static bool is_unary_specially_handled(struct node *expression) {
	assert(NODE_UNARY_EXPR == expression->kind);

	int operation = expression->data.unary_expr.operation;
	return (
		UNARY_HYPHEN == operation ||
		UNARY_LOGICAL_NEGATION == operation ||
		UNARY_BITWISE_NEGATION == operation ||
		UNARY_PREDEC == operation ||
		UNARY_PREINC == operation ||
		UNARY_POSTDEC == operation ||
		UNARY_POSTINC == operation
	);
}

struct ir_operand *node_get_ir_operands(struct node *expression) {
	switch (expression->kind) {
		case NODE_NUMBER:
			return expression->data.number.result.ir_operand;
		case NODE_IDENTIFIER:
			return expression->data.identifier.result.ir_operand;
		case NODE_BINARY_OPERATION:
			return expression->data.binary_operation.result.ir_operand;
		case NODE_STRING: 
			return expression->data.string.result.ir_operand;
		case NODE_FUNCTION_DECL: 
			return expression->data.function.result.ir_operand;
		case NODE_CONDITIONAL_STATEMENT:
			return expression->data.conditional_statement.result.ir_operand;
		case NODE_UNARY_EXPR: {
			if (is_unary_specially_handled(expression)) {
				return expression->data.unary_expr.result.ir_operand;
			}
			return node_get_ir_operands(expression->data.unary_expr.expression);
		}
		case NODE_CAST_EXPR: {
			if (NULL == expression->data.cast_expr.result.ir_operand) {
				return node_get_ir_operands(expression->data.cast_expr.expression);
			}
			return expression->data.cast_expr.result.ir_operand;
		}
		case NODE_EXPRESSION_LIST_STATEMENT: 
		case NODE_EXPRESSION_STATEMENT: {
			if (NULL == expression->data.expression_list_statement.next) {
				return node_get_ir_operands(expression->data.expression_list_statement.self);
			} else {
				return node_get_ir_operands(expression->data.expression_list_statement.next);
			}
		}
		default:
			assert(0);
			return NULL;
	}
}

static struct ir_operand* ir_convert_lval_to_rval(struct node *expression) {
	int value = get_value_from_expression(expression);

	if (L_VALUE == value) {
		/* perform a load operation */
		enum ir_instruction_kind kind = ir_get_instruction_type(expression, IR_LOAD);
		if (type_is_pointer(get_type_from_expression(expression))) {
			kind = IR_LOAD_WORD;
		}
		struct ir_instruction *instruction = ir_instruction(kind);
		ir_operand_temporary(instruction, 0);
		ir_operand_copy(instruction, 1, node_get_ir_operands(expression));

		ir_append(expression, instruction);

		node_get_result(expression)->ir_operand = &instruction->operands[0];

		return &instruction->operands[0];
	}
	return NULL;
}

static void ir_convert_identifier_to_rval(struct node *expression) {
	if (NODE_IDENTIFIER == expression->kind) {
		struct ir_operand *operand = ir_convert_lval_to_rval(expression);
		if (NULL != operand) node_get_result(expression)->ir_operand = operand;
	} else if (NODE_EXPRESSION_STATEMENT == expression->kind) {
		ir_convert_identifier_to_rval(expression->data.expression_statement.expression);
	} else if (NODE_EXPRESSION_LIST_STATEMENT == expression->kind) {
		if (expression->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
			ir_convert_identifier_to_rval(expression->data.expression_list_statement.self);
		} else {
			struct ir_operand *operand = ir_convert_lval_to_rval(expression->data.expression_list_statement.self);
			if (NULL != operand) node_get_result(expression)->ir_operand = operand;
		}

		if (expression->data.expression_list_statement.next) {
			struct ir_operand *operand = ir_convert_lval_to_rval(expression->data.expression_list_statement.next);
			if (NULL != operand) node_get_result(expression)->ir_operand = operand;
		}
	}
}

static void ir_create_instructions_for_pointer_addition(struct node *pointer, struct node *add_node) {
	/* get size of pointer type */
	struct type *type = get_type_from_expression(pointer);
	int size = type_size(type);
	struct ir_instruction *instruction = ir_instruction(IR_CONST_INT);
	ir_operand_temporary(instruction, 0);
	ir_operand_number(instruction, 1, size);

	ir_add_const_to_map(instruction);

	ir_append_to_main_list(instruction);

	/* multiply it with other node */
	struct ir_instruction *instruction_2 = ir_instruction(IR_MULT_SIGNED_WORD);
	ir_operand_temporary(instruction_2, 0);
	ir_operand_copy(instruction_2, 1, &instruction->operands[0]);
	ir_operand_copy(instruction_2, 2, node_get_ir_operands(add_node));

	ir_append(add_node, instruction_2);

	/* it can be other things too? */
	node_get_result(add_node)->ir_operand = &instruction_2->operands[0];
}

static void ir_check_if_addition_to_pointer(struct node *node) {
	assert(NODE_BINARY_OPERATION == node->kind);

	if (BINOP_ADDITION == node->data.binary_operation.operation || BINOP_SUBTRACTION == node->data.binary_operation.operation) {
		struct type *type_left = get_type_from_expression(node->data.binary_operation.left_operand);
		struct type *type_right = get_type_from_expression(node->data.binary_operation.right_operand);

		/* 
		 * check if left operand is of type pointer
		 * you also shouldn't allow something like (&p + 9) so I added the second check
		*/
		if (
			TYPE_POINTER == type_left->kind &&
			!node_is_pointer_address(node->data.binary_operation.left_operand) &&
			!node_is_dereference(node->data.binary_operation.left_operand)  
		) {
			ir_create_instructions_for_pointer_addition(
				node->data.binary_operation.left_operand,
				node->data.binary_operation.right_operand
			);
		} else if (
			TYPE_POINTER == type_right->kind &&
			!node_is_pointer_address(node->data.binary_operation.right_operand) &&
			!node_is_dereference(node->data.binary_operation.right_operand)  
		) {
			ir_create_instructions_for_pointer_addition(
				node->data.binary_operation.right_operand,
				node->data.binary_operation.left_operand
			);
		}
	}
}

static void ir_check_if_inc_dec_to_pointer(struct node *node) {
	assert(NODE_UNARY_EXPR == node->kind);

	struct type *type = get_type_from_expression(node->data.unary_expr.expression);
	if (TYPE_POINTER == type->kind) {
		ir_generate_for_expression(node->data.unary_expr.compound_operator->right_operator);

		ir_create_instructions_for_pointer_addition(
			node->data.unary_expr.expression,
			node->data.unary_expr.compound_operator->right_operator
		);
	}
}

static void ir_create_generated_label_instruction(
	struct node *statement, struct node *condition_statement,
	enum ir_instruction_kind kind, char* label
) {
	assert(NULL != statement);

	struct ir_instruction *instruction = ir_instruction(kind);
	ir_operand_copy(instruction, 0, node_get_ir_operands(condition_statement));
	ir_operand_string(instruction, 1, label);
	ir_append(statement, instruction);
}


/*******************************
 * GENERATE IR FOR EXPRESSIONS *
 *******************************/
void ir_generate_for_number(struct node *number) {
	struct ir_instruction *instruction;
	assert(NODE_NUMBER == number->kind);

	instruction = ir_instruction(IR_CONST_INT);
	ir_operand_temporary(instruction, 0);
	ir_operand_number(instruction, 1, number->data.number.value);

	ir_add_const_to_map(instruction);

	ir_append(number, instruction);

	number->data.number.result.ir_operand = &instruction->operands[0];
}

static void ir_generate_for_identifier(struct node *identifier) {
	assert(NODE_IDENTIFIER == identifier->kind);

	char* name = identifier->data.identifier.name;
	bool is_global = strstr(identifier->data.identifier.symbol->symbol_table_tag, "FILE_SCOPE") != NULL;
	if (is_global) {
		name = ir_create_function_name(identifier->data.identifier.symbol->name);
	}

	struct ir_instruction *instruction = ir_instruction(IR_ADDRESS_OF);
	ir_operand_temporary(instruction, 0);
	ir_operand_string(instruction, 1, name);
	ir_operand_bool(instruction, 2, false);	   /* is this variable string? */
	ir_operand_variable_id(instruction, 3, identifier->data.identifier.symbol->id);
	ir_operand_size(instruction, 4, type_get_size(identifier->data.identifier.symbol->type_tree->type));
	ir_operand_number(instruction, 5, identifier->data.identifier.symbol->arg_number);
	ir_operand_number(instruction, 6, is_global);	   /* is this variable global? */

	strcpy(identifier->data.identifier.symbol->name, name);
	
	ir_append(identifier, instruction);
	
	identifier->data.identifier.result.ir_operand = &instruction->operands[0];
}

static void ir_generate_for_string(struct node *expression) {
	assert(NODE_STRING == expression->kind);

	struct ir_instruction *instruction = ir_instruction(IR_ADDRESS_OF);
	ir_operand_temporary(instruction, 0);
	ir_operand_string(instruction, 1, search_string_literal_table(expression->data.string.value));
	ir_operand_bool(instruction, 2, true);
	ir_operand_string(instruction, 2, search_string_literal_table(expression->data.string.value));
	
	ir_append(expression, instruction);
	
	expression->data.string.result.ir_operand = &instruction->operands[0];
}

static void ir_generate_for_arithmetic_binary_operation(
	enum ir_instruction_kind_generic generic_kind, struct node *binary_operation
) {
	assert(NODE_BINARY_OPERATION == binary_operation->kind);
	
	ir_generate_for_expression(binary_operation->data.binary_operation.left_operand);
	ir_generate_for_expression(binary_operation->data.binary_operation.right_operand);

	ir_convert_lval_to_rval(binary_operation->data.binary_operation.left_operand);
	ir_convert_lval_to_rval(binary_operation->data.binary_operation.right_operand);

	ir_check_if_addition_to_pointer(binary_operation);

	enum ir_instruction_kind kind = ir_get_instruction_type(binary_operation, generic_kind);
	struct ir_instruction *instruction = ir_instruction(kind);
	ir_operand_temporary(instruction, 0);
	ir_operand_copy(instruction, 1, node_get_ir_operands(binary_operation->data.binary_operation.left_operand));
	ir_operand_copy(instruction, 2, node_get_ir_operands(binary_operation->data.binary_operation.right_operand));

	ir_append(binary_operation, instruction);

	binary_operation->data.binary_operation.result.ir_operand = &instruction->operands[0];
}

static void ir_generate_for_simple_assignment(struct node *binary_operation) {
	struct ir_instruction *instruction;
	struct node *left;
	assert(NODE_BINARY_OPERATION == binary_operation->kind);

	ir_generate_for_expression(binary_operation->data.binary_operation.left_operand);
	ir_generate_for_expression(binary_operation->data.binary_operation.right_operand);

	left = binary_operation->data.binary_operation.left_operand;
	enum ir_instruction_kind kind = ir_get_instruction_type(left, IR_STORE);
	if (type_is_pointer(get_type_from_expression(binary_operation->data.binary_operation.left_operand))) {
		kind = IR_STORE_WORD;
	}
	instruction = ir_instruction(kind);

	if (NODE_IDENTIFIER == left->kind) {
		if (NULL == left->data.identifier.result.ir_operand) {
			ir_operand_temporary(instruction, 0);
			left->data.identifier.result.ir_operand = &instruction->operands[0];
		} else {
			assert(NULL != left->data.identifier.result.ir_operand);
			ir_operand_copy(instruction, 0, left->data.identifier.result.ir_operand);
		}
	} else if (NODE_UNARY_EXPR == left->kind && UNARY_POINTER == left->data.unary_expr.operation) {
		ir_operand_copy(instruction, 0, node_get_ir_operands(left->data.unary_expr.expression));
	} else {
		assert(0);
	}

	ir_convert_lval_to_rval(binary_operation->data.binary_operation.right_operand);

	assert(NULL != node_get_ir_operands(binary_operation->data.binary_operation.right_operand));
	ir_operand_copy(instruction, 1, node_get_ir_operands(binary_operation->data.binary_operation.right_operand));

	ir_append(binary_operation, instruction);
}

static void ir_generate_for_compound_binary_operation(enum ir_instruction_kind_generic generic_kind, struct node *binary_operation) {
	assert(NODE_BINARY_OPERATION == binary_operation->kind);
	ir_generate_for_expression(binary_operation->data.binary_operation.left_operand);
	ir_generate_for_expression(binary_operation->data.binary_operation.right_operand);

	struct ir_operand *operand = node_get_ir_operands(binary_operation->data.binary_operation.left_operand);

	ir_convert_lval_to_rval(binary_operation->data.binary_operation.left_operand);
	ir_convert_lval_to_rval(binary_operation->data.binary_operation.right_operand);

	ir_check_if_addition_to_pointer(binary_operation);

	enum ir_instruction_kind kind = ir_get_instruction_type(binary_operation, generic_kind);

	// perform the operation
	struct ir_instruction *instruction = ir_instruction(kind);
	ir_operand_temporary(instruction, 0);
	ir_operand_copy(instruction, 1, node_get_ir_operands(binary_operation->data.binary_operation.left_operand));
	ir_operand_copy(instruction, 2, node_get_ir_operands(binary_operation->data.binary_operation.right_operand));
	ir_append(binary_operation, instruction);
	
	// store it back to the left value
	enum ir_instruction_kind kind_2 = ir_get_instruction_type(binary_operation, IR_STORE);
	struct ir_instruction *instruction_2 = ir_instruction(kind_2);
	ir_operand_copy(instruction_2, 0, operand);
	ir_operand_copy(instruction_2, 1, &instruction->operands[0]);
	ir_append(binary_operation, instruction_2);

	binary_operation->data.binary_operation.result.ir_operand = &instruction_2->operands[0];
}

static void ir_generate_for_short_circuit_operations(enum node_binary_operation kind, struct node *binary_operation) {
	assert(NODE_BINARY_OPERATION == binary_operation->kind);

	char *inside_block_label = ir_create_label_name();
	char *outside_block_label = ir_create_label_name();

	bool is_or = LOGICAL_OR == kind;

	ir_generate_for_expression(binary_operation->data.binary_operation.left_operand);
	ir_convert_identifier_to_rval(binary_operation->data.binary_operation.left_operand);

	if (is_or) {
		/* goto if true */
		ir_create_generated_label_instruction(
			binary_operation, binary_operation->data.binary_operation.left_operand,
			IR_GOTO_IF_TRUE, inside_block_label
		);
	} else {
		/* goto if false */
		ir_create_generated_label_instruction(
			binary_operation, binary_operation->data.binary_operation.left_operand,
			IR_GOTO_IF_FALSE, inside_block_label
		);
	}

	ir_generate_for_expression(binary_operation->data.binary_operation.right_operand);
	ir_convert_identifier_to_rval(binary_operation->data.binary_operation.right_operand);

	if (is_or) {
		/* goto if true */
		ir_create_generated_label_instruction(
			binary_operation, binary_operation->data.binary_operation.right_operand,
			IR_GOTO_IF_TRUE, inside_block_label
		);
	} else {
		/* goto if false */
		ir_create_generated_label_instruction(
			binary_operation, binary_operation->data.binary_operation.right_operand,
			IR_GOTO_IF_FALSE, inside_block_label
		);
	}

	/* constInt = 1/0 */
	struct ir_instruction *instruction_1 = ir_instruction(IR_CONST_INT);
	ir_operand_temporary(instruction_1, 0);
	ir_operand_number(instruction_1, 1, is_or ? 0 : 1);
	ir_append(binary_operation, instruction_1);

	ir_add_const_to_map(instruction_1);

	struct ir_instruction *instruction_2 = ir_instruction(IR_GOTO);
	ir_operand_string(instruction_2, 0, outside_block_label);
	ir_append(binary_operation, instruction_2);

	/* create label for outer block */
	struct ir_instruction *instruction_3 = ir_instruction(IR_LABEL);
	ir_operand_string(instruction_3, 0, inside_block_label);
	ir_append(binary_operation, instruction_3);

	/* constInt = 1/0 */
	struct ir_instruction *instruction_4 = ir_instruction(IR_CONST_INT);
	ir_operand_temporary(instruction_4, 0);
	ir_operand_number(instruction_4, 1, is_or ? 1 : 0);
	ir_append(binary_operation, instruction_4);

	ir_add_const_to_map(instruction_4);

	/* create label for outer block */
	struct ir_instruction *instruction_5 = ir_instruction(IR_LABEL);
	ir_operand_string(instruction_5, 0, outside_block_label);
	ir_append(binary_operation, instruction_5);

	/* create phi instruction */
	struct ir_instruction *instruction_6 = ir_instruction(IR_PHI);
	ir_operand_temporary(instruction_6, 0);
	ir_operand_copy(instruction_6, 1, instruction_1->operands);
	ir_operand_copy(instruction_6, 2, instruction_4->operands);
	ir_append(binary_operation, instruction_6);

	ir_assign_final_register_short_circuit(instruction_6, instruction_1, instruction_4);

	binary_operation->data.binary_operation.result.ir_operand = &instruction_6->operands[0];
}

static void ir_generate_for_binary_operation(struct node *binary_operation) {
	assert(NODE_BINARY_OPERATION == binary_operation->kind);

	switch (binary_operation->data.binary_operation.operation) {
		case BINOP_MULTIPLICATION:
			ir_generate_for_arithmetic_binary_operation(IR_MULT, binary_operation);
			break;
		case BINOP_DIVISION:
			ir_generate_for_arithmetic_binary_operation(IR_DIV, binary_operation);
			break;
		case BINOP_ADDITION:
			ir_generate_for_arithmetic_binary_operation(IR_ADD, binary_operation);
			break;
		case BINOP_SUBTRACTION:
			ir_generate_for_arithmetic_binary_operation(IR_SUB, binary_operation);
			break;
		case BINOP_MOD: 
			ir_generate_for_arithmetic_binary_operation(IR_REM, binary_operation);
			break;
		case BITWISE_OR: 
			ir_generate_for_arithmetic_binary_operation(IR_BITWISE_OR, binary_operation);
			break;
		case BITWISE_XOR: 
			ir_generate_for_arithmetic_binary_operation(IR_BITWISE_XOR, binary_operation);
			break;
		case BITWISE_AND:
			ir_generate_for_arithmetic_binary_operation(IR_BITWISE_AND, binary_operation);
			break;
		case EQUALITY_OP: 
			ir_generate_for_arithmetic_binary_operation(IR_EQ, binary_operation);
			break;
		case INEQUALITY_OP: 
			ir_generate_for_arithmetic_binary_operation(IR_NE, binary_operation);
			break;
		case LESS_THAN: 
			ir_generate_for_arithmetic_binary_operation(IR_LT, binary_operation);
			break;
		case LESS_THAN_EQ: 
			ir_generate_for_arithmetic_binary_operation(IR_LE, binary_operation);
			break;
		case GREATER_THAN: 
			ir_generate_for_arithmetic_binary_operation(IR_GT, binary_operation);
			break;
		case GREATER_THAN_EQ: 
			ir_generate_for_arithmetic_binary_operation(IR_GE, binary_operation);
			break;
		case LEFT_SHIFT: 
			ir_generate_for_arithmetic_binary_operation(IR_LEFT_SHIFT, binary_operation);
			break;
		case RIGHT_SHIFT: 
			ir_generate_for_arithmetic_binary_operation(IR_RIGHT_SHIFT, binary_operation);
			break;
		case BINOP_ASSIGN:
			ir_generate_for_simple_assignment(binary_operation);
			break;
		case BINOP_PLUS_EQUAL:
			ir_generate_for_compound_binary_operation(IR_ADD, binary_operation);
			break;
		case BINOP_MINUS_EQUAL:
			ir_generate_for_compound_binary_operation(IR_SUB, binary_operation);
			break;
		case BINOP_MULTIPLY_EQUAL:
			ir_generate_for_compound_binary_operation(IR_MULT, binary_operation);
			break;
		case BINOP_DIVIDE_EQUAL:
			ir_generate_for_compound_binary_operation(IR_DIV, binary_operation);
			break;
		case BINOP_MODULO_EQUAL:
			ir_generate_for_compound_binary_operation(IR_REM, binary_operation);
			break;
		case BINOP_LEFT_SHIFT_EQUAL:
			ir_generate_for_compound_binary_operation(IR_LEFT_SHIFT, binary_operation);
			break;
		case BINOP_RIGHT_SHIFT_EQUAL:
			ir_generate_for_compound_binary_operation(IR_RIGHT_SHIFT, binary_operation);
			break;
		case BINOP_BITWISE_AND_EQUAL:
			ir_generate_for_compound_binary_operation(IR_BITWISE_AND, binary_operation);
			break;
		case BINOP_BITWISE_XOR_EQUAL:
			ir_generate_for_compound_binary_operation(IR_BITWISE_XOR, binary_operation);
			break;
		case BINOP_BITWISE_OR_EQUAL:
			ir_generate_for_compound_binary_operation(IR_BITWISE_OR, binary_operation);
			break;
		case LOGICAL_AND:
			ir_generate_for_short_circuit_operations(LOGICAL_AND, binary_operation);
			break;
		case LOGICAL_OR:
			ir_generate_for_short_circuit_operations(LOGICAL_OR, binary_operation);
			break;
		default:
			assert(0);
			break;
	}
}

static void ir_generate_for_specific_unary_operation(enum ir_instruction_kind kind, struct node *unary_expr) {
	assert(NULL != unary_expr);

	ir_generate_for_expression(unary_expr->data.unary_expr.expression);
	ir_convert_lval_to_rval(unary_expr->data.unary_expr.expression);

	struct ir_instruction *instruction = NULL;

	/* handle the special unary operation */
	switch(kind) {
		case IR_UNARY_BITWISE_NEGATION:
		case IR_UNARY_LOGICAL_NEGATION:
		case IR_UNARY_MINUS: {
			instruction = ir_instruction(kind);
			ir_operand_temporary(instruction, 0);
			ir_operand_copy(instruction, 1, node_get_ir_operands(unary_expr->data.unary_expr.expression));
			ir_append(unary_expr->data.unary_expr.expression, instruction);
			break;
		}
		default: 
			assert(0);
			break;
	}

	if (NULL != instruction) {
		unary_expr->data.unary_expr.result.ir_operand = &instruction->operands[0];
	}
}

static void ir_generate_for_inc_dec_operations(
	enum ir_instruction_kind_generic generic_kind, struct node *expression
) {
	assert(NODE_UNARY_EXPR == expression->kind);
	
	bool is_post = UNARY_POSTINC == expression->data.unary_expr.operation || UNARY_POSTDEC == expression->data.unary_expr.operation;

	ir_generate_for_expression(expression->data.unary_expr.expression);

	struct ir_operand *operand = node_get_ir_operands(expression->data.unary_expr.expression);

	// now get the r-value of this operator
	ir_convert_identifier_to_rval(expression->data.unary_expr.expression);

	struct type *type = get_type_from_expression(expression->data.unary_expr.expression);
	bool is_pointer = type_is_pointer(type);

	// create number 1 instruction
	struct ir_instruction *instruction = ir_instruction(IR_CONST_INT);
	if (!is_pointer) {
		ir_operand_temporary(instruction, 0);
		ir_operand_number(instruction, 1, 1);
		ir_append(expression, instruction);
	}

	ir_add_const_to_map(instruction);

	ir_check_if_inc_dec_to_pointer(expression);

	// add them together
	enum ir_instruction_kind kind = ir_get_instruction_type(expression, generic_kind);
	struct ir_instruction *instruction_2 = ir_instruction(kind);
	ir_operand_temporary(instruction_2, 0);
	ir_operand_copy(instruction_2, 1, node_get_ir_operands(expression->data.unary_expr.expression));
	ir_operand_copy(instruction_2, 2,
		is_pointer ?
		node_get_ir_operands(expression->data.unary_expr.compound_operator->right_operator) :
		&instruction->operands[0]
	);
	ir_append(expression, instruction_2);

	// store it back to the expression
	enum ir_instruction_kind kind_2 = ir_get_instruction_type(expression, IR_STORE);
	struct ir_instruction *instruction_3 = ir_instruction(kind_2);
	ir_operand_copy(instruction_3, 0, operand);
	ir_operand_copy(instruction_3, 1, &instruction_2->operands[0]);
	ir_append(expression, instruction_3);

	if (is_post) {
		expression->data.unary_expr.result.ir_operand = operand;
	} else {
		expression->data.unary_expr.result.ir_operand = &instruction_2->operands[0];
	}
}

static void ir_generate_for_unary_operation(struct node *expression) {
	assert(NODE_UNARY_EXPR == expression->kind);

	switch(expression->data.unary_expr.operation) {
		case UNARY_HYPHEN: 
			ir_generate_for_specific_unary_operation(IR_UNARY_MINUS, expression);
			break;
		case UNARY_LOGICAL_NEGATION:
			ir_generate_for_specific_unary_operation(IR_UNARY_LOGICAL_NEGATION, expression);
			break;
		case UNARY_BITWISE_NEGATION:
			ir_generate_for_specific_unary_operation(IR_UNARY_BITWISE_NEGATION, expression);
			break;
		case UNARY_PLUS: {
			// ir_generate_for_expression(expression->data.unary_expr.expression);
			// ir_convert_lval_to_rval(expression->data.unary_expr.expression);
		}
		case UNARY_ADDRESS:
			ir_generate_for_expression(expression->data.unary_expr.expression);
			break;
		case UNARY_POINTER:
			ir_generate_for_expression(expression->data.unary_expr.expression);
			ir_convert_lval_to_rval(expression->data.unary_expr.expression);
			break;
		case UNARY_PREINC:
		case UNARY_POSTINC:
			ir_generate_for_inc_dec_operations(IR_ADD, expression);
			break;
		case UNARY_PREDEC:
		case UNARY_POSTDEC:
			ir_generate_for_inc_dec_operations(IR_SUB, expression);
			break;
		default:
			assert(0);
			break;
	}
}

static void ir_generate_for_cast_operation(struct node *expression) {
	assert(NODE_CAST_EXPR == expression->kind);

	ir_generate_for_expression(expression->data.cast_expr.expression);

	ir_convert_lval_to_rval(expression->data.cast_expr.expression);

	enum ir_instruction_kind kind;
	bool is_unsigned = get_type_from_expression(expression->data.cast_expr.type)->data.basic.is_unsigned;
	enum type_basic_kind cast_type = get_type_from_expression(expression->data.cast_expr.type)->data.basic.datatype;
	enum type_basic_kind expression_type = get_type_from_expression(expression->data.cast_expr.expression)->data.basic.datatype;

	switch(expression_type) {
		case TYPE_BASIC_CHAR: {
			switch(cast_type) {
				case TYPE_BASIC_CHAR:
					kind = IR_NO_OPERATION;
					break;
				case TYPE_BASIC_SHORT:
					kind = is_unsigned ? IR_CAST_UNSIGNED_BYTE_TO_HALF_WORD : IR_CAST_SIGNED_BYTE_TO_HALF_WORD;
					break;
				case TYPE_BASIC_INT:
					kind = is_unsigned ? IR_CAST_UNSIGNED_BYTE_TO_WORD : IR_CAST_SIGNED_BYTE_TO_WORD;
					break;
				case TYPE_BASIC_LONG:
					kind = is_unsigned ? IR_CAST_UNSIGNED_BYTE_TO_WORD : IR_CAST_SIGNED_BYTE_TO_WORD;
					break;
				default:
					assert(0);
					break;
			}
			break;
		}
		case TYPE_BASIC_SHORT: {
			switch(cast_type) {
				case TYPE_BASIC_CHAR:
					kind = IR_CAST_HALF_WORD_TO_BYTE;
					break;
				case TYPE_BASIC_SHORT:
					kind = IR_NO_OPERATION;
					break;
				case TYPE_BASIC_INT:
					kind = is_unsigned ? IR_CAST_UNSIGNED_HALF_WORD_TO_WORD : IR_CAST_SIGNED_HALF_WORD_TO_WORD;
					break;
				case TYPE_BASIC_LONG:
					kind = is_unsigned ? IR_CAST_UNSIGNED_HALF_WORD_TO_WORD : IR_CAST_SIGNED_HALF_WORD_TO_WORD;
					break;
				default:
					assert(0);
					break;
			}
			break;
		}
		case TYPE_BASIC_INT: 
		case TYPE_BASIC_LONG: {
			switch(cast_type) {
				case TYPE_BASIC_CHAR:
					kind = IR_CAST_WORD_TO_BYTE;
					break;
				case TYPE_BASIC_SHORT:
					kind = IR_CAST_WORD_TO_HALF_WORD;
					break;
				case TYPE_BASIC_INT:
					kind = IR_NO_OPERATION;
					break;
				case TYPE_BASIC_LONG:
					kind = IR_NO_OPERATION;
					break;
				default:
					assert(0);
					break;
			}
			break;
		}
		default: 
			assert(0);
			break;
	}


	if (IR_NO_OPERATION != kind) {
		struct ir_instruction *instruction = ir_instruction(kind);
		ir_operand_temporary(instruction, 0);
		ir_operand_copy(instruction, 1, node_get_ir_operands(expression->data.cast_expr.expression));
		ir_append(expression->data.cast_expr.expression, instruction);

		expression->data.cast_expr.result.ir_operand = &instruction->operands[0];
	}
}

static void ir_handle_function_arguments(struct node *expression_statement) {
	assert(
		NODE_EXPRESSION_STATEMENT == expression_statement->kind ||
		NODE_EXPRESSION_LIST_STATEMENT == expression_statement->kind
	);
	if (expression_statement->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
		ir_handle_function_arguments(expression_statement->data.expression_list_statement.self);
	} else {
		ir_generate_for_expression(expression_statement->data.expression_list_statement.self);
		ir_convert_lval_to_rval(expression_statement->data.expression_list_statement.self);

		struct ir_instruction *instruction = ir_instruction(IR_PARAMETER);
		ir_operand_number(instruction, 0, parameter_count);
		ir_operand_copy(instruction, 1, node_get_ir_operands(expression_statement->data.expression_list_statement.self));
		ir_append(expression_statement->data.expression_list_statement.self, instruction);
		parameter_count++;
	}

	if (expression_statement->data.expression_list_statement.next) {
		ir_generate_for_expression(expression_statement->data.expression_list_statement.next);
		ir_convert_lval_to_rval(expression_statement->data.expression_list_statement.next);

		struct ir_instruction *instruction = ir_instruction(IR_PARAMETER);
		ir_operand_number(instruction, 0, parameter_count);
		ir_operand_copy(instruction, 1, node_get_ir_operands(expression_statement->data.expression_list_statement.next));
		ir_append(expression_statement->data.expression_list_statement.next, instruction);
		parameter_count++;
	}
}

static void ir_generate_for_function_call(struct node *function) {
	assert(NODE_FUNCTION_DECL == function->kind);

	// parameters, assign return value
	if (NULL != function->data.function.argument_list) {
		/* reset parameter count */
		parameter_count = 0;
		ir_handle_function_arguments(function->data.function.argument_list);
	}

	char* func_name = function->data.function.name->data.identifier.name;

	struct ir_instruction *instruction;
	if(is_syscall(func_name)) {
		instruction = ir_instruction(IR_SYS_CALL);
	} else {
		instruction = ir_instruction(IR_CALL);
		func_name = ir_create_function_name(function->data.function.name->data.identifier.name);
	}

	ir_operand_string(instruction, 0, func_name);
	// need this in mips step
	ir_operand_string(instruction, 1, function->data.function.name->data.identifier.name);
	ir_append(function, instruction);

	// get function's return type and create a temporary for it
	// but only if the function return type is not void
	if (TYPE_BASIC_VOID != get_type_from_expression(function)->data.basic.datatype) {
		enum ir_instruction_kind kind = ir_get_instruction_type(function, IR_RESULT);
		struct ir_instruction *instruction_2 = ir_instruction(kind);
		ir_operand_temporary(instruction_2, 0);
		ir_append(function, instruction_2);

		function->data.function.result.ir_operand = &instruction_2->operands[0];
	}
}

static void ir_generate_for_conditional_statement(struct node *statement) {
	assert(NODE_CONDITIONAL_STATEMENT == statement->kind);

	ir_generate_for_expression(statement->data.conditional_statement.condition);
	
	// ir_reset_register();

	ir_convert_identifier_to_rval(statement->data.conditional_statement.condition);

	/* goto if false */
	char *label = ir_create_label_name();

	/* create another label for the true block - for optimization purposes */
	char *true_block_label = (char *)malloc(strlen(label) + strlen("_true") + 1);
	strcpy(true_block_label, label);
	strcat(true_block_label, "_true");

	char* previous_scope = ir_update_scope(true_block_label);

	ir_create_generated_label_instruction(
		statement, statement->data.conditional_statement.condition,
		IR_GOTO_IF_FALSE, label
	);

	/* go inside true block */
	ir_generate_for_possible_statement(statement->data.conditional_statement.true_block);

	ir_reset_register();

	ir_convert_identifier_to_rval(statement->data.conditional_statement.true_block);

	/* jump outside if-block */
	char *outside_block_label = ir_create_label_name();
	struct ir_instruction *instruction = ir_instruction(IR_GOTO);
	ir_operand_string(instruction, 0, outside_block_label);
	ir_append(statement, instruction);
		
	/* go inside false block */
	if (NULL != statement->data.conditional_statement.false_block) {
		/* create label for outer block */
		char* previous_scope_1 = ir_update_scope(label);
		struct ir_instruction *instruction_1 = ir_instruction(IR_LABEL);
		ir_operand_string(instruction_1, 0, label);
		ir_append(statement->data.conditional_statement.false_block, instruction_1);

		if (statement->data.conditional_statement.is_ternary) {
			ir_generate_for_expression(statement->data.conditional_statement.false_block);
		} else {
			ir_generate_for_possible_statement(statement->data.conditional_statement.false_block);
		}

		// ir_reset_register();

		ir_convert_identifier_to_rval(statement->data.conditional_statement.false_block);
		ir_update_scope(previous_scope_1);
	} else {
		/* point false block label to the block that's outside the if condition */
		/* create label for outer block */
		struct ir_instruction *instruction_2 = ir_instruction(IR_LABEL);
		ir_operand_string(instruction_2, 0, label);
		ir_append(statement, instruction_2);
	}

	/* create label for outer block */
	struct ir_instruction *instruction_4 = ir_instruction(IR_LABEL);
	ir_operand_string(instruction_4, 0, outside_block_label);
	ir_append(statement, instruction_4);
	
	ir_update_scope(previous_scope);

	/* create phi operator - only if ternery because its needed for expressions only */
	if (statement->data.conditional_statement.is_ternary) {

		struct ir_instruction *instruction_3 = ir_instruction(IR_PHI);
		ir_operand_temporary(instruction_3, 0);
		ir_operand_copy(instruction_3, 1, node_get_ir_operands(statement->data.conditional_statement.true_block));
		ir_operand_copy(instruction_3, 2, node_get_ir_operands(statement->data.conditional_statement.false_block));
		ir_append(statement, instruction_3);

		ir_assign_final_register_conditional();

		statement->data.conditional_statement.result.ir_operand = &instruction_3->operands[0];
	}
}

void ir_generate_for_expression(struct node *expression) {
	assert(NULL != expression);

	switch (expression->kind) {
		case NODE_IDENTIFIER:
			ir_generate_for_identifier(expression);
			/* feels like bit of a hack but, eh */
			if (node_is_expression_just_identifier(expression)) {
				ir_convert_lval_to_rval(expression);
			}
			break;
		case NODE_NUMBER:
			ir_generate_for_number(expression);
			break;
		case NODE_STRING: 
			ir_generate_for_string(expression);
			break;
		case NODE_BINARY_OPERATION:
			ir_generate_for_binary_operation(expression);
			break;
		case NODE_UNARY_EXPR: 
			ir_generate_for_unary_operation(expression);
			break;
		case NODE_CAST_EXPR: 
			ir_generate_for_cast_operation(expression);
			break;
		case NODE_FUNCTION_DECL: 
			ir_generate_for_function_call(expression);
			break;
		case NODE_CONDITIONAL_STATEMENT: 
			ir_generate_for_conditional_statement(expression);
			break;
		case NODE_EXPRESSION_LIST_STATEMENT:
		case NODE_EXPRESSION_STATEMENT:
			ir_generate_for_expression_statement(expression);
			break;
		default:
			assert(0);
			break;
	}
}

static void ir_generate_for_compound_statement(struct node *statement) {
	if (NULL == statement) return;

	assert(NODE_COMPOUND_STATEMENT == statement->kind);

	ir_generate_for_statement_list(statement->data.compound_statement.statement);
}

static void ir_generate_for_function_definition(struct node *function) {
	assert(NODE_FUNCTION_DEFINITION == function->kind);

	char *outer_block_label = ir_create_label_name();
	// assign it to func_def node so the return statements can access it
	function->data.function_definition_statement.label_name = outer_block_label;

	char* func_name = ir_create_function_name(function->data.function_definition_statement.function_decl->data.function.name->data.identifier.name);

	struct ir_instruction *instruction = ir_instruction(IR_PROC_BEGIN);
	ir_operand_string(instruction, 0, func_name);
	// need this in mips step
	ir_operand_string(instruction, 1, function->data.function_definition_statement.function_decl->data.function.name->data.identifier.name);
	ir_append(function, instruction);
	
	ir_generate_for_compound_statement(function->data.function_definition_statement.function_body);

	struct ir_instruction *instruction_2 = ir_instruction(IR_LABEL);
	ir_operand_string(instruction_2, 0, outer_block_label);
	ir_append(function, instruction_2);

	struct ir_instruction *instruction_3 = ir_instruction(IR_PROC_END);
	ir_operand_string(instruction_3, 0, func_name);
	ir_append(function, instruction_3);

}

void ir_generate_for_expression_statement(struct node *expression_statement) {
	assert(
		NODE_EXPRESSION_STATEMENT == expression_statement->kind ||
		NODE_EXPRESSION_LIST_STATEMENT == expression_statement->kind
	);
	if (expression_statement->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
		ir_generate_for_expression_statement(expression_statement->data.expression_list_statement.self);
	} else {
		ir_generate_for_expression(expression_statement->data.expression_list_statement.self);
	}

	if (expression_statement->data.expression_list_statement.next) {
		ir_generate_for_expression(expression_statement->data.expression_list_statement.next);
	}
}

void ir_generate_for_goto_statement(struct node *statement) {
	assert(NODE_GOTO_STATEMENT == statement->kind);

	/* get function name in which this label is defined */
	char label[40] = "_UserLabel_";
	strcat(label, search_statement_label_table(statement->data.goto_statement.symbol));
	strcat(label, "_");
	strcat(label, statement->data.goto_statement.label->data.identifier.name);

	struct ir_instruction *instruction = ir_instruction(IR_GOTO);
	ir_operand_string(instruction, 0, label);
	ir_append(statement, instruction);
	// statement->data.goto_statement.result.ir_operand = &instruction->operands[0];
}

void ir_generate_for_label_statement(struct node *statement) {
	assert(NODE_LABEL_STATEMENT == statement->kind);

	/* get function name in which this label is defined */
	char label[40] = "_UserLabel_";
	strcat(label, search_statement_label_table(statement->data.label_statement.symbol));
	strcat(label, "_");
	strcat(label, statement->data.label_statement.label->data.identifier.name);

	// convert label to char*
	char *label_converted = (char *)malloc(strlen(label) + 1);
	strcpy(label_converted, label);
	
	char* previous_scope = ir_update_scope(label_converted);
	
	struct ir_instruction *instruction = ir_instruction(IR_LABEL);
	ir_operand_string(instruction, 0, label);
	ir_append(statement, instruction);
	// statement->data.label_statement.result.ir_operand = &instruction->operands[0];

	ir_generate_for_possible_statement(statement->data.label_statement.statement);

	ir_update_scope(previous_scope);
}

static void ir_handle_while_loop(struct node *statement) {
	assert(NODE_ITERATIVE_STATEMENT == statement->kind);
	assert(0 == statement->data.iterative_statement.type);

	/* add label so you can come back to this point at the next iteration */
	char *loop_block_label = ir_create_label_name();
	
	char* previous_scope = ir_update_scope(loop_block_label);

	struct ir_instruction *instruction = ir_instruction(IR_LABEL);
	ir_operand_string(instruction, 0, loop_block_label);
	ir_append(statement->data.iterative_statement.expression, instruction);

	ir_generate_for_expression_statement(statement->data.iterative_statement.expression);

	ir_convert_identifier_to_rval(statement->data.iterative_statement.expression);

	/* goto if false */
	char *outer_block_label = ir_create_label_name();
	ir_create_generated_label_instruction(
		statement, statement->data.iterative_statement.expression,
		IR_GOTO_IF_FALSE, outer_block_label
	);

	ir_reset_register();

	/* enter loop body */
	if(NULL != statement->data.iterative_statement.statement) {
		ir_generate_for_possible_statement(statement->data.iterative_statement.statement);
	}

	/* goto start of loop */
	struct ir_instruction *instruction_2 = ir_instruction(IR_GOTO);
	ir_operand_string(instruction_2, 0, loop_block_label);
	ir_append(statement->data.iterative_statement.expression, instruction_2);

	/* create label for outer block */
	struct ir_instruction *instruction_3 = ir_instruction(IR_LABEL);
	ir_operand_string(instruction_3, 0, outer_block_label);
	ir_append(statement, instruction_3);
	
	ir_update_scope(previous_scope);
}

static void ir_handle_do_while_loop(struct node *statement) {
	assert(NODE_ITERATIVE_STATEMENT == statement->kind);
	assert(1 == statement->data.iterative_statement.type);

	/* add label so you can come back to this point at the next iteration */
	char *loop_block_label = ir_create_label_name();
	struct ir_instruction *instruction = ir_instruction(IR_LABEL);
	ir_operand_string(instruction, 0, loop_block_label);
	ir_append(statement->data.iterative_statement.expression, instruction);

	char* previous_scope = ir_update_scope(loop_block_label);

	/* enter loop body */
	if(NULL != statement->data.iterative_statement.statement) {
		ir_generate_for_possible_statement(statement->data.iterative_statement.statement);
	}

	ir_generate_for_expression_statement(statement->data.iterative_statement.expression);

	ir_convert_identifier_to_rval(statement->data.iterative_statement.expression);

	ir_reset_register();

	/* goto if false */
	char *outer_block_label = ir_create_label_name();
	ir_create_generated_label_instruction(
		statement, statement->data.iterative_statement.expression,
		IR_GOTO_IF_FALSE, outer_block_label
	);

	
	/* goto start of loop */
	struct ir_instruction *instruction_2 = ir_instruction(IR_GOTO);
	ir_operand_string(instruction_2, 0, loop_block_label);
	ir_append(statement->data.iterative_statement.expression, instruction_2);

	/* create label for outer block */
	struct ir_instruction *instruction_3 = ir_instruction(IR_LABEL);
	ir_operand_string(instruction_3, 0, outer_block_label);
	ir_append(statement, instruction_3);

	ir_update_scope(previous_scope);
}

static void ir_handle_for_loop(struct node *statement) {
	assert(NODE_ITERATIVE_STATEMENT == statement->kind);
	assert(2 == statement->data.iterative_statement.type);

	char *loop_block_label = ir_create_label_name();

	char* previous_scope = ir_update_scope(loop_block_label);

	if (NULL != statement->data.iterative_statement.expression->data.for_expr.initializer) {
		ir_generate_for_possible_statement(statement->data.iterative_statement.expression->data.for_expr.initializer);
		ir_reset_register();
	}
	
	/* add label so you can come back to this point at the next iteration */
	struct ir_instruction *instruction = ir_instruction(IR_LABEL);
	ir_operand_string(instruction, 0, loop_block_label);
	ir_append(statement->data.iterative_statement.expression, instruction);

	/* handle for expression */
	char *outer_block_label = ir_create_label_name();
	if (NULL != statement->data.iterative_statement.expression->data.for_expr.condition) {
		ir_generate_for_expression_statement(statement->data.iterative_statement.expression->data.for_expr.condition);
		ir_reset_register();
		ir_convert_identifier_to_rval(statement->data.iterative_statement.expression->data.for_expr.condition);
	
		/* goto if false */
		ir_create_generated_label_instruction(
			statement, statement->data.iterative_statement.expression->data.for_expr.condition,
			IR_GOTO_IF_FALSE, outer_block_label
		);
	}

	/* enter loop body */
	if(NULL != statement->data.iterative_statement.statement) {
		ir_generate_for_possible_statement(statement->data.iterative_statement.statement);
	}

	if (NULL != statement->data.iterative_statement.expression->data.for_expr.bound) {
		ir_generate_for_possible_statement(statement->data.iterative_statement.expression->data.for_expr.bound);
		ir_reset_register();
	}

	/* goto start of loop */
	struct ir_instruction *instruction_2 = ir_instruction(IR_GOTO);
	ir_operand_string(instruction_2, 0, loop_block_label);
	ir_append(statement->data.iterative_statement.expression, instruction_2);

	if (NULL != statement->data.iterative_statement.expression->data.for_expr.condition) {
		/* create label for outer block */
		struct ir_instruction *instruction_3 = ir_instruction(IR_LABEL);
		ir_operand_string(instruction_3, 0, outer_block_label);
		ir_append(statement, instruction_3);
	}
	ir_update_scope(previous_scope);
}

static void ir_generate_for_iterative_statement(struct node *statement) {
	assert(NODE_ITERATIVE_STATEMENT == statement->kind);

	switch(statement->data.iterative_statement.type) {
		case 0: 
			ir_handle_while_loop(statement);
			break;
		case 1: 
			ir_handle_do_while_loop(statement);
			break;
		case 2: 
			ir_handle_for_loop(statement);
			break;
	}
}

static void ir_generate_for_break_statement(struct node *statement) {
	assert(NODE_BREAK_STATEMENT == statement->kind);
	
	struct node *loop = statement;

	/* we can be sure this'll always give the iterative statement
	 * because a check was added in the type stage
	 */
	while(NULL != loop) {
		if (NODE_ITERATIVE_STATEMENT == loop->kind) {
			break;
		}
		loop = loop->parent;
	}

	// get outside block label by searching the ir_instruction list of the loop node
	struct ir_list *iter;
	for (iter = loop->ir_list; NULL != iter; iter = iter->next) {
		if (IR_GOTO_IF_FALSE == iter->data->kind) {
			break;
		}
	}

	assert(NULL != iter);
	struct ir_instruction *instruction = ir_instruction(IR_GOTO);
	ir_operand_string(instruction, 0, iter->data->operands[1].data.name);
	ir_append(statement, instruction);
}

static void ir_generate_for_continue_statement(struct node *statement) {
	assert(NODE_CONTINUE_STATEMENT == statement->kind);
	
	struct node *loop = statement;

	/* we can be sure this'll always give the iterative statement
	 * because a check was added in the type stage
	 */
	while(NULL != loop) {
		if (NODE_ITERATIVE_STATEMENT == loop->kind) {
			break;
		}
		loop = loop->parent;
	}

	// get loop block label by searching the ir_instruction list of the loop's condition node
	struct ir_list *iter;
	loop = loop->data.iterative_statement.expression;
	for (iter = loop->ir_list; NULL != iter; iter = iter->next) {
		if (IR_LABEL == iter->data->kind) {
			break;
		}
	}

	assert(NULL != iter);
	struct ir_instruction *instruction = ir_instruction(IR_GOTO);
	ir_operand_string(instruction, 0, iter->data->operands[0].data.name);
	ir_append(statement, instruction);
}

static void ir_generate_for_return_statement(struct node *statement) {
	assert(NODE_RETURN_STATEMENT == statement->kind);
	
	struct node *function = statement;

	/* we can be sure this'll always give the iterative statement
	 * because a check was added in the type stage
	 */
	while(NULL != function) {
		if (NODE_FUNCTION_DEFINITION == function->kind) {
			break;
		}
		function = function->parent;
	}

	// handle return expression
	if (NULL != statement->data.return_statement.expression) {
		ir_generate_for_expression(statement->data.return_statement.expression);
		ir_convert_identifier_to_rval(statement->data.return_statement.expression);
		
		enum ir_instruction_kind kind = ir_get_instruction_type(function, IR_RETURN);
		struct ir_instruction *instruction_2 = ir_instruction(kind);
		ir_operand_copy(instruction_2, 0, node_get_ir_operands(statement->data.return_statement.expression));
		ir_append(statement, instruction_2);
	}

	// get function block label by searching the ir_instruction list of the loop's condition node
	char *outer_block_label = function->data.function_definition_statement.label_name;

	struct ir_instruction *instruction = ir_instruction(IR_GOTO);
	ir_operand_string(instruction, 0, outer_block_label);
	ir_append(statement, instruction);

	ir_reset_register();
}

void ir_generate_for_possible_statement(struct node *statement) {
	if (statement == NULL) return;

	switch(statement->kind) {
		case NODE_IDENTIFIER:
			ir_generate_for_identifier(statement);
			break;
		case NODE_NUMBER:
			ir_generate_for_number(statement);
			break;
		case NODE_STRING: 
			ir_generate_for_string(statement);
			break;
		case NODE_FUNCTION_DEFINITION:
			ir_generate_for_function_definition(statement);
			break;
		case NODE_COMPOUND_STATEMENT:
			ir_generate_for_compound_statement(statement);
			break;
		case NODE_GOTO_STATEMENT:
			ir_generate_for_goto_statement(statement);
			break;
		case NODE_LABEL_STATEMENT:
			ir_generate_for_label_statement(statement);
			break;
		case NODE_CONDITIONAL_STATEMENT: 
			ir_generate_for_conditional_statement(statement);
			break;
		case NODE_BREAK_STATEMENT: 
			ir_generate_for_break_statement(statement);
			break;
		case NODE_CONTINUE_STATEMENT: 
			ir_generate_for_continue_statement(statement);
			break;
		case NODE_RETURN_STATEMENT: 
			ir_generate_for_return_statement(statement);
			break;
		case NODE_ITERATIVE_STATEMENT: 
			ir_generate_for_iterative_statement(statement);
			break;
		case NODE_EXPRESSION_LIST_STATEMENT:
		case NODE_EXPRESSION_STATEMENT: 
			ir_generate_for_expression_statement(statement);
			break;
		case NODE_STATEMENT_LIST: 
			ir_generate_for_statement_list(statement);
			break;
		default: 
			break;
	}
}

int ir_generate_for_statement_list(struct node *statement_list) {
	if (NULL == statement_list) return 0;  

	assert(NODE_STATEMENT_LIST == statement_list->kind);
	struct node *init = statement_list->data.statement_list.init;
	struct node *statement = statement_list->data.statement_list.statement;

	if (NULL != init) {
		ir_generate_for_statement_list(init);
		ir_generate_for_possible_statement(statement);
		ir_reset_register();
	} else {
		ir_generate_for_possible_statement(statement);
		ir_reset_register();
	}
	return 0;
}

int ir_generate_for_statement_list_wrapper(struct node *statement_list) {
	ir_initialise_const_registers_map();

	return ir_generate_for_statement_list(statement_list);
}
/***********************
 * PRINT IR STRUCTURES *
 ***********************/

void ir_print_opcode(FILE *output, enum ir_instruction_kind kind) {
	static char const * const instruction_names[] = {
		"addressOf",
		"loadWord",
		"loadHalfWord",
		"loadSignedHalfWord",
		"loadByte",
		"loadSignedByte",
		"storeWord",
		"storeHalfWord",
		"storeByte",
		"multSignedWord",
		"multUnsignedWord",
		"divSignedWord",
		"divUnsignedWord",
		"addSignedWord",
		"addUnsignedWord",
		"subSignedWord",
		"subUnsignedWord",
		"remSignedWord",
		"remUnsignedWord",
		"leftShiftWord",
		"rightShiftSignedWord",
		"rightShiftUnsignedWord",
		"ltSignedWord",
		"ltUnsignedWord",
		"leSignedWord",
		"leUnsignedWord",
		"geSignedWord",
		"geUnsignedWord",
		"gtSignedWord",
		"gtUnsignedWord",
		"eqWord",
		"neWord",
		"bitwiseAndWord",
		"bitwiseXorWord",
		"bitwiseOrWord",
		"unaryMinus",
		"unaryLogicalNegation",
		"unaryBitwiseNegation",
		"constInt",
		"castWordToHalfWord",
		"castWordToByte",
		"castHalfWordToByte",
		"castUnsignedHalfWordToWord",
		"castSignedHalfWordToWord",
		"castUnsignedByteToHalfWord",
		"castSignedByteToHalfWord",
		"castUnsignedByteToWord",
		"castSignedByteToWord",
		"label",
		"goto",
		"gotoIfFalse",
		"gotoIfTrue",
		"procBegin",
		"procEnd",
		"returnWord",
		"returnHalfWord",
		"returnByte",
		"call",
		"syscall",
		"parameter",
		"resultWord",
		"resultHalfWord",
		"resultByte",
		"phi",
		"noOperation",
		NULL
	};

	fprintf(output, "%s, ", instruction_names[kind]);
}

static void ir_print_operand(FILE *output, struct ir_operand *operand) {
	switch (operand->kind) {
		case OPERAND_NUMBER:
			fprintf(output, "%hu", (unsigned short)operand->data.number);
			break;
		case OPERAND_STRING:
			fprintf(output, "%s", operand->data.name);
			break;
		case OPERAND_TEMPORARY:
			fprintf(output, "r%d", operand->data.temporary);
			break;
		case OPERAND_BOOL:
			assert(0);
			break;
	}
}

void ir_print_instruction(FILE *output, struct ir_instruction *instruction) {

	ir_print_opcode(output, instruction->kind);

	switch (instruction->kind) {
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
		case IR_PHI:
			ir_print_operand(output, &instruction->operands[0]);
			fprintf(output, ", ");
			ir_print_operand(output, &instruction->operands[1]);
			fprintf(output, ", ");
			ir_print_operand(output, &instruction->operands[2]);
			// printf(", scope: %s", instruction->scope);
			break;
		case IR_CONST_INT:
		case IR_STORE_WORD: 
		case IR_STORE_HALF_WORD: 
		case IR_STORE_BYTE:
		case IR_ADDRESS_OF:
		case IR_LOAD_WORD:
		case IR_LOAD_HALF_WORD:
		case IR_LOAD_SIGNED_HALF_WORD:
		case IR_LOAD_BYTE:
		case IR_LOAD_SIGNED_BYTE:
		case IR_UNARY_MINUS:
		case IR_UNARY_LOGICAL_NEGATION:
		case IR_UNARY_BITWISE_NEGATION:
		case IR_CAST_WORD_TO_HALF_WORD:
		case IR_CAST_WORD_TO_BYTE:
		case IR_CAST_HALF_WORD_TO_BYTE:
		case IR_CAST_UNSIGNED_HALF_WORD_TO_WORD:
		case IR_CAST_SIGNED_HALF_WORD_TO_WORD:
		case IR_CAST_UNSIGNED_BYTE_TO_HALF_WORD:
		case IR_CAST_SIGNED_BYTE_TO_HALF_WORD:
		case IR_CAST_UNSIGNED_BYTE_TO_WORD:
		case IR_CAST_SIGNED_BYTE_TO_WORD:
		case IR_GOTO_IF_FALSE:
		case IR_GOTO_IF_TRUE:
		case IR_PARAMETER:
			ir_print_operand(output, &instruction->operands[0]);
			fprintf(output, ", ");
			ir_print_operand(output, &instruction->operands[1]);
			// printf(", scope: %s", instruction->scope);
			break;
		case IR_GOTO:
		case IR_LABEL:
		case IR_PROC_BEGIN:
		case IR_PROC_END:
		case IR_RETURN_WORD:
		case IR_RETURN_HALF_WORD:
		case IR_RETURN_BYTE:
		case IR_CALL:
		case IR_SYS_CALL:
		case IR_RESULT_WORD:
		case IR_RESULT_HALF_WORD:
		case IR_RESULT_BYTE:
			ir_print_operand(output, &instruction->operands[0]);
			// printf(", scope: %s", instruction->scope);
			break;
		case IR_NO_OPERATION:
			break;
		default:
			assert(0);
			break;
	}
}

void ir_print_section(FILE *output) {
	// int i = 0;
	struct ir_list *iter = NULL;
	struct ir_instruction *instruction_iter = NULL;

	/* iterate ir_list */
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		assert(NULL != iter->data);
		instruction_iter = iter->data;
		while (NULL != instruction_iter) {
			// fprintf(output, "%d: ", i++);
			if (instruction_iter->is_used) {
				fprintf(output, "(");
				ir_print_instruction(output, instruction_iter);
				fprintf(output, ")");
				fprintf(output, "\n");
			};

			instruction_iter = instruction_iter->next;
		}
	}

}

struct ir_instruction *ir_get_instruction_by_temporary(int temporary) {
	struct ir_list *iter = NULL;
	struct ir_instruction *instruction_iter = NULL;
	struct ir_instruction *instruction = NULL;

	/* iterate ir_list */
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		assert(NULL != iter->data);
		instruction_iter = iter->data;
		while (NULL != instruction_iter) {
			if (instruction_iter->operands[0].data.temporary == temporary) {
				instruction = instruction_iter;
				break;
			}
			instruction_iter = instruction_iter->next;
		}
	}

	/* cleanup */
	instruction_iter = NULL;
	free(instruction_iter);
	iter = NULL;
	free(iter);

	return instruction;
}

struct ir_instruction *ir_get_next_instruction(struct ir_instruction *instruction) {
	struct ir_list *iter = NULL;
	struct ir_instruction *instruction_iter = NULL;
	struct ir_instruction *next_instruction = NULL;

	/* iterate ir_list */
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		assert(NULL != iter->data);
		instruction_iter = iter->data;
		while (NULL != instruction_iter) {
			if (instruction_iter->position == (instruction->position + 2)) {
				next_instruction = instruction_iter;
				break;
			}
			instruction_iter = instruction_iter->next;
		}
	}

	/* cleanup */
	instruction_iter = NULL;
	free(instruction_iter);
	iter = NULL;
	free(iter);

	return next_instruction;
}

void ir_remove_instruction_from_list(struct ir_instruction *instruction) {
	assert(NULL != instruction);

	struct ir_list *iter = NULL;
	
	for (iter = ir_list; NULL != iter; iter = iter->next) {
		if (iter->data->position == instruction->position) {
			if (NULL == iter->prev) {
				ir_list = iter->next;
			} else {
				/* is 1 okay? Will this work for all kinds of instructions? */
				if (
					IR_CONST_INT == iter->next->data->kind ||
					IR_SYS_CALL == iter->next->data->kind ||
					IR_CALL == iter->next->data->kind
				) {
					// don't update this
					iter->prev->next = iter->next;
					iter->next->data->position = instruction->position;
				} else {
					ir_operand_copy(iter->next->data, 1, &iter->prev->data->operands[0]);
					iter->prev->next = iter->next;
					iter->next->data->position = instruction->position;
				}
			}
			if (NULL != iter->next) {
				iter->next->prev = iter->prev;
			}
			break;
		}
	}

	/* cleanup */
	instruction = NULL;
	free(instruction);
	iter = NULL;
	free(iter);
}

void ir_mark_constant_as_obselete(int temporary) {
	struct ir_instruction *instruction = ir_get_instruction_by_temporary(temporary);
	assert(IR_CONST_INT == instruction->kind);
	instruction->is_used = false;
}

struct ir_list* ir_get_ir_list() {
	return ir_list;
}
