#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "node.h"
#include "symbol.h"
#include "helpers/type-tree.c"
#include "helpers/stack.c"

#define MAX_SYSCALLS 10

unsigned int nextSymbolId = 0;
unsigned int nextStringSymbolId = 0;
unsigned int nextStatementSymbolId = 0;

unsigned int fileScope_level = 0;
unsigned int procedureScope_level = 0;
unsigned int blockScope_level = 0;

unsigned int argument_count = 0;

struct symbol_table *file_scope_table;

extern int evaluate_expr_error_count;

static const char *symbol_table_kind_map[] = {
  "FILE_SCOPE",
  "PROCEDURE_SCOPE",
  "BLOCK_SCOPE",
  "STATEMENT_LABEL",
  "STRING_LITERLS"
};

int syscalls_count = 0;
struct node* syscalls[MAX_SYSCALLS];

/************************
 * CREATE SYMBOL TABLES *
 ************************/

bool is_syscall(char *function_name) {
	return strstr(function_name, "syscall") != NULL;
}

static bool syscalls_equal(char *syscall_decl, char *syscall_call) {
	return 0 == strcmp(syscall_decl, syscall_call);
}

struct symbol_table *symbol_create_table(enum symbol_table_kind kind) {
	struct symbol_table *table = malloc(sizeof(struct symbol_table));
	assert(NULL != table);
	table->kind = kind;
	table->variables = NULL;
	table->parent = NULL;
	struct symbol_table_children *children = malloc(sizeof(struct symbol_table_children));
	assert(NULL != children);
	table->children = children;
	table->children->self = NULL;
	table->children->next = NULL;

	/* Link table to string literal table if its File Scope */
	if (kind == FILE_SCOPE) {
		table->data.string_literals.string_literals = NULL;
		table->data.string_literals.kind = STRING_LITERALS;
	}

	/* Link table to statement label table if its Procedure Scope */
	if (kind == PROCEDURE_SCOPE) {
		table->data.statement_labels.statement_labels = NULL;
		table->data.statement_labels.kind = STATEMENT_LABELS;
	}

	if (kind == FILE_SCOPE) {
		fileScope_level++;
	} else if (kind == PROCEDURE_SCOPE) {
		procedureScope_level++;
	} else if (kind == BLOCK_SCOPE) {
		blockScope_level++;
	}

	/* push new table to stack */
	push_to_stack(table);

	return table;
}

struct symbol *symbol_get(struct symbol_table *table, char name[]) {
	struct symbol_list *iter;
	for (iter = table->variables; NULL != iter; iter = iter->next) {
		if (!strcmp(name, iter->symbol.name)) {
			return &iter->symbol;
		}
	}
	return NULL;
}

struct symbol *symbol_get_by_id(struct symbol_table *table, int variable_id) {
	struct symbol_list *iter;
	for (iter = table->variables; NULL != iter; iter = iter->next) {
		printf("searching %d %d\n", variable_id, iter->symbol.id);
		if (variable_id == iter->symbol.id) {
			return &iter->symbol;
		}
	}
	return NULL;
}

static struct symbol *symbol_put(struct symbol_table *table, char name[], struct type_tree *type_tree) {
	struct symbol_list *symbol_list;

	symbol_list = malloc(sizeof(struct symbol_list));
	assert(NULL != symbol_list);

	strncpy(symbol_list->symbol.name, name, IDENTIFIER_MAX);
	symbol_list->symbol.result.type = NULL;
	symbol_list->symbol.result.ir_operand = NULL;
	symbol_list->symbol.id = nextSymbolId++;
	symbol_list->symbol.type_tree = type_tree;
	symbol_list->symbol.is_used = 0;
	symbol_list->symbol.is_function_defined = 0;
	symbol_list->symbol.is_system_call = 0;
	symbol_list->symbol.is_only_for_saving_type = 0;
	symbol_list->symbol.arg_number = -1;

	char level[IDENTIFIER_MAX];
	if (table->kind == FILE_SCOPE) {
		sprintf(level, "%d", fileScope_level);
	} else if (table->kind == PROCEDURE_SCOPE) {
		sprintf(level, "%d", procedureScope_level);
	} else if (table->kind == BLOCK_SCOPE) {
		sprintf(level, "%d", blockScope_level);
	} else {
		assert(0);
	}

	strcat(level, "_");
	strcat(level, symbol_table_kind_map[table->kind]);
	strncpy(symbol_list->symbol.symbol_table_tag, level, IDENTIFIER_MAX);

	symbol_list->next = table->variables;
	table->variables = symbol_list;

	return &symbol_list->symbol;
}

void insert_child(struct symbol_table *parent, struct symbol_table *child) {
	assert(NULL != parent);
	assert(NULL != child);

	struct symbol_table_children *children = parent->children;
	assert(NULL != children);

	if (NULL == children->self) {
		children->self = child;
		children->next = NULL;
	} else {
		while (NULL != children->next) {
			children = children->next;
		}
		children->next = malloc(sizeof(struct symbol_table_children));
		assert(NULL != children->next);
		children->next->self = child;
		children->next->next = NULL;
	}
}

