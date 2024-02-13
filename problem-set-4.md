          Problem Set 4

(100 Points) Based on all the work you have completed so far, your
lexer from Problem Set 1, your parser from Problem Set 2, and your
symbol table management system from Problem Set 3, develop code to
perform semantic analysis.  That is, your code should walk over the
abstract syntax tree (AST) and perform type checking of the entire
program by referring to the symbol table.

1. After the entire AST and symbol table have been created as per Problem Set 3, your program will perform type checking of every expression in the program being compiled.
   - All appropriate conversions need to be applied (usual casting conversions, usual assignment conversions, usual unary conversions, usual binary conversions, etc.).
   - As each conversion is applied, any implicit type conversion that was present in the original program must be made explicit by adding appropriate cast operator nodes into the AST.
   - The type checking should include validity of references to identifiers such as:
     - Appropriateness of expressions used as lvalues
     - Compatibility for all operators
     - Compatibility of actual function arguments to formal function parameters
     - Propagation of function return type to utilization of the function's return value in an expression.
     - For appropriately declared functions, remember to check for no arguments and no return values.


2. In order to perform type checking, each node of your AST within an
expression needs to be tagged with the type of that node.  This type
is represented as a type data structure, or type tree, as described in
Problem Set 3 and in the Parse Tree, AST, and Type Tree slides.  In
addition to the components of the type tree already discussed, each
node of the AST within an expression should have a tag that indicates:
    - If the type at that node may be an lvalue/rvalue or is simply an
rvalue
    - If the type at that AST node is modifiable.  Note, for
example, that in H&S 7.3.1 on page 208, it states that "The name of an
array evaluates to that array; it is an lvalue, but not modifiable."

3. The lvalue/rvalue vs. rvalue and the modifiable flag may be directly
associated with each node of the AST within an expression or they may
be attached to the type tree that is associated with each node of the
AST within an expression.  It is also possible, but not required that
these attributes can also be associated with symbols in the symbol
table -- either directly with the symbol or associated with the type
tree associated with the symbol.
    - To be clear, all type trees do not need to include the lvalue/rvalue
vs. rvalue attribute and non-modifiable/modifiable attribute, because
there are some cases in which a type appears without an association
with an identifier or an expression.  This occurs in abstract
declarators.  Examples of this are that a type can be specified in a
cast operator or in a prototype declaration for a function.

4. Any errors you detect during type checking should be flagged with
emitted error messages.
    - These include parameter mismatches and return
type mismatches.  
    - If the types of actual parameters to functions are
not *identical* but are compatible with the formal parameters, your
compiler should add appropriate cast nodes into the AST to explicitly
perform those type conversions.  These might include conversion from
char to short, short to int, int to long, signed to unsigned, etc.  In
all cases, if the applied conversions do not result in an expression
with valid types, an error must be emitted.

5. You should update your
pretty printer to dump the additional explicit type conversion nodes
in the parse tree.

6. For 4 points of extra credit, for our language, check to ensure that a
function with a return value does *not* return by reaching the end of
the function body; this would allow a return without a return value.

7. For 2 points of extra credit, ensure that main is declared with a
return type and formal parameter types that are acceptable to Standard
C (See H&S 9.9 on page 303).

As mentioned in Problem Set 3,

8. "In our implementation, we will not allow declarations of variables
that are not also definitions of those variables.  The C programming
language allows an initial tentative declaration and later
declarations that add information to that initial declaration.  In our
implementation, we will allow only one declaration of an identifier at
any one scope.  The one exception to this rule is that we will allow
one or more declarations of a function that are not definitions (i.e.,
for a function, we will allow declarations of the types of the return
value and parameters -- with or without names of the formal parameters
-- without specifying the body of the function)."
Therefore, you should emit an error for any identifier that is
declared more than once (except for allowing multiple prototype
declarations of functions, but only a single function definition).

9. A cast needs to be explicitly added in each case in which the integral
constant 0 is used as a pointer.  For example, the program fragment:
```
signed int *p;
0 == p;

should be converted into:

signed int *p;
(((signed int *)0) == p);
```

