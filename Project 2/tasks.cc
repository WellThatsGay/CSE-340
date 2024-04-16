/* Christopher Harris*/

#pragma GCC diagnostic ignored "-Wswitch"

// Include necessary libraries and headers
#include <iostream>  // For standard input/output
#include <cstdio>    // For input/output
#include <cstdlib>   // For general purpose functions

// Include custom headers for lexical analysis, execution, and tasks
#include "lexer.h"   
#include "execute.h" 
#include "tasks.h"   

// Include libraries for data structures and memory management
#include <map>       // For map data structure
#include <stack>     // For stack data structure
#include <memory>    // For smart pointers
#include <algorithm> // For algorithm functions
#include <queue>     // For queue data structure


// Use the standard namespace
using namespace std;

// Initialize global variables
string abstractSyntaxTree = "";  // String to hold the abstract syntax tree
string abstractSyntaxTree2 = ""; // Another string to hold the abstract syntax tree

// Create an instance of LexicalAnalyzer for lexical analysis
LexicalAnalyzer lexer;

// Create vectors to hold scalar and array tokens
vector<Token> scalars;
vector<Token> arrays;

// Initialize flags for output statement and access
bool ouputStatement = false;
bool accesOn = false;
// PT --> presedence table
map<TokenType, map<TokenType, string>> PT = {
    {PLUS, {{PLUS, ">"}, {MINUS, ">"}, {MULT, "<"}, {DIV, "<"}, {LPAREN, "<"}, {RPAREN, ">"}, {LBRAC, "<"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, ">"} }},
    {MINUS, {{PLUS, ">"}, {MINUS, ">"}, {MULT, "<"}, {DIV, "<"}, {LPAREN, "<"}, {RPAREN, ">"}, {LBRAC, "<"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, ">"} }},
    {MULT, {{PLUS, ">"}, {MINUS, ">"}, {MULT, ">"}, {DIV, ">"}, {LPAREN, "<"}, {RPAREN, ">"}, {LBRAC, "<"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, ">"} }},
    {DIV, {{PLUS, ">"}, {MINUS, ">"}, {MULT, ">"}, {DIV, ">"}, {LPAREN, "<"}, {RPAREN, ">"}, {LBRAC, "<"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, ">"} }},
    {LPAREN, {{PLUS, "<"}, {MINUS, "<"}, {MULT, "<"}, {DIV, "<"}, {LPAREN, "<"}, {RPAREN, "="}, {LBRAC, "<"}, {DOT, "err"}, {RBRAC, "<"}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, "err"} }},
    {RPAREN, {{PLUS, ">"}, {MINUS, ">"}, {MULT, ">"}, {DIV, ">"}, {LPAREN, "err"}, {RPAREN, ">"}, {LBRAC, ">"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "err"}, {ID, "err"}, {END_OF_FILE, ">"} }},
    {LBRAC, {{PLUS, "<"}, {MINUS, "<"}, {MULT, "<"}, {DIV, "<"}, {LPAREN, "<"}, {RPAREN, "<"}, {LBRAC, "<"}, {DOT, "="}, {RBRAC, "="}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, "err"} }},
    {DOT, {{PLUS, "err"}, {MINUS, "err"}, {MULT, "err"}, {DIV, "err"}, {LPAREN, "err"}, {RPAREN, "err"}, {LBRAC, "err"}, {DOT, "err"}, {RBRAC, "="}, {NUM, "err"}, {ID, "err"}, {END_OF_FILE, "err"} }},
    {RBRAC, {{PLUS, ">"}, {MINUS, ">"}, {MULT, ">"}, {DIV, ">"}, {LPAREN, "err"}, {RPAREN, ">"}, {LBRAC, ">"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "err"}, {ID, "err"}, {END_OF_FILE, ">"} }},
    {NUM, {{PLUS, ">"}, {MINUS, ">"}, {MULT, ">"}, {DIV, ">"}, {LPAREN, "err"}, {RPAREN, ">"}, {LBRAC, ">"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "err"}, {ID, "err"}, {END_OF_FILE, ">"} }},
    {ID, {{PLUS, ">"}, {MINUS, ">"}, {MULT, ">"}, {DIV, ">"}, {LPAREN, "err"}, {RPAREN, ">"}, {LBRAC, ">"}, {DOT, "err"}, {RBRAC, ">"}, {NUM, "err"}, {ID, "err"}, {END_OF_FILE, ">"} }},
    {END_OF_FILE, {{PLUS, "<"}, {MINUS, "<"}, {MULT, "<"}, {DIV, "<"}, {LPAREN, "<"}, {RPAREN, "err"}, {LBRAC, "<"}, {DOT, "err"}, {RBRAC, "err"}, {NUM, "<"}, {ID, "<"}, {END_OF_FILE, "acc"} }},
};


