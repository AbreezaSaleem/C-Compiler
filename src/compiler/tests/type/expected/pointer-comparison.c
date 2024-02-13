
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: q  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: p  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: b  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
q -> pointer((signed char))
p -> pointer((signed int))
b -> pointer((signed short int))
a -> pointer((signed short int))
Error (8, 69) to (8, 69): Incompatible type conversion
Type checking encountered 1 error.
