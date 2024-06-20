//
// Name: Divanshu Chauhan
//
// Date: 06/19/2024
//
// Description: This file contains the implementation of the parser class,
//              which is responsible for parsing the input program and
//              generating the output.

// import files
#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"

// import necessary libraries
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>

// Parses the entire program, starting with global variables and then scopes.
void Parser::parse_program()
{
    currentScope = "::";       // Initialize the current scope to global scope
    scopes.push(currentScope); // Push the global scope onto the scope stack
    Token token = lexer.GetToken();
    if (token.token_type != ID) // Ensure the first token is an identifier
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }

    Token token2 = lexer.GetToken();
    lexer.UngetToken(token2);  // Put the second token back for further processing
    lexer.UngetToken(token);   // Put the first token back to process it in the correct context
    switch (token2.token_type) // Determine the next step based on the second token
    {
    case COMMA:
    case SEMICOLON:
        parse_global_vars(); // Parse global variables
        parse_scope();       // Parse the main scope of the program
        break;
    case LBRACE:
        parse_scope(); // Directly parse the main scope if it starts with a left brace
        break;
    default:
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }
}

// Parses global variables.
void Parser::parse_global_vars()
{
    Token token = lexer.GetToken();
    switch (token.token_type)
    {
    case ID:                     // If the token is an identifier, it's a variable declaration
        lexer.UngetToken(token); // Put the token back to process it in the correct context
        parse_var_list(1);       // Parse the list of variables
        break;
    default:
        lexer.UngetToken(token); // If not an identifier, put the token back and return
        return;
    }

    token = lexer.GetToken();
    if (token.token_type != SEMICOLON) // Ensure the variable declaration ends with a semicolon
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }
}

// Parses a list of variables.
void Parser::parse_var_list(int pubpriv)
{
    Token token = lexer.GetToken();
    if (token.token_type != ID) // Ensure the token is an identifier for a variable name
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }

    Token token2 = lexer.GetToken();
    if (token2.token_type == COMMA) // If there's a comma, there are more variables to parse
        parse_var_list(pubpriv);
    else
        lexer.UngetToken(token2); // If no comma, put the token back

    struct sTableItem newItem;    // Create a new symbol table item for the variable
    newItem.scope = currentScope; // Set the scope of the variable
    newItem.pubpriv = pubpriv;    // Set the visibility of the variable
    newItem.name = token.lexeme;  // Set the name of the variable

    struct tableStruct *newTable = new struct tableStruct; // Create a new table entry
    newTable->item = newItem;                              // Assign the symbol table item to the entry
    newTable->next = table;                                // Link the new entry to the existing table
    table = newTable;                                      // Update the table to point to the new entry
}

// Parses a scope, which can contain variables and statements.
void Parser::parse_scope()
{
    Token token = lexer.GetToken();
    Token token2 = lexer.GetToken();
    if (token.token_type != ID && token2.token_type != LBRACE) // Ensure the scope starts correctly
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }

    scopes.push(currentScope);   // Push the current scope onto the stack
    currentScope = token.lexeme; // Update the current scope to the new scope

    parse_public_vars();  // Parse public variables within the scope
    parse_private_vars(); // Parse private variables within the scope
    parse_stmt_list();    // Parse statements within the scope

    currentScope = scopes.top();     // Revert to the previous scope
    scopes.pop();                    // Remove the previous scope from the stack
    Token token3 = lexer.GetToken(); // Consume the token that marks the end of the scope
}

// Parses public variables within a scope.
void Parser::parse_public_vars()
{
    Token token = lexer.GetToken();
    if (token.token_type != PUBLIC) // Check if the token indicates public variables
    {
        lexer.UngetToken(token); // If not, put the token back and return
        return;
    }
    Token token2 = lexer.GetToken();
    if (token2.token_type != COLON) // Ensure the public declaration is followed by a colon
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }
    parse_var_list(1);               // Parse the list of public variables
    Token token3 = lexer.GetToken(); // Consume the token that ends the public variable declaration
}

// Parses private variables within a scope.
void Parser::parse_private_vars()
{
    Token token = lexer.GetToken();
    if (token.token_type != PRIVATE) // Check if the token indicates private variables
    {
        lexer.UngetToken(token); // If not, put the token back and return
        return;
    }

    Token token2 = lexer.GetToken();
    if (token2.token_type != COLON) // Ensure the private declaration is followed by a colon
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }
    parse_var_list(0);               // Parse the list of private variables
    Token token3 = lexer.GetToken(); // Consume the token that ends the private variable declaration
}

// Parses a list of statements within a scope.
void Parser::parse_stmt_list()
{
    Token token = lexer.GetToken();
    while (token.token_type == ID) // Loop through all statement starting identifiers
    {
        Token token2 = lexer.GetToken();
        lexer.UngetToken(token2); // Put the second token back for further processing
        lexer.UngetToken(token);  // Put the first token back to process it in the correct context

        if (token2.token_type == EQUAL)       // If the statement is an assignment
            parse_stmt();                     // Parse the assignment statement
        else if (token2.token_type == LBRACE) // If the statement starts a new scope
            parse_scope();                    // Parse the new scope
        else
        {
            std::cout << "Syntax Error" << std::endl;
            exit(0);
        }

        token = lexer.GetToken(); // Get the next token to continue or end the loop
    }
    lexer.UngetToken(token); // Put back the token that ended the loop
}

// Finds the scope of an identifier.
std::string Parser::find_scope_by_ID(std::string id)
{
    struct tableStruct *currentCheck = table;                       // Start at the beginning of the symbol table
    for (; currentCheck != NULL; currentCheck = currentCheck->next) // Loop through the symbol table
    {
        if (currentCheck->item.name == id &&                                          // If the name matches and
            (currentCheck->item.scope == currentScope || currentCheck->item.pubpriv)) // it's in the current scope or is public/private
        {
            if (currentCheck->item.scope == "::") // If it's in the global scope
            {
                return "::"; // Return the global scope indicator
            }
            return currentCheck->item.scope + "."; // Return the scope followed by a dot
        }
    }
    return "?.";
}

// Parses a statement, which can be an assignment.
void Parser::parse_stmt()
{
    Token token = lexer.GetToken();
    if (token.token_type != ID) // Ensure the statement starts with an identifier
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }

    Token token2 = lexer.GetToken();
    if (token2.token_type != EQUAL) // Ensure the statement is an assignment
    {
        lexer.UngetToken(token2); // If not, put the token back
        lexer.UngetToken(token);  // Put the first token back and parse it as a scope instead
        parse_scope();
    }

    Token token3 = lexer.GetToken();                                                                                                           // Get the value being assigned
    Token token4 = lexer.GetToken();                                                                                                           // Ensure the statement ends with a semicolon
    if (token3.token_type == ID && token4.token_type == SEMICOLON)                                                                             // If the assignment is valid
        std::cout << find_scope_by_ID(token.lexeme) << token.lexeme << " = " << find_scope_by_ID(token3.lexeme) << token3.lexeme << std::endl; // Print the assignment with scopes
    else
    {
        std::cout << "Syntax Error" << std::endl;
        exit(0);
    }
}

int main()
{
    Parser parser;                           // Create a parser instance
    parser.table = new struct tableStruct(); // Initialize the symbol table
    parser.parse_program();                  // Start parsing the program
    return 0;
}
