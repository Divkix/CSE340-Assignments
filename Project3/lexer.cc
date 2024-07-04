//
// Name: Divanshu Chauhan
//
// Date: 07/03/2024
//
//
// Description: This is project 3 for cse340 summer 24.
//              This file contains the implementation of the lexer and all the parser functions.
//
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
#include <functional>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <map>
#include <set>
#include <iterator>
#include "lexer.h"
#include "inputbuf.h"
#include <utility>

std::string reserved[] = {
    "END_OF_FILE", "INT", "REAL", "BOOL", "TR", "FA", "IF", "WHILE", "SWITCH", "CASE", "PUBLIC", "PRIVATE", "NUM", "REALNUM", "NOT", "PLUS", "MINUS", "MULT", "DIV", "GTEQ", "GREATER", "LTEQ", "NOTEQUAL", "LESS", "LPAREN", "RPAREN", "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE", "RBRACE", "ID", "ERROR"};

#define KEYWORDS_COUNT 11
std::string keyword[] = {"int", "real", "bool", "true", "false", "if", "while", "switch", "case", "public", "private"};

LexicalAnalyzer lexer;
Token token;
TokenType tempTokenType;

int numEnum = 4;

struct stru
{
    std::string name;
    int lineNO;
    int type;
    int isPrinted;
};

struct Node
{
    stru *item;
    Node *prev;
    Node *next;
};

Node *symbolTable;
char *lResolve;
char *rResolve;

void addList(std::string name, int line, int type)
{
    Node *newEntry = new Node();
    stru *newItem = new stru();

    newItem->name = name;
    newItem->lineNO = token.line_no;
    newItem->type = type;
    newItem->isPrinted = 0;
    newEntry->item = newItem;
    newEntry->next = NULL;

    if (symbolTable == NULL)
    {
        newEntry->prev = NULL;
        symbolTable = newEntry;
        return;
    }

    Node *temp = symbolTable;
    for (; temp->next != NULL; temp = temp->next)
    {
    }
    newEntry->prev = temp;
    temp->next = newEntry;
}

int searchList(const std::string &n)
{
    Node *temp = symbolTable;
    if (symbolTable == nullptr)
    {
        addList(n, token.line_no, numEnum);
        numEnum++;
        return 4;
    }

    for (; temp != nullptr; temp = temp->next)
    {
        if (temp->item->name == n)
            return temp->item->type;
    }

    addList(n, token.line_no, numEnum);
    return numEnum++;
}

void Token::Print()
{
    std::cout << "{" << this->lexeme << " , " << reserved[(int)this->token_type] << " , " << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    line_no = tmp.line_no = 1;
    tmp.lexeme = "";
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool got_space = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c))
    {
        got_space = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput())
        input.UngetChar(c);

    return got_space;
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

    if (c != '/')
    {
        input.UngetChar(c);
        return comments;
    }

    input.GetChar(c);
    if (c != '/')
    {
        comments = false;
        std::cout << "Syntax Error\n";
        exit(0);
    }

    comments = true;
    while (c != '\n')
    {
        comments = true;
        input.GetChar(c);
    }
    line_no++;
    SkipComments();
    return 0;
}