struct symbol* search_symbol_table(char* name, struct symbol_table* table) {
	if (NULL == table) {
		/* Variable does not exist */
		return NULL;
	}
	struct symbol *symbol = symbol_get(
		table,
		name
	);

	if (NULL == symbol) {
		/* search parent table */
		return search_symbol_table(name, table->parent);
	} else {
		return symbol;
	}
	return NULL;
}

char* search_string_literal_table(char* name) {
	struct string_symbol_list *symbol_list = file_scope_table->data.string_literals.string_literals;
	char *result = "";

	while(symbol_list != NULL) {
		if (0 == strcmp(symbol_list->string_symbol.string, name)) {
			result = symbol_list->string_symbol.id;
			break;
		}
		symbol_list = symbol_list->next;
	}

	/* cleanup */
	symbol_list = NULL;
	free(symbol_list);

	return result;
}

char* search_statement_label_table(struct statement_label *statement_label) {
	assert(NULL != statement_label);
	assert(NULL != statement_label->parent);
	assert(PROCEDURE_SCOPE == statement_label->parent->kind);

	return statement_label->parent->data.procedure.name;
}

struct string_symbol *create_node_for_string_table(char* string) {
	struct string_symbol_list *symbol_list = file_scope_table->data.string_literals.string_literals;
	struct string_symbol *symbol = NULL;

	while(symbol_list != NULL) {
		if (0 == strcmp(symbol_list->string_symbol.string, string)) {
			symbol = &symbol_list->string_symbol;
			break;
		}
		
		symbol_list = symbol_list->next;
	}

	if (NULL == symbol) {
		symbol = malloc(sizeof(struct string_symbol));
		strncpy(symbol->string, string, STRING_MAX);
		char id[10];
		sprintf(id, "%d", nextStringSymbolId++);
		char label[40] = "_StringLabel_";
		strcat(label, id);
		strncpy(symbol->id, label, 40);
		/* Create a new node for the linked list and add it to the end */
    struct string_symbol_list *new_node = malloc(sizeof(struct string_symbol_list));
    new_node->string_symbol = *symbol;
    new_node->next = NULL;

    
		struct string_symbol_list *current = file_scope_table->data.string_literals.string_literals;
		if (NULL == current) {
			file_scope_table->data.string_literals.string_literals = new_node;
		} else {
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = new_node;
		}
		/* cleanup */
		symbol_list = NULL;
		free(symbol_list);
	}
	return symbol;
}

int create_node_for_statement_label_table(
	struct symbol_table *table,
	struct node* statement,
	int is_goto_statement
) {
	int error_count = 0;
	char *name;
	if (is_goto_statement == 1) {
		name = statement->data.goto_statement.label->data.identifier.name;
	} else {
		name = statement->data.label_statement.label->data.identifier.name;
	}

	while (PROCEDURE_SCOPE != table->kind) {
		if (table == NULL) {
			compiler_print_error(statement->location, "Label \"%s\" is not within a procedure scope", name);
			return 1;
		}
		table = table->parent;
	}

	struct statement_labels_list *statement_label = table->data.statement_labels.statement_labels;
	struct statement_label *symbol = NULL;


	// scan the table in search for a statement label
	while(statement_label != NULL ) {
		if (0 == strcmp(statement_label->statement_label.label, name)) {
			symbol = &statement_label->statement_label;

			if (is_goto_statement == 0) {
				if (symbol->is_defined) {
					compiler_print_error(statement->location, "Label \"%s\" was already defined", name);
					error_count++;
				} else {
					symbol->is_defined = 1;
				}
			} else {
				symbol->is_used = 1;
			}
			break;
		}
		statement_label = statement_label->next;
	}

	if (NULL == symbol) {
		symbol = malloc(sizeof(struct statement_label));
		strncpy(symbol->label, name, IDENTIFIER_MAX);
		symbol->id = ++nextStatementSymbolId;
		symbol->parent = table;
		if (is_goto_statement == 1) {
			symbol->is_defined = 0;
			symbol->is_used = 1;
		} else {
			symbol->is_defined = 1;
			symbol->is_used = 0;
		}

		/* Create a new node for the linked list and add it to the end */
    struct statement_labels_list *new_node = malloc(sizeof(struct statement_labels_list));
    new_node->statement_label = *symbol;
    new_node->next = NULL;

		struct statement_labels_list *current = table->data.statement_labels.statement_labels;
		if (NULL == current) {
			table->data.statement_labels.statement_labels = new_node;
		} else {
			while (current->next != NULL) {
				current = current->next;
			}
			current->next = new_node;
		}
		/* cleanup */
		statement_label = NULL;
		free(statement_label);
	}

	if (is_goto_statement == 1) {
		statement->data.goto_statement.symbol = symbol;
	} else {
		statement->data.label_statement.symbol = symbol;
	}

	return error_count;
}

static int symbol_add_from_identifier(struct symbol_table *table, struct node *identifier, bool define) {
	int error_count = 0;
	if (NODE_IDENTIFIER == identifier->kind) {
		struct symbol * symbol = search_symbol_table(identifier->data.identifier.name, table);
		if (NULL == symbol) {
			// because you're referencing a variable that does not exist
			compiler_print_error(identifier->location, "Undefined identifier %s", identifier->data.identifier.name);
			free(symbol);
			error_count++;
		} else {
			symbol->is_used = 1;
			identifier->data.identifier.symbol = symbol;
		}
	} else if (NODE_UNARY_EXPR == identifier->kind) {
		return symbol_add_from_unary_operation(table, identifier);
	} else {
		printf("kind %d\n", identifier->kind);
		compiler_print_error(identifier->location, "Unknown identifier");
		assert(0);
		error_count++;
	}
	return error_count;

}

