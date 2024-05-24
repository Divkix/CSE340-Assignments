
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <vector>
#include "compiler.h"
#include "lexer.h"


using namespace std;

LexicalAnalyzer *lexer = new LexicalAnalyzer();

std::vector<InstructionNode *> instructionList;

std::string address[1000];

InstructionNode *start;
InstructionNode *current;

int indexOfToken(std::string str);

bool isPrimaryToken(int index);    
bool isRelationalOperator(int index);      
bool isArithmeticOperator(int index);         
bool isValidCondition(int index);  
bool isValidExpression(int index); 

std::vector<Token> evaluateCondition();
std::vector<Token> evaluateExpression();
void processOutput();                 
void processInput();                  
void evaluateWhileLoop();                  
void evaluateIfCondition();                     
void processStatementList();          
void evaluateStatement();              
void processProgramBody();                   
void evaluateCaseStatements(Token switchId); 
void evaluateSwitchStatement();                 

ConditionalOperatorType condOpType(std::string op);

void parseFor();                   
void processInputs();                
void allocateMemoryForVariables(); 
void parseVarSection();            
void parseProgram();               

InstructionNode *createConditionalJumpNode(const std::vector<Token> &conditionTokens); 
InstructionNode *createNoopNode();                                                     

ConditionalOperatorType getConditionOperator(TokenType tokenType);

int getOperand2Index(const Token &token);

InstructionNode *createConditionalJumpNode(Token switchId, int op2Index, ConditionalOperatorType condition);

int storeTokenInMemory(Token op2);

void assignMemoryToVariable();

std::vector<Token> processAssignment();

InstructionNode *createAssignNode();

int getIndexOfToken(std::string lexeme);

int getOperandIndex(Token token);

ArithmeticOperatorType getArithmeticOperator(TokenType tokenType);

void linkNodes(InstructionNode *node);


int indexOfToken(std::string target)
{
    int index = 0; 
    while (index < 1000) 
    {
        if (address[index] == target) 
        {
            return index; 
        }
        index++; 
    }
    return -1; 
}

int indexOfTokenMem(int value)
{
    int index = 0; 
    while (index < 1000) 
    {
        if (mem[index] == value) 
        {
            return index; 
        }
        index++; 
    }
    return -1; 
}

int countTokensInMemory(std::string tokenValue)
{
    int count = 0;
    for (int i = 0; i < 1000; i++)
    {
        if (address[i] == tokenValue)
        {
            count++;
        }
    }
    return count; 
}


bool isPrimaryToken(int position) {
    
    TokenType tokenAtPosition = lexer->peek(position).token_type;
    
    
    bool isIdentifier = tokenAtPosition == ID;
    bool isNumber = tokenAtPosition == NUM;
    
    
    return isIdentifier || isNumber;
}


bool isRelationalOperator(int idx) {
    TokenType tokenType = lexer->peek(idx).token_type;
    return tokenType == GREATER || tokenType == LESS || tokenType == NOTEQUAL;
}

bool isArithmeticOperator(int pos) {
    TokenType type = lexer->peek(pos).token_type;
    return type == PLUS || type == MINUS || type == MULT || type == DIV;
}

bool isValidCondition(int idx) {
    int next = idx + 1;
    int nextNext = idx + 2;
    return isPrimaryToken(idx) && isRelationalOperator(next) && isPrimaryToken(nextNext);
}

bool isValidExpression(int idx) {
    int next = idx + 1;
    int nextNext = idx + 2;
    return isPrimaryToken(idx) && isArithmeticOperator(next) && isPrimaryToken(nextNext);
}

bool isAssignmentOperator(int pos) {
    TokenType tokenAtPosition = lexer->peek(pos).token_type;
    return tokenAtPosition == ASSIGN;
}

bool isPotentialArithmeticSequence(int pos) {
    
    if (isPrimaryToken(pos) && isArithmeticOperator(pos + 1) && isPrimaryToken(pos + 2)) {
        return true;
    }
    
    int nextPos = pos + 2;
    while (isArithmeticOperator(nextPos)) {
        nextPos++; 
        if (!isPrimaryToken(nextPos)) return false;
        nextPos++; 
    }
    return true; 
}


