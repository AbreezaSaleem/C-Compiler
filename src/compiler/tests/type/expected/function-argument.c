
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 3 
  variable: func2  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 1 
  variable: func  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
func2 -> function(signed int, [(signed int), ])
func -> function(signed short int, [(void), ])
a -> (signed int)

=============== PARSE TREE ===============
signed short int func(void ){return  (signed short int )0;}signed int func2(signed int a){return a;}signed int main(void ){func2( (signed int )func());return 1;}

