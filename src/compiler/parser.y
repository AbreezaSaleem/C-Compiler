%verbose
%debug
%defines
%locations
%define api.pure
%lex-param {yyscan_t scanner}
%parse-param {YYSTYPE *root}
%parse-param {int *error_count}
%parse-param {yyscan_t scanner}
%token-table
%precedence IF
%precedence T_ELSE

%{

	#ifndef YY_TYPEDEF_YY_SCANNER_T
	#define YY_TYPEDEF_YY_SCANNER_T
	typedef void* yyscan_t;
	#endif

	#include <stdio.h>

	#include "compiler.h"
	#include "parser.tab.h"
	#include "scanner.yy.h"
	#include "node.h"
	#include "./helpers/util-functions.h"

	#define YYERROR_VERBOSE
	static void yyerror(YYLTYPE *loc, YYSTYPE *root,
											int *error_count, yyscan_t scanner,
											char const *s);
%}

%token T_BREAK
%token T_CHAR
%token T_CONTINUE
%token T_DO
%token T_ELSE
%token T_FOR
%token T_GOTO
%token T_IF
%token T_LONG
%token T_RETURN
%token T_SHORT
%token T_VOID
%token T_WHILE
%token T_DOUBLE
%token T_IDENTIFIER
%token T_NUMBER
%token T_STRING
%token T_DECIMAL
%token T_CHARACTER_LITERAL
%token T_PLUS
%token T_DOUBLE_QUOTE
%token T_NUMBER_SIGN
%token T_EQUAL
%token T_LEFT_BRACE
%token T_RIGHT_BRACE
%token T_PERCENT
%token T_TILDE
%token T_CIRCUMFLEX
%token T_LEFT_BRACKET
%token T_RIGHT_BRACKET
%token T_COMMA
%token T_PERIOD
%token T_ASTERISK
%token T_BACKSLASH
%token T_SLASH
%token T_LESS_THAN
%token T_GREATER_THAN
%token T_VERTICAL_BAR
%token T_LEFT_PARENTHESIS
%token T_RIGHT_PARENTHESIS
%token T_SEMICOLON
%token T_COLON
%token T_HYPHEN
%token T_QUESTION_MARK
%token T_APOSTROPHE
%token T_AMPERSAND
%token T_PLUS_EQUAL
%token T_MINUS_EQUAL
%token T_MULTIPLY_EQUAL
%token T_DIVIDE_EQUAL
%token T_MOD_EQUAL
%token T_LESS_LESS_EQUAL
%token T_GREATER_GREATER_EQUAL
%token T_AND_EQUAL
%token T_CIRCUMFLEX_EQUAL
%token T_VERTICAL_BAR_EQUAL
%token T_PLUS_PLUS
%token T_MINUS_MINUS
%token T_LESS_LESS
%token T_GREATER_GREATER
%token T_LESS_EQUAL
%token T_GREATER_EQUAL
%token T_EQUAL_EQUAL
%token T_NOT_EQUAL
%token T_AND_AND
%token T_OR_OR
%token T_COMMENT
%token T_SWITCH
%token T_CASE
%token T_DEFAULT
%token T_UNSIGNED
%token T_SIGNED
%token T_INT
%token T_STATIC
%token T_SIZEOF
%token T_CONST
%token T_NOT



%start program

%%

  /* ENTRY RULES */
program
  : translation_unit
          { *root = $1; }
	;



translation_unit
  : top_level_decl
        { $$ = node_statement_list(yylloc, NULL, $1); }
  | translation_unit top_level_decl
        { $$ = node_statement_list(yylloc, $1, $2); }
	;



top_level_decl
	: function_definition
				{ $$ = $1; }
	| declaration_statement T_SEMICOLON
				{ $$ = $1; }
	| error T_SEMICOLON
					{ $$ = node_null_statement($1, yylloc); yyerrok; }
	;
  /* ENTRY RULES END */



compound_statement 
	: declaration_or_statement_list
				{ $$ = node_compound_statement(yylloc, $1); }
	;



