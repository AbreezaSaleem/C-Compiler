# Grammar for the CSCI E-95 (Formerly CSCI E-295) C Programming Language Subset
#
# Changes on 13-Feb-2018
#   Changed comments saying something like "not in PS2" to "not in our subset
#   C Language"
#
# Changes on 31-Jan-2018
#   Added missing "or" after "percent_equals_t" in production assignment_op
#
# Changes on 22-Sep-2016
#   Changed production for function_def_specifier from
#     function_def_specifier = ( declaration_specifiers_opt and declarator
#     # and declaration_list_opt
#     )
#   to
#     function_def_specifier = ( declaration_specifiers and declarator
#     # and declaration_list_opt
#     )
#   because a return type must be specified in all function definitions
#
# Changes on 26-Feb-2015
#   Changed constant_t to constant in primary_expr
#
# Changes on 5-Nov-2013
#   Renamed left_paran_t to left_paren_t
#   Renamed right_paran_t to right_paren_t
#   Renamed paranthesized_expr to parenthesized_expr
#
# Changes on 6-Mar-2012
#   Renamed pointer_decl to pointer_declarator
#   Renamed a reference to expression to expr in expression_statement
#
# Changes on 23-Feb-2012
#   Removed c_char_sequence from the grammar because we do not allow
#     multicharacter constants
#   Removed last alternative from direct_abstract_declarator
#     ( direct_abstract_declarator_opt and left_paran_t and
#     parameter_type_list_opt and right_paren_t ) because we do not allow
#     function pointers
#
# Changes on 20-Feb-2012
#   Removed declaration_list because it is no longer referenced because we
#     do not allow pre-Standard, so-called traditional or non-prototyped,
#     function definitions
#   Removed the decl alternative from an initial_clause referenced by the
#     for_expr because we do not allow the C99 form that allows declaration
#     of variables in the initial expression of a for statement
#
# Changes on 14-Feb-2012
#   Enhanced the html version of the grammar as follows:
#     Terminal grammar symbols are now rendered in red
#     Optional non-terminal symbols are now linked to the symbol
#     Multiple references to a symbol in a single production are now handled
#       correctly
#     Correctly maintain multiple spaces from the txt file
#
# Changes on 13-Feb-2012
#   Changed abstract_decl to be abstract_declarator consistently
#   Changed direct_abstract_decl to be direct_abstract_declarator consistently
#   Added missing and's in declaration_or_statement_list,
#     direct_abstract_declarator, function_call, indirection_expr,
#     parameter_decl, pointer, pointer_decl, relational_expr, shift_expr
#
# Changes on 4-Feb-2012
#   Changed constant_expr to be constant_expr_opt between left_bracket_t
#   and right_bracket_t in direct_abstract_declarator
#
# Changes on 27-Sep-2010
#   Removed declaration_list_opt clause from function_def_specifier
#   Added comment identifying the start symbol
#
# Changes on 27-Feb-2009
#   Removed identifier and identifier_list
#   Removed sign_part
#   Removed declaration_specifiers_opt clause from declaration_specifiers
#
# Naming convention:
# decl -> declaration
# expr -> expression
# _opt -> optional
# _t -> means a terminal
# pound sign (#) means comment
#
# "and" and "or" symbols have been inserted explicitly into the grammar.
# For example,
# non_terminal_1 = ( non_term_2 and non_term_3 ) means we must see first
#   non_term_2 then non_term_3 to satisfy non_terminal_1
# non_terminal_1 = ( non_term_2 or non_term_3 ) means either non_term_2 or
#   non_term_3 must be seen to satisfy non_terminal_1
#
# Parentheses are used to denote groupings, such as:
# non_terminal_1 = ( term_1 and ( term_2 or term_3 ) and term_4 )
# which means term_1 must be followed by either term_2 or term_3 and then
#   by term_4 to satisfy non_terminal_1
#
############################################################################
# NOTE: There may be mistakes in typing or even in the grammar specified.
#       Please use this as a staring point and compare it against the
#       grammar specified in the book.
############################################################################
#
# The start symbol is translation_unit
#

abstract_declarator = ( pointer ) or
                ( pointer_opt and direct_abstract_declarator)

additive_expr = ( multiplicative_expr ) or
                ( additive_expr and add_op and multiplicative_expr )

add_op = ( plus_t ) or
         ( minus_t )

address_expr = ( ampersand_t and cast_expr ) 

array_declarator = ( direct_declarator and left_bracket_t and constant_expr_opt and right_bracket_t )
#            ( direct_declarator and left_bracket_t and array_size_expr_opt and right_bracket_t ) or
#            ( direct_declarator and left_bracket_t and asterisk_t and right_bracket_t )