static int symbol_add_from_string(struct node *string) {
	assert(NODE_STRING == string->kind);
	assert(NULL != file_scope_table);
	
	string->data.string.symbol = create_node_for_string_table(string->data.string.value);
	return 0;
}

static int symbol_add_from_label_statement(struct symbol_table *table, struct node *label_statement) {
	assert(NODE_LABEL_STATEMENT == label_statement->kind);
	int error_count = 0;

	// here we check populate the statement label table
	error_count += create_node_for_statement_label_table(table, label_statement, 0);

	error_count += symbol_add_from_possible_statement(table, label_statement->data.label_statement.statement);

	return error_count;
}

static int symbol_add_from_goto_statement(struct symbol_table *table, struct node *goto_statement) {
	assert(NODE_GOTO_STATEMENT == goto_statement->kind);
	int error_count = 0;

	// here we check populate the statement label table
	error_count += create_node_for_statement_label_table(table, goto_statement, 1);

	return error_count;
}

int symbol_add_from_unary_operation(struct symbol_table *table, struct node *unary_operation) {
	assert(NODE_UNARY_EXPR == unary_operation->kind);

	switch (unary_operation->data.unary_expr.operation) {
		case UNARY_HYPHEN:
		case UNARY_PLUS:
		case UNARY_LOGICAL_NEGATION:
		case UNARY_BITWISE_NEGATION:
		case UNARY_ADDRESS:
		case UNARY_POINTER:
		case UNARY_PREINC:
		case UNARY_PREDEC:
		case UNARY_POSTINC:
		case UNARY_POSTDEC:
			return symbol_add_from_expression(table, unary_operation->data.unary_expr.expression);
		default:
			printf("kind %d\n", unary_operation->data.unary_expr.operation);
			compiler_print_error(unary_operation->location, "Unknown expression");
			assert(0);
			return 1;
	}
}

static int symbol_is_subscript_or_pointer_operator(struct node *expression) {
	return NODE_UNARY_EXPR == expression->kind && UNARY_POINTER == expression->data.unary_expr.operation;
}

static int symbol_add_from_binary_operation(struct symbol_table *table, struct node *binary_operation) {
	assert(NODE_BINARY_OPERATION == binary_operation->kind);

	switch (binary_operation->data.binary_operation.operation) {
		case BINOP_MULTIPLICATION:
		case BINOP_DIVISION:
		case BINOP_ADDITION:
		case BINOP_SUBTRACTION:
		case BINOP_MOD:
		case LOGICAL_OR:
		case LOGICAL_AND:
		case BITWISE_OR:
		case BITWISE_XOR:
		case BITWISE_AND:
		case EQUALITY_OP:
		case INEQUALITY_OP:
		case LESS_THAN:
		case LESS_THAN_EQ:
		case GREATER_THAN:
		case GREATER_THAN_EQ:
		case LEFT_SHIFT:
		case RIGHT_SHIFT:
			return symbol_add_from_expression(table, binary_operation->data.binary_operation.left_operand)
					 + symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);
		case BINOP_ASSIGN:
		case BINOP_PLUS_EQUAL:
		case BINOP_MINUS_EQUAL:
		case BINOP_MULTIPLY_EQUAL:
		case BINOP_DIVIDE_EQUAL:
		case BINOP_MODULO_EQUAL:
		case BINOP_LEFT_SHIFT_EQUAL:
		case BINOP_RIGHT_SHIFT_EQUAL:
		case BINOP_BITWISE_AND_EQUAL:
		case BINOP_BITWISE_XOR_EQUAL:
		case BINOP_BITWISE_OR_EQUAL:
			/* We're handling these separately because we have to add an identifier/expression check for l-values */
			if (
				NODE_IDENTIFIER == binary_operation->data.binary_operation.left_operand->kind ||
				symbol_is_subscript_or_pointer_operator(binary_operation->data.binary_operation.left_operand)
			) {
				return symbol_add_from_identifier(table, binary_operation->data.binary_operation.left_operand, true)
						 + symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);
			} else {
				compiler_print_error(binary_operation->data.binary_operation.left_operand->location,
														 "Left operand of assignment must be an identifier");
				return 1
						 + symbol_add_from_expression(table, binary_operation->data.binary_operation.left_operand)
						 + symbol_add_from_expression(table, binary_operation->data.binary_operation.right_operand);
			}
		default:
			printf("kind %d\n", binary_operation->data.binary_operation.operation);
			compiler_print_error(binary_operation->location, "Unknown expression");
			assert(0);
			return 1;
	}
}

static int symbol_add_from_cast_expression(struct symbol_table *table, struct node *cast_expression) {
	assert(NODE_CAST_EXPR == cast_expression->kind);
	return symbol_add_from_expression(table, cast_expression->data.cast_expr.expression);
}

