
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 5 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: d  ||  type: signed long int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: c  ||  type: signed long int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: b  ||  type: signed short int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: c  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 6 

================= SYMBOLS FORMATTED ================
main2 -> function(signed int, [(void), ])
main -> function(signed int, [(void), ])
d -> (signed long int)
c -> (signed long int)
b -> (signed short int)
a -> (signed int)
c -> (signed char)

=============== PARSE TREE ===============
signed int main(void ){signed int a;signed short int b;signed long int c;signed long int d;( (signed long int )(a +  (signed int )b) * c);( !d);( & (signed int )b);(( !c) +  (signed long int )( -a));return 1;}signed int main2(void ){signed char c;(c =  (signed char )128);(c = (c + c));return 1;}

