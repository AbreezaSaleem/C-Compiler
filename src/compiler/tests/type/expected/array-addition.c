
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: two  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 
  variable: three  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: one  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
two -> array(56, (signed char))
three -> array(56, (signed int))
one -> array(56, (signed int))
Error (5, 77) to (5, 77): Incompatible operation between (pointer/array) with (pointer/array)
Error (7, 93) to (7, 93): Incompatible operation between (pointer/array) with (pointer/array)
Type checking encountered 2 errors.
