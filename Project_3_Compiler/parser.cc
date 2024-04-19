#include <iostream>
#include "parser.h"
#include "lexer.h"

/*Christopher Harris*/

using namespace std;

void Parser::syntax_error() {
	cout << "SYNTAX ERROR !!!\n";
	exit(1);
}

Token Parser::expect(TokenType expected_type){
	Token t = lexer.GetToken();
	if (t.token_type != expected_type)
		syntax_error();
	return t;
}

void Parser::parse_program() {
	parse_var_sction();
	parse_body();
	parse_inputs();

	if (lexer.peek(1).token_type != END_OF_FILE)
		syntax_error();
}

void Parser::parse_var_sction() {
	parse_id_list();

	Token t = lexer.GetToken();
	if (t.token_type != SEMICOLON)
		syntax_error();
}

void Parser::parse_id_list() {
	Token t = lexer.GetToken();
	if (t.token_type != ID)
		syntax_error();

	if (lexer.peek(1).token_type == COMMA) {
		t = lexer.GetToken();
		parse_id_list();
	}
}

void Parser::parse_body() {
	Token t = lexer.GetToken();
	// if (t.token_type != LBRACE)
	// 	syntax_error();

	// parse_stmt_list();

	// t = lexer.GetToken();
	// if (t.token_type != RBRACE)
	// 	syntax_error();
	expect(LBRACE);
	parse_stmt_list();
	t = lexer.GetToken();
	expect(RBRACE);
}

void Parser::parse_stmt_list() {

	parse_stmt();

	Token t = lexer.peek(1);
	if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT)
		parse_stmt_list();
}

void Parser::parse_stmt() {
	if (lexer.peek(1).token_type == ID)
		parse_assign_stmt();
	else if (lexer.peek(1).token_type == WHILE)
		parse_while_stmt();
	else if (lexer.peek(1).token_type == IF)
		parse_if_stmt();
	else if (lexer.peek(1).token_type == SWITCH)
		parse_switch_stmt();
	else if (lexer.peek(1).token_type == FOR)
		parse_for_stmt();
	else if (lexer.peek(1).token_type == OUTPUT)
		parse_output_stmt();
	else if (lexer.peek(1).token_type == INPUT)
		parse_input_stmt();
	else
		syntax_error();
}

void Parser::parse_assign_stmt() {
	Token t = lexer.peek(1);
	if (t.token_type != ID)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != EQUAL)
		syntax_error();

	t = lexer.GetToken();
	if (lexer.peek(1).token_type == PLUS || lexer.peek(1).token_type == MINUS || lexer.peek(1).token_type == MULT || lexer.peek(1).token_type == DIV)
	{
		
		parse_expr();
	}else{
		
		parse_primary();
	}

	t = lexer.GetToken();
	if (t.token_type != SEMICOLON)
		syntax_error();
}

void Parser::parse_expr() {
	parse_primary();
	parse_op();
	parse_primary();
}

void Parser::parse_primary() {
	Token t = lexer.GetToken();
	if (t.token_type == ID || t.token_type == NUM)
		return;
	else
		syntax_error();
}

void Parser::parse_op() {
	Token t = lexer.GetToken();
	if (t.token_type == PLUS || t.token_type == MINUS || t.token_type == MULT || t.token_type == DIV)
		return;
	else
		syntax_error();
}

void Parser::parse_output_stmt() {
	Token t = lexer.GetToken();
	if (t.token_type != OUTPUT)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != ID)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != SEMICOLON)
		syntax_error();
}

void Parser::parse_input_stmt() {
	Token t = lexer.GetToken();
	if (t.token_type != INPUT)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != ID)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != SEMICOLON)
		syntax_error();
}

void Parser::parse_while_stmt() {
	Token t = lexer.GetToken();
	if (t.token_type != WHILE)
		syntax_error();

	parse_condition();
	parse_body();
}

void Parser::parse_if_stmt() {
	Token t = lexer.GetToken();
	if (t.token_type != IF)
		syntax_error();

	parse_condition();
	parse_body();
}

void Parser::parse_condition() {
	parse_primary();
	parse_relop();
	parse_primary();
}

void Parser::parse_relop() {
	Token t = lexer.GetToken();
	if (t.token_type == GREATER)
		return;
	else if (t.token_type == LESS)
		return;
	else if (t.token_type == NOTEQUAL)
		return;
	else
		syntax_error();
}

void Parser::parse_switch_stmt() {
	Token t = lexer.GetToken();
	if (t.token_type != SWITCH)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != ID)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != LBRACE)
		syntax_error();

	parse_case();

	if (lexer.peek(1).token_type == DEFAULT)
		parse_default_case();

	t = lexer.GetToken();
	if (t.token_type != RBRACE)
		syntax_error();
}

void Parser::parse_for_stmt() {
	Token t = lexer.GetToken();
	if (t.token_type != FOR)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != LPAREN)
		syntax_error();

	parse_assign_stmt();
	parse_condition();

	t = lexer.GetToken();
	if (t.token_type = SEMICOLON)
		syntax_error();

	parse_assign_stmt();

	t = lexer.GetToken();
	if (t.token_type != RPAREN)
		syntax_error();

	parse_body();
}

void Parser::parse_case_list() {
	parse_case();

	if (lexer.peek(1).token_type == CASE)
		parse_case_list();
}

void Parser::parse_case() {
	Token t = lexer.GetToken();
	if (t.token_type != CASE)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != NUM)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != COLON)
		syntax_error();

	parse_body();
}

void Parser::parse_default_case() {
	Token t = lexer.GetToken();
	if (t.token_type != DEFAULT)
		syntax_error();

	t = lexer.GetToken();
	if (t.token_type != COLON)
		syntax_error();

	parse_body();
}

void Parser::parse_inputs() {
	parse_num_list();
}

void Parser::parse_num_list() {
	Token t = lexer.GetToken();
	if (t.token_type != NUM)
		syntax_error();

	if (lexer.peek(1).token_type == NUM)
		parse_num_list();
}