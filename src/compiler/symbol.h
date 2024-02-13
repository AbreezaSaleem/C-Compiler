#ifndef _SYMBOL_H
#define _SYMBOL_H

#include <stdio.h>
#include <stdbool.h>
#include "compiler.h"
struct node;
struct type;
struct type_tree;

enum symbol_table_kind {
  FILE_SCOPE = 0,
  PROCEDURE_SCOPE,
  BLOCK_SCOPE
};

enum inner_symbol_table_kind {
  STATEMENT_LABELS = 3,
  STRING_LITERALS
};

struct symbol {
  char name[IDENTIFIER_MAX + 1];
  unsigned int id;
  char symbol_table_tag[IDENTIFIER_MAX + 1];
  int is_used;
  int is_function_defined;        // this is only for functions
  int is_system_call;
  int arg_number;                 // will be -1 for non-arguments
  int is_only_for_saving_type;    // we want to save types of function declaration arguments too
  struct result result;
  struct type_tree *type_tree;
};

struct symbol_list {
  struct symbol symbol;
  struct symbol_list *next;
};

struct string_symbol {
  char string[STRING_MAX + 1];
  char id[40];
};

struct string_symbol_list {
  struct string_symbol string_symbol;
  struct string_symbol_list *next;
};

struct statement_label {
  char label[IDENTIFIER_MAX + 1];
  unsigned int id;
  int is_defined;
  int is_used;
  struct symbol_table* parent;
};

struct statement_labels_list {
  struct statement_label statement_label;
  struct statement_labels_list *next;
};

struct symbol_table_children {
  struct symbol_table *self;
  struct symbol_table_children *next;
};

struct symbol_table {
  enum symbol_table_kind kind;
  struct symbol_list *variables;
  struct symbol_table *parent;
  struct symbol_table_children *children;
  union {
    struct {
      enum inner_symbol_table_kind kind;
      struct string_symbol_list *string_literals;
    } string_literals;
    struct {
      enum inner_symbol_table_kind kind;
      struct statement_labels_list *statement_labels;
    } statement_labels;
    struct {
      char *name;
    } procedure;
  } data;
};

struct symbol_table *symbol_create_table(enum symbol_table_kind kind);
struct symbol *symbol_get(struct symbol_table *table, char name[]);
void insert_child(struct symbol_table *parent, struct symbol_table *child);
struct symbol* search_symbol_table(char* name, struct symbol_table* table);
char* search_string_literal_table(char* name);
char* search_statement_label_table(struct statement_label *symbol);
struct string_symbol *create_node_for_string_table(char* string);
int create_node_for_statement_label_table(struct symbol_table *table, struct node* statement, int is_goto_statement);

/**********************/
/* Add From Functions */
/**********************/
int symbol_add_from_statement_list(struct symbol_table *table, struct node *statement_list);
int symbol_add_from_possible_statement(struct symbol_table *table, struct node *statement);
int symbol_add_from_function_call(struct symbol_table *table, struct node *function);
int symbol_add_from_expression(struct symbol_table *table, struct node *expression);
int symbol_add_from_expression_list_statement(struct symbol_table *table, struct node *expression_statement);
int symbol_add_from_unary_operation(struct symbol_table *table, struct node *unary_operation);
int symbol_add_from_arguments_list(struct symbol_table *table, struct node *arguments_list, int only_for_type);

void symbol_print_table_handler(FILE *output, struct symbol_table *table);
void symbol_print_table(FILE *output);
int check_statement_labels_tables_validity();
int check_function_declarations_validity();
void symbol_print_table_assignment_format_handler(FILE *output, struct symbol_table *table);
void symbol_print_table_assignment_format(FILE *output);

struct string_symbol_list* symbol_get_string_table();
struct symbol_table* symbol_get_global_symbol_table();
struct symbol_table* symbol_get_procedure_table(char *procedure_name);

bool is_syscall(char *function_name);
#endif /* _SYMBOL_H */