# array_qualifier = ( static ) or ( restrict ) or ( const ) or ( volatile )
# array_qualifier_list = ( array_qualifier ) or ( array_qualifier_list and array_qualifier )

# array_size_expr = ( assignment_expr ) or
#                 ( asterisk_t )


assignment_expr = ( conditional_expr ) or
                  ( unary_expr and assignment_op and assignment_expr )

assignment_op = ( equals_t ) or
                ( plus_equals_t ) or
                ( minus_equals_t ) or
                ( asterisk_equals_t ) or
                ( slash_equals_t ) or
                ( percent_equals_t ) or
                ( left_shift_equals_t ) or
                ( right_shift_equals_t ) or
                ( ampersand_equals_t ) or
                ( xor_equals_t ) or
                ( or_equals_t )

# binary_exponent: no floating point support for our subset C Language

# bit_field = ( declarator_opt colon_t width )

bitwise_and_expr = ( equality_expr ) or
                   ( bitwise_and_expr and ampersand_t and equality_expr )


bitwise_negation_expr = ( tilde_t and cast_expr )

bitwise_or_expr = ( bitwise_xor_expr ) or
                  ( bitwise_or_expr and or_t and bitwise_xor_expr )

bitwise_xor_expr = ( bitwise_and_expr ) or
                   ( bitwise_xor_expr and xor_t and bitwise_and_expr )

break_statement = ( break_t and semicolon_t )

#case_label: not in our subset C Language

cast_expr = ( unary_expr ) or
            ( left_paren_t and type_name and right_paren_t and cast_expr )

# c-char # see grammar
# c_char_sequence # see grammar

character_constant # see associated grammar

character_escape_code # see associated grammar

character_type_specifier = ( char_t ) or
                           ( signed_t and char_t ) or
                           ( unsigned_t and char_t )

comma_expr = ( assignment_expr ) or
             ( comma_expr and comma_t and assignment_expr )

#complex_type_specifier: not in our subset C Language

# component_decl = ( type_specifier and component_declarator_list )

# component_declarator = ( simple_component ) or
#                        ( bit_field )

# component_declarator_list = ( component_declarator ) or
#                             ( component_declarator_list and comma_t and component_declarator )

# component_selection_expr = ( direct_component_selection ) or
#                            ( indirect_component_selection )

# compound_literal = ( left_paren_t and type_name and right_paren_t and left_brace_t and initializer_list and comma_t_opt right_brace_t )

compound_statement = ( left_brace_t and declaration_or_statement_list_opt and right_brace_t )

conditional_expr = ( logical_or_expr ) or
                   ( logical_or_expr and question_mark_t and expr and colon_t and conditional_expr )

conditional_statement = ( if_statement ) or
                        ( if_else_statement )

constant = ( integer_constant ) or
           # ( floating_constant ) not a part of our subset C Language
           ( character_constant ) or
           ( string_constant )

constant_expr = ( conditional_expr )

continue_statement = ( continue_t and semicolon_t )

# decimal_constant = ( nonzero_digit ) or
#                    ( decimal_constant and digit )

# decimal_floating_constant: not a part of our subset C Language

decl = ( declaration_specifiers and initialized_declarator_list and semicolon_t )

# declaration_list = ( decl ) or
#                    ( declaration_list and decl )

declaration_or_statement = ( decl ) or
                           ( statement )

declaration_or_statement_list = ( declaration_or_statement ) or
                                ( declaration_or_statement_list and declaration_or_statement )

declaration_specifiers =  ( type_specifier )
# declaration_specifiers =  ( type_specifier and declaration_specifiers_opt )
#                           ( storage_class_specifier and declaration_specifiers_opt ) or
#                           ( type_qualifier and declaration_specifiers_opt ) or
#                           ( function_specifier and declaration_specifiers_opt )

declarator = ( pointer_declarator ) or
             ( direct_declarator )

#default_label: not a part of our subset C Language grammar

# designation = ( designator_list and equals_t )

# designator = ( left_bracket_t and constant_expr and right_bracket_t )
             # (  dot_t and identifier ) not a part of our subset C Language

# designator_list = ( designator ) or
#                   ( designator_list and designator )

digit # see associated grammar

digit_sequence # see associated grammar

direct_abstract_declarator = ( left_paren_t and abstract_declarator and right_paren_t ) or
                             ( direct_abstract_declarator_opt and left_bracket_t and constant_expr_opt and right_bracket_t )
#                             ( direct_abstract_declarator_opt left_bracket_t and constant_expr and right_bracket_t )
#                            ( direct_abstract_declarator_opt and left_bracket_t and expr and right_bracket_t )
#                            ( direct_abstract_declarator_opt and left_bracket_t and asterisk_t right_bracket_t )
#                             ( direct_abstract_declarator_opt and left_paren_t and parameter_type_list_opt and right_paren_t )