std::vector<Token> evaluateCondition() {
    std::vector<Token> conditionTokens;
    int conditionStartIndex = 1;

    while (isValidCondition(conditionStartIndex)) {
        for (int i = 0; i < 3; ++i) { 
            conditionTokens.push_back(lexer->GetToken());
        }
    }
    return conditionTokens;
}


std::vector<Token> evaluateExpression() {
    std::vector<Token> expressionResult; 

    
    while (isValidExpression(1)) {
        for (int i = 0; i < 3; ++i) { 
            expressionResult.push_back(lexer->GetToken());
        }
    }
    return expressionResult; 
}

std::vector<Token> processAssignment() {
    std::vector<Token> tokensCollected; 

    InstructionNode *assignmentNode = createAssignNode(); 

    
    for (int i = 0; i < 3; ++i) {
        tokensCollected.push_back(lexer->GetToken());
    }

    
    assignmentNode->assign_inst.left_hand_side_index = getIndexOfToken(tokensCollected[0].lexeme);
    assignmentNode->assign_inst.operand1_index = getOperandIndex(tokensCollected[2]);

    
    if (lexer->peek(1).token_type != SEMICOLON) {
        for (int i = 0; i < 2; ++i) {
            tokensCollected.push_back(lexer->GetToken());
        }
        assignmentNode->assign_inst.op = getArithmeticOperator(tokensCollected[3].token_type);
        assignmentNode->assign_inst.operand2_index = getOperandIndex(tokensCollected[4]);
    } else {
        assignmentNode->assign_inst.op = OPERATOR_NONE;
    }

    lexer->GetToken(); 

    linkNodes(assignmentNode); 

    return tokensCollected; 
}

InstructionNode *createAssignNode() {
    InstructionNode *assignNode = new InstructionNode(); 
    assignNode->type = ASSIGN; 
    return assignNode; 
}


int getIndexOfToken(std::string lexeme)
{
    return indexOfToken(lexeme); 
}

int getOperandIndex(Token token) {
    
    if (token.token_type != ID) {
        
        int convertedValue = std::stoi(token.lexeme);
        mem[next_available] = convertedValue; 
        address[next_available] = token.lexeme; 
        next_available++; 
    }
    
    return indexOfToken(token.lexeme);
}

bool isNumericLiteral(const std::string& lexeme) {
    if (lexeme.empty()) return false;
    char* ptr;
    strtol(lexeme.c_str(), &ptr, 10);
    return *ptr == '\0';  // Check if the entire string was consumed indicating it's a valid number
}

ArithmeticOperatorType getArithmeticOperator(TokenType tokenType) {
    
    switch (tokenType) {
        case PLUS: return OPERATOR_PLUS;
        case MINUS: return OPERATOR_MINUS;
        case MULT: return OPERATOR_MULT;
        case DIV: return OPERATOR_DIV;
        default: return OPERATOR_NONE;
    }
}


void linkNodes(InstructionNode *newNode) {
    current->next = newNode;  
    newNode->next = nullptr;  
    current = newNode;        
}


void processOutput() {
    lexer->GetToken();  

    InstructionNode *outputNode = new InstructionNode();
    outputNode->type = OUT;
    
    Token outputToken = lexer->GetToken(); 
    outputNode->output_inst.var_index = indexOfToken(outputToken.lexeme); 

    if (current) {  
        current->next = outputNode; 
    }
    outputNode->next = nullptr;  
    current = outputNode;        

    lexer->GetToken(); 
}

void logOutputInstruction(const InstructionNode* node) {
    if (node && node->type == OUT) {
        std::cout << "Output Instruction: Variable index " << node->output_inst.var_index << std::endl;
    } else {
        std::cout << "Attempted to log non-output instruction." << std::endl;
    }
}

void processInput() {
    lexer->GetToken(); 

    InstructionNode *inputNode = new InstructionNode();
    inputNode->type = IN; 

    Token inputToken = lexer->GetToken(); 
    inputNode->input_inst.var_index = indexOfToken(inputToken.lexeme); 

    if (current) { 
        current->next = inputNode; 
    }
    inputNode->next = nullptr; 
    current = inputNode; 

    lexer->GetToken(); 
}

void logInputInstruction(const InstructionNode* node) {
    if (node && node->type == IN) {
        std::cout << "Input Instruction: Variable index " << node->input_inst.var_index << std::endl;
    } else {
        std::cout << "Attempted to log non-input instruction." << std::endl;
    }
}