declaration_or_statement_list 
	: statement_list
				{ $$ = node_statement_list(yylloc, NULL, $1); }
	| declarations_grouped
				{ $$ = node_statement_list(yylloc, NULL, $1); }
	| declarations_grouped statement_list
				{ $$ = node_statement_list(yylloc, $1, $2); }
	;



declarations_grouped 
	: declaration_statement T_SEMICOLON
				{ $$ = node_statement_list(yylloc, NULL, $1); }
	| declarations_grouped declaration_statement T_SEMICOLON
				{ $$ = node_statement_list(yylloc, $1, $2); }	
	;



statement_list 
	: statement 
				{ $$ = node_statement_list(yylloc, NULL, $1); }
	| statement_list statement 
				{ $$ = node_statement_list(yylloc, $1, $2); }
	;



statement 
	: expression_statement T_SEMICOLON
				{ $$ = $1; }
	| T_LEFT_BRACE compound_statement T_RIGHT_BRACE
				{ $$ = $2; }
	| T_LEFT_BRACE T_RIGHT_BRACE
				{ $$ = node_compound_statement(yylloc, NULL); }
	| conditional_statement
	| while_statement
	| do_while_statement
	| for_statement
	| label_statement
	| go_to_statement
	| T_BREAK T_SEMICOLON
				{ $$ = node_break_statement(yylloc); }
	| T_CONTINUE T_SEMICOLON
				{ $$ = node_continue_statement(yylloc); }
	| T_RETURN expression_statement T_SEMICOLON
				{ $$ = node_return_statement(yylloc, $2); }
	| T_RETURN T_SEMICOLON
				{ $$ = node_return_statement(yylloc, NULL); }
	;



label_statement 
	: T_IDENTIFIER T_COLON statement 
				{ $$ = node_label_statement(yylloc, $1, $3); }
	;



go_to_statement 
	: T_GOTO T_IDENTIFIER T_SEMICOLON 
				{ $$ = node_goto_statement(yylloc, $2); }
	;



conditional_statement 
	: T_IF T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS statement %prec IF
				{ $$ = node_conditional_statement(yylloc, false, $3, $5, NULL); }
	| T_IF T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS statement T_ELSE statement 
				{ $$ = node_conditional_statement(yylloc, false, $3, $5, $7); }
	;



expression_statement 
	: expression 
				{ $$ = node_expression_statement(yylloc, $1); } // only created this node because its getting used in the next steps...
	;



expression
	: assignment_expression
				{ $$ = node_expression_list_statement(yylloc, $1, NULL); }
	| expression T_COMMA assignment_expression 
				{ $$ = node_expression_list_statement(yylloc, $1, $3); }
	;



assignment_expression 
	: conditional_expression
	| unary_expr T_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_ASSIGN, $1, $3); }
	| unary_expr T_PLUS_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_PLUS_EQUAL, $1, $3); }
	| unary_expr T_MINUS_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_MINUS_EQUAL, $1, $3); }
	| unary_expr T_MULTIPLY_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_MULTIPLY_EQUAL, $1, $3); }
	| unary_expr T_DIVIDE_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_DIVIDE_EQUAL, $1, $3); }
	| unary_expr T_MOD_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_MODULO_EQUAL, $1, $3); }
	| unary_expr T_LESS_LESS_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_LEFT_SHIFT_EQUAL, $1, $3); }
	| unary_expr T_GREATER_GREATER_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_RIGHT_SHIFT_EQUAL, $1, $3); }
	| unary_expr T_AND_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_BITWISE_AND_EQUAL, $1, $3); }
	| unary_expr T_CIRCUMFLEX_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_BITWISE_XOR_EQUAL, $1, $3); }
	| unary_expr T_VERTICAL_BAR_EQUAL assignment_expression
				{ $$ = node_binary_operation(yylloc, BINOP_BITWISE_OR_EQUAL, $1, $3); }
	;



  /* CHAIN OF CONDITIONAL EXPRESSIONS */
