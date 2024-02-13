
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: s  ||  type: signed long int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(signed char), (signed int), (signed long int), ])
s -> (signed long int)
a -> (signed int)
a -> (signed char)
Error (1, 1) to (1, 1): Invalid arguments of 'main' function
Type checking encountered 1 error.
