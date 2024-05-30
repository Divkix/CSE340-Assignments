//
// Name: Divanshu Chauhan
// ASU ID: 1224807311
// Date: 5/29/2024
//

// Gyllian Gaylor 1214978271

// lexer.cc file

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

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

    // newly added scanning variables
    char scanVariable, scanVariable2, scanVariable3;
    int flag0 = 0, flag8 = 0;
    char arr[100];
    input.GetChar(c);
    if (isdigit(c))
    {
        if (c != '0')
        {
            tmp.lexeme = ""; // else operation
            while (!input.EndOfInput() && isdigit(c))
            {
                tmp.lexeme += c;
                input.GetChar(c);
                if (c == '8' || c == '9')
                {
                    flag8 = 1;
                }
            }
            if (!input.EndOfInput())
            {
                input.UngetChar(c);
            }
        }
        else
        {
            tmp.lexeme = "0";
            flag0 = 1;
        }
        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        input.GetChar(scanVariable);
        if (scanVariable == '.')
        {
            int flagnotzero = 0;
            string newstring = "";
            input.GetChar(scanVariable2);
            if (isdigit(scanVariable2))
            {
                while (!input.EndOfInput() && isdigit(scanVariable2))
                {
                    if (scanVariable2 != '0')
                    {
                        flagnotzero = 1;
                    }
                    newstring += scanVariable2;
                    input.GetChar(scanVariable2);
                }
                if (!input.EndOfInput())
                {
                    input.UngetChar(scanVariable2);
                }
            }
            else
            {
                input.UngetChar(scanVariable2);
            }
            // flags respectively 0 and 1
            if (flagnotzero == 0 && flag0 == 1)
            {
                // ungets newstring after use
                input.UngetString(newstring);
            }
            else
            {
                input.UngetString(newstring);
                input.GetChar(scanVariable2);
                if (isdigit(scanVariable2))
                {
                    tmp.lexeme += '.';
                    while (!input.EndOfInput() && (isdigit(scanVariable2)))
                    {
                        tmp.lexeme += scanVariable2;
                        input.GetChar(scanVariable2);
                    }
                    if (!input.EndOfInput())
                    {
                        input.UngetChar(scanVariable2);
                    }
                    // similar functions are executed as before for token relations
                    tmp.token_type = REALNUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else
                {
                    input.UngetChar(scanVariable2);
                }
            }
            input.UngetChar(scanVariable);
            // x must be x in order to pass the tests
        }
        else if (scanVariable == 'x') /// where is a initiallized
        {
            input.GetChar(scanVariable2);
            if (scanVariable2 == '0')
            {
                input.GetChar(scanVariable3);
                if (scanVariable3 == '8' && flag8 == 0)
                {
                    tmp.lexeme = tmp.lexeme + scanVariable + scanVariable2 + scanVariable3;
                    // the following token commands are executed and the tmp returns
                    tmp.token_type = BASE08NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else
                {
                    input.UngetChar(scanVariable3);
                }
                input.UngetChar(scanVariable2);
            }
            // if newvari is equal to 1 instead of zero and newvaria is equal to six instead of
            else if (scanVariable2 == '1')
            {
                input.GetChar(scanVariable3);
                if (scanVariable3 == '6')
                {
                    tmp.lexeme = tmp.lexeme + scanVariable + scanVariable2 + scanVariable3;
                    tmp.token_type = BASE16NUM;
                    tmp.line_no = line_no;
                    return tmp;
                }
                else
                {
                    input.UngetChar(scanVariable3);
                }
                input.UngetChar(scanVariable2);
            }
            else
            {
                input.UngetChar(scanVariable2);
            }
            input.UngetChar(scanVariable);
        }
        // checking letters that are currently associated with IFS for newVar1
        else if (isSpecifiedChar(scanVariable))
        // this checks for strings that are visible
        {
            int counter = 0;
            int newcount = 0;
            char arr[100];
            // creates and array with elements equal to the current count of 0 (empty)

            arr[counter] = scanVariable;
            while (isdigit(arr[counter]) || isSpecifiedChar(arr[counter]))
            {
                counter++;
                input.GetChar(arr[counter]);
            }
            newcount = counter;
            scanVariable = arr[counter];
            if (scanVariable == 'x')
            {
                input.GetChar(scanVariable2);
                if (scanVariable2 == '1')
                {
                    input.GetChar(scanVariable3);
                    if (scanVariable3 == '6')
                    {
                        for (counter = 0; counter < newcount; counter++)
                        {
                            tmp.lexeme = tmp.lexeme + arr[counter];
                        }
                        tmp.lexeme = tmp.lexeme + scanVariable + scanVariable2 + scanVariable3;
                        tmp.token_type = BASE16NUM;
                        tmp.line_no = line_no;
                        return tmp;
                    }
                    else
                    {
                        input.UngetChar(scanVariable3);
                        input.UngetChar(scanVariable2);
                    }
                }
                else
                {
                    input.UngetChar(scanVariable2);
                }

                input.UngetChar(scanVariable);
            }
            else
            {
                input.UngetChar(scanVariable);
            }
            counter--;
            while (counter > -1)
            {
                input.UngetChar(arr[counter]);
                counter--;
            }
        }
        else
        {
            input.UngetChar(scanVariable);
        }

        tmp.token_type = NUM;
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
