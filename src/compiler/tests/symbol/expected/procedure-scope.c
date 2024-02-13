
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: second  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 5 
  variable: first  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 4 
  variable: fun  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: d  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 
  variable: b  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: y  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 
  variable: x  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 6 

================= SYMBOLS FORMATTED ================
second -> function(signed int, [(signed char), ])
first -> (signed int)
fun -> function(signed int, [(signed int), (signed int), ])
d -> (signed int)
b -> (signed int)
a -> (signed int)
y -> (signed int)
x -> (signed char)

=============== PARSE TREE ===============
signed int fun(signed int a,signed int b){signed int d;}signed int first;signed int fun(signed int a,signed int b);signed int second(signed char x){signed int y;fun(2,4);}

