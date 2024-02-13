
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: p  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
p -> pointer((signed int))
a -> array(10, (signed int))

=============== PARSE TREE ===============
signed int main(void ){signed int (a[10]),(* p);(p =  (signed int *)a);return 1;}

