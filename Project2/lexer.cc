//
// Name: Divanshu Chauhan
//
// Date: 06/19/2024
//
// Description: This file contains the implementation of a lexical analyzer for a simple programming language.
//
/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
// import necessary libraries
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

// import files
#include "lexer.h"    // Header file for the lexical analyzer class
#include "inputbuf.h" // Header file for the input buffer class

using namespace std;

// Array of strings representing reserved tokens in the language
string reserved[] = {
    "END_OF_FILE",
    "PUBLIC", "PRIVATE", "EQUAL",
    "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR"};

#define KEYWORDS_COUNT 2                  // Number of keywords in the language
string keyword[] = {"public", "private"}; // Array of keyword strings

// Print function for a token, displaying its lexeme, type, and line number
void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int)this->token_type] << " , "
         << this->line_no << "}\n";
}

// Constructor for the LexicalAnalyzer class, initializes line number and temporary token
LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

// Function to skip over comments in the input
Token LexicalAnalyzer::SkipComment()
{
    // Skip characters until a newline or end of input is found
    char c;
    input.GetChar(c);

    for (; !input.EndOfInput() && c != '\n'; input.GetChar(c))
    {
    }

    // Increment line number for the skipped line
    line_no = line_no + 1;

    // If not at the end of input, put the last character back
    if (!input.EndOfInput())
        input.UngetChar(c);

    // Proceed to get the next token after the comment
    return GetToken();
}

// Function to skip whitespace characters in the input
bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool whitespaceFound = false;

    input.GetChar(c);
    if (c == '\n')
        line_no++;

    for (; !input.EndOfInput() && isspace(c); input.GetChar(c))
    {
        whitespaceFound = true;
        if (c == '\n')
            line_no++; // Increment line number for newlines
    }

    if (!input.EndOfInput())
        input.UngetChar(c);
    return whitespaceFound;
}

// Function to check if a string is a keyword
bool LexicalAnalyzer::IsKeyword(string s)
{
    int i = 0;
    while (i < KEYWORDS_COUNT)
    {
        if (s == keyword[i])
            return true;
        i++;
    }
    return false;
}

// Function to find the index of a keyword, used to determine its token type
TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    int i = 0;
    while (i < KEYWORDS_COUNT)
    {
        if (s == keyword[i])
            return (TokenType)(i + 1); // Return the token type, offset by 1 to account for the END_OF_FILE token
        i++;
    }
    return ERROR; // Return ERROR token type if not found
}

// Function to scan identifiers or keywords in the input
Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);
    if (!isalpha(c))
    {
        if (!input.EndOfInput())
            input.UngetChar(c);
        tmp.token_type = ERROR;
    }
    else
    {
        tmp.lexeme = "";
        for (; !input.EndOfInput() && isalnum(c); input.GetChar(c))
            tmp.lexeme += c;
        if (!input.EndOfInput())
            input.UngetChar(c);
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme); // Set token type if it's a keyword
        else
            tmp.token_type = ID; // Set token type to ID for identifiers
    }
    return tmp;
}

// Function to unget (put back) a token, used for lookahead
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);
    return tok.token_type;
}

// Main function to get the next token from the input
Token LexicalAnalyzer::GetToken()
{
    char c;

    // If there are ungotten tokens, return the last one
    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    // Skip any whitespace before getting the next token
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);

    // Check the first character to determine the token type
    if (c == '/')
    {
        char next_char;
        input.GetChar(next_char);
        if (next_char == '/') // If it's a comment, skip it
            return SkipComment();
        input.UngetChar(next_char);
        tmp.token_type = ERROR;
    }
    else if (c == '=')
        tmp.token_type = EQUAL;
    else if (c == ':')
        tmp.token_type = COLON;
    else if (c == ',')
        tmp.token_type = COMMA;
    else if (c == ';')
        tmp.token_type = SEMICOLON;
    else if (c == '{')
        tmp.token_type = LBRACE;
    else if (c == '}')
        tmp.token_type = RBRACE;
    else if (isalpha(c)) // If it starts with a letter, it could be an ID or keyword
    {
        input.UngetChar(c);
        return ScanIdOrKeyword();
    }
    else if (input.EndOfInput())
        tmp.token_type = END_OF_FILE; // End of file token
    else
        tmp.token_type = ERROR; // Error token for unrecognized characters

    return tmp;
}