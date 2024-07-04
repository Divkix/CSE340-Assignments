//
// Name: Divanshu Chauhan
//
// Date: 07/03/2024
//
//
// Description: This is project 3 for cse340 summer 24.
//              This file contains the class definition for the InputBuffer class.
//
/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */

#ifndef __INPUT_BUFFER__H__
#define __INPUT_BUFFER__H__

#include <string>

class InputBuffer
{
public:
    char UngetChar(char);
    bool EndOfInput();
    void GetChar(char &);

private:
    std::vector<char> input_buffer;
};

#endif //__INPUT_BUFFER__H__
