//
// Name: Divanshu Chauhan
// ASU ID: 1224807311
// Date: 5/29/2024
//

// Gyllian Gaylor 1214978271

// lexer.cc file

#include <cctype>
#include <iostream>
#include <istream>
#include <string>
#include <vector>

#include "inputbuf.h"
#include "lexer.h"

using namespace std;

string reserved[] = {
    "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM",
    "BASE16NUM" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = {"IF", "WHILE", "DO", "THEN", "PRINT"};

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
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c))
    {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput())
    {
        input.UngetChar(c);
    }
    return space_encountered;
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

// ----- start changed code ----

// checks for the specified characters
bool isSpecifiedChar(char newVar)
{
    return newVar == 'A' ||
           newVar == 'B' ||
           newVar == 'C' ||
           newVar == 'D' ||
           newVar == 'E' ||
           newVar == 'F';
}

Token LexicalAnalyzer::ScanNumber()
{
    char c;
    // define an array to store the characters
    // setting the size to 999 as the maximum number of characters
    // long enough to store the characters, lower fail the test case
    char storeArr[999];
    // define variables for the new characters
    char scanVar, scanVar2, scanVar3;
    // define flags for the new characters
    int firstFlag = 0, secondFlag = 0;
    // get the first character
    input.GetChar(c);
    // check if the first character is a digit or not
    if (!isdigit(c))
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
    else
    {
        if (c == '0')
        {
            tmp.lexeme = "0";
            firstFlag = 1;
        }
        else
        {
            tmp.lexeme = ""; // else operation
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
                if (c == '8' || c == '9')
                {
                    secondFlag = 1;
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        input.GetChar(scanVar);
        switch (scanVar)
        {
        case '.':
        {
            int flagNotZero = 0;
            string newString = "";
            input.GetChar(scanVar2);
            switch (isdigit(scanVar2))
            {
            case 1: // isdigit(scanVar2) is true
            {
                while (!input.EndOfInput() && isdigit(scanVar2))
                {
                    if (scanVar2 != '0')
                    {
                        flagNotZero = 1;
                    }
                    newString += scanVar2;
                    input.GetChar(scanVar2);
                }
                if (!input.EndOfInput())
                {
                    input.UngetChar(scanVar2);
                }
                break;
            }
            default: // isdigit(scanVar2) is false
            {
                input.UngetChar(scanVar2);
                break;
            }
            }
            switch ((flagNotZero == 0 && firstFlag == 1) ? 1 : 2)
            {
            case 1:
            {
                input.UngetString(newString);
                break;
            }
            case 2:
            {
                input.UngetString(newString);
                input.GetChar(scanVar2);
                if (!isdigit(scanVar2))
                {
                    input.UngetChar(scanVar2);
                }
                else
                {
                    tmp.lexeme += '.';
                    for (; !input.EndOfInput() && isdigit(scanVar2); input.GetChar(scanVar2))
                    {
                        tmp.lexeme += scanVar2;
                    }
                    if (!input.EndOfInput())
                    {
                        input.UngetChar(scanVar2);
                    }
                    tmp.token_type = REALNUM;
                    tmp.line_no = line_no;
                    return tmp;
                }

                break;
            }
            }
            input.UngetChar(scanVar);
            break;
        }
        case 'x':
        {
            input.GetChar(scanVar2);
            switch (scanVar2)
            {
            case '0':
            {
                input.GetChar(scanVar3);
                if (scanVar3 == '8' && secondFlag == 0)
                {
                    tmp.lexeme += scanVar;
                    tmp.lexeme += scanVar2;
                    tmp.lexeme += scanVar3;
                    tmp.token_type = BASE08NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else
                {
                    input.UngetChar(scanVar3);
                }
                input.UngetChar(scanVar2);
                break;
            }
            case '1':
            {
                input.GetChar(scanVar3);
                if (scanVar3 == '6')
                {
                    tmp.lexeme += scanVar;
                    tmp.lexeme += scanVar2;
                    tmp.lexeme += scanVar3;
                    tmp.token_type = BASE16NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else
                {
                    input.UngetChar(scanVar3);
                }
                input.UngetChar(scanVar2);
                break;
            }
            default:
            {
                input.UngetChar(scanVar2);
                break;
            }
            }
            input.UngetChar(scanVar);
            break;
        }
        default:
        {
            if (isSpecifiedChar(scanVar))
            {
                int counter = 0, newCount = 0;
                storeArr[counter] = scanVar;
                do
                {
                    counter = counter + 1;
                    input.GetChar(storeArr[counter]);
                } while (isdigit(storeArr[counter]) || isSpecifiedChar(storeArr[counter]));
                newCount = counter; // change the value of newCount to counter
                scanVar = storeArr[counter];
                switch (scanVar)
                {
                case 'x':
                {
                    input.GetChar(scanVar2);
                    switch (scanVar2)
                    {
                    case '1':
                    {
                        input.GetChar(scanVar3);
                        switch (scanVar3)
                        {
                        case '6':
                        {
                            for (counter = 0; counter < newCount; counter = counter + 1)
                            {
                                tmp.lexeme += storeArr[counter];
                            }
                            tmp.lexeme += scanVar;
                            tmp.lexeme += scanVar2;
                            tmp.lexeme += scanVar3;
                            tmp.token_type = BASE16NUM;
                            tmp.line_no = line_no;
                            return tmp;
                        }
                        default:
                        {
                            input.UngetChar(scanVar3);
                            input.UngetChar(scanVar2);
                            break;
                        }
                        }
                        break;
                    }
                    default:
                    {
                        input.UngetChar(scanVar2);
                        break;
                    }
                    }
                    input.UngetChar(scanVar);
                    break;
                }
                default:
                {
                    input.UngetChar(scanVar);
                    break;
                }
                }
                counter = counter - 1;
                do
                {
                    input.UngetChar(storeArr[counter]);
                    counter = counter - 1;
                } while (counter > -1);
            }
            else
            {
                input.UngetChar(scanVar);
            }
            break;
        }
        }
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;
    }
}
// ----- end changed code ----

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
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
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
    ;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input

    if (!tokens.empty())
    {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);

    switch (c)
    {
    case '.':
        tmp.token_type = DOT;
        return tmp;
    case '+':
        tmp.token_type = PLUS;
        return tmp;
    case '-':
        tmp.token_type = MINUS;
        return tmp;
    case '/':
        tmp.token_type = DIV;
        return tmp;
    case '*':
        tmp.token_type = MULT;
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
    case '[':
        tmp.token_type = LBRAC;
        return tmp;
    case ']':
        tmp.token_type = RBRAC;
        return tmp;
    case '(':
        tmp.token_type = LPAREN;
        return tmp;
    case ')':
        tmp.token_type = RPAREN;
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
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
            tmp.token_type = LESS;
        }
        return tmp;
    case '>':
        input.GetChar(c);
        if (c == '=')
        {
            tmp.token_type = GTEQ;
        }
        else
        {
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
            tmp.token_type = GREATER;
        }
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
            tmp.token_type = END_OF_FILE;
        else
            tmp.token_type = ERROR;

        return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