// Function to check if the next token matches the expected type
Token expect(TokenType expected_type) {
    // Get the next token
    Token t = lexer.GetToken();

    // If the token type does not match the expected type, throw a syntax error
    if (t.token_type != expected_type) {
        syntax_error();
    }

    // Return the token
    return t;
}

// Function to peek at the next symbol
Token peek_symbol(int t) {
    // If the next token is a semicolon, return an END_OF_FILE token
    if(lexer.peek(1).token_type == SEMICOLON){
        Token inputToken;
        inputToken.token_type = END_OF_FILE;
        return inputToken;
    }
    // If the next two tokens are RBRAC and EQUAL, and accesOn is false, return an END_OF_FILE token
    else if(lexer.peek(1).token_type == RBRAC && lexer.peek(2).token_type == EQUAL && accesOn == false){
        Token inputToken;
        inputToken.token_type = END_OF_FILE;
        return inputToken;
    }
    // If the next two tokens are RBRAC and SEMICOLON, and ouputStatement is true, return an END_OF_FILE token
    else if(lexer.peek(1).token_type == RBRAC && lexer.peek(2).token_type == SEMICOLON && ouputStatement == true){
        Token inputToken;
        inputToken.token_type = END_OF_FILE;
        return inputToken;
    }
    // If the next token is EQUAL and accesOn is true, return an END_OF_FILE token
    else if (lexer.peek(1).token_type == EQUAL && accesOn == true) {
        Token inputToken;
        inputToken.token_type = END_OF_FILE;
        return inputToken;
    }
    // Otherwise, return the next token
    else{
        return lexer.peek(1);
    }
}

Token get_symbol() {
    Token tok = lexer.peek(1);
    Token tok2 = lexer.peek(2);
    // (tok.token_type == RBRAC && tok2.token_type == EQUAL) ||
    if (tok.token_type == SEMICOLON || (tok.token_type == RBRAC && tok2.token_type == SEMICOLON && ouputStatement == true) || (tok.token_type == RBRAC && tok2.token_type == EQUAL && accesOn == false) || (tok.token_type == EQUAL && accesOn == true)) {
        Token newtok;
        newtok.token_type = END_OF_FILE;
    } else {
        // lexer.peek(1).Print();
        return lexer.GetToken();
    }
    // Return a default Token if none of the conditions are met.
    Token defaultToken;
    defaultToken.token_type = ERROR; // Assuming ERROR is a defined TokenType for errors.
    return defaultToken;
}

void syntax_error() {
    cout << "SNYATX EORRR !!!" << endl;
    exit(1);
}

void parse_primary() {
    Token t = lexer.peek(1);
    if (t.token_type == ID) {
        expect(ID);
    } else if (t.token_type == NUM) {
        expect(NUM);
    }
}