conditional_expression 
	: logical_or_expr
	| logical_or_expr T_QUESTION_MARK expression_statement T_COLON conditional_expression
				{ $$ = node_conditional_statement(yylloc, true, $1, $3, $5); }
	;



logical_or_expr 
	: logical_and_expr 
	| logical_or_expr T_OR_OR logical_and_expr
				{ $$ = node_binary_operation(yylloc, LOGICAL_OR, $1, $3); }
	;



logical_and_expr 
	: bitwise_or_expr
	| logical_and_expr T_AND_AND bitwise_or_expr
				{ $$ = node_binary_operation(yylloc, LOGICAL_AND, $1, $3); }
	;



bitwise_or_expr 
	: bitwise_xor_expr 
	| bitwise_or_expr T_VERTICAL_BAR bitwise_xor_expr 
				{ $$ = node_binary_operation(yylloc, BITWISE_OR, $1, $3); }
	;	



bitwise_xor_expr 
	: bitwise_and_expr 
	| bitwise_xor_expr T_CIRCUMFLEX bitwise_and_expr 
				{ $$ = node_binary_operation(yylloc, BITWISE_XOR, $1, $3); }
	;



bitwise_and_expr 
	: equality_expr  
	| bitwise_and_expr T_AMPERSAND equality_expr
				{ $$ = node_binary_operation(yylloc, BITWISE_AND, $1, $3); }
	;



equality_expr
	: relational_expr
	|	equality_expr T_EQUAL_EQUAL relational_expr 
				{ $$ = node_binary_operation(yylloc, EQUALITY_OP, $1, $3); }
	|	equality_expr T_NOT_EQUAL relational_expr 
				{ $$ = node_binary_operation(yylloc, INEQUALITY_OP, $1, $3); }
	;



relational_expr
	: shift_expr 
	|	relational_expr T_LESS_THAN shift_expr
				{ $$ = node_binary_operation(yylloc, LESS_THAN, $1, $3); }
	|	relational_expr T_LESS_EQUAL shift_expr
				{ $$ = node_binary_operation(yylloc, LESS_THAN_EQ, $1, $3); }
	|	relational_expr T_GREATER_THAN shift_expr
				{ $$ = node_binary_operation(yylloc, GREATER_THAN, $1, $3); }
	|	relational_expr T_GREATER_EQUAL shift_expr
				{ $$ = node_binary_operation(yylloc, GREATER_THAN_EQ, $1, $3); }
	;



shift_expr
	: additive_expr 
	| shift_expr T_LESS_LESS additive_expr
				{ $$ = node_binary_operation(yylloc, LEFT_SHIFT, $1, $3); }
	| shift_expr T_GREATER_GREATER additive_expr
				{ $$ = node_binary_operation(yylloc, RIGHT_SHIFT, $1, $3); }
	;	



additive_expr 
	: multiplicative_expr 
	|	additive_expr T_PLUS multiplicative_expr
				{ $$ = node_binary_operation(yylloc, BINOP_ADDITION, $1, $3); }
	|	additive_expr T_HYPHEN multiplicative_expr
				{ $$ = node_binary_operation(yylloc, BINOP_SUBTRACTION, $1, $3); }
	;



multiplicative_expr 
	: cast_expr 
	|	multiplicative_expr T_ASTERISK cast_expr
				{ $$ = node_binary_operation(yylloc, BINOP_MULTIPLICATION, $1, $3); }
	|	multiplicative_expr T_SLASH cast_expr
				{ $$ = node_binary_operation(yylloc, BINOP_DIVISION, $1, $3); }
	|	multiplicative_expr T_PERCENT cast_expr
				{ $$ = node_binary_operation(yylloc, BINOP_MOD, $1, $3); }
	;



cast_expr 
	: unary_expr
	| T_LEFT_PARENTHESIS declaration_type_pointer T_RIGHT_PARENTHESIS cast_expr
				{ $$ = node_cast_expression(yylloc, $2, $4); }
	;	