void evaluateWhileLoop() {
    
    if (lexer->peek(1).token_type != WHILE) {
        return;
    }

    lexer->GetToken(); 
    std::vector<Token> conditionTokens = evaluateCondition(); 

    
    if (conditionTokens.empty()) {
        return;
    }

    
    InstructionNode *conditionalJumpNode = createConditionalJumpNode(conditionTokens);
    if (current != nullptr) {
        current->next = conditionalJumpNode;
    }
    current = conditionalJumpNode; 

    processProgramBody(); 

    
    current->next = conditionalJumpNode;

    
    InstructionNode *noopNode = createNoopNode();
    conditionalJumpNode->cjmp_inst.target = noopNode;
    current = noopNode; 
}

InstructionNode *createConditionalJumpNode(const std::vector<Token> &conditionTokens) {
    InstructionNode *jumpNode = new InstructionNode(); 
    jumpNode->type = CJMP; 

    
    jumpNode->cjmp_inst.operand1_index = indexOfToken(conditionTokens[0].lexeme);
    jumpNode->cjmp_inst.condition_op = getConditionOperator(conditionTokens[1].token_type);
    jumpNode->cjmp_inst.operand2_index = getOperand2Index(conditionTokens[2]);

    return jumpNode; 
}

ConditionalOperatorType getConditionOperator(TokenType tokenType) {
    
    switch (tokenType) {
        case GREATER: return CONDITION_GREATER;   
        case LESS: return CONDITION_LESS;         
        default: return CONDITION_NOTEQUAL;      
    }
}

int getOperand2Index(const Token &token) {
    
    if (token.token_type == ID) {
        return indexOfToken(token.lexeme); 
    }

    int numericValue = std::stoi(token.lexeme); 
    mem[next_available] = numericValue;         
    address[next_available] = token.lexeme;     
    next_available++;                           

    return indexOfToken(token.lexeme); 
}

InstructionNode *createNoopNode() {
    InstructionNode *noopNode = new InstructionNode(); 
    noopNode->type = NOOP; 
    return noopNode; 
}

void logConditionalJumpInstruction(const InstructionNode* node) {
    if (node && node->type == CJMP) {
        std::cout << "Conditional Jump Instruction:" << std::endl;
        std::cout << "  Operand1 Index: " << node->cjmp_inst.operand1_index << std::endl;
        std::cout << "  Condition Operation: " << node->cjmp_inst.condition_op << std::endl;
        std::cout << "  Operand2 Index: " << node->cjmp_inst.operand2_index << std::endl;
    } else {
        std::cout << "Attempted to log non-conditional jump instruction." << std::endl;
    }
}


void evaluateIfCondition() {
    lexer->GetToken();  

    std::vector<Token> conditionTokens = evaluateCondition();  

    if (!conditionTokens.empty()) {  
        InstructionNode *conditionalJumpNode = createConditionalJumpNode(conditionTokens);  
        if (current) {  
            current->next = conditionalJumpNode;
        }
        current = conditionalJumpNode;  

        processProgramBody();  

        
        InstructionNode *noopNode = createNoopNode();
        conditionalJumpNode->cjmp_inst.target = noopNode;  
        current->next = noopNode;  
        current = noopNode;  
    }
}

void processStatementList() {
    TokenType nextToken;
    
    while ((nextToken = lexer->peek(1).token_type) != END_OF_FILE && nextToken != RBRACE) {
        evaluateStatement();  
    }
}


void evaluateStatement()
{
    TokenType typeOfToken = lexer->peek(1).token_type;

    switch (typeOfToken)
    {
    case WHILE:
        evaluateWhileLoop();
        break;
    case IF:
        evaluateIfCondition();
        break;
    case SWITCH:
        evaluateSwitchStatement();
        break;
    case FOR:
        parseFor();
        break;
    case INPUT:
        processInput();
        break;
    case OUTPUT:
        processOutput();
        break;
    default:
        processAssignment();
        break;
    }
}

void processProgramBody()
{
    lexer->GetToken();
    processStatementList();
    lexer->GetToken();
}