exprNode* parse_expr() {
    vector<stackNode> main_stack;
    accesOn = false;
    // cout<<"parsing expression"<<endl;
    Token newToken = Token();
    exprNode *returnNode = new exprNode;
    vector<stackNode> nextRHS;
    newToken.token_type = END_OF_FILE;
    stackNode *s = new stackNode;
    s->type = TERM;
    s->term = newToken;
    main_stack.push_back(*s);
    while (!(lexer.peek(1).token_type == END_OF_FILE && main_stack[main_stack.size() - 1].term.token_type ==  END_OF_FILE)) {
        
        Token t  = peek_symbol(1);
        TokenType c = t.token_type;
        int n =1;
        while (main_stack[main_stack.size() - n].type != TERM) {
            n++;
        }
        TokenType a = main_stack[main_stack.size() - n].term.token_type;
        
        if (PT[a][c] == "<" || PT[a][c] == "=") {
            Token token = get_symbol();
            
            
            stackNode *newToken = new stackNode;
            newToken->type = TERM;
            newToken->term = token;

            main_stack.push_back(*newToken);
        } else if (PT[a][c] == ">") {
            // cout<<"reducing"<<endl;
            bool termpopped = false;
            stackNode lastPoppedTerminal;
            while((main_stack.back().type == TERM && termpopped == true && PT[main_stack.back().term.token_type][lastPoppedTerminal.term.token_type] == "<") != true) {
                // cout<<"popping"<<endl;
                stackNode s = main_stack.back();
                main_stack.pop_back();
                //stackNode *lastPoppedTerminal = new stackNode;
                if (s.type  == TERM) {
                    lastPoppedTerminal = s;
                    termpopped = true;
                }
                // s.term.Print();
                nextRHS.push_back(s);
            }
            reverse(nextRHS.begin(), nextRHS.end());
            // cout<<"going to expressions"<<endl;
            // cout<<"candidate rhs size: " + nextRHS.size() << endl;
            vector<string> expressions = {"EXPR PLUS EXPR", "EXPR MINUS EXPR", "EXPR MULT EXPR", "EXPR DIV EXPR", "LPAREN EXPR RPAREN", "EXPR LBRAC EXPR RBRAC", "EXPR LBRAC DOT RBRAC", "ID", "NUM"};
            string expressionBuilt;
            for (size_t i = 0; i < nextRHS.size(); i++) {
                if (nextRHS[i].type == EXPR) {
                    expressionBuilt += " EXPR";
                } else if  (nextRHS[i].type == TERM) {
                    switch(nextRHS[i].term.token_type) {
                        case PLUS:
                            expressionBuilt += " PLUS"; break;
                        case MINUS:
                            expressionBuilt += " MINUS"; break;
                        case MULT:
                            expressionBuilt += " MULT";  break;
                        case DIV:
                            expressionBuilt += " DIV";   break;
                        case LPAREN:
                            expressionBuilt += " LPAREN";break;
                        case RPAREN:
                            expressionBuilt += " RPAREN"; break;
                        case LBRAC:
                            expressionBuilt  += " LBRAC"; break;
                        case RBRAC:
                            expressionBuilt += " RBRAC"; break;
                        case DOT:
                            expressionBuilt += " DOT"; break;
                        case ID:
                            expressionBuilt += " ID";  break;
                        case NUM:
                            expressionBuilt += " NUM";  break;
                        default:
                            syntax_error();
                    }
                }
            }
            bool match = false;
            // cout<<expressionBuilt<<endl;
            string finalExpression = expressionBuilt.substr(1);
            for (size_t i =0; i < expressions.size(); i++) {
                
                if (finalExpression  == expressions[i]) {
                    match = true;
                    if (finalExpression == "NUM") {
                        stackNode *n = new stackNode;
                        n->type = EXPR;
                        exprNode *expressionNode = new exprNode;
                        expressionNode->exprOperator = NUM_OPER;
                        expressionNode->type = SCALAR_TYPE;
                        expressionNode->number.val = stoi(nextRHS[0].term.lexeme);
                        // expressionNode->number. = nextRHS[0].term.line_no;
                        n->expr = expressionNode;
                        main_stack.push_back(*n);
                        returnNode = expressionNode;
                    } else if (finalExpression == "ID") {
                        stackNode *n = new stackNode;
                        n->type = EXPR;
                        exprNode *expressionNode = new exprNode;
                        bool isScalar = false;
                        if(nextRHS[0].term.lexeme.size()>1){
                            syntax_error();
                        }
                        for (size_t i =0; i < scalars.size(); i++) {
                            if (nextRHS[0].term.lexeme ==  scalars[i].lexeme) {
                                isScalar = true;
                            }
                        }
                        if (isScalar) {
                            expressionNode -> type = SCALAR_TYPE;
                        } else {
                            expressionNode->type = ARRAYDDECL_TYPE;
                        }
                        
                        expressionNode->exprOperator = ID_OPER;
                        expressionNode->id.varName =  nextRHS[0].term.lexeme;
                        expressionNode->id.line_no =  nextRHS[0].term.line_no;
                        
                        n->expr = expressionNode;
                        returnNode = expressionNode;
                        main_stack.push_back(*n);
                    } else if (finalExpression == "EXPR PLUS EXPR") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new  exprNode;
                        n->exprOperator = PLUS_OPER;
                        n->operations.left = nextRHS[0].expr;
                        n->operations.right = nextRHS[2].expr;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                    } else if (finalExpression == "EXPR MINUS EXPR") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new exprNode;
                        n->exprOperator = MINUS_OPER;
                        n->operations.left = nextRHS[0].expr;
                        n->operations.right = nextRHS[2].expr;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode  = n;
                    } else if (finalExpression == "EXPR DIV EXPR") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new  exprNode;
                        n->exprOperator = DIV_OPER;
                        n->operations.left = nextRHS[0].expr;
                        n->operations.right = nextRHS[2].expr;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                    } else if (finalExpression == "EXPR MULT EXPR") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new exprNode;
                        n->exprOperator = MULT_OPER;
                        n->operations.left = nextRHS[0].expr;
                        n->operations.right = nextRHS[2].expr;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                    } else if (finalExpression == "LPAREN EXPR RPAREN") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        s->expr = nextRHS[1].expr;
                        main_stack.push_back(*s);
                        returnNode = s->expr;
                    } else if (finalExpression == "EXPR LBRAC DOT RBRAC") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new exprNode;
                        n->exprOperator = WHOLE_ARRAY;
                        n->type = ARRAY_TYPE;
                        n->operations.right = nextRHS[0].expr;
                        n->array.arrayexpr = nextRHS[0].expr;
                        n->array.line_no = nextRHS[0].term.line_no;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                    } else if (finalExpression == "EXPR LBRAC EXPR RBRAC") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new exprNode;
                        exprNode *leftSide = nextRHS[0].expr;
                        exprNode *rightSide = nextRHS[2].expr;
                        // rightSide->exprOperator = ARRAY_ELEM;
                        // rightSide->type = ARRAYDDECL_TYPE;
                        rightSide->array.arrayexpr =  nextRHS[2].expr;
                        n->exprOperator = ARRAY_ELEM;
                        n->operations.left = leftSide;
                        n->operations.right = rightSide;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                    }
                    else {
                        syntax_error();
                }
            } 
        } 
        if (match == false) {
            syntax_error();
        }
        nextRHS.clear();
        } else if (PT[a][c] == "acc") {
            return returnNode;
            
        } else {
            syntax_error();
        }
    }
    for (auto& node : nextRHS) {
        if (node.type == EXPR && node.expr != nullptr) {
            //delete node.expr;
            node.expr = nullptr; // Set the pointer to nullptr after deletion
        }
    }
    for (auto& node : main_stack) {
        if (node.type == EXPR && node.expr != nullptr) {
            //delete node.expr;
            node.expr = nullptr; // Set the pointer to nullptr after deletion
        }
    }
    for (auto& node : nextRHS) {
        if (node.type == EXPR && node.expr != nullptr) {
            //delete node.expr;
        }
        
    }
    return returnNode;
}