bool LexicalAnalyzer::IsKeyword(std::string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
            return true;
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(std::string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++)
    {
        if (s == keyword[i])
            return (TokenType)(i + 1);
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    bool realNUM = false;
    input.GetChar(c);

    if (!isdigit(c))
    {
        if (!input.EndOfInput())
            input.UngetChar(c);

        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }

    if (c == '0')
    {
        tmp.lexeme = "0";
        input.GetChar(c);
        if (c == '.')
        {
            input.GetChar(c);

            if (!isdigit(c))
                input.UngetChar(c);
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
            input.UngetChar(c);
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
                input.UngetChar(c);

            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
                realNUM = true;
            }
        }
        if (!input.EndOfInput())
            input.UngetChar(c);
    }

    if (realNUM)
        tmp.token_type = REALNUM;
    else
        tmp.token_type = NUM;

    tmp.line_no = line_no;
    return tmp;
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
            input.UngetChar(c);

        tmp.line_no = line_no;

        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    }
    else
    {
        if (!input.EndOfInput())
            input.UngetChar(c);
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
    std::map<char, std::function<Token()>> tokenMap;
    tokenMap = {
        {'!', [&]()
         { tmp.token_type = NOT; return tmp; }},
        {'+', [&]()
         { tmp.token_type = PLUS; return tmp; }},
        {'-', [&]()
         { tmp.token_type = MINUS; return tmp; }},
        {'*', [&]()
         { tmp.token_type = MULT; return tmp; }},
        {'/', [&]()
         { tmp.token_type = DIV; return tmp; }},
        {'>', [&]() -> Token
         {
             input.GetChar(c);
             if (c == '=')
             {
                 input.UngetChar(c);
                 tmp.token_type = GREATER;
                 return tmp;
             }
             tmp.token_type = GTEQ;
             return tmp;
         }},
        {'<', [&]() -> Token
         {
             input.GetChar(c);
             if (c == '=')
                 tmp.token_type = LTEQ;
             else if (c == '>')
                 tmp.token_type = NOTEQUAL;
             else
             {
                 input.UngetChar(c);
                 tmp.token_type = LESS;
             }
             return tmp;
         }},
        {'(', [&]()
         { tmp.token_type = LPAREN; return tmp; }},
        {')', [&]()
         { tmp.token_type = RPAREN; return tmp; }},
        {'=', [&]()
         { tmp.token_type = EQUAL; return tmp; }},
        {':', [&]()
         { tmp.token_type = COLON; return tmp; }},
        {',', [&]()
         { tmp.token_type = COMMA; return tmp; }},
        {';', [&]()
         { tmp.token_type = SEMICOLON; return tmp; }},
        {'{', [&]()
         { tmp.token_type = LBRACE; return tmp; }},
        {'}', [&]()
         { tmp.token_type = RBRACE; return tmp; }}};
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
    auto it = tokenMap.find(c);
    if (it == tokenMap.end())
    {
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
            tmp.token_type = END_OF_FILE;
        else
            tmp.token_type = ERROR;
        return tmp;
    }
    return it->second(); // Execute the corresponding lambda function
}

int parse_varlist()
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
            tempI = parse_varlist();
        else if (token.token_type == COLON)
            tempTokenType = lexer.UngetToken(token);
        else
            std::cout << "\n Syntax Error \n";
        return (0);
    }

    std::cout << "\n Syntax Error \n";
    return (0);
}

int parse_body();
Token token1;

int parse_unaryOperator()
{
    token = lexer.GetToken();

    if (token.token_type == NOT || token.token_type == PLUS || token.token_type == MINUS)
    {
        switch (token.token_type)
        {
        case NOT:
            return (1);
        case PLUS:
            return (2);
        case MINUS:
            return (3);
        default:
            std::cout << "\nSyntax Error\n";
            return (0);
        }
        return (0);
    }
    std::cout << "\nSyntax Error\n";
    return (0);
}

int parse_binaryOperator()
{
    std::map<TokenType, int> binaryOperatorMap = {
        {PLUS, 15},
        {MINUS, 16},
        {MULT, 17},
        {DIV, 18},
        {GREATER, 20},
        {LESS, 23},
        {GTEQ, 19},
        {LTEQ, 21},
        {EQUAL, 26},
        {NOTEQUAL, 22}};
    token = lexer.GetToken();
    auto it = binaryOperatorMap.find(token.token_type);
    if (it != binaryOperatorMap.end())
        return it->second;

    std::cout << "\n Syntax Error \n";
    return (-1);
}

int parse_primary()
{
    std::map<TokenType, int> primaryTypeMap = {
        {NUM, 1},
        {REALNUM, 2},
        {TR, 3},
        {FA, 3}};
    token = lexer.GetToken();
    if (token.token_type == ID)
        return searchList(token.lexeme);

    auto it = primaryTypeMap.find(token.token_type);
    if (it != primaryTypeMap.end())
        return it->second;

    std::cout << "\n Syntax Error \n";
    return 0;
}

