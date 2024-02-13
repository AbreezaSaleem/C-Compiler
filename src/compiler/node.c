#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <regex.h>

#include "node.h"
#include "symbol.h"
#include "type.h"
#include "utils.h"

extern int annotation;
int string_id = 0;

/***************************
 * CREATE PARSE TREE NODES *
 ***************************/

/* Allocate and initialize a generic node. */
static struct node *node_create(enum node_kind kind, YYLTYPE location) {
  struct node *n;

  n = malloc(sizeof(struct node));
  assert(NULL != n);

  n->kind = kind;
  n->location = location;

  n->ir_list = NULL;
  return n;
}

static void assign_parent(struct node *child, struct node *parent) {
  if (NULL != child) {
    child->parent = parent;
  }
}


int compare_nodes(struct node *node_1, struct node *node_2) {
  if (
    node_1->location.first_column == node_2->location.first_column &&
    node_1->location.first_line == node_2->location.first_line &&
    node_1->location.last_column == node_2->location.last_column &&
    node_1->location.last_line == node_2->location.last_line
  ) {
    return 0;
  } else {
    return 1;
  }
}

/*
 * node_identifier - allocate a node to represent an identifier
 *
 * Parameters:
 *   text - string - contains the name of the identifier
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 *
 */
struct node *node_identifier(YYLTYPE location, char *text, int length) {
  struct node *node = node_create(NODE_IDENTIFIER, location);
  if (length > IDENTIFIER_MAX) {
    node->data.identifier.overflow = true;
  } else {
    memset(node->data.identifier.name, 0, IDENTIFIER_MAX + 1);
    strncpy(node->data.identifier.name, text, length);
    node->data.identifier.overflow = false;
  }
  node->data.identifier.symbol = NULL;
  node->data.identifier.result.type = NULL; // fetch type through the type tree
  node->data.identifier.result.value = L_VALUE;
  return node;
}

/*
 * node_identifier - allocate a node to represent an identifier
 *
 * Parameters:
 *   text - string - contains the name of the identifier
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 *
 */
struct node *node_string(YYLTYPE location, char *text, int length) {
  struct node *node = node_create(NODE_STRING, location);
  if ((length - 2) > STRING_MAX) { // subtracting 2 because we can't count the double quotes
    node->data.string.overflow = true;
    node->data.string.symbol = NULL;
    node->data.binary_operation.result.type = NULL;
    node->data.binary_operation.result.ir_operand = NULL;
    node->data.identifier.result.value = R_VALUE;
  } else {
    /*
     * Convert string to ASCII
     * we're subtracting 2 from length because
     * the text contains the extra double quotes
     * then we're adding a 1 to cater for the terminating character
    */
    const int arraySize = length - 2 + 1;
    int stringInASCII[arraySize];
    int index = 0;
    int escapeCharCount = 0;
    /* Replace any escape codes you encounter with their ASCII representation */
    for (int size = 1; size < (length - 1); size++) {
      if (size < (length - 2) && text[size] == '\\') {

        /* HANDLE OCTAL ESCAPE CODES */
        if (size + 3 < length) {
          if (isdigit(text[size + 1]) && isdigit(text[size + 2]) && isdigit(text[size + 3])) {
            int octalCode = 
              ((text[size + 1] - '0')*100) +
              ((text[size + 2] - '0')*10) +
              (text[size + 3] - '0');
            if (octalCode > 377) {
              node->data.string.octal_overflow = true;
            }
          }
        }
        /* HANDLE OCTAL ESCAPE CODES END */


        /* HANDLE ESCAPE CODES
         * Look ahead to see if the next character is an escape code
         */
        size++;
        bool isEscapeChar = false;
        int escapeCodeIndex;
        for (int size1 = 0; size1 < ESCAPE_CHAR_LENGTH; size1++) {
          if (text[size] == escapeCodes[size1]) {
            isEscapeChar = true;
            escapeCodeIndex = size1;
            break;
          }
        }
        if (isEscapeChar) {
          stringInASCII[index] = escapeCharactersASCII[escapeCodeIndex];
          index++;
          escapeCharCount++;
          continue;
        } else {
          size--;
        }
      }
      /* HANDLE ALL ESCAPE CODES END */

      stringInASCII[index] = text[size];
      index++;
    }
    /* Add null terminator */
    stringInASCII[index] = 0;
    

    int * newArray = malloc((arraySize - escapeCharCount) * sizeof(int));
    memcpy(newArray, stringInASCII, (arraySize - escapeCharCount) * sizeof(int));

    node->data.string.stringInASCII = newArray;
    node->data.string.overflow = false;
    node->data.string.length = arraySize - escapeCharCount;
    node->data.string.symbol = NULL;
    node->data.binary_operation.result.type = NULL;
    node->data.binary_operation.result.ir_operand = NULL;
    node->data.identifier.result.value = R_VALUE;
    strncpy(node->data.string.value, text, length);
  }
  return node;
}

/*
 * node_character - allocate a node to represent a number
 *
 * Parameters:
 *   text - string - contains the numeric literal
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 */

struct node *node_character(YYLTYPE location, char *text, int length) {

  int charInASCII_int = text[1];
  if (length > 3) {
    /* Convert the char to its ASCII representation
     * Find ASCII code of the escape code from the escapeCodes array
    */
    char escapeCode = text[2];
    for (int size = 0; size < ESCAPE_CHAR_LENGTH; size++) {
      if (escapeCode == escapeCodes[size]) {
        charInASCII_int = escapeCharactersASCII[size];
      }
    }
  }

  char charInASCII_char[3 + 1];  // set the length to 3 because max ASCII is 255
  sprintf(charInASCII_char, "%d", charInASCII_int);
  struct node *node = node_create(NODE_NUMBER, location);
  errno = 0;
  node->data.number.value = strtoul(charInASCII_char, NULL, 10);
  if (node->data.number.value == ULONG_MAX && ERANGE == errno) {
    /* Strtoul indicated overflow. */
    node->data.number.overflow = true;
    node->data.number.result.type = type_basic(false, TYPE_BASIC_UNDEF);
  } else {
    node->data.number.overflow = false;
    node->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
  }
  node->data.number.result.ir_operand = NULL;
  node->data.identifier.result.value = R_VALUE;
  return node;
}

/*
 * node_number - allocate a node to represent a number
 *
 * Parameters:
 *   text - string - contains the numeric literal
 *   length - integer - the length of text (not including terminating NUL)
 *
 * Side-effects:
 *   Memory may be allocated on the heap.
 */

