
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: p  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
p -> pointer((signed char))
a -> array(10, (signed int))
Error (4, 55) to (4, 55): Incompatible type conversion
Error (6, 65) to (6, 65): Incompatible type conversion between arithmetic and pointer/array
Type checking encountered 2 errors.
