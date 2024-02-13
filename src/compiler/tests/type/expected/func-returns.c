
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: func  ||  type: signed long int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

================= SYMBOLS FORMATTED ================
func -> function(signed long int, [(void), ])
main -> function(signed int, [(void), ])

=============== PARSE TREE ===============
signed int main(void ){return 45;}signed long int func(void ){return  (signed long int )45;}