struct node *node_number(YYLTYPE location, char *text, int length) {
  struct node *node = node_create(NODE_NUMBER, location);
  errno = 0;

  /* CHECK LEADING ZEROS */
  if (text[0] == '0' && text[1] == '0') {
    node->data.number.leading_zeros = true;
    node->data.number.result.type = type_basic(false, TYPE_BASIC_UNDEF);
  } else {
    node->data.number.value = strtoul(text, NULL, 10);
    if (node->data.number.value == ULONG_MAX && ERANGE == errno) {
      /* Strtoul indicated overflow. */
      node->data.number.overflow = true;
      node->data.number.result.type = type_basic(false, TYPE_BASIC_UNDEF);
    } else if (node->data.number.value > _UNSIGNED_LONG_MAX) {
      /* Out of range */
      node->data.number.overflow = true;
      node->data.number.result.type = type_basic(false, TYPE_BASIC_UNDEF);
    } else if (node->data.number.value <= _SIGNED_INT_MAX) {
      /* Value is in range for signed int type. */
      node->data.number.overflow = false;
      node->data.number.result.type = type_basic(false, TYPE_BASIC_INT);
    } else if (node->data.number.value <= _SIGNED_LONG_MAX) {
      /* Value is in range for signed long type. */
      node->data.number.overflow = false;
      node->data.number.result.type = type_basic(false, TYPE_BASIC_LONG);
    } else if (node->data.number.value <= _UNSIGNED_INT_MAX && node->data.number.value >= _UNSIGNED_INT_MIN) {
      /* Value is in range for unsigned int type. */
      node->data.number.overflow = false;
      node->data.number.result.type = type_basic(true, TYPE_BASIC_INT);
    } else if (node->data.number.value <= _UNSIGNED_LONG_MAX && node->data.number.value >= _UNSIGNED_LONG_MIN) {
      /* Value is in range for unsigned long type. */
      node->data.number.overflow = false;
      node->data.number.result.type = type_basic(true, TYPE_BASIC_LONG);
    } else {
      /* Default handler */
      node->data.number.overflow = true;
      node->data.number.result.type = type_basic(false, TYPE_BASIC_UNDEF);
    }
    node->data.number.leading_zeros = false;
  }

  node->data.identifier.result.value = R_VALUE;
  node->data.number.result.ir_operand = NULL;
  return node;
}

struct node *node_binary_operation(YYLTYPE location,
                                   enum node_binary_operation operation,
                                   struct node *left_operand,
                                   struct node *right_operand)
{
  struct node *node = node_create(NODE_BINARY_OPERATION, location);
  assign_parent(left_operand, node);
  assign_parent(right_operand, node);
  node->data.binary_operation.operation = operation;
  node->data.binary_operation.left_operand = left_operand;
  node->data.binary_operation.right_operand = right_operand;
  node->data.binary_operation.result.type = NULL;
  node->data.binary_operation.result.ir_operand = NULL;
  node->data.binary_operation.result.value = NOT_AVAIALABLE;
  node->data.binary_operation.compound_operator = NULL;
  return node;
}

struct node *node_unary_expression(YYLTYPE location, enum node_unary_operation operation,
                                  struct node *expression) {
  struct node *node = node_create(NODE_UNARY_EXPR, location);
  assign_parent(expression, node);
  node->data.unary_expr.is_subscript_operator = false;
  node->data.unary_expr.expression = expression;
  node->data.unary_expr.operation = operation;
  node->data.unary_expr.result.type = NULL;
  node->data.unary_expr.result.ir_operand = NULL;
  node->data.unary_expr.result.value = NOT_AVAIALABLE;
  node->data.unary_expr.compound_operator = NULL;
  return node;
}

struct node *node_array(YYLTYPE location, struct node *name, struct node *size) {
  struct node *node = node_create(NODE_ARRAY, location);
  if (NULL != size) {
    struct node *expression =  node_expression_list_statement(location, size, NULL);
    assign_parent(name, node);
    assign_parent(expression, node);
    node->data.array.name = name;
    node->data.array.size = expression;
  } else {
    assign_parent(name, node);
    node->data.array.name = name;
    node->data.array.size = NULL;
  }
  return node;
}

struct node *node_subscript_expr(YYLTYPE location, struct node *arr, struct node *index) {
  struct node *node = node_unary_expression(location, UNARY_POINTER, arr);
  struct node *binop =  node_binary_operation(location, BINOP_ADDITION, arr, index);
  struct node *expression =  node_expression_list_statement(location, binop, NULL);
  node->data.unary_expr.is_subscript_operator = true;
  node->data.unary_expr.result.value = L_VALUE;
  assign_parent(expression, node);
  node->data.unary_expr.expression = expression;
  return node;
}

struct node *node_pointer(YYLTYPE location, struct node *pointes_to) {
  struct node *node = node_create(NODE_POINTER, location);
  assign_parent(pointes_to, node);
  node->data.pointer.points_to = pointes_to;
  return node;
}

struct node *node_function_definition(
  YYLTYPE location, struct node *type, struct node *function_decl, struct node *function_body, bool is_type_pointer
) {
  struct node *node = node_create(NODE_FUNCTION_DEFINITION, location);
  if (is_type_pointer) {
    type->data.type.type_node->kind = TYPE_POINTER;
    struct node *type_pointer = node_pointer(location, type);
    assign_parent(type_pointer, node);
    node->data.function_definition_statement.type = type_pointer;
  } else {
    assign_parent(type, node);
    node->data.function_definition_statement.type = type;
  }
  assign_parent(function_decl, node);
  assign_parent(function_body, node);
  node->data.function_definition_statement.label_name = "";
  node->data.function_definition_statement.function_decl = function_decl;
  node->data.function_definition_statement.function_body = function_body;
  return node;
}

struct node *node_function(
  YYLTYPE location, struct node *name, struct node *argument_list, bool is_declaration
) {
  struct node *node = node_create(NODE_FUNCTION_DECL, location);
  assign_parent(name, node);
  assign_parent(argument_list, node);
  node->data.function.name = name;
  node->data.function.argument_list = argument_list;
  node->data.function.result.type = NULL;
  node->data.function.result.value = is_declaration ? NOT_AVAIALABLE : R_VALUE;
  return node;
}

struct node *node_cast_expression(YYLTYPE location, struct node *type, struct node *expression) {
  struct node *node = node_create(NODE_CAST_EXPR, location);
  assign_parent(type, node);
  assign_parent(expression, node);
  node->data.cast_expr.type = type;
  node->data.cast_expr.expression = expression;
  node->data.cast_expr.result.value = R_VALUE;
  node->data.cast_expr.result.ir_operand = NULL;
  return node;
}

