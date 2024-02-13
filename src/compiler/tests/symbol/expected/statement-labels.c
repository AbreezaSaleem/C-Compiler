
================= SYMBOLS ================

string literal table (STRING_LITERLS):
  string: "abreeza"  ||  id: _StringLabel_0 

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: func3  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 8 
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 3 
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: another  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: first_function_variable  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 2 

statement label table (PROCEDURE_SCOPE):
  label: label  ||  id: 1  ||  is_defined: 1  ||  is_used: 1 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: c  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 4 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: g  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 5 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: b  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 
  variable: a  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 6 

statement label table (PROCEDURE_SCOPE):
  label: abreeza  ||  id: 2  ||  is_defined: 1  ||  is_used: 1 
  label: saleem  ||  id: 3  ||  is_defined: 1  ||  is_used: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: three  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 9 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: saleem  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 10 

================= SYMBOLS FORMATTED ================
func3 -> function(signed int, [(void), ])
main2 -> function(signed int, [(void), ])
main -> function(signed int, [(void), ])
another -> pointer((signed char))
first_function_variable -> (signed int)
c -> (signed int)
g -> (signed int)
b -> (signed int)
a -> (signed int)
three -> (signed int)
saleem -> (signed int)

=============== PARSE TREE ===============
signed char (* another);signed int main(void ){signed int first_function_variable;}signed int main2(void ){signed int c;label:{signed int g;}{signed int a;signed int b;}goto label;}signed int func3(void ){signed int three;(another = "abreeza");goto abreeza;abreeza:{three;}goto abreeza;saleem:{signed int saleem;}}

