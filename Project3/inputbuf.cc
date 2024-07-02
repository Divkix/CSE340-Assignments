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

bool InputBuffer::EndOfInput()
{
    return input_buffer.empty() && std::cin.eof();
}

char InputBuffer::UngetChar(char c)
{
    if (c != EOF)
        input_buffer.push_back(c);
    return c;
}

void InputBuffer::GetChar(char &c)
{
    if (input_buffer.empty())
    {
        std::cin.get(c);
        return;
    }

    c = input_buffer.back();
    input_buffer.pop_back();
}

std::string InputBuffer::UngetString(std::string s)
{
    int i = 0;
    while (i < s.size())
    {
        input_buffer.push_back(s[s.size() - i - 1]);
        i++;
    }
    return s;
}

char InputBuffer::PeekChar()
{
    char c = input_buffer.empty() ? (std::cin.get(c), c) : input_buffer.back();

    if (!input_buffer.empty() || c != EOF)
    {
        input_buffer.push_back(c);
    }

    return c;
}