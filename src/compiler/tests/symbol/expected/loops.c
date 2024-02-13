
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: az  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: i  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: one_for  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 4 

symbol table (BLOCK_SCOPE) ||  parent (BLOCK_SCOPE):
  variable: xyz  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 5 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: two_while  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 6 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: three_do  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 

================= SYMBOLS FORMATTED ================
main2 -> function(signed int, [(void), ])
az -> pointer((signed char))
i -> (signed int)
a -> (signed int)
one_for -> (signed int)
xyz -> (signed int)
two_while -> (signed int)
three_do -> pointer((signed char))

=============== PARSE TREE ===============
signed char (* az);signed int main2(void ){signed int a;signed int i;for ((i = 0);(i < 10);( i++)){signed int one_for;( i++);{signed int xyz;}}while (a) {signed int two_while;break;}return 0;do {signed char (* three_do);continue;}while (a);}

