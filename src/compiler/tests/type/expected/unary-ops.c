
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 4 
  variable: func  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 3 
  variable: func2  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: b  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: arr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 6 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
func -> function(signed int, [(void), ])
func2 -> function(signed int, [(signed int), (signed int), ])
b -> (signed int)
a -> (signed int)
arr -> array(56, (signed int))
a -> (signed int)
Error (17, 157) to (17, 161): Unary plus/minus cannot be applied to function
Error (18, 167) to (18, 169): Unary plus/minus cannot be applied to array
Error (19, 175) to (19, 177): Logical negation cannot be applied to array
Error (21, 190) to (21, 190): Incompatible operation between (pointer/array) with (pointer/array)
Error (21, 190) to (21, 190): Binary operation type not assigned
Error (24, 198) to (24, 201): Logical negation cannot be applied to function
Type checking encountered 6 errors.
