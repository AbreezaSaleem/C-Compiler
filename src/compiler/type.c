#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "symbol.h"
#include "helpers/type-tree.h"

extern int type_range_error_count;
extern struct symbol_table *file_scope_table;
int type_conversion_error_count = 0;
int call_argument_number = 1;
int main_return_valid = 0;
/***************************
 * CREATE TYPE EXPRESSIONS *
 ***************************/

struct type *type_create(enum type_kind kind, enum type_basic_kind datatype, bool is_unsigned) {
  struct type *basic;

  basic = malloc(sizeof(struct type));
  assert(NULL != basic);

  basic->kind = kind;
  basic->data.basic.is_unsigned = is_unsigned;
  basic->data.basic.datatype = datatype;
  return basic;
}

struct type *type_basic(bool is_unsigned, enum type_basic_kind datatype) {
  struct type *basic;

  basic = malloc(sizeof(struct type));
  assert(NULL != basic);

  basic->kind = TYPE_BASIC;
  basic->data.basic.is_unsigned = is_unsigned;
  basic->data.basic.datatype = datatype;
  return basic;
}

int type_get_basic_size(struct type *type) {
  assert(NULL != type);

  switch(type->data.basic.datatype) {
    case TYPE_BASIC_CHAR: 
      return 1;
    case TYPE_BASIC_SHORT: 
      return 2;
    case TYPE_BASIC_INT: 
    case TYPE_BASIC_LONG: 
      return 4;
    default: 
      return 0;
  }
}

int type_get_size(struct type *type) {
  assert(NULL != type);

  switch(type->kind) {
    case TYPE_POINTER: 
      return 4;
    default: {
      switch(type->data.basic.datatype) {
        case TYPE_BASIC_CHAR: 
          return 1;
        case TYPE_BASIC_SHORT: 
          return 2;
        case TYPE_BASIC_INT: 
        case TYPE_BASIC_LONG: 
          return 4;
        default: 
          return 0;
      }
    }
  }
}

static int get_conversion_rank(struct type *type) {
  switch(type->data.basic.datatype) {
    case TYPE_BASIC_LONG:
      return 50;
    case TYPE_BASIC_INT:
      return 40;
    case TYPE_BASIC_SHORT:
      return 30;
    case TYPE_BASIC_CHAR:
      return 20;
    default:
      return 0;
  }
}

bool is_expr_modifiable(struct node *expression) {
  assert(NULL != expression);
  // handle subscripts + dereferences here
  // but dont allow assignment to arrays
  if (NODE_UNARY_EXPR == expression->kind) {
    if (NODE_IDENTIFIER == expression->data.unary_expr.expression->kind) {
      if (TYPE_ARRAY == get_type_from_expression(expression->data.unary_expr.expression)->kind) {
        return false;
      }
    }
    return UNARY_POINTER == expression->data.unary_expr.operation;
  }
  return true;
}

void traverse_arguments(
  struct node *arguments_list, struct node *function_decl_node, struct type *function_declaration_arg,
  int decl_argument_number
) {
    assert(
    arguments_list->kind == NODE_EXPRESSION_LIST_STATEMENT
    || arguments_list->kind == NODE_EXPRESSION_STATEMENT
  );
  if (arguments_list->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
    traverse_arguments(
      arguments_list->data.expression_list_statement.self,
      function_decl_node,
      function_declaration_arg,
      decl_argument_number
    );
  } else {
    if (decl_argument_number == call_argument_number) {
      struct type *type_converted_to = perform_assignment_conversions(
        function_decl_node,
        arguments_list->data.expression_list_statement.self,
        function_declaration_arg,
        get_type_from_expression(arguments_list->data.expression_list_statement.self),
        true
      );
      node_assign_result(
        get_value_from_expression(arguments_list->data.expression_list_statement.self),
        type_converted_to,
        arguments_list->data.expression_list_statement.self
      );
    }
    call_argument_number++;
  }

  if (arguments_list->data.expression_list_statement.next) {
    if (decl_argument_number == call_argument_number) {
      struct type *type_converted_to = perform_assignment_conversions(
        function_decl_node,
        arguments_list->data.expression_list_statement.next,
        function_declaration_arg,
        get_type_from_expression(arguments_list->data.expression_list_statement.next),
        true
      );
      node_assign_result(
        get_value_from_expression(arguments_list->data.expression_list_statement.next),
        type_converted_to,
        arguments_list->data.expression_list_statement.next
      );
    }
    call_argument_number++;
  }
}

int compare_func_arguments(struct type_tree *type_tree, struct node *arguments_list, struct node *function) {
  int identical = 0;
  int decl_argument_number = 1;

  struct type_chain *type_chain = type_tree->type_chain;
  struct node *argument = arguments_list;
  struct node *next_argument = NULL;
  struct type *type = NULL;

  while(NULL != type_chain) {
    if (NULL != type_chain->next) {
      bool is_void = NODE_TYPE == type_chain->type->kind && TYPE_BASIC_VOID == type_chain->type->data.type.type_node->data.basic.datatype;
      bool is_identifier = NODE_IDENTIFIER == type_chain->type->kind;
      if (is_void || is_identifier) {
        call_argument_number = 1;
        type = get_type_from_expression(type_chain->type);

        if(is_void && NULL == arguments_list) {
          // do nothing - this is valid
        } else if (!is_void && NULL == arguments_list) {
          decl_argument_number++;
        } else {
          traverse_arguments(
            arguments_list, type_chain->type, type, decl_argument_number
          );
          decl_argument_number++;
        }
      }
    }
    type_chain = type_chain->next;
  }

  if (decl_argument_number != call_argument_number) {
    compiler_print_error(function->location, "Function arguments too many/too few");
    type_conversion_error_count++;
  }

  /* cleanup */
  type = NULL;
  free(type);
  type_chain = NULL;
  free(type_chain);
  argument = NULL;
  free(argument);
  next_argument = NULL;
  free(next_argument);

  return identical;
}

void assign_child_to_parent(struct node *parent, struct node *child, struct node *current_node) {
  switch(parent->kind) {
    case NODE_UNARY_EXPR: 
      parent->data.unary_expr.expression = child;
      break;
    case NODE_BINARY_OPERATION: {
      if (compare_nodes(parent->data.binary_operation.left_operand, current_node) == 0) {
        parent->data.binary_operation.left_operand = child;
      } else {
        parent->data.binary_operation.right_operand = child;
      }
      break;
    }
    case NODE_CAST_EXPR: 
      parent->data.cast_expr.expression = child;
      break;
    case NODE_EXPRESSION_LIST_STATEMENT: {
      if (compare_nodes(parent->data.expression_list_statement.self, current_node) == 0) {
        parent->data.expression_list_statement.self = child;
      } else {
        parent->data.expression_list_statement.next = child;
      }
      break;
    }
    case NODE_RETURN_STATEMENT: 
      parent->data.return_statement.expression = child;
      break;
    default:
      assert(0);
      break;
  }
}

void create_cast_node(
  struct node *expression, enum type_basic_kind type,
  enum type_kind kind, bool is_unsigned, bool append_pointer
) {
  struct node *type_node = node_type(expression->location, type, is_unsigned);
  type_node->data.type.type_node->kind = kind;
  struct node *parent = expression->parent;
  struct node *node = NULL;

  /* if the expression is pointer then move it to node_type */
  if (
    NODE_UNARY_EXPR == expression->kind && UNARY_POINTER == expression->data.unary_expr.operation &&
    !node_is_dereference(expression)
  ) {
    struct node *pointer = node_pointer(expression->location, type_node);
    node = node_cast_expression(expression->location, pointer, expression->data.unary_expr.expression);
  } else if (append_pointer) {
    /* this check is only getting used for adding a char* to string literalls */
    struct node *pointer = node_pointer(expression->location, type_node);
    node = node_cast_expression(expression->location, pointer, expression);
  } else {
    node = node_cast_expression(expression->location, type_node, expression);
  }

  assert(NULL != expression->parent);
  node->parent = parent;

  assign_child_to_parent(parent, node, expression);
}