void evaluateCaseStatements(Token switchIdentifier)
{
    InstructionNode *switchEndNode = createNoopNode();

    while (lexer->peek(1).token_type != RBRACE)
    {
        if (lexer->peek(1).token_type == DEFAULT)
        {
            lexer->GetToken();         
            lexer->GetToken();         
            processProgramBody();               
            current->next = switchEndNode;
        }
        else 
        {
            lexer->GetToken(); 
            Token caseToken = lexer->GetToken(); 

            int caseTokenIndex = storeTokenInMemory(caseToken);

            lexer->GetToken(); 

            InstructionNode *conditionalNode = createConditionalJumpNode(switchIdentifier, caseTokenIndex, CONDITION_NOTEQUAL);
            current->next = conditionalNode;
            current = conditionalNode;      

            processProgramBody(); 

            InstructionNode *noopNode = createNoopNode();
            conditionalNode->cjmp_inst.target = conditionalNode->next; 
            conditionalNode->next = noopNode;                          
            current->next = switchEndNode;                             
            current = noopNode;                                        
        }
    }

    current = switchEndNode; 
}

int storeTokenInMemory(Token op2)
{
    int op2ValasInt = std::stoi(op2.lexeme);
    mem[next_available] = op2ValasInt;
    address[next_available] = op2.lexeme;
    next_available++;
    return indexOfToken(op2.lexeme);
}

InstructionNode *createConditionalJumpNode(Token switchId, int op2Index, ConditionalOperatorType condition)
{
    InstructionNode *conditionalNode = new InstructionNode();
    conditionalNode->type = CJMP;
    conditionalNode->cjmp_inst.operand1_index = indexOfToken(switchId.lexeme);
    conditionalNode->cjmp_inst.operand2_index = op2Index;
    conditionalNode->cjmp_inst.condition_op = condition;
    return conditionalNode;
}

void evaluateSwitchStatement()
{
    if (lexer->peek(1).token_type == SWITCH && lexer->peek(2).token_type == ID && lexer->peek(3).token_type == LBRACE)
    {
        lexer->GetToken();                 
        Token switchIdentifier = lexer->GetToken(); 
        lexer->GetToken();                  
        evaluateCaseStatements(switchIdentifier);    
        lexer->GetToken();                  
    }
}

void parseFor()
{
    lexer->GetToken(); 
    lexer->GetToken(); 

    processAssignment();                                
    InstructionNode *initialSetup = current;            

    std::vector<Token> conditionExpression = evaluateCondition(); 

    lexer->GetToken(); 

    processAssignment();                                
    InstructionNode *iterationStep = current;           

    lexer->GetToken(); 

    InstructionNode *conditionalNode = createConditionalJumpNode(conditionExpression);
    initialSetup->next = conditionalNode;               
    current = conditionalNode;                          

    processProgramBody(); 

    current->next = iterationStep;                       
    iterationStep->next = conditionalNode;               

    InstructionNode *endLoopNode = createNoopNode();
    conditionalNode->cjmp_inst.target = endLoopNode;     
    current = endLoopNode;                               
}

void addInputValue(LexicalAnalyzer *lexer, std::vector<int> &inputList)
{
    Token token = lexer->GetToken();                 
    int value = std::stoi(token.lexeme);             
    inputList.push_back(value);                      
}


void processInputs()
{
    while (lexer->peek(1).token_type != END_OF_FILE)
    {
        addInputValue(lexer, inputs);
    }
    lexer->GetToken();
}

void allocateMemoryForVariables()
{
    next_available = 0;

    while (lexer->peek(1).token_type != SEMICOLON)
    {
        if (lexer->peek(1).token_type == COMMA)
        {
            lexer->GetToken();
        }
        else
        {
            assignMemoryToVariable();
        }
    }
    lexer->GetToken();
}

void assignMemoryToVariable()
{
    address[next_available] = lexer->GetToken().lexeme;
    mem[next_available] = 0;
    next_available++;
}

void parseVarSection()
{
    allocateMemoryForVariables(); 
}

void parseProgram()
{
    
    InstructionNode *noop = createNoopNode();
    current = noop; 
    start = current;

    parseVarSection(); 

    processProgramBody(); 

    processInputs(); 
}

struct InstructionNode *parse_generate_intermediate_representation()
{
    parseProgram(); 

    return start; 
}