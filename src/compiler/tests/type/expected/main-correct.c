
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: f  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(signed int), array(N/A, pointer((signed char), ))])
f -> array(N/A, pointer((signed char)))
a -> (signed int)

=============== PARSE TREE ===============
signed int main(signed int a,signed char (* (f[]))){return 1;}