int symbol_add_from_conditional_statement(struct symbol_table *table, struct node *statement) {
	assert(NODE_CONDITIONAL_STATEMENT == statement->kind);

	int error_count = 0;

  if (statement->data.conditional_statement.is_ternary) {
		error_count += symbol_add_from_expression(table, statement->data.conditional_statement.condition);
		error_count += symbol_add_from_expression(table, statement->data.conditional_statement.true_block);
		error_count += symbol_add_from_expression(table, statement->data.conditional_statement.false_block);
	} else {
		// handle condition
		error_count += symbol_add_from_expression_list_statement(table, statement->data.conditional_statement.condition);

		// handle true block
		error_count += symbol_add_from_possible_statement(table, statement->data.conditional_statement.true_block);

		if (NULL != statement->data.conditional_statement.false_block) {
			error_count += symbol_add_from_possible_statement(table, statement->data.conditional_statement.false_block);
		}
	}
	return error_count;
}

int symbol_add_from_for_expression(struct symbol_table *table, struct node *statement) {
	assert(NODE_FOR_EXPR == statement->kind);

	int error_count = 0;

	// handle initializer
	if (NULL != statement->data.for_expr.initializer) {
		error_count += symbol_add_from_expression_list_statement(table, statement->data.for_expr.initializer);
	}

	// handle condition
	if (NULL != statement->data.for_expr.condition) {
		error_count += symbol_add_from_expression_list_statement(table, statement->data.for_expr.condition);
	}

	// handle bound
	if (NULL != statement->data.for_expr.bound) {
		error_count += symbol_add_from_expression_list_statement(table, statement->data.for_expr.bound);
	}

	return error_count;
}

int symbol_add_from_expression(struct symbol_table *table, struct node *expression) {
	switch (expression->kind) {
		case NODE_BINARY_OPERATION:
			return symbol_add_from_binary_operation(table, expression);
		case NODE_UNARY_EXPR:
			return symbol_add_from_unary_operation(table, expression);
		case NODE_IDENTIFIER:
			return symbol_add_from_identifier(table, expression, false);
		case NODE_NUMBER:
			return 0;
		case NODE_STRING:
			return symbol_add_from_string(expression);
		case NODE_CAST_EXPR:
			return symbol_add_from_cast_expression(table, expression);
		/* For ternary operators */
    case NODE_CONDITIONAL_STATEMENT:
      return symbol_add_from_conditional_statement(table, expression);
		case NODE_EXPRESSION_LIST_STATEMENT:
			return symbol_add_from_expression_list_statement(table, expression);
		case NODE_EXPRESSION_STATEMENT:
			return symbol_add_from_expression_list_statement(table, expression);
		case NODE_FUNCTION_DECL: 
			/* this is a function call */
			return symbol_add_from_function_call(table, expression);
		case NODE_FOR_EXPR:
			return symbol_add_from_for_expression(table, expression);
		default:
			printf("Kind %d\n", expression->kind);
			compiler_print_error(expression->location, "Unknown expression");
			assert(0);
			return 1;
	}
}

int symbol_add_from_expression_list_statement(struct symbol_table *table, struct node *expression_statement) {
	int result = 0;
	assert(
		NODE_EXPRESSION_STATEMENT == expression_statement->kind
		|| NODE_EXPRESSION_LIST_STATEMENT == expression_statement->kind
	);
	if (expression_statement->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
		result += symbol_add_from_expression_list_statement(table, expression_statement->data.expression_list_statement.self);
  } else {
    result += symbol_add_from_expression(table, expression_statement->data.expression_list_statement.self);
  }

  if (NULL != expression_statement->data.expression_list_statement.next) {
    result += symbol_add_from_expression(table, expression_statement->data.expression_list_statement.next);
  }
	return result;
}

static int symbol_add_from_array(struct symbol_table *table, struct node *statement) {
	assert(NULL != statement);

	return symbol_add_from_expression_list_statement(table, statement->data.array.size);

}

static int symbol_add_from_pointer(struct symbol_table *table, struct node *pointer) {
	assert(NODE_POINTER == pointer->kind);
	int error_count = 0;
	switch(pointer->data.pointer.points_to->kind) {
    case NODE_POINTER:
      error_count += symbol_add_from_pointer(table, pointer->data.pointer.points_to);
      break;
    case NODE_ARRAY: 
      error_count += symbol_add_from_array(table, pointer->data.pointer.points_to);
      break;
    default:
      break;
  }
	return error_count;
}

static int symbol_add_from_declaration_type(struct symbol_table *table, struct node *declaration_type) {
	assert(NULL != declaration_type);
	int error_count = 0;

	switch (declaration_type->kind) {
		case NODE_ARRAY:
			error_count += symbol_add_from_array(table, declaration_type);
			break;
		case NODE_POINTER:
			error_count += symbol_add_from_pointer(table, declaration_type);
			break;
		default:
			break;
	}

	return error_count;
}