void create_null_pointer(
  struct node *expression, enum type_basic_kind type,
  enum type_kind kind, bool is_unsigned
) {
  struct node *parent = expression->parent;
  struct node *type_node = node_type(expression->location, type, is_unsigned);
  type_node->data.type.type_node->kind = kind;
  struct node *pointer = node_pointer(expression->location, type_node);
  struct node *node = node_cast_expression(expression->location, pointer, expression);

  assert(NULL != expression->parent);
  node->parent = parent;

  assign_child_to_parent(parent, node, expression);
}

void type_convert_string(struct node *expression) {
  assert(NODE_STRING == expression->kind);

  /* do not convert if string appears as an argument of address operator */
  if (
    NODE_UNARY_EXPR == expression->parent->kind &&
    UNARY_ADDRESS == expression->parent->data.unary_expr.operation
  ) return;

  create_cast_node(expression, TYPE_BASIC_CHAR, TYPE_POINTER, false, true);
  struct type *type_converted_to = type_basic(false, TYPE_BASIC_CHAR);
  type_converted_to->kind = TYPE_POINTER;

  expression->data.string.result.type = type_converted_to;
  /* after explicity adding a casting conversion to pointer to type 'x'
   * treat it as rvalue (discussed in class + mentioned)
   * in IR slides
   */
  expression->data.string.result.value = R_VALUE;
}

void type_convert_usual_unary_array(struct node *expression) {
  /* we're handling array unary conversions here */
  enum type_basic_kind kind = get_type_from_expression(expression)->data.basic.datatype;
  bool is_unsigned = get_type_from_expression(expression)->data.basic.is_unsigned;
  create_cast_node(expression, kind, TYPE_POINTER, is_unsigned, true);
  struct type *type_converted_to = type_basic(is_unsigned, kind);
  type_converted_to->kind = TYPE_POINTER;

  expression->data.identifier.result.type = type_converted_to;
  /* after explicity adding a casting conversion to pointer to type 'x'
   * treat it as rvalue (discussed in class + mentioned)
   * in IR slides
   */
  expression->data.identifier.result.value = R_VALUE;
}

struct type* perform_unary_conversions(struct node *expression, struct type *type, bool create_node) {
  int conversion_rank = 0;
  struct type *type_converted_to = NULL;

  if (NULL == type) {
    return NULL;
  }

  conversion_rank = get_conversion_rank(type);

  if (conversion_rank >= 40) {
    type_converted_to = type;
  } else {
    if (type->data.basic.is_unsigned) {
      if (can_represent_values(
        TYPE_BASIC_INT, false,
        type->data.basic.datatype, type->data.basic.is_unsigned
      ) == 1) {
        /* check for erros during type range checking */
        if (type_range_error_count > 0) {
          compiler_print_error(expression->location, "Incompatible type conversion");
          type_conversion_error_count++;
        }

        /* int type CAN represent this type */
        if(create_node) create_cast_node(expression->data.unary_expr.expression, TYPE_BASIC_INT, type->kind, false, false);
        type_converted_to = type_basic(false, TYPE_BASIC_INT);
      } else {
        if(create_node) create_cast_node(expression->data.unary_expr.expression, TYPE_BASIC_INT, type->kind, true, false);
        type_converted_to = type_basic(true, TYPE_BASIC_INT);
      }
    } else {
      // convert to int
      if(create_node) create_cast_node(expression->data.unary_expr.expression, TYPE_BASIC_INT, type->kind, false, false);
      type_converted_to = type_basic(false, TYPE_BASIC_INT);
    }
  }

  // replace 'type_basic' with a general type creation function and pass kind there
  // and remove this
  type_converted_to->kind = type->kind;

  return type_converted_to;
}

struct type* perform_binary_conversions(
  int operation,
  struct node *left_expression,
  struct node *right_expression,
  struct type *type_left,
  struct type *type_right,
  bool create_node
) {
  if (NULL == type_left || NULL == type_right) {
    return NULL;
  }

  int conversion_found = 0;

  int conversion_rank_left = get_conversion_rank(type_left);
  int conversion_rank_right = get_conversion_rank(type_right);

  if (!types_are_compatible_binary(
    type_left, type_right,
    left_expression, right_expression,
    operation
  )) {
    compiler_print_error(left_expression->location, "Incompatible type conversion");
    type_conversion_error_count++;
    return NULL;
  }

  struct type *type_converted_to = NULL;

  // handle scenario if you're equating a pointer to 0
  if (node_is_equality_operation(operation)) {
    if (TYPE_POINTER == type_right->kind && TYPE_BASIC == type_left->kind) {
      if (NODE_NUMBER == left_expression->kind) {
        if (left_expression->data.number.value == 0) {
          // cast 0 to a pointer of type right
          create_null_pointer(left_expression, type_right->data.basic.datatype, type_right->kind, type_right->data.basic.is_unsigned);
          type_converted_to = type_basic(type_right->data.basic.is_unsigned, type_right->data.basic.datatype);
          return type_converted_to;
        } else {
          compiler_print_error(left_expression->location, "Incompatible type conversion");
          type_conversion_error_count++;
          return NULL;
        }
      } 
    }
    if (TYPE_POINTER == type_left->kind && TYPE_BASIC == type_right->kind) {
      if (NODE_NUMBER == right_expression->kind) {
        if (right_expression->data.number.value == 0) {
          // cast 0 to a pointer of type left
          create_null_pointer(right_expression, type_left->data.basic.datatype, type_left->kind, type_left->data.basic.is_unsigned);
          type_converted_to = type_basic(type_left->data.basic.is_unsigned, type_left->data.basic.datatype);
          return type_converted_to;
        } else {
          compiler_print_error(left_expression->location, "Incompatible type conversion");
          type_conversion_error_count++;
          return NULL;
        }
      } 
    }
  }
  
  // ignore if they have the same type
  if (type_is_equal(type_left, type_right)) {
    if (
      NODE_UNARY_EXPR == left_expression->kind &&
      NODE_UNARY_EXPR == right_expression->kind &&
      left_expression->data.unary_expr.operation == UNARY_POINTER &&
      right_expression->data.unary_expr.operation == UNARY_POINTER
    ) {
      type_converted_to = type_basic(type_left->data.basic.is_unsigned, type_left->data.basic.datatype);
      // changing this to TYPE_BASIC because we're working with dereferenced pointers
      type_converted_to->kind = TYPE_BASIC;
      return type_converted_to;
    }
    return type_left;
  }

  if (type_left->data.basic.is_unsigned && type_right->data.basic.is_unsigned) {
    if (conversion_rank_left > conversion_rank_right) {
      // cast right operand to left type
      if(create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, true, false);
      type_converted_to = type_basic(true, type_left->data.basic.datatype);
      conversion_found = 1;
    } else {
      // cast left operand to right type
      if(create_node) create_cast_node(left_expression, type_right->data.basic.datatype, type_right->kind, true, false);
      type_converted_to = type_basic(true, type_right->data.basic.datatype);
      conversion_found = 1;
    }
  } else if (!type_left->data.basic.is_unsigned && !type_right->data.basic.is_unsigned) {
    if (conversion_rank_left > conversion_rank_right) {
      // cast right operand to left type
      if(create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, false, false);
      type_converted_to = type_basic(false, type_left->data.basic.datatype);
      conversion_found = 1;
    } else {
      // cast left operand to right type
      if(create_node) create_cast_node(left_expression, type_right->data.basic.datatype, type_right->kind, false, false);
      type_converted_to = type_basic(false, type_right->data.basic.datatype);
      conversion_found = 1;
    }
  } else if (type_left->data.basic.is_unsigned && !type_right->data.basic.is_unsigned) {
    if (conversion_rank_right > conversion_rank_left) {
      if (
        can_represent_values(
          type_right->data.basic.datatype, type_right->data.basic.is_unsigned,
          type_left->data.basic.datatype, type_left->data.basic.is_unsigned
        ) == 1) {
          /* rhs signed type CAN represent lhs unsigned type */
          // cast left operand to right type
          if(create_node) create_cast_node(left_expression, type_right->data.basic.datatype, type_right->kind, false, false);
          type_converted_to = type_basic(false, type_right->data.basic.datatype);
          conversion_found = 1;
      } else {
        // cast left operand to right type
        if(create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, true, false);
        type_converted_to = type_basic(true, type_right->data.basic.datatype);
        conversion_found = 1;
      }
    } else {
      if(create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, true, false);
      type_converted_to = type_basic(true, type_left->data.basic.datatype);
      conversion_found = 1;
    }
  } else if (!type_left->data.basic.is_unsigned && type_right->data.basic.is_unsigned) {
    // this is the same condition as above, just the sides are opposite now
    if (conversion_rank_left > conversion_rank_right) {
      if (
        can_represent_values(
          type_left->data.basic.datatype, type_left->data.basic.is_unsigned,
          type_right->data.basic.datatype, type_right->data.basic.is_unsigned
        ) == 1) {
          /* rhs signed type CAN represent lhs unsigned type */
          // cast left operand to right type
          if(create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, false, false);
          type_converted_to = type_basic(false, type_left->data.basic.datatype);
          conversion_found = 1;
      } else {
        // cast left operand to right type
        if(create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, true, false);
        type_converted_to = type_basic(true, type_left->data.basic.datatype);
        conversion_found = 1;
      }
    } else {
      if(create_node) create_cast_node(left_expression, type_right->data.basic.datatype, type_right->kind, true, false);
      type_converted_to = type_basic(true, type_right->data.basic.datatype);
      conversion_found = 1;
    }
  }

  if (type_range_error_count > 0) {
    compiler_print_error(left_expression->location, "Incompatible type conversion");
    type_conversion_error_count++;
  }

  if (conversion_found == 0) {
    compiler_print_error(left_expression->location, "Binary conversion unsuccessful");
    type_conversion_error_count++;
  }

  return type_converted_to;
}

