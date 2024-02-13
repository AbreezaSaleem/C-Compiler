%option yylineno
%option reentrant
%option bison-bridge
%option bison-locations
%option nounput
%option noyyalloc
%option noyyrealloc
%option noyyfree

%option   8bit reentrant bison-bridge
%option   warn nodefault
%option   header-file="scanner.yy.h"

%{
/*
 * scanner.lex
 *
 * This file contains the specification for the Flex generated scanner
 * for the CSCI E-95 sample language.
 *
 */

  #include <stdlib.h>
  #include <errno.h>
  #include <string.h>
  #include <assert.h>


  /* Suppress compiler warnings about unused variables and functions. */
  #define YY_EXIT_FAILURE ((void)yyscanner, 2)
  #define YY_NO_INPUT

  /* Track locations. */
  #define YY_EXTRA_TYPE int
  #define YY_USER_ACTION { yylloc->first_line = yylloc->last_line = yylineno; \
                           yylloc->first_column = yyextra; \
                           yylloc->last_column = yyextra + yyleng - 1; \
                           yyextra += yyleng; }

  #include "compiler.h"
  #include "parser.tab.h"
  #include "node.h"
  #include "type.h"
  #include "tokens.h"

  extern int reset_registers;

%}



  /* ESCAPE CHARACTERS */
