#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "../symbol.h"
#include "./util-functions.h"

#include <stdbool.h>
#include <assert.h>

int evaluate_expr_error_count = 0;
int type_range_error_count = 0;

static int power_of_twos[] = {
  1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
};

static struct expression_result evaluate_expression_handler(struct node *expression) {
  struct expression_result result;

  result.does_contain_identifier = false; // Initialize to false by default
  result.value = _SIGNED_LONG_MIN; // Initialize to 0 by default

  if (expression == NULL) {
    return result;
  }

  switch(expression->kind) {
    case NODE_IDENTIFIER: 
      // Can't evaluate an identifier
      result.does_contain_identifier = true;
      break;
    case NODE_NUMBER: 
      result.value = expression->data.number.value;
      break;
    case NODE_BINARY_OPERATION: {
      struct expression_result left_operand = evaluate_expression_handler(expression->data.binary_operation.left_operand);
      struct expression_result right_operand = evaluate_expression_handler(expression->data.binary_operation.right_operand);
      switch(expression->data.binary_operation.operation) {
        case BINOP_MULTIPLICATION:
          result.value = left_operand.value * right_operand.value;
          break;
        case BINOP_DIVISION:
          result.value = left_operand.value / right_operand.value;
          break;
        case BINOP_ADDITION:
          result.value = left_operand.value + right_operand.value;
          break;
        case BINOP_SUBTRACTION:
          result.value = left_operand.value - right_operand.value;
          break;
        case BINOP_MOD:
          result.value = left_operand.value % right_operand.value;
          break;
        case LOGICAL_OR:
          result.value = left_operand.value || right_operand.value;
          break;
        case LOGICAL_AND:
          result.value = left_operand.value && right_operand.value;
          break;
        case BITWISE_OR:
          result.value = left_operand.value | right_operand.value;
          break;
        case BITWISE_XOR:
          result.value = left_operand.value ^ right_operand.value;
          break;
        case BITWISE_AND:
          result.value = left_operand.value & right_operand.value;
          break;
        case EQUALITY_OP:
          result.value = left_operand.value == right_operand.value;
          break;
        case INEQUALITY_OP:
          result.value = left_operand.value != right_operand.value;
          break;
        case LESS_THAN:
          result.value = left_operand.value < right_operand.value;
          break;
        case LESS_THAN_EQ:
          result.value = left_operand.value <= right_operand.value;
          break;
        case GREATER_THAN:
          result.value = left_operand.value > right_operand.value;
          break;
        case GREATER_THAN_EQ:
          result.value = left_operand.value >= right_operand.value;
          break;
        case LEFT_SHIFT:
          result.value = left_operand.value << right_operand.value;
          break;
        case RIGHT_SHIFT:
          result.value = left_operand.value >> right_operand.value;
          break;
        default:
          break;
      }
      result.does_contain_identifier = left_operand.does_contain_identifier || right_operand.does_contain_identifier;
      break;
    }
    case NODE_CAST_EXPR: 
      break;
    case NODE_UNARY_EXPR: {
      struct expression_result unary_result = evaluate_expression_handler(expression->data.unary_expr.expression);
      switch(expression->data.unary_expr.operation) {
        case UNARY_HYPHEN: 
          result.value = -unary_result.value;
          break;
        case UNARY_PLUS: 
          result.value = +unary_result.value;
          break;
        case UNARY_LOGICAL_NEGATION: 
          result.value = !unary_result.value;
          break;
        case UNARY_BITWISE_NEGATION:
          result.value = ~unary_result.value;
          break;
        case UNARY_ADDRESS:
          evaluate_expr_error_count++;
          break;
        case UNARY_PREINC:
          result.value = ++unary_result.value;
          break;
        case UNARY_PREDEC:
          result.value = --unary_result.value;
          break;
        case UNARY_POSTINC:
          result.value = unary_result.value++;
          break;
        case UNARY_POSTDEC:
          result.value = unary_result.value--;
          break;
        default:
          assert(0);
          break;
      }
      result.does_contain_identifier = unary_result.does_contain_identifier;
      break;
    }
    case NODE_CONDITIONAL_STATEMENT: {
      struct expression_result condition = evaluate_expression_handler(expression->data.conditional_statement.condition);
      if (condition.value) {
        return evaluate_expression_handler(expression->data.conditional_statement.true_block);
      } else {
        return evaluate_expression_handler(expression->data.conditional_statement.false_block);
      }
    }
    case NODE_EXPRESSION_LIST_STATEMENT:
      return evaluate_expression_handler(expression->data.expression_list_statement.self);
    case NODE_EXPRESSION_STATEMENT:
      return evaluate_expression_handler(expression->data.expression_statement.expression);
    default:
      printf("Error: Unknown expression kind %d\n", expression->kind);
      assert(0);
      break;
  }
  return result;
}

