#ifndef __PARSER__H__
#define __PARSER__H__

#include <stack>

#include "lexer.h"

struct sTableItem
{
    std::string name;
    std::string scope;
    int pubpriv;
};

struct tableStruct
{
    sTableItem item;
    tableStruct *next = NULL;
    tableStruct *prev = NULL;
};

class Parser
{
public:
    void parse_program();
    struct tableStruct *table;

private:
    LexicalAnalyzer lexer;
    std::string currentScope;
    std::stack<std::string> scopes;

    std::string find_scope_by_ID(std::string);
    void parse_global_vars();
    void parse_var_list(int);
    void parse_scope();
    void parse_public_vars();
    void parse_private_vars();
    void parse_stmt_list();
    void parse_stmt();
};

#endif