// Function to start parsing
void runner() {
    parse_decl_section(); // Parse declaration section
    parse_block();        // Parse block
    expect(END_OF_FILE);  // Expect end of file token
}

// Function to parse declaration section
void parse_decl_section() {
    parse_scalar_decl_section(); // Parse scalar declaration section
    parse_array_decl_section();  // Parse array declaration section
}

// Function to parse scalar declaration section
void  parse_scalar_decl_section() {
    expect(SCALAR); // Expect scalar token
    parse_id_list(); // Parse list of identifiers
}

// Function to parse array declaration section
void parse_array_decl_section() {
    expect(ARRAY);  // Expect array token
    parse_id_list(); // Parse list of identifiers
}

// Function to parse list of identifiers
void parse_id_list() {
    Token tok = lexer.peek(1); // Peek at the next token

    // If the token is an identifier and its line number is 1, add it to the scalars vector
    if (tok.line_no == 1 && tok.token_type == ID) {
        scalars.push_back(tok);
    } 
    // If the token is an identifier and its line number is 2, add it to the arrays vector
    else if (tok.line_no == 2 && tok.token_type == ID) {
        arrays.push_back(tok);
    }

    expect(ID); // Expect identifier token

    Token t = lexer.peek(1); // Peek at the next token

    // If the token is an identifier, parse the next identifier list
    if (t.token_type == ID) {
        parse_id_list();
    }
}

// Function to parse block
void parse_block() {
    expect(LBRACE); // Expect left brace token
    parse_stmt_list(); // Parse statement list
    expect(RBRACE); // Expect right brace token
}

void parse_stmt_list() {
    // Parse a statement
    parse_stmt();

    // Peek at the next token
    Token t = lexer.peek(1);

    // If the next token is OUTPUT or ID, parse the next statement list
    // If the next token is END_OF_FILE, stop parsing
    if (t.token_type == OUTPUT || t.token_type == ID) {
        parse_stmt_list();
    } else if (t.token_type == END_OF_FILE) {
        return;
    }
}

void parse_stmt() {
    Token t = lexer.peek(1);
    // cout<<t.line_no<<endl;
    if (t.token_type == ID ){
        parse_assign_stmt();
    }else if(t.token_type == OUTPUT){
        parse_output_stmt();
    }else{
        syntax_error();
    }
}

void parse_assign_stmt() {
   
    if (lexer.peek(1).token_type == EQUAL) {
        syntax_error();
    }
    exprNode *variableAccesParsed = parse_variable_access();
    
    expect(EQUAL);
    if (lexer.peek(1).token_type == SEMICOLON) {
        syntax_error();
    }
    exprNode * equalnode = new exprNode;
    equalnode->exprOperator = EQUAL_OPER;
    equalnode->operations.left = variableAccesParsed;
    exprNode *expressionParsed = new exprNode; 
    expressionParsed = parse_expr();
    equalnode->operations.right = expressionParsed;
    string parsedString = "";
    parsedString = stringFromExpression(equalnode);
    abstractSyntaxTree += parsedString + ";";
    
    expect(SEMICOLON);
    // Free the dynamically allocated objects
    delete variableAccesParsed;
    variableAccesParsed = nullptr;
    delete equalnode;
    equalnode = nullptr;
}

void parse_output_stmt() {
    ouputStatement = true;
    expect(OUTPUT);
    exprNode *variableAccesParsed = parse_variable_access();
    string access = "";
    access = stringFromExpression(variableAccesParsed);
    abstractSyntaxTree += access;
    expect(SEMICOLON);
    // Free the dynamically allocated object
    delete variableAccesParsed;
    variableAccesParsed = nullptr;
}