struct node *node_for_expression(
  YYLTYPE location, struct node *initializer,
  struct node *condition, struct node *bound
) {
  struct node *node = node_create(NODE_FOR_EXPR, location);
  assign_parent(initializer, node);
  assign_parent(condition, node);
  assign_parent(bound, node);
  node->data.for_expr.initializer = initializer;
  node->data.for_expr.condition = condition;
  node->data.for_expr.bound = bound;
  return node;
}

struct node *node_expression_statement(YYLTYPE location, struct node *expression) {
  struct node *node = node_create(NODE_EXPRESSION_STATEMENT, location);
  assign_parent(expression, node);
  node->data.expression_statement.expression = expression;
  return node;
}

struct node *node_expression_list_statement(YYLTYPE location, struct node *self, struct node *next) {
  struct node *node = node_create(NODE_EXPRESSION_LIST_STATEMENT, location);
  assign_parent(self, node);
  assign_parent(next, node);
  node->data.expression_list_statement.self = self;
  node->data.expression_list_statement.next = next;
  return node;
}

struct node *node_declaration_statement(YYLTYPE location, struct node *type, struct node *declaration) {
  struct node *node = node_create(NODE_DECLARATION_STATEMENT, location);
  assign_parent(type, node);
  assign_parent(declaration, node);
  node->data.declaration_statement.type = type;
  node->data.declaration_statement.declaration = declaration;
  return node;
}

struct node *node_pointer_argument(YYLTYPE location, struct node *type) {
  struct node *node = node_create(NODE_DECLARATION_STATEMENT, location);
  assign_parent(type, node);

  type->data.type.type_node->kind = TYPE_POINTER;
  struct node *type_pointer = node_pointer(location, type);
  assign_parent(type_pointer, node);
  node->data.declaration_statement.type = type_pointer;
  node->data.declaration_statement.declaration = NULL;
  return node;
}

struct node *node_declaration_list_statement(YYLTYPE location, struct node *self, struct node *next) {
  struct node *node = node_create(NODE_DECLARATION_LIST_STATEMENT, location);
  assign_parent(self, node);
  assign_parent(next, node);
  node->data.declaration_list_statement.self = self;
  node->data.declaration_list_statement.next = next;
  return node;
}

struct node *node_arguments_list(YYLTYPE location, struct node *self, struct node *next) {
  struct node *node = node_create(NODE_ARGUMENTS_LIST, location);
  assign_parent(self, node);
  assign_parent(next, node);
  node->data.arguments_list.self = self;
  node->data.arguments_list.next = next;
  return node;
}

struct node *node_statement_list(YYLTYPE location,
                                 struct node *init,
                                 struct node *statement) {
  struct node *node = node_create(NODE_STATEMENT_LIST, location);
  assign_parent(init, node);
  assign_parent(statement, node);
  node->data.statement_list.init = init;
  node->data.statement_list.statement = statement;
  return node;
}

struct node *node_compound_statement(YYLTYPE location, struct node *statement) {
  struct node *node = node_create(NODE_COMPOUND_STATEMENT, location);
  assign_parent(statement, node);
  node->data.compound_statement.statement = statement;
  return node;
}

struct node *node_iterative_statement(
  YYLTYPE location, enum iterative_type type, struct node *expression, struct node *statement
) {
  struct node *node = node_create(NODE_ITERATIVE_STATEMENT, location);
  assign_parent(expression, node);
  assign_parent(statement, node);
  node->data.iterative_statement.type = type;
  node->data.iterative_statement.expression = expression;
  node->data.iterative_statement.statement = statement;
  return node;
}

struct node *node_conditional_statement(
  YYLTYPE location, bool is_ternary, struct node *condition,
  struct node *true_block, struct node *false_block) {
  struct node *node = node_create(NODE_CONDITIONAL_STATEMENT, location);
  assign_parent(condition, node);
  assign_parent(true_block, node);
  assign_parent(false_block, node);
  node->data.conditional_statement.condition = condition;
  node->data.conditional_statement.is_ternary = is_ternary;
  node->data.conditional_statement.true_block = true_block;
  node->data.conditional_statement.false_block = false_block;
  node->data.conditional_statement.result.type = NULL;
  node->data.conditional_statement.result.value = is_ternary ? R_VALUE : NOT_AVAIALABLE;
  return node;
}

struct node *node_label_statement(YYLTYPE location, struct node *label, struct node *statement) {
  struct node *node = node_create(NODE_LABEL_STATEMENT, location);
  assign_parent(label, node);
  assign_parent(statement, node);
  node->data.label_statement.label = label;
  node->data.label_statement.statement = statement;
  return node;
}

struct node *node_goto_statement(YYLTYPE location, struct node *label) {
  struct node *node = node_create(NODE_GOTO_STATEMENT, location);
  assign_parent(label, node);
  node->data.goto_statement.label = label;
  return node;
}

struct node *node_null_statement(struct node *error_stmt, YYLTYPE location) {
  return node_create(NODE_NULL_STATEMENT, location);
}

struct node *node_break_statement(YYLTYPE location) {
  return node_create(NODE_BREAK_STATEMENT, location);
}

struct node *node_continue_statement(YYLTYPE location) {
  return node_create(NODE_CONTINUE_STATEMENT, location);
}

struct node *node_type(YYLTYPE location, enum type_basic_kind type, bool is_unsigned) {
  struct node *node = node_create(NODE_TYPE, location);
  node->data.type.type_node = type_basic(is_unsigned, type);
  return node;
}

struct node *node_return_statement(YYLTYPE location, struct node *expression) {
  struct node *node = node_create(NODE_RETURN_STATEMENT, location);
  assign_parent(expression, node);
  node->data.return_statement.expression = expression;
  return node;
}

struct result *node_get_result(struct node *expression) {
  switch (expression->kind) {
    case NODE_NUMBER:
      return &expression->data.number.result;
    case NODE_IDENTIFIER:
      return &expression->data.identifier.result;
    case NODE_STRING: 
      return &expression->data.string.result;
    case NODE_BINARY_OPERATION:
      return &expression->data.binary_operation.result;
    case NODE_FUNCTION_DECL:
      /* using symbol->result instead of directly using result because
       * symbol is used for function calls
       */
      return &expression->data.function.symbol->result;
      break;
    case NODE_CONDITIONAL_STATEMENT:
      return node_get_result(expression->data.conditional_statement.true_block);
    case NODE_UNARY_EXPR:
      return node_get_result(expression->data.unary_expr.expression);
    case NODE_CAST_EXPR: 
      return node_get_result(expression->data.cast_expr.expression);
    case NODE_EXPRESSION_LIST_STATEMENT: 
    case NODE_EXPRESSION_STATEMENT: {
      if (NULL == expression->data.expression_list_statement.next) {
        return node_get_result(expression->data.expression_list_statement.self);
      } else {
        return node_get_result(expression->data.expression_list_statement.next);
      }
    }      
    default:
      assert(0);
      return NULL;
  }
}

