
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: f  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 7 
  variable: s  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 6 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 5 
  variable: p  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 4 
  variable: array  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 
  variable: arr2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: arr  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 
  variable: b  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: p  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 11 
  variable: i  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 10 

================= SYMBOLS FORMATTED ================
f -> function(signed char, [(signed int), pointer((signed int), )])
s -> array(10, pointer(array(50, (signed int))))
a -> array(30, array(20, pointer((signed int))))
p -> pointer((signed char))
array -> array(5, pointer(function(signed int, [(void), ])))
arr2 -> array(4, (signed int))
arr -> array(b, (signed int))
b -> (signed int)
p -> pointer((signed int))
i -> (signed int)

=============== PARSE TREE ===============
signed int b;signed int (arr[b]);signed int (arr2[4]);signed int (* (array[5]))(void );signed char (* p);signed int (* ((a[30])[20]));signed int ((*(s[10]))[50]);signed char f(signed int i,signed int (* p));signed char f(signed int i,signed int (* p)){return 0;}

