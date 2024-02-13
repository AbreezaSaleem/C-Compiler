
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: a  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: crr  ||  type: signed char  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 
  variable: brr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: arr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
a -> (signed char)
crr -> array(45, (signed char))
brr -> array(54, (signed int))
arr -> array(45, (signed int))

=============== PARSE TREE ===============
signed int main(void ){signed int (arr[45]);signed int (brr[54]);signed char (crr[45]);signed char a;(a =  (signed char )(( *( (signed int *)arr + 4)) + ( *( (signed int *)brr + 43))));return 1;}