exprNode* parse_variable_access() {
    vector<stackNode> main_stack;
    accesOn = true;
    // cout<<"parsing expression"<<endl;
    Token newToken = Token();
    exprNode *returnNode = new exprNode;
    vector<stackNode> candidateRHS;
    newToken.token_type = END_OF_FILE;
    stackNode *s = new stackNode;
    s->type = TERM;
    s->term = newToken;
    main_stack.push_back(*s);
    while (!(lexer.peek(1).token_type == END_OF_FILE && main_stack[main_stack.size() - 1].term.token_type ==  END_OF_FILE)) {
        // cout<<"not end of expression"<<endl;
        Token check = peek_symbol(1);
            stackNode top = main_stack.back();
            if (top.term.token_type == LBRAC) {
                if (check.token_type != DOT) {
                    exprNode * expression = parse_expr();
                    stackNode *stackExpression = new stackNode;
                    stackExpression -> type = EXPR;
                    stackExpression -> expr = expression;
                    main_stack.push_back(*stackExpression);
                    // if (accesOn == true) {
                    //     cout<<"true"<<endl;
                    // } else {
                    //     cout<<"false"<<endl;
                    // }
                }
            }
        accesOn = true;
        Token t  = peek_symbol(1);
        TokenType c = t.token_type;
        int n =1;
        while (main_stack[main_stack.size() - n].type != TERM) {
            n++;
        }
        TokenType a = main_stack[main_stack.size() - n].term.token_type;
        
        if (PT[a][c] == "<" || PT[a][c] == "=") {
            
            Token token = get_symbol();
            stackNode *newToken = new stackNode;
            newToken->type = TERM;
            newToken->term = token;

            main_stack.push_back(*newToken);
        } else if (PT[a][c] == ">") {
            bool termpopped = false;
            stackNode lastPoppedTerminal;
            while((main_stack.back().type == TERM && termpopped == true && PT[main_stack.back().term.token_type][lastPoppedTerminal.term.token_type] == "<") != true) {
                
                stackNode s = main_stack.back();
                main_stack.pop_back();
                //stackNode *lastPoppedTerminal = new stackNode;
                if (s.type  == TERM) {
                    lastPoppedTerminal = s;
                    termpopped = true;
                }
                candidateRHS.push_back(s);
            }
            reverse(candidateRHS.begin(), candidateRHS.end());
            // cout<<"going to expressions"<<endl;
            
            // cout<<"candidate rhs size: " + candidateRHS.size()<<endl;
            vector<string> expressions = {"ID", "EXPR LBRAC EXPR RBRAC", "EXPR LBRAC DOT RBRAC"};
            string expressionMade;
            for (size_t i = 0; i < candidateRHS.size(); i++ ) {
                if (candidateRHS[i].type == EXPR) {
                    expressionMade += " EXPR";
                } else if (candidateRHS[i].type == TERM) {
                    switch(candidateRHS[i].term.token_type) {
                        case ID:
                            expressionMade += " ID";  break;
                        case LBRAC:
                            expressionMade += " LBRAC";  break;
                        case RBRAC:
                            expressionMade += " RBRAC";  break;
                        case DOT:
                            expressionMade += " DOT";   break;
                    }
                }
            }
            bool match = false;
            // cout<<expressionMade<<endl;
            string finalExpression = expressionMade.substr(1);
            // cout<<finalExpression<<endl;
            for (size_t i =0; i < expressions.size(); i++) {
                if (finalExpression == expressions[i]) {
                    match = true;
                    if (finalExpression == "ID") {
                        // cout<<"ID"<<endl;
                        stackNode n;
                        n.type = EXPR;
                        exprNode *expressionNode = new exprNode;
                        bool partofdeclared = false;
                        if(candidateRHS[0].term.lexeme.size()>1){
                            syntax_error();
                        }
                        bool isScalar = false;
                        for (size_t i =0; i < scalars.size(); i++) {
                            if (candidateRHS[0].term.lexeme ==  scalars[i].lexeme) {
                                isScalar = true;
                                partofdeclared = true;
                            }
                        }
                        for (size_t i =0; i < arrays.size(); i++) {
                            if (candidateRHS[0].term.lexeme ==  arrays[i].lexeme) {
                                partofdeclared = true;
                            }
                        }
                        if (isScalar) {
                            expressionNode -> type = SCALAR_TYPE;
                        } else {
                            expressionNode->type = ARRAYDDECL_TYPE;
                        }
                        expressionNode->exprOperator = ID_OPER;
                        expressionNode->id.varName =  candidateRHS[0].term.lexeme;
                        expressionNode->id.line_no =  candidateRHS[0].term.line_no;
                        n.expr = expressionNode;
                       
                        returnNode = expressionNode;  
                        
                        main_stack.push_back(n);
                        // if (partofdeclared == false) {
                        //     syntax_error();
                        // }
                    } else if (finalExpression == "EXPR LBRAC EXPR RBRAC") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new exprNode;
                        exprNode *leftSide = candidateRHS[0].expr;
                        if (leftSide->exprOperator != ID_OPER) {
                            syntax_error();
                        }
                        exprNode *rightSide = candidateRHS[2].expr;;
                       
                        rightSide->array.arrayexpr =  candidateRHS[2].expr;
                        n->exprOperator = ARRAY_ELEM;
                        n->operations.left = leftSide;
                        n->operations.right = rightSide;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                        Token nextTok = lexer.peek(1);
                        if (nextTok.token_type != EQUAL) {
                            syntax_error();
                        }
                    } else if (finalExpression == "EXPR LBRAC DOT RBRAC") {
                        stackNode *s = new stackNode;
                        s->type = EXPR;
                        exprNode *n = new exprNode;
                        n->exprOperator = WHOLE_ARRAY;
                        n->type = ARRAY_TYPE;
                        exprNode *leftSide = candidateRHS[0].expr;
                        if (leftSide->exprOperator != ID_OPER) {
                            syntax_error();
                        }
                        n->operations.right = leftSide;
                        n->operations.left = NULL;
                        n->array.arrayexpr = candidateRHS[0].expr;
                        n->array.line_no = candidateRHS[0].term.line_no;
                        s->expr = n;
                        main_stack.push_back(*s);
                        returnNode = n;
                        Token nextTok = lexer.peek(1);
                        if (nextTok.token_type != EQUAL) {
                            syntax_error();
                        }
                    }  
                }
            }
            if (match == false) {
                syntax_error();
            }
            candidateRHS.clear();
   
        }else if (PT[a][c] == "acc") {
        // cout<<"accepted"<<endl;
        accesOn = false;
    return returnNode;
    } else {
        syntax_error();
    }
    }
        // Free the dynamically allocated objects
    for (auto& node : candidateRHS) {
        if (node.type == EXPR && node.expr != nullptr) {
            delete node.expr;
        }
    }    // Free the dynamically allocated objects
    for (auto& node : main_stack) {
        if (node.type == EXPR && node.expr != nullptr) {
            delete node.expr;
        }
        delete &node;
    }
    return returnNode;
    
}

