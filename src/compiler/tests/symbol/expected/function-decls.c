
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: x  ||  type: pointer(signed int)  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 13 
  variable: f  ||  type: pointer(signed char)  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 12 
  variable: func4  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 6 
  variable: fun2  ||  type: signed short int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 5 
  variable: fun1  ||  type: signed short int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: array  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 9 
  variable: b  ||  type: signed long int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 8 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: q  ||  type: signed long int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 11 
  variable: p  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 10 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

================= SYMBOLS FORMATTED ================
x -> function(pointer(signed int), [(void), ])
f -> function(pointer(signed char), [(void), ])
func4 -> function(signed int, [(void), ])
fun2 -> function(signed short int, [(signed int), (signed long int), ])
fun1 -> function(signed short int, [(signed int), (signed long int), array(45, array(89, (signed int), ))])
array -> array(5, pointer(function(signed int, [(void), ])))
a -> array(45, array(89, (signed int)))
b -> (signed long int)
a -> (signed int)
q -> (signed long int)
p -> (signed int)

=============== PARSE TREE ===============
signed int (* (array[5]))(void );signed short int fun1(signed int a,signed long int b,signed int ((a[45])[89]));signed short int fun2(signed int ,signed long int );signed int func4(void );signed short int fun1(signed int a,signed long int b,signed int ((a[45])[89])){return 0;}signed short int fun2(signed int p,signed long int q){return 0;}signed int func4(void ){return 0;}signed char (* f(void ));signed char *f(void ){return 0;}signed int (* x(void ));signed int *x(void ){return 0;}

