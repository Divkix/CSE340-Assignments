/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "lexer.h"
#include "inputbuf.h"
#include "sTable.h"

using namespace std;

string reserved[] = {
    "END_OF_FILE", "INT", "REAL", "BOOL", "TR", "FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", "REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR"};

#define KEYWORDS_COUNT 11

static std::string keyword[] = {"int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

LexicalAnalyzer lexer;
Token token;
TokenType tempTokenType;

int numEnum = 4;

extern int parse_program();
extern void printList();

char *lResolve;
char *rResolve;
int line = 0;

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int)this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    line = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');
    line = line_no;

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
        line = line_no;
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComments()
{
    bool comments = false;
    char c;
    if (input.EndOfInput())
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);

    if (c == '/')
    {
        input.GetChar(c);
        if (c == '/')
        {
            comments = true;
            while (c != '\n')
            {
                comments = true;
                input.GetChar(c);
            }
            line_no++;
            line = line_no;
            SkipComments();
        }
        else
        {
            comments = false;
            cout << "Syntax Error\n";
            exit(0);
        }
    }
    else
    {
        input.UngetChar(c);
        return comments;
    }
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
        {
            return (TokenType)(i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            input.GetChar(c);
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                    input.UngetChar(c);
                }
            }
            else
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
            }
            if (c == '.')
            {
                input.GetChar(c);

                if (!isdigit(c))
                {
                    input.UngetChar(c);
                }
                else
                {
                    while (!input.EndOfInput() && isdigit(c))
                    {
                        tmp.lexeme += c;
                        input.GetChar(c);
                        realNUM = true;
                    }
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        if (realNUM)
        {
            tmp.token_type = REALNUM;
        }
        else
        {
            tmp.token_type = NUM;
        }
        tmp.line_no = line_no;
        return tmp;
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c))
    {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c))
        {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
        {
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        }
        else
        {
            tmp.token_type = ID;
        }
    }
    else
    {
        if (!input.EndOfInput())
        {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    SkipComments();
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c)
    {
    case '!':
        tmp.token_type = NOT;
        return tmp;
    case '+':
        tmp.token_type = PLUS;
        return tmp;
    case '-':
        tmp.token_type = MINUS;
        return tmp;
    case '*':
        tmp.token_type = MULT;
        return tmp;
    case '/':
        tmp.token_type = DIV;
        return tmp;
    case '>':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = GTEQ;
        }
        else
        {
            input.UngetChar(c);
            tmp.token_type = GREATER;
        }
        return tmp;
    case '<':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = LTEQ;
        }
        else if (c == '>')
        {
            tmp.token_type = NOTEQUAL;
        }
        else
        {
            input.UngetChar(c);
            tmp.token_type = LESS;
        }
        return tmp;
    case '(':
        tmp.token_type = LPAREN;
        return tmp;
    case ')':
        tmp.token_type = RPAREN;
        return tmp;
    case '=':
        tmp.token_type = EQUAL;
        return tmp;
    case ':':
        tmp.token_type = COLON;
        return tmp;
    case ',':
        tmp.token_type = COMMA;
        return tmp;
    case ';':
        tmp.token_type = SEMICOLON;
        return tmp;
    case '{':
        tmp.token_type = LBRACE;
        return tmp;
    case '}':
        tmp.token_type = RBRACE;
        return tmp;
    default:
        if (isdigit(c))
        {
            input.UngetChar(c);
            return ScanNumber();
        }
        else if (isalpha(c))
        {
            input.UngetChar(c);
            return ScanIdOrKeyword();
        }
        else if (input.EndOfInput())
        {
            tmp.token_type = END_OF_FILE;
        }
        else
        {
            tmp.token_type = ERROR;
        }
        return tmp;
    }
}