static int symbol_add_from_declaration_list(
	struct symbol_table *table, struct node *declaration_type, struct node *declaration_list
) {
	int error_count = 0;
	assert(NULL != declaration_type);
	if (NODE_DECLARATION_LIST_STATEMENT == declaration_list->data.declaration_list_statement.self->kind) {
		error_count += symbol_add_from_declaration_list(table, declaration_type, declaration_list->data.declaration_list_statement.self);
	} else {
		struct type_tree *type_tree = create_type_tree(declaration_type, declaration_list->data.declaration_list_statement.self);
		struct symbol *symbol = symbol_get(table, type_tree->identifier->data.identifier.name);
		/* Check if declaration already exists IN CURRENT SCOPE */
		if (NULL != symbol) {
			/* Check for functions definitions - we allow multiple definitions + declarations to coexist */
			if (symbol->type_tree->type->kind != TYPE_FUNCTION) {
				compiler_print_error(
					declaration_list->data.declaration_list_statement.self->location,
					"variable \"%s\" already exists", type_tree->identifier->data.identifier.name
				);
				error_count++;
			} else {
				if (0 != compare_type_trees(symbol->type_tree, type_tree)) {
					compiler_print_error(declaration_list->location, "function \"%s\" has already been declared with a different signature", type_tree->identifier->data.identifier.name);
					error_count++;
				} 
			}
		} else {
			symbol = symbol_put(table, type_tree->identifier->data.identifier.name, type_tree);
			type_tree->identifier->data.identifier.symbol = symbol;

			if (is_syscall(type_tree->identifier->data.identifier.name)) {
				symbol->is_system_call = 1;
				symbol->is_function_defined = 1;
			}

			if (type_tree->type->kind == TYPE_FUNCTION) {
				// check arguments
				if (NULL != declaration_list->data.declaration_list_statement.self->data.function.argument_list) {
					argument_count = 0;
					error_count += symbol_add_from_arguments_list(
						table,
						declaration_list->data.declaration_list_statement.self->data.function.argument_list,
						true
					);
				}
			}
		}

		/* Check for arrays */
		if (type_tree->type->kind == TYPE_ARRAY && evaluate_expr_error_count > 0) {
			compiler_print_error(declaration_list->location, "Array size must have a valid constant expression");
			error_count++;
		}

		error_count += symbol_add_from_declaration_type(table, declaration_list->data.declaration_list_statement.self);
	}

	if (NULL != declaration_list->data.declaration_list_statement.next) {
		struct type_tree *type_tree = create_type_tree(declaration_type, declaration_list->data.declaration_list_statement.next);
		struct symbol *symbol = symbol_get(table, type_tree->identifier->data.identifier.name);
		if (NULL != symbol) {
			compiler_print_error(
				declaration_list->data.declaration_list_statement.self->location, 
				"variable \"%s\" already exists", type_tree->identifier->data.identifier.name
			);
			error_count++;
		} else {
			symbol = symbol_put(table, type_tree->identifier->data.identifier.name, type_tree);
			type_tree->identifier->data.identifier.symbol = symbol;
		}
		error_count += symbol_add_from_declaration_type(table, declaration_list->data.declaration_list_statement.next);
	}
	return error_count;
}

static int symbol_add_from_declaration(struct symbol_table *table, struct node *declaration) {
	assert(NULL != declaration);

	/* The declaration part of the declaration node can have
	 * Multiple declarations because of the comma operator
	 * eg int x, y, z;
	 * we need to handle each individual declaration separately
	 */


	return symbol_add_from_declaration_list(
		table, declaration->data.declaration_statement.type, declaration->data.declaration_statement.declaration
	);
}

int symbol_add_from_arguments_list(struct symbol_table *table, struct node *arguments_list, int only_for_type) {
	int error_count = 0;

	if(NODE_TYPE == arguments_list->kind) {
		return error_count;
	}

	if (NODE_ARGUMENTS_LIST == arguments_list->data.arguments_list.self->kind) {
    error_count += symbol_add_from_arguments_list(table, arguments_list->data.arguments_list.self, only_for_type);
  } else {
		if(NULL == arguments_list->data.arguments_list.self->data.declaration_statement.declaration) {
		} else {
			struct type_tree *type_tree = create_type_tree(
				arguments_list->data.arguments_list.self->data.declaration_statement.type,
				arguments_list->data.arguments_list.self->data.declaration_statement.declaration
			);
			struct symbol *symbol = symbol_put(table, type_tree->identifier->data.identifier.name, type_tree);
			symbol->is_only_for_saving_type = only_for_type;
			symbol->arg_number = argument_count++;
			type_tree->identifier->data.identifier.symbol = symbol;
		}
  }

  if (NULL != arguments_list->data.arguments_list.next) {
		if(NULL == arguments_list->data.arguments_list.next->data.declaration_statement.declaration) {
			// compiler_print_error(arguments_list->data.arguments_list.next->location, "Missing argument name");
			// error_count++;
		} else {
			struct type_tree *type_tree = create_type_tree(
				arguments_list->data.arguments_list.next->data.declaration_statement.type,
				arguments_list->data.arguments_list.next->data.declaration_statement.declaration
			);
			struct symbol *symbol = symbol_put(table, type_tree->identifier->data.identifier.name, type_tree);
			symbol->is_only_for_saving_type = only_for_type;
			symbol->arg_number = argument_count++;
			type_tree->identifier->data.identifier.symbol = symbol;
		}
  }

	return error_count;
}

