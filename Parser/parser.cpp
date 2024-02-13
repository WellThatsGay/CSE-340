// Christopher Harris 
// CSE 340

#include <iostream>
#include <cstdlib>
#include <stack>
#include <set>
#include "parser.h"



using namespace std;

using namespace std;

// Define static members
std::vector <std::string> Parser::semanticError;
std::vector<Token> Parser::tokenList;

// Function to handle syntax errors
void Parser::syntax_error()
{
    cout << "SNYTAX ERORR\n";
    exit(1);
}

// Function to handle syntax errors with a specific message
void Parser::syntax_error(string tokenRecent_lexeme)
{
    cout << tokenRecent_lexeme << " HAS A SYNTAX ERROR IN ITS EXPRESSION\n";
    exit(1);
}

void Parser::syntax_errorEpsilon(string tokenRecent_lexemeEpsilon)
{
    
    cout << "EPSILON IS NOOOOOOOT A TOKEN !!! " << tokenRecent_lexemeEpsilon << endl;
    exit(1);
}

// Function to check if the next token is of the expected type
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// Function to check if the next token is of the expected type and lexeme
Token Parser::expect(TokenType expected_type,string tokenRecent)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error(tokenRecent);
    return t;
}

// Creating the Regular Expression Graph REG
Parser::REG* Parser::creationREG(char label)
{
    // Create start and accept nodes
    REG_node* start = new REG_node();
    REG_node* accept = new REG_node(nullptr, '_');

    // Create a transition from start to accept with the given label
    start->first_neighbor = accept;
    start->first_label = label;

    // Return the new REG object
    return new REG(start, accept);
}

// Function to apply the Kleene star operation to a regular expression
Parser::REG* Parser::KleeneREG(Parser::REG* R_itself)
{
    REG_node* start = new REG_node(R_itself->start, '_', nullptr, '_');
    REG_node* accept = new REG_node();

    // Connect new start and accept nodes, and loop from old accept to old start
    start->second_neighbor = accept;
    R_itself->accept->first_neighbor = accept;
    R_itself->accept->second_neighbor = R_itself->start;
    R_itself->accept->second_label = '_';

    // Return the new REG object
    return new REG(start, accept);
}

//Creation of the union
Parser::REG* Parser::dotREG(Parser::REG* R1, Parser::REG* R2)
{
			    
  R1->accept->first_neighbor = R2->start;
  R1->accept->first_label = '_';
  REG* r = new REG(R1->start, R2->accept);

  return r;
}

// Function to create a union of two regular expressions
Parser::REG* Parser::unionREG(Parser::REG* R1_union, Parser::REG* R2_union)
{
    // Create new start node with transitions to the start nodes of the input REGs
    REG_node* start = new REG_node(R1_union->start, '_', R2_union->start, '_');

    // Create new accept node
    REG_node* accept = new REG_node();

    // Connect the accept nodes of the input REGs to the new accept node
    R1_union->accept->first_neighbor = accept;
    R1_union->accept->first_label = '_';
    R2_union->accept->first_neighbor = accept;
    R2_union->accept->first_label = '_';

    // Return the new REG object
    return new REG(start, accept);
}

//Implementation of the parse_expr function

Parser::REG* Parser::parse_expr(string tokenPrevious){
    
    Token t = lexer.peek(1);

    REG* REGEXPR1; 
    REG* REGEXPR2; 

    Token tt;
    switch(t.token_type) {
        case CHAR: //Base case where we encounter a character
            expect(CHAR,tokenPrevious);
            return creationREG(t.lexeme[0]);

        case UNDERSCORE: //Second base case where we encounter an underscore
            expect(UNDERSCORE,tokenPrevious);
            return creationREG('_');

        case LPAREN: //Third base case where we encounter a left parenthesis
            expect(LPAREN,tokenPrevious);
            REGEXPR1 = parse_expr(tokenPrevious);
            expect(RPAREN,tokenPrevious);
            tt = lexer.peek(1);

            switch(tt.token_type) {
                case DOT:
                    expect(DOT,tokenPrevious);
                    expect(LPAREN,tokenPrevious);
                    REGEXPR2 = parse_expr(tokenPrevious);
                    expect(RPAREN,tokenPrevious);
                    return dotREG(REGEXPR1,REGEXPR2);

                case OR: // Handle OR operator
                    expect(OR,tokenPrevious);
                    expect(LPAREN,tokenPrevious);
                    REGEXPR2 = parse_expr(tokenPrevious);
                    expect(RPAREN,tokenPrevious);
                    return unionREG(REGEXPR1,REGEXPR2); //Return the union of the two expressions

                case STAR:
                    expect(STAR,tokenPrevious);
                    return KleeneREG(REGEXPR1);

                default:
                    syntax_error(tokenPrevious);
            }
            break;

        default:
            syntax_error(tokenPrevious);
    }

    return nullptr;
}
// Function to parse a single token
void Parser::parse_token() {
    // Expect an ID token and store it in lastToken
    Token lastToken = expect(ID);

    // Flag to check if the token already exists in the tokenList
    bool tokenExists = false;

    // Iterate over the tokenList
    for (std::vector<Token>::size_type i = 0; i < tokenList.size(); i++) {
        // If the token already exists in the tokenList, set the flag to true and record an error
        if (tokenList[i].lexeme == lastToken.lexeme) {
            semanticError.push_back("Line " + to_string(lastToken.line_no) + ": " + lastToken.lexeme + " already declared on line " + to_string(tokenList[i].line_no));
            tokenExists = true;
        }
    }

    // If the token doesn't exist in the tokenList, add it
    if (!tokenExists) {
        tokenList.push_back(lastToken);
    }

    
    parse_expr(lastToken.lexeme);
}

// Function to parse a list of tokens
void Parser::parse_token_list()
{
    
    parse_token();
    Token t = lexer.peek(1);

    // If the next token is a comma, expect a comma and recursively parse the rest of the list
    if(t.token_type == COMMA){
        expect(COMMA);
        parse_token_list();
    }
    // If the next token is a hash, return as we've reached the end of the list
    else if(t.token_type == HASH){
        return;
    }
    
    else{
        syntax_error();
    }
}


// Function to parse the tokens section
void Parser::parse_tokens_section()
{
    // Parse the list of tokens and expect a HASH token at the end
    parse_token_list();
    expect(HASH);
}

// Function to parse the entire input
void Parser::parse_Input(){
    // Parse the input and expect an END_OF_FILE token at the end
    parse_input();
    expect(END_OF_FILE);
}

// Function to parse the input
void Parser::parse_input()
{
    // Parse the tokens section and expect an INPUT_TEXT token
    parse_tokens_section();
    expect(INPUT_TEXT);
}

// Function to parse the end of the input
void Parser::parse_inputEnd(){
    // Parse an expression and expect an END_OF_FILE token at the end
    REG* parse_expr();
    expect(END_OF_FILE);
}

// Function to read and print all input tokens
void Parser::readAndPrintAllInput()
{
    Token t;

    // Get the first token
    t = lexer.GetToken();

    // Loop until the end of the input is reached
    while (t.token_type != END_OF_FILE) 
    {
        // Print the current token
        t.Print();
        // Get the next token
        t = lexer.GetToken();
    }
}

// Main function
int main()
{
    // Create a Parser object
    Parser parser;
    
    // Parse the input
    parser.parse_Input();
    
    // Print any semantic errors
    for (const std::string& error : Parser::semanticError) {
        std::cout << error << std::endl;
    }
    
    // Read and print all input tokens
    parser.readAndPrintAllInput();
}
