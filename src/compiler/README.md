To build type:

`make`

or

`make scanner`


To clean (doesn't remove scanner executable):

`make clean`

===============================================================================

                    CSCI-E95 Sample Scanner

===============================================================================

The program built from the source code here can be used to scan a program
written in the sample language. It will read the provided file (or standard input) 
and produce as output a stream of lines, each line representing a token in the 
sample language.

Usage:
    compiler -s scanner [-o <output>] [<input>]

The input file should be a (lexically) valid source language program. Output
consists of lines with fields:

line = <line-number>            the current line number in the source program

text = <text>                   the source text consumed

token = <token-type>            if the scanner found a valid token, one of:
                                    RESERVED_WORD, IDENTIFIER, OPERATOR,
                                    NUMBER, STRING

error = SCANNING_ERROR          if the scanner did not find a valid token

word = <reserved-word>          an enumerated type indicating which reserved
                                word was found, if token-type = RESERVED_WORD

id = <identifier>               a string representing the identifier found, if
                                token-type = IDENTIFIER

op = <operator>                 an enumerated type indicating which operator
                                was found, if token-type = OPERATOR

type = <number_type>            an enumerated type indicating the size of
                                integer constant found, if token-type = NUMBER
                                one of:
                                    INT_TYPE, LONG_TYPE, UNSIGNED_LONG_TYPE
                                character constants always have INT_TYPE

value = <integer>               the value of the integer constant, if
                                token-type = NUMBER

error = OVERFLOW                if the number found was too large to fit in
                                UNSIGNED_LONG_TYPE, when token-type = NUMBER

string = <string>               the value of the string constant, not including
                                the terminating NUL, if token-type = <STRING>

length = <length>               the length of the string constant, including
                                terminating NUL, if token-type = <STRING>