struct result *node_assign_result(enum value value, struct type *type, struct node *expression) {
  switch (expression->kind) {
    case NODE_NUMBER:
      expression->data.number.result.value = value;
      expression->data.number.result.type = type;
      break;
    case NODE_IDENTIFIER:
      expression->data.identifier.result.value = value;
      expression->data.identifier.result.type = type;
      break;
    case NODE_STRING: 
      expression->data.string.result.value = value;
      expression->data.string.result.type = type;
      break;
    case NODE_BINARY_OPERATION:
      expression->data.binary_operation.result.value = value;
      expression->data.binary_operation.result.type = type;
      break;
    case NODE_FUNCTION_DECL:
      /* using symbol->result instead of directly using result because
       * symbol is used for function calls
       */
      expression->data.function.symbol->result.value = value;
      expression->data.function.symbol->result.type = type;
      break;
    case NODE_UNARY_EXPR:
      expression->data.unary_expr.result.value = value;
      expression->data.unary_expr.result.type = type;
      break;
    case NODE_CAST_EXPR:
      expression->data.cast_expr.result.value = value;
      expression->data.cast_expr.result.type = type;
      break;
    case NODE_CONDITIONAL_STATEMENT:
      node_assign_result(value, type, expression->data.conditional_statement.false_block);
      return node_assign_result(value, type, expression->data.conditional_statement.true_block);
    case NODE_EXPRESSION_STATEMENT: 
      return node_assign_result(value, type, expression->data.expression_statement.expression);
    case NODE_EXPRESSION_LIST_STATEMENT: {
      if (NULL == expression->data.expression_list_statement.next) {
        node_assign_result(value, type, expression->data.expression_list_statement.self);
        break;
      } else {
        node_assign_result(value, type, expression->data.expression_list_statement.next);
        break;
      }
    }      
    default:
      assert(0);
      return NULL;
  }
  return NULL;
}

/**************************
 * PRINT PARSE TREE NODES *
 **************************/


void node_print_statement_opts(FILE *output, struct node *statement) {
  assert(NULL != statement);

  switch (statement->kind) {
    case NODE_EXPRESSION_STATEMENT: 
      printf("{");
      node_print_expression_list_statement(output, statement);
      printf(";}");
      break;
    case NODE_COMPOUND_STATEMENT: 
      node_print_compound_statement(output, statement);
      break;
    case NODE_CONDITIONAL_STATEMENT: 
      printf("{");
      node_print_conditional_statement(output, statement);
      printf("}");
      break;
    case NODE_BREAK_STATEMENT: 
      printf("{");
      fprintf(output, "break;");
      printf("}");
      break;
    case NODE_CONTINUE_STATEMENT: 
      printf("{");
      fprintf(output, "continue;");
      printf("}");
      break;
    default:
      assert(0);
      break;
  }
}

void node_print_conditional_expression_opts(FILE *output, struct node *expression) {
  assert(NULL != expression);

  switch (expression->kind) {
    case NODE_BINARY_OPERATION: 
      node_print_binary_operation(output, expression);
      break;
    case NODE_CAST_EXPR: 
      node_print_cast_expression(output, expression);
      break;
    case NODE_UNARY_EXPR: 
      node_print_unary_expression(output, expression);
      break;
    case NODE_IDENTIFIER:
      node_print_identifier(output, expression);
      break;
    case NODE_NUMBER:
      node_print_number(output, expression);
      break;
    case NODE_STRING:
      node_print_string(output, expression);
      break;
    case NODE_CONDITIONAL_STATEMENT:
      node_print_conditional_statement(output, expression);
      break;
    case NODE_EXPRESSION_LIST_STATEMENT:
      node_print_expression_list_statement(output, expression);
      break;
    default:
      assert(0);
      break;
  }
}


void node_print_binary_operation(FILE *output, struct node *binary_operation) {
  static const char *binary_operators[] = {
    "*",    /*  0 = BINOP_MULTIPLICATION */
    "/",    /*  1 = BINOP_DIVISION */
    "+",    /*  2 = BINOP_ADDITION */
    "%",    /*  3 = BINOP_MOD */
    "-",    /*  4 = BINOP_SUBTRACTION */
    "=",    /*  5 = BINOP_ASSIGN */
    "||",   /*  6 = LOGICAL_OR */
    "&&",   /*  7 = LOGICAL_AND */
    "|",    /*  8 = BITWISE_OR */
    "^",    /*  9 = BITWISE_XOR */
    "&",    /* 10 = BITWISE_AND */
    "==",   /* 11 = EQUALITY_OP */
    "!=",   /* 12 = INEQUALITY_OP */
    "<",    /* 13 = LESS_THAN */
    "<=",   /* 14 = LESS_THAN_EQ */
    ">",    /* 15 = GREATER_THAN */
    ">=",   /* 16 = GREATER_THAN_EQ */
    "<<",   /* 17 = LEFT_SHIFT */
    ">>",   /* 18 = RIGHT_SHIFT */
    "+=",   /* 19 = BINOP_PLUS_EQUAL, */
    "-=",   /* 20 = BINOP_MINUS_EQUAL */
    "*=",   /* 21 = BINOP_MULTIPLY_EQUAL */
    "/=",   /* 22 = BINOP_DIVIDE_EQUAL */
    "%=",   /* 23 = BINOP_MODULO_EQUAL */
    "<<=",  /* 24 = BINOP_LEFT_SHIFT_EQUAL */
    ">>=",  /* 25 = BINOP_RIGHT_SHIFT_EQUAL */
    "&=",   /* 26 = BINOP_BITWISE_AND_EQUAL */
    "^=",   /* 27 = BINOP_BITWISE_XOR_EQUAL */
    "|=" ,  /* 28 = BINOP_BITWISE_OR_EQUAL */
    NULL
  };

  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  fputs("(", output);
  node_print_expression(output, binary_operation->data.binary_operation.left_operand);
  if (NULL != binary_operation->data.binary_operation.compound_operator) {
    fprintf(output, "/*result:(%s)*/", get_type_string(binary_operation->data.binary_operation.compound_operator->result_type));
    fprintf(output, "/*operand:(%s)*/", get_type_string(binary_operation->data.binary_operation.compound_operator->operand_type));
  }
  fputs(" ", output);
  fputs(binary_operators[binary_operation->data.binary_operation.operation], output);
  fputs(" ", output);
  node_print_expression(output, binary_operation->data.binary_operation.right_operand);
  fputs(")", output);
}