struct type* perform_assignment_conversions(
  struct node *left_expression,
  struct node *right_expression,
  struct type *type_left,
  struct type *type_right,
  bool create_node
) {
  // just convert rhs type to lhs type
  if (NULL == type_left || NULL == type_right) {
    return NULL;
  }

  if (!types_are_compatible_assignment(
    type_left, type_right,
    left_expression, right_expression
  )) {
    compiler_print_error(right_expression->location, "Incompatible type conversion");
    type_conversion_error_count++;
    return NULL;
  }

  // you cant assign pointer type to arithmetic etc
  // should be handled in 'types_are_compatible_pointer_assignment' function
  if (
    (type_is_arithmetic(type_left) && type_is_pointer_or_array(type_right)) ||
    (type_is_arithmetic(type_right) && type_is_pointer_or_array(type_left))
  ) {
    /*
     * but we do allow dereferencing of pointers + assigning '0' because its considered a NULL constant
     * note: this logic should be moved to the compatibility check functions
    */
    if (
      node_is_dereference(left_expression) || node_is_dereference(right_expression) ||
      node_is_pointer_address(left_expression) || node_is_pointer_address(right_expression) ||
      (type_is_pointer(type_left) && NODE_NUMBER == right_expression->kind && right_expression->data.number.value == 0)
      ) {
      // do nothing
    } else {
      compiler_print_error(right_expression->location, "Incompatible type conversion between arithmetic and pointer/array");
      type_conversion_error_count++;
      return NULL;
    }
  }

  // ignore if they have the same type
  if (type_is_equal(type_left, type_right)) {
    return type_left;
  }

  struct type *type_converted_to = NULL;

  if (create_node) create_cast_node(right_expression, type_left->data.basic.datatype, type_left->kind, false, false);
  type_converted_to = type_left;

  return type_converted_to;
}

/****************************************
 * TYPE EXPRESSION INFO AND COMPARISONS *
 ****************************************/

bool type_is_equal(struct type *left, struct type *right) {
  if (left->kind != right->kind) {
    // we only consider pointer vs array types to be different
    if ((left->kind == TYPE_POINTER && right->kind == TYPE_ARRAY)
      || (right->kind == TYPE_POINTER && left->kind == TYPE_ARRAY)) {
        return false;
    } else {
      return left->data.basic.is_unsigned == right->data.basic.is_unsigned
          && left->data.basic.datatype == right->data.basic.datatype;
    }
  } else {
    return left->data.basic.is_unsigned == right->data.basic.is_unsigned
          && left->data.basic.datatype == right->data.basic.datatype;
  }
}

bool type_is_equal_basic(struct type *left, struct type *right) {
  return left->data.basic.is_unsigned == right->data.basic.is_unsigned
          && left->data.basic.datatype == right->data.basic.datatype;
}

bool type_is_arithmetic_equal(struct type *left, struct type *right) {
  return left->data.basic.is_unsigned == right->data.basic.is_unsigned
          && left->data.basic.datatype == right->data.basic.datatype;
}

bool type_is_arithmetic(struct type *t) {
  return TYPE_BASIC == t->kind;
}

bool type_is_void(struct type *t) {
  return TYPE_BASIC_VOID == t->data.basic.datatype;
}

bool type_is_pointer(struct type *t) {
  return TYPE_POINTER == t->kind;
}

bool type_is_array(struct type *t) {
  return TYPE_ARRAY == t->kind;
}

bool type_is_function(struct type *t) {
  return TYPE_FUNCTION == t->kind;
}

bool type_is_pointer_or_array(struct type *t) {
  return TYPE_POINTER == t->kind || TYPE_ARRAY == t->kind;
}

int get_type_basic(struct type *t) {
  return t->data.basic.datatype;
}

bool type_is_unsigned(struct type *t) {
  return type_is_arithmetic(t) && t->data.basic.is_unsigned;
}

bool type_is_binop_pointer_operation_valid(int operation) {
  return (
    operation == BINOP_ADDITION || operation == BINOP_SUBTRACTION || operation == BINOP_ASSIGN ||
    operation == EQUALITY_OP || operation == INEQUALITY_OP || operation == LOGICAL_AND || operation == LOGICAL_OR ||
    operation == LESS_THAN || operation == LESS_THAN_EQ || operation == GREATER_THAN || operation == GREATER_THAN_EQ
  );
}

bool types_are_compatible_pointer_binary(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node,
  int operation
) {
  if (type_is_pointer(left) && type_is_pointer(right)) {
    return type_is_equal_basic(left, right);
  }
  if (type_is_pointer(left)) {
    if (type_is_arithmetic(right)) {
      if (node_is_dereference(lhs_node)) {
        return type_is_equal_basic(
          get_type_from_expression(lhs_node),
          right
        );
      } else {
        /* you only allow binop addition with pointer for a subset of operators */
        return type_is_binop_pointer_operation_valid(operation);
      }
    }
  }
  if (type_is_pointer(right)) {
    if (type_is_arithmetic(left)) {
      if (node_is_dereference(rhs_node)) {
        return type_is_equal_basic(
          get_type_from_expression(rhs_node),
          left
        );
      } else {
        /* you only allow binop addition with pointer for a subset of operators */
        return type_is_binop_pointer_operation_valid(operation);
      }
    }
  }
  return true;
}

