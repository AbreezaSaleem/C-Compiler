#ifndef _IR_H
#define _IR_H

#include <stdio.h>
#include <stdbool.h>

struct node;
struct symbol;
struct symbol_table;

enum ir_operand_kind {
  OPERAND_NUMBER,
  OPERAND_STRING,
  OPERAND_TEMPORARY,
  OPERAND_BOOL
};
struct ir_operand {
  enum ir_operand_kind kind;

  union {
    char name[IDENTIFIER_MAX + 1];
    unsigned long number;
    bool is_string;         /* used to distinguish between number and string */
    int temporary;
    int variable_id;       /* used in mips step to fetch address location for variable#(1/2/3..) */
  } data;                  /* since we're using int as key we can't just rely on name since thats string */
};

enum ir_instruction_kind_generic {
  IR_LOAD,
  IR_STORE,
  IR_MULT,
  IR_DIV,
  IR_ADD,
  IR_SUB,
  IR_REM,
  IR_LEFT_SHIFT,
  IR_RIGHT_SHIFT,
  IR_LT,
  IR_LE,
  IR_GT,
  IR_GE,
  IR_BITWISE_OR,
  IR_BITWISE_XOR,
  IR_BITWISE_AND,
  IR_EQ,
  IR_NE,
  IR_RETURN,
  IR_RESULT
};

enum ir_instruction_kind {
  IR_ADDRESS_OF,
  IR_LOAD_WORD,
  IR_LOAD_HALF_WORD,
  IR_LOAD_SIGNED_HALF_WORD,
  IR_LOAD_BYTE,
  IR_LOAD_SIGNED_BYTE,
  IR_STORE_WORD,
  IR_STORE_HALF_WORD,
  IR_STORE_BYTE,
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
  IR_RIGHT_SHIFT_UNSIGNED_WORD,
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
  IR_BITWISE_OR_WORD,
  IR_UNARY_MINUS,
  IR_UNARY_LOGICAL_NEGATION,
  IR_UNARY_BITWISE_NEGATION,
  IR_CONST_INT,
  IR_CAST_WORD_TO_HALF_WORD,
  IR_CAST_WORD_TO_BYTE,
  IR_CAST_HALF_WORD_TO_BYTE,
  IR_CAST_UNSIGNED_HALF_WORD_TO_WORD,
  IR_CAST_SIGNED_HALF_WORD_TO_WORD,
  IR_CAST_UNSIGNED_BYTE_TO_HALF_WORD,
  IR_CAST_SIGNED_BYTE_TO_HALF_WORD,
  IR_CAST_UNSIGNED_BYTE_TO_WORD,
  IR_CAST_SIGNED_BYTE_TO_WORD,
  IR_LABEL,
  IR_GOTO,
  IR_GOTO_IF_FALSE,
  IR_GOTO_IF_TRUE,
  IR_PROC_BEGIN,
  IR_PROC_END,
  IR_RETURN_WORD,
  IR_RETURN_HALF_WORD,
  IR_RETURN_BYTE,
  IR_CALL,
  IR_SYS_CALL,
  IR_PARAMETER,
  IR_RESULT_WORD,
  IR_RESULT_HALF_WORD,
  IR_RESULT_BYTE,
  IR_PHI,


  IR_NO_OPERATION,     /* 0 */
};
struct ir_instruction {
  enum ir_instruction_kind kind;
  struct ir_instruction *prev, *next;
  struct ir_operand operands[7];
  char* scope;
  int position;
  bool is_used;
  struct ir_instruction *offset_variable;      /* for store and load instructions */
};

struct ir_section {
  struct ir_instruction *first, *last;
};

struct ir_list {
  struct ir_instruction *data;
  struct ir_list *prev, *next;
};

int ir_generate_for_statement_list_wrapper(struct node *statement_list);
int ir_generate_for_statement_list(struct node *statement_list);
void ir_generate_for_expression_statement(struct node *expression_statement);
void ir_generate_for_possible_statement(struct node *statement);
void ir_generate_for_expression(struct node *expression);
void ir_generate_for_number(struct node *number);

struct ir_instruction *ir_instruction(enum ir_instruction_kind kind);
void ir_operand_temporary(struct ir_instruction *instruction, int position);
void ir_operand_copy(struct ir_instruction *instruction, int position, struct ir_operand *operand);
void ir_operand_number(struct ir_instruction *instruction, int position, signed long number);
void ir_append(struct node *node, struct ir_instruction *instruction);
void ir_insert(struct ir_instruction *old_instruction, struct ir_instruction *new_instruction, struct ir_instruction *original_instruction);

void ir_print_section(FILE *output);
void ir_print_instruction(FILE *output, struct ir_instruction *instruction);
void ir_print_opcode(FILE *output, enum ir_instruction_kind kind);


/* Get/Set IR_List */
struct ir_list* ir_get_ir_list();
void ir_remove_instruction_from_list(struct ir_instruction *instruction);
struct ir_instruction *ir_get_instruction_by_temporary(int temporary);
struct ir_instruction *ir_get_next_instruction(struct ir_instruction *instruction);

/* consts map */
const char* mips_get_register_from_temp(int temp_number);
const char* ir_get_const_map_entry(int temp_number);
void ir_update_constants_map(int key, int value);

/* optimization */
void ir_mark_constant_as_obselete(int temporary);
#endif