int symbol_add_from_compound_statement(
	struct symbol_table *table, struct node *compound_statement, bool is_parent_function
) {
	/* Created a separate handler for compound statement because we need to manage symbol tables for them */
	assert(NULL != compound_statement);
	int error_count = 0;
	struct symbol_table *new_table = table;

	if (!is_parent_function) {
		new_table = symbol_create_table(BLOCK_SCOPE);
		new_table->parent = table;
		insert_child(table, new_table);
	}

	error_count += symbol_add_from_possible_statement(new_table, compound_statement->data.compound_statement.statement);

	return error_count;
}

int symbol_add_from_iterative_statement(struct symbol_table *table, struct node *statement) {
	assert(NODE_ITERATIVE_STATEMENT == statement->kind);

	// handle type - which is an expression
	int error_count = 0;
	if (statement->data.iterative_statement.type == FOR) {
		error_count += symbol_add_from_expression(table, statement->data.iterative_statement.expression);
	} else {
		error_count += symbol_add_from_expression_list_statement(table, statement->data.iterative_statement.expression);
	}

	// handle body
	if (NULL != statement->data.iterative_statement.statement) {
		error_count += symbol_add_from_possible_statement(table, statement->data.iterative_statement.statement);
	}

	return error_count;
}

static int symbol_add_from_function_definition(struct symbol_table *table, struct node *function) {
	int error_count = 0;
	assert(NULL != function);

	char* name = function->data.function_definition_statement.function_decl->data.function.name->data.identifier.name;
	// check if a declaration for this function exists
	struct symbol *symbol = search_symbol_table(name, table);

	if (NULL != symbol) {
		/* Declaration does exist - now check if the return types and arguments are the same */
		struct type_tree *type_tree = create_type_tree(
			function->data.function_definition_statement.type,
			function->data.function_definition_statement.function_decl
		);

		if (0 != compare_type_trees(symbol->type_tree, type_tree)) {
			compiler_print_error(function->location, "function \"%s\" has already been declared with a different signature", name);
			error_count++;
		} else {
			if (symbol->is_function_defined == 1) {
				compiler_print_error(function->location, "Cannot have multiple function definitions \"%s\"", name);
				error_count++;
			} else {
				symbol->is_function_defined = true;
			}
		}
	} else {
		/* Declaration does not exist - create a new symbol */
		struct type_tree *type_tree = create_type_tree(
			function->data.function_definition_statement.type,
			function->data.function_definition_statement.function_decl
		);
		symbol = symbol_put(table, name, type_tree);
		symbol->is_function_defined = true;
		type_tree->identifier->data.identifier.symbol = symbol;
	}

	while(peek_into_stack()->kind != FILE_SCOPE) {
		/* We have exited the previous function so remove it from stack */
		pop_from_stack();
	}

	struct symbol_table *procedure_symbol_table = symbol_create_table(PROCEDURE_SCOPE);
	procedure_symbol_table->parent = file_scope_table;
	procedure_symbol_table->data.procedure.name = name;
	insert_child(file_scope_table, procedure_symbol_table);

	/* cleanup */
	symbol = NULL;
	free(symbol);

	/* Add the arguments to the symbol table */
	/* Arguments can be NULL */
	if (NULL != function->data.function_definition_statement.function_decl->data.function.argument_list) {
		argument_count = 0;
		error_count += symbol_add_from_arguments_list(
			procedure_symbol_table,
			function->data.function_definition_statement.function_decl->data.function.argument_list,
			false
		);
	}

	
	if (NULL != function->data.function_definition_statement.function_body) {
		if (NODE_COMPOUND_STATEMENT == function->data.function_definition_statement.function_body->kind) {
			error_count += symbol_add_from_compound_statement(
				procedure_symbol_table,
				function->data.function_definition_statement.function_body,
				true
			);
		} else {
			error_count += symbol_add_from_possible_statement(
				procedure_symbol_table,
				function->data.function_definition_statement.function_body
			);
		}
	}

	return error_count;
}

int symbol_add_from_function_call(struct symbol_table *table, struct node *function) {
	assert(NULL != function);

	char* name = function->data.function.name->data.identifier.name;

	int error_count = 0;

	// check if a declaration for this function exists
	struct symbol *symbol = search_symbol_table(name, table);

	if (NULL == symbol) {
		compiler_print_error(function->location, "function \"%s\" does not exist", name);
		error_count++;
	} else {
		function->data.function.symbol = symbol;
		symbol->is_used = 1;
	}

	/* process arguments - can be NULL */
	if (NULL != function->data.function.argument_list) {
		return symbol_add_from_expression_list_statement(table, function->data.function.argument_list);
	}

	return error_count;
}

int symbol_add_from_return_statement(struct symbol_table *table, struct node *statement) {
	assert(NODE_RETURN_STATEMENT == statement->kind);

	if (NULL != statement->data.return_statement.expression) {
		return symbol_add_from_expression_list_statement(table, statement->data.return_statement.expression);
	}
	return 0;
}