bool types_are_compatible_pointer_assignment(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
) {
  if (type_is_pointer(left) && type_is_pointer(right)) {
    return type_is_equal_basic(left, right);
  }
  if (type_is_pointer(left)) {
    if (type_is_arithmetic(right)) {
      if (node_is_dereference(lhs_node)) {
        struct type type = *get_type_from_expression(lhs_node);
        return get_conversion_rank(&type) >= get_conversion_rank(right);
      }
    }
  }
  if (type_is_pointer(right)) {
    if (type_is_arithmetic(left)) {
      if (node_is_dereference(rhs_node)) {
        struct type type = *get_type_from_expression(rhs_node);
        return get_conversion_rank(&type) <= get_conversion_rank(left);
      }
    }
  }

  return true;
}

bool types_are_compatible_array(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
) {
  if(type_is_array(left) && type_is_array(right)) {
    if (get_type_basic(left) != get_type_basic(right)) return false;

    // get size of both arrays
    assert(NODE_IDENTIFIER == lhs_node->kind);
    assert(NODE_IDENTIFIER == rhs_node->kind);

    signed long size_left = get_array_size(lhs_node->data.identifier.symbol->type_tree);
    signed long size_right = get_array_size(rhs_node->data.identifier.symbol->type_tree);

    if (size_left == _SIGNED_LONG_MIN || size_right == _SIGNED_LONG_MIN) {
      return true;
    }
    if (
      get_array_size(lhs_node->data.identifier.symbol->type_tree) !=
      get_array_size(rhs_node->data.identifier.symbol->type_tree)
    ) return false;
  }
  return true;
}

bool types_are_compatible_array_assignment(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
) {
  if (type_is_array(left) && type_is_array(right)) {
    if (get_type_basic(left) != get_type_basic(right)) return false;

    // get size of both arrays
    assert(NODE_IDENTIFIER == lhs_node->kind);
    assert(NODE_IDENTIFIER == rhs_node->kind);

    signed long size_left = get_array_size(lhs_node->data.identifier.symbol->type_tree);
    signed long size_right = get_array_size(rhs_node->data.identifier.symbol->type_tree);

    if (size_left == _SIGNED_LONG_MIN || size_right == _SIGNED_LONG_MIN) {
      return true;
    }
    if (
      get_array_size(lhs_node->data.identifier.symbol->type_tree) !=
      get_array_size(rhs_node->data.identifier.symbol->type_tree)
    ) return false;
  }

  if (
    (type_is_array(left) && type_is_pointer(right)) ||
    (type_is_pointer(left) && type_is_array(right))) {
    return type_is_equal_basic(left, right);
  }

  return true;
}

bool types_are_compatible_function(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
) {
  if (type_is_function(left) && type_is_function(right)) {
    // compare their return values
    assert(NODE_FUNCTION_DECL == lhs_node->kind);
    assert(NODE_FUNCTION_DECL == rhs_node->kind);
    if (!type_is_equal(
      lhs_node->data.function.symbol->type_tree->type,
      rhs_node->data.function.symbol->type_tree->type
    )) {
      return false;
    } else {
      return true;
    }
  }
  if (type_is_function(left)) {
    if (type_is_arithmetic(right)) {
      if (type_is_void(left) && !type_is_void(right)) return false;
      if (!type_is_void(left) && type_is_void(right)) return false;
      struct type type = *get_type_from_expression(lhs_node);
      return get_conversion_rank(&type) <= get_conversion_rank(right);
    }
  }
  if (type_is_function(right)) {
    if (type_is_arithmetic(left)) {
      if (type_is_void(left) && !type_is_void(right)) return false;
      if (!type_is_void(left) && type_is_void(right)) return false;
      struct type type = *get_type_from_expression(rhs_node);
      return get_conversion_rank(&type) <= get_conversion_rank(left);
    }
  }
  return true;
}

bool types_are_compatible_arithmetic(struct type *left, struct type *right) {
  if(type_is_arithmetic(left) && type_is_arithmetic(right)) {
    return type_is_arithmetic_equal(left, right);
  }
  return true;
}

bool types_are_compatible_arithmetic_assignment(struct type *left, struct type *right) {
  if(type_is_arithmetic(left) && type_is_arithmetic(right)) {
    if (type_is_void(left) && !type_is_void(right)) return false;
    if (!type_is_void(left) && type_is_void(right)) return false;
  }
  return true;
}

bool types_are_compatible_binary(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node,
  int operation
) {
  /* this is only for complex types i.e arrays, pointers, functions */
  bool result = true;

  result = types_are_compatible_pointer_binary(left, right, lhs_node, rhs_node, operation);

  if (!result) return result;

  result = types_are_compatible_function(left, right, lhs_node, rhs_node);

  return result;
}

bool types_are_compatible_assignment(
  struct type *left, struct type *right,
  struct node *lhs_node, struct node *rhs_node
) {
  bool result = true;

  result = types_are_compatible_arithmetic_assignment(left, right);

  if (!result) return result;

  result = types_are_compatible_array_assignment(left, right, lhs_node, rhs_node);

  if (!result) return result;

  result = types_are_compatible_pointer_assignment(left, right, lhs_node, rhs_node);

  if (!result) return result;

  result = types_are_compatible_function(left, right, lhs_node, rhs_node);

  return result;
}

int type_size(struct type *t) {
  switch (t->kind) {
    case TYPE_POINTER:
    case TYPE_BASIC:
      switch (t->data.basic.datatype) {
        case TYPE_BASIC_CHAR:
          return 1;
        case TYPE_BASIC_SHORT:
          return 2;
        case TYPE_BASIC_INT:
          return 4;
        case TYPE_BASIC_LONG:
          return 4;
        default:
          assert(0);
          break;
      }
      break;
    case TYPE_ARRAY:
    case TYPE_FUNCTION:
    case TYPE_LABEL:
      return 0;
    default:
      return 0;
  }
}

/*****************
 * TYPE CHECKING *
 *****************/

void type_assign_in_expression(struct node *expression);
struct type *get_type_from_expression(struct node *expression);

void type_convert_usual_unary(struct node *expression) {
  assert(NODE_UNARY_EXPR == expression->kind);

  struct type *type = get_type_from_expression(expression->data.unary_expr.expression);

  if (NULL == type) return;

  // if you're trying to dereference smth make sure its either pointer or array or array subscript
  if (UNARY_POINTER == expression->data.unary_expr.operation) {
    if ((TYPE_POINTER != type->kind) && (TYPE_ARRAY != type->kind) && (!expression->data.unary_expr.is_subscript_operator)) {
      compiler_print_error(expression->location, "Indirection requires pointer operand");
      type_conversion_error_count++;
    }
  }

  // if you're trying to get address of smth make sure its either identifier or array subscript
  if (UNARY_ADDRESS == expression->data.unary_expr.operation) {
    if (NODE_IDENTIFIER != expression->data.unary_expr.expression->kind) {
      compiler_print_error(expression->location, "Address operator requires identifier operand");
      type_conversion_error_count++;
    }
  }

  // if its subscript then make sure you're using it on array only
  if (expression->data.unary_expr.is_subscript_operator) {
    // get name of the subscript operator
    struct node *temp = expression->data.unary_expr.expression;
    assert(NODE_EXPRESSION_LIST_STATEMENT == temp->kind);
    while (NODE_EXPRESSION_LIST_STATEMENT == temp->kind) {
      temp = temp->data.expression_list_statement.self;
    }
    if (NODE_BINARY_OPERATION == temp->kind) {
      if (NODE_CAST_EXPR == temp->data.binary_operation.left_operand->kind) {
        temp = temp->data.binary_operation.left_operand->data.cast_expr.expression;
      } else {
        temp = temp->data.binary_operation.left_operand;
      }
      if (TYPE_ARRAY != get_type_from_expression(temp)->kind && TYPE_POINTER != get_type_from_expression(temp)->kind) {
        compiler_print_error(expression->location, "Subscript operator requires array operand"  );
        type_conversion_error_count++;
      }
    }
    /* cleanup */
    temp = NULL;
    free(temp);
  }

  struct type *type_converted_to = perform_unary_conversions(
    expression,
    type,
    true
  );

  assert(NULL != type_converted_to);
  expression->data.unary_expr.result.type = type_converted_to;
  expression->data.unary_expr.result.value =
    expression->data.unary_expr.operation == UNARY_POINTER ? L_VALUE : R_VALUE;

  /* cleanup */
  type = NULL;
  free(type);
}

