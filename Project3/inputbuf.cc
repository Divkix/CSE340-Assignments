/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */

#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cstdio>

#include "inputbuf.h"

using namespace std;

bool InputBuffer::EndOfInput()
{
    return input_buffer.empty() && cin.eof();
}

char InputBuffer::UngetChar(char c)
{
    if (c != EOF)
    {
        input_buffer.push_back(c);
    }
    return c;
}

void InputBuffer::GetChar(char &c)
{
    if (input_buffer.empty())
    {
        cin.get(c);
        return;
    }

    c = input_buffer.back();
    input_buffer.pop_back();
}

string InputBuffer::UngetString(string s)
{
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