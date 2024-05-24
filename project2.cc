/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 * Group Member: Ridham Shah
 *               Jalpan Pansuriya
 */
#include <iostream>      // for std::cout
#include <cstdio>        // for printf
#include <cstdlib>       // for exit
#include <vector>        // for std::vector
#include <string>        // for std::string
#include <unordered_set> // for std::unordered_set
#include <unordered_map> // for std::unordered_map
#include <set>           // for std::set
#include <map>           // for std::map
#include "lexer.h"       // for TokenType, Token, LexicalAnalyzer

 // using struct node to store the rules
struct node
{
    std::string left;
    std::vector<std::string> right;
};

// using class Grammar to store the grammar
class Grammar
{
public:
    Grammar();                                    // constructor
    std::vector<node> listKeyRules;               // vector to store the rules
    std::unordered_set<std::string> nonTerminals; // set to store the non-terminals
    std::unordered_set<std::string> terminals;    // set to store the terminals
    std::vector<std::string> allSymbols;          // vector to store all the symbols

private:
    LexicalAnalyzer* lexer;
    // put all void functions here
    void inputParser(), grammarIdentifier(), ruleListIdentifier(), idListIdentifier(), rulesKeIdentifier(), rhsIdentifier(), executionalError();
    Token whatCanIExpect(TokenType token);
};

// global variable to store the grammar
Grammar* gram;

// constructor
Grammar::Grammar()
{
    lexer = new LexicalAnalyzer(); // create a new lexer
    inputParser();                 // parse the input
}

// read grammar
void ReadGrammar()
{
    gram = new Grammar(); // create a new grammar
}

// function to parse the input
void Grammar::inputParser()
{
    grammarIdentifier();         // parse the grammar
    whatCanIExpect(END_OF_FILE); // check if the input has ended
}

// function to handle executional errors
void Grammar::executionalError()
{
    std::cout << "SYNTAX ERROR !!!\n"; // print error message
    exit(1);                           // exit the program
}

// function to parse the grammar
void Grammar::grammarIdentifier()
{
    ruleListIdentifier(); // parse the rule list
    whatCanIExpect(HASH); // check if the input has ended
}

// function to parse the rule list
void Grammar::ruleListIdentifier()
{
    rulesKeIdentifier();      // parse the rules
    Token t = lexer->peek(1); // peek the next token
    if (t.token_type == HASH) // check if the next token is HASH
    {
        return; // stop parsing RULE_LIST
    }
    else
    {
        ruleListIdentifier(); // recurse until HASH is detected.
    }
}

// to be refactored
void Grammar::idListIdentifier()
{
    // Parse the ID List
    Token t = whatCanIExpect(ID);

    // Push to right hand side of the last rule
    listKeyRules.back().right.push_back(t.lexeme);

    // Add it to the allSymbols list if we've never seen it before
    if (!nonTerminals.count(t.lexeme) && !terminals.count(t.lexeme))
    {
        allSymbols.push_back(t.lexeme); // Assume all IDs are terminals by default
        terminals.insert(t.lexeme);     // Assume all IDs are terminals by default
    }

    Token y = lexer->peek(1); // Peek the next token

    // If the next token is STAR, stop parsing ID List
    if (y.token_type == STAR)
    {
        return; // Stop if there are no more ID Lists.
    }
    else
    {
        idListIdentifier(); // Recurse.
    }
}

// function to parse the rules
void Grammar::rulesKeIdentifier()
{
    // Parse the rules
    Token t = whatCanIExpect(ID);

    // Emplace back without trying to capture the return value
    listKeyRules.emplace_back(node{ t.lexeme, {} });

    // Access the newly added rule by directly referencing the last element
    node& newRule = listKeyRules.back();

    // Check if the symbol was previously considered a terminal, adjust if necessary.
    if (terminals.erase(t.lexeme) > 0)
    {
        // If the symbol was previously considered a terminal, adjust if necessary.
        nonTerminals.insert(t.lexeme);
    }
    else if (nonTerminals.insert(t.lexeme).second)
    {
        // If the symbol was not previously considered a terminal, add it to the allSymbols list.
        allSymbols.emplace_back(t.lexeme);
    }

    whatCanIExpect(ARROW); // Check if the next token is ARROW
    rhsIdentifier();       // Parse the right hand side
    whatCanIExpect(STAR);  // Check if the next token is STAR
}