void type_convert_inc_dec_unary(struct node *expression) {
  assert(NODE_UNARY_EXPR == expression->kind);

  struct type *type = get_type_from_expression(expression->data.unary_expr.expression);

  // check for lvalue
  if (get_value_from_expression(expression->data.unary_expr.expression) != L_VALUE) {
    compiler_print_error(expression->location, "Lvalue required");
    type_conversion_error_count++;
    return;
  }
  
  struct compound_operator *compound_operator = malloc(sizeof(struct compound_operator));
  compound_operator->operator = expression->data.unary_expr.expression;
  compound_operator->result = expression->data.unary_expr.expression;
  // Setting the length arg to 0 because its not getting used inside the function!
  compound_operator->right_operator = node_number(expression->location, "1", 0);

  if (NULL == type) return;

  struct type *type_converted_to = perform_unary_conversions(
    expression,
    type,
    false
  );

  struct type *final_type_converted_to = perform_assignment_conversions(
      compound_operator->result,
      compound_operator->operator,
      type, type_converted_to,
      false
    );

  assert(NULL != final_type_converted_to);
  
  compound_operator->operand_type = type_converted_to;
  compound_operator->result_type = final_type_converted_to;
  expression->data.unary_expr.compound_operator = compound_operator;

  expression->data.unary_expr.result.type = final_type_converted_to;
  expression->data.unary_expr.result.value = R_VALUE;

  /* cleanup */
  type = NULL;
  free(type);
}

void type_convert_compound_assignment(struct node *expression) {
  assert(NODE_BINARY_OPERATION == expression->kind);

  struct type *type_left = get_type_from_expression(expression->data.binary_operation.left_operand);
  struct type *type_right = get_type_from_expression(expression->data.binary_operation.right_operand);
  
  struct compound_operator *compound_operator = malloc(sizeof(struct compound_operator));
  compound_operator->operator = expression->data.binary_operation.left_operand;
  compound_operator->result = expression->data.binary_operation.left_operand;


  if (NULL == type_left || NULL == type_right) return;

  struct type *type_converted_to = perform_binary_conversions(
    expression->data.binary_operation.operation,
    expression->data.binary_operation.left_operand,
    expression->data.binary_operation.right_operand,
    type_left, type_right, false
  );

  struct type *final_type_converted_to = perform_assignment_conversions(
    compound_operator->result,
    compound_operator->operator,
    type_left, type_converted_to,
    false
  );

  assert(NULL != final_type_converted_to);
  
  compound_operator->operand_type = type_converted_to;
  compound_operator->result_type = final_type_converted_to;
  expression->data.binary_operation.compound_operator = compound_operator;

  expression->data.binary_operation.result.type = final_type_converted_to;
  expression->data.binary_operation.result.value = R_VALUE;

  /* cleanup */
  type_left = NULL;
  free(type_left);
  type_right = NULL;
  free(type_right);
}


void type_convert_usual_binary(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  struct type *type_left = get_type_from_expression(binary_operation->data.binary_operation.left_operand);
  struct type *type_right = get_type_from_expression(binary_operation->data.binary_operation.right_operand);

  /* we don't allow (pointer/array) with (pointer/array) combinations for the following binops */
  if ((type_left->kind == TYPE_ARRAY && type_right->kind == TYPE_POINTER) 
  || (type_right->kind == TYPE_ARRAY && type_left->kind == TYPE_POINTER)
  || (type_left->kind == TYPE_ARRAY && type_right->kind == TYPE_ARRAY)
  || (type_left->kind == TYPE_POINTER && type_right->kind == TYPE_POINTER)) {

    // we do allow subscript additions
    if (
      (NODE_UNARY_EXPR == binary_operation->data.binary_operation.left_operand->kind && (
        binary_operation->data.binary_operation.left_operand->data.unary_expr.is_subscript_operator ||
        UNARY_POINTER == binary_operation->data.binary_operation.left_operand->data.unary_expr.operation
      )) ||
      (NODE_UNARY_EXPR == binary_operation->data.binary_operation.right_operand->kind && (
        binary_operation->data.binary_operation.right_operand->data.unary_expr.is_subscript_operator ||
        UNARY_POINTER == binary_operation->data.binary_operation.right_operand->data.unary_expr.operation
      ))
    ) {
      // do nothing
    } else {
      switch (binary_operation->data.binary_operation.operation) {
        case BINOP_MULTIPLICATION:
        case BINOP_DIVISION:
        case BINOP_ADDITION:
        case BINOP_MOD:
        case BITWISE_OR:
        case BITWISE_XOR:
        case BITWISE_AND:
        case LEFT_SHIFT:
        case RIGHT_SHIFT: {
          compiler_print_error(binary_operation->location, "Incompatible operation between (pointer/array) with (pointer/array)");
          type_conversion_error_count++;
          return;
        }
      }
    }
  }

  struct type *type_converted_to = perform_binary_conversions(
    binary_operation->data.binary_operation.operation,
    binary_operation->data.binary_operation.left_operand,
    binary_operation->data.binary_operation.right_operand,
    type_left, type_right, true);

  // set type of result of binary operation
  if (NULL != type_converted_to) {
    binary_operation->data.binary_operation.result.type = type_converted_to;
    binary_operation->data.binary_operation.result.value = R_VALUE;
  }

  /* cleanup */
  type_left = NULL;
  free(type_left);
  type_right = NULL;
  free(type_right);
}


void type_convert_assignment(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  struct type *type_left = get_type_from_expression(binary_operation->data.binary_operation.left_operand);
  struct type *type_right = get_type_from_expression(binary_operation->data.binary_operation.right_operand);

  if (
    get_value_from_expression(binary_operation->data.binary_operation.left_operand) != L_VALUE
    || !is_expr_modifiable(binary_operation->data.binary_operation.left_operand)
  ) {
    compiler_print_error(binary_operation->location, "Expression must be a modifiable lvalue");
    type_conversion_error_count++;
  } else {
    struct type *type_converted_to = perform_assignment_conversions(
      binary_operation->data.binary_operation.left_operand,
      binary_operation->data.binary_operation.right_operand,
      type_left, type_right,
      true
    );

    binary_operation->data.binary_operation.result.type = type_converted_to;
    binary_operation->data.binary_operation.result.value = R_VALUE;
  }
}