10. The C Standard also says that, "the null pointer constant in C is any
integer constant expression with the value 0..."  In our language, at
this stage in the compilation process, we're not going to try to
evaluate all integer expressions which would be required to determine
if the expression has the value 0.  Therefore, in our language, we
will accept only the literal integer constant 0 as the null pointer
constant.

11. Casts should also be explicitly added in all cases where an array is
converted into a pointer to the first element of the array (See H&S
7.3.1 on page 208 and the usual unary conversions in H&S 6.3.3 on
pages 195-198).  For example, the program fragment:
```
  signed int a[10], *p;
  p = a;

should be converted into:

  signed int a[10], *p;
  p = (signed int *)a;
```
- Even though the conversion from an array to a pointer to the first
element of the array is not mentioned in H&S 7.5.2 where casts are
discussed, this conversion is applied to the operand of a cast because
of H&S 6.3.3 and H&S 7.3.1.  Table 6-2 (Permitted casting conversions)
on page 194 does not mention an array type as a permitted source type
for casting because the conversion to a pointer would have already
occurred.

12. There is one more category of type conversion that would result in an
invalid program if cast operators were simply added to the AST.  These
cases occur in the C language where an operator uses an operand as
both a source and a destination yet evaluates that operand only once.
This occurs with the ++ and -- operators and also with all of the
compound assignment operators (*=, /=, %=, +=, -=, <<=, >>=, &=, ^=,
and |=) (see Harbison & Steele, Section 7.9.2).  The operand of ++ and
-- has the usual binary conversion applied to it and the constant 1
(see Harbison & Steele, Sections 7.4.4 & 7.5.8).  That conversion
would add a cast to convert a short int into an int in the AST when
applied to the expression ++si where si is a short int.  That
conversion would result in an AST that would represent ++(signed
int)si.  Because the result of a cast conversion is never an lvalue
and because ++ and -- require their operand to be an lvalue, an error
would be output if ++(signed int)si (the output of the pretty printer)
were compiled.  Also, when the result is stored back into si, the
usual assignment conversions are performed.  There is no way to
represent both of these implicit casts explicitly in a valid C program
with the same semantics as the original program.  Therefore, please
tag any cast added to the operand of ++ or -- with an indication that
it will be pretty-printed as a comment and also whether the cast is to
be applied to the operand or to the result.  Thus, ++si would be
pretty-printed as:

  ```++/*result:(signed short int)*//*operand:(signed int)*/si```

after this problem set.

13. Similarly, the compound assignment operators will apply "usual
conversions" to their left-hand-side (lhs) when treating it as an
rvalue operand (H&S 7.9.2).  And, of course, the usual assignment
conversions will apply when using the lhs as the lvalue result of the
assignment (H&S 7.9.2).  Any implied cast that would be needed on the
rvalue of the lhs operand *and* any implied cast that would be needed
on the lvalue of the lhs result should be represented as explicit
casts applied to the lhs of the compound assignment operator -- and
also tagged to indicate if the cast is when the lhs is used as an
operand or as a result.  Such casts should also be tagged with an
indication that they will be pretty-printed as comments.  Because
casts can never appear on the lhs of an assignment operator, the later
stages of our compiler will know that a cast of the lhs of a compound
assignment operator was added in the type checking phase and should be
applied to the lhs operand when being used as a rvalue operand and as
an lvalue result, as appropriate.  So, c+=i where c is a char and i is
an int, would emerge from type checking as:

  ```/*result:(signed char)*//*operand:(signed int)*/c+=i.```

14. As mentioned in Harbison & Steele, Section 8.8 on page 277, any
"break" statement must be located within the context of a looping
construct ("while", "do", or "for").  (In the complete C Programming
Language, of course, a "break" statement would also be valid in the
context of a "switch" statement.)  Similarly, any "continue" statement
must be located within the context of a looping construct ("while",
"do", or "for").  If these constraints are not met, an error should be
emitted.

As always, the solution that you turn in should include a
demonstration that your program works and has been tested.  Thus, the
test programs you use as input and the output that your compiler
produces should be included with your other files.  Continue to use
"git" to turn in your completed work.
