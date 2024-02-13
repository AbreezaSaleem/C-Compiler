			    Problem Set 3

1. (100 Points) Using your lexer from Problem Set 1 and your YACC or
Bison parser from Problem Set 2, develop a complete symbol table
management system.

As your compiler reads the input program being compiled, it should:

1. Generate symbol tables in memory which represent all identifiers in
the program.



The symbol tables are described in the "Symbol Table
Management" slides on the class web site
(https://cscie95.dce.harvard.edu/fall2023/slides/Symbol%20Table%20Management.pdf).



2. Each identifier needs to be associated with its appropriate type data
structure that describes the declared type of the identifier.  This
type data structure should be of the form discussed in class -- it is
the data structure shown in the "Type Checking" slides on the class
web site (https://cscie95.dce.harvard.edu/fall2023/slides/Type%20Checking.pdf).



3. All references to identifiers in the abstract syntax tree should be
replaced with pointers into the symbol table.  As in Problem Set 2,
all statements, expressions, and other elements from the grammar
should be represented by data structures in the abstract syntax tree.



4. Keep in mind that your compiler needs to produce multiple symbol
tables:
    - One for each overloading class:
        -  Ourverloading classes are: (1) statement labels and (2) "other names" (see Harbison & Steele, page 78, table 4-2).
    - One for each scope.
    - You will have a possible symbol table for statement labels for each procedure.
    - You will have a possible symbol table for "other names":
        - At file scope
        - At each procedure scope
            - Remember that each procedure scope symbol table includes the outermost block of that procedure.
        - At each block scope (see Harbison & Steele, page
75, table 4-1).



5. Errors should be emitted for identifiers that are defined more than
once in the same overloading class in the same scope and also for
references to identifiers that are not declared in that scope or in
any enclosing scope.



6. As part of this problem set, your compiler needs to ensure that:
    - A procedure prototype declaration and a procedure definition for the
same procedure have the same number of parameters, identical parameter
types, and identical return types.
    - In addition, all declarations must be checked to verify that they are for legal types.
        - For example, functions are not allowed to return an array type (see Harbison &
Steele at the bottom on page 101 and the top of page 102).
    - Also, every parameter must be named in a procedure definition.
    - Note that if an array is declared as a formal parameter, the leftmost
dimension need not be specified and, if specified, it is ignored.
Also, If an array is declared as a formal parameter, the top-level
"array of T" must be rewritten to have type "pointer to T".



7. As already required in Problem Set 2, all arithmetic types should be
converted into canonical types.  For example, "int," "signed int," and
"signed" should all be represented by the same base type.



8. For array declarations, the bounds of arrays are allowed to be
constant expressions.  These should be evaluated to an integer
constant as part of this problem set.  The original constant
expression should persist in the AST unchanged; the integer constant
to which each array bound is evaluated should appear in the symbol
table for that dimension of the array.  The AST for the original
constant expression should not appear in the symbol table.  You should
check that array indexes are specified when they are required (see
Harbison & Steele, pages 98-99 on Incomplete Array Types).




9. There are at least two approaches to dealing with statement labels:
      - Approach 1:
          - When you see a statement label defined (as in "label:")
and the label is not already in the appropriate statement label symbol
table, add it to that statement label symbol table with a "defined"
attribute. 
          - When you see a statement label defined and the label is
already in the symbol table, if the attribute is "defined," then the
label is multiply defined (emit an error message).
          - If the attribute is "referenced," then change the attribute to "defined."
          - When you see a "goto" statement, if the label referred to in the goto is already in
the symbol table, do nothing.
          - If the label referred to in the goto is
not already in the symbol table, add it to the appropriate statement
label symbol table with a "referenced" attribute.
          - After each symbol table is fully built, make a pass over the symbol table to ensure that all labels in the statement label symbol table are tagged as defined.
          - Any label tagged as "referenced" is a reference to an undefined label
(emit an error message).

      - Approach 2:
          - When you see a statement label defined (as in "label:"),
add it to the appropriate statement label symbol table.  If the label
is already in the statement label symbol table, then the label is
multiply defined (emit an error message).
          - No information needs to be
associated with the statement label in the symbol table.
          - When you see a "goto" statement, you can remember the label as an uninterpreted
string.  After the symbol tables are fully built, make a pass over the
abstract syntax tree to ensure that all references to labels in goto
statements are defined in the appropriate symbol table.
          - Error messages should be emitted for any references to undefined labels.
          - You don't need to resolve the references to statement labels in goto
statements to point into the symbol table.



10. More information will be furnished later about how to generate code
for statement label definitions and references.  Leave both the
statement label definitions and the goto statements in the abstract
syntax tree.



11. As part of this problem set, you should also populate a string literal
table that contains all constant strings that are contained in the
input program.  All references to constant strings in the abstract
syntax tree should be replaced with pointers into the string literal
table.
      - Identical string constants should point to the same string
literal table entry.
      - Associate with each string literal table entry a
compiler-generated label.  The labels should have a prefix of
_StringLabel_ followed by an integer beginning with 1 (one).  The
order in which the labels are assigned is not specified.  The first
constant string label should be _StringLabel_1, the second should be
_StringLabel_2, etc.




12. The C programming language distinguishes a declaration from a
definition.  A declaration is a statement that sets forth the name of
an identifier, but does not necessarily reserve storage for that
identifier.  A definition is a declaration statement that also
reserves storage for an identifier.  In our implementation, we will
not allow declarations of variables that are not also definitions of
those variables.
      - The C programming language allows an initial
_tentative declaration_ and _later declarations_ that add information to
that initial declaration.
      - In our implementation, we will allow only
one declaration of an identifier at any one scope.
      - We cannot have more than one definition of the same name in the same scope! Confirm this.
      - The one exception to this rule is that we will allow one or more declarations of a
function that are not definitions (i.e., for a function, we will allow
declarations of the types of the return value and parameters -- with
or without names of the formal parameters -- without specifying the
body of the function).
          - This is required so that we can have a forward
declaration of a function to allow the function to be called before
being defined (necessary for where an ordering of function definitions
before being called is not possible). 
      - Any function that is declared
must also be defined at some point in the program.  And, it is an
error to define any identifier more than once in the same scope.



13. In addition to pretty printing the abstract syntax tree (AST) from
Problem Set 2, your compiler should include a facility that dumps both
the string literal table and the symbol tables in a meaningful way
when the end of the input program has been reached.  Types of
identifiers in the symbol tables should be printed as follows:
```
  Examples for simple base types:
    int i  =>  i -> signed int
    unsigned char uc  =>  uc -> unsigned char
    long int li  =>  li -> signed long int
```
```
  Examples for pointers:
    int *p  =>  p -> pointer(signed int)
    unsigned char *p  =>  p -> pointer(unsigned char)
```
 ``` Examples for arrays:
    int a[20]  =>  a -> array(20, signed int)
    int *(a[30][20])  =>  a -> array(30, array(20, pointer(signed int)))
    int (*(a[10]))[50]  =>  a -> array(10, pointer(array(50, signed int)))
```
 ``` Examples for functions:
    char f(int i, int *p)  =>
      f -> function(signed char, [signed int, pointer(signed int)])
```


14. Finally, you need to add a switch (namely, -annotate) to your compiler
that will enable an annotation to be output after each reference to an
identifier.  If enabled, each reference to an identifier should be
tagged in such a way as to indicate in which symbol table that
identifier is declared.  This should be accomplished by inserting a
comment that includes a specific symbol table tag after each
identifier is emitted in the pretty print of the abstract syntax tree
(AST).



15. Remember the solution that you turn in should include a demonstration
that your program works and has been tested.  Thus, the test programs
you use as input and the output that your compiler produces should be
included with your other files.
