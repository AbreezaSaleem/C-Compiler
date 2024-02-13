
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: brr  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
brr -> array(45, (signed char))

=============== PARSE TREE ===============
signed int main(void ){signed char (brr[45]);(( *( (signed int ) (signed char *)brr + 0)) = 97);return 1;}

