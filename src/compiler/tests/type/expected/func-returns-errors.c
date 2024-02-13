
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: func2  ||  type: pointer(signed int)  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 3 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 2 
  variable: main2  ||  type: void  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: fun  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

================= SYMBOLS FORMATTED ================
func2 -> function(pointer(signed int), [(void), ])
main -> function(signed int, [])
main2 -> function(void, [(void), ])
fun -> function(signed int, [(void), ])
Error (2, 26) to (2, 26): Non-void function should return a value
Error (6, 60) to (6, 60): Incompatible type conversion
Error (14, 123) to (14, 123): Incompatible type conversion between arithmetic and pointer/array
Type checking encountered 3 errors.