int symbol_add_from_possible_statement(struct symbol_table *table, struct node *statement) {
	if (statement == NULL) return 0;

	switch(statement->kind) {
		case NODE_DECLARATION_STATEMENT:
			return symbol_add_from_declaration(table, statement);
		case NODE_FUNCTION_DEFINITION:
			return symbol_add_from_function_definition(table, statement);
		case NODE_EXPRESSION_LIST_STATEMENT:
		case NODE_EXPRESSION_STATEMENT: 
			return symbol_add_from_expression_list_statement(table, statement);
		case NODE_STATEMENT_LIST:
			return symbol_add_from_statement_list(table, statement);
		case NODE_COMPOUND_STATEMENT:
			return symbol_add_from_compound_statement(table, statement, false);
		case NODE_GOTO_STATEMENT: 
			return symbol_add_from_goto_statement(table, statement);
		case NODE_LABEL_STATEMENT: 
			return symbol_add_from_label_statement(table, statement);
		case NODE_ITERATIVE_STATEMENT: 
			return symbol_add_from_iterative_statement(table, statement);
		case NODE_CONDITIONAL_STATEMENT: 
			return symbol_add_from_conditional_statement(table, statement);
		case NODE_BREAK_STATEMENT:
		case NODE_CONTINUE_STATEMENT:
			return 0;
		case NODE_RETURN_STATEMENT:
			return symbol_add_from_return_statement(table, statement);
		default:
			printf("kind %d\n", statement->kind);
			assert(0);
			return 1;
	}
}

int symbol_add_from_statement_list(struct symbol_table *table, struct node *statement_list) {
	int error_count = 0;
	assert(NODE_STATEMENT_LIST == statement_list->kind);

	if (table->kind == FILE_SCOPE) {
		file_scope_table = table;
	}

	if (NULL != statement_list->data.statement_list.init) {
		error_count += symbol_add_from_statement_list(table, statement_list->data.statement_list.init);
	}
	error_count += symbol_add_from_possible_statement(table, statement_list->data.statement_list.statement);
	return error_count;
}

/***********************
 * PRINT SYMBOL TABLES *
 ***********************/

int iterate_into_symbol_tables(struct symbol_table_children *table) {
	static int error_count = 0;
	if (NULL == table || NULL == table->self) {
		return 0;
	}

	if (PROCEDURE_SCOPE == table->self->kind) {
		/* Access the symbol table of this procedure */
		if (NULL != table->self->data.statement_labels.statement_labels) {
			struct statement_labels_list *statement_labels_list = table->self->data.statement_labels.statement_labels;

			/* Confirm that all labels mentioned in the table are marked as DEFINED */
			while(NULL != statement_labels_list) {
				if (!statement_labels_list->statement_label.is_defined) {
					printf("Label \"%s\" is not within a procedure scope\n", statement_labels_list->statement_label.label);
					error_count++;
				}
				statement_labels_list = statement_labels_list->next;
			}

			/* cleanup */
			statement_labels_list = NULL;
			free(statement_labels_list);
		}
	} else {
		printf("other scope %d\n", table->self->kind);
	}
	/* Continue processing other children */
	iterate_into_symbol_tables(table->next);

	return error_count;
}

int check_statement_labels_tables_validity() {
	int error_count = 0;
	assert(NULL != file_scope_table);

	if (NULL == file_scope_table->children) {
		return 0;
	}

	struct symbol_table_children *temp = file_scope_table->children;

	error_count += iterate_into_symbol_tables(temp);

	/* cleanup */
	temp = NULL;
	free(temp);

	return error_count;
}

int iterate_tables(struct symbol_table *table) {
	static int error_count = 0;
	struct symbol_list *iter = table->variables;
	while (NULL != iter) {
		if (iter->symbol.type_tree->type->kind == TYPE_FUNCTION) {
			if (is_syscall(iter->symbol.name)) {
				iter->symbol.is_function_defined = true;
				iter->symbol.is_system_call = true;

				for(int i = 0; i < syscalls_count; i++) {
					if( syscalls_equal(
						iter->symbol.name,
						syscalls[i]->data.function.name->data.identifier.name
					) ) {
						iter->symbol.is_used = 1;
						syscalls[i]->data.function.symbol = &iter->symbol;
					}
				}
			} 
			if (!iter->symbol.is_function_defined) {
				printf("Function \"%s\" is declared but not defined\n", iter->symbol.type_tree->identifier->data.identifier.name);
				error_count++;
			}
		}
		iter = iter->next;
	}

	if (NULL != table->children->self) {
		iterate_tables(table->children->self);
	}

	iter = NULL;
	free(iter);

	return error_count;
}

int check_function_declarations_validity() {
	int error_count = 0;
	assert(NULL != file_scope_table);

	struct symbol_table *temp = file_scope_table;

	error_count += iterate_tables(temp);

	temp = NULL;
	free(temp);

	return error_count;
}

void symbol_print_statement_label_table(FILE *output, struct symbol_table *table) {

	assert(NULL != table);

	struct symbol_table *temp = table;

	if (NULL == temp->data.statement_labels.statement_labels) return;

	struct statement_labels_list *iter = temp->data.statement_labels.statement_labels;
	printf("\nstatement label table (%s):\n", symbol_table_kind_map[temp->kind]);

	while (NULL != iter) {
		fprintf(
			output,
			"  label: %s  ||  id: %u  ||  is_defined: %d  ||  is_used: %d \n",
			iter->statement_label.label, iter->statement_label.id,
			iter->statement_label.is_defined,
			iter->statement_label.is_used
		);
		iter = iter->next;	
	}

	iter = NULL;
	free(iter);

	temp = NULL;
	free(temp);
}