void node_print_unary_expression(FILE *output, struct node *expression) {
  static const char *unary_operators[] = {
    "-",    /*  0 = UNARY_HYPHEN */
    "+",    /*  1 = UNARY_PLUS */
    "!",    /*  2 = UNARY_LOGICAL_NEGATION */
    "~",    /*  3 = UNARY_BITWISE_NEGATION */
    "&",    /*  4 = UNARY_ADDRESS */
    "*",    /*  5 = UNARY_POINTER */
    "++",   /*  6 = UNARY_PREINC */
    "--",   /*  7 = UNARY_PREDEC */
    "++",   /*  8 = UNARY_POSTINC */
    "--",   /*  9 = UNARY_POSTDEC */
    NULL
  };

  assert(NODE_UNARY_EXPR == expression->kind);

  fputs("( ", output);
  // for ++ and -- expressions
  if (expression->data.unary_expr.operation == UNARY_POSTINC || expression->data.unary_expr.operation == UNARY_POSTDEC) {
    node_print_expression(output, expression->data.unary_expr.expression);
    if (NULL != expression->data.unary_expr.compound_operator) {
      fprintf(output, "/*result:(%s)*/", get_type_string(expression->data.unary_expr.compound_operator->result_type));
      fprintf(output, "/*operand:(%s)*/", get_type_string(expression->data.unary_expr.compound_operator->operand_type));
    }
    fputs(unary_operators[expression->data.unary_expr.operation], output);
  } else {
    fputs(unary_operators[expression->data.unary_expr.operation], output);
    if (NULL != expression->data.unary_expr.compound_operator) {
      fprintf(output, "/*result:(%s)*/", get_type_string(expression->data.unary_expr.compound_operator->result_type));
      fprintf(output, "/*operand:(%s)*/", get_type_string(expression->data.unary_expr.compound_operator->operand_type));
    }
    if (
      NODE_EXPRESSION_LIST_STATEMENT == expression->data.unary_expr.expression->kind
      || NODE_EXPRESSION_STATEMENT == expression->data.unary_expr.expression->kind
    ) {
      node_print_expression_list_statement(output, expression->data.unary_expr.expression);
    } else {
      node_print_expression(output, expression->data.unary_expr.expression);
    }
  }
  fputs(")", output);
}


void node_print_number(FILE *output, struct node *number) {
  assert(NODE_NUMBER == number->kind);

  fprintf(output, "%lu", number->data.number.value);
}

void node_print_type(FILE *output, struct node *type) {
  assert(NODE_TYPE == type->kind);

  type_print(output, type->data.type.type_node);
  fputs(" ", output);
}

void node_print_cast_pointers(FILE *output, struct node *points_to) {
  assert(NULL != points_to);

  switch(points_to->data.pointer.points_to->kind) {
    case NODE_POINTER:
      node_print_cast_pointers(output, points_to->data.pointer.points_to);
      break;
    case NODE_TYPE: 
      node_print_type(output, points_to->data.pointer.points_to);
      break;
    default:
      assert(0);
      break;
  }
  printf("*");
}

void node_print_array_pointers(FILE *output, struct node *points_to) {
  assert(NULL != points_to);

  printf("*");
  switch(points_to->data.pointer.points_to->kind) {
    case NODE_POINTER:
      node_print_array_pointers(output, points_to->data.pointer.points_to);
      break;
    case NODE_IDENTIFIER: 
      node_print_identifier(output, points_to->data.pointer.points_to);
      break;
    case NODE_ARRAY: 
      node_print_array(output, points_to->data.pointer.points_to);
      break;
    default:
      assert(0);
      break;
  }
}

void node_print_pointers(FILE *output, struct node *points_to) {
  assert(NULL != points_to);
  printf("(* ");

  switch(points_to->data.pointer.points_to->kind) {
    case NODE_POINTER:
      node_print_pointers(output, points_to->data.pointer.points_to);
      break;
    case NODE_IDENTIFIER:
      node_print_identifier(output, points_to->data.pointer.points_to);
      break;
    case NODE_ARRAY: 
      node_print_array(output, points_to->data.pointer.points_to);
      break;
    case NODE_FUNCTION_DECL: 
      node_print_function_decl(output, points_to->data.pointer.points_to);
      break;
    default:
      assert(0);
      break;
  }
  printf(")");
}

/*
 * After the symbol table pass, we can print out the symbol address
 * for each identifier, so that we can compare instances of the same
 * variable and ensure that they have the same symbol.
 */
void node_print_identifier(FILE *output, struct node *identifier) {
  assert(NODE_IDENTIFIER == identifier->kind);

  if (annotation == 1) {
    if (identifier->data.identifier.symbol != NULL) {
      fprintf(output, "%s /* %s */", identifier->data.identifier.name,
              identifier->data.identifier.symbol->symbol_table_tag);
    }
  } else {
    fprintf(output, "%s", identifier->data.identifier.name);
  }
}

void node_print_string(FILE *output, struct node *string) {
  assert(NODE_STRING == string->kind);

  printf("\"");
  print_string_node(string);
  printf("\"");
}

void node_print_array(FILE *output, struct node *array) {
  // print array name
  printf("(");
  switch (array->data.array.name->kind) {
    case NODE_ARRAY:
      node_print_array(output, array->data.array.name);
      break;
    case NODE_IDENTIFIER: 
      node_print_identifier(output, array->data.array.name);
      break;
    case NODE_POINTER: 
      printf("(");
      node_print_array_pointers(output, array->data.array.name);
      printf(")");
      break;
    default:
      assert(0);
      break;
  }

  // print array size
  printf("[");
  if (NULL != array->data.array.size) {
    node_print_expression_list_statement(output, array->data.array.size);
  }
  printf("]");
  printf(")");
}

void node_print_return_statement(FILE *output, struct node *expression) {
  assert(NULL != expression);
  printf("return ");

  // print return value
  if (NULL != expression->data.return_statement.expression) {
    switch(expression->data.return_statement.expression->kind) {
      case NODE_CAST_EXPR:
        node_print_cast_expression(output, expression->data.return_statement.expression);
        break;
      case NODE_EXPRESSION_LIST_STATEMENT: 
      case NODE_EXPRESSION_STATEMENT: 
        node_print_expression_list_statement(output, expression->data.return_statement.expression);
        break;
      default: 
        assert(0);
        break;
    }
  }
  printf(";");
}

