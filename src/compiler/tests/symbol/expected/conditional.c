
================= SYMBOLS ================

string literal table (STRING_LITERLS):
  string: "hello"  ||  id: _StringLabel_0 
  string: "world"  ||  id: _StringLabel_1 

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: az  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: z  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: y  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: x  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):

symbol table (BLOCK_SCOPE) ||  parent (BLOCK_SCOPE):
  variable: y  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):

================= SYMBOLS FORMATTED ================
main2 -> function(signed int, [(void), ])
az -> pointer((signed char))
z -> (signed int)
y -> (signed int)
x -> (signed int)
y -> (signed int)

=============== PARSE TREE ===============
signed char (* az);signed int main2(void ){signed int x,y,z;(x ? y : z);if((45 > 3)){(az = "hello");{signed int y;(y = 3);}} else {(az = "world");}}

