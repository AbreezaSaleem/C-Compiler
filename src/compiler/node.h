#ifndef _NODE_H
#define _NODE_H

#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

#include <stdio.h>
#include <stdbool.h>
#include "compiler.h"
#include "type.h"
#include "./helpers/util-functions.h"

enum node_kind {
  /* INDIVIDUAL WORDS */
  NODE_NUMBER = 1,                   
  NODE_RESERVED_WORD,                
  NODE_IDENTIFIER,                   
  NODE_STRING,            
  NODE_ARRAY,
  NODE_TYPE,
  NODE_POINTER,
  /* EXPRESSIONS */    
  NODE_BINARY_OPERATION,  
  NODE_CAST_EXPR,
  NODE_UNARY_EXPR,
  NODE_FOR_EXPR,
  /* STATEMENTS */
  NODE_STATEMENT_LIST,               
  NODE_COMPOUND_STATEMENT,   
  NODE_ITERATIVE_STATEMENT,   
  NODE_CONDITIONAL_STATEMENT,            
  NODE_EXPRESSION_STATEMENT,         
  NODE_EXPRESSION_LIST_STATEMENT,         
  NODE_DECLARATION_STATEMENT,           /* Contains type + multiple decls in one line */    
  NODE_DECLARATION_LIST_STATEMENT,      /* Contains list of decls */
  NODE_LABEL_STATEMENT,
  NODE_GOTO_STATEMENT,
  NODE_NULL_STATEMENT,
  NODE_BREAK_STATEMENT,
  NODE_CONTINUE_STATEMENT,
  NODE_RETURN_STATEMENT,
  /* FUNCTIONS */
  NODE_FUNCTION_DEFINITION,   
  NODE_ARGUMENTS_LIST,
  NODE_FUNCTION_DECL
};

// static const char *nodes_names[] = {
//   "",
//   "NODE_NUMBER",
//   "NODE_RESERVED_WORD",
//   "NODE_IDENTIFIER",
//   "NODE_STRING",
//   "NODE_ARRAY",
//   "NODE_TYPE",
//   "NODE_POINTER",
//   "NODE_BINARY_OPERATION",
//   "NODE_CAST_EXPR",
//   "NODE_UNARY_EXPR",
//   "NODE_FOR_EXPR",
//   "NODE_STATEMENT_LIST",
//   "NODE_COMPOUND_STATEMENT",
//   "NODE_ITERATIVE_STATEMENT",
//   "NODE_CONDITIONAL_STATEMENT",
//   "NODE_EXPRESSION_STATEMENT",
//   "NODE_EXPRESSION_LIST_STATEMENT",
//   "NODE_DECLARATION_STATEMENT",
//   "NODE_DECLARATION_LIST_STATEMENT",
//   "NODE_LABEL_STATEMENT",
//   "NODE_GOTO_STATEMENT",
//   "NODE_NULL_STATEMENT",
//   "NODE_BREAK_STATEMENT",
//   "NODE_CONTINUE_STATEMENT",
//   "NODE_RETURN_STATEMENT",
//   "NODE_FUNCTION_DEFINITION",
//   "NODE_ARGUMENTS_LIST",
//   "NODE_FUNCTION_DECL"
// };

struct compound_operator {
  struct node *operator;
  struct node *right_operator;
  struct node *result;
  struct type *operand_type;
  struct type *result_type;
};