void node_print_cast_expression(FILE *output, struct node *expression) {
  assert(NULL != expression);
  // print type
  printf(" (");
  switch (expression->data.cast_expr.type->kind) {
    case NODE_TYPE:
      node_print_type(output, expression->data.cast_expr.type);
      break;
    case NODE_POINTER:
      node_print_cast_pointers(output, expression->data.cast_expr.type);
      break;
    default:
      assert(0);
      break;
  }
  printf(")");


  // print expression
  switch (expression->data.cast_expr.expression->kind) {
    case NODE_IDENTIFIER:
      node_print_identifier(output, expression->data.cast_expr.expression);
      break;
    case NODE_NUMBER:
      node_print_number(output, expression->data.cast_expr.expression);
      break;
    case NODE_STRING:
      node_print_string(output, expression->data.cast_expr.expression);
      break;
    case NODE_UNARY_EXPR:
      node_print_unary_expression(output, expression->data.cast_expr.expression);
      break;
    case NODE_EXPRESSION_STATEMENT:
    case NODE_EXPRESSION_LIST_STATEMENT:
      node_print_expression_list_statement(output, expression->data.cast_expr.expression);
      break;
    case NODE_BINARY_OPERATION:
      node_print_binary_operation(output, expression->data.cast_expr.expression);
      break;
    case NODE_CAST_EXPR: 
      node_print_cast_expression(output, expression->data.cast_expr.expression);
      break;
    case NODE_FUNCTION_DECL: 
      node_print_function_decl(output, expression->data.cast_expr.expression);
      break;
    default:
      assert(0);
      break;
  }
}

void node_print_declaration_options(FILE *output, struct node *declaration_options) {
  assert(NULL != declaration_options);
  switch (declaration_options->kind) {
    case NODE_ARRAY:
      node_print_array(output, declaration_options);
      break;
    case NODE_FUNCTION_DECL:
      node_print_function_decl(output, declaration_options);
      break;
    case NODE_IDENTIFIER:
      node_print_identifier(output, declaration_options);
      break;
    case NODE_POINTER:
      node_print_pointers(output, declaration_options);
      break;
    default:
      assert(0);
      break;
  }
}

void node_print_arguments_list(FILE *output, struct node *arguments_list) {
  if (arguments_list->data.arguments_list.self->kind == NODE_ARGUMENTS_LIST) {
    node_print_arguments_list(output, arguments_list->data.arguments_list.self);
  } else {
    node_print_declaration(output, arguments_list->data.arguments_list.self);
  }

  if (arguments_list->data.arguments_list.next) {
    printf(",");
    node_print_declaration(output, arguments_list->data.arguments_list.next);
  }
}

void node_print_function_decl(FILE *output, struct node *function_node) {
  assert(NODE_FUNCTION_DECL == function_node->kind);

  // print function name
  switch (function_node->data.function.name->kind) {
    case NODE_ARRAY:
      node_print_array(output, function_node->data.function.name);
      break;
    case NODE_IDENTIFIER:
      node_print_identifier(output, function_node->data.function.name);
      break;
    case NODE_POINTER:
      node_print_pointers(output, function_node->data.function.name);
      break;
    default:
      assert(0);
      break;
  }

  // print function arguments
  printf("(");
  
  // Argument list can be NULL for function calls
  if (NULL != function_node->data.function.argument_list) {
    switch (function_node->data.function.argument_list->kind) {
      /* for function decls */
      case NODE_ARGUMENTS_LIST:
        node_print_arguments_list(output, function_node->data.function.argument_list);
        break;
      /* for function calls */
      case NODE_EXPRESSION_STATEMENT:
        node_print_expression_list_statement(output, function_node->data.function.argument_list);
        break;
      case NODE_TYPE: 
        node_print_type(output, function_node->data.function.argument_list);
        break;
      default:
        assert(0);
        break;
    }
  }
  printf(")");
}

void node_print_function_definition(FILE *output, struct node *function_node) {
  assert(NODE_FUNCTION_DEFINITION == function_node->kind);

  // print function return type
  if (function_node->data.function_definition_statement.type->kind == NODE_POINTER) {
    node_print_cast_pointers(output, function_node->data.function_definition_statement.type);
  } else {
    node_print_type(output, function_node->data.function_definition_statement.type);
  }

  // print function declaration
  node_print_function_decl(output, function_node->data.function_definition_statement.function_decl);

  // print function body
  // empty bodies are allowed
  if (function_node->data.function_definition_statement.function_body != NULL) {
    switch (function_node->data.function_definition_statement.function_body->kind) {
      case NODE_COMPOUND_STATEMENT:
        node_print_compound_statement(output, function_node->data.function_definition_statement.function_body);
        break;
      /* this is only for void */
      case NODE_TYPE: 
        printf("{");
        node_print_type(output, function_node->data.function_definition_statement.function_body);
        printf("; }");
        break;
      default:
        assert(0);
        break;
    }
  }
}

void node_print_declaration_list_statement(FILE *output, struct node *declaration) {
  if (declaration->data.declaration_list_statement.self->kind == NODE_DECLARATION_LIST_STATEMENT) {
    node_print_declaration_list_statement(output, declaration->data.declaration_list_statement.self);
  } else {
    node_print_declaration_options(output, declaration->data.declaration_list_statement.self);
  }

  if (declaration->data.declaration_list_statement.next) {
    printf(",");
    node_print_declaration_options(output, declaration->data.declaration_list_statement.next);
  }
}


void node_print_declaration(FILE *output, struct node *declaration) {
  assert(NULL != declaration);
  assert(NULL != declaration->data.declaration_statement.type);

  // print type
  switch (declaration->data.declaration_statement.type->kind) {
    case NODE_TYPE:
      node_print_type(output, declaration->data.declaration_statement.type);
      break;
    case NODE_POINTER: 
      node_print_cast_pointers(output, declaration->data.declaration_statement.type);
      break;
    default:
      assert(0);
      break;
  }

  // print declarations
  // names can be optional in function decls
  if (NULL != declaration->data.declaration_statement.declaration) {
    switch (declaration->data.declaration_statement.declaration->kind) {
      case NODE_DECLARATION_LIST_STATEMENT:
        node_print_declaration_list_statement(output, declaration->data.declaration_statement.declaration);
        break;
      case NODE_IDENTIFIER:
        node_print_identifier(output, declaration->data.declaration_statement.declaration);
        break;
      case NODE_ARRAY:
        node_print_array(output, declaration->data.declaration_statement.declaration);
        break;
      case NODE_POINTER:
        node_print_pointers(output, declaration->data.declaration_statement.declaration);
        break;
      default:
        assert(0);
        break;
    }
  }
}