void type_assign_in_binary_operation(struct node *binary_operation) {
  assert(NODE_BINARY_OPERATION == binary_operation->kind);

  switch (binary_operation->data.binary_operation.operation) {
    case BINOP_MULTIPLICATION:
		case BINOP_DIVISION:
		case BINOP_ADDITION:
		case BINOP_SUBTRACTION:
		case BINOP_MOD:
		case BITWISE_OR:
		case BITWISE_XOR:
		case BITWISE_AND:
		case LEFT_SHIFT:
		case RIGHT_SHIFT:
		case EQUALITY_OP:
		case LOGICAL_OR:
		case LOGICAL_AND:
		case INEQUALITY_OP:
		case LESS_THAN:
		case LESS_THAN_EQ:
		case GREATER_THAN:
		case GREATER_THAN_EQ:
      type_assign_in_expression(binary_operation->data.binary_operation.left_operand);
      type_assign_in_expression(binary_operation->data.binary_operation.right_operand);
      type_convert_usual_binary(binary_operation);
      break;
    case BINOP_ASSIGN: {
      // dont perform any conversion only if its an identifier of basic type :((((
      if (
        NODE_IDENTIFIER == binary_operation->data.binary_operation.left_operand->kind &&
        TYPE_BASIC == binary_operation->data.binary_operation.left_operand->data.identifier.symbol->type_tree->type->kind
      ) { /* do nothing */ } else {
        type_assign_in_expression(binary_operation->data.binary_operation.left_operand);
      }
      type_assign_in_expression(binary_operation->data.binary_operation.right_operand);
      type_convert_assignment(binary_operation);
      break;
    }
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
      type_assign_in_expression(binary_operation->data.binary_operation.left_operand);
      type_assign_in_expression(binary_operation->data.binary_operation.right_operand);
      type_convert_compound_assignment(binary_operation);
      break;
    default:
      assert(0);
      break;
  }
}

void type_assign_in_usual_unary(struct node *unary_expr) {
  assert(NODE_UNARY_EXPR == unary_expr->kind);

  type_assign_in_expression(unary_expr->data.unary_expr.expression);
  
  switch (unary_expr->data.unary_expr.operation) {
		case UNARY_PREINC:
		case UNARY_PREDEC:
		case UNARY_POSTINC:
		case UNARY_POSTDEC:
      type_convert_inc_dec_unary(unary_expr);
      break;
		case UNARY_HYPHEN:
		case UNARY_PLUS:
		case UNARY_LOGICAL_NEGATION:
		case UNARY_BITWISE_NEGATION:
		case UNARY_ADDRESS:
		case UNARY_POINTER:
      type_convert_usual_unary(unary_expr);
      break;
    default:
      assert(0);
      break;
  }
}

void type_convert_usual_cast(struct node *cast, struct node *expression) {

}

void type_assign_in_cast(struct node *cast_expr) {
  assert(NODE_CAST_EXPR == cast_expr->kind);

  // handle this later
  type_convert_usual_cast(cast_expr->data.cast_expr.type, cast_expr->data.cast_expr.expression);
}

static void type_assign_in_function_arguments(struct node *arguments_list) {
  if (arguments_list->data.arguments_list.self->kind == NODE_ARGUMENTS_LIST) {
    type_assign_in_function_arguments(arguments_list->data.arguments_list.self);
  } else {
    type_assign_in_expression(arguments_list->data.arguments_list.self);
  }

  if (arguments_list->data.arguments_list.next) {
    type_assign_in_expression(arguments_list->data.arguments_list.next);
  }
}


void type_traverse_into_function_call(struct node *function) {
  assert(NODE_FUNCTION_DECL == function->kind);

  if (NULL != function->data.function.argument_list) {
    type_assign_in_expression(function->data.function.argument_list);
  }

  if (
    is_syscall(function->data.function.name->data.identifier.name)
  ) return;


  /* check if arguments type match function signature */
  /* get type tree of this function */
  assert(NULL != function->data.function.symbol->type_tree);
  struct type_tree *type_tree = function->data.function.symbol->type_tree;

  assert(NULL != type_tree);

  /* get argument types of this function declaration */
  compare_func_arguments(type_tree, function->data.function.argument_list, function);
}

enum value get_value_from_expression(struct node *expression) {
  assert(NULL != expression);

  switch (expression->kind) {
    case NODE_IDENTIFIER:
      return expression->data.identifier.result.value;
    case NODE_NUMBER:
      return expression->data.number.result.value;
    case NODE_STRING:
      return expression->data.string.result.value;
    case NODE_FUNCTION_DECL: 
      return expression->data.function.result.value;
    case NODE_BINARY_OPERATION: {
      if (NULL == expression->data.binary_operation.result.type) {
        return NOT_AVAIALABLE;
      } else {
        return expression->data.binary_operation.result.value;
      }
    }
    case NODE_UNARY_EXPR: {
      if (NULL == expression->data.unary_expr.result.type) {
        return NOT_AVAIALABLE;
      } else {
        return expression->data.unary_expr.result.value;
      }
    }
    case NODE_CAST_EXPR: {
      if (NULL == expression->data.cast_expr.result.type) {
        return R_VALUE;
      } else {
        return expression->data.cast_expr.result.value;
      }
    }
    case NODE_CONDITIONAL_STATEMENT: 
      return expression->data.conditional_statement.result.value;
    case NODE_EXPRESSION_STATEMENT: 
      return get_value_from_expression(expression->data.expression_statement.expression);
    case NODE_EXPRESSION_LIST_STATEMENT: {
      if (NULL == expression->data.expression_list_statement.next) {
        return get_value_from_expression(expression->data.expression_list_statement.self);
      } else {
        return get_value_from_expression(expression->data.expression_list_statement.next);
      }
    }
    default:
      assert(0);
      return NOT_AVAIALABLE;
  }
}

struct type *get_type_from_expression(struct node *expression) {
  assert(NULL != expression);
  switch (expression->kind) {
    case NODE_IDENTIFIER:
      assert(NULL != expression->data.identifier.symbol);
      return expression->data.identifier.symbol->type_tree->type;
    case NODE_NUMBER:
      return expression->data.number.result.type;
    case NODE_POINTER: 
      return expression->data.pointer.points_to->data.type.type_node;
    case NODE_STRING:
      return expression->data.string.result.type;
    case NODE_TYPE: 
      return expression->data.type.type_node;
    case NODE_FUNCTION_DECL:
      assert(NULL != expression->data.function.symbol);
      return expression->data.function.symbol->type_tree->type;
    case NODE_FUNCTION_DEFINITION:
      return get_type_from_expression(expression->data.function_definition_statement.type);
    case NODE_CONDITIONAL_STATEMENT: 
      return get_type_from_expression(expression->data.conditional_statement.true_block);
    case NODE_BINARY_OPERATION: {
      if (expression->data.binary_operation.result.type == NULL) {
        compiler_print_error(expression->location, "Binary operation type not assigned");
        type_conversion_error_count++;
        return NULL;
      } else {
        return expression->data.binary_operation.result.type;
      }
    }
    case NODE_UNARY_EXPR: {
      if (expression->data.unary_expr.result.type == NULL) {
        compiler_print_error(expression->location, "Unary operation type not assigned");
        type_conversion_error_count++;
        return NULL;
      } else {
        return expression->data.unary_expr.result.type;
      }
    }
    case NODE_CAST_EXPR: {
      if (NODE_TYPE ==  expression->data.cast_expr.type->kind) {
        if (expression->data.cast_expr.type->data.type.type_node == NULL) {
          compiler_print_error(expression->location, "Cast operation type not assigned");
          type_conversion_error_count++;
          return NULL;
        } else {
          return expression->data.cast_expr.type->data.type.type_node;
        }
      } else if (NODE_POINTER == expression->data.cast_expr.type->kind) {
        if (expression->data.cast_expr.type->data.pointer.points_to->data.type.type_node == NULL) {
          compiler_print_error(expression->location, "Cast operation type not assigned");
          type_conversion_error_count++;
          return NULL;
        } else {
          return expression->data.cast_expr.type->data.pointer.points_to->data.type.type_node;
        }
      }
    }
    case NODE_EXPRESSION_STATEMENT: {
      return get_type_from_expression(expression->data.expression_statement.expression);
    }
    case NODE_EXPRESSION_LIST_STATEMENT: {
      if (NULL == expression->data.expression_list_statement.next) {
        return get_type_from_expression(expression->data.expression_list_statement.self);
      } else {
        return get_type_from_expression(expression->data.expression_list_statement.next);
      }
    }
    default:
      assert(0);
      return NULL;
  }
  return NULL;
}

