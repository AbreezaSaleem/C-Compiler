
================= SYMBOLS ================

string literal table (STRING_LITERLS):

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 4 
  variable: p  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 
  variable: arr  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: x  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 
  variable: a  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: z  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 8 
  variable: y  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 7 
  variable: x  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 6 
  variable: arr2  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 5 

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
p -> pointer(array(x, pointer((signed int))))
arr -> array(a, (signed int))
x -> (signed int)
a -> (signed int)
z -> (signed int)
y -> (signed int)
x -> (signed int)
arr2 -> array(45, (signed int))

=============== PARSE TREE ===============
signed int a,x;signed int (arr[a]);signed int (* ((*p)[x]));signed int main(void ){signed int (arr2[45]);signed int x,y,z;( *(arr2 + ( ~x)));( *(arr + (x ? y : z)));}

