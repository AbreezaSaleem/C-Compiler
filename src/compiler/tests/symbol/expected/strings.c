
================= SYMBOLS ================

string literal table (STRING_LITERLS):
  string: "hello world"  ||  id: _StringLabel_0 
  string: "hello world again"  ||  id: _StringLabel_1 
  string: "third"  ||  id: _StringLabel_2 

symbol table (FILE_SCOPE) ||  parent (NULL):
  variable: main  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 1  ||  id: 9 
  variable: four  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 8 
  variable: three  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 7 
  variable: two  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 6 
  variable: one  ||  type: signed int  ||  is_used: 0  ||  is_function_defined: 0  ||  id: 5 
  variable: not_string  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 4 
  variable: another_another  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 3 
  variable: another  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 2 
  variable: another_string  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 1 
  variable: string  ||  type: signed char  ||  is_used: 1  ||  is_function_defined: 0  ||  id: 0 

symbol table (PROCEDURE_SCOPE) ||  parent (FILE_SCOPE):

================= SYMBOLS FORMATTED ================
main -> function(signed int, [(void), ])
four -> (signed int)
three -> (signed int)
two -> (signed int)
one -> (signed int)
not_string -> (signed char)
another_another -> pointer((signed char))
another -> pointer((signed char))
another_string -> pointer((signed char))
string -> pointer((signed char))

=============== PARSE TREE ===============
signed char (* string);signed char (* another_string);signed char (* another);signed char (* another_another);signed char not_string;signed int one;signed int two;signed int three;signed int four;signed int main(void ){(string = "hello world");(another_string = "hello world again");(not_string = 97);(another = "third");(another_another = "third");}

