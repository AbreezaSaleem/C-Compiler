
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: x  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 
  variable: q  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: p  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: b  ||  type: signed short int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed short int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
x -> (signed int)
q -> pointer((signed int))
p -> pointer((signed int))
b -> pointer((signed short int))
a -> pointer((signed short int))
Error (9, 80) to (9, 80): Incompatible type conversion between arithmetic and pointer/array
Error (10, 89) to (10, 89): Incompatible type conversion between arithmetic and pointer/array
Type checking encountered 2 errors.