void type_assign_in_conditional_statement(struct node *statement) {
  assert(NODE_CONDITIONAL_STATEMENT == statement->kind);

  type_assign_in_expression(statement->data.conditional_statement.condition);

  if (NULL != statement->data.conditional_statement.true_block) {
    type_assign_in_possible_statement(statement->data.conditional_statement.true_block);
  }
  if (NULL != statement->data.conditional_statement.false_block) {
    if (statement->data.conditional_statement.is_ternary) {
      type_assign_in_expression(statement->data.conditional_statement.false_block);
    } else {
      type_assign_in_possible_statement(statement->data.conditional_statement.false_block);
    }
  }
}

static void type_check_op_validity(struct node *expression) {
  assert(NODE_IDENTIFIER == expression->kind);

  struct node *temp = expression; 
  if (TYPE_FUNCTION == get_type_from_expression(expression)->kind) {
    // check if its coming under a subscript operator
    while (NULL != temp) {
      if (NODE_UNARY_EXPR == temp->kind) {
        if (temp->data.unary_expr.is_subscript_operator) {
          compiler_print_error(expression->location, "Subscript operator cannot be applied to function");
          type_conversion_error_count++;
          break;
        }
        // check for logical negation
        if (UNARY_LOGICAL_NEGATION == temp->data.unary_expr.operation) {
          compiler_print_error(expression->location, "Logical negation cannot be applied to function");
          type_conversion_error_count++;
          break;
        }
        // check for unary +/-
        if (UNARY_HYPHEN == temp->data.unary_expr.operation || UNARY_PLUS == temp->data.unary_expr.operation) {
          compiler_print_error(expression->location, "Unary plus/minus cannot be applied to function");
          type_conversion_error_count++;
          break;
        }
      }
      temp = temp->parent;
    }
  }
  if (TYPE_ARRAY == get_type_from_expression(expression)->kind) {
    // check if its coming under a subscript operator
    while (NULL != temp) {
      if (NODE_UNARY_EXPR == temp->kind) {
        if (!temp->data.unary_expr.is_subscript_operator) {
          // check for logical negation
          if (UNARY_LOGICAL_NEGATION == temp->data.unary_expr.operation) {
            compiler_print_error(expression->location, "Logical negation cannot be applied to array");
            type_conversion_error_count++;
            break;
          }
          // check for unary +/-
          if (UNARY_HYPHEN == temp->data.unary_expr.operation || UNARY_PLUS == temp->data.unary_expr.operation) {
            compiler_print_error(expression->location, "Unary plus/minus cannot be applied to array");
            type_conversion_error_count++;
            break;
          }
        }
      }
      temp = temp->parent;
    }
  }
  /* cleanup */
  temp = NULL;
  free(temp);
}

// do a depth first traversal!
void type_assign_in_expression(struct node *expression) {
  assert(NULL != expression);

  switch (expression->kind) {
    case NODE_IDENTIFIER: 
      if (TYPE_ARRAY == expression->data.identifier.symbol->type_tree->type->kind) {
        type_convert_usual_unary_array(expression);
      }
      type_check_op_validity(expression);
      break;
    case NODE_STRING:
      type_convert_string(expression);
      break;
    case NODE_BINARY_OPERATION:
      type_assign_in_binary_operation(expression);
      break;
    case NODE_UNARY_EXPR:
      type_assign_in_usual_unary(expression);
      break;
    case NODE_CAST_EXPR: 
      type_assign_in_cast(expression);
      break;
    case NODE_CONDITIONAL_STATEMENT: 
      type_assign_in_conditional_statement(expression);
      break;
    case NODE_ARGUMENTS_LIST: 
      type_assign_in_function_arguments(expression);
      break;
    case NODE_EXPRESSION_LIST_STATEMENT: 
    case NODE_EXPRESSION_STATEMENT: 
      type_assign_in_expression_list_statement(expression);
      break;
    case NODE_FUNCTION_DECL: 
      type_traverse_into_function_call(expression);
      break;
    default:
      break;
  }
}

void type_assign_in_expression_list_statement(struct node *expression_statement) {
	assert(
		NODE_EXPRESSION_STATEMENT == expression_statement->kind
		|| NODE_EXPRESSION_LIST_STATEMENT == expression_statement->kind
	);
	if (expression_statement->data.expression_list_statement.self->kind == NODE_EXPRESSION_LIST_STATEMENT) {
		type_assign_in_expression_list_statement(expression_statement->data.expression_list_statement.self);
  } else {
    type_assign_in_expression(expression_statement->data.expression_list_statement.self);
  }

  if (NULL != expression_statement->data.expression_list_statement.next) {
    type_assign_in_expression(expression_statement->data.expression_list_statement.next);
  }
}

void type_traverse_into_compound_statement(struct node *statement) {
  if (NULL == statement) return;

  assert(NODE_COMPOUND_STATEMENT == statement->kind);

  type_assign_in_statement_list(statement->data.compound_statement.statement);
}

void type_traverse_into_function_definition(struct node *function) {
  assert(NODE_FUNCTION_DEFINITION == function->kind);

  type_traverse_into_compound_statement(function->data.function_definition_statement.function_body);
}

void type_check_break_or_continue_stmt_scope(struct node *statement) {
  assert(NULL != statement);

  struct node *temp = statement;
  int wrapped_in_iterative_stmt = 0;

  while(NULL != temp) {
    if (NODE_ITERATIVE_STATEMENT == temp->kind) {
      wrapped_in_iterative_stmt = 1;
      break;
    }
    temp = temp->parent;
  }
  /* cleanup */
  temp = NULL;
  free(temp);

  if (!wrapped_in_iterative_stmt) {
    compiler_print_error(statement->location, "'break'/'continue' statement not in loop statement ");
    type_conversion_error_count++;
  }
}

void type_assign_in_return_statement(struct node *statement) {
  assert(NODE_RETURN_STATEMENT == statement->kind);
  // check if return statement is in function definiton scope
  struct node *temp = statement;
  int wrapped_in_function_definition = 0;

  if (NULL != statement->data.return_statement.expression) {
    type_assign_in_expression_list_statement(statement->data.return_statement.expression);
  }
  
  while(temp != NULL) {
    if (NODE_FUNCTION_DEFINITION == temp->kind) {
      wrapped_in_function_definition = 1;

      // check if this is the return for the main function
      if (strcmp(
        temp->data.function_definition_statement.function_decl->data.function.name->data.identifier.name,
        "main") == 0
      ) {
        if (types_are_compatible_assignment(
          temp->data.function_definition_statement.type->data.type.type_node,
          get_type_from_expression(statement->data.return_statement.expression),
          temp->data.function_definition_statement.type,
          statement->data.return_statement.expression
        )) {
          main_return_valid = 1;
        }
      }

      break;
    }
    temp = temp->parent;
  }

  if (!wrapped_in_function_definition) {
    compiler_print_error(statement->location, "'return' statement not in function definition ");
    type_conversion_error_count++;
    /* cleanup */
    temp = NULL;
    free(temp);
    return;
  }

  assert(NODE_FUNCTION_DEFINITION == temp->kind);

  struct type *function_return_type = NULL;
  struct type *return_type = NULL;

  if (NULL != temp->data.function_definition_statement.type) {
    function_return_type = get_type_from_expression(temp);
  }
  if (NULL != statement->data.return_statement.expression) {
    return_type = get_type_from_expression(statement->data.return_statement.expression);
  }

  if (NULL != function_return_type && NULL != return_type) {
    struct type *type_converted_to = perform_assignment_conversions(
      temp->data.function_definition_statement.type,
      statement->data.return_statement.expression,
      function_return_type,
      return_type,
      true
    );
    node_assign_result(
      get_value_from_expression(statement->data.return_statement.expression),
      type_converted_to,
      statement->data.return_statement.expression
    );
  } else if (NULL == function_return_type && NULL != return_type) {
    compiler_print_error(statement->location, "Function should not return a value");
    type_conversion_error_count++;
  } else if (NULL != function_return_type && !type_is_void(function_return_type) && NULL == return_type) {
    compiler_print_error(statement->location, "Non-void function should return a value");
    type_conversion_error_count++;
  }

  /* cleanup */
  function_return_type = NULL;
  free(function_return_type);
  return_type = NULL;
  free(return_type);
  temp = NULL;
  free(temp);
}

