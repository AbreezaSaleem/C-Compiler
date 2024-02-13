
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: fib  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 1 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: n  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

================= SYMBOLS FORMATTED ================
fib -> function(signed short int, [(signed short int), ])
main -> function(signed int, [(void), ])
n -> (signed short int)

=============== PARSE TREE ===============
signed int main(void ){return 0;}signed short int fib(signed short int n){return  (signed short int )(( (signed int )n < 2) ? 1 : (fib( (signed short int )( (signed int )n - 1)) + fib( (signed short int )( (signed int )n - 2))));}