// function to parse the right hand side
void Grammar::rhsIdentifier()
{
    // Parse the right hand side
    if (lexer->peek(1).token_type != STAR)
    {
        idListIdentifier(); // Only parse ID List if the next token is not a STAR.
    }
}

// function to check if the next token is as expected
Token Grammar::whatCanIExpect(TokenType expectedToken)
{
    // Check if the next token is as expected
    Token tok = lexer->GetToken();

    // If the next token is not as expected, handle executional error
    if (tok.token_type != expectedToken)
    {
        // Enhanced error handling could go here.
        // For example, logging expected vs. actual token for easier debugging.
        std::cerr << "Expected token: " << expectedToken << ", but got: " << tok.token_type << std::endl;
        executionalError(); // Assuming this exits the program or throws.
    }

    // Return the token
    return tok;
}

// Task 1
void printTerminalsAndNonTerminals()
{
    // Print terminals
    for (const auto& symbol : gram->allSymbols)
    {
        // Print terminals
        if (gram->terminals.count(symbol))
        {
            // Print the terminal
            std::cout << symbol << " ";
        }
    }

    // Print non-terminals
    for (const auto& symbol : gram->allSymbols)
    {
        // Print non-terminals
        if (gram->nonTerminals.count(symbol))
        {
            // Print the non-terminal
            std::cout << symbol << " ";
        }
    }
}

// Task 2
void RemoveUselessSymbols()
{
    // variables
    std::unordered_set<std::string> terminals = gram->terminals;
    std::unordered_set<std::string> nonTerminals = gram->nonTerminals;

    std::unordered_map<std::string, bool> generatingSymbols; // create a map to check for generating symbols

    std::vector<node> rule_list = gram->listKeyRules;

    // "#" to represent epsilon
    generatingSymbols["#"] = true;

    for (auto it = terminals.begin(); it != terminals.end(); ++it)
    { // all terminals are generating
        generatingSymbols[*it] = true;
    }

    for (auto jt = nonTerminals.begin(); jt != nonTerminals.end(); ++jt)
    { // set all non terminals to false for now.
        generatingSymbols[*jt] = false;
    }

    std::vector<node> RulesGen;           // new vector
    std::set<std::string> updatedSymbols; // new set

    // Initially, any symbols known to be generating could be added to updatedSymbols.
    // For simplicity, let's assume generatingSymbols already contains terminals and '#' marked as true.
    for (const auto& gs : generatingSymbols)
    {
        // If the symbol is generating, add it to the updatedSymbols set
        if (gs.second)
        {
            // Add the symbol to the set of updated symbols
            updatedSymbols.insert(gs.first);
        }
    }

    // Repeat until no new updates are made
    while (!updatedSymbols.empty())
    {
        // Create a new set to store the updated symbols
        std::set<std::string> newUpdates;

        // Iterate over all rules to check for generating symbols
        for (const auto& r : rule_list)
        {
            // Skip if this rule's left side is already known to be generating
            if (generatingSymbols[r.left])
                continue;

            // Check if all symbols on the right-hand side are generating
            bool ruleIsGenerating = true;

            // Iterate over all symbols on the right-hand side
            for (const auto& symbol : r.right)
            {
                // If any symbol is not generating, the rule is not generating
                if (!generatingSymbols[symbol])
                {
                    // Mark the rule as not generating
                    ruleIsGenerating = false;
                    break;
                }
            }

            // If the rule is generating, mark the left-hand side as generating
            if (ruleIsGenerating)
            {
                // Mark the left-hand side as generating
                generatingSymbols[r.left] = true;
                newUpdates.insert(r.left); // Track new updates for next iteration
            }
        }

        updatedSymbols.swap(newUpdates); // Prepare for the next iteration
    }

    // Populate RulesGen based on updated generating symbols
    for (const auto& r : rule_list)
    {
        // Check if all symbols on the right-hand side are generating
        bool rightGen = true;

        // Iterate over all symbols on the right-hand side
        for (const auto& symbol : r.right)
        {
            // If any symbol is not generating, the rule is not generating
            if (!generatingSymbols[symbol])
            {
                // Mark the rule as not generating
                rightGen = false;
                break; // No need to check further
            }
        }

        // If the rule is generating, add it to the list of generating rules
        if (rightGen)
        {
            // Add the rule to the list of generating rules
            RulesGen.push_back(r);
        }
    }

    // Print the useful rules
    std::unordered_map<std::string, bool> reachableSymbols;
    // Mark the start symbol as reachable
    reachableSymbols[rule_list[0].left] = true;

    // Updated reachability loop
    bool change;
    do
    {
        // Assume no changes have been made yet
        change = false;
        for (const auto& rule : RulesGen)
        { // Iterate over rules that are potentially generating
            if (reachableSymbols[rule.left])
            {
                // If the left-hand symbol is reachable
                for (const auto& symbol : rule.right)
                {
                    if (!reachableSymbols[symbol])
                    { // If any right-hand symbol was not reachable before
                        reachableSymbols[symbol] = true;
                        change = true; // Mark change to continue the loop
                    }
                }
            }
        }
    } while (change); // Continue until no more changes

    // Filter for useful rules
    std::vector<node> usefulRules;

    // Iterate over all rules to check for useful rules
    for (const auto& rule : RulesGen)
    {
        if (reachableSymbols[rule.left])
        { // Check if left-hand side is reachable
            bool rightReach = true;
            // Iterate over all symbols on the right-hand side
            for (const auto& symbol : rule.right)
            {
                // If any symbol on the right-hand side is not reachable
                if (!reachableSymbols[symbol])
                {
                    rightReach = false; // If any symbol on the right-hand side is not reachable
                    break;
                }
            }
            // Add rule to usefulRules if all symbols are reachable
            if (rightReach)
            {
                // Add the rule to the list of useful rules
                usefulRules.push_back(rule); // Add rule to usefulRules if all symbols are reachable
            }
        }
    }

    // Printing useful rules
    for (const auto& rule : usefulRules)
    {
        // Print the rule
        std::cout << rule.left << " -> ";
        // Print the right-hand side
        if (rule.right.empty())
        {
            // Print epsilon if RHS is empty
            std::cout << "#";
        }
        else
        {
            // Print the right-hand side
            for (size_t i = 0; i < rule.right.size(); ++i)
            {
                // Print the right-hand side
                std::cout << rule.right[i];

                // Print space if not the last symbol
                if (i != rule.right.size() - 1)
                    std::cout << " ";
            }
        }
        // Print newline
        std::cout << "\n";
    }
}

