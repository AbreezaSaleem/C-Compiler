
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 3 
  variable: func2  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 1 
  variable: func  ||  type: void  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
func2 -> function(signed int, [(signed int), ])
func -> function(void, [(void), ])
a -> (signed int)
Error (2, 29) to (2, 29): Incompatible type conversion
Error (10, 92) to (10, 93): Incompatible type conversion
Error (12, 120) to (12, 120): Incompatible type conversion
Error (13, 132) to (13, 132): Function arguments too many/too few
Type checking encountered 4 errors.
