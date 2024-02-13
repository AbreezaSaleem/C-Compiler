
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: a  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
a -> array(45, (signed char))
Error (4, 40) to (4, 40): Expression must be a modifiable lvalue
Error (5, 54) to (5, 54): Incompatible type conversion between arithmetic and pointer/array
Type checking encountered 2 errors.
