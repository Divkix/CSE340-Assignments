/*
 * Created by: Joshua Elkins
 * Date: June 24th, 2023
 */

#include <iostream>
#include <string>
#include "parser.h"
#include "lexer.h"
#include "inputbuf.h"

using namespace std;

// Syntax Error Function.
void syntax_error(){
	cout << "Syntax Error\n";
	exit(1);
}

/*
 * Completed Function.
 * Entry point to the program.
 */
int Parser::parse_program(){
	#ifdef DEBUG
		cout << "Entered Parse Program" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == ID){
		lexer.UngetToken(token);
		parse_globalVars();
		parse_body();
	}
	else if(token.token_type == LBRACE){
		lexer.UngetToken(token);
		parse_body();
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function.
 * Acts as basic entry into the global variable list. 
 */
int Parser::parse_globalVars(){
	#ifdef DEBUG
		cout << "Entered Parse Global Variables" << endl;
	#endif
	parse_vardecllist();

	return 0;
}

/*
 * Completed
 * Loops our variable declaration list. 
 */
int Parser::parse_vardecllist(){
	#ifdef DEBUG
		cout << "Entered Parse Variable Declaration List" << endl;
	#endif
	token = lexer.GetToken();
	while(token.token_type == ID){
		lexer.UngetToken(token);
		parse_vardecl();
		token = lexer.GetToken();
	}
	lexer.UngetToken(token);

	return 0;
}

/*
 * Completed Function
 * Acts as a method to handle the declaration statements.
 */
int Parser::parse_vardecl(){
	#ifdef DEBUG
		cout << "Entered Parse Variable Declaration" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != ID){
		syntax_error();
	}
	lexer.UngetToken(token);
	parse_varlist();
	token = lexer.GetToken();
	if(token.token_type != COLON){
		syntax_error();
	}
	token = lexer.GetToken();
	if(token.token_type == INT || token.token_type == REAL || token.token_type == BOO){
		lexer.UngetToken(token);
		parse_typename();
		token = lexer.GetToken();
		if(token.token_type != SEMICOLON){
			syntax_error();
		}
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Acts as the gathering function for our variables
 */
int Parser::parse_varlist(){
	#ifdef DEBUG
		cout << "Entered Parse Variable List" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != ID){
		syntax_error();
	}
	else{
		Token t2 = lexer.GetToken();
		if(t2.token_type == COMMA){
			while(token.token_type == ID && t2.token_type == COMMA){
				// Gather ID token info here
				token = lexer.GetToken();
				if(token.token_type != ID){
					syntax_error();
				}
				t2 = lexer.GetToken();
			}
			lexer.UngetToken(t2);


		}
		else{
			// Gather singular ID token info here
			lexer.UngetToken(t2);
		}
	}

	return 0;
}

/*
 * Completed Function
 * Just consumes the INT, REAL, or BOO tokens
 */
int Parser::parse_typename(){
	#ifdef DEBUG
		cout << "Entered Parse Type Name" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == INT){
		// We'll be gathering info here
	}
	else if(token.token_type == REAL){
		// We'll be gathering info here
	}
	else if(token.token_type == BOO){
		// We'll be gathering info here
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Acts as the method to consume braces and enter statement list
 */
int Parser::parse_body(){
	#ifdef DEBUG
		cout << "Entered Parse Body" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == LBRACE){
		parse_stmtlist();
		token = lexer.GetToken();
		if(token.token_type != RBRACE){
			syntax_error();
		}
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Acts as our looper to enter all our statements
 */
int Parser::parse_stmtlist(){
	#ifdef DEBUG
		cout << "Entered Parse Statement List" << endl;
	#endif
	token = lexer.GetToken();
	while(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH){
		lexer.UngetToken(token);
		parse_stmt();
		token = lexer.GetToken();
	}
	lexer.UngetToken(token);

	return 0;
}

/*
 * Completed Function
 * Acts as our method to enter the specific statements
 */
int Parser::parse_stmt(){
	#ifdef DEBUG
		cout << "Entered Parse Statement" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == ID){
		lexer.UngetToken(token);
		parse_assstmt();
	}
	else if(token.token_type == IF){
		lexer.UngetToken(token);
		parse_ifstmt();
	}
	else if(token.token_type == WHILE){
		lexer.UngetToken(token);
		parse_whilestmt();
	}
	else if(token.token_type == SWITCH){
		lexer.UngetToken(token);
		parse_switchstmt();
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Function Completed
 * Acts as our assignment statement parser
 */
int Parser::parse_assstmt(){
	#ifdef DEBUG
		cout << "Entered Parse Assignment Statement" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != ID){
		syntax_error();
	}
	// Do something with ID
	token = lexer.GetToken();
	if(token.token_type != EQUAL){
		syntax_error();
	}
	parse_expression();
	token = lexer.GetToken();
	if(token.token_type != SEMICOLON){
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Acts as our expression handling.
 */
int Parser::parse_expression(){
	#ifdef DEBUG
		cout << "Entered Parse Expression" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == NOT){
		lexer.UngetToken(token);
		parse_unaryOperator();
		parse_expression();
	}
	else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT ||  token.token_type == DIV){
		lexer.UngetToken(token);
		parse_binaryOperator();
		parse_expression();
		parse_expression();
	}
	else if(token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL){
		lexer.UngetToken(token);
		parse_binaryOperator();
		parse_expression();
		parse_expression();
	}
	else if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA){
		lexer.UngetToken(token);
		parse_primary();
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Gets our NOT token
 */
int Parser::parse_unaryOperator(){
	#ifdef DEBUG
		cout << "Entered Parse Unary Operator" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != NOT){
		syntax_error();
	}
	//Do something with the NOT

	return 0;
}

/*
 * Completed Function
 * Acts as our binary handler
 */
int Parser::parse_binaryOperator(){
	#ifdef DEBUG
		cout << "Entered Binary Operator" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT ||  token.token_type == DIV){
		// Do something with these Tokens
	}
	else if(token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL){
		// Do something with these Tokens
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Acts as our primary handler
 */
int Parser::parse_primary(){
	#ifdef DEBUG
		cout << "Entered Parse Primary" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA){
		// Do something with these Tokens
	}
	else{
		syntax_error();
	}

	return 0;
}

/*
 * Completed Function
 * Acts as our If Statement handler
 */
int Parser::parse_ifstmt(){
	#ifdef DEBUG
		cout << "Entered Parse If Statement" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != IF){
		syntax_error();
	}
	token = lexer.GetToken();
	if(token.token_type != LPAREN){
		syntax_error();
	}
	parse_expression();
	token = lexer.GetToken();
	if(token.token_type != RPAREN){
		syntax_error();
	}
	parse_body();

	return 0;
}

/*
 * Completed Function
 * Acts as our While Statement handler
 */
int Parser::parse_whilestmt(){
	#ifdef DEBUG
		cout << "Entered Parse While Statement" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != WHILE){
		syntax_error();
	}
	token = lexer.GetToken();
	if(token.token_type != LPAREN){
		syntax_error();
	}
	parse_expression();
	token = lexer.GetToken();
	if(token.token_type != RPAREN){
		syntax_error();
	}
	parse_body();

	return 0;
}

/*
 * Completed Function
 * Acts as out Switch Statement handler
 */
int Parser::parse_switchstmt(){
	#ifdef DEBUG
		cout << "Entered Switch Statement" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != SWITCH){
		syntax_error();
	}
	token = lexer.GetToken();
	if(token.token_type != LPAREN){
		syntax_error();
	}
	parse_expression();
	token = lexer.GetToken();
	if(token.token_type != RPAREN){
		syntax_error();
	}
	token = lexer.GetToken();
	if(token.token_type != LBRACE){
		syntax_error();
	}
	parse_caselist();
	token = lexer.GetToken();
	if(token.token_type != RBRACE){
		syntax_error();
	}

	return 0;
}

/*
 *
 */
int Parser::parse_caselist(){
	#ifdef DEBUG
		cout << "Entered Parse Case List" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type == CASE){
		while(token.token_type == CASE){
			lexer.UngetToken(token);
			parse_case();
			token = lexer.GetToken();
		}
		lexer.UngetToken(token);
	}
	else{
		syntax_error();
	}

	return 0;
}

int Parser::parse_case(){
	#ifdef DEBUG
		cout << "Entered Parse Case" << endl;
	#endif
	token = lexer.GetToken();
	if(token.token_type != CASE){
		syntax_error();
	}
	token = lexer.GetToken();
	if(token.token_type != NUM){
		syntax_error();
	}
	// Do something with this
	token = lexer.GetToken();
	if(token.token_type != COLON){
		syntax_error();
	}
	parse_body();

	return 0;
}

int main(){
	#ifdef DEBUG
		cout << "Entered Main" << endl;
	#endif

	int i;
    	Parser* parseProgram = new Parser();
    	i = parseProgram->parse_program();
    	cout << "\nEnd of Program" << endl;
	return 0;
}
