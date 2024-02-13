
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: func  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 4 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: c  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: b  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: c  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 
  variable: b  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 6 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 

================= SYMBOLS FORMATTED ================
func -> function(signed int, [(signed int), (signed short int), (signed int), ])
main -> function(signed int, [(void), ])
c -> (signed int)
b -> (signed char)
a -> (signed char)
c -> (signed int)
b -> (signed short int)
a -> (signed int)

=============== PARSE TREE ===============
signed int main(void ){signed char a,b;signed int c;(a =  (signed char )( (signed int )b + c));return 1;}signed int func(signed int a,signed short int b,signed int c){return (a +  (signed int )b);}