// Function to calculate the first sets
std::unordered_map<std::string, std::unordered_set<std::string>> algoKaPehlaSet()
{
    // variables
    auto& terminals = gram->terminals;
    auto& nonTerminals = gram->nonTerminals;
    auto& ruleList = gram->listKeyRules;

    // Create a map to store the first sets
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets;

    // Initialize first sets for non-terminals as empty.
    for (const auto& nonTerminal : nonTerminals)
    {
        // Initialize first sets for non-terminals as empty.
        firstSets[nonTerminal] = {};
    }

    // Directly initialize first sets for terminals.
    for (const auto& terminal : terminals)
    {
        // Directly initialize first sets for terminals.
        firstSets[terminal].insert(terminal);
    }

    // Repeat until no more changes are made
    bool changed = true;

    // Repeat until no more changes are made
    while (changed)
    {
        // Assume no changes have been made yet
        changed = false;

        // Iterate over all rules to calculate first sets
        for (const auto& rule : ruleList)
        {
            // Get the first set for the left-hand side
            auto& lhsFirstSet = firstSets[rule.left];
            // Store the initial size of the first set
            size_t initialSize = lhsFirstSet.size();

            // Iterate over all symbols on the right-hand side
            if (rule.right.empty())
            {
                // Add epsilon if RHS is empty
                lhsFirstSet.insert("#");
            }
            else
            {
                // Iterate over all symbols on the right-hand side
                for (size_t j = 0; j < rule.right.size(); ++j)
                {
                    // Iterate over all symbols on the right-hand side
                    const auto& symbol = rule.right[j];

                    // If the symbol is a terminal, add it to the first set
                    if (terminals.count(symbol) > 0)
                    {
                        // If the symbol is a terminal, add it to the first set
                        lhsFirstSet.insert(symbol);
                        break;
                    }
                    else
                    {
                        // If the symbol is a non-terminal, add its first set to the first set of the left-hand side
                        bool hasEpsilon = false;
                        // Iterate over all symbols on the right-hand side
                        for (const auto& firstSymbol : firstSets[symbol])
                        {
                            // If the symbol is a non-terminal, add its first set to the first set of the left-hand side
                            if (firstSymbol != "#")
                            {
                                lhsFirstSet.insert(firstSymbol);
                            }
                            else
                            {
                                hasEpsilon = true;
                            }
                        }
                        // If epsilon is not present, break the loop
                        // else if it is the last symbol, add epsilon to the first set
                        if (!hasEpsilon)
                        {
                            break;
                        }
                        else if (j == rule.right.size() - 1)
                        {
                            lhsFirstSet.insert("#");
                        }
                    }
                }
            }
            // If the size of the first set has changed, mark the change
            if (lhsFirstSet.size() != initialSize)
            {
                // If the size of the first set has changed, mark the change
                changed = true;
            }
        }
    }

    // Return the first sets
    return firstSets;
}