unary_expr 
	: postfix_expr
	| T_HYPHEN cast_expr
				{ $$ = node_unary_expression(yylloc, UNARY_HYPHEN, $2); }
	| T_PLUS cast_expr
				{ $$ = node_unary_expression(yylloc, UNARY_PLUS, $2); }
	| T_NOT cast_expr
				{ $$ = node_unary_expression(yylloc, UNARY_LOGICAL_NEGATION, $2); }
	| T_TILDE cast_expr
				{ $$ = node_unary_expression(yylloc, UNARY_BITWISE_NEGATION, $2); }
	| T_AMPERSAND cast_expr
				{ $$ = node_unary_expression(yylloc, UNARY_ADDRESS, $2); }
	| T_ASTERISK cast_expr
				{ $$ = node_unary_expression(yylloc, UNARY_POINTER, $2); }
	| T_PLUS_PLUS unary_expr
				{ $$ = node_unary_expression(yylloc, UNARY_PREINC, $2); }
	| T_MINUS_MINUS unary_expr
				{ $$ = node_unary_expression(yylloc, UNARY_PREDEC, $2); }
	;	



postfix_expr 
	: primary_expr
	| function_call
	| postfix_expr T_LEFT_BRACKET expression_statement T_RIGHT_BRACKET
				{ $$ = node_subscript_expr(yylloc, $1, $3); }
	| postfix_expr T_PLUS_PLUS
				{ $$ = node_unary_expression(yylloc, UNARY_POSTINC, $1); }
	| postfix_expr T_MINUS_MINUS
				{ $$ = node_unary_expression(yylloc, UNARY_POSTDEC, $1); }
	;	



primary_expr
	: T_IDENTIFIER 
	| T_NUMBER
	| T_STRING
	| T_LEFT_PARENTHESIS expression T_RIGHT_PARENTHESIS
					{ $$ = $2; }
	;	
  /* CHAIN OF CONDITIONAL EXPRESSIONS END */



  /* ITERATIVE STATEMENTS */
while_statement 
	: T_WHILE T_LEFT_PARENTHESIS expression_statement T_RIGHT_PARENTHESIS statement
				{ $$ = node_iterative_statement(yylloc, WHILE, $3, $5); }
	;



do_while_statement 
	: T_DO statement T_WHILE T_LEFT_PARENTHESIS expression_statement T_RIGHT_PARENTHESIS T_SEMICOLON
				{ $$ = node_iterative_statement(yylloc, DO_WHILE, $5, $2); }
	;



for_statement 
	: T_FOR for_expr statement
				{ $$ = node_iterative_statement(yylloc, FOR, $2, $3); }
	;



for_expr 
	: T_LEFT_PARENTHESIS expression_statement T_SEMICOLON expression_statement T_SEMICOLON expression_statement T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, $2, $4, $6); }
	| T_LEFT_PARENTHESIS T_SEMICOLON expression_statement T_SEMICOLON expression_statement T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, NULL, $3, $5); }
	| T_LEFT_PARENTHESIS T_SEMICOLON T_SEMICOLON expression_statement T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, NULL, NULL, $4); }
	| T_LEFT_PARENTHESIS T_SEMICOLON T_SEMICOLON T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, NULL, NULL, NULL); }
	| T_LEFT_PARENTHESIS expression_statement T_SEMICOLON T_SEMICOLON expression_statement T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, $2, NULL, $5); }
	| T_LEFT_PARENTHESIS expression_statement T_SEMICOLON T_SEMICOLON T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, $2, NULL, NULL); }
	| T_LEFT_PARENTHESIS expression_statement T_SEMICOLON expression_statement T_SEMICOLON T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, $2, $4, NULL); }
	| T_LEFT_PARENTHESIS T_SEMICOLON expression_statement T_SEMICOLON T_RIGHT_PARENTHESIS 
				{ $$ = node_for_expression(yylloc, NULL, $3, NULL); }
	;
	/* ITERATIVE STATEMENTS END */



	/* FUNCTION RULES */
