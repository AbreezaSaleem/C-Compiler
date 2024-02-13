# CSCI E-95 Compiler Design and Implementation

[Course Website](https://cscie95.dce.harvard.edu/fall2023/index.html)

### Overview
This project is a compiler implementation that follows a six-step process to transform source code into MIPS assembly language. The six steps include:

1. Scanner (Lexical Analysis)
2. Parser (Bison/Yacc)
3. Symbol Table
4. Type Checking
5. Intermediate Representation (IR)
6. MIPS Code Generation

After these 6 steps, further two levels of optimization can be applied using the appropiate flag.

### Building the compiler

Run `make` to execute the Makefile

Use any of the following commands to build the compiler:
##### Basic
`./compiler test.c`
`./compiler -s ir test.c`

##### Output file
`./compiler -s ir -o output.s  test.c`

##### Annotation
`./compiler -a -s symbol test.c`

##### Annotation + output file
`./compiler -a -s symbol -o output.s  test.c`

##### Optimization
`./compiler -s ir -O 2 test.c`

##### Optimization + output file
`./compiler -s ir -o output.s -O 2 test.c`

Note: use `-O 1` for level 1 optimization and `-O 2` for level 2. 

### Optimization

Here is a flow of my optimizations steps:

#### Level 1
1. (MIPS level) Save/restore only the t-registers that are being used
2. (MIPS level) Convert MIPS instruction to immediate instruction where ever possible
3. (MIPS level) Make use of `offset` field in `lw` and `sw` to remove redundant `la` calls
4. (IR level) Perform identity optimizations where ever possible. This includes:
    - Addition+Subtraction
       - Remove instructions for `a +/- 0`
       - Calculate the result if both are constants and use that
    - Multiply
       - Convert `a * (log2(x))  ==>  a << x`
       - Remove instructions for `a * 1`
       - Calculate the result if both are constants and use that
       - For `a * x` where `x` is not a power of 2 I break down the operation to multiple left-shift instructions. For example: `a * 50`  =>  `a << 64  - (a * 14)  ==>   (a << 6) - (a*16 - (a*2))  ==>  (a << 6) - ((a << 4) - (a << 1)) `. I tried this approach with a bunch of different inputs and it was working
    - Divide
       - Convert `a * (log2(x))  ==>  a >> x` _only_ for unsigned numbers. I have not handled signed positive numbers yet
       - Remove instructions for `a / 1`
       - Calculate the result if both are constants and use that
    - Greater than/Less than
       - Calculate the result if both are constants and use that
    - Remainder
       - Replace instructions for `a % 1` with `(constInt, 0)`
       - Calculate the result if both are constants and use that

#### Level 2
This mainly focuses on deadcode removal. I keep iterating through the IR list until no more optimizations can be performed.
1. (MIPS level) Convert condition+branch instruction to branch instruction
3. (IR level) Remove unused `constInt` instructions 
4. (IR level) Remove goto+label combination if the goto is immediately followed by the goto statement
5. (IR level) Remove complete label block if no goto statement exist for it
6. (IR level) Remove unreachable blocks.


### Testing

Use the following script to invoke various test cases:

##### Parser
`./idempotency-test.sh -s parser`

##### Symbol
`./idempotency-test.sh -s symbol`

##### Type
`./idempotency-test.sh -s type`

##### IR
`./idempotency-test.sh -s ir`

##### Mips
`./idempotency-test.sh -s mips`

Find all the test cases here: https://github.com/CSCIE9X/csci-e-95-2023-fall-AbreezaSaleem/tree/master/src/compiler/tests

I hope this README is helpful. Please reach out if you have any questions! 

#### Abreeza Saleem
