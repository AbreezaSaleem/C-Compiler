#ifndef _TOKENS_H
#define _TOKENS_H

#include <stdio.h>
#include <stdbool.h>

const char* tokenNames[] = {
  /* RESVWORDs */
  [T_BREAK] = "Break", [T_SWITCH] = "Switch", [T_CASE] = "Case", [T_DEFAULT] = "Default", [T_CHAR] = "Char",
  [T_CONTINUE] = "Continue", [T_DO] = "Do", [T_ELSE] = "Else", [T_FOR] = "For", [T_GOTO] = "Goto", [T_IF] = "If",
  [T_LONG] = "Long", [T_RETURN] = "Return", [T_SHORT] = "Short", [T_UNSIGNED] = "Unsigned", [T_SIGNED] = "Signed",
  [T_INT] = "Int", [T_VOID] = "Void", [T_WHILE] = "While", [T_DOUBLE] = "Double",
  [T_STATIC] = "Static", [T_CONST] = "Const", 
  /* IDs */
  [T_IDENTIFIER] = "Identifier",
  /* TYPES */
  [T_NUMBER] = "Number", [T_STRING] = "String", [T_DECIMAL] = "Decimal",
  [T_CHARACTER_LITERAL] = "Character Literal",
  /* OPERATORS */
  [T_SIZEOF] = "Sizeof", [T_NOT] = "Not", [T_PLUS] = "Plus", [T_EQUAL] = "Equal", [T_PERCENT] = "Percent",
  [T_CIRCUMFLEX] = "Circumflex", [T_ASTERISK] = "Asterisk", [T_SLASH] = "Slash", [T_HYPHEN] = "Hyphen",
  /* GRAPHIC CHARACTERS */
  [T_DOUBLE_QUOTE] = "Double Quote", [T_NUMBER_SIGN] = "Number Sign", [T_AMPERSAND] = "Ampersand",
  [T_LEFT_BRACE] = "Left Brace", [T_RIGHT_BRACE] = "Right Brace",
  [T_TILDE] = "Tilde", [T_LEFT_BRACKET] = "Left Bracket", [T_RIGHT_BRACKET] = "Right Bracket",
  [T_COMMA] = "Comma", [T_PERIOD] = "Period", [T_BACKSLASH] = "Backslash",
  [T_LESS_THAN] = "Less Than", [T_GREATER_THAN] = "Greater Than", [T_VERTICAL_BAR] = "Vertical Bar", [T_LEFT_PARENTHESIS] = "Left Parenthesis",
  [T_RIGHT_PARENTHESIS] = "Right Parenthesis", [T_SEMICOLON] = "Semicolon", [T_COLON] = "Colon",
  [T_QUESTION_MARK] = "Question Mark", [T_APOSTROPHE] = "Apostrophe",
  /* COMPOUND CHARACTERS */
  [T_PLUS_EQUAL] = "Plus Equal", [T_MINUS_EQUAL] = "Minus Equal", [T_MULTIPLY_EQUAL] = "Multiple Equal", [T_DIVIDE_EQUAL] = "Divide Equal",
  [T_MOD_EQUAL] = "Mod Equal", [T_LESS_LESS_EQUAL] = "Less Less Equal", [T_GREATER_GREATER_EQUAL] = "Greater Greater Equal",
  [T_AND_EQUAL] = "And Equal", [T_CIRCUMFLEX_EQUAL] = "Circumflex Equal", [T_VERTICAL_BAR_EQUAL] = "Vertical Bar Equal",
  [T_PLUS_PLUS] = "Plus Plus", [T_MINUS_MINUS] = "Minus Minus", [T_LESS_LESS] = "Less Less", [T_GREATER_GREATER] = "Greater Greater",
  [T_LESS_EQUAL] = "Less Equal", [T_GREATER_EQUAL] = "Greater Equal", [T_EQUAL_EQUAL] = "Equal Equal", [T_NOT_EQUAL] = "Not Equal",
  [T_AND_AND] = "And And", [T_OR_OR] = "Or Or",
  /* MSC */
  [T_COMMENT] = "Comment",
};

#endif /* _TOKENS_H */
