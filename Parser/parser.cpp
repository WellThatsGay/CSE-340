/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <cstdlib>
#include <stack>
#include <set>
#include "parser.h"
// Christopher Harris


using namespace std;

//Declaration of the REG data structure
struct REG_node{
    struct REG_node *first_neighbor; //first_neighbor is the first node pointed to by a REG node
    char first_label;
    struct REG_node *second_neighbor;//second_neighbor is the second node pointed to by a REG node
    char second_label;
};
//The REG graph itself is represented as a structure with two pointers to two REG nodes:

/*The first pointer points to the (unique) start node and the second pointer points to the (unique)
accept node. Later, when we discuss the match() function, we will explain what these nodes are
used for.
*/
struct REG{
    struct REG_node *start;
    struct REG_node *accept;
};

// this syntax error function needs to be 
// modified to produce the appropriate message
void Parser::syntax_error()
{

    //OUTSIDE EXPRESSION: 
    //INSIDE EXPRESSION: WE NEED TO RETURN WHERE THE ERROR OCURR
    cout << "SYNTAX ERROR\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi

//THIS FUNCTION NEED TO BE MODIFIED
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

REG_node *node1 = new REG_node(); //Declaring a new node for the REG graph
REG_node* node2 = new REG_node();// Create a new node for the second expression




Parser:: REG * Parser::parse_expr() {
    Token t = lexer.peek(1);
    REG * expression = new REG;
    switch (t.token_type) {
        case CHAR: {

            expect(CHAR,tokenRecent);

            REG_node * charStart = new REG_node;
            REG_node * charEnd = new REG_node;
            charStart->first_neighbor = charEnd;
            charStart->first_label = t.lexeme[CHAR];
            expression->start = charStart;
            expression->accept = charEnd;
            // construct REG for a
            // return REG for a
            // Handle CHAR case
            break;
        }
        case LPAREN: {
            expect(LPAREN);
            REG* expressionLparen = parse_expr();
            expect(RPAREN);
            Token t2 = lexer.peek(1);
            if (t2.token_type == DOT) {
                expect(DOT);  // Consume DOT
                // reg for dot
                expect(LPAREN);
                REG * expressionNext = parse_expr();
                expressionLparen->accept->first_neighbor = expressionNext->start;
                expression->start = expressionLparen->start;
                expression->accept = expressionNext->accept;
                expect(RPAREN);
            }
             else if (t2.token_type == OR) {
                expect(OR); // Consume OR
                // reg for or
                expect(LPAREN);
                REG * expressionNext = parse_expr();
                REG_node * startNode = new REG_node;
                REG_node * acceptNode = new REG_node;
                startNode->first_neighbor = expressionLparen->start;
                startNode->second_neighbor = expressionNext->start;
                expressionLparen->accept->first_neighbor = acceptNode;
                expressionNext->accept->first_neighbor = acceptNode;
                expression->start = startNode;
                expression->accept = acceptNode;
                expect(RPAREN);
            }
else if (t2.token_type == STAR) {
                expect(STAR); // Consume STAR
                REG_node * startNode = new REG_node;
                expression->start = startNode;
                REG_node * node = new REG_node;
                expression->accept = node;
                expression->start->first_neighbor = expression->accept;
                expression->start->second_neighbor = expressionLparen->start;
                expressionLparen->accept->second_neighbor = expressionLparen->start;
                expressionLparen->accept->first_neighbor = expression->accept;

                //reg for dot
            } else {
                syntax_error();
            }
            break;
        }
        case UNDERSCORE:{
            expect(UNDERSCORE,tokenRecent);
            // Handle epsilon case
            REG_node * charStart = new REG_node;
            REG_node * charEnd = new REG_node;
            charStart->first_neighbor = charEnd;
            charStart->first_label = t.lexeme[CHAR];
            expression->start = charStart;
            expression->accept = charEnd;
            break;
        }
        default: {
            syntax_error();
        }
    }
    return expression;

}

//THE CODE BELOW IS BASED ON THE VIDEO FROM THE LECTURE
void Parser::parse_token()
{
   //token -> ID expression "This is the form by new token"
  Token mostRecentToken=expect(ID); //Id : expression

  // parse_expr(mostRecentToken.lexeme);
}
//THE CODE BELOW IS BASED ON THE VIDEO FROM THE LECTURE
void Parser::parse_token_list()
{
    //token_list --> token
    //token_list --> token COMMA token_list
    parse_token();
    Token t = lexer.peek(1);

    if(t.token_type == COMMA){
        expect(COMMA);
        //lexer.GetToken();
        parse_token_list();
    }else if(t.token_type == HASH){
        return;
    }else{
        syntax_error();
    }

    
}

//THE CODE BELOW IS BASED ON THE VIDEO FROM THE LECTURE
void Parser::parse_tokens_section()
{
    //token_segment is equal to token_list HASH
    parse_token_list();
    expect(HASH);
}
//THE CODE BELOW IS BASED ON THE VIDEO FROM THE LECTURE
void Parser::parse_input()
{
    //The input is gonna be the token section INPUT_TEXT
    parse_tokens_section();
    expect(INPUT_TEXT);
    REG* parse_expr();// Parse expression after input text
    
}

//BASED ON THE VIDEO FROM THE LECTURE
void Parser::parse_inputEnd(){
    REG* parse_expr();
    expect(END_OF_FILE);
}
/*void Parser::match(int**r,string s, int** p){ //Could be wrong

}*/

// This function simply reads and prints all tokens
// I included it as an example. You should compile the provided code
// as it is and then run ./a.out < tests/test0.txt to see what this function does
// This function is not needed for your solution and it is only provided to
// illustrate the basic functionality of getToken() and the Token type.

void Parser::readAndPrintAllInput()
{
    Token t;

    // get a token
    t = lexer.GetToken();

    // while end of input is not reached
    while (t.token_type != END_OF_FILE) 
    {
        t.Print();         	// pringt token
        t = lexer.GetToken();	// and get another one
    }
        
    // note that you should use END_OF_FILE and not EOF
}

int main()
{
    // note: the parser class has a lexer object instantiated in it (see file
    // parser.h). You should not be declaring a separate lexer object. 
    // You can access the lexer object in the parser functions as shown in 
    // the example  method Parser::readAndPrintAllInput()
    // If you declare another lexer object, lexical analysis will 
    // not work correctly
    Parser parser;

    parser.readAndPrintAllInput();
	
}
