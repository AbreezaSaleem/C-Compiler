#ifndef _UTILFUNCS_H
#define _UTILFUNCS_H

#define ARRAY_LENGTH 11

#include <stdbool.h>
#include <assert.h>
#include "../type.h"

struct node;

#define _SIGNED_CHAR_MAX 127
#define _SIGNED_CHAR_MIN -127

#define _SIGNED_SHORT_MAX 32767
#define _SIGNED_SHORT_MIN -32768

#define _SIGNED_INT_MAX 2147483647
#define _SIGNED_INT_MIN -2147483648

#define _SIGNED_LONG_MAX 2147483647
#define _SIGNED_LONG_MIN -2147483648

#define _UNSIGNED_CHAR_MAX 255
#define _UNSIGNED_CHAR_MIN 0

#define _UNSIGNED_SHORT_MAX 65535
#define _UNSIGNED_SHORT_MIN 0

#define _UNSIGNED_INT_MAX 4294967295
#define _UNSIGNED_INT_MIN 0

#define _UNSIGNED_LONG_MAX 4294967295
#define _UNSIGNED_LONG_MIN 0

struct expression_result {
  bool does_contain_identifier;
  signed long value;
};

enum types_list {
  SIGNED_INT,
  SIGNED_LONG_INT, 
  SIGNED_SHORT_INT,
  UNSIGNED_INT,
  UNSIGNED_SHORT_INT,
  UNSIGNED_LONG_INT,
  SIGNED_CHAR,
  UNSIGNED_CHAR,
  VOID
};

struct expression_result evaluate_expression(struct node *expression);

int can_represent_values(
  enum type_basic_kind type_1, bool is_unsigned_1,
  enum type_basic_kind type_2, bool is_unsigned_2
);

unsigned int mylog2 (unsigned int val);
int find_nearest_power_of_two(int constant);
bool is_power_of_two(int constant);

#endif /* _UTILFUNCS_H */