struct node {
  enum node_kind kind;
  struct location location;
  struct ir_list *ir_list;
  struct node *parent;
  union {
    struct {
      int type;
      struct type *type_node;
    } type;
    struct {
      unsigned long value;
      bool overflow;
      bool leading_zeros;
      struct result result;
    } number;
    struct {
      char name[IDENTIFIER_MAX + 1];
      bool overflow;
      struct symbol *symbol;
      struct result result;
    } identifier;
    struct {
      int length;
      char value[STRING_MAX + 1];
      bool overflow;
      bool octal_overflow;
      bool nullTerminated;
      int* stringInASCII;
      struct string_symbol *symbol;
      struct result result;
    } string;
    struct {
      struct node *name;
      struct node *size;
    } array;
    struct {
      struct node *points_to;
    } pointer;
    struct {
      int operation;
      struct node *left_operand;
      struct node *right_operand;
      struct result result;
      struct compound_operator *compound_operator;
    } binary_operation;
    struct {
      struct node *type;
      struct node *expression;
      struct result result;
    } cast_expr;
    struct {
      int operation;
      struct node *expression;
      struct result result;
      bool is_subscript_operator;
      struct compound_operator *compound_operator;
    } unary_expr;
    struct {
      struct node *initializer;
      struct node *condition;
      struct node *bound;
    } for_expr;
    struct {
      struct node *name;
      struct node *argument_list;
      struct symbol *symbol;        // for function calls
      struct result result;
    } function;
    /* STATEMENTS */
    struct {
      char *label_name;
      struct node *type;
      struct node *function_decl;
      struct node *function_body;
    } function_definition_statement;
    struct {
      struct node *label;
      struct node *statement;
      struct statement_label *symbol;
    } label_statement;
    struct {
      struct node *label;
      struct statement_label *symbol;
    } goto_statement;
    struct {
      struct node *expression;
    } return_statement;
    struct {
      struct node *expression;
      struct type *type;
    } expression_statement;
    struct {
      struct node *statement;
    } compound_statement;
    struct {
      int type;
      struct node *expression;
      struct node *statement;
    } iterative_statement;
    struct {
      bool is_ternary;
      struct node *condition;
      struct node *true_block;
      struct node *false_block;
      struct result result;
    } conditional_statement;
    struct {
      struct node *self;
      struct node *next;
    } expression_list_statement;
    struct {
      struct node *type;
      struct node *declaration;
    } declaration_statement;
    struct {
      struct node *self;
      struct node *next;
    } declaration_list_statement;
    struct {
      struct node *self;
      struct node *next;
    } arguments_list;
    struct {
      struct node *init;
      struct node *statement;
    } statement_list;
  } data;
};

enum iterative_type {
  WHILE,
  DO_WHILE,
  FOR
};

enum node_unary_operation {
  UNARY_HYPHEN,
  UNARY_PLUS,
  UNARY_LOGICAL_NEGATION,
  UNARY_BITWISE_NEGATION,
  UNARY_ADDRESS,
  UNARY_POINTER,
  UNARY_PREINC,
  UNARY_PREDEC,
  UNARY_POSTINC,
  UNARY_POSTDEC
};

enum node_binary_operation {
  /* BINARY OPERATIONS */
  BINOP_MULTIPLICATION,
  BINOP_DIVISION,
  BINOP_ADDITION,
  BINOP_MOD,
  BINOP_SUBTRACTION,
  BINOP_ASSIGN,
  LOGICAL_OR,
  LOGICAL_AND,
  BITWISE_OR,
  BITWISE_XOR,
  BITWISE_AND,
  EQUALITY_OP,
  INEQUALITY_OP,
  LESS_THAN,
  LESS_THAN_EQ,
  GREATER_THAN,
  GREATER_THAN_EQ,
  LEFT_SHIFT,
  RIGHT_SHIFT,
  /* ASSIGNMENT OPERATIONS */
  BINOP_PLUS_EQUAL,
  BINOP_MINUS_EQUAL,
  BINOP_MULTIPLY_EQUAL,
  BINOP_DIVIDE_EQUAL,
  BINOP_MODULO_EQUAL,
  BINOP_LEFT_SHIFT_EQUAL,
  BINOP_RIGHT_SHIFT_EQUAL,
  BINOP_BITWISE_AND_EQUAL,
  BINOP_BITWISE_XOR_EQUAL,
  BINOP_BITWISE_OR_EQUAL
};

/* Constructors */
int compare_nodes(struct node *node_1, struct node *node_2);

struct node *node_type(YYLTYPE location, enum type_basic_kind type, bool is_unsigned);
struct node *node_number(YYLTYPE location, char *text, int length);
struct node *node_character(YYLTYPE location, char *text, int length);
struct node *node_identifier(YYLTYPE location, char *text, int length);
struct node *node_string(YYLTYPE location, char *text, int length);
struct node *node_array(YYLTYPE location, struct node *name, struct node *size);
struct node *node_subscript_expr(YYLTYPE location, struct node *arr, struct node *index);
struct node *node_pointer(YYLTYPE location, struct node *points_to);
struct node *node_function(YYLTYPE location, struct node *name, struct node *argument_list, bool is_declaration);
struct node *node_cast_expression(YYLTYPE location, struct node *type, struct node *expression);
struct node *node_for_expression(YYLTYPE location, struct node *initializer,
                                  struct node *condition, struct node *bound);
