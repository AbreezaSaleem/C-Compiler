
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 6 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: c  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: b  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: d  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 

symbol table (BLOCK_SCOPE) ||  parent (BLOCK_SCOPE):
  variable: e  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: f  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 7 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):

================= SYMBOLS FORMATTED ================
main2 -> function(signed int, [(void), ])
main -> function(signed int, [(void), ])
a -> (signed int)
c -> (signed int)
b -> (signed int)
d -> (signed int)
e -> (signed int)
f -> (signed int)

=============== PARSE TREE ===============
signed int a;signed int main(void ){signed int b,c;(a = 34);(a = 89);(b = 5);(c = 45);{signed int d;(d = 56);{signed int e;(e = 67);}}}signed int main2(void ){signed int f;if((f > 45)){(f = 56);}}