void symbol_print_table_handler(FILE *output, struct symbol_table *table) {
	struct symbol_list *iter = NULL;

	assert(NULL != table);

	/* Print string literal table */
	if (table->kind == FILE_SCOPE) {
		struct string_symbol_list *symbol_list = table->data.string_literals.string_literals;
		printf("\nstring literal table (%s):\n", symbol_table_kind_map[table->data.string_literals.kind]);
		while (NULL != symbol_list) {
			fprintf(output, "  string: %s  ||  id: %s \n", symbol_list->string_symbol.string, symbol_list->string_symbol.id);
			symbol_list = symbol_list->next;
		}
		symbol_list = NULL;
	}

	if (table->kind == PROCEDURE_SCOPE) {
		symbol_print_statement_label_table(output, table);
	}

	printf("\nsymbol table (%s) ||  ", symbol_table_kind_map[table->kind]);
	if (NULL != table->parent) {
		printf("parent (%s):\n", symbol_table_kind_map[table->parent->kind]);
	} else {
		printf("parent (NULL):\n");
	}

	iter = table->variables;
	while (NULL != iter) {
		if (!iter->symbol.is_only_for_saving_type) {
			fprintf(
				output,
				"  variable: %s  ||  type: %s  ||  is_used: %d  ||  is_function_defined: %d  ||  id: %u \n",
					iter->symbol.name, get_return_type(iter->symbol.type_tree),
					iter->symbol.is_used, iter->symbol.is_function_defined, iter->symbol.id
				);
		}
		iter = iter->next;
	}

	iter = NULL;
	free(iter);


	if (NULL != table->children->self) {
		symbol_print_table_handler(output, table->children->self);
	}

	struct symbol_table_children *temp = table->children->next;
	while(true) {
		if (NULL == temp) break;
		symbol_print_table_handler(output, temp->self);
		temp = temp->next;
	}
	temp = NULL;
	free(temp);
}

void symbol_print_table(FILE *output) {
	assert(NULL != file_scope_table);

	struct symbol_table *temp = file_scope_table;

	symbol_print_table_handler(output, temp);

	temp = NULL;
	free(temp);
}


void symbol_print_table_assignment_format_handler(FILE *output, struct symbol_table *table) {
	assert(NULL != table);

	struct symbol_list *iter = table->variables;

	while(NULL != iter) {
		if (!iter->symbol.is_only_for_saving_type) {
			print_type_tree_assignment_format(iter->symbol.type_tree);
			printf("\n");
		}
		iter = iter->next;
	}

	iter = NULL;
	free(iter);

	if (NULL != table->children->self) {
		symbol_print_table_assignment_format_handler(output, table->children->self);
	}

	struct symbol_table_children *temp = table->children->next;
	while(true) {
		if (NULL == temp) break;
		symbol_print_table_assignment_format_handler(output, temp->self);
		temp = temp->next;
	}
	temp = NULL;
	free(temp);

}

void symbol_print_table_assignment_format(FILE *output) {
	assert(NULL != file_scope_table);

	struct symbol_table *temp = file_scope_table;

	symbol_print_table_assignment_format_handler(output, temp);

	temp = NULL;
	free(temp);
}

struct string_symbol_list* symbol_get_string_table() {
	return file_scope_table->data.string_literals.string_literals;
}

struct symbol_table* symbol_get_global_symbol_table() {
	return file_scope_table;
}

static struct symbol_table* symbol_get_procedure_table_handler(
	struct symbol_table_children* table,
	char *procedure_name
) {
	if (NULL == table) return NULL;

	if (PROCEDURE_SCOPE == table->self->kind) {
		if (0 == strcmp(table->self->data.procedure.name, procedure_name)) {
			return table->self;
		}
	}
	return symbol_get_procedure_table_handler(table->next, procedure_name);
}

struct symbol_table* symbol_get_procedure_table(char *procedure_name) {
	assert(NULL != file_scope_table);

	struct symbol_table *temp = file_scope_table;

	struct symbol_table *table = symbol_get_procedure_table_handler(temp->children, procedure_name);

	temp = NULL;
	free(temp);

	return table;
}

struct symbol* search_symbol_table_by_id(struct symbol_table *table, int variable_id) {
	if (NULL == table) {
		/* Variable does not exist */
		printf("null table\n");
		return NULL;
	}
	struct symbol *symbol = NULL;
	symbol = symbol_get_by_id(
		table,
		variable_id
	);

	if (NULL == symbol) {
		/* search parent table */
		struct symbol_table_children *temp = table->children->next;
		while(true) {
			if (NULL == temp) break;
			symbol = search_symbol_table_by_id(temp->self, variable_id);
			if (NULL != symbol) { printf("found!\n"); break; }
			temp = temp->next;
		}
		temp = NULL;
		free(temp);
	} else {
		printf("found symbol %s\n", symbol->name);
		return symbol;
	}
	return symbol;
}
