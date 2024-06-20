//
// Name: Divanshu Chauhan
//
// Date: 06/19/2024
//
// Description: This file contains the implementation of a parser for a simple programming language.
//

#ifndef __PARSER__H__ // Include guard to prevent multiple inclusions
#define __PARSER__H__

#include <stack> // Include the standard stack container

#include "lexer.h" // Include the lexer header for lexical analysis

// Structure to represent an item in the symbol table
struct sTableItem
{
    std::string name;  // Name of the symbol
    std::string scope; // Scope of the symbol (global, local, etc.)
    int pubpriv;       // Indicates whether the symbol is public or private
};

// Linked list structure for the symbol table
struct tableStruct
{
    sTableItem item;          // The symbol table item
    tableStruct *next = NULL; // Pointer to the next item in the list
    tableStruct *prev = NULL; // Pointer to the previous item in the list
};

// Parser class definition
class Parser
{
public:
    void parse_program();      // Parses the entire program
    struct tableStruct *table; // Pointer to the symbol table

private:
    LexicalAnalyzer lexer;          // Lexer instance for tokenizing input
    std::string currentScope;       // Tracks the current scope during parsing
    std::stack<std::string> scopes; // Stack to manage scope levels

    std::string find_scope_by_ID(std::string); // Finds the scope of a given ID
    void parse_global_vars();                  // Parses global variable declarations
    void parse_var_list(int);                  // Parses a list of variable declarations
    void parse_scope();                        // Parses a scope (global or within a function)
    void parse_public_vars();                  // Parses public variable declarations
    void parse_private_vars();                 // Parses private variable declarations
    void parse_stmt_list();                    // Parses a list of statements
    void parse_stmt();                         // Parses a single statement
};

#endif // End of include guard
