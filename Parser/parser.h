// Christopher Harris
// CSE 340

#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include <set>
#include "lexer.h"

class Parser {
public:
    void parse_input();
    void parse_Input();
    void parse_inputEnd();
    static std::vector <std::string> semanticError;
    static std::vector <Token> tokenList;
    void readAndPrintAllInput();
    void parse_token();
    
    //Declaration of the REG data structure
    struct REG_node{
    struct REG_node *first_neighbor; 
    char first_label;
    struct REG_node *second_neighbor;
    char second_label;

    REG_node(REG_node* fn = nullptr, char fl = ' ', REG_node* sn = nullptr, char sl = ' '): first_neighbor(fn), first_label(fl), second_neighbor(sn), second_label(sl){}
    };

    struct REG {
    struct REG_node *start; // Start node of the regular expression graph
    struct REG_node *accept; // Accept node of the regular expression graph
    REG(REG_node* s = nullptr, REG_node* a = nullptr): start(s), accept(a) {} // Constructor
};

// Function to create a regular expression graph with a single label
REG* creationREG(char label);

// Function to concatenate two regular expressions
REG* dotREG(REG* R1, REG* R2);

// Function to apply the Kleene star operation to a regular expression
REG* KleeneREG(REG* R_itself);

// Function to create a union of two regular expressions
REG* unionREG(REG* R1_union, REG* R2_union);

// Function to parse an expression and return the corresponding regular expression graph
REG* parse_expr(std::string tokenPrevious);

private:
    LexicalAnalyzer lexer; // Lexer to tokenize the input

    // Function to handle syntax errors
    void syntax_error();

    // Overloaded function to handle syntax errors with a specific message
    void syntax_error(std::string);

    void syntax_errorEpsilon(std::string);

    // Function to expect a token of a certain type
    Token expect(TokenType expected_type);

    // Overloaded function to expect a token of a certain type and lexeme
    Token expect(TokenType expected_type,std::string);

    // Function to parse the tokens section of the input
    void parse_tokens_section();

    // Function to parse a list of tokens
    void parse_token_list();
};

#endif