int parse_varlist(void)
{
    token = lexer.GetToken();
    int tempI;

    char *lexeme = (char *)malloc(sizeof(token.lexeme) + 1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size() + 1);
    addList(lexeme, token.line_no, 0);

    if (token.token_type == ID)
    {
        token = lexer.GetToken();
        if (token.token_type == COMMA)
        {
            tempI = parse_varlist();
        }
        else if (token.token_type == COLON)
        {
            tempTokenType = lexer.UngetToken(token);
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_body(void);
Token token1;

int parse_unaryOperator(void)
{
    token = lexer.GetToken();

    if (token.token_type == NOT)
    {
        return (1);
    }
    else
    {
        cout << "\n Syntax Error\n";
        return (0);
    }
}

int parse_binaryOperator(void)
{
    token = lexer.GetToken();
    if (token.token_type == PLUS)
    {
        return (15);
    }
    else if (token.token_type == MINUS)
    {
        return (16);
    }
    else if (token.token_type == MULT)
    {
        return (17);
    }
    else if (token.token_type == DIV)
    {
        return (18);
    }
    else if (token.token_type == GREATER)
    {
        return (20);
    }
    else if (token.token_type == LESS)
    {
        return (23);
    }
    else if (token.token_type == GTEQ)
    {
        return (19);
    }
    else if (token.token_type == LTEQ)
    {
        return (21);
    }
    else if (token.token_type == EQUAL)
    {
        return (26);
    }
    else if (token.token_type == NOTEQUAL)
    {
        return (22);
    }
    else
    {
        cout << "\n Syntax Error \n";
        return (-1);
    }
}

int parse_primary(void)
{
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        return (searchList(token.lexeme));
    }
    else if (token.token_type == NUM)
    {

        return (1);
    }
    else if (token.token_type == REALNUM)
    {

        return (2);
    }
    else if (token.token_type == TR)
    {

        return (3);
    }
    else if (token.token_type == FA)
    {

        return (3);
    }
    else
    {
        cout << "\n Syntax Error \n";
        return (0);
    }
}

int parse_expression(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA)
    {
        lexer.UngetToken(token);
        tempI = parse_primary();
    }
    else if (token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_binaryOperator();
        int tempI1 = parse_expression();
        int tempI2 = parse_expression();

        if ((tempI1 != tempI2) || (tempI != 15 && tempI != 16 && tempI != 17 && tempI != 18 && tempI != 19 && tempI != 20 && tempI != 21 && tempI != 22 && tempI != 23 && tempI != 26))
        {

            if (tempI == 15 || tempI == 16 || tempI == 17 || tempI == 18)
            {
                if (tempI1 <= 2 && tempI2 > 3)
                {
                    typeUpdating(tempI2, tempI1);
                    tempI2 = tempI1;
                }
                else if (tempI1 > 3 && tempI2 <= 2)
                {
                    typeUpdating(tempI2, tempI1);
                    tempI1 = tempI2;
                }
                else if (tempI1 > 3 && tempI2 > 3)
                {
                    typeUpdating(tempI2, tempI1);
                    tempI2 = tempI1;
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(1);
                }
            }
            else if (tempI == 19 || tempI == 20 || tempI == 21 || tempI == 22 || tempI == 23 || tempI == 26)
            {
                if (tempI2 > 3 && tempI1 > 3)
                {
                    typeUpdating(tempI2, tempI1);
                    tempI2 = tempI1;
                    return (3);
                }
                else
                {
                    cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                    exit(1);
                }
            }
            else
            {
                cout << "TYPE MISMATCH " << token.line_no << " C2" << endl;
                exit(1);
            }
        }
        if (tempI == 19 || tempI == 20 || tempI == 21 || tempI == 23 || tempI == 26)
        {
            tempI = 3;
        }
        else
        {
            tempI = tempI2;
        }
    }

    else if (token.token_type == NOT)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_unaryOperator();
        tempI = parse_expression();
        if (tempI != 3)
        {
            cout << "TYPE MISMATCH " << token.line_no << " C3" << endl;
            exit(1);
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
        return (0);
    }
    return tempI;
}

void comparingLeft(int line_No, int token_Type)
{
    sTable *temp = symbolTable;
    while (temp->next != NULL)
    {
        if (temp->item->lineNO == line_No)
        {
            temp->item->type = token_Type;
        }
        temp = temp->next;
    }
    if (temp->item->lineNO == line_No)
    {
        temp->item->type = token_Type;
    }
}

int parse_assstmt(void)
{

    int leftHS;
    int rightHS;
    token = lexer.GetToken();
    int tempI;
    string name;

    if (token.token_type == ID)
    {

        leftHS = searchList(token.lexeme);
        token = lexer.GetToken();
        if (token.token_type == EQUAL)
        {
            token = lexer.GetToken();
            if (token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == LESS || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL || token.token_type == NOT)
            {
                lexer.UngetToken(token);
                rightHS = parse_expression();
                if (leftHS == 1 || leftHS == 2 || leftHS == 3)
                {
                    if (leftHS == rightHS)
                    {
                    }
                    else
                    {
                        if (leftHS < 3)
                        {
                            cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
                            exit(1);
                        }
                        else
                        {
                            typeUpdating(rightHS, leftHS);
                            rightHS = leftHS;
                        }
                    }
                }
                else
                {
                    typeUpdating(leftHS, rightHS);
                    leftHS = rightHS;
                }

                token = lexer.GetToken();
                if (token.token_type == SEMICOLON)
                {
                }
                else
                {
                    cout << "\n HI Syntax Error " << token.token_type << " \n";
                }
            }

            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_case(void)
{

    int tempI;
    token = lexer.GetToken();
    if (token.token_type == CASE)
    {
        token = lexer.GetToken();
        if (token.token_type == NUM)
        {
            token = lexer.GetToken();
            if (token.token_type == COLON)
            {
                tempI = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
}

int parse_caselist(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == CASE)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_case();
        token = lexer.GetToken();
        if (token.token_type == CASE)
        {
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_caselist();
        }
        else if (token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
        }
    }
    return (0);
}

int parse_switchstmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == SWITCH)
    {
        token = lexer.GetToken();
        if (token.token_type == LPAREN)
        {
            tempI = parse_expression();
            if (tempI <= 3 && tempI != 1)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C5" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if (token.token_type == RPAREN)
            {
                token = lexer.GetToken();
                if (token.token_type == LBRACE)
                {
                    tempI = parse_caselist();
                    token = lexer.GetToken();
                    if (token.token_type == RBRACE)
                    {
                    }
                    else
                    {
                        cout << "\n Syntax Error \n";
                    }
                }
                else
                {
                    cout << "\n Syntax Error \n";
                }
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_whilestmt(void)
{
    int tempI;

    token = lexer.GetToken();
    if (token.token_type == WHILE)
    {
        token = lexer.GetToken();
        if (token.token_type == LPAREN)
        {
            tempI = parse_expression();

            if (tempI != 3)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if (token.token_type == RPAREN)
            {
                tempI = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_ifstmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == IF)
    {
        token = lexer.GetToken();
        if (token.token_type == LPAREN)
        {
            tempI = parse_expression();

            if (tempI != 3)
            {
                cout << "TYPE MISMATCH " << token.line_no << " C4" << endl;
                exit(1);
            }
            token = lexer.GetToken();
            if (token.token_type == RPAREN)
            {
                tempI = parse_body();
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_stmt(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_assstmt();
    }
    else if (token.token_type == IF)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_ifstmt();
    }
    else if (token.token_type == WHILE)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_whilestmt();
    }
    else if (token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_switchstmt();
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_stmtlist(void)
{
    token = lexer.GetToken();
    int tempI;
    if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_stmt();
        token = lexer.GetToken();
        if (token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH)
        {
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_stmtlist();
        }
        else if (token.token_type == RBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_body(void)
{
    token = lexer.GetToken();
    int tempI;

    if (token.token_type == LBRACE)
    {
        tempI = parse_stmtlist();
        token = lexer.GetToken();
        if (token.token_type == RBRACE)
        {

            return (0);
        }
        else
        {
            cout << "\n Syntax Error\n ";
            return (0);
        }
    }
    else if (token.token_type == END_OF_FILE)
    {
        tempTokenType = lexer.UngetToken(token);
        return (0);
    }
    else
    {
        cout << "\n Syntax Error \n ";
        return (0);
    }
}

int parse_typename(void)
{
    token = lexer.GetToken();
    if (token.token_type == INT || token.token_type == REAL || token.token_type == BOO)
    {
        comparingLeft(token.line_no, token.token_type);
    }
    else
    {
        cout << "\n Syntax Error in parse_typename \n";
    }
    return (0);
}

int parse_vardecl(void)
{
    int tempI;
    token = lexer.GetToken();
    if (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_varlist();
        token = lexer.GetToken();
        if (token.token_type == COLON)
        {
            tempI = parse_typename();

            token = lexer.GetToken();
            if (token.token_type == SEMICOLON)
            {
            }
            else
            {
                cout << "\n Syntax Error \n";
            }
        }
        else
        {
            cout << "\n Syntax Error \n";
        }
    }
    else
    {
        cout << "\n Syntax Error \n";
    }
    return (0);
}

int parse_vardecllist(void)
{
    int tempI;
    token = lexer.GetToken();
    while (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecl();
        token = lexer.GetToken();
        if (token.token_type != ID)
        {
        }
        else
        {
        }
    }
    tempTokenType = lexer.UngetToken(token);
    return (0);
}

string global = "::";
int parse_globalVars(void)
{
    token = lexer.GetToken();
    int tempI;
    if (token.token_type == ID)
    {
        tempTokenType = lexer.UngetToken(token);

        tempI = parse_vardecllist();
    }
    else
    {
        cout << "Syntax Error";
    }
    return (0);
}

int parse_program(void)
{
    token = lexer.GetToken();
    int tempI;
    while (token.token_type != END_OF_FILE)
    {
        if (token.token_type == ID)
        {
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_globalVars();
            tempI = parse_body();
        }
        else if (token.token_type == LBRACE)
        {
            tempTokenType = lexer.UngetToken(token);
            tempI = parse_body();
        }
        else if (token.token_type == END_OF_FILE)
        {
            return (0);
        }
        else
        {
            cout << "\n Syntax Error\n";
            return (0);
        }
        token = lexer.GetToken();
    }
}

string output = "";

char null[] = "NULL";
int main()
{
    int input;
    input = parse_program();
    printList();
}
