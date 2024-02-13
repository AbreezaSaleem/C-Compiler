
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 2 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: si  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: b  ||  type: signed long int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: a  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 

================= SYMBOLS FORMATTED ================
main2 -> function(signed int, [(void), ])
main -> function(signed int, [(void), ])
si -> (signed char)
b -> (signed long int)
a -> (signed char)

=============== PARSE TREE ===============
signed int main(void ){signed char si;( si/*result:(signed char)*//*operand:(signed int)*/++);( ++/*result:(signed char)*//*operand:(signed int)*/si);return 1;}signed int main2(void ){signed char a;signed long int b;(a/*result:(signed char)*//*operand:(signed long int)*/ *= b);return 1;}

