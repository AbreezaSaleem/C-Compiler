
================= SYMBOLS ================

string literal table (STRING_LITERLS):
  string: "rizwana"  ||  id: _StringLabel_0 
  string: "saleem"  ||  id: _StringLabel_1 
  string: "abreeza"  ||  id: _StringLabel_2 

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main2  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 1 
  variable: az  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

statement label table (PROCEDURE_SCOPE):
  label: label  ||  id: 1  ||  is_defined: 1  ||  is_used: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):
  variable: c  ||  type: signed int  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: g  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 3 

symbol table (BLOCK_SCOPE) ||  parent (BLOCK_SCOPE):
  variable: j  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 4 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: h  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 5 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: k  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 6 

symbol table (BLOCK_SCOPE) ||  parent (PROCEDURE_SCOPE):
  variable: l  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 

================= SYMBOLS FORMATTED ================
main2 -> function(signed int, [(void), ])
az -> pointer((signed char))
c -> (signed int)
g -> (signed int)
j -> (signed int)
h -> (signed int)
k -> (signed int)
l -> (signed int)

=============== PARSE TREE ===============
signed char (* az);signed int main2(void ){signed int c;{signed int g;(c = 44);(az = "rizwana");{signed int j;(az = "saleem");}}{signed int h;(az = "saleem");}{signed int k;}label:{signed int l;}(az = "abreeza");}

