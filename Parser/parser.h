/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string> // Include the string header
#include <set>
#include "lexer.h"
//Yosef Pineda CSE 340 :(
class Parser {
  public:
    
    void parse_input();
    void parse_inputEnd();
    void match(int **r, std::string s, int **p); // Use std::string and specify the namespace
    Token tokenRecent();
    void readAndPrintAllInput();
    
    struct REG{
    struct REG_node *start;
    struct REG_node *accept;
    };

private:
    
    LexicalAnalyzer lexer;
    void syntax_error();
    Token expect(TokenType expected_type, recentToken);
    void parse_tokens_section();
    void parse_token_list();
    void parse_token();
    //void parse_expr();
     struct REG* parse_expr();
    
};

#endif
