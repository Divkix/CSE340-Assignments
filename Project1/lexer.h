//
// Name: Divanshu Chauhan
// ASU ID: 1224807311
// Date: 5/29/2024
//

/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */

#ifndef __LEXER__H__
#define __LEXER__H__

// import necessary libraries
#include <vector>
#include <string>
#include "inputbuf.h"

// ------- token types -------------------

typedef enum
{
  END_OF_FILE = 0,
  IF,
  WHILE,
  DO,
  THEN,
  PRINT,
  PLUS,
  MINUS,
  DIV,
  MULT,
  EQUAL,
  COLON,
  COMMA,
  SEMICOLON,
  LBRAC,
  RBRAC,
  LPAREN,
  RPAREN,
  NOTEQUAL,
  GREATER,
  LESS,
  LTEQ,
  GTEQ,
  DOT,
  NUM,
  ID,
  ERROR,
  // added new labels for new token types below
  REALNUM,
  BASE08NUM,
  BASE16NUM
} TokenType;

// token class
class Token
{
public:
  void Print();
  int line_no;
  std::string lexeme;
  TokenType token_type;
};

// lexical analyzer class
class LexicalAnalyzer
{
public:
  Token GetToken();
  TokenType UngetToken(Token);

  LexicalAnalyzer();

private:
  std::vector<Token> tokens;

  int line_no;
  bool SkipSpace();
  bool IsKeyword(std::string);

  Token tmp;
  InputBuffer input;
  TokenType FindKeywordIndex(std::string);
  Token ScanIdOrKeyword();
  Token ScanNumber();
};

#endif //__LEXER__H__