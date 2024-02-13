
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: arr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 
  variable: c  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: b  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: p  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
arr -> array(45, (signed int))
c -> (signed int)
b -> pointer((signed char))
p -> pointer((signed int))
a -> pointer((signed int))
Error (8, 99) to (8, 99): Incompatible type conversion between arithmetic and pointer/array
Error (9, 110) to (9, 110): Incompatible type conversion between arithmetic and pointer/array
Error (10, 124) to (10, 124): Expression must be a modifiable lvalue
Error (12, 147) to (12, 147): Incompatible type conversion
Error (14, 170) to (14, 170): Incompatible operation between (pointer/array) with (pointer/array)
Type checking encountered 5 errors.