function_definition
	: declaration_type function_declaration T_LEFT_BRACE compound_statement T_RIGHT_BRACE 
				{ $$ = node_function_definition(yylloc, $1, $2, $4, false); }
	| declaration_type T_ASTERISK function_declaration T_LEFT_BRACE compound_statement T_RIGHT_BRACE 
				{ $$ = node_function_definition(yylloc, $1, $3, $5, true); }
	| declaration_type function_declaration T_LEFT_BRACE T_RIGHT_BRACE
				{ $$ = node_function_definition(yylloc, $1, $2, NULL, false); }
	| declaration_type T_ASTERISK function_declaration T_LEFT_BRACE T_RIGHT_BRACE
				{ $$ = node_function_definition(yylloc, $1, $3, NULL, true); }
	| declaration_type function_declaration T_LEFT_BRACE void_keyword T_SEMICOLON T_RIGHT_BRACE
				{ $$ = node_function_definition(yylloc, $1, $2, $4, false); }
	| declaration_type T_ASTERISK function_declaration T_LEFT_BRACE void_keyword T_SEMICOLON T_RIGHT_BRACE
				{ $$ = node_function_definition(yylloc, $1, $3, $5, true); }
	;
	/* FUNCTION RULES END */



  /* DECLARATION RULES */
declaration_statement
	: declaration_type declaration_list
				{ $$ = node_declaration_statement(yylloc, $1, $2); }
	;



/* Handles declaration type pointers */
declaration_type_pointer
	: declaration_type_pointer T_ASTERISK
				{ $$ = node_pointer(yylloc, $1); }
	| declaration_type
	;



declaration_type
	: signed_type_specifier
	| unsigned_type_specifier
	| character_type_specifier
	| void_keyword
	;



signed_type_specifier
	: T_SHORT 
				{ $$ = node_type(yylloc, TYPE_BASIC_SHORT, false); }
	| T_SHORT T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_SHORT, false); }
	| T_SIGNED T_SHORT 
				{ $$ = node_type(yylloc, TYPE_BASIC_SHORT, false); }
	| T_SIGNED T_SHORT T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_SHORT, false); }
	| T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_INT, false); }
	| T_SIGNED T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_INT, false); }
	| T_SIGNED 
				{ $$ = node_type(yylloc, TYPE_BASIC_INT, false); }
	| T_LONG 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, false); }
	| T_LONG T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, false); }
	| T_SIGNED T_LONG 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, false); }
	| T_SIGNED T_LONG T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, false); }
	;



unsigned_type_specifier 
	: T_UNSIGNED T_SHORT T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_SHORT, true); }
	| T_UNSIGNED T_SHORT 
				{ $$ = node_type(yylloc, TYPE_BASIC_SHORT, true); }
	| T_UNSIGNED T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_INT, true); }
	| T_UNSIGNED 
				{ $$ = node_type(yylloc, TYPE_BASIC_INT, true); }
	| T_UNSIGNED T_LONG 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, true); }
	| T_UNSIGNED T_LONG T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, true); }
	| T_UNSIGNED T_LONG T_LONG T_INT 
				{ $$ = node_type(yylloc, TYPE_BASIC_LONG, true); }
	;



character_type_specifier  
	: T_CHAR 
				{ $$ = node_type(yylloc, TYPE_BASIC_CHAR, false); }
	| T_SIGNED T_CHAR
				{ $$ = node_type(yylloc, TYPE_BASIC_CHAR, false); }
	| T_UNSIGNED T_CHAR
				{ $$ = node_type(yylloc, TYPE_BASIC_CHAR, true); }
	;



void_keyword 
	: T_VOID 
				{ $$ = node_type(yylloc, TYPE_BASIC_VOID, false); }
	;



