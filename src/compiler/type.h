#ifndef _TYPE_H
#define _TYPE_H

#include <stdio.h>
#include <stdbool.h>

struct node;

enum type_kind {
  TYPE_BASIC,
  TYPE_POINTER,
  TYPE_ARRAY,
  TYPE_FUNCTION,
  TYPE_LABEL
};

enum type_basic_kind {
  TYPE_BASIC_CHAR,
  TYPE_BASIC_SHORT,
  TYPE_BASIC_INT,
  TYPE_BASIC_LONG,
  TYPE_BASIC_VOID,
  TYPE_BASIC_UNDEF
};
struct type {
  enum type_kind kind;
  union {
    struct {
      bool is_unsigned;
      enum type_basic_kind datatype;
    } basic;
  } data;
};

struct type *type_create(enum type_kind kind, enum type_basic_kind datatype, bool is_unsigned);
struct type *type_basic(bool is_unsigned, enum type_basic_kind datatype);

int type_get_size(struct type *type);
int type_get_basic_size(struct type *type);

int type_assign_in_statement_list(struct node *statement_list);
void type_assign_in_expression_list_statement(struct node *expression_statement);
int type_assign_in_possible_statement(struct node *statement);

void type_print(FILE *output, struct type *type);
char* get_type_string(struct type *basic);
bool type_is_equal(struct type *left, struct type *right);
void type_convert_usual_unary(struct node *expression);
struct type* perform_unary_conversions(struct node *expression, struct type *type, bool create_node);
struct type *get_type_from_expression(struct node *expression);
bool type_is_arithmetic(struct type *t);
bool type_is_pointer_or_array(struct type *t);

bool type_is_binop_pointer_operation_valid(int operation);

bool type_is_equal_basic(struct type *left, struct type *right);
bool types_are_compatible_pointer(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
);
bool types_are_compatible_array(struct type *left, struct type *right, struct node *lhs_node, struct node *rhs_node);
bool types_are_compatible_function(struct type *left, struct type *right, struct node *lhs_node, struct node *rhs_node);
bool type_is_array(struct type *t);
bool type_is_void(struct type *t);
bool type_is_function(struct type *t);
bool type_is_pointer(struct type *t);
bool types_are_compatible_binary(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node,
  int operation
);
bool types_are_compatible_assignment(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
);

struct type* perform_assignment_conversions(
  struct node *left_expression,
  struct node *right_expression,
  struct type *type_left,
  struct type *type_right,
  bool create_node
);

enum value get_value_from_expression(struct node *expression);
int type_size(struct type *t);

int type_check_main_func_validity();

#endif /* _TYPE_H */