// todo: need refactoring
int parse_expression()
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
        auto isTypeMismatch = [tempI, tempI1, tempI2]()
        {
            return (tempI1 != tempI2) || (tempI != 15 && tempI != 16 && tempI != 17 && tempI != 18 &&
                                          tempI != 19 && tempI != 20 && tempI != 21 && tempI != 22 &&
                                          tempI != 23 && tempI != 26);
        };
        if (isTypeMismatch())
        {
            switch (tempI)
            {
            case 15:
            case 16:
            case 17:
            case 18:
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
                    std::cout << "TYPE MISMATCH " << token.line_no << " C2" << std::endl;
                    exit(1);
                }
                break;
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 26:
                if (tempI2 > 3 && tempI1 > 3)
                {
                    typeUpdating(tempI2, tempI1);
                    tempI2 = tempI1;
                    return (3);
                }
                else
                {
                    std::cout << "TYPE MISMATCH " << token.line_no << " C2" << std::endl;
                    exit(1);
                }
                break;

            default:
                std::cout << "TYPE MISMATCH " << token.line_no << " C2" << std::endl;
                exit(1);
            }
        }
        switch (tempI)
        {
        case 19:
        case 20:
        case 21:
        case 23:
        case 26:
            tempI = 3;
            break;
        default:
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
            std::cout << "TYPE MISMATCH " << token.line_no << " C3" << std::endl;
            exit(1);
        }
    }
    else
    {
        std::cout << "\n Syntax Error \n";
        return (0);
    }
    return tempI;
}

void comparingLeft(int line_No, int token_Type)
{
    Node *temp = symbolTable;
    while (temp != NULL)
    {
        if (temp->item->lineNO == line_No)
            temp->item->type = token_Type;
        temp = temp->next;
    }
}

void typeUpdating(int currentType, int newType)
{
    Node *temp = symbolTable;
    while (temp != NULL)
    {
        if (temp->item->type == currentType)
            temp->item->type = newType;
        temp = temp->next;
    }
}

