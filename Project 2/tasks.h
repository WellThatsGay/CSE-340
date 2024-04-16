/* Christopher Harris was here*/

#include "execute.h"
#include "lexer.h"

using namespace std;

// Enumerations
enum oper {ID_OPER, PLUS_OPER, MINUS_OPER, DIV_OPER, MULT_OPER, ARRAY_ELEM, WHOLE_ARRAY, NUM_OPER, EQUAL_OPER};
enum exprtype {SCALAR_TYPE, ARRAY_TYPE, ARRAYDDECL_TYPE, ERROR_TYPE};
enum snodeType {EXPR, TERM};

// Structs
struct stackNode {
    snodeType type;
    struct exprNode *expr; 
    Token term; 
};

struct exprNode {
    oper exprOperator;
    exprtype type;

    struct { 
        string varName;
        int line_no; 
    } id;

    struct { 
        exprNode *left; 
        exprNode *right;
    } operations;

    struct {
        exprNode *arrayexpr;
        int line_no;
    } array;

    struct {
        int val;
    } number;
};

// Function declarations
Token peek_symbol(int n);
Token get_symbol();
Token expect(TokenType expected_type);
exprNode* parse_expr();
exprNode* parse_variable_access();
string printCurrentLevel(exprNode* expressionNode, int level);
string formStringwithQueue(exprNode* root);
instNode* parse_and_generate_statement_list();
string stringFromExpression(exprNode *expressionNode);

void parse_primary();
void syntax_error();
void parse_output_stmt();
void parse_assign_stmt();
void parse_stmt();
void parse_stmt_list();
void parse_block();
void parse_id_list();
void parse_scalar_decl_section();
void parse_decl_section();
void runner();
void parse_array_decl_section();
void parse_and_generate_AST();
void parse_and_type_check();
int heightOfExpression(exprNode* expression);