void node_print_expression(FILE *output, struct node *expression) {
  assert(NULL != expression);

  switch (expression->kind) {
    /* This means the expression is nested */
    case NODE_BINARY_OPERATION:
      node_print_binary_operation(output, expression);
      break;
    case NODE_UNARY_EXPR:
      node_print_unary_expression(output, expression);
      break;
    /* For ternary operators */
    case NODE_CONDITIONAL_STATEMENT:
      node_print_conditional_statement(output, expression);
      break;
    /* For nested pointers */
    case NODE_POINTER:
      node_print_pointers(output, expression);
      break;
    /* This means we've reached the innermost part of the expression */
    case NODE_TYPE:
      node_print_type(output, expression);
      break;
    /* This means we've reached the innermost part of the expression */
    case NODE_IDENTIFIER:
      node_print_identifier(output, expression);
      break;
    /* This means we've reached the innermost part of the expression */
    case NODE_NUMBER:
      node_print_number(output, expression);
      break;
    /* This means we've reached the innermost part of the expression */
    case NODE_STRING:
      node_print_string(output, expression);
      break;
    /* This means we've reached the innermost part of the expression */
    case NODE_CAST_EXPR:
      node_print_cast_expression(output, expression);
      break;
    /* This means we've reached the innermost part of the expression - this is used as function call*/
    case NODE_FUNCTION_DECL:
      node_print_function_decl(output, expression);
      break;
    case NODE_EXPRESSION_STATEMENT:
      node_print_expression_list_statement(output, expression);
      break;
    case NODE_EXPRESSION_LIST_STATEMENT:
      node_print_expression_list_statement(output, expression);
      break;
    default:
      assert(0);
      break;
  }
}

void node_print_iterative_statement(FILE *output, struct node *iterative_statement) {
  assert(NULL != iterative_statement);

  // while
  if (iterative_statement->data.iterative_statement.type == 0) {
    printf("while ");

    // print expression
    printf("(");
    node_print_expression_list_statement(output, iterative_statement->data.iterative_statement.expression);
    printf(") ");

    // print statement
    node_print_statement(output, iterative_statement->data.iterative_statement.statement);
  }
  
  // do while
  if (iterative_statement->data.iterative_statement.type == 1) {
    printf("do ");

    // print statement
    node_print_compound_statement(output, iterative_statement->data.iterative_statement.statement);
    
    // print expression
    printf("while ");
    printf("(");
    node_print_expression_list_statement(output, iterative_statement->data.iterative_statement.expression);
    printf(");");
  }

  // for
  if (iterative_statement->data.iterative_statement.type == 2) {
    printf("for ");

    // print expression
    printf("(");
    node_print_for_expression(output, iterative_statement->data.iterative_statement.expression);
    printf(")");
    
    // print statement
    node_print_statement(output, iterative_statement->data.iterative_statement.statement);
  }

}

void node_print_for_expression(FILE *output, struct node *expression) {
  assert(NULL != expression);

  // print initializer
  if (NULL != expression->data.for_expr.initializer) {
    switch(expression->data.for_expr.initializer->kind) {
      case NODE_DECLARATION_LIST_STATEMENT:
        node_print_function_decl(output, expression->data.for_expr.initializer);
        break;
      case NODE_EXPRESSION_STATEMENT:
        node_print_expression_list_statement(output, expression->data.for_expr.initializer);
        break;
      default:
        assert(0);
        break;
    }
  }

  // print condition
  printf(";");
  if (NULL != expression->data.for_expr.condition) {
    node_print_expression_list_statement(output, expression->data.for_expr.condition);
  }

  // print bound
  printf(";");
  if (NULL != expression->data.for_expr.bound) {
    node_print_expression_list_statement(output, expression->data.for_expr.bound);
  }
}

void node_print_label_statement(FILE *output, struct node *label) {
  assert(NODE_LABEL_STATEMENT == label->kind);

  // print label
  node_print_identifier(output, label->data.label_statement.label);


  // print statement
  printf(":");
  node_print_statement(output, label->data.label_statement.statement);
}

void node_print_goto_statement(FILE *output, struct node *statement) {
  assert(NODE_GOTO_STATEMENT == statement->kind);

  printf("goto ");
  node_print_identifier(output, statement->data.compound_statement.statement);
  printf(";");
}

void node_print_compound_statement(FILE *output, struct node *statement) {
  assert(NODE_COMPOUND_STATEMENT == statement->kind);

  printf("{");
  // compound statements can be empty
  if (NULL != statement->data.compound_statement.statement) {
    node_print_statement_list(output, statement->data.compound_statement.statement);
  }
  printf("}");
}

void node_print_conditional_statement(FILE *output, struct node *statement) {
  assert(NULL != statement);

  if (statement->data.conditional_statement.is_ternary) {
    printf("(");
    // print condition
    node_print_conditional_expression_opts(output, statement->data.conditional_statement.condition);

    printf(" ? ");

    // print true block
    node_print_expression_list_statement(output, statement->data.conditional_statement.true_block);

    printf(" : ");

    // print false block
    // can point to another conditional statement
    node_print_conditional_expression_opts(output, statement->data.conditional_statement.false_block);
    printf(")");
  } else {
    // print condition
    printf("if");
    printf("(");
    node_print_expression_list_statement(output, statement->data.conditional_statement.condition);
    printf(")");

    // print true block
    node_print_statement_opts(output, statement->data.conditional_statement.true_block);

    // print false block
    if (NULL != statement->data.conditional_statement.false_block) {
      printf(" else ");
      node_print_statement_opts(output, statement->data.conditional_statement.false_block);
    }
  }

}

void node_print_expression_list_statement(FILE *output, struct node *expression_statement) {
  assert(
    expression_statement->kind == NODE_EXPRESSION_LIST_STATEMENT
    || expression_statement->kind == NODE_EXPRESSION_STATEMENT
  );
  if (expression_statement->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
    node_print_expression_list_statement(output, expression_statement->data.expression_list_statement.self);
  } else {
    node_print_expression(output, expression_statement->data.expression_list_statement.self);
  }

  if (expression_statement->data.expression_list_statement.next) {
    printf(",");
    node_print_expression(output, expression_statement->data.expression_list_statement.next);
  }

}