void handle_greater_operator_precedence(vector<stackNode>& main_stack, vector<stackNode>& nextRHS, exprNode*& returnNode) {
    // Your code for handling ">" operator precedence
}

// Function to free memory allocated to expression nodes in a vector of stack nodes
void free_allocated_objects(vector<stackNode>& nodes) {
    // Iterate over each node in the vector
    for (auto& node : nodes) {
        // If the node type is EXPR and the expr pointer is not null
        if (node.type == EXPR && node.expr != nullptr) {
            // Delete the expression node
            delete node.expr;
            // Set the expr pointer to nullptr to avoid dangling pointer
            node.expr = nullptr;
        }
    }
}

// Function to convert an expression tree into a string
string stringFromExpression(exprNode *expressionNode) {
    // If the expression node is null, return an empty string
    if (expressionNode == nullptr) {
        return "";
    }

    // Initialize an empty string to hold the result
    string result = "";
    // Calculate the height of the expression tree
    int h = heightOfExpression(expressionNode);

    // Iterate over each level of the tree
    for (int i =1; i <= h; i++ ) {
        // Add the string representation of the current level to the result
        result += printCurrentLevel(expressionNode, i);
    }
    // Return the result string
    return result;
}

string printCurrentLevel(exprNode* expressionNode, int level) {
    // Initialize an empty string to hold the result
    string  result = "";

    // If the node is null, return an empty string
    if (expressionNode == NULL) {
        return "";
    }

    // If we're at the desired level, process the current node
    if (level == 1) {
        // Switch on the operator type of the current node
        switch (expressionNode->exprOperator)
        {
        case EQUAL_OPER:
            // If it's an EQUAL operator, add "=" to the result
            result += "=";
            break;
        case ID_OPER:
            // If it's an ID operator, add " ID " and the variable name to the result
            result += " ID ";
            result +='"' + expressionNode->id.varName +  '"';
            break;
        case NUM_OPER:
            // If it's a NUM operator, add " NUM " and the number value to the result
            result += " NUM ";
            result += '"' + to_string(expressionNode->number.val) + '"';
            break;
        case MULT_OPER:
            // If it's a MULT operator, add " *" to the result
            result += " *";
            break;
        case DIV_OPER:
            // If it's a DIV operator, add " /" to the result
            result += " /";
            break;
        case PLUS_OPER:
            // If it's a PLUS operator, add " +" to the result
            result += " +";
            break;
        case  MINUS_OPER:
            // If it's a MINUS operator, add " -" to the result
            result += " -";
            break;
        case WHOLE_ARRAY:
            // If it's a WHOLE_ARRAY operator, add " [.]" to the result
            result += " [.]";
            break;
        case ARRAY_ELEM:
            // If it's an ARRAY_ELEM operator, add " []" to the result
            result += " []";
            break;
        default:
            // If it's an unknown operator, do nothing
            break;
        }
    } else if (level > 1) {
        // If we're not at the desired level yet, recursively process the left and right children at the next level
        result += printCurrentLevel(expressionNode->operations.left, level -1);
        result += printCurrentLevel(expressionNode->operations.right, level -1);
    }
    
    // Return the result string
    return result;
}
int heightOfExpression(exprNode* expressionNode) {
    // Initialize heights for left and right subtrees
    int lheight = 0;
    int rheight = 0;

    // If the node is null, its height is 0
    if (expressionNode == nullptr) {
        return 0;
    } else {
        // If the node represents an operation, calculate the heights of the left and right subtrees
        if (expressionNode->exprOperator == ARRAY_ELEM || expressionNode->exprOperator == MULT_OPER || expressionNode->exprOperator == PLUS_OPER || expressionNode->exprOperator == MINUS_OPER || expressionNode->exprOperator == DIV_OPER || expressionNode->exprOperator == WHOLE_ARRAY || expressionNode->exprOperator == EQUAL_OPER) {
            // If the left subtree exists, calculate its height
            if (expressionNode->operations.left != nullptr) {
                lheight = heightOfExpression(expressionNode->operations.left);
            }
            // If the right subtree exists, calculate its height
            if (expressionNode->operations.right != nullptr) {
                rheight = heightOfExpression(expressionNode->operations.right);
            }
        }

        // Return the height of the taller subtree, plus 1 for the current node
        if (lheight > rheight) {
            return (lheight + 1);
        } else {
            return (rheight + 1);
        }
    }
}

