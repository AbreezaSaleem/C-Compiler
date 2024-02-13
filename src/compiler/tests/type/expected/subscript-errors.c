
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main3  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 4 
  variable: func  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 3 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: arr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: arr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 6 
  variable: brr  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 

================= SYMBOLS FORMATTED ================
main3 -> function(signed int, [(void), ])
func -> function(signed int, [(void), ])
main -> function(signed int, [(void), ])
arr -> array(45, (signed int))
a -> (signed int)
arr -> array(90, (signed int))
brr -> array(45, (signed char))
Error (5, 48) to (5, 48): Subscript operator requires array operand
Error (9, 75) to (9, 75): Lvalue required
Error (22, 184) to (22, 184): Incompatible operation between (pointer/array) with (pointer/array)
Error (22, 184) to (22, 184): Binary operation type not assigned
Error (22, 184) to (22, 184): Unary operation type not assigned
Error (22, 191) to (22, 191): Expression must be a modifiable lvalue
Error (24, 216) to (24, 219): Subscript operator cannot be applied to function
Type checking encountered 7 errors.
