
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: a  ||  type: signed long int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: p  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
a -> (signed long int)
p -> pointer((signed int))

=============== PARSE TREE ===============
signed int main(void ){signed int (* p);signed long int a;(a =  (signed long int )( *p));return 1;}