string formStringwithQueue(exprNode* root) {
    // Initialize an empty string to hold the result
    string result;

    // If the root node is null, return an empty string
    if (root == nullptr) {
        return "";
    }

    // Initialize a queue and add the root node to it
    std::queue<exprNode*> q;
    q.push(root);

    // Process nodes in the queue until it's empty
    while (!q.empty()) {
        // Get the node at the front of the queue
        exprNode* current = q.front();
        q.pop();

        // Process the current node based on its operator type
        switch (current->exprOperator)
        {
        case ID_OPER:
            // If it's an ID operator, add " ID " and the variable name to the result
            result += " ID ";
            result +='"' + current->id.varName +  '"';
            break;
        case NUM_OPER:
            // If it's a NUM operator, add " NUM " and the number value to the result
            result += " NUM ";
            result += '"' + to_string(current->number.val) + '"';
            break;
        case MULT_OPER:
            // If it's a MULT operator, add " *" to the result
            result += " *";
            break;
        case DIV_OPER:
            // If it's a DIV operator, add " /" to the result
            result += " /";
            break;
        case PLUS_OPER:
            // If it's a PLUS operator, add " +" to the result
            result += " +";
            break;
        case  MINUS_OPER:
            // If it's a MINUS operator, add " -" to the result
            result += " -";
            break;
        case WHOLE_ARRAY:
            // If it's a WHOLE_ARRAY operator, add " [.]" to the result
            result += " [.]";
            break;
        case ARRAY_ELEM:
            // If it's an ARRAY_ELEM operator, add " []" to the result
            result += " []";
            break;
        default:
            // If it's an unknown operator, do nothing
            break;
        }
        
        // If the current node has a left child, add it to the queue
        if (current->operations.left != nullptr) {
            q.push(current->operations.left);
        }
        // If the current node has a right child, add it to the queue
        if (current->operations.right != nullptr) {
            q.push(current->operations.right);
        }
    }

    // Return the result string
    return result;
}



// Task 1
void parse_and_generate_AST()
{
    // Call the runner function. This function is expected to populate the 'abstractSyntaxTree' string.
    runner(); 

    // Initialize an empty string to hold the first line of the abstract syntax tree
    string firstline ="";

    // Find the position of the first semicolon in the abstract syntax tree. This is assumed to be the end of the first line.
    int firstending = abstractSyntaxTree.find_first_of(';');

    // Extract the first line from the abstract syntax tree by taking a substring from the start to the first semicolon
    firstline = abstractSyntaxTree.substr(0,firstending);

    // Print the first line to the console
    cout<<firstline;
}

// Task 2
void parse_and_type_check()
{
	cout << "Amazing! No type errors here :)" << endl;
}