void type_assign_in_iterative_statement(struct node *statement) {
  assert(NODE_ITERATIVE_STATEMENT == statement->kind);

  if (statement->data.iterative_statement.type == 2) {
    if (NULL != statement->data.iterative_statement.expression->data.for_expr.initializer) {
      type_assign_in_expression_list_statement(statement->data.iterative_statement.expression->data.for_expr.initializer);
    }
    if (NULL != statement->data.iterative_statement.expression->data.for_expr.condition) {
      type_assign_in_expression_list_statement(statement->data.iterative_statement.expression->data.for_expr.condition);
    }
    if (NULL != statement->data.iterative_statement.expression->data.for_expr.bound) {
      type_assign_in_expression_list_statement(statement->data.iterative_statement.expression->data.for_expr.bound);
    }
  } else {
    type_assign_in_expression(statement->data.iterative_statement.expression);
  }
  type_assign_in_possible_statement(statement->data.iterative_statement.statement);
}

int type_assign_in_possible_statement(struct node *statement) {
	if (statement == NULL) return 0;

	switch(statement->kind) {
		case NODE_FUNCTION_DEFINITION:
			type_traverse_into_function_definition(statement);
      break;
		case NODE_EXPRESSION_LIST_STATEMENT:
		case NODE_EXPRESSION_STATEMENT: 
			type_assign_in_expression_list_statement(statement);
      break;
		case NODE_STATEMENT_LIST:
			return type_assign_in_statement_list(statement);
		case NODE_CONTINUE_STATEMENT:
		case NODE_BREAK_STATEMENT:
      type_check_break_or_continue_stmt_scope(statement);
      break;
		case NODE_COMPOUND_STATEMENT:
      type_assign_in_possible_statement(statement->data.compound_statement.statement);
      break;
		case NODE_RETURN_STATEMENT:
      type_assign_in_return_statement(statement);
      break;
		case NODE_ITERATIVE_STATEMENT:
      type_assign_in_iterative_statement(statement);
      break;
		case NODE_CONDITIONAL_STATEMENT: 
      type_assign_in_conditional_statement(statement);
      break;
		case NODE_LABEL_STATEMENT: {
      type_assign_in_possible_statement(statement->data.label_statement.statement);
      break;
    }
		case NODE_GOTO_STATEMENT: 
		default:
      break;
	}
  return 0;
}

int type_assign_in_statement_list(struct node *statement_list) {
  assert(NODE_STATEMENT_LIST == statement_list->kind);
  if (NULL != statement_list->data.statement_list.init) {
    type_assign_in_statement_list(statement_list->data.statement_list.init);
  }
  type_assign_in_possible_statement(statement_list->data.statement_list.statement);
  return type_conversion_error_count;
}

int type_check_main_func_validity() {
  assert(NULL != file_scope_table);

  // get type tree of main function
  struct symbol *symbol = symbol_get(file_scope_table, "main");

  struct location location = {1, 1, 1, 1};
  
  if (NULL == symbol) {
    // compiler_print_error(location, "'main' function does not exist!");
    // type_conversion_error_count++;
  } else {
    // check return type
    if (get_return_type_struct(symbol->type_tree)->data.basic.datatype != TYPE_BASIC_INT) {
      compiler_print_error(location, "Invalid return type of 'main' function");
      type_conversion_error_count++;
    }

    // check arguments... most difficult
    struct type_chain *type_chain = symbol->type_tree->type_chain;
    struct type *type = NULL;
    struct type *prev_type = NULL;
    int current_arg_num = 0;
    int args_valid = 0;
    while(NULL != type_chain) {
      if (NODE_IDENTIFIER == type_chain->type->kind && NULL != type_chain->next) {
        current_arg_num++;
        type = type_chain->type->data.identifier.symbol->type_tree->type;
        assert(NULL != type);
        if (current_arg_num == 1) {
          if (type->data.basic.datatype == TYPE_BASIC_VOID) {
            args_valid = 1;
          }
        } if (current_arg_num == 2 && prev_type->data.basic.datatype == TYPE_BASIC_INT) {
          if (
            TYPE_BASIC_CHAR == type->data.basic.datatype
            && TYPE_ARRAY == type->kind
            && does_array_contain_pointer(type_chain->type->data.identifier.symbol->type_tree)
          ) {
            args_valid = 1;
          }
        }
        prev_type = type;
      } 
      type_chain = type_chain->next;
    }

    if (current_arg_num >= 3 || (current_arg_num > 0 && current_arg_num < 3 && args_valid == 0)) {
      compiler_print_error(location, "Invalid arguments of 'main' function");
      type_conversion_error_count++;
    }
    if (main_return_valid == 0) {
      compiler_print_error(location, "Invalid return value of 'main' function");
      type_conversion_error_count++;
    }
  
    /* cleanup */
    type = NULL;
    free(type);
    prev_type = NULL;
    free(prev_type);
    type_chain = NULL;
    free(type_chain);
  }

  return type_conversion_error_count;
}


/**************************
 * PRINT TYPE EXPRESSIONS *
 **************************/
static void type_print_basic(FILE *output, struct type *basic) {
  if (TYPE_BASIC_VOID != basic->data.basic.datatype) {
    if (basic->data.basic.is_unsigned) {
      fputs("unsigned ", output);
    } else {
      fputs("signed ", output);
    }
  }

  switch (basic->data.basic.datatype) {
    case TYPE_BASIC_INT:
      fputs("int", output);
      break;
    case TYPE_BASIC_LONG:
      fputs("long int", output);
      break;
    case TYPE_BASIC_SHORT:
      fputs("short int", output);
      break;
    case TYPE_BASIC_CHAR:
      fputs("char", output);
      break;
    case TYPE_BASIC_VOID:
      fputs("void", output);
      break;
    case TYPE_BASIC_UNDEF:
      fputs("undef", output);
      break;
    default:
      assert(0);
      break;
  }
}

char* get_type_string(struct type *basic) {
  char type_string_temp[100];

  type_string_temp[0] = '\0';

  if (TYPE_BASIC_VOID != basic->data.basic.datatype) {
    if (basic->data.basic.is_unsigned) {
      strcat(type_string_temp, "unsigned ");
    } else {
      strcat(type_string_temp, "signed ");
    }
  }

  switch (basic->data.basic.datatype) {
    case TYPE_BASIC_INT:
      strcat(type_string_temp, "int");
      break;
    case TYPE_BASIC_LONG:
      strcat(type_string_temp, "long int");
      break;
    case TYPE_BASIC_SHORT:
      strcat(type_string_temp, "short int");
      break;
    case TYPE_BASIC_CHAR:
      strcat(type_string_temp, "char");
      break;
    case TYPE_BASIC_VOID:
      strcat(type_string_temp, "void");
      break;
    case TYPE_BASIC_UNDEF:
      strcat(type_string_temp, "undef");
      break;
    default:
      assert(0);
      break;
  }

  char *type_string = (char*)malloc(strlen(type_string_temp) + 1);
  strcpy(type_string, type_string_temp);
  return type_string;

  // should free memory
}

void type_print(FILE *output, struct type *kind) {
  assert(NULL != kind);

  type_print_basic(output, kind);
}
