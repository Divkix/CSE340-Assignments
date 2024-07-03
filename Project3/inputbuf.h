#ifndef __INPUT_BUFFER__H__
#define __INPUT_BUFFER__H__

#include <string>
#include <vector>

class InputBuffer
{
public:
<<<<<<< HEAD
    char UngetChar(char);
    char PeekChar();
    bool EndOfInput();
    void GetChar(char &);
    std::string UngetString(std::string);
=======
  void GetChar(char &);
  char UngetChar(char);
  std::string UngetString(std::string);
  bool EndOfInput();
>>>>>>> parent of 98aff49 (this works too w/o comments)

private:
  std::vector<char> input_buffer;
};

#endif //__INPUT_BUFFER__H__
