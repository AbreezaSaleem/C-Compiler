
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: b  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(signed int), pointer((signed char), )])
b -> pointer((signed char))
a -> (signed int)
Error (1, 1) to (1, 1): Invalid arguments of 'main' function
Type checking encountered 1 error.