// Task 3
void CalculateFirstSets()
{
    // variables
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets = algoKaPehlaSet(); // map of first sets
    std::unordered_set<std::string> terminals = gram->terminals;
    std::unordered_set<std::string> nonTerminals = gram->nonTerminals;
    std::vector<node> ruleList = gram->listKeyRules;

    std::vector<std::string> allSymbols = gram->allSymbols;

    // Print the first sets
    for (const auto& symbol : allSymbols)
    {
        // Print the first sets
        if (nonTerminals.count(symbol))
        {

            std::cout << "FIRST(" << symbol << ") = {";

            bool isFirst = true; // To manage comma placement

            // If "#" (epsilon) is present in the FIRST set
            if (firstSets[symbol].count("#"))
            {
                std::cout << "#";
                isFirst = false; // After the first element, commas are needed
            }

            // Loop through all symbols to print the rest of the FIRST set
            for (const auto& innerSymbol : allSymbols)
            {
                // If the symbol is in the FIRST set
                if (firstSets[symbol].count(innerSymbol) && innerSymbol != "#")
                {
                    // Print the symbol
                    std::cout << (isFirst ? "" : ", ") << innerSymbol;
                    isFirst = false;
                }
            }

            // Print the closing brace
            std::cout << " }\n";
        }
    }
}