# direct_component_selection = ( postfix_expr and dot_t and identifier )
# dot not a part of our subset C Language

direct_declarator = ( simple_declarator ) or
                    ( left_paren_t and declarator and right_paren_t ) or
                    ( function_declarator ) or
                    ( array_declarator )

do_statement = ( do_t and statement and while_t and left_paren_t and expr and right_paren_t and semicolon_t )

# dotted_digits floating point not a part of our subset C Language
# dotted_hex_digits floating point not a part of our subset C Language

# enum not a part of our subset C Language
# enumeration_constant
# enumeration_constant_definition
# enumeration_definition_list
# enumeration_tag
# enumeration_type_definition
# enumeration_type_reference
# enumeration_type_specifier

equality_expr = ( relational_expr ) or
                ( equality_expr and equality_op and relational_expr )

equality_op = ( equals_equals_t ) or
              ( not_equals_t )

escape_char # see associated grammar

escape_code # see associated grammar

# exponent: not a part of our subset C Language grammar

expr = ( comma_expr )

expression_list = ( assignment_expr ) or
                  ( expression_list and comma_t and assignment_expr )

expression_statement = ( expr and semicolon_t )

# field_list = ( component_decl ) or
#              ( field_list and component_decl )

# no floating point support.
# floating_constant: not a part of our subset C Language
# floating_point_type_specifier
# floating_suffix

for_expr = ( left_paren_t and initial_clause_opt and semicolon_t and expr_opt and semicolon_t and expr_opt and right_paren_t )

for_statement = ( for_t and for_expr and statement )

function_call = ( postfix_expr and left_paren_t and expression_list_opt and right_paren_t )

function_declarator = ( direct_declarator and left_paren_t and parameter_type_list and right_paren_t )
#                     ( direct_declarator and left_paren_t and identifier_list_opt and right_paren_t )

function_definition = ( function_def_specifier and compound_statement )

function_def_specifier = ( declaration_specifiers and declarator
# and declaration_list_opt
)

# function specifier: not a part of our subset C Language

goto_statement = ( goto_t and named_label and semicolon_t )

# h_char_statement # see associated grammar
# hexadecimal_constant # see associated grammar
# hexadecimal_floating_constant: not a part of our subset C Language
# all hex: see associated grammar

# identifier # see associated grammar

# identifier_list = ( identifier ) or
#                   ( parameter_list and comma_t and identifier_t )

if_else_statement = ( if_t and left_paren_t and expr and right_paren_t and statement and else_t and statement )

if_statement = ( if_t and left_paren_t and expr and right_paren_t and statement )

# indirect_component_selection # see associated grammar 
# indirect_expr # see grammar

indirection_expr = asterisk_t and cast_expr

initial_clause = ( expr )
#                or ( decl )

initialized_declarator = ( declarator )
#                        ( declarator and equals_t and initializer )

initialized_declarator_list = ( initialized_declarator ) or
                              ( initialized_declarator_list and comma_t and initialized_declarator )

# initializer = ( assignment_expr ) or
#              ( left_brace_t and initializer_list and comma_t_opt right_brace_t )

# initializer_list = ( initializer ) or
#                    ( initializer_list and comma_t and initializer ) or
#                    ( designation and initializer )
#                    ( initializer_list and comma_t and designation and initializer )

integer_constant # see associated grammar

# integer_suffix - not in our subset C Language

integer_type_specifier = ( signed_type_specifier ) or
                         ( unsigned_type_specifier ) or
                         ( character_type_specifier )
                         # ( bool_type_specifier ) not a part of our subset C Language

iterative_statement = ( while_statement ) or
                      ( do_statement ) or
                      ( for_statement )

label = ( named_label )
        # ( case_label ) not part of our subset C Language
        # ( default_label ) not part of our subset C Language

labeled_statement = ( label and colon_t and statement )

logical_and_expr = ( bitwise_or_expr ) or
                   ( logical_and_expr and logical_and_t and bitwise_or_expr )

logical_negation_expr = ( not_t and cast_expr )

logical_or_expr = ( logical_and_expr ) or
                  ( logical_or_expr and logical_or_t and logical_and_expr )

# long_long_suffix not part of our subset C Language
# long_suffix

multiplicative_expr = ( cast_expr ) or
                      ( multiplicative_expr and mult_op and cast_expr )

mult_op = ( asterisk_t ) or
          ( slash_t ) or
          ( percent_t )

named_label = ( identifier_t )

nondigit # see grammar

nonzero_digit # see grammar

null_statement = ( semicolon_t )

# octal* see grammar
# on-off switch not a part of our subset C Language

parameter_decl = ( declaration_specifiers and declarator ) or
                 ( declaration_specifiers and abstract_declarator_opt )