struct node *node_unary_expression(YYLTYPE location, enum node_unary_operation operation,
                                  struct node *expression);
struct node *node_function_definition(YYLTYPE location, struct node *type, 
                                  struct node *function, struct node *function_body, bool is_type_pointer);
struct node *node_binary_operation(YYLTYPE location, enum node_binary_operation operation,
                                  struct node *left_operand, struct node *right_operand);


/* Statements */
struct node *node_expression_statement(YYLTYPE location, struct node *expression);
struct node *node_expression_list_statement(YYLTYPE location, struct node *self, struct node *next);
struct node *node_declaration_statement(YYLTYPE location, struct node *type, struct node *declaration);
struct node *node_pointer_argument(YYLTYPE location, struct node *type);
struct node *node_declaration_list_statement(YYLTYPE location, struct node *self, struct node *next);
struct node *node_arguments_list(YYLTYPE location, struct node *self, struct node *next);
struct node *node_statement_list(YYLTYPE location, struct node *init, struct node *statement);
struct node *node_compound_statement(YYLTYPE location, struct node *statement);
struct node *node_iterative_statement(YYLTYPE location, enum iterative_type type, struct node *expression, 
                                        struct node *statement);
struct node *node_conditional_statement(YYLTYPE location, bool is_ternary, struct node *condition,
                                        struct node *true_block, struct node *false_block);
struct node *node_null_statement(struct node *error_stmt, YYLTYPE location);
struct node *node_label_statement(YYLTYPE location, struct node *label, struct node *statement);
struct node *node_goto_statement(YYLTYPE location, struct node *label);
struct node *node_break_statement(YYLTYPE location);
struct node *node_continue_statement(YYLTYPE location);
struct node *node_return_statement(YYLTYPE location, struct node *expression);

void print_string_node(struct node *expression);

struct result *node_get_result(struct node *expression);
struct result *node_assign_result(enum value value, struct type *type, struct node *expression);


/* PRINT FUNCTIONS */
void node_print_binary_operation(FILE *output, struct node *binary_operation);
void node_print_unary_expression(FILE *output, struct node *expression);
void node_print_number(FILE *output, struct node *number);
void node_print_string(FILE *output, struct node *string);
void node_print_type(FILE *output, struct node *type);
void node_print_pointers(FILE *output, struct node *points_to);
void node_print_cast_pointers(FILE *output, struct node *points_to);
void node_print_identifier(FILE *output, struct node *identifier);
void node_print_array(FILE *output, struct node *array);
void node_print_declaration_options(FILE *output, struct node *declaration_options);
void node_print_function_decl(FILE *output, struct node *function_node);
void node_print_function_definition(FILE *output, struct node *function_node);
void node_print_cast_expression(FILE *output, struct node *expression);
void node_print_for_expression(FILE *output, struct node *expression);
void node_print_return_statement(FILE *output, struct node *expression);


/* STATEMENT PRINT FUNCTIONS */
void node_print_arguments_list(FILE *output, struct node *arguments_list);
void node_print_declaration_list_statement(FILE *output, struct node *declaration);
void node_print_declaration_list_statement(FILE *output, struct node *declaration);
void node_print_declaration(FILE *output, struct node *declaration);
void node_print_expression(FILE *output, struct node *expression);
void node_print_expression_statement(FILE *output, struct node *expression_statement);
void node_print_expression_list_statement(FILE *output, struct node *expression_statement);
void node_print_statement(FILE *output, struct node *statement);
void node_print_statement_list(FILE *output, struct node *statement_list);
void node_print_compound_statement(FILE *output, struct node *statement);
void node_print_iterative_statement(FILE *output, struct node *iterative_statement);
void node_print_conditional_statement(FILE *output, struct node *statement);
void node_print_label_statement(FILE *output, struct node *label);
void node_print_goto_statement(FILE *output, struct node *statement);


/* print helper functions */
void node_print_statement_opts(FILE *output, struct node *statement);
void node_print_conditional_expression_opts(FILE *output, struct node *expression);

void node_print_error(FILE *output, char* error_message);

bool node_is_pointer_address(struct node *node);
bool node_is_dereference(struct node *node);
bool node_is_unary_pointer_operation(struct node *node);
bool node_is_parent_unary_pointer(struct node *node);

bool node_is_equality_operation(int operation);
bool node_is_expression_just_identifier(struct node *node);
#endif