// Task 4
void CalculateFollowSets()
{

    // variables
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets = algoKaPehlaSet(); // map of first sets

    std::unordered_map<std::string, std::unordered_set<std::string>> followSets;
    std::unordered_set<std::string> terminals = gram->terminals;
    std::unordered_set<std::string> nonTerminals = gram->nonTerminals;
    std::vector<std::string> allSymbols = gram->allSymbols;
    std::vector<node> ruleList = gram->listKeyRules;

    // Initialize all follow sets as empty
    for (auto it = nonTerminals.begin(); it != nonTerminals.end(); ++it)
    {
        // Initialize all follow sets as empty
        followSets[*it] = {}; // sets all nonterminals as empty for now
    }

    // Set FOLLOW of first rule as $
    followSets[ruleList[0].left] = { "$" }; // set FOLLOW of first rule as $

    // Repeat until no more changes are made
    bool changed = true;
    while (changed)
    {
        changed = false;
        // Iterate over all rules to calculate follow sets
        for (int i = 0; i < ruleList.size(); i++)
        { // loop through all rules
            for (auto rit = ruleList[i].right.rbegin(); rit != ruleList[i].right.rend(); ++rit)
            {
                // Skip if the symbol is not a non-terminal
                if (nonTerminals.count(*rit))
                {

                    auto& followSetOfJ = followSets[*rit];
                    const int initialSize = followSetOfJ.size();

                    // Insert all elements from FOLLOW set of the LHS into the FOLLOW set of the current symbol
                    const auto& followSetOfLeft = followSets[ruleList[i].left];
                    followSetOfJ.insert(followSetOfLeft.begin(), followSetOfLeft.end());

                    // If the size of the FOLLOW set has changed, mark the change
                    if (initialSize != followSetOfJ.size())
                    {
                        changed = true;
                    }
                }

                // If the current symbol's FIRST set contains epsilon, continue to the next symbol
                // Otherwise, break the loop
                if (!firstSets[*rit].count("#"))
                {
                    break;
                }
            }

            // Iterate over all symbols on the right-hand side
            for (size_t k = 0; k < ruleList[i].right.size(); ++k)
            {
                const auto& symbolK = ruleList[i].right[k];
                // Skip if the symbol is not a non-terminal
                if (!nonTerminals.count(symbolK))
                {
                    continue;
                }

                //  Iterate over all symbols on the right-hand side
                for (size_t l = k + 1; l < ruleList[i].right.size(); ++l)
                {
                    // Skip if the symbol is not a non-terminal
                    const auto& symbolL = ruleList[i].right[l];
                    auto& followSetK = followSets[symbolK];
                    const auto& firstSetL = firstSets[symbolL];

                    size_t initialSize = followSetK.size();
                    bool epsilonPresent = false;

                    // Add everything from the first set of symbol at l to the follow set of symbol at k
                    for (const auto& firstSymbol : firstSetL)
                    {
                        // Add everything from the first set of symbol at l to the follow set of symbol at k
                        if (firstSymbol != "#")
                        {
                            followSetK.insert(firstSymbol);
                        }
                        // If epsilon is present, mark it
                        else
                        {
                            epsilonPresent = true;
                        }
                    }

                    size_t finalSize = followSetK.size();
                    // If the size of the FOLLOW set has changed, mark the change
                    if (initialSize != finalSize)
                    {
                        changed = true;
                    }

                    // Break the loop if epsilon is not present in the first set of the current symbol
                    if (!epsilonPresent)
                    {
                        break;
                    }
                }
            }
        }
    }

    // Print the follow sets
    for (const auto& symbol : allSymbols)
    {
        // Print the follow sets
        if (nonTerminals.count(symbol))
        {
            std::cout << "FOLLOW(" << symbol << ") = {";

            bool isFirst = true; // Flag to manage comma placement in output

            // Directly check and print if "$" is in the FOLLOW set
            if (followSets[symbol].count("$"))
            {
                std::cout << "$";
                isFirst = false;
            }

            // Iterate through all symbols to build the rest of the FOLLOW set
            for (const auto& followSymbol : allSymbols)
            {
                // Iterate through all symbols to build the rest of the FOLLOW set
                if (followSets[symbol].count(followSymbol) && followSymbol != "$")
                {
                    std::cout << (isFirst ? "" : ", ") << followSymbol;
                    isFirst = false; // After printing the first element, commas are needed
                }
            }

            std::cout << " }\n";
        }
    }
}

// Task 5
void CheckIfGrammarHasPredictiveParser()
{
    // Get the first and follow sets
    std::unordered_map<std::string, std::unordered_set<std::string>> firstSets = algoKaPehlaSet();
    std::unordered_map<std::string, std::unordered_set<std::string>> followSets; // You need to implement a function to calculate follow sets

    // Iterate over all non-terminals
    for (const auto& nonTerminal : gram->nonTerminals)
    {
        // Get the first and follow sets for the non-terminal
        const auto& firstSet = firstSets[nonTerminal];
        const auto& followSet = followSets[nonTerminal];

        // Calculate the intersection of the first and follow sets
        std::unordered_set<std::string> intersection;
        for (const auto& symbol : firstSet)
        {
            if (followSet.count(symbol) > 0)
            {
                intersection.insert(symbol);
            }
        }

        // If the intersection is not empty, the grammar does not have a predictive parser
        if (!intersection.empty())
        {
            return;
        }
    }

    // If we reach here, the grammar has a predictive parser

    std::cout << "NO";
}

int main(int argc, char* argv[])
{
    int task;

    if (argc < 2)
    {
        std::cout << "Error: missing argument\n";
        return 1;
    }

    /*
       Note that by convention argv[0] is the name of your executable,
       and the first argument to your program is stored in argv[1]
     */

    task = atoi(argv[1]);

    ReadGrammar(); // Reads the input grammar from standard input
    // and represent it internally in data structures
    // ad described in project 2 presentation file

    switch (task)
    {
    case 1:
        printTerminalsAndNonTerminals();
        break;

    case 2:
        RemoveUselessSymbols();
        break;

    case 3:
        CalculateFirstSets();
        break;

    case 4:
        CalculateFollowSets();
        break;

    case 5:
        CheckIfGrammarHasPredictiveParser();
        break;

    default:
        std::cout << "Error: unrecognized task number " << task << "\n";
        break;
    }
    return 0;
}