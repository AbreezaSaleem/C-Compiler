
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: func  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 5 
  variable: f  ||  type: pointer(signed char)  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 4 
  variable: main4  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 3 
  variable: main2  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 1  ||  id: 1 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: g  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: b  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 9 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 8 

================= SYMBOLS FORMATTED ================
func -> function(signed int, [(signed int), (signed int), ])
f -> function(pointer(signed char), [(void), ])
main4 -> function(signed int, [])
main2 -> function(signed int, [(void), ])
a -> (signed int)
g -> (signed int)
b -> (signed int)
a -> (signed int)

=============== PARSE TREE ===============
signed int a;signed int main2(void ){signed int g;(a = 3);}signed int main2(void );signed int main2(void );signed int main2(void );signed int main4(){return main2();}signed char (* f(void ));signed char *f(void ){return 0;}signed int func(signed int a,signed int b);signed int func(signed int a,signed int v);signed int func(signed int a,signed int b){(a = b);return 0;}

