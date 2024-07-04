//
// Name: Divanshu Chauhan
//
// Date: 07/03/2024
//
//
// Description: This is project 3 for cse340 summer 24.
//              This file contains the class definition for the Lexer class.
//
/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */

#ifndef __LEXER__H__
#define __LEXER__H__

#include <vector>
#include <string>

#include "inputbuf.h"

typedef enum
{
    END_OF_FILE,
    INT,
    REAL,
    BOO,
    TR,
    FA,
    IF,
    WHILE,
    SWITCH,
    CASE,
    PUBLIC,
    PRIVATE,
    NUM,
    REALNUM,
    NOT,
    PLUS,
    MINUS,
    MULT,
    DIV,
    GTEQ,
    GREATER,
    LTEQ,
    NOTEQUAL,
    LESS,
    LPAREN,
    RPAREN,
    EQUAL,
    COLON,
    COMMA,
    SEMICOLON,
    LBRACE,
    RBRACE,
    ID,
    ERROR
} TokenType;

class Token
{
public:
    void Print();

    std::string lexeme;
    TokenType token_type;
    int line_no;
};

void typeUpdating(int LHS, int RHS);
void addList(std::string n, int lN, int t);
int searchList(const std::string &n);

class LexicalAnalyzer
{
public:
    Token GetToken();
    TokenType UngetToken(Token);
    LexicalAnalyzer();

private:
    std::vector<Token> tokens;
    int line_no;
    Token tmp;
    InputBuffer input;

    bool SkipSpace();
    bool SkipComments();
    bool IsKeyword(std::string);
    TokenType FindKeywordIndex(std::string);
    Token ScanIdOrKeyword();
    Token ScanNumber();
};

#endif //__LEXER__H__