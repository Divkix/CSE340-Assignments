#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>

#include "inputbuf.h"

using namespace std;

bool InputBuffer::EndOfInput()
{
    if (!input_buffer.empty())
        return false;
    else
        return cin.eof();
}

char InputBuffer::UngetChar(char c)
{
    if (c != EOF)
    {
        input_buffer.push_back(c);
<<<<<<< HEAD
    }
=======
    ;
>>>>>>> parent of 98aff49 (this works too w/o comments)
    return c;
}

void InputBuffer::GetChar(char &c)
{
    if (!input_buffer.empty())
    {
        c = input_buffer.back();
        input_buffer.pop_back();
    }
    else
    {
        cin.get(c);
    }
}

string InputBuffer::UngetString(string s)
{
<<<<<<< HEAD
    for (auto it = s.rbegin(); it != s.rend(); ++it)
    {
        input_buffer.push_back(*it);
    }
    return s;
}

char InputBuffer::PeekChar()
{
    if (input_buffer.empty())
    {
        char c;
        cin.get(c);
        if (c != EOF)
        {
            input_buffer.push_back(c);
        }
        return c;
    }
    else
    {
        return input_buffer.back();
    }
}
=======
    for (int i = 0; i < s.size(); i++)
        input_buffer.push_back(s[s.size() - i - 1]);
    return s;
}
>>>>>>> parent of 98aff49 (this works too w/o comments)