parameter_list = ( parameter_decl ) or
                 ( parameter_list and comma_t and parameter_decl )

parameter_type_list = ( parameter_list )
                      #( parameter_list and comma_t and dot_t dot_t dot_t ) not in our subset C Language

parenthesized_expr = ( left_paren_t and expr and right_paren_t )

pointer = ( asterisk_t ) or
          ( asterisk_t and pointer )

pointer_declarator = ( pointer and direct_declarator )

postdecrement_expr = ( postfix_expr and minus_minus_t )

postfix_expr = ( primary_expr ) or
               ( subscript_expr ) or
#              ( component_selection_expr ) or
               ( function_call ) or
               ( postincrement_expr ) or
               ( postdecrement_expr )
#              ( compound_literal )

postincrement_expr = ( postfix_expr and plus_plus_t )

predecrement_expr = ( minus_minus_t and unary_expr )

preincrement_expr = ( plus_plus_t and unary_expr )

# preprocessor_tokens not part of our subset C Language

primary_expr = ( identifier_t ) or
               ( constant ) or
               ( parenthesized_expr )

# q_char_sequence # see grammar

relational_expr = ( shift_expr ) or
                  ( relational_expr and relational_op and shift_expr )

relational_op = ( less_than_t ) or
                ( less_than_equals_t ) or
                ( greater_than_t ) or
                ( greater_than_equals_t )

return_statement = ( return_t and expr_opt and semicolon_t )

# s_char # see grammar
# s_char_sequence # see grammar


# This symbol was missing in original grammar
shift_expr = ( additive_expr ) or 
             ( shift_expr and shift_op and additive_expr )

shift_op = ( left_shift_t ) or
           ( right_shift_t )

signed_type_specifier = ( short_t ) or
                        ( short_t and int_t ) or
                        ( signed_t and short_t ) or
                        ( signed_t and short_t and int_t ) or
                        ( int_t ) or
                        ( signed_t and int_t ) or
                        ( signed_t ) or
                        ( long_t ) or
                        ( long_t and int_t ) or
                        ( signed_t and long_t ) or
                        ( signed_t and long_t and int_t )
#                       ( long_t and long_t ) or
#                       ( long_t and long_t and int_t ) or
#                       ( signed_t and long_t and long_t ) or
#                       ( signed_t and long_t and long_t and int_t )

# sign_part = ( plus_t ) or
#             ( minus_t )

# This should really  be "simple_component" but it is out of scope either way
# single_component = ( declarator )

simple_declarator = ( identifier_t )

#sizeof_expr

statement = ( expression_statement ) or
            ( labeled_statement ) or
            ( compound_statement ) or
            ( conditional_statement ) or
            ( iterative_statement ) or
            # ( switch_statement ) or
            ( break_statement ) or
            ( continue_statement ) or
            ( return_statement ) or
            ( goto_statement ) or
            ( null_statement )

# storage_class_specifier = ( auto_t ) or
#                           ( extern_t ) or
#                           ( register_t ) or
#                           ( static_t) or
#                           ( typedef_t ) or

string_constant # see grammar

subscript_expr = ( postfix_expr and left_bracket_t and expr and right_bracket_t )

top_level_decl = ( decl ) or
                 ( function_definition )

translation_unit = ( top_level_decl ) or
                   ( translation_unit and top_level_decl )

#typedef
type_name = ( declaration_specifiers and abstract_declarator_opt )

# type_qualifier not a part of our subset C Language

# type_qualifier_list = ( type_qualifier ) or
#                       ( type_qualifier_list and type_qualifier )

type_specifier = #( enumeration_type_specifier ) or
                 # ( floating_point_type_specifier ) or
                 ( integer_type_specifier ) or
                 # ( structure_type_specifier )
                 # typedef
                 # union
                 ( void_type_specifier )

unary_expr = ( postfix_expr ) or
#            ( sizeof_expr ) or
             ( unary_minus_expr ) or
             ( unary_plus_expr ) or
             ( logical_negation_expr ) or
             ( bitwise_negation_expr ) or
             ( address_expr ) or
             ( indirection_expr ) or
             ( preincrement_expr ) or
             ( predecrement_expr )

unary_minus_expr = ( minus_t and cast_expr )

unary_plus_expr = (plus_t and cast_expr )

# union*

unsigned_type_specifier = ( unsigned_t and short_t and int_t_opt ) or
                          ( unsigned_t and int_t_opt ) or
                          ( unsigned_t and long_t and int_t_opt )
#                         ( unsigned_t and long_t and long_t and int_t_opt )

void_type_specifier = ( void_t )

while_statement = ( while_t and left_paren_t and expr and right_paren_t and statement )

# width = ( constant_expr )