void node_print_statement(FILE *output, struct node *statement) {
  assert(NULL != statement);

    switch(statement->kind) {
    case NODE_EXPRESSION_STATEMENT:
      node_print_expression_list_statement(output, statement);
      printf(";");
      break;
    case NODE_COMPOUND_STATEMENT:
      node_print_compound_statement(output, statement);
      break;
    case NODE_CONDITIONAL_STATEMENT:
      node_print_conditional_statement(output, statement);
      break;
    case NODE_ITERATIVE_STATEMENT:
      node_print_iterative_statement(output, statement);
      break;
    case NODE_GOTO_STATEMENT:
      node_print_goto_statement(output, statement);
      break;
    case NODE_LABEL_STATEMENT:
      node_print_label_statement(output, statement);
      break;
    case NODE_RETURN_STATEMENT: 
      node_print_return_statement(output, statement);
      break;
    case NODE_BREAK_STATEMENT: 
      fprintf(output, "break;");
      break;
    case NODE_CONTINUE_STATEMENT: 
      fprintf(output, "continue;");
      break;
    default:
      assert(0);
      break;
  }
}


void node_print_statement_list(FILE *output, struct node *statement_list) {
  assert(NODE_STATEMENT_LIST == statement_list->kind);

  if (NULL != statement_list->data.statement_list.init) {
    node_print_statement_list(output, statement_list->data.statement_list.init);
  }

  switch(statement_list->data.statement_list.statement->kind) {
    case NODE_EXPRESSION_STATEMENT:
      node_print_expression_list_statement(output, statement_list->data.statement_list.statement);
      printf(";");
      break;
    case NODE_FUNCTION_DEFINITION:
      node_print_function_definition(output, statement_list->data.statement_list.statement);
      break;
    case NODE_DECLARATION_STATEMENT:
      node_print_declaration(output, statement_list->data.statement_list.statement);
      fputs(";", output);
      break;
    case NODE_COMPOUND_STATEMENT:
      node_print_compound_statement(output, statement_list->data.statement_list.statement);
      break;
    case NODE_CONDITIONAL_STATEMENT:
      node_print_conditional_statement(output, statement_list->data.statement_list.statement);
      break;
    case NODE_ITERATIVE_STATEMENT:
      node_print_iterative_statement(output, statement_list->data.statement_list.statement);
      break;
    case NODE_GOTO_STATEMENT:
      node_print_goto_statement(output, statement_list->data.statement_list.statement);
      break;
    case NODE_LABEL_STATEMENT:
      node_print_label_statement(output, statement_list->data.statement_list.statement);
      break;
    case NODE_RETURN_STATEMENT: 
      node_print_return_statement(output, statement_list->data.statement_list.statement);
      break;
    case NODE_BREAK_STATEMENT: 
      fprintf(output, "break;");
      break;
    case NODE_CONTINUE_STATEMENT: 
      fprintf(output, "continue;");
      break;
    case NODE_STATEMENT_LIST:
      node_print_statement_list(output, statement_list->data.statement_list.statement);
      break;
    default:
      printf("statement kind: %d\n", statement_list->data.statement_list.statement->kind);
      assert(0);
      break;
  }
}


void node_print_error(FILE *output, char* error_message) {
   fprintf(output, "%s", error_message);
}

void print_string_node(struct node *expression) {
  const int arraySize = expression->data.string.length;
  for (size_t size = 0; size < arraySize - 1; size++) {
    bool isEscapeChar = false;
    int val = expression->data.string.stringInASCII[size];
    for (int size1 = 0; size1 < ESCAPE_CHAR_LENGTH; size1++) {
      if (val == escapeCharactersASCII[size1]) {
        printf("\\%c", escapeCodes[size1]); 
        isEscapeChar = true;   
        break;   
      }
    }
    if (!isEscapeChar) {
      printf("%c", expression->data.string.stringInASCII[size]);
    }
  }
}

bool node_is_pointer_address(struct node *node) {
  assert(NULL != node);

  return (
    (NODE_UNARY_EXPR == node->kind && UNARY_ADDRESS == node->data.unary_expr.operation) || 
    (NODE_CAST_EXPR == node->kind && node_is_pointer_address(node->data.cast_expr.expression)));
}

/* this only returns true for expressions like *p NOT *(.....nested exp) */
bool node_is_dereference(struct node *node) {
  assert(NULL != node);

  return 
    (NODE_UNARY_EXPR == node->kind &&
    UNARY_POINTER == node->data.unary_expr.operation);
}

bool node_is_unary_pointer_operation(struct node *node) {
  assert(NULL != node);

  return (
    (NODE_UNARY_EXPR == node->kind && UNARY_POINTER == node->data.unary_expr.operation) ||
    (TYPE_POINTER == get_type_from_expression(node)->kind) ||
    (NODE_CAST_EXPR == node->kind && node_is_unary_pointer_operation(node->data.cast_expr.expression))
  );
}

bool node_is_parent_unary_pointer(struct node *node) {
  assert(NULL != node);
  bool result = false;

  // go up the tree until we find a unary pointer operation
  struct node *temp = node;
  while (NULL != temp) {
    if (NODE_UNARY_EXPR == temp->kind && UNARY_POINTER == temp->data.unary_expr.operation) {
      result = true;
      break;
    }
    temp = temp->parent;
  }

  /* cleanup */
  temp = NULL;
  free(temp);

  return result;
}

bool node_is_equality_operation(int operation) {
  return (
    operation == EQUALITY_OP ||
    operation == INEQUALITY_OP ||
    operation == LESS_THAN ||
    operation == LESS_THAN_EQ ||
    operation == GREATER_THAN ||
    operation == GREATER_THAN_EQ ||
    operation == LOGICAL_AND ||
    operation == LOGICAL_OR
  );
}

bool node_is_expression_just_identifier(struct node *node) {
  assert (NODE_IDENTIFIER == node->kind);

  bool result = true;

  // traverse up parent until u reach top
  struct node *temp = node->parent;
  while (NULL != temp) {
    if (
      NODE_EXPRESSION_LIST_STATEMENT != temp->kind &&
      NODE_EXPRESSION_STATEMENT != temp->kind &&
      NODE_STATEMENT_LIST != temp->kind &&
      NODE_COMPOUND_STATEMENT != temp->kind &&
      NODE_FUNCTION_DEFINITION != temp->kind
    ) {
      result = false;
      break;
    }
    temp = temp->parent;
  }

  /* cleanup */
  temp = NULL;
  free(temp);

  return result;
}
