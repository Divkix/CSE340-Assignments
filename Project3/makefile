# Compiler
CC = g++

# Compiler flags, using C++20 standard and all warnings
CFLAGS = -std=c++20 -Wall

# Name of the executable
EXEC = a.out

# Source files
SOURCES = inputbuf.cpp lexer.cpp sTable.cpp parser.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Header files
HEADERS = inputbuf.h lexer.hpp sTable.h parser.h

# Default target
all: $(EXEC)

# Rule to link the program
$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule to compile the source files
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean the workspace
clean:
	rm -f $(EXEC) $(OBJECTS) $(OUTPUTS)

.PHONY: all clean run