/* Handles commas */
declaration_list
	: declaration
				{ $$ = node_declaration_list_statement(yylloc, $1, NULL); }
	| declaration_list T_COMMA declaration
				{ $$ = node_declaration_list_statement(yylloc, $1, $3); }
	;



/* Handles pointers */
declaration
	: T_ASTERISK declaration
				{ $$ = node_pointer(yylloc, $2); }
	| declaration_options_wrapped
	;



/* Handles parenthesis */
declaration_options_wrapped
	: direct_declarator
	| T_LEFT_PARENTHESIS declaration T_RIGHT_PARENTHESIS
				{ $$ = $2; }
	;



direct_declarator
	: simple_declaration 
	|	array_declaration
	| function_declaration
	;



simple_declaration
	: T_IDENTIFIER
	;



array_declaration
	: declaration_options_wrapped T_LEFT_BRACKET conditional_expression T_RIGHT_BRACKET 
				{ $$ = node_array(yylloc, $1, $3); }
	;



function_call 
	: postfix_expr T_LEFT_PARENTHESIS expression_statement T_RIGHT_PARENTHESIS
				{ $$ = node_function(yylloc, $1, $3, false); }
	| postfix_expr T_LEFT_PARENTHESIS void_keyword T_RIGHT_PARENTHESIS
				{ $$ = node_function(yylloc, $1, $3, false); }
	| postfix_expr T_LEFT_PARENTHESIS T_RIGHT_PARENTHESIS
				{ $$ = node_function(yylloc, $1, NULL, false); }
	;



function_declaration
	: declaration_options_wrapped T_LEFT_PARENTHESIS arguments_list T_RIGHT_PARENTHESIS
				{ $$ = node_function(yylloc, $1, $3, true); }
	| declaration_options_wrapped T_LEFT_PARENTHESIS T_RIGHT_PARENTHESIS
				{ $$ = node_function(yylloc, $1, NULL, true); }
	;



arguments_list
	: single_argument
				{ $$ = node_arguments_list(yylloc, $1, NULL); }
	| arguments_list T_COMMA single_argument
				{ $$ = node_arguments_list(yylloc, $1, $3); }
	;



single_argument
	: declaration_type argument
				{ $$ = node_declaration_statement(yylloc, $1, $2); }
	| declaration_type
				{ $$ = node_declaration_statement(yylloc, $1, NULL); }
	| declaration_type T_ASTERISK
				{ $$ = node_pointer_argument(yylloc, $1); }
	;



/* Handles pointers */
argument
	: T_ASTERISK argument
				{ $$ = node_pointer(yylloc, $2); }
	| argument_options_wrapped
	;



// /* Handles parenthesis */
argument_options_wrapped
	: argument_options
	| T_LEFT_PARENTHESIS argument T_RIGHT_PARENTHESIS
			{ $$ = $2; }
	;



array_argument_declaration
	: argument_options_wrapped T_LEFT_BRACKET conditional_expression T_RIGHT_BRACKET
			{ $$ = node_array(yylloc, $1, $3); }
	| argument_options_wrapped T_LEFT_BRACKET T_RIGHT_BRACKET
			{ $$ = node_array(yylloc, $1, NULL); }
	;



argument_options
	: simple_declaration 
	|	array_argument_declaration
	;



  /* DECLARATION RULES END */
%%

static void yyerror(YYLTYPE *loc,
										YYSTYPE *root __attribute__((unused)),
										int *error_count,
										yyscan_t scanner __attribute__((unused)),
										char const *s)
{
	compiler_print_error(*loc, s);
	(*error_count)++;
}

struct node *parser_create_tree(int *error_count, yyscan_t scanner) {
	struct node *parse_tree;
	int result = yyparse(&parse_tree, error_count, scanner);
	if (result == 1 || *error_count > 0) {
		return NULL;
	} else if (result == 2) {
		fprintf(stdout, "Parser ran out of memory.\n");
		return NULL;
	} else {
		return parse_tree;
	}
}

char const *parser_token_name(int token) {
	return yytname[token - 255];
}

