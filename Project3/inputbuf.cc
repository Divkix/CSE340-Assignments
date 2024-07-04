//
// Name: Divanshu Chauhan
//
// Date: 07/03/2024
//
//
// Description: This is project 3 for cse340 summer 24.
//              This file contains the implementation of the InputBuffer class.
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
