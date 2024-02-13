			    Problem Set 5

		 Harvard Extension School CSCI E-95:
		 Compiler Design and Implementation

			      Fall 2023

	 Due: November 12, 2023 at Midnight ET (Eastern Time)

(100 Points) Based on all the work you have completed so far, your
lexer from Problem Set 1, your parser from Problem Set 2, your symbol
table management system from Problem Set 3, and the type checker from
Problem Set 4, develop code to walk over the abstract syntax tree
(AST) and generate intermediate code to allow optimization and real
code generation to take place in the future.

1. Using the three-address quadruple representation we discussed in
class, walk over the AST -- which, after Problem Set 4, is type
correct -- and emit intermediate instructions in a doubly-linked-list
representation.  Proceed with the simpler AST nodes first and then
progress to the more complicated nodes.  Remember that many
instructions in the intermediate code representation may contain fewer
than three addresses.

2. When generating the intermediate representation (IR) code, assume that
an infinite number of registers is available.  Therefore, you should
generate a new register temporary as the result of each IR node
(except if the result is a user-specified location).  During the code
generation phase these temporaries will be allocated to real
registers.

3. Your IR code must use the operations in the Intermediate
Representation slides presented in class.  Once you're working on
optimizations for your Final Project, feel free to add additional
opcodes as necessary using similar naming conventions.  Because our IR
is based on a load/store architecture, only the load and store IR
operations access user variables.  Of course, the addressOf IR
operation can be used to make a register/temporary contain the address
of a user variable.

4. As presented in class, all object type information, such as signedness
and size, is reflected in the opcode of the IR node rather than
determined from the type of an operand. 
      - Therefore, there are several
versions of opcodes to perform, say, less-than comparison -- a signed
word less-than comparison (lessThanSignedWord) and an unsigned word
less-than comparison (lessThanUnsignedWord). 
      - Similarly, there are several versions of the loadWord opcode -- a loadByte, a loadHalfWord,
and a loadWord.
      - Because of the type conversions that are defined by
the C Programming Language, many versions of opcodes could never be
utilized and should not be defined (such as any version of
addHalfWord).
      - Also, some type information may not be relevant to some
opcodes and may be omitted.  For example, there is no need to
distinguish loadSignedWord from loadUnsignedWord -- either of these IR
instuctions should be emitted as loadWord.

5. The C Standard allows the right shift operator, >>, when applied to a
signed left operand, to either sign extend the operand or to shift in
zeros.  In our implementation, when the right shift operator is
applied to a signed left operand, we will sign extend it.

6. As described in class, always evaluate an expression as an lvalue, if
possible.  Only convert an lvalue to an rvalue when needed.  Tag each
temporary to indicate if it is an lvalue or an rvalue.

7. Remember to ensure that the user has defined a function named main.
There is required to be such a function.

8. Write a pretty-printer for the intermediate code instructions in the
linked-list representation so that we can see the code you generated from
your AST.

As always, the solution that you turn in should include a demonstration
that your program works and has been tested.  Thus, the test programs
you use as input and the output that your compiler produces should be
included with your other files.


			Last revised 24-Oct-23