int parse_assstmt()
{

    int leftHS, rightHS;
    token = lexer.GetToken();
    int tempI;
    std::string name;

    if (token.token_type != ID)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    leftHS = searchList(token.lexeme);
    token = lexer.GetToken();
    switch (token.token_type)
    {
    case EQUAL:
        token = lexer.GetToken();
        switch (token.token_type)
        {
        case ID:
        case NUM:
        case REALNUM:
        case TR:
        case FA:
        case PLUS:
        case MINUS:
        case MULT:
        case DIV:
        case LESS:
        case GREATER:
        case GTEQ:
        case LTEQ:
        case EQUAL:
        case NOTEQUAL:
        case NOT:
            lexer.UngetToken(token);
            rightHS = parse_expression();
            if (leftHS == 1 || leftHS == 2 || leftHS == 3)
            {
                if (leftHS != rightHS)
                {
                    if (leftHS < 3)
                    {
                        std::cout << "TYPE MISMATCH " << token.line_no << " C1" << std::endl;
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
            if (token.token_type != SEMICOLON)
            {
                std::cout << "\n HI Syntax Error " << token.token_type << " \n";
            }
            break;

        default:
            std::cout << "\n Syntax Error \n";
            break;
        }
        break;
    default:
        std::cout << "\n Syntax Error \n";
        break;
    }

    return (0);
}

int parse_case()
{
    std::vector<TokenType> expectedTokens;
    expectedTokens.push_back(CASE);
    expectedTokens.push_back(NUM);
    expectedTokens.push_back(COLON);

    std::vector<std::string> errorMessages;
    errorMessages.push_back("\n Syntax Error at CASE expected \n");
    errorMessages.push_back("\n Syntax Error at NUM expected \n");
    errorMessages.push_back("\n Syntax Error at COLON expected \n");

    int tempI = 0;
    TokenType tokenType;
    for (size_t i = 0; i < expectedTokens.size(); ++i)
    {
        token = lexer.GetToken();
        tokenType = token.token_type;
        if (tokenType != expectedTokens[i])
        {
            std::cout << errorMessages[i];
            return 0;
        }
        if (tokenType == COLON)
        {
            tempI = parse_body();
            break;
        }
    }
    return tempI;
}

std::map<TokenType, std::function<void()>> caselistActionMap;
int parse_caselist()
{
    caselistActionMap = {
        {CASE, [&]()
         { lexer.UngetToken(token); parse_case(); }},
        {RBRACE, [&]()
         { lexer.UngetToken(token); throw std::runtime_error("break"); }}};
    try
    {
        while (true)
        {
            token = lexer.GetToken();
            auto action = caselistActionMap.find(token.token_type);
            if (action != caselistActionMap.end())
                action->second();
            else
            {
                std::cout << "\n Syntax Error: Unexpected token in caselist \n";
                break;
            }
        }
    }
    catch (const std::runtime_error &e)
    {
        if (std::string(e.what()) == "break")
            return 0;
        else
            throw;
    }
    return 0;
}

int parse_switchstmt()
{
    token = lexer.GetToken();
    if (token.token_type != SWITCH)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    token = lexer.GetToken();
    if (token.token_type != LPAREN)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    int tempI = parse_expression();
    if (tempI <= 3 && tempI != 1)
    {
        std::cout << "TYPE MISMATCH " << token.line_no << " C5" << std::endl;
        exit(1);
    }

    token = lexer.GetToken();
    if (token.token_type != RPAREN)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    token = lexer.GetToken();
    if (token.token_type != LBRACE)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    tempI = parse_caselist();

    token = lexer.GetToken();
    if (token.token_type != RBRACE)
        std::cout << "\n Syntax Error \n";

    return 0;
}

int parse_whilestmt()
{
    token = lexer.GetToken();
    if (token.token_type != WHILE)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    token = lexer.GetToken();
    if (token.token_type != LPAREN)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    int tempI = parse_expression();
    if (tempI != 3)
    {
        std::cout << "TYPE MISMATCH " << token.line_no << " C4" << std::endl;
        exit(1);
    }

    token = lexer.GetToken();
    if (token.token_type != RPAREN)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    return parse_body();
}

int parse_ifstmt()
{
    token = lexer.GetToken();
    if (token.token_type != IF)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    token = lexer.GetToken();
    if (token.token_type != LPAREN)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    int tempI = parse_expression();
    if (tempI != 3)
    {
        std::cout << "TYPE MISMATCH " << token.line_no << " C4" << std::endl;
        exit(1);
    }

    token = lexer.GetToken();
    if (token.token_type != RPAREN)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    return parse_body();
}

int parse_stmt()
{
    std::map<TokenType, int (*)()> stmtFunctionMap = {
        {ID, parse_assstmt},
        {IF, parse_ifstmt},
        {WHILE, parse_whilestmt},
        {SWITCH, parse_switchstmt}};
    token = lexer.GetToken();
    auto it = stmtFunctionMap.find(token.token_type);
    if (it != stmtFunctionMap.end())
    {
        lexer.UngetToken(token);
        return it->second();
    }
    std::cout << "\n Syntax Error \n";
    return 0;
}

int parse_stmtlist()
{
    std::set<TokenType> validStatementInitiators = {
        ID, IF, WHILE, SWITCH};
    while (true)
    {
        token = lexer.GetToken();
        if (validStatementInitiators.find(token.token_type) == validStatementInitiators.end())
        {
            std::cout << "\n Syntax Error: Invalid token to initiate statement \n";
            break;
        }
        lexer.UngetToken(token);
        parse_stmt();
        token = lexer.GetToken();
        if (token.token_type == RBRACE)
        {
            lexer.UngetToken(token);
            break;
        }
        else if (validStatementInitiators.find(token.token_type) == validStatementInitiators.end())
        {
            std::cout << "\n Syntax Error: Unexpected token after statement \n";
            break;
        }
        lexer.UngetToken(token);
    }
    return 0;
}

int parse_body()
{
    token = lexer.GetToken();
    bool isValid = false;
    if (token.token_type == LBRACE)
    {
        if (parse_stmtlist() == 0)
        {
            token = lexer.GetToken();
            isValid = (token.token_type == RBRACE);
        }
    }
    else if (token.token_type == END_OF_FILE)
    {
        lexer.UngetToken(token);
        isValid = true;
    }
    else
        isValid = false;

    if (!isValid)
        std::cout << "\n Syntax Error \n ";

    return 0;
}

int parse_typename()
{
    std::set<TokenType> validTypes;
    validTypes.insert(INT);
    validTypes.insert(REAL);
    validTypes.insert(BOO);

    token = lexer.GetToken();
    if (validTypes.find(token.token_type) != validTypes.end())
    {
        comparingLeft(token.line_no, token.token_type);
        return 0;
    }

    std::cout << "\n Syntax Error in parse_typename \n";
    return 0;
}

int parse_vardecl()
{
    token = lexer.GetToken();
    if (token.token_type != ID)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    lexer.UngetToken(token);
    int tempI = parse_varlist();

    token = lexer.GetToken();
    if (token.token_type != COLON)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    tempI = parse_typename();

    token = lexer.GetToken();
    if (token.token_type != SEMICOLON)
    {
        std::cout << "\n Syntax Error \n";
        return 0;
    }

    return 0;
}

int parse_vardecllist()
{
    int tempI;
    token = lexer.GetToken();
    for (; token.token_type == ID; token = lexer.GetToken())
    {
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_vardecl();
    }
    tempTokenType = lexer.UngetToken(token);
    return (0);
}

std::string global = "::";
int parse_globalVars()
{
    token = lexer.GetToken();
    std::vector<int> tempIList;
    if (token.token_type != ID)
    {
        std::cout << "Syntax Error";
        return 0;
    }
    tempTokenType = lexer.UngetToken(token);
    tempIList.push_back(parse_vardecllist());
    return 0;
}

std::map<TokenType, std::function<int()>> programParsingMap;
void setupProgramParsingMap()
{
    programParsingMap = {
        {ID, [&]() -> int
         {
             lexer.UngetToken(token);
             parse_globalVars();
             return parse_body();
         }},
        {LBRACE, [&]() -> int
         {
             lexer.UngetToken(token);
             return parse_body();
         }},
        {END_OF_FILE, []() -> int
         {
             return 0; // Normally this might not be called because the loop checks for EOF
         }}};
}
int parse_program()
{
    setupProgramParsingMap(); // Ensure the map is initialized

    while (true)
    {
        token = lexer.GetToken();

        if (token.token_type == END_OF_FILE)
            break; // Exit the loop correctly on EOF

        auto action = programParsingMap.find(token.token_type);
        if (action == programParsingMap.end())
        {

            std::cout << "\n Syntax Error\n";
            return 0;
        }
        int tempI = action->second(); // Execute the function mapped to the token type
    }
    return 0;
}

std::string output = "";

// todo: need refactoring
void printList()
{
    Node *temp = symbolTable;
    int temp1;

    while (temp->next != NULL)
    {
        if (temp->item->type > 3 && temp->item->isPrinted == 0)
        {
            temp1 = temp->item->type;
            output += temp->item->name;
            temp->item->isPrinted = 1;
            Node *lookAhead = temp->next;
            while (temp->next != NULL)
            {
                temp = temp->next;
                if (temp->item->type == temp1)
                {
                    output += ", " + temp->item->name;
                    temp->item->isPrinted = 1;
                }
            }
            output += ": ? #";
            std::cout << output << std::endl;
            temp->item->isPrinted = 1;
            output = "";
            temp = symbolTable;
        }
        else if (temp->item->type < 4 && temp->item->isPrinted == 0)
        {
            std::string lCase = keyword[(temp->item->type) - 1];
            int temp1 = temp->item->type;
            output = temp->item->name + ": " + lCase + " #";
            std::cout << output << std::endl;
            output = "";
            temp->item->isPrinted = 1;

            while (temp->next != NULL && temp->next->item->type == temp1)
            {
                temp = temp->next;
                std::string lCase2 = keyword[(temp->item->type) - 1];
                output = temp->item->name + ": " + lCase2 + " #";
                std::cout << output << std::endl;
                temp->item->isPrinted = 1;
                output = "";
            }
        }
        else
            temp = temp->next;
    }
    if (temp->item->type <= 3 && temp->item->isPrinted == 0)
    {
        std::string lCase3 = keyword[(temp->item->type) - 1];
        output += temp->item->name + ": " + lCase3 + " #";
        std::cout << output << std::endl;
        output = "";
    }
    else if (temp->item->type > 3 && temp->item->isPrinted == 0)
    {
        output += temp->item->name + ":" + " ? " + "#";
        std::cout << output << std::endl;
        output = "";
    }
}

char null[] = "NULL";
int main()
{
    int input;
    input = parse_program();
    printList();
}