// Task 3
instNode* parse_and_generate_statement_list()
{
    cout << "3" << endl;

    // The following is the hardcoded statement list 
    // generated for a specific program
    // you should replace this code with code that parses the
    // input and generayes a statement list
    // 
    // program
    // SCALAR a b c d
    // ARRAY x y z
    // a = 1;
    // b = 2;
    // c = 3;
    // d = (a+b)*(b+c);
    // OUTPUT d;
    // x[a+b] = d;
    // OUTPUT x[3];
    //
    //  a will be at location 0
    //  b will be at location 1
    //  c will be at location 2
    //  d will be at location 3
    //  x will be at location 4 - 13
    //  y will be at location 14 - 23
    //  z will be at location 24 - 33
    //  t1 will be at location 34 : intermediate value for (a+b)
    //  t2 will be at location 35 : intermediate value for (b+c)
    //  t3 will be at location 36 : intermediate value (a+b)*(b+c)
    //  t4 will be at location 37 : intermediate value for a+b index of array
    //  t5 will be at location 38 : intermediate value for addr of x[a+b] =
    //                              address_of_x + value of a+b =
    //                              4 + value of a+b
    //  t6 will be at location 39 : intermediate value for addr of x[3] =
    //                              address_of_x + value of 3 =
    //                              4 + value of 3 (computation is not done at
    //                              compile time)
    //
    instNode * i01 = new instNode();
    i01->lhsat=DIRECT; i01->lhs = 0;    // a
    i01->iType=ASSIGN_INST;             // =
    i01->op1at=IMMEDIATE; i01->op1 = 1; // 1
    i01->oper = OP_NOOP;                // no operator

    instNode * i02 = new instNode();
    i02->lhsat=DIRECT; i02->lhs = 1;    // b
    i02->iType=ASSIGN_INST;             // =
    i02->op1at=IMMEDIATE; i02->op1 = 2; // 2
    i02->oper = OP_NOOP;                // no operator

    i01->next = i02;

    instNode * i03 = new instNode();
    i03->lhsat=DIRECT; i03->lhs = 2;    // c
    i03->iType=ASSIGN_INST;             // =
    i03->op1at=IMMEDIATE; i03->op1 = 3; // 3
    i03->oper = OP_NOOP;                // no operator

    i02->next = i03;


    instNode * i1 = new instNode();
    i1->lhsat=DIRECT; i1->lhs = 34; // t1
    i1->iType=ASSIGN_INST;          // =
    i1->op1at=DIRECT; i1->op1 = 0;  // a
    i1->oper = OP_PLUS;             // +
    i1->op2at=DIRECT; i1->op2 = 1;  // b

    i03->next = i1;

    instNode * i2 = new instNode();
    i2->lhsat=DIRECT; i2->lhs = 35; // t2
    i2->iType=ASSIGN_INST;          // =
    i2->op1at=DIRECT; i2->op1 = 1;  // b
    i2->oper = OP_PLUS;             // +
    i2->op2at=DIRECT; i2->op2 = 2;  // c

    i1->next = i2;

    instNode * i3 = new instNode();
    i3->lhsat=DIRECT; i3->lhs = 36; // t3
    i3->iType=ASSIGN_INST;          // =
    i3->op1at=DIRECT; i3->op1 = 34;  // t1
    i3->oper = OP_MULT;             // *
    i3->op2at=DIRECT; i3->op2 = 35;  // t2


    i2->next = i3;                  // i3 should be after i1 and i2

    instNode * i4 = new instNode();
    i4->lhsat=DIRECT; i4->lhs = 3;  // d
    i4->iType=ASSIGN_INST;          // =
    i4->op1at=DIRECT; i4->op1 = 36; // t3
    i4->oper = OP_NOOP;             // no operator


    i3->next = i4;

    instNode * i5 = new instNode();
    i5->iType=OUTPUT_INST;          // OUTPUT
    i5->op1at=DIRECT; i5->op1 = 3;  // d

    i4->next = i5;

    instNode * i6 = new instNode();
    i6->lhsat=DIRECT; i6->lhs = 37; // t4
    i6->iType=ASSIGN_INST;          // =
    i6->op1at=DIRECT; i6->op1 = 0;  // a
    i6->oper = OP_PLUS;             // +
    i6->op2at=DIRECT; i6->op2 = 1;  // b
    i5->next = i6;

    instNode * i7 = new instNode();
    i7->lhsat=DIRECT; i7->lhs = 38;    // t5
    i7->iType=ASSIGN_INST;             // =
    i7->op1at=IMMEDIATE; i7->op1 = 4;  // address of x = 4 available
                                       // at compile time
    i7->oper = OP_PLUS;                // +
    i7->op2at=DIRECT; i7->op2 = 37;    // t5 (contains value of a+b

    i6->next = i7;

    instNode * i8 = new instNode();
    i8->lhsat=INDIRECT; i8->lhs = 38;  // x[a+b]
    i8->iType=ASSIGN_INST;             // =
    i8->op1at=DIRECT; i8->op1 = 3;     // d
    i8->oper = OP_NOOP;

    i7->next = i8;

    instNode * i9 = new instNode();
    i9->lhsat=DIRECT; i9->lhs = 39;    // t6 will contain address of x[3]
    i9->iType=ASSIGN_INST;             // =
    i9->op1at=IMMEDIATE; i9->op1 = 4;  // address of x = 4 available
                                       // at compile time
    i9->oper = OP_PLUS;                // +
    i9->op2at=IMMEDIATE; i9->op2 = 3;  // 3

    i8->next = i9;

    instNode * i10 = new instNode();
    i10->iType=OUTPUT_INST;              // OUTPUT
    i10->op1at=INDIRECT; i10->op1 = 39;  // x[3] by providing its
                                         // address indirectly through
                                         // t6

    i9->next = i10;


    instNode* code = i01;

    return code;
}