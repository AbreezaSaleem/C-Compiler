
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 3 
  variable: func  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: two  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: one  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: two  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 
  variable: one  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
func -> function(signed int, [array(56, (signed char), array(34, (signed int), ))])
two -> array(34, (signed int))
one -> array(56, (signed char))
two -> array(4, (signed int))
one -> array(56, (signed int))
Error (9, 108) to (9, 110): Incompatible type conversion
Type checking encountered 1 error.