static long get_type_max(enum type_basic_kind type, bool is_unsigned) {
  if (is_unsigned) {
    switch(type) {
      case TYPE_BASIC_CHAR:
        return _UNSIGNED_CHAR_MAX;
      case TYPE_BASIC_SHORT:
        return _UNSIGNED_SHORT_MAX;
      case TYPE_BASIC_INT:
        return _UNSIGNED_INT_MAX;
      case TYPE_BASIC_LONG:
        return _UNSIGNED_LONG_MAX;
      default:
        type_range_error_count++;
        return 0;
    }
  } else {
    switch(type) {
      case TYPE_BASIC_CHAR: 
        return _SIGNED_CHAR_MAX;
      case TYPE_BASIC_SHORT:
        return _SIGNED_SHORT_MAX;
      case TYPE_BASIC_INT:
        return _SIGNED_INT_MAX;
      case TYPE_BASIC_LONG:
        return _SIGNED_LONG_MAX;
      default:
        type_range_error_count++;
        return 0;
    }
  }
}

struct expression_result evaluate_expression(struct node *expression) {
  evaluate_expr_error_count = 0;
  return evaluate_expression_handler(expression);
}

static long get_type_min(enum type_basic_kind type, bool is_unsigned) {
  if (is_unsigned) {
    return 0;
  }

  switch(type) {
    case TYPE_BASIC_CHAR: 
      return _SIGNED_CHAR_MIN;
    case TYPE_BASIC_SHORT:
      return _SIGNED_SHORT_MIN;
    case TYPE_BASIC_INT:
      return _SIGNED_INT_MIN;
    case TYPE_BASIC_LONG:
      return _SIGNED_LONG_MIN;
    default:
      type_range_error_count++;
      return 0;
  }
}

int can_represent_values(
  enum type_basic_kind type_1, bool is_unsigned_1,
  enum type_basic_kind type_2, bool is_unsigned_2
) {
  /* Check if type_1 can represent type_2 */
  int can_represent = 0; 
  type_range_error_count = 0; 

  if (
    get_type_max(type_1, is_unsigned_1) >= get_type_max(type_2, is_unsigned_2) &&
    get_type_min(type_1, is_unsigned_1) <= get_type_min(type_2, is_unsigned_2)
  ) {
    can_represent = 1;
  }
  return can_represent;
}

unsigned int mylog2 (unsigned int val) {
	// got this from https://stackoverflow.com/a/994647/6051241
	if (val == 0) return _SIGNED_INT_MAX;
	if (val == 1) return 0;
	unsigned int ret = 0;
	while (val > 1) {
		val >>= 1;
		ret++;
	}
	return ret;
}

bool is_power_of_two(int constant) {
	// got it from https://stackoverflow.com/a/600306/6051241
	return (constant != 0) && ((constant & (constant - 1)) == 0);
}

int find_nearest_power_of_two(int constant) {
	int min = _SIGNED_INT_MAX;
	int power_of_two = 99;
	for (int i = 0; i < ARRAY_LENGTH; i++) {
		if (abs(power_of_twos[i] - constant) < min) {
			min = abs(power_of_twos[i] - constant);
			power_of_two = power_of_twos[i];
		}
	}
	return power_of_two;
}