newline                 [\n]
whitespace              [ \t\v\f]
carriage_return         [\r\n]
escape_characters       [\t\v\f\n\b\a\'\"\?]
octal_escape_characters \\[0-7]{1,3}
  /* ESCAPE CHARACTERS END */




 /* MISC */ 
extra_graphic           [$@`]
 /* MISC END */ 




/* COMMONLY USED REGEX */
digit                   [0-9]
letter                  [A-Za-z]
/* COMMONLY USED REGEX END */




/* TYPES IN C string <--- add then + null terminator */
type                    int|long|char|unsigned|signed
number                  ([1-9]+[0-9]*)
single_digit_number     (0)
string                  \"[^\"]*\"
character               \'[0-9a-zA-Z$@`]\'|\'(\\[nrtbv?\'\\"]{1})\'
identifier              ({letter}|_)+({letter}|{digit}|_)*
block_comment           (\/\*(.|\n)*\*\/)
/* TYPES IN C END */




%%
  /* WHITESPACE CHARACTERS */
{block_comment}*          /* Do nothing */
{newline}*                /* Do nothing */
{carriage_return}*        /* Do nothing */
{whitespace}+             /* Do nothing */
  /* WHITESPACE CHARACTERS END */




  /* RESERVED WORDS */
break                     { return T_BREAK; }
continue                  { return T_CONTINUE; }
do                        { return T_DO; }
for                       { return T_FOR; }
goto                      { return T_GOTO; }
if                        { return T_IF; }
else                      { return T_ELSE; }
return                    { return T_RETURN; }
while                     { return T_WHILE; }
  /* RESERVED WORDS END*/




  /* TYPES */
char                      { return T_CHAR; }
int                       { return T_INT; }
long                      { return T_LONG; }
short                     { return T_SHORT; }
void                      { return T_VOID; }
signed                    { return T_SIGNED; }
unsigned                  { return T_UNSIGNED; }
  /* TYPES END */




  /* COMPOUND CHARACTERS */
"+="                     { return T_PLUS_EQUAL; }
"-="                     { return T_MINUS_EQUAL; }
"*="                     { return T_MULTIPLY_EQUAL; }
"/="                     { return T_DIVIDE_EQUAL; }
"%="                     { return T_MOD_EQUAL; }
"<<="                    { return T_LESS_LESS_EQUAL; }
">>="                    { return T_GREATER_GREATER_EQUAL; }
"&="                     { return T_AND_EQUAL; }
"^="                     { return T_CIRCUMFLEX_EQUAL; }
"|="                     { return T_VERTICAL_BAR_EQUAL; }
"++"                     { return T_PLUS_PLUS; }
"--"                     { return T_MINUS_MINUS; }
"<<"                     { return T_LESS_LESS; }
">>"                     { return T_GREATER_GREATER; }
"<="                     { return T_LESS_EQUAL; }
">="                     { return T_GREATER_EQUAL; }
"=="                     { return T_EQUAL_EQUAL; }
"!="                     { return T_NOT_EQUAL; }
"&&"                     { return T_AND_AND; }
"||"                     { return T_OR_OR; }
  /* COMPOUND CHARACTERS END */




  /* COMMON VARIABLES */  
{number}                 { *yylval = node_number(*yylloc, yytext, yyleng); return T_NUMBER; }
{single_digit_number}    { *yylval = node_number(*yylloc, yytext, yyleng); return T_NUMBER; }
{string}                 { *yylval = node_string(*yylloc, yytext, yyleng); return T_STRING; }
{character}              { *yylval = node_character(*yylloc, yytext, yyleng); return T_NUMBER; }
{identifier}             { *yylval = node_identifier(*yylloc, yytext, yyleng); return T_IDENTIFIER; }
  /* COMMON VARIABLES END */  




  /* OPERATROS */
"+"                      { return T_PLUS; }
=                        { return T_EQUAL; }
"%"                      { return T_PERCENT; }
"*"                      { return T_ASTERISK; }
"/"                      { return T_SLASH; }
"-"                      { return T_HYPHEN; }
"^"                      { return T_CIRCUMFLEX; }
"!"                      { return T_NOT; }
  /* OPERATROS END */




  /* GRAPHIC CHARACTERS */
";"                      { return T_SEMICOLON; }
"#"                      { return T_NUMBER_SIGN; }
"{"                      { return T_LEFT_BRACE; }
"}"                      { return T_RIGHT_BRACE; }
"~"                      { return T_TILDE; }
"["                      { return T_LEFT_BRACKET; }
"]"                      { return T_RIGHT_BRACKET; }
","                      { return T_COMMA; }
"."                      { return T_PERIOD; }
"\\"                     { return T_BACKSLASH; }
"<"                      { return T_LESS_THAN; }
">"                      { return T_GREATER_THAN; }
"|"                      { return T_VERTICAL_BAR; }
"("                      { return T_LEFT_PARENTHESIS; }
")"                      { return T_RIGHT_PARENTHESIS; }
":"                      { return T_COLON; }
"?"                      { return T_QUESTION_MARK; }
"'"                      { return T_APOSTROPHE; }
"&"                      { return T_AMPERSAND; }
  /* GRAPHIC CHARACTERS END */




.|\n                      { return -1; }
%%

void scanner_initialize(yyscan_t *scanner, FILE *input) {
  yylex_init(scanner);
  yyset_in(input, *scanner);
  yyset_extra(1, *scanner);
}

void scanner_destroy(yyscan_t *scanner) {
  yylex_destroy(*scanner);
  scanner = NULL;
}

void scanner_print_tokens(FILE *output, int *error_count, yyscan_t scanner) {
  YYSTYPE val;
  YYLTYPE loc;
  int token;

  /* Start processing the file */
  token = yylex(&val, &loc, scanner);
  while (0 != token) {
    /*
     * Print the line number. Use printf formatting and tabs to keep columns
     * lined up.
     */
    fprintf(output, "loc = %04d:%04d-%04d:%04d",
            loc.first_line, loc.first_column, loc.last_line, loc.last_column);

    /*
     * Print the scanned text. Try to use formatting but give up instead of
     * truncating if the text is too long.
     */
    if (yyget_leng(scanner) <= 20) {
      fprintf(output, "     text = %-15s", yyget_text(scanner));
    } else {
      fprintf(output, "     text = %s", yyget_text(scanner));
    }

    if (token <= 0) {
      fputs("     token = ERROR", output);
      fprintf(output, "\n \n *** Error encountered at line: %04d *** \n", loc.first_line);
      (*error_count)++;
    } else {
      assert(NULL != tokenNames[token]);
      fprintf(output, "     token = [%-15s]", tokenNames[token]);

      switch (token) {
        case T_STRING:
          if (val->data.string.octal_overflow) {
            fputs("     OCTAL OVERFLOW\n", output);
            fprintf(output, "\n \n *** Octal overflow error encountered at line: %04d *** \n", loc.first_line);
            (*error_count)++;
          } else if (val->data.string.overflow) {
            fputs("     OVERFLOW ", output);
            fprintf(output, "\n \n *** Overflow error encountered at line: %04d *** \n", loc.first_line);
            (*error_count)++;
          } else {
            fprintf(output, "     length = %d", val->data.string.length);
            fprintf(output, "     name = ");
            print_string_node(val);
          }
          break;
        case T_IDENTIFIER: {}
          fprintf(output, "     id = %s", val->data.identifier.name);
          if (val->data.identifier.overflow) {
            fputs("     OVERFLOW\n", output);
            fprintf(output, "\n \n *** Overflow error encountered at line: %04d *** \n", loc.first_line);
            (*error_count)++;
          }
          break;
        case T_CHARACTER_LITERAL:
        case T_NUMBER:
          fputs("     type = ", output);
          type_print(output, val->data.number.result.type);
          fprintf(output, "     value = [%-10lu]", val->data.number.value);
          if (val->data.number.overflow) {
            fputs("     OVERFLOW", output);
            fprintf(output, "\n \n *** Overflow error encountered at line: %04d *** \n", loc.first_line);
            (*error_count)++;
          }
          if (val->data.number.leading_zeros) {
            fputs("     LEADING ZEROS", output);
            fprintf(output, "\n \n *** Leading zeros error encountered at line: %04d *** \n", loc.first_line);
            (*error_count)++;
          }
          break;
        case T_COMMENT:
          break;
        default:
          break;
      }
    }
    fputs("\n", output);
    token = yylex(&val, &loc, scanner);
  }
}

/* Suppress compiler warnings about unused parameters. */
void *yyalloc (yy_size_t size, yyscan_t yyscanner __attribute__((unused))) {
  return (void *)malloc(size);
}

/* Suppress compiler warnings about unused parameters. */
void *yyrealloc  (void *ptr, yy_size_t size, yyscan_t yyscanner __attribute__((unused))) {
  return (void *)realloc((char *)ptr, size );
}

/* Suppress compiler warnings about unused parameters. */
void yyfree (void *ptr, yyscan_t yyscanner __attribute__((unused))) {
  free((char *)ptr); /* see yyrealloc() for (char *) cast */
}